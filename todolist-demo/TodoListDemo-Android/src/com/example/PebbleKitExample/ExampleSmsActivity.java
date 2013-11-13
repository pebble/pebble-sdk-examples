package com.example.PebbleKitExample;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.location.*;
import android.telephony.PhoneNumberUtils;
import android.text.format.DateUtils;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.ContactsContract;
import android.provider.ContactsContract.Contacts;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.telephony.SmsManager;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.Random;
import java.util.UUID;
import java.util.List;

/**
 * Sample activity that sends an SMS containing the user's location when the user presses a button on the watch *
 */
public class ExampleSmsActivity extends Activity {

    private final String TAG = "ExampleSmsActivity";
    private final String PREF_SMS_ACTIVITY = "pref_sms";
    private final String PREF_NUMBER = "phone_number";

    private final Random rand = new Random();
    private final static UUID PEBBLE_APP_UUID = UUID.fromString("EC7EE5C6-8DDF-4089-AA84-C3396A11CC95");
    private final static int CMD_KEY = 0x00;
    private final static int CMD_UP = 0x01;

    private final static int REQUEST_CONTACT = 0x01;

    private PebbleKit.PebbleDataReceiver dataReceiver;

    private LocationManager mLocationManager;
    private String mProviderName;
    private Handler mHandler;

