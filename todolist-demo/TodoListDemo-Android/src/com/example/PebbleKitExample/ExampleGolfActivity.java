package com.example.PebbleKitExample;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.TextView;
import com.getpebble.android.kit.Constants;
import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.Random;

/**
 * Sample code demonstrating how Android applications can send+receive data using the 'Golf' app,
 * one of Pebble's built-in sports watch-apps.
 */
public class ExampleGolfActivity extends Activity {

    private final Random rand = new Random();
    private PebbleKit.PebbleDataReceiver dataReceiver;
    private int appData[][] = new int[18][4];
    private int selection = 0;

    // Generate random (but somewhat believable) values to be displayed on the watch.
    private void generateGolfData() {
        for (int i = 0; i < appData.length; ++i) {
            int backDistance = rand.nextInt(800) + 50;
            appData[i][0] = 1 + rand.nextInt(5); // par
            appData[i][1] = backDistance; // back of the green
            appData[i][2] = backDistance - 15; // middle of the green
            appData[i][3] = backDistance - 30; // front of the green
        }
    }

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_golf);
        generateGolfData();
        selection = 0;
    }

    @Override
    protected void onPause() {
        super.onPause();

        // Always deregister any Activity-scoped BroadcastReceivers when the Activity is paused
        if (dataReceiver != null) {
            unregisterReceiver(dataReceiver);
            dataReceiver = null;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        updateUi();

        // In order to interact with the UI thread from a broadcast receiver, we need to perform any updates through
        // an Android handler. For more information, see: http://developer.android.com/reference/android/os/Handler.html
        final Handler handler = new Handler();

        // To receive data back from the sports watch-app, android
        // applications must register a "DataReceiver" to operate on the
        // dictionaries received from the watch.
        //
        // In this example, we're registering a receiver to listen for
        // button presses sent from the watch, allowing us to page
        // through the holes displayed on the phone and watch.
        dataReceiver = new PebbleKit.PebbleDataReceiver(Constants.GOLF_UUID) {
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
                final int cmd = data.getUnsignedInteger(Constants.GOLF_CMD_KEY).intValue();

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        // All data received from the Pebble must be ACK'd, otherwise you'll hit time-outs in the
                        // watch-app which will cause the watch to feel "laggy" during periods of frequent
                        // communication.
                        PebbleKit.sendAckToPebble(context, transactionId);

                        switch (cmd) {
                            case Constants.GOLF_CMD_NEXT:
                                next();
                                break;
                            case Constants.GOLF_CMD_PREV:
                                previous();
                                break;
                        }
                    }
                });
            }
        };
        PebbleKit.registerReceivedDataHandler(this, dataReceiver);
    }

    // Send a broadcast to launch the specified application on the connected Pebble
    public void startWatchApp(View view) {
        PebbleKit.startAppOnPebble(getApplicationContext(), Constants.GOLF_UUID);
    }

    // Send a broadcast to close the specified application on the connected Pebble
    public void stopWatchApp(View view) {
        PebbleKit.closeAppOnPebble(getApplicationContext(), Constants.GOLF_UUID);
    }

    // A custom icon and name can be applied to the golf-app to
    // provide some support for "branding" your Pebble-enabled sports
    // application on the watch.
    //
    // It is recommended that applications customize the sports
    // application before launching it. Only one application may
    // customize the sports application at a time on a first-come,
    // first-serve basis.
    public void customizeWatchApp(View view) {
        final String customAppName = "My Golf App";
        final Bitmap customIcon = BitmapFactory.decodeResource(getResources(), R.drawable.watch);

        PebbleKit.customizeWatchApp(
                getApplicationContext(), Constants.PebbleAppType.GOLF, customAppName, customIcon);
    }

    // Update the Activity with the data for a given hole
    public void updateUi() {
        int holeInfo[] = appData[selection];

        String holePar = String.format("Hole: %d Par: %d", selection + 1, holeInfo[0]);
        String back = String.format("%d", holeInfo[1]);
        String mid = String.format("%d", holeInfo[2]);
        String front = String.format("%d", holeInfo[3]);

        TextView parText = (TextView) findViewById(R.id.textHole);
        parText.setText(holePar);

        TextView backText = (TextView) findViewById(R.id.textBack);
        backText.setText(back);

        TextView midText = (TextView) findViewById(R.id.textMiddle);
        midText.setText(mid);

        TextView frontText = (TextView) findViewById(R.id.textFront);
        frontText.setText(front);
    }

    // Push {range, hole, par} data to be displayed on Pebble's Golf app.
    // To simplify formatting, values are transmitted to Pebble as null-terminated strings.
    public void updateWatch() {
        int holeInfo[] = appData[selection];

        String hole = String.format("%d", selection + 1);
        String par = String.format("%d", holeInfo[0]);
        String back = String.format("%d", holeInfo[1]);
        String mid = String.format("%d", holeInfo[2]);
        String front = String.format("%d", holeInfo[3]);

        // All data sent handled by PebbleKit is wrapped in a PebbleDictionary. The interface is somewhat similar to
        // Map<Integer, Object>, as PebbleDictionaries may contain heterogeneous types.
        //
        // Here we are building up a Dictionary that maps integer keys to String values that will be interpreted &
        // displayed by the watch.
        PebbleDictionary data = new PebbleDictionary();
        data.addString(Constants.GOLF_HOLE_KEY, hole);
        data.addString(Constants.GOLF_PAR_KEY, par);
        data.addString(Constants.GOLF_BACK_KEY, back);
        data.addString(Constants.GOLF_MID_KEY, mid);
        data.addString(Constants.GOLF_FRONT_KEY, front);

        // Once the dictionary has been populated, it is scheduled to be sent to the watch. The sender/recipient of
        // all PebbleKit messages is determined by the UUID. In this case, since we're sending the data to the golf app,
        // we specify the Golf UUID.
        PebbleKit.sendDataToPebble(getApplicationContext(), Constants.GOLF_UUID, data);
    }

    private void previous() {
        if (selection > 0) {
            selection -= 1;
        }

        updateUi();
        updateWatch();
    }

    public void goPrevious(View view) {
        previous();
    }

    public void next() {
        if (selection < 17) {
            selection += 1;
        }

        updateUi();
        updateWatch();
    }

    public void goNext(View view) {
        next();
    }
}
