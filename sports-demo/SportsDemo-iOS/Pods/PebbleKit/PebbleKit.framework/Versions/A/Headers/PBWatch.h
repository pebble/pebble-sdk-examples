//
//  PBWatch.h
//  PebbleSDK-iOS
//
//  Created by Martijn Thé on 4/24/12.
//  Copyright (c) 2012 Pebble Technology. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol PBWatchDelegate;
@class PBVersionInfo;
@class PBDataLoggingService;

/**
 *  Represents a Pebble watch.
 *
 *  On iOS, there is the notion of a "communication session" between an app and
 *  the Pebble watch. You can imagine this as a "data pipe" directly from the
 *  watch to your app. However, there is only one session for 3rd party apps.
 *  This one session has to be shared among all 3rd party apps that want to
 *  communicate with Pebble (excluding the official Pebble iOS app).
 *
 *  Unfortunately, the session can only be opened from the phone app. This means
 *  that the iOS app has to start talking to the watch first to open the
 *  communications channel; the other way around is not possible.
 *  Most methods in the PBWatch categories (Ping), (Version), (Sports), (Golf)
 *  and (AppMessages) all implicitly open the shared communication session.
 *
 *  Lastly, iOS provides no good way to manage how the one shared session is
 *  accessed or used. Simply, the last iOS app to try to open it, wins and gets
 *  to use it. Therefore, it is important that your iOS app does **not** try
 *  communicate when a Pebble is connected.
 *
 *  When the user has stopped using your app (e.g. when going to the background
 *  or when explicitely stopping a certain activity in the app) your iOS app
 *  should close the communication session using the -closeSession: method.
 */
@interface PBWatch : NSObject

/**
 *  YES if the receiver is connected and NO if the receiver is disconnected.
 *  @discussion This property is KVO-compliant.
 */
@property (nonatomic, readonly, getter=isConnected) BOOL connected;

/**
 *  The human-friendly name of the receiver.
 *  This is the same name as the user will see in the iOS Bluetooth Settings.
 */
@property (nonatomic, readonly) NSString *name;

/**
 *  The serial number of the receiver.
 */
@property (nonatomic, readonly) NSString *serialNumber;

/**
 *  The versionInfo of the receiver.
 *  @see PBVersionInfo
 */
@property (nonatomic, readonly) PBVersionInfo *versionInfo;

/**
 *  The delegate of the watch that will be notified of disconnections and errors.
 *  @see PBWatchDelegate
 */
@property (nonatomic, readwrite, weak) id<PBWatchDelegate> delegate;

/**
 *  The userInfo property can be used to associate application specific data
 *  with the watch. Note that the application itself is responsible for persisting
 *  the information if neccessary.
 */
@property (nonatomic, readwrite, strong) id userInfo;

/**
 *  The date when the watch was last known to be connected.
 *  This date will be updated automatically when the watch connects and
 *  disconnects. While the watch is being connected, this date will not be updated.
 */
@property (nonatomic, readonly) NSDate *lastConnectedDate;

/**
 *  Developer-friendly debugging description of the watch.
 *  @return localized, user-friendly summary of the receiver, including
 *  software and hardware version information, if available.
 */
- (NSString*)friendlyDescription;

/**
 *  Closes the communication session with the watch.
 *  Since there is only one, shared session for all 3rd party iOS apps,
 *  an app should close the session after the user is done using the app/watch-integration,
 *  so it can be used by other apps.
 *  The communication session is implicitely opened automatically when needed.
 *  @param onDone Callback block that will be called after the closing of the session
 *  has completed. If there is no open session, the onDone block will (also) be executed
 *  asynchronously on the calling queue.
 */
- (void)closeSession:(void(^)(void))onDone;

@end

@protocol PBWatchDelegate <NSObject>
@optional

/**
 *  Called when the watch got disconnected.
 */
- (void)watchDidDisconnect:(PBWatch*)watch;

/**
 *  Called when the watch caught an error.
 */
- (void)watch:(PBWatch*)watch handleError:(NSError*)error;

/**
 *  Called when the internal EASession is about to be reset.
 */
- (void)watchWillResetSession:(PBWatch*)watch;

/**
 *  Called when the internal EASession is opened
 */
- (void)watchDidOpenSession:(PBWatch*)watch;

/**
 *  Called when the internal EASession is closed
 */
- (void)watchDidCloseSession:(PBWatch*)watch;

@end
