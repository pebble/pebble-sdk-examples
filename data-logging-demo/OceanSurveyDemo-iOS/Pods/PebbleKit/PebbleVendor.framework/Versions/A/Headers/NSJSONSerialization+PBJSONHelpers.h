//
//  NSJSONSerialization+PBJSONHelpers.h
//  PebbleVendor
//
//  Created by matth on 9/17/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSJSONSerialization (PBJSONHelpers)

/** @name Reflection helpers */

/**
 * Derived from the Box SDK (MIT license) NSJSONSerialization+BoxAdditions...
 * https://github.com/box/box-ios-sdk-v2/tree/master/BoxSDK/Categories
 
 * Ensure that the object at `key` in `dictionary` is a member of the expected class.
 * `NSNull` may be specified as an allowable value. This method may return nil if `key`
 * is not present in `dictionary`.
 *
 * @param key The key to lookup in `dictionary`.
 * @param dictionary A dictionary resulting from deserializing a JSON object.
 * @param cls The expected class of the value at key.
 * @param nullAllowed If true, `NSNull` will be returned. If not, NSNull will be converted to nil. Property getters that pass `YES` for this parameter should have the return type of `id`.
 *
 * @return The object at `key` in `dictionary` if expectations are satisfied; `nil` otherwise.
 */
+ (id)ensureObjectForKey:(NSString *)key inDictionary:(NSDictionary *)dictionary hasExpectedType:(Class)cls nullAllowed:(BOOL)nullAllowed;
@end
