# PebbleKit iOS

Welcome to Pebble's official iOS SDK!

## Compatibility

- iOS 5.0+
- iPhone 3GS or later

## Examples

All Pebble SDK examples are now grouped in Pebble SDK, please look into the `examples/` folder of your Pebble SDK for sample code.

## Integrating PebbleKit using Cocoapods

- Install [Cocoapods](http://www.cocoapods.org) or make sure it is up to date
- Add a Podfile to your project if you don't have one already (Hint: use `pod init`).
- Add this line to the Podfile: `pod 'PebbleKit', :path => "/path/to/PebbleKit-iOS"`
- Run `pod install`

## Integrating PebbleKit Manually

- Drag PebbleKit.framework into project
- Drag in PebbleVendor.framework into the project, or, drag the PebbleVendor.xcodeproj into project if you need to control the 3rd party components needed for PebbleKit.
- Link ExternalAccessory.framework, libz.dylib, CoreBluetooth.framework, CoreMotion.framework and MessageUI.framework
- Add "-ObjC" linker flag to your project's build settings
- Add the value "com.getpebble.public" to the "Supported external accessory protocols" (UISupportedExternalAccessoryProtocols) array in your app's Info.plist
- Optionally, add the value "App communicates with an accessory" (external-accessory) to the "Required background modes" (UIBackgroundModes) array in your app's Info.plist


## Xcode Documentation

- An Xcode docset is included with documentation about all public APIs.
- Copy `com.getpebble.PebbleKit.docset` content into `~/Library/Developer/Shared/Documentation/DocSets`
- Restart Xcode. The documentation will now be available from `Help > Documentation and API Reference`

## Submitting iOS apps with PebbleKit to Apple's App Store

In order for Pebble to work with iPhones, Pebble is part of the Made For iPhone program (a requirement for hardware accessories to interact with iOS apps). Unfortunately this also means that if you build an iOS app with PebbleKit, we (Pebble) will need to whitelist your iOS app before you can upload it to the App Store. If you have completed a Pebble app and would like to learn more about making it available on the App Store, please visit [the whitelisting guide](https://developer.getpebble.com/2/distribute/whitelisting.html)

## Change Log

#### TODO-SetDate - 2.0

- Remove examples from PebbleKit and move them into Pebble SDK

#### 2013-07-03 - 1.12
- Log errors to the console if there is no delegate set on PBWatch
- Bugfixes
-- Fix threading bugs in WeatherDemo
-- Fix bugs that could cause callbacks to be called on the wrong thread
-- Fix a bug that could cause a crash when re-connecting to the watch
-- Fix a bug that caused some types to be unavailable (gtypes.h)

#### 2013-05-06
- Added WeatherDemo sample project to demonstrate custom use of the AppMessage subsystem
- Added -[PBWatch closeSession:] to enable 3rd party apps to explicitely close the shared communication session.
- Added PBBitmap helper class to convert UIImage to the native Pebble bitmap format
- Exposed category methods on NSData/NSDictionary to (de)serialize from/to Pebble dicts
- Added documentation for the NSNumber+stdint category

#### 2013-03-25
- Added generic bi-directional phone app <-> watch app communication layer, called "App Messages"
- Refactored legacy Sports protocol to use App Messages
- Added APIs to query the watch whether App / Sports Messages are supported (-appMessagesGetIsSupported: and -sportsGetIsSupported:)
- Added API to set custom icon / title to the Sports watch app
- Added API to receive Sports activity state changes by pressing the SELECT button on the watch (-sportsAppAddReceiveUpdateHandler:)
