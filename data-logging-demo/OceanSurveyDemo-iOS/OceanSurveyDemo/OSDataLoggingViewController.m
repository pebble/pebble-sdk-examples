//
//  OSDataLoggingViewController.m
//
//  Created by Martijn The on 8/13/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "OSDataLoggingViewController.h"

enum {
  OSAnimalSealion = 0x5,
  OSAnimalPelican = 0xb,
  OSAnimalDolphin = 0xd,
};

@interface OSDataLoggingViewController () <PBDataLoggingServiceDelegate>

@end

@implementation OSDataLoggingViewController {
  UITextView *_textView;
  NSDateFormatter *_dateFormatter;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self) {
    self.navigationItem.title = @"Ocean Survey";
    [[[PBPebbleCentral defaultCentral] dataLoggingService] setDelegate:self];

    _dateFormatter = [[NSDateFormatter alloc] init];
    _dateFormatter.timeZone = [NSTimeZone localTimeZone];
    _dateFormatter.dateStyle = NSDateFormatterShortStyle;
    _dateFormatter.timeStyle = NSDateFormatterShortStyle;
  }
  return self;
}

- (void)addLogLine:(NSString*)line {
  line = [line stringByAppendingString:@"\n"];
  _textView.text = [_textView.text stringByAppendingString:line];
}

- (void)viewDidLoad {
  [super viewDidLoad];
  _textView = [[UITextView alloc] initWithFrame:self.view.bounds];
  _textView.text = @"Log:\n-------------------\n";
  _textView.editable = NO;
  _textView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
  _textView.autoresizesSubviews = YES;
  [self.view addSubview:_textView];
}

- (NSString*)animalStringWithTag:(UInt32)tag {
  switch (tag) {
    case OSAnimalSealion: return @"Sealion";
    case OSAnimalPelican: return @"Pelican";
    case OSAnimalDolphin: return @"Dolphin";
  }
  return @"Unknown";
}

- (BOOL)dataLoggingService:(PBDataLoggingService *)service hasUInt32s:(const UInt32 [])data numberOfItems:(UInt16)numberOfItems forDataLoggingSession:(PBDataLoggingSessionMetadata *)sessionMetadata{
  for (int i = 0; i < numberOfItems; ++i) {
    NSString *animal = [self animalStringWithTag:sessionMetadata.tag];
    const UInt32 loggedTime = data[i];

    /* Add seconds to GMT, since the watch only keeps local time. */
    NSTimeZone *timeZone = [NSTimeZone localTimeZone];
    NSInteger secondsOffset = [timeZone secondsFromGMT];
    NSTimeInterval secondsSince1970 = loggedTime - secondsOffset;
    NSDate *date = [NSDate dateWithTimeIntervalSince1970:secondsSince1970];

    NSString *log = [NSString stringWithFormat:@"%@: %@", animal, [_dateFormatter stringFromDate:date]];
    [self addLogLine:log];
  }
  // We consumed the data, let the data logging service know:
  return YES;
}

- (void)dataLoggingService:(PBDataLoggingService *)service sessionDidFinish:(PBDataLoggingSessionMetadata *)sessionMetadata {
  NSString *animal = [self animalStringWithTag:sessionMetadata.tag];
  NSString *log = [NSString stringWithFormat:@"%@ session closed.", animal];
  
  [self addLogLine:log];
}

@end
