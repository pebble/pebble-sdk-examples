//
//  PBPebbleCentral.h
//  PebbleSDK-iOS
//
//  Created by Martijn Thé on 4/24/12.
//  Copyright (c) 2012 Pebble Technology. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PBWatch;
@class PBDataLoggingService;
@protocol PBPebbleCentralDelegate;

/**
 PebbleCentral plays the central role for client iOS apps (e.g. RunKeeper).
 */
@interface PBPebbleCentral : NSObject

/**
 @returns The default central singleton instance.
 */
+ (PBPebbleCentral*)defaultCentral;

/**
 *  Enables debug logs. The logs will be routed to the system log (ASL) and
 *  console. It is advised to call this before making any other calls to PebbleKit.
 */
+ (void)setDebugLogsEnabled:(BOOL)logsEnabled;

/**
 The watches that are currently connected. Do not cache the array because it can change over time.
 */
@property (nonatomic, readonly, strong) NSArray *connectedWatches;

/**
 The watches that are stored in the user preferences of the application.
 */
@property (nonatomic, readonly, strong) NSArray *registeredWatches;

/**
 The central's delegate.
 */
@property (nonatomic, readwrite, weak) id<PBPebbleCentralDelegate> delegate;

/**
 *  The UUID is used as the identifier of the watch application and is used
 *  to make sure that appMessage and dataLogging communications arrives at its companion app
 *  on the other device (and not in another app).
 *  For most app message methods there is are two variants: one that does not take a UUID parameter and one that does take
 *  a UUID parameter (for example, -appMessagesAddReceiveUpdateHandler: vs appMessagesAddReceiveUpdateHandler:withUUID:).
 *  The methods that do not take a UUID, will use the UUID as set prior to this property.
 *  @param uuid The 16 byte UUID of your app.
 *  @note The UUID needs to be set before using either app message or data logging.
 */
@property (nonatomic, strong, readwrite) NSData *appUUID;

/**
 *  Verifies the currently set application UUID.
 *  @return YES if the currently set UUID is valid, NO if it is not.
 *  @see -setAppUuid:
 */
- (BOOL)hasValidAppUUID;

/**
 @returns YES if the Pebble iOS app is installed, NO if it is not installed.
 */
- (BOOL)isMobileAppInstalled;

/**
 Redirects to Pebble in the App Store, so the user can install the app.
 */
- (void)installMobileApp;

/**
 Wipes out the data associated with the registered watches, that is stored on the phone.
 */
- (void)unregisterAllWatches;

/**
 Returns the most recently connected watch from the -registeredWatches array.
 */
- (PBWatch*)lastConnectedWatch;

@property (nonatomic, readonly) PBDataLoggingService *dataLoggingService;

@end


@protocol PBPebbleCentralDelegate <NSObject>
@optional

/**
 @param central The Pebble Central responsible for calling the delegate method.
 @param watch The PBWatch object representing the watch that was connected.
 @param isNew YES if the watch has been connected for the first time since the app has been installed or NO if not.
 */
- (void)pebbleCentral:(PBPebbleCentral*)central watchDidConnect:(PBWatch*)watch isNew:(BOOL)isNew;

/**
 @param central The Pebble Central responsible for calling the delegate method.
 @param watch The PBWatch object representing the watch that was disconnected.
 */
- (void)pebbleCentral:(PBPebbleCentral*)central watchDidDisconnect:(PBWatch*)watch;

@end
