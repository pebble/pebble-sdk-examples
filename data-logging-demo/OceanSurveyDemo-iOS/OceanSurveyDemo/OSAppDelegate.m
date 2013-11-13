//
//  OSAppDelegate.m
//
//  Created by Martijn The on 8/13/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "OSAppDelegate.h"

#import "OSDataLoggingViewController.h"

#import <PebbleKit/PebbleKit.h>

@interface OSAppDelegate () <PBPebbleCentralDelegate>
@end

@implementation OSAppDelegate {
  OSDataLoggingViewController *_dataLoggingViewController;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  [PBPebbleCentral setDebugLogsEnabled:YES];
  
  uuid_t myAppUUIDbytes;
  NSUUID *myAppUUID = [[NSUUID alloc] initWithUUIDString:@"0a5399d9-5693-4f3e-b768-9c99b5f5dcea"];
  [myAppUUID getUUIDBytes:myAppUUIDbytes];
  [[PBPebbleCentral defaultCentral] setAppUUID:[NSData dataWithBytes:myAppUUIDbytes length:16]];
  [[PBPebbleCentral defaultCentral] setDelegate:self];

  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  _dataLoggingViewController = [[OSDataLoggingViewController alloc] initWithNibName:nil bundle:nil];
  self.window.rootViewController = [[UINavigationController alloc] initWithRootViewController:_dataLoggingViewController];
  [self.window makeKeyAndVisible];

  return YES;
}

@end
