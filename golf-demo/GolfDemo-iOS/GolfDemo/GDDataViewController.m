//
//  GDSecondViewController.m
//  GolfDemo
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "GDDataViewController.h"

enum {
  GDIndexTime = 0,
  GDIndexDistance,
  GDIndexPace,
};

@interface GDDataViewController ()

@end

@implementation GDDataViewController {
  PBWatch *_watch;
  IBOutletCollection(UISlider) NSArray *_textFields;
  __weak IBOutlet UITextView *_logTextView;
  id _opaqueUpdateHandler;
}

- (void)setTargetWatch:(PBWatch*)watch {
  if (_watch && _opaqueUpdateHandler) {
    // Unregister the update handler that was registered below:
    [_watch golfAppRemoveUpdateHandler:_opaqueUpdateHandler];
  }
  _watch = watch;
  if (_watch) {
    // Add the handler for inbound (watch -> phone) updates:
    // For the Golf protocol, there are currently only two commands that can get sent, "previous" and "next":
    __weak GDDataViewController *weakSelf = self;
    _opaqueUpdateHandler = [_watch golfAppAddReceiveUpdateHandler:^BOOL(PBWatch *watch, GolfAppCommand command) {
      NSString *commandString = nil;
      switch (command) {
        case GolfAppCommandNext: commandString = @"Next"; break;
        case GolfAppCommandPrevious: commandString = @"Previous"; break;
        case GolfAppCommandSelect: commandString = @"Select"; break;
        default: commandString = @"Unknown Command"; break;
      }
      GDDataViewController *strongSelf = weakSelf;
      UITextView *logTextView = strongSelf->_logTextView;
      logTextView.text = [logTextView.text stringByAppendingFormat:@"Received command: %@\n", commandString];
      return YES;
    }];
  }
  self.navigationItem.prompt = _watch ? [_watch name] : @"No Pebble";
}

- (void)logIfNoWatch {
  if (_watch == nil) {
    _logTextView.text = [_logTextView.text stringByAppendingString:@"No Pebble connected!\n"];
  }
}

- (IBAction)updateAction:(id)sender {
  [self logIfNoWatch];

  NSUInteger count = _textFields.count;
  NSMutableDictionary *update = [NSMutableDictionary dictionary];
  for (int i = 0; i < count; ++i) {
    UITextField *field = _textFields[i];
    NSNumber *key = nil;
    switch (field.tag) {
      case 0: key = PBGolfFrontKey; break;
      case 1: key = PBGolfMidKey; break;
      case 2: key = PBGolfBackKey; break;
      case 3: key = PBGolfParKey; break;
      case 4: key = PBGolfHoleKey; break;
      default: continue;
    }
    [update setObject:field.text forKey:key];
  }

  NSString *log = [NSString stringWithFormat:@"Sent update: F:%@, M:%@, B:%@, P:%@, H:%@\n",
                   update[PBGolfFrontKey],
                   update[PBGolfMidKey],
                   update[PBGolfBackKey],
                   update[PBGolfParKey],
                   update[PBGolfHoleKey]];
  
  [_watch golfAppUpdate:update onSent:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Failed sending update: %@\n", error];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingString:log];
    }
  }];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self) {
    self.navigationItem.title = @"Golf Data";
  }
  return self;
}

@end
