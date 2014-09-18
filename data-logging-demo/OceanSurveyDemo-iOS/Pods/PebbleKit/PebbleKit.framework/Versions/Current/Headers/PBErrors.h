//
//  PBErrors.h
//  PebbleSDK
//
//  Created by Martijn The on 7/26/12.
//  Copyright (c) 2012 Pebble Technology. All rights reserved.
//

#import <Foundation/Foundation.h>

extern NSString *PBErrorDomain;

/**
 *  PebbleKit Error codes.
 *  Inspect the localizedDescription of any NSError objects for a description.
 */

typedef enum {
  PBErrorCodeNoSupportedProtocolsInInfoPlist,
  PBErrorCodeErrorCreatingSession,
  PBErrorCodeOutputStreamError,
  PBErrorCodeInputStreamError,
  PBErrorCodeSendMessageTimeout,
  PBErrorCodeEndpointDisallowed,
  PBErrorCodeVersionInfoTimeout,
  PBErrorCodeCannotCreateSessionWhileClosing,
  PBErrorCodeNoAppUUID,
  PBErrorCodeAppMessageRejected,
  PBErrorCodeAppMessageAckTimeout,
  PBErrorCodeDictionaryUnsupportedKeyClass,
  PBErrorCodeDictionaryUnsupportedValueClass,
  PBErrorCodeDictionaryInternalConsistency,
  PBErrorCodeInvalidArguments,
  PBErrorCodePebbleAppUnreachable,
  PBErrorCodePebbleAppRequestTimeout,
  PBErrorCodePebbleAppEmptyResponse,
  PBErrorCodePebbleAppUnexpectedResponse,
  PBErrorCodeServerNotAvailable,
  PBErrorCodeDownloadFailed,
  PBErrorCodePebbleNotConnected
} PBErrorCode;

@interface NSError (Pebble)

/**
 *  Convenience method to create an NSError object with <PBErrorDomain> error domain
 *  and a given PBErrorCode and with an underlying NSError object.
 *  @param code The error code for which to create the error
 *  @param error The underlying error
 */
+ (NSError*)pebbleErrorWithCode:(PBErrorCode)code underLyingError:(NSError*)error;

/**
 *  Convenience method to create an NSError object with <PBErrorDomain> error domain
 *  and a given PBErrorCode.
 *  @param code The error code for which to create the error
 */
+ (NSError*)pebbleErrorWithCode:(PBErrorCode)code;

@end