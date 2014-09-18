//
//  NSString+HexData.h
//  PebblePrivateKit
//
//  Created by Martijn The on 11/19/12.
//  Copyright (c) 2012 Pebble Technology. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (HexData)

- (NSData*)hexData;

@end


@interface NSData (HexString)

- (NSString*)hexString;

@end