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
 * Sample code demonstrating how Android applications can send+receive data using the 'Sports' app, one of Pebble's
 * built-in watch-apps that supports app messaging.
 */
public class ExampleSportsActivity extends Activity {

    private static final String TAG = "ExampleSportsActivity";

    private final Random rand = new Random();
    private PebbleKit.PebbleDataReceiver sportsDataHandler = null;
    private int sportsState = Constants.SPORTS_STATE_INIT;
    private boolean useMetric = false;
    private boolean isPaceLabel = true;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sports);
    }

    @Override
    protected void onPause() {
        super.onPause();

        // Always deregister any Activity-scoped BroadcastReceivers when the Activity is paused
        if (sportsDataHandler != null) {
            unregisterReceiver(sportsDataHandler);
            sportsDataHandler = null;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        final Handler handler = new Handler();

        // To receive data back from the sports watch-app, Android
        // applications must register a "DataReceiver" to operate on the
        // dictionaries received from the watch.
        //
        // In this example, we're registering a receiver to listen for
        // changes in the activity state sent from the watch, allowing
        // us the pause/resume the activity when the user presses a
        // button in the watch-app.
        sportsDataHandler = new PebbleKit.PebbleDataReceiver(Constants.SPORTS_UUID) {
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
                int newState = data.getUnsignedInteger(Constants.SPORTS_STATE_KEY).intValue();
                sportsState = newState;

                PebbleKit.sendAckToPebble(context, transactionId);

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        updateUi();
                    }
                });
            }
        };
        PebbleKit.registerReceivedDataHandler(this, sportsDataHandler);
    }

    public void updateUi() {
        TextView statusText = (TextView) findViewById(R.id.status);
        if (sportsState == Constants.SPORTS_STATE_RUNNING) {
            statusText.setText("Running");
        } else {
            statusText.setText("Paused");
        }
    }

    // Send a broadcast to launch the specified application on the connected Pebble
    public void startWatchApp(View view) {
        PebbleKit.startAppOnPebble(getApplicationContext(), Constants.SPORTS_UUID);
    }

    // Send a broadcast to close the specified application on the connected Pebble
    public void stopWatchApp(View view) {
        PebbleKit.closeAppOnPebble(getApplicationContext(), Constants.SPORTS_UUID);
    }

    // A custom icon and name can be applied to the sports-app to
    // provide some support for "branding" your Pebble-enabled sports
    // application on the watch.
    //
    // It is recommended that applications customize the sports
    // application before launching it. Only one application may
    // customize the sports application at a time on a first-come,
    // first-serve basis.
    public void customizeWatchApp(View view) {
        final String customAppName = "My Sports App";
        final Bitmap customIcon = BitmapFactory.decodeResource(getResources(), R.drawable.watch);

        PebbleKit.customizeWatchApp(
                getApplicationContext(), Constants.PebbleAppType.SPORTS, customAppName, customIcon);
    }

    // Push (distance, time, pace) data to be displayed on Pebble's Sports app.
    //
    // To simplify formatting, values are transmitted to the watch as strings.
    public void updateWatchApp(View view) {
        String time = String.format("%02d:%02d", rand.nextInt(60), rand.nextInt(60));
        String distance = String.format("%02.02f", 32 * rand.nextDouble());
	String addl_data = String.format("%02d:%02d", rand.nextInt(10), rand.nextInt(60));

        PebbleDictionary data = new PebbleDictionary();
        data.addString(Constants.SPORTS_TIME_KEY, time);
        data.addString(Constants.SPORTS_DISTANCE_KEY, distance);
	data.addString(Constants.SPORTS_DATA_KEY, addl_data);
	data.addUint8(Constants.SPORTS_LABEL_KEY, (byte) (isPaceLabel ? Constants.SPORTS_DATA_SPEED : Constants.SPORTS_DATA_PACE));

        PebbleKit.sendDataToPebble(getApplicationContext(), Constants.SPORTS_UUID, data);
	isPaceLabel = !isPaceLabel;
    }

    // The units in the sports app can be toggled between Metric (1) and Imperial (0)
    // by sending the following message to Pebble once the sports app is running.
    public void changeUnitsOnWatch(View view) {
        PebbleDictionary data = new PebbleDictionary();
        data.addUint8(Constants.SPORTS_UNITS_KEY,
                (byte) (useMetric ? Constants.SPORTS_UNITS_METRIC : Constants.SPORTS_UNITS_IMPERIAL));

        PebbleKit.sendDataToPebble(getApplicationContext(), Constants.SPORTS_UUID, data);
        useMetric = !useMetric;
    }
}
