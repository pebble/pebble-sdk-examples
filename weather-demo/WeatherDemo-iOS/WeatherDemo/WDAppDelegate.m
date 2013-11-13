//
//  WDAppDelegate.m
//  WeatherDemo
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "WDAppDelegate.h"
#import <PebbleKit/PebbleKit.h>
#import <CoreLocation/CoreLocation.h>

@interface WDAppDelegate () <PBPebbleCentralDelegate, CLLocationManagerDelegate>
@end

@implementation WDAppDelegate {
  PBWatch *_targetWatch;
  CLLocationManager *_locationManager;
}

- (int)getIconFromWeatherId:(int)weatherId {
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

- (void)refreshAction:(id)sender {
  if (_targetWatch == nil || [_targetWatch isConnected] == NO) {
    [[[UIAlertView alloc] initWithTitle:nil message:@"No connected watch!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    return;
  }

  // Fetch weather at current location using openweathermap.org's JSON API:
  CLLocationCoordinate2D coordinate = _locationManager.location.coordinate;
  NSString *apiURLString = [NSString stringWithFormat:@"http://api.openweathermap.org/data/2.1/find/city?lat=%f&lon=%f&cnt=1", coordinate.latitude, coordinate.longitude];
  NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:apiURLString]];
  NSOperationQueue *queue = [[NSOperationQueue alloc] init];
  [NSURLConnection sendAsynchronousRequest:request queue:queue completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
    NSHTTPURLResponse *httpResponse = nil;
    if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
      httpResponse = (NSHTTPURLResponse *) response;
    }

    // NSURLConnection's completionHandler is called on the background thread.
    // Prepare a block to show an alert on the main thread:
    __block NSString *message = @"";
    void (^showAlert)(void) = ^{
      [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        [[[UIAlertView alloc] initWithTitle:nil message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
      }];
    };

    // Check for error or non-OK statusCode:
    if (error || httpResponse.statusCode != 200) {
      message = @"Error fetching weather";
      showAlert();
      return;
    }

    // Parse the JSON response:
    NSError *jsonError = nil;
    NSDictionary *root = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonError];
    @try {
      if (jsonError == nil && root) {
        // TODO: type checking / validation, this is really dangerous...
        NSDictionary *firstListItem = [root[@"list"] objectAtIndex:0];
        NSDictionary *main = firstListItem[@"main"];

        // Get the temperature:
        NSNumber *temperatureNumber = main[@"temp"]; // in degrees Kelvin
        int temperature = [temperatureNumber integerValue] - 273.15;

        // Get weather icon:
        NSNumber *weatherIconNumber = firstListItem[@"weather"][0][@"icon"];
        uint8_t weatherIconID = [self getIconFromWeatherId:[weatherIconNumber integerValue]];

        // Send data to watch:
        // See demos/feature_app_messages/weather.c in the native watch app SDK for the same definitions on the watch's end:
        NSNumber *iconKey = @(0); // This is our custom-defined key for the icon ID, which is of type uint8_t.
        NSNumber *temperatureKey = @(1); // This is our custom-defined key for the temperature string.
        NSDictionary *update = @{ iconKey:[NSNumber numberWithUint8:weatherIconID],
                                  temperatureKey:[NSString stringWithFormat:@"%d\u00B0C", temperature] };
        [_targetWatch appMessagesPushUpdate:update onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
          message = error ? [error localizedDescription] : @"Update sent!";
          showAlert();
        }];
        return;
      }
    }
    @catch (NSException *exception) {
    }
    message = @"Error parsing response";
    showAlert();
  }];
}

- (void)setTargetWatch:(PBWatch*)watch {
  _targetWatch = watch;

  // NOTE:
  // For demonstration purposes, we start communicating with the watch immediately upon connection,
  // because we are calling -appMessagesGetIsSupported: here, which implicitely opens the communication session.
  // Real world apps should communicate only if the user is actively using the app, because there
  // is one communication session that is shared between all 3rd party iOS apps.

  // Test if the Pebble's firmware supports AppMessages / Weather:
  [watch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
    if (isAppMessagesSupported) {
      // Configure our communications channel to target the weather app:
      // See demos/feature_app_messages/weather.c in the native watch app SDK for the same definition on the watch's end:
      uint8_t bytes[] = {0x28, 0xAF, 0x3D, 0xC7, 0xE4, 0x0D, 0x49, 0x0F, 0xBE, 0xF2, 0x29, 0x54, 0x8C, 0x8B, 0x06, 0x00};
      NSData *uuid = [NSData dataWithBytes:bytes length:sizeof(bytes)];
      [[PBPebbleCentral defaultCentral] setAppUUID:uuid];

      NSString *message = [NSString stringWithFormat:@"Yay! %@ supports AppMessages :D", [watch name]];
      [[[UIAlertView alloc] initWithTitle:@"Connected!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    } else {

      NSString *message = [NSString stringWithFormat:@"Blegh... %@ does NOT support AppMessages :'(", [watch name]];
      [[[UIAlertView alloc] initWithTitle:@"Connected..." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    }
  }];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

  _locationManager = [[CLLocationManager alloc] init];
  _locationManager.distanceFilter = 5.0 * 1000.0; // Move at least 5km until next location event is generated
  _locationManager.desiredAccuracy = kCLLocationAccuracyThreeKilometers;
  _locationManager.delegate = self;
  [_locationManager startUpdatingLocation];

  UIButton *refreshButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
  [refreshButton setTitle:@"Fetch Weather" forState:UIControlStateNormal];
  [refreshButton addTarget:self action:@selector(refreshAction:) forControlEvents:UIControlEventTouchUpInside];
  [refreshButton setFrame:CGRectMake(10, 100, 300, 100)];
  [self.window addSubview:refreshButton];

  [self.window makeKeyAndVisible];

  // We'd like to get called when Pebbles connect and disconnect, so become the delegate of PBPebbleCentral:
  [[PBPebbleCentral defaultCentral] setDelegate:self];

  // Initialize with the last connected watch:
  [self setTargetWatch:[[PBPebbleCentral defaultCentral] lastConnectedWatch]];
  return YES;
}

/*
 *  PBPebbleCentral delegate methods
 */

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidConnect:(PBWatch*)watch isNew:(BOOL)isNew {
  [self setTargetWatch:watch];
}

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidDisconnect:(PBWatch*)watch {
  [[[UIAlertView alloc] initWithTitle:@"Disconnected!" message:[watch name] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
  if (_targetWatch == watch || [watch isEqual:_targetWatch]) {
    [self setTargetWatch:nil];
  }
}

/*
 *  CLLocationManagerDelegate
 */

// iOS 5 and earlier:
- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation {
  NSLog(@"New Location: %@", newLocation);
}

// iOS 6 and later:
- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations {
  CLLocation *lastLocation = [locations lastObject];
  [self locationManager:manager didUpdateToLocation:lastLocation fromLocation:nil];
}

@end
