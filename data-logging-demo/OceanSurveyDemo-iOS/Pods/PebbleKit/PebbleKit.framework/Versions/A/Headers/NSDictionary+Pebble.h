//
//  NSDictionary+Pebble.h
//  PebbleKit
//
//  Created by Martijn The on 3/20/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSDictionary (Pebble)

/**
 *  Serializes the receiver into a Pebble dict.
 *  @note The receiver dictionary must comply to the following constraints:
 *  * The keys must be instances of NSNumber. Their -unsignedLongValue return value will be used as final key.
 *  * The values must be instances of either NSString, NSData or NSNumber. Use the NSNumber (stdint) category
 *  to specify the signedness and width of NSNumbers.
 *  @param error[out] Pointer to an NSError that will be set after the method has returned
 *  in case there was an error. Possible error codes: PBErrorCodeDictionaryUnsupportedKeyClass,
 *  PBErrorCodeDictionaryUnsupportedValueClass or PBErrorCodeDictionaryUnsupportedValueClass.
 *  @see -dictionaryFromPebbleDictionaryDataWithError:
 */
- (NSData*)pebbleDictionaryData:(NSError * __autoreleasing *)error;

@end

@interface NSData (Pebble)

/**
 *  Interprets the receiver as Pebble dict data and deserializes it into an NSDictionary.
 *  @param error[out] Pointer to an NSError that will be set after the method has returned
 *  in case there was an error. Possible error codes: PBErrorCodeDictionaryInternalConsistency.
 *  @see -pebbleDictionaryData:
 */
- (NSDictionary*)dictionaryFromPebbleDictionaryDataWithError:(NSError * __autoreleasing *)error;

@end