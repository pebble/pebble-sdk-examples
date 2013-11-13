package com.example.PebbleKitExample;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.nfc.NfcAdapter.*;
import android.nfc.NfcEvent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Parcelable;
import android.text.format.Time;

import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gcm.GCMRegistrar;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.Random;
import java.util.UUID;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.charset.Charset;

import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.BasicHttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.impl.client.DefaultHttpClient;


/**
 * Allows for sending messages between two Android-paired Pebbles via Google Cloud Messaging.
 * Uses NFC to exchange GCM registration IDs.
 *
 * Pressing a button on the Pebble running the companion app (app_vibrate_paired_pebble in watch examples) will
 * cause this to send a message via GCM to a phone paired via NFC.  That phone will use app_messages to
 * vibrate the Pebble it's paired with.
 *
 * @author matt.hornyak@getpebble.com
 *
 */
public class ExampleGCMActivity extends Activity implements CreateNdefMessageCallback,
        OnNdefPushCompleteCallback {

    AsyncTask<Void, Void, Void> mRegisterTask;

    NfcAdapter mNfcAdapter;
    private static final int MESSAGE_SENT = 1;

    private final String TAG = "ExampleGCMActivity";
    private final String PREF_GCM_ACTIVITY = "pref_gcm";
    private final String PREF_PAIRED_ID = "paired_gcm_id";

    static final String SENDER_ID = "501014417325";
    static final String API_KEY = "AIzaSyAg9KiFRSkQtvaOCA6ESesVawkmm7OnVUk";

    private final Random rand = new Random();
    private final static UUID PEBBLE_APP_UUID = UUID.fromString("7BD103D3-0F87-4D31-AEF8-F5237A79C72E");
    private final static int MSG_CMD_OUT_KEY = 0x00;
    private final static int MSG_CMD_IN_KEY = 0x01;

    private final static int MSG_CMD_VIBRATE = 0x00;
    private final static int MSG_CMD_UP = 0x01;

    private PebbleKit.PebbleDataReceiver dataReceiver;

    public static String myGcmId = "";
    private String mPairedGcmId;
    private TextView mInfoText;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gcm);

        mInfoText = (TextView) findViewById(R.id.textGcmStatus);
        loadPairedId();

        if (mPairedGcmId != null && !mPairedGcmId.equals("")) {
            mInfoText.setText("Paired to another phone. Ready to use.");
        }  else {
            mInfoText.setText("Run this Activity on the other phone you want to pair with, then tap them together.");
        }

        // Check for available NFC Adapter
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
        if (mNfcAdapter == null) {
            mInfoText.setText("NFC is not available on this device.");
        } else {
            // Register callback to set NDEF message
            mNfcAdapter.setNdefPushMessageCallback(this, this);
            // Register callback to listen for message-sent success
            mNfcAdapter.setOnNdefPushCompleteCallback(this, this);
        }

        // GCM

        // Make sure the device has the proper dependencies.
        GCMRegistrar.checkDevice(this);
