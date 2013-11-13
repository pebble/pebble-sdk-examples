PebbleKitExample
================

A class containing sample code that demonstrates how Android applications can
send to and receive data from Pebble Smartwatch applications that support
AppMessages.

Emergency SMS: ExampleSmsActivity
---------------------------------
This app looks like an ordinary clock, but pressing a button on your Pebble will discreetly alert a friend to your current location so they can rescue you. 

1. From the watch SDK, build and install demos/feature_app_messages_send to your Pebble.
2. Launch the ExamplePebbleKitActivity on your phone.
3. Tap "GPS/SMS App Example"
4. Enter a phone number, or tap "Select Contact" to choose one.
5. If the app hasn't already launched on your watch, pick GPS/SMS Time from the watch menu
6. On your watch, press the up button

You'll see GPS start up on your phone.  After it gets a fix, the number you entered will receive two texts: one with GPS coordinates, and another with the address corresponding to those coordinates (if the Geocoding API can find one.)

Note: this app will only function if it's in the foreground, and the screen is on.

---

Pebble-to-pebble communication via Google Cloud Messaging and NFC: ExampleGCMActivity
-------------------------------------------------------------------------------------
This app lets you pair a friend's Pebble with yours so you can send a vibrating notification from your watch to theirs.   It's called Happy Hour because it could be a good way to tell a friend about free beer or food.  You'll need two Pebbles and two NFC-enabled Android devices for this app.

1. From the watch SDK, build and install demos/app_vibrate_paired_pebble to your Pebble.
2. Launch the ExamplePebbleKitActivity on your phones.  
3. Tap "GCM Activity" on both phones.
4. Tap the two phones together.  When the Android Beam UI comes up, tap "Touch to Beam" on one of the phones.  Tap the two phones again, and tap the send button on the other phone.
5. If the app hasn't already launched on your watch, pick Happy Hour from the watch menu
6. The phones are now paired.  Pressing the up button on the watch will make the other watch vibrate.

How it works: the NFC tap sends one phone's GCM id to the other phone.  We ping the GCM server directly from the phone, rather than needing to have a backend.  When a GCM notification arrives, it sends an app message to the watch, causing it to vibrate.

Note: this app will only function if it's in the foreground, and the screen is on.
