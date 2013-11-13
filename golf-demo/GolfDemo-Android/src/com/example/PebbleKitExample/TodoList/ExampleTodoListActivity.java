package com.example.PebbleKitExample.TodoList;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.os.*;
import android.util.Log;
import android.view.*;
import android.widget.AdapterView;
import android.widget.EditText;
import com.example.PebbleKitExample.R;
import com.getpebble.android.kit.PebbleKit;
 import com.getpebble.android.kit.util.PebbleDictionary;
import com.mobeta.android.dslv.DragSortListView;

import java.util.HashSet;
import java.util.UUID;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Sample activity demonstrating how to synchronize data with a "To-Do List" watch-app.
 */
public class ExampleTodoListActivity extends ListActivity {
    private PebbleKit.PebbleDataReceiver dataReceiver;
    private PebbleKit.PebbleAckReceiver ackReceiver;
    private PebbleKit.PebbleNackReceiver nackReceiver;

    private static final int APPEND_KEY = 0;
    private static final int DELETE_KEY = 1;
    private static final int MOVE_KEY = 2;
    private static final int TOGGLE_KEY = 3;
    private static final int FETCH_KEY = 4;

    private static final UUID TODO_LIST_UUID = UUID.fromString("e0ca665a-cf96-4c09-bbea-aa113e8cc18f");

    private TodoListAdapter todoListAdapter;
    private DialogManager dialogManager = new DialogManager();
    private final MessageManager messageManager = new MessageManager();

    /**
     * Manages a thread-safe message queue using a Looper worker thread to complete blocking tasks.
     */
    public class MessageManager implements Runnable {
        public Handler messageHandler;
        private final BlockingQueue<PebbleDictionary> messageQueue = new LinkedBlockingQueue<PebbleDictionary>();
        private Boolean isMessagePending = Boolean.valueOf(false);

