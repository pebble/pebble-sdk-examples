# Ocean Surveying

An example Android application that exercises the data spooling APIs of `PebbleKit`.

## Dependencies

1. Android SDK (http://developer.android.com/sdk/index.html)
1. PebbleKit-Android, which targets Android API 10 by default
1. Google Guava (https://code.google.com/p/guava-libraries/), included in `libs/`

## Building the APK

Apache Ant may be used to build the example application from the command line.
For information, see the Android documentation on [building from the command line](http://developer.android.com/tools/building/building-cmdline.html).

To build this example application outside the directory structure of `pebblesdk-examples`, you may need to update the relative path of the `PebbleKit-Android` directory.
To do this, use the android command line tools to update the build scripts, indicating the new relative path to the
PebbleKit directory.
For example, to compile this example application in its default location in the `pebblesdk-examples` repository:

    rm project.properties; android update project --path ./ --library ../../../PebbleKit-Android/PebbleKit

To build and deploy a debug APK using Apache Ant:

    ant clean debug install

