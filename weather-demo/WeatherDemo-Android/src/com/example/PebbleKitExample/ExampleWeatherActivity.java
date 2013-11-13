package com.example.PebbleKitExample;

import android.app.Activity;
import android.content.Context;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.UUID;

/**
 * Sample activity demonstrating how to send data to a "weather" watch-app.
 *
 * The watch-app in question is available here:
 * https://github.com/pebble/pebble-apps/tree/master/demos/feature_app_messages
 *
 * @author zulak@getpebble.com
 */
public class ExampleWeatherActivity extends Activity {
    // the tuple key corresponding to the weather icon displayed on the watch
    private static final int ICON_KEY = 0;
    // the tuple key corresponding to the temperature displayed on the watch
    private static final int TEMP_KEY = 1;
    // This UUID identifies the weather app
    private static final UUID WEATHER_UUID = UUID.fromString("28AF3DC7-E40D-490F-BEF2-29548C8B0600");

    private LocationManager mLocationManager;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_weather);
        mLocationManager = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);
    }

    public void updateWeather(View view) {
        // when this button is clicked, get the handset's approximate location and request weather data from a
        // third-party web service
        LocationListener locationListener = new LocationListener() {
            public void onLocationChanged(Location location) {
                mLocationManager.removeUpdates(this);
                doWeatherUpdate(location);
            }

            public void onStatusChanged(String provider, int status, Bundle extras) {
            }

            public void onProviderEnabled(String provider) {
            }

            public void onProviderDisabled(String provider) {
            }
        };	
        
        String locationProvider = mLocationManager.getBestProvider(new Criteria(), true);
        
        if (locationProvider == null) {
        	Toast.makeText(getApplicationContext(), "No location services enabled.",
        			   Toast.LENGTH_LONG).show();
        } else {
        	mLocationManager.requestLocationUpdates(locationProvider, 0, 0, locationListener);
        }

        
    }

    public void sendWeatherDataToWatch(int weatherIconId, int temperatureCelsius) {
        // Build up a Pebble dictionary containing the weather icon and the current temperature in degrees celsius
        PebbleDictionary data = new PebbleDictionary();
        data.addUint8(ICON_KEY, (byte) weatherIconId);
        data.addString(TEMP_KEY, String.format("%d\u00B0C", temperatureCelsius));

        // Send the assembled dictionary to the weather watch-app; this is a no-op if the app isn't running or is not
        // installed
        PebbleKit.sendDataToPebble(getApplicationContext(), WEATHER_UUID, data);
    }

    public void doWeatherUpdate(Location location) {
        // A very sketchy, rough way of getting the local weather forecast from the phone's approximate location
        // using the OpenWeatherMap webservice: http://openweathermap.org/wiki/API/JSON_API
        double latitude = location.getLatitude();
        double longitude = location.getLongitude();

        try {
            URL u = new URL(String.format("http://api.openweathermap.org/data/2.1/find/city?lat=%f&lon=%f&cnt=1",
                    latitude,
                    longitude));

            HttpURLConnection urlConnection = (HttpURLConnection) u.openConnection();
            try {
                BufferedReader reader =
                        new BufferedReader(new InputStreamReader(urlConnection.getInputStream(), "UTF-8"));
                String json = reader.readLine();

                JSONObject jsonObject = new JSONObject(json);
                JSONObject l = jsonObject.getJSONArray("list").getJSONObject(0);
                JSONObject m = l.getJSONObject("main");
                double temperature = m.getDouble("temp");
                int wtype = l.getJSONArray("weather").getJSONObject(0).getInt("id");

                int weatherIcon = getIconFromWeatherId(wtype);
                int temp = (int) (temperature - 273.15);

                sendWeatherDataToWatch(weatherIcon, temp);
            } finally {
                urlConnection.disconnect();
            }

            Log.d("WeatherActivity", String.format("%f, %f", latitude, longitude));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private int getIconFromWeatherId(int weatherId) {
        if (weatherId < 600) {
            return 2;
        } else if (weatherId < 700) {
            return 3;
        } else if (weatherId > 800) {
            return 1;
        } else {
            return 0;
        }
    }

}
