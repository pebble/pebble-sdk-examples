//
//  PBWatch+Sports.h
//  PebbleKit
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <PebbleKit/PebbleKit.h>
#import <UIKit/UIKit.h>

/**
 *  The UUID of the Sports watch app.
 */
extern NSData *PBSportsUUID;

/**
 *  The key of the "Time" field.
 *  The accompanying value must be an NSString (max. ~8 characters).
 *  @see -sportsAppUpdate:onSent:
 *  @see PBSportsUpdate
 */
extern NSNumber *PBSportsTimeKey;

/**
 *  The key of the "Distance" field.
 *  The value must be an NSString (max. ~5 characters).
 *  @see -sportsAppUpdate:onSent:
 *  @see PBSportsUpdate
 */
extern NSNumber *PBSportsDistanceKey;

/**
 *  The key of the general purpose "Data" field.
 *  The value must be an NSString (max. ~5 characters).
 *  @see -sportsAppUpdate:onSent:
 *  @see PBSportsUpdate
 */
extern NSNumber *PBSportsDataKey;

/**
 *  The key of the "Activity State".
 *  The value must be an NSNumber created using +[NSNumber numberWithUint8:].
 *  @see SportsAppActivityState for valid values to use
 *  @see -sportsAppUpdate:onSent:
 *  @see PBSportsUpdate
 */
extern NSNumber *PBSportsActivityStateKey;

/**
 *  Instances of this convenience class represent an update of the current time, distance and pace,
 *  and has a method to convert the values into a dictionary that can be used with -sportsAppUpdate:onSent:.
 *  @see -sportsAppUpdate:onSent:
 */
@interface PBSportsUpdate : NSObject

/**
 *  The current time in seconds. The possible range is currently limited from -7140. to 7140., inclusive. (±1h 59min 59sec).
 */
@property (nonatomic, readwrite, assign) NSTimeInterval time;

/**
 *  The current distance in km or mi. The possible range is currently limited from -99.9 to 99.9, inclusive.
 *  The unit of distance is dependent on the current unit setting.
 *  @see -sportsAppSetMetric:onSent:
 */
@property (nonatomic, readwrite, assign) float distance;

/**
 *  General purpose data variable. The possible range is currently limited from -99.99 to 99.99, inclusive.
 *  @see -sportsAppSetMetric:onSent:
 */
@property (nonatomic, readwrite, assign) float data;

/**
 *  Convenience method to create an update object.
 */
+ (PBSportsUpdate*)updateWithTime:(NSTimeInterval)time distance:(float)distance data:(float)data;

/**
 *  Creates an update dictionary from the receiver, that can be used with -sportsAppUpdate:onSent:.
 *  @see -sportsAppUpdate:onSent:
 */
- (NSDictionary*)dictionary;

/**
 *  Creates a formatted time string with from a total seconds value, formatted as "h:mm:ss".
 *  For example, supplying the value 3930.0f seconds will return @"1:05:30"
 *  @param seconds The number of seconds from which to create the time string.
 *  @return Formatted time as "h:mm:ss"
 */
+ (NSString*)timeStringFromFloat:(float)seconds;

@end

@interface PBWatch (Sports)

/**
 *  The state of the Sports activity.
 *  @see -sportsAppAddReceiveUpdateHandler:
 */
typedef enum {
  SportsAppActivityStateInit = 0x00,
  SportsAppActivityStateRunning = 0x01,
  SportsAppActivityStatePaused = 0x02,
  SportsAppActivityStateEnd = 0x03,
} SportsAppActivityState;

/**
 *  Queries the watch whether Sports Messages are supported.
 *  @param fetchedBlock The block that will be called whenthe inquiry has finished. The fetchedBlock will be called
 *  asynchronously on the queue that was originally used when calling this method.
 *  @param watch The watch on which the query was performed.
 *  @param isSportsSupported YES if Sports Messages are supported, NO if not.
 */
- (void)sportsGetIsSupported:(void(^)(PBWatch *watch, BOOL isSportsSupported))fetchedBlock;

/**
 *  Send a command to launch the sports app on the watch that the receiver represents.
 *  @param onSent The handler that will be called when the launch command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsAppLaunch:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Send a command to kill the sports app on the watch that the receiver represents.
 *  @param onSent The handler that will be called when the kill command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsAppKill:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Send a command to the sports app on the watch that the receiver represents, to set the preferred
 *  unit system, either metric or imperial.
 *  @param isMetric YES to request metric units or NO to request imperial units
 *  @param onSent The handler that will be called when the unit command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsAppSetMetric:(BOOL)isMetric onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Send a command to the sports app on the watch that the receiver represents,
 *  to set the preferred  data label (either PACE or SPEED) and corresponding units.
 *  @param isMetric YES to request metric units or NO to request imperial units
 *  @param onSent The handler that will be called when the unit command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsAppSetLabel:(BOOL)isPace onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Send a command to the sports app on the watch that the receiver represents, to set the state of the
 *  sports activity. Currently only SportsAppActivityStateRunning and SportsAppActivityStatePaused are
 *  supported.
 *  @param state The new sports activity state
 *  @param onSent The handler that will be called when the unit command has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsAppSetActivityState:(SportsAppActivityState)state onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Sends the update to the sports app on the watch that the receiver represents.
 *  @param update The update to send. Use one or more keys (PBSportsTimeKey, PBSportsPaceKey, PBSportsDistanceKey)
 *  note that the value for each key MUST be of NSString type.
 *  @param onSent The handler that will be called when the update has been sent or timed out (after 1.5 secs).
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsAppUpdate:(NSDictionary*)update onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

/**
 *  Add a receive handler for incoming Sports updates that are send by the Sports watch application.
 *  @param onReceive The block that will be called every time a new update message arrives.
 *  @param watch The watch that has sent the update.
 *  @param state The new sports activity state as set by the watch.
 *  @return An opaque handle object representing the installed receive handler, that can be used in -sportsAppRemoveUpdateHandler:
 *  @see SportsAppActivityState
 *  @see -sportsAppRemoveUpdateHandler:
 */
- (id)sportsAppAddReceiveUpdateHandler:(BOOL(^)(PBWatch *watch, SportsAppActivityState state))onReceive;

/**
 *  Removes a receive handler that was previously installed using -sportsAppAddReceiveUpdateHandler:
 *  @param opaqueHandle The handle object as returned by -sportsAppAddReceiveUpdateHandler:
 *  @see -sportsAppAddReceiveUpdateHandler:
 */
- (void)sportsAppRemoveUpdateHandler:(id)opaqueHandle;

/**
 *  Assigns a custom title and icon to the sports app on the watch.
 *  @discussion It is recommended to perform this as the first call after -sportsGetIsSupported:
 *  to avoid changing the title and icon while it is being displayed in the menu as to avoid confusion.
 *  @param title The custom title (max. 20 bytes of UTF-8 string)
 *  @param icon The custom icon (max. 32x32 pixels, black/white only)
 *  @param onSent The handler that will be called when the title and icon have been set or timed out.
 *  @param watch The watch on which the custom title and icon have been set.
 *  @param error nil if the operation was successful, or else an NSError with more information why it failed.
 */
- (void)sportsSetTitle:(NSString*)title icon:(UIImage*)icon onSent:(void(^)(PBWatch *watch, NSError *error))onSent;

@end
