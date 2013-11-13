package com.example.PebbleKitExample.TodoList;

import android.content.Context;
import android.util.Log;
import android.widget.ArrayAdapter;
import com.example.PebbleKitExample.R;

import java.util.LinkedList;
import java.util.List;

public class TodoListAdapter extends ArrayAdapter<TodoListItem> {
    private final Context mContext;
    private static final int MAX_TODO_LIST_LENGTH = 10;
    private static final int MAX_DESCRIPTION_LENGTH = 15;

    private TodoListAdapter(final Context context, final int resource, final int textViewResourceId,
                            final List<TodoListItem> items) {
        super(context, resource, textViewResourceId, items);
        mContext = context;
    }

    public static TodoListAdapter createTodoListAdapter(final Context context) {
        final List<TodoListItem> items = new LinkedList<TodoListItem>();
        // FIXME can getContext() be used here?
        return new TodoListAdapter(context, R.layout.list_item_checkable, R.id.text, items);
    }

    public TodoListItem addItem(String description) {
        if (getCount() >= MAX_TODO_LIST_LENGTH) {
            return null;
        }

        if (description == null || description.length() == 0) {
            return null;
        }

        // FIXME Can you make this UTF-8 safe? :)
        if (description.length() >= MAX_DESCRIPTION_LENGTH) {
            description = description.substring(0, MAX_DESCRIPTION_LENGTH);
        }

        final TodoListItem item = TodoListItem.getItemWithDescription(description);
        this.add(item);

        return item;
    }

    public LinkedList<TodoListItem> addItems(String[] descriptions) {
        LinkedList<TodoListItem> itemsAdded = new LinkedList<TodoListItem>();

        if (descriptions == null) {
            Log.w("TodoList", "Null descriptions...");
            return null;
        }

        for (String description : descriptions) {
            if (description == null) {
                continue;
            }

            final TodoListItem item = addItem(description);
            if (item != null) {
                itemsAdded.add(item);
            }
        }

        return itemsAdded;
    }

    public TodoListItem removeItemAtIndex(final int index) {
        if (index >= getCount()) {
            return null;
        }
        final TodoListItem item = getItem(index);
        remove(item);
        return item;
    }

    public void moveItemFromIndexToIndex(final int sourceIndex, final int destIndex) {
        if (sourceIndex >= getCount() || destIndex >= getCount()) {
            return;
        }

        if (sourceIndex == destIndex) {
            return;
        }

        TodoListItem toAdd = getItem(sourceIndex);
        remove(toAdd);

        if (sourceIndex < destIndex) {
            insert(toAdd, destIndex + 1);
            return;
        }

        insert(toAdd, destIndex);
    }

    public void toggleItemCompletenessAtIndex(final int index) {
        if (index >= getCount()) {
            return;
        }

        TodoListItem item = getItem(index);
        item.setComplete(!item.isComplete());
    }

    public List<String> getDescriptions() {
        LinkedList<String> descriptions = new LinkedList<String>();
        for (int i = 0; i < getCount(); i++) {
            descriptions.add(getItem(i).getDescription());
        }
        return descriptions;
    }
}