//        GCMRegistrar.checkManifest(this);

        final String regId = GCMRegistrar.getRegistrationId(this);
        if (regId.equals("")) {
            // Automatically registers application on startup.
            GCMRegistrar.register(this, SENDER_ID);
        } else {
            myGcmId = regId;
            // Device is already registered on GCM, check server.
            if (!GCMRegistrar.isRegisteredOnServer(this)) {
                GCMRegistrar.setRegisteredOnServer(this, true);
            }
        }

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
                final int cmd = data.getUnsignedInteger(MSG_CMD_IN_KEY).intValue();

                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        // All data received from the Pebble must be ACK'd, otherwise you'll hit time-outs in the
                        // watch-app which will cause the watch to feel "laggy" during periods of frequent
                        // communication.
                        PebbleKit.sendAckToPebble(context, transactionId);

                        switch (cmd) {
                            // send GCM message when the up button is pressed
                            case MSG_CMD_UP:
                                sendGCMMessageToPairedDevice();
                                break;
                            default:
                                break;
                        }
                    }
                });
            }
        };
        PebbleKit.registerReceivedDataHandler(this, dataReceiver);
        startWatchApp(getApplicationContext());

        // Check to see that the Activity started due to an Android Beam
        if (NfcAdapter.ACTION_NDEF_DISCOVERED.equals(getIntent().getAction())) {
            processIntent(getIntent());
        }
    }

    protected void onDestroy() {
        GCMRegistrar.onDestroy(this);
        super.onDestroy();
    }

    @Override
    public void onNewIntent(Intent intent) {
        // onResume gets called after this to handle the intent
        setIntent(intent);
    }

    /**
     * On an NFC tap, retrieve the GCM ID of the other device & save it
     */
    void processIntent(Intent intent) {
        Parcelable[] rawMsgs = intent.getParcelableArrayExtra(
                NfcAdapter.EXTRA_NDEF_MESSAGES);

        NdefMessage msg = (NdefMessage) rawMsgs[0];
        // record 0 contains the MIME type, record 1 is the AAR, if present
        String otherDeviceId = new String(msg.getRecords()[0].getPayload());
        mInfoText.setText("Paired successfully. Pressing the up button on your Pebble while running the app will now vibrate the Pebble paired to that phone.");

        setPairedId(otherDeviceId);
        sendGCMMessageToPairedDevice();
    }

    public NdefMessage createNdefMessage(NfcEvent event) {
        Time time = new Time();
        time.setToNow();
        String text;
        if (ExampleGCMActivity.myGcmId.equals("")) {
            text = "NO-GCM-ID";
        } else {
            text = ExampleGCMActivity.myGcmId;
        }
        NdefMessage msg = new NdefMessage(
                new NdefRecord[] { createMimeRecord(
                        "application/com.example.android.beam", text.getBytes())
                });
        return msg;
    }

    /**
     * Creates a custom MIME type encapsulated in an NDEF record
     *
     * @param mimeType
     */
    public NdefRecord createMimeRecord(String mimeType, byte[] payload) {
        byte[] mimeBytes = mimeType.getBytes(Charset.forName("US-ASCII"));
        NdefRecord mimeRecord = new NdefRecord(
                NdefRecord.TNF_MIME_MEDIA, mimeBytes, new byte[0], payload);
        return mimeRecord;
    }

    /**
     * Implementation for the OnNdefPushCompleteCallback interface
     */
    @Override
    public void onNdefPushComplete(NfcEvent arg0) {
        // A handler is needed to send messages to the activity when this
        // callback occurs, because it happens from a binder thread
        mHandler.obtainMessage(MESSAGE_SENT).sendToTarget();
    }

    /** This handler receives a message from onNdefPushComplete */
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_SENT:
                    Toast.makeText(getApplicationContext(), "GCM id sent", Toast.LENGTH_LONG).show();
                    break;
            }
        }
    };

    protected void loadPairedId() {
        SharedPreferences settings = getSharedPreferences(PREF_GCM_ACTIVITY, 0);
        mPairedGcmId = settings.getString(PREF_PAIRED_ID, null);
    }

    protected void setPairedId(String id) {
        mPairedGcmId = id;
        SharedPreferences settings = getSharedPreferences(PREF_GCM_ACTIVITY, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(PREF_PAIRED_ID, mPairedGcmId);

        editor.commit();
    }

    private void sendGCMMessageToPairedDevice() {
        HttpClient httpclient = new DefaultHttpClient();

        HttpPost httppost = new HttpPost("https://android.googleapis.com/gcm/send");
        httppost.setHeader("Authorization", "key=" + API_KEY);
        httppost.setHeader("Content-Type", "application/json");

        try {
            String reqBody = String.format("{\"registration_ids\" : [\"%s\"],\"data\" : { \"hi\":\"pebble\"}}", mPairedGcmId);
            BasicHttpEntity e = new BasicHttpEntity();

            ByteArrayInputStream is = new ByteArrayInputStream(reqBody.getBytes());
            e.setContent(is);
            httppost.setEntity(e);
//            Log.e(TAG, "Data being sent via push : " + reqBody);

            HttpResponse response = httpclient.execute(httppost);

            Log.e(TAG, response.toString());
        } catch (ClientProtocolException e) {
            Log.e(TAG, e.getMessage());
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public static void vibrateWatch(Context c) {
        ExampleGCMActivity.startWatchApp(c);
        PebbleDictionary data = new PebbleDictionary();
        data.addUint8(MSG_CMD_OUT_KEY, (byte) MSG_CMD_VIBRATE);
        PebbleKit.sendDataToPebble(c, PEBBLE_APP_UUID, data);
    }

    // Send a broadcast to launch the specified application on the connected Pebble
    public static void startWatchApp(Context c) {
        PebbleKit.startAppOnPebble(c, PEBBLE_APP_UUID);
    }
}
