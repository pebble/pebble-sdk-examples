Important information for Android Examples users
================================================

Pebble Android examples are built and easier to run with Android Studio. If you
prefer to use Eclipse, follow the instructions below to import the examples and
add the PebbleKit library.

## Opening Android Examples with Android Studio

 - Open 'Android Studio'
 - Select 'Open project' in the start menu (or the main menu)
 - Select one of the Android examples (e.g.: weather-demo/WeatherDemo-Android)
 - Click "Open"

Android Studio will automatically download the PebbleKit library. You can now
build and run the project.

## Opening Android Examples with Eclipse ADT (Android Developer Tools)

To import one of Pebble Android examples in Eclipse:

 - Open Eclipse
 - Select File -> Import... -> Android / Existing Android Code Into Workspace
 - Click on 'Browse' next to 'Root Directory' and select the root of one of the
   Android examples For example: weather-demo/WeatherDemo-Android
 - Click "Finish"

With Eclipse, you must manually download the PebbleKit Android for Eclipse
library and add it to the project.

The most up to date version will always be available from GitHub:
https://github.com/pebble/pebblekit-android

Download the 'pebblekit-2.x.x-eclipse.jar' file and:

 - Create a "libs" folder in ExampleName-Android/app/src/main/
 - Add the file in this folder
 - Refresh the project in Eclipse

Finally, Eclipse cannot import Android Studio projects properly and you will
need to manually fix the list of source folder:

 - Right click on the project and select Properties
 - Open "Java Build Path"
 - In the Source tab, click "Add Folder", select the checkbox next to "java" and
   click Ok twice to go back to the project.

That's it! You can now build and run the project.

## Documentation and Support

Full documentation is available on Pebble developer website:
http://developer.getpebble.com/guides/mobile-apps/android/

We would love to hear your questions and feedbacks on those examples, you can
contact us at: devsupport@getpebble.com.