    private boolean mLocationPending;


    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms);

        // allocate instance of the LocationManager
        mLocationManager =
                (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);

        Criteria criteria = new Criteria();
        criteria.setAccuracy(Criteria.ACCURACY_FINE);
        criteria.setCostAllowed(false);

        mProviderName = mLocationManager.getBestProvider(criteria, true);

        mHandler = new Handler();

        loadLastNumber();
    }

    @Override
    protected void onPause() {
        super.onPause();
        // Always deregister any Activity-scoped BroadcastReceivers when the Activity is paused
        if (dataReceiver != null) {
            unregisterReceiver(dataReceiver);
            dataReceiver = null;
        }

        mLocationManager.removeUpdates(mLocationListener);
        saveLastNumber();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // In order to interact with the UI thread from a broadcast receiver, we need to perform any updates through
        // an Android handler. For more information, see: http://developer.android.com/reference/android/os/Handler.html

        // To receive data back from the app, android
        // applications must register a "DataReceiver" to operate on the
        // dictionaries received from the watch.
        //
        // In this example, we're registering a receiver to listen for
        // button presses sent from the watch

        dataReceiver = new PebbleKit.PebbleDataReceiver(PEBBLE_APP_UUID) {
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
                final int cmd = data.getUnsignedInteger(CMD_KEY).intValue();

                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        // All data received from the Pebble must be ACK'd, otherwise you'll hit time-outs in the
                        // watch-app which will cause the watch to feel "laggy" during periods of frequent
                        // communication.
                        PebbleKit.sendAckToPebble(context, transactionId);

                        switch (cmd) {
                            // send SMS when the up button is pressed
                            case CMD_UP:
                                requestLocationForSms();
                                break;
                            default:
                                break;
                        }
                    }
                });
            }
        };
        PebbleKit.registerReceivedDataHandler(this, dataReceiver);
        startWatchApp(null);
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK) {
            if(data != null && requestCode == REQUEST_CONTACT) {
                Uri uriOfPhoneNumberRecord = data.getData();
                String idOfPhoneRecord = uriOfPhoneNumberRecord.getLastPathSegment();
                Cursor cursor = getContentResolver().query(Phone.CONTENT_URI, new String[]{Phone.NUMBER}, Phone._ID + "=?", new String[]{idOfPhoneRecord}, null);
                if(cursor != null) {
                    if(cursor.getCount() > 0) {
                        cursor.moveToFirst();
                        String formattedPhoneNumber = cursor.getString( cursor.getColumnIndex(Phone.NUMBER) );
                        if (formattedPhoneNumber != null) {
                            EditText destinationEditText = (EditText)findViewById(R.id.destinationNumber);
                            destinationEditText.setText(PhoneNumberUtils.stripSeparators(formattedPhoneNumber));
                        }
                    }
                    cursor.close();
                }
            }
        }
    }

    public void pickContact(View view) {
        Intent intent = new Intent(Intent.ACTION_PICK, Contacts.CONTENT_URI);
        intent.setType(ContactsContract.CommonDataKinds.Phone.CONTENT_TYPE);
        startActivityForResult(intent, 1);
    }

    protected void loadLastNumber() {
        SharedPreferences settings = getSharedPreferences(PREF_SMS_ACTIVITY, 0);
        String number = settings.getString(PREF_NUMBER, "");

        EditText destinationEditText = (EditText)findViewById(R.id.destinationNumber);
        destinationEditText.setText(number);
    }

    protected void saveLastNumber() {
        EditText destinationEditText = (EditText)findViewById(R.id.destinationNumber);
        String destinationNumber = destinationEditText.getText().toString();
        if (destinationNumber == null || destinationNumber.equals("")) {
            return;
        }

        SharedPreferences settings = getSharedPreferences(PREF_SMS_ACTIVITY, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(PREF_NUMBER, destinationNumber);

        editor.commit();
    }

    // Send a broadcast to launch the specified application on the connected Pebble
    public void startWatchApp(View view) {
        PebbleKit.startAppOnPebble(getApplicationContext(), PEBBLE_APP_UUID);
    }

    // Send a broadcast to close the specified application on the connected Pebble
    public void stopWatchApp(View view) {
        PebbleKit.closeAppOnPebble(getApplicationContext(), PEBBLE_APP_UUID);
    }

    private final LocationListener mLocationListener = new LocationListener() {
        @Override
        public void onLocationChanged(Location location) {
            sendLocationSms(location);
            mLocationManager.removeUpdates(this);
        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle bundle) {
            Log.e(TAG, "onStatusChanged");
        }

        @Override
        public void onProviderEnabled(String provider) {
            Log.e(TAG, "onProviderEnabled");
        }

        @Override
        public void onProviderDisabled(String provider) {
            Log.e(TAG, "onProviderDisabled");
        }
    };

    public void requestLocationForSms() {
        if (mProviderName != null && mLocationPending == false) {
            mLocationPending = true;

            Location lastLocation = mLocationManager.getLastKnownLocation(mProviderName);
             // if we have a location that's newer than 10 minutes, use it; otherwise get a new location
            if (lastLocation != null && (System.currentTimeMillis() - lastLocation.getTime() > DateUtils.MINUTE_IN_MILLIS * 10)) {
                mLocationManager.requestLocationUpdates(mProviderName,
                    10000,
                    10,
                    mLocationListener);
            } else {
                 sendLocationSms(lastLocation);
            }
        }
    }

    public void sendLocationSms(Location l) {
        if (mLocationPending) {
            mLocationPending = false;

            EditText destinationEditText = (EditText)findViewById(R.id.destinationNumber);
            String destinationNumber = destinationEditText.getText().toString();
            if (destinationNumber == null || destinationNumber.equals("")) {
                return;
            }

            // send SMS with GPS coordinates
            SmsManager smsManager = SmsManager.getDefault();
            String locationString = "Get me: " + l.getLatitude() + ", " + l.getLongitude();
            smsManager.sendTextMessage(destinationNumber, null, locationString, null, null);

            // get address text if we can
            Geocoder geocoder = new Geocoder(ExampleSmsActivity.this);

            try {
                List<Address> addresses = geocoder.getFromLocation(l.getLatitude(), l.getLongitude(), 1);

                if (addresses.size() > 0) {
                    Address a = addresses.get(0);
                    String addressText = "";
                    for (int i = 0; i <= a.getMaxAddressLineIndex(); i++) {
                        addressText += a.getAddressLine(i) + " ";
                    }
                    vibrateWatch(getApplicationContext());
                    smsManager.sendTextMessage(destinationNumber, null, addressText, null, null);
                }
            } catch (Exception e) {
                // unable to geocode
            }
        }
    }

    public static void vibrateWatch(Context c) {
        PebbleDictionary data = new PebbleDictionary();
        data.addUint8(CMD_KEY, (byte) CMD_UP);
        PebbleKit.sendDataToPebble(c, PEBBLE_APP_UUID, data);
    }

}
