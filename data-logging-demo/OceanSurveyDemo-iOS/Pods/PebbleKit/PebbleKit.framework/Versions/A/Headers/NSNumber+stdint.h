//
//  NSNumber+stdint.h
//  PebbleKit
//
//  Created by Martijn The on 3/20/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *  This category exposes the capabilities the underlying CFNumber to explicitely
 *  store the signedness and the width of the storage.
 */

@interface NSNumber (stdint)

/**
 *  Interprets the receiver as a 32-bits wide, unsigned integer.
 */
- (uint32_t)uint32Value;

/**
 *  Interprets the receiver as a 16-bits wide, unsigned integer.
 */
- (uint16_t)uint16Value;

/**
 *  Interprets the receiver as a 8-bits wide, unsigned integer.
 */
- (uint8_t)uint8Value;

/**
 *  Interprets the receiver as a 32-bits wide, signed integer.
 */
- (int32_t)int32Value;

/**
 *  Interprets the receiver as a 16-bits wide, signed integer.
 */
- (int16_t)int16Value;

/**
 *  Interprets the receiver as a 8-bits wide, signed integer.
 */
- (int8_t)int8Value;

/**
 *  Creates an NSNumber with a 32-bits wide, unsigned integer.
 */
+ (NSNumber *)numberWithUint32:(uint32_t)value;

/**
 *  Creates an NSNumber with a 16-bits wide, unsigned integer.
 */
+ (NSNumber *)numberWithUint16:(uint16_t)value;

/**
 *  Creates an NSNumber with a 8-bits wide, unsigned integer.
 */
+ (NSNumber *)numberWithUint8:(uint8_t)value;

/**
 *  Creates an NSNumber with a 32-bits wide, signed integer.
 */
+ (NSNumber *)numberWithInt32:(int32_t)value;

/**
 *  Creates an NSNumber with a 16-bits wide, signed integer.
 */
+ (NSNumber *)numberWithInt16:(int16_t)value;

/**
 *  Creates an NSNumber with a 8-bits wide, signed integer.
 */
+ (NSNumber *)numberWithInt8:(int8_t)value;

/**
 *  Gets whether the number that is stored by the receiver should be interpreted
 *  as a floating pointer number or not.
 */
- (BOOL)isFloat;

/**
 *  Gets whether the number that is stored by the receiver should be interpreted
 *  as a signed integer or not.
 */
- (BOOL)isSigned;

/**
 *  Gets the width in bytes of the integer that is stored by the receiver.
 */
- (uint8_t)width;

@end
