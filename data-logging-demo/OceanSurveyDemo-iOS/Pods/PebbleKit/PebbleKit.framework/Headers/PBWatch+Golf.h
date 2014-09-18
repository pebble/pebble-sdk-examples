//
//  PBWatch+Golf.h
//  PebbleKit
//
//  Created by Martijn The on 4/4/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <PebbleKit/PebbleKit.h>
#import <UIKit/UIKit.h>

@interface PBWatch (Golf)

/**
 *  The UUID of the Golf watch app.
 */
extern NSData *PBGolfUUID;

/**
 *  The key of the "Front" field.
 *  The accompanying value must be an NSString (max. ~5 characters).
 *  @see -golfAppUpdate:onSent:
 */
extern NSNumber *PBGolfFrontKey;

/**
 *  The key of the "Mid" field.
 *  The accompanying value must be an NSString (max. ~5 characters).
 *  @see -golfAppUpdate:onSent:
 */
extern NSNumber *PBGolfMidKey;

/**
 *  The key of the "Back" field.
 *  The accompanying value must be an NSString (max. ~5 characters).
 *  @see -golfAppUpdate:onSent:
 */
extern NSNumber *PBGolfBackKey;

/**
 *  The key of the "Hole" field.
 *  The accompanying value must be an NSString (max. ~5 characters).
 *  @see -golfAppUpdate:onSent:
 */
extern NSNumber *PBGolfHoleKey;

/**
 *  The key of the "Par" field.
 *  The accompanying value must be an NSString (max. ~5 characters).
 *  @see -golfAppUpdate:onSent:
 */
extern NSNumber *PBGolfParKey;

/**
 *  The command IDs that can be sent back by the Golf watch app.
 *  @see -golfAppAddReceiveUpdateHandler:
 */
typedef enum {
  GolfAppCommandPrevious = 0x01,
  GolfAppCommandNext = 0x02,
  GolfAppCommandSelect = 0x03,
} GolfAppCommand;

/**
 *  Queries the watch whether Golf Messages are supported.
 *  @param fetchedBlock The block that will be called whenthe inquiry has finished. The fetchedBlock will be called
 *  asynchronously on the queue that was originally used when calling this method.
 *  @param watch The watch on which the query was performed.
 *  @param isGolfSupported YES if Golf Messages are supported, NO if not.
 */
- (void)golfGetIsSupported:(void(^)(PBWatch *watch, BOOL isGolfSupported))fetchedBlock;

/**
 *  Send a command to launch the golf app on the watch that the receiver represents.
 *  @param onSent The handler that will be called when the launch command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)golfAppLaunch:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Send a command to kill the golf app on the watch that the receiver represents.
 *  @param onSent The handler that will be called when the kill command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)golfAppKill:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Sends an update to the golf app on the watch that the receiver represents.
 *  @param update The update to send. Use one or more keys (PBGolfFrontKey, PBGolfMidKey, PBGolfBackKey, PBGolfHoleKey, PBGolfParKey)
 *  note that the value for each key MUST be of NSString type.
 *  @param onSent The handler that will be called when the update has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 *  @see PBGolfFrontKey, PBGolfMidKey, PBGolfBackKey, PBGolfHoleKey and PBGolfParKey
 */
- (void)golfAppUpdate:(NSDictionary*)update onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Add a receive handler for incoming Golf updates that are send by the Golf watch application.
 *  @param onReceive The block that will be called every time a new update message arrives.
 *  @param watch The watch that has sent the update.
 *  @param command The command as sent by the watch.
 *  @return An opaque handle object representing the installed receive handler, that can be used in -golfAppRemoveUpdateHandler:
 *  @see GolfAppCommand
 *  @see -golfAppRemoveUpdateHandler:
 */
- (id)golfAppAddReceiveUpdateHandler:(BOOL(^)(PBWatch *watch, GolfAppCommand command))onReceive;

/**
 *  Removes a receive handler that was previously installed using -golfAppAddReceiveUpdateHandler:
 *  @param opaqueHandle The handle object as returned by -golfAppAddReceiveUpdateHandler:
 *  @see -golfAppAddReceiveUpdateHandler:
 */
- (void)golfAppRemoveUpdateHandler:(id)opaqueHandle;

/**
 *  Assigns a custom title and icon to the golf app on the watch.
 *  @discussion It is recommended to perform this as the first call after -golfGetIsSupported:
 *  to avoid changing the title and icon while it is being displayed in the menu as to avoid confusion.
 *  @param title The custom title (max. 20 bytes of UTF-8 string)
 *  @param icon The custom icon (max. 32x32 pixels, black/white only)
 *  @param onSent The handler that will be called when the title and icon have been set or timed out.
 *  @param watch The watch on which the custom title and icon have been set.
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)golfSetTitle:(NSString*)title icon:(UIImage*)icon onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

@end
