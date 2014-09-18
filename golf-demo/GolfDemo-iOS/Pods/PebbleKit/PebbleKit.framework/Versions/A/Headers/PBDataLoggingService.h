//
//  PBDataLoggingService.h
//  PebbleKit
//
//  Created by Martijn The on 7/19/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <PebbleKit/PebbleKit.h>

/**
 *  Data logging datatypes.
 */
enum {
  //! Array of bytes
  PBDataLoggingTypeByteArray = 0,
  //! unsigned integer
  PBDataLoggingTypeUInt = 2,
  //! signed integer
  PBDataLoggingTypeInt = 3,
};
typedef UInt8 PBDataLoggingType;

/**
 *  The metadata of a data logging session. Used to identify a session.
 */
@interface PBDataLoggingSessionMetadata : NSObject <NSSecureCoding, NSCopying>

/**
 *  A tag associated with the session.
 */
@property (nonatomic, readonly) UInt32 tag;

/**
 *  The timestamp of the moment the session was created by the watchapp.
 */
@property (nonatomic, readonly) UInt32 timestamp;

/**
 *  The type of data stored in this session.
 */
@property (nonatomic, readonly) PBDataLoggingType type;

/**
 *  The size of a data item.
 */
@property (nonatomic, readonly) UInt16 itemSize;

/**
 *  The serial number of the watch that created the sessions.
 */
@property (nonatomic, readonly) NSString *serialNumber;

/**
 *  Creates a new data logging session metadata object, given all its property values.
 *  This method is provided to create a metadata object that can be used to
 *  compare it with a metadata object that is passed into one of the
 *  PBDataLoggingServiceDelegate methods.
 *  @param tag The tag associated with the session
 *  @param timestamp The timestamp of the creation of the session
 *  @param type The type of data stored in the session
 *  @param itemSize The size of a data item
 *  @param serialNumber The serial number of the watch that created the session
 *  @return A session metadata object with the specified information
 */
+ (id)metadataWithTag:(UInt32)tag
            timestamp:(UInt32)timestamp
                 type:(PBDataLoggingType)type
             itemSize:(UInt16)itemSize
         serialNumber:(NSString *)serialNumber;

/**
 *  Tests the equality of two data logging session metadata objects.
 *  This method can be used to compare two session metadata objects
 *  to check whether they are referring to the same session or not.
 *  @param sessionMetadata
 *  @return YES if all the property values of receiver are equal to the property
 *  values of the given session.
 *  @note Equivalent to -[PBDataLoggingSessionMetadata isEqual:]
 */
- (BOOL)isEqualToDataLoggingSessionMetadata:(PBDataLoggingSessionMetadata *)sessionMetadata;
@end

/**
 *  Data logging delegate protocol. The object that implements this protocol
 *  is responsible for handling incoming data.
 */
@protocol PBDataLoggingServiceDelegate <NSObject>
@optional

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param bytes Pointer to the array of bytes.
 *  The array contains (numberOfItems * session.itemSize) bytes.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasByteArrays:(const UInt8 * const)bytes numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param data Pointer to the array of UInt8`s.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasUInt8s:(const UInt8[])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param data Pointer to the array of UInt16`s.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasUInt16s:(const UInt16[])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param data Pointer to the array of UInt32`s.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasUInt32s:(const UInt32[])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param data Pointer to the array of SInt8`s.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasSInt8s:(const SInt8[])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param data Pointer to the array of SInt16`s.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasSInt16s:(const SInt16[])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time there is data available that has not been
 *  consumed yet.
 *  @param service The data logging service.
 *  @param data Pointer to the array of SInt32`s.
 *  @param numberOfItems The number of items that the array contains.
 *  @param session The metadata of the session.
 *  @return YES if the data was consumed and the service can discard the data.
 *  Return NO if the data was not be consumed after the method returned.
 *  If NO is returned, the next time this callback is invoked for the session, the
 *  data argument will (also) contain the data of the items of the previous
 *  invocation of the callback.
 */
- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasSInt32s:(const SInt32[])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)session;

/**
 *  Called by the service every time a session is finished.
 *  This is guaranteed to be called only after all data has been received and
 *  consumed by the application.
 *  @param service The data logging service.
 *  @param session The metadata of the session.
 */
- (void)dataLoggingService:(PBDataLoggingService *)service sessionDidFinish:(PBDataLoggingSessionMetadata *)session;

@end

/**
 *  The data logging service. Assign a delegate object in order to receive
 *  data from your watchapp.
 */
@interface PBDataLoggingService : NSObject

/**
 *  The delegate that has the responsility of handling callbacks from the
 *  data logging service.
 */
@property (atomic, readwrite, weak) id<PBDataLoggingServiceDelegate> delegate;

/**
 *  Sets the queue on which delegate methods will be executed.
 *  Callbacks are intended to be processed in the order as they come in, so it
 *  you must use a serial queue. If set to nil (default) the main queue is used.
 *  @param delegateQueue The queue on which the delegate methods will be executed.
 */
- (void)setDelegateQueue:(dispatch_queue_t)delegateQueue;

/**
 * Query the watch for data logging data.
 */
- (void)pollForData;
  
@end
