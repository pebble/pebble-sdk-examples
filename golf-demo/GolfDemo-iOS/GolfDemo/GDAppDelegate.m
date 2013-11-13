//
//  GDAppDelegate.m
//  GolfDemo
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "GDAppDelegate.h"
#import "GDAppViewController.h"
#import "GDDataViewController.h"
#import "GDIconViewController.h"

#import <PebbleKit/PebbleKit.h>

@interface GDAppDelegate () <PBPebbleCentralDelegate>
@end

@implementation GDAppDelegate {
  PBWatch *_targetWatch;
  GDAppViewController *_appViewController;
  GDDataViewController *_dataViewController;
  GDIconViewController *_iconViewController;
}

- (void)setTargetWatch:(PBWatch*)watch {
  _targetWatch = watch;

  // NOTE:
  // For demonstration purposes, we start communicating with the watch immediately upon connection,
  // because we are calling -appMessagesGetIsSupported: here, which implicitely opens the communication session.
  // Real world apps should communicate only if the user is actively using the app, because there
  // is one communication session that is shared between all 3rd party iOS apps.

  // Test if the Pebble's firmware supports AppMessages / Golf:
  [watch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
    if (isAppMessagesSupported) {
      // Configure our communications channel to target the golf app:
      [[PBPebbleCentral defaultCentral] setAppUUID:PBGolfUUID];
      [_appViewController setTargetWatch:_targetWatch];
      [_dataViewController setTargetWatch:_targetWatch];
      [_iconViewController setTargetWatch:_targetWatch];

      NSString *message = [NSString stringWithFormat:@"Yay! %@ supports AppMessages :D", [watch name]];
      [[[UIAlertView alloc] initWithTitle:@"Connected!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    } else {
      [_appViewController setTargetWatch:nil];
      [_dataViewController setTargetWatch:nil];
      [_iconViewController setTargetWatch:nil];

      NSString *message = [NSString stringWithFormat:@"Blegh... %@ does NOT support AppMessages :'(", [watch name]];
      [[[UIAlertView alloc] initWithTitle:@"Connected..." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    }
  }];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  // Override point for customization after application launch.
  _appViewController = [[GDAppViewController alloc] init];
  _dataViewController = [[GDDataViewController alloc] init];
  _iconViewController = [[GDIconViewController alloc] init];
  UINavigationController *nav1 = [[UINavigationController alloc] initWithRootViewController:_appViewController];
  nav1.tabBarItem.title = @"App";
  nav1.tabBarItem.image = [UIImage imageNamed:@"first"];
  UINavigationController *nav2 = [[UINavigationController alloc] initWithRootViewController:_dataViewController];
  nav2.tabBarItem.title = @"Data";
  nav2.tabBarItem.image = [UIImage imageNamed:@"second"];
  UINavigationController *nav3 = [[UINavigationController alloc] initWithRootViewController:_iconViewController];
  nav3.tabBarItem.title = @"Icon";
  nav3.tabBarItem.image = [UIImage imageNamed:@"second"];
  self.tabBarController = [[UITabBarController alloc] init];
  self.tabBarController.viewControllers = @[nav1, nav2, nav3];
  self.window.rootViewController = self.tabBarController;
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

@end
