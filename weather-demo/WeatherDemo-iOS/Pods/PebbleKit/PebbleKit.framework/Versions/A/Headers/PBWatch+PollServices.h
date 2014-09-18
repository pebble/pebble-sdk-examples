//
//  PBWatch+PollServices.h
//  PebblePrivateKit
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#import <PebbleKit/PebbleKit.h>

/**
 * PollServiceType
 */ 


enum {
  PBPollRemoteServiceMail = 0,
  PBPollRemoteServiceDataLogging = 1,
  PBNumPollRemoteServices
};
typedef UInt8 PBPollRemoteServiceType;

@interface PBWatch (PollServices)
// Call this upon connect with the watch to add handlers:
- (id)addPollServicesHandler:(void(^)(PBWatch *))handler forService:(PBPollRemoteServiceType)service interval:(UInt8)interval;
- (void)sendPollRequestForService:(PBPollRemoteServiceType)service;
@end
