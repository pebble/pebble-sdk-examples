//
//  PBWatch+AppMessages.h
//  PebbleKit
//
//  Created by Martijn The on 3/20/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <PebbleKit/PebbleKit.h>

typedef NS_ENUM(UInt8, PBAppState) {
  PBAppStateNotRunning = 0x00,
  PBAppStateRunning = 0x01,
};

@interface PBWatch (AppMessages)

/**
 *  Queries the watch whether AppMessages are supported.
 *  @param fetchedBlock The block that will be called whenthe inquiry has finished. The fetchedBlock will be called
 *  asynchronously on the queue that was originally used when calling this method.
 *  @param watch The watch on which the query was performed.
 *  @param isAppMessagesSupported YES if AppMessages are supported, NO if not.
 */
- (void)appMessagesGetIsSupported:(void(^)(PBWatch *watch, BOOL isAppMessagesSupported))fetchedBlock;

/**
 *  Pushes an update to the watch application with UUID as set using
 *  -[PBPebbleCentral setAppUUID:]
 *  @param dictionary Contains the key/value pairs to update. The dictionary can only contain items with an NSNumber
 *  key and only contain NSString, NSNumber or NSData values. Use the methods in the NSNumber (stdint) category to
 *  create NSNumbers with specific types standard integer types.
 *  @param onSent The block that will be called when the message was accepted, rejected or timed out.
 *  @param watch The watch to which the update was sent.
 *  @param update The dictionary that was sent.
 *  @param error If there was a problem, this will contain information about the underlying problem. See PBError.h for error codes.
 */
- (void)appMessagesPushUpdate:(NSDictionary*)dictionary onSent:(void(^)(PBWatch *watch, NSDictionary *update, NSError *error))onSent;

/**
 *  Pushes an update to the watch application with the specified UUID.
 *  @param dictionary Contains the key/value pairs to update. The dictionary can only contain items with an NSNumber
 *  key and only contain NSString, NSNumber or NSData values. Use the methods in the NSNumber (stdint) category to
 *  create NSNumbers with specific types standard integer types.
 *  @param onSent The block that will be called when the message was accepted, rejected or timed out.
 *  @param watch The watch to which the update was sent.
 *  @param update The dictionary that was sent.
 *  @param error If there was a problem, this will contain information about the underlying problem. See PBError.h for error codes.
 *  @param uuid The UUID of the watchapp to which the update should be sent.
 */
- (void)appMessagesPushUpdate:(NSDictionary*)dictionary withUUID:(NSData*)uuid onSent:(void(^)(PBWatch *watch, NSDictionary *update, NSError *error))onSent;

/**
 *  Add a receive handler for incoming updates that are send by the watch application with UUID as set using
 *  -[PBPebbleCentral setAppUUID:]
 *  @param onReceive The block that will be called every time a new update message arrives.
 *  @param watch The watch that has sent the update.
 *  @param update The dictionary containing the values sent by the watch.
 *  @return An opaque handle object representing the installed receive handler, that can be used in -appMessagesRemoveUpdateHandler:
 *  @see -appMessagesRemoveUpdateHandler:
 */
- (id)appMessagesAddReceiveUpdateHandler:(BOOL(^)(PBWatch *watch, NSDictionary *update))onReceive;

/**
 *  Add a receive handler for incoming updates that are send by the watch application with the specified UUID.
 *  @param onReceive The block that will be called every time a new update message arrives.
 *  @param watch The watch that has sent the update.
 *  @param update The dictionary containing the values sent by the watch.
 *  @param uuid The UUID of the watchapp for which sent messages should be handled by the onReceive block.
 *  @return An opaque handle object representing the installed receive handler, that can be used in -appMessagesRemoveUpdateHandler:
 *  @see -appMessagesRemoveUpdateHandler:
 */
- (id)appMessagesAddReceiveUpdateHandler:(BOOL(^)(PBWatch *watch, NSDictionary *update))onReceive withUUID:(NSData*)uuid;

/**
 *  Add a handler for lifecycle events of watch applications.
 *  @param onLifecycleUpdate This block will be called when an application starts and
 *  stops running.
 *  @param watch The watch that has sent the event.
 *  @param uuid The uuid of the application that caused the event.
 *  @param newAppState The new state of the app (either PBAppStateNotRunning or PBAppStateRunning).
 */
- (id)appMessagesAddAppLifecycleUpdateHandler:(void(^)(PBWatch *watch, NSUUID *uuid,
                                                       PBAppState newAppState))onLifecycleUpdate;


// INTERNAL USE ONLY
- (id)appMessagesAddReceiveAllUpdatesHandler:(BOOL(^)(PBWatch *watch, NSUUID *uuid, NSDictionary *update))onReceive;

/**
 *  Removes a receive handler that was previously installed using -appMessagesAddReceiveUpdateHandler:
 *  @param opaqueHandle The handle object as returned by -appMessagesAddReceiveUpdateHandler:
 *  @see -appMessagesAddReceiveUpdateHandler:
 */
- (void)appMessagesRemoveUpdateHandler:(id)opaqueHandle;

/**
 *  Fetches the run state of the watch application with UUID as set using
 *  -[PBPebbleCentral setAppUUID:]. You can use this method to determine whether the watch
 *  application is running or not.
 *  TODO: Ask Francois how this API works.
 *  @param onSent The block that will be called after the fetch state command has been
 *  sent to the watch.
 */
- (void)appMessagesFetchAppState:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Sends a command to launch the watch application with UUID as set using
 *  -[PBPebbleCentral setAppUUID:]
 *  @param onSent The block that will be called after the launch command has been
 *  sent to the watch.
 *  @param watch The watch to which the command was sent.
 *  @param error If there was a problem, this will contain information about the underlying problem. See PBError.h for error codes.
 */
- (void)appMessagesLaunch:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Sends a command to launch the watch application with the specified UUID.
 *  @param onSent The block that will be called after the launch command has been
 *  sent to the watch.
 *  @param watch The watch to which the command was sent.
 *  @param error If there was a problem, this will contain information about the underlying problem. See PBError.h for error codes.
 *  @param uuid The UUID of the watch application to launch.
 */
- (void)appMessagesLaunch:(void(^)(PBWatch *watch, NSError *error))onSent withUUID:(NSData*)uuid;

/**
 *  Sends a command to kill the watch application with UUID as set using
 *  -[PBPebbleCentral setAppUUID:]
 *  @param onSent The block that will be called after the kill command has been
 *  sent to the watch.
 *  @param watch The watch to which the command was sent.
 *  @param error If there was a problem, this will contain information about the underlying problem. See PBError.h for error codes.
 */
- (void)appMessagesKill:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Sends a command to kill the watch application with the specified UUID.
 *  @param onSent The block that will be called after the kill command has been
 *  sent to the watch.
 *  @param watch The watch to which the command was sent.
 *  @param error If there was a problem, this will contain information about the underlying problem. See PBError.h for error codes.
 *  @param uuid The UUID of the watch application to launch.
 */
- (void)appMessagesKill:(void(^)(PBWatch *watch, NSError *error))onSent withUUID:(NSData*)uuid;

@end