        @Override
        public void run() {
            Looper.prepare();
            messageHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    Log.w(this.getClass().getSimpleName(), "Please post() your blocking runnables to Mr Manager, " +
                            "don't use sendMessage()");
                }

            };
            Looper.loop();
        }

        private void consumeAsync() {
            messageHandler.post(new Runnable() {
                @Override
                public void run() {
                    synchronized (isMessagePending) {
                        if (isMessagePending.booleanValue()) {
                            return;
                        }

                        synchronized (messageQueue) {
                            if (messageQueue.size() == 0) {
                                return;
                            }
                            PebbleKit.sendDataToPebble(getApplicationContext(), TODO_LIST_UUID, messageQueue.peek());
                        }

                        isMessagePending = Boolean.valueOf(true);
                    }
                }
            });
        }

        public void notifyAckReceivedAsync() {
            messageHandler.post(new Runnable() {
                @Override
                public void run() {
                    synchronized (isMessagePending) {
                        isMessagePending = Boolean.valueOf(false);
                    }
                    messageQueue.remove();
                }
            });
            consumeAsync();
        }

        public void notifyNackReceivedAsync() {
            messageHandler.post(new Runnable() {
                @Override
                public void run() {
                    synchronized (isMessagePending) {
                        isMessagePending = Boolean.valueOf(false);
                    }
                }
            });
            consumeAsync();
        }

        public boolean offer(final PebbleDictionary data) {
            final boolean success = messageQueue.offer(data);

            if (success) {
                consumeAsync();
            }

            return success;
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_todo_list);

        todoListAdapter = TodoListAdapter.createTodoListAdapter(getApplicationContext());

        // Populate list before starting app on the Pebble
        String[] dummyItems = getResources().getStringArray(R.array.jazz_artist_names);
        todoListAdapter.addItems(dummyItems);

        setListAdapter(todoListAdapter);
        final DragSortListView dslv = getListViewAsDSLV();

        dslv.setDropListener(new DragSortListView.DropListener() {
            @Override
            public void drop(final int from, final int to) {
                if (from == to) {
                    return;
                }
                todoListAdapter.moveItemFromIndexToIndex(from, to);
                dslv.moveCheckState(from, to);

                PebbleDictionary data = new PebbleDictionary();
                data.addBytes(MOVE_KEY, new byte[]{(byte) from, (byte) to});
                messageManager.offer(data);
            }
        });

        dslv.setRemoveListener(new DragSortListView.RemoveListener() {
            @Override
            public void remove(final int which) {
                final TodoListItem item = todoListAdapter.removeItemAtIndex(which);
                if (item == null) {
                    return;
                }

                dslv.removeCheckState(which);

                // Notify the watch
                PebbleDictionary data = new PebbleDictionary();
                data.addUint8(DELETE_KEY, (byte) which);
                messageManager.offer(data);
            }
        });

        dslv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(final AdapterView<?> parentAdapterView, final View view, final int position,
                                    final long rowId) {
                todoListAdapter.toggleItemCompletenessAtIndex(position);
                PebbleDictionary data = new PebbleDictionary();
                data.addUint8(TOGGLE_KEY, (byte) position);
                messageManager.offer(data);
            }
        });

        PebbleKit.startAppOnPebble(getApplicationContext(), TODO_LIST_UUID);
    }

    @Override
    public void onStart() {
        // FIXME do I need to do any cleanup in onStop()?
        super.onStart();
        new Thread(messageManager).start();

    }

    private DragSortListView getListViewAsDSLV() {
        return (DragSortListView) super.getListView();
    }

    @Override
    public DragSortListView getListView() {
        return getListViewAsDSLV();
    }

    @Override
    protected void onPause() {
        super.onPause();

        // Always deregister any Activity-scoped BroadcastReceivers when the Activity is paused
        if (dataReceiver != null) {
            unregisterReceiver(dataReceiver);
            dataReceiver = null;
        }

        if (ackReceiver != null) {
            unregisterReceiver(ackReceiver);
            ackReceiver = null;
        }

        if (nackReceiver != null) {
            unregisterReceiver(nackReceiver);
            nackReceiver = null;
        }

        dialogManager.dismissAll();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // In order to interact with the UI thread from a broadcast receiver, we need to perform any updates through
        // an Android handler. For more information, see: http://developer.android.com/reference/android/os/Handler
        // .html
        final Handler handler = new Handler();

        // To receive data back from a watch-app, android
        // applications must register a "DataReceiver" to operate on the
        // dictionaries received from the watch.
        dataReceiver = new PebbleKit.PebbleDataReceiver(TODO_LIST_UUID) {
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        // All data received from the Pebble must be ACK'd, otherwise you'll hit time-outs in the
                        // watch-app which will cause the watch to feel "laggy" during periods of frequent
                        // communication.
                        PebbleKit.sendAckToPebble(context, transactionId);

                        if (!data.iterator().hasNext()) {
                            return;
                        }

                        final DragSortListView dslv = getListViewAsDSLV();

                        final Long deleteValue = data.getUnsignedInteger(DELETE_KEY);
                        if (deleteValue != null) {
                            final int value = deleteValue.intValue();
                            final TodoListItem item = todoListAdapter.removeItemAtIndex(value);
                            if (item == null) {
                                return;
                            }

                            dslv.removeCheckState(value);
                        }

                        final Long toggleValue = data.getUnsignedInteger(TOGGLE_KEY);
                        if (toggleValue != null) {
                            final int value = toggleValue.intValue();
                            todoListAdapter.toggleItemCompletenessAtIndex(value);
                            dslv.setItemChecked(value, todoListAdapter.getItem(value).isComplete());
                        }

                        final Long fetchValue = data.getUnsignedInteger(FETCH_KEY);
                        if (fetchValue != null) {
                            // Add entire list to messageQueue
                            for (int i = 0; i < todoListAdapter.getCount(); i++) {
                                PebbleDictionary data = new PebbleDictionary();
                                TodoListItem item = todoListAdapter.getItem(i);
                                data.addString(APPEND_KEY, item.getDescription());

                                if (item.isComplete()) {
                                    data.addUint8(TOGGLE_KEY, (byte) i);
                                }
                                if (!messageManager.offer(data)) {
                                    return;
                                }
                            }
                        }
                    }
                });
            }
        };

        PebbleKit.registerReceivedDataHandler(this, dataReceiver);

        ackReceiver = new PebbleKit.PebbleAckReceiver(TODO_LIST_UUID) {
            @Override
            public void receiveAck(final Context context, final int transactionId) {
                messageManager.notifyAckReceivedAsync();
            }
        };

        PebbleKit.registerReceivedAckHandler(this, ackReceiver);


        nackReceiver = new PebbleKit.PebbleNackReceiver(TODO_LIST_UUID) {
            @Override
            public void receiveNack(final Context context, final int transactionId) {
                messageManager.notifyNackReceivedAsync();
            }
        };

        PebbleKit.registerReceivedNackHandler(this, nackReceiver);
    }

    private static class DialogManager {
        public void showDialog(Dialog d) {
            if (mDialogs.contains(d) == false) {
                mDialogs.add(d);
            }
            d.show();
        }

        public void dismissAll() {
            for (Dialog d : mDialogs) {
                if (d != null) {
                    if (d.isShowing()) {
                        d.dismiss();
                    }
                }
            }
            mDialogs.clear();
        }

        public HashSet<Dialog> mDialogs = new HashSet<Dialog>();
    }

    private void removeAllCompletedItems() {
        for (int i = todoListAdapter.getCount() - 1; i >= 0; i--) {
            final TodoListItem item = todoListAdapter.getItem(i);
            if (!item.isComplete()) {
                continue;
            }

            getListViewAsDSLV().removeCheckState(i);
            todoListAdapter.remove(item);

            PebbleDictionary data = new PebbleDictionary();
            data.addUint8(DELETE_KEY, (byte) i);
            if (!messageManager.offer(data)) {
                return;
            }
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
            case (R.id.add_mi):
                dialogManager.showDialog(createDialog());
                return true;
            case (R.id.remove_all_checked_mi):
                removeAllCompletedItems();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.options_menu, menu);
        return true;
    }

    public Dialog createDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        final LayoutInflater inflater = getLayoutInflater();

        // Inflate and set the layout for the dialog
        // Pass null as the parent view because its going in the dialog layout
        final View rootView = inflater.inflate(R.layout.dialog_add_todo_item, null);
        builder.setView(rootView)
                // Add action buttons
                .setPositiveButton(R.string.todo_list_add_dialog_ok_button, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {
                        final EditText editTextWidget = ((EditText) rootView.findViewById(R.id.todo_list_add_item_dialog_description));
                        final String description = editTextWidget.getText().toString();
                        final TodoListItem item = todoListAdapter.addItem(description);

                        if (item == null) {
                            dialog.dismiss();
                            return;
                        }

                        final PebbleDictionary data = new PebbleDictionary();
                        data.addString(APPEND_KEY, item.getDescription());
                        messageManager.offer(data);
                        dialog.dismiss();
                    }
                })
                .setNegativeButton(R.string.todo_list_add_dialog_cancel_button, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });
        return builder.create();
    }
}
