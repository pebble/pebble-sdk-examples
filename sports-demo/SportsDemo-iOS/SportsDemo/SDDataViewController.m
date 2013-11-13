//
//  SDSecondViewController.m
//  SportsDemo
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "SDDataViewController.h"

enum {
  SDIndexTime = 0,
  SDIndexDistance,
  SDIndexData,
};

@interface SDDataViewController ()

@end

@implementation SDDataViewController {
  PBWatch *_watch;
  IBOutletCollection(UILabel) NSArray *_labels;
  IBOutletCollection(UISlider) NSArray *_sliders;
  __weak IBOutlet UITextView *_logTextView;
  id _opaqueUpdateHandler;
  __weak IBOutlet UISegmentedControl *_activityStateSegmentedControl;
}

- (void)setTargetWatch:(PBWatch*)watch {
  if (_watch && _opaqueUpdateHandler) {
    // Unregister the update handler that was registered below:
    [_watch sportsAppRemoveUpdateHandler:_opaqueUpdateHandler];
  }
  _watch = watch;
  if (_watch) {
    // Add the handler for inbound (watch -> phone) updates:
    // For the Sports protocol, there is currently only one value that can get updated, which is the "activity state".
    __weak SDDataViewController *weakSelf = self;
    _opaqueUpdateHandler = [_watch sportsAppAddReceiveUpdateHandler:^BOOL(PBWatch *watch, SportsAppActivityState state) {
      NSString *newStateString = nil;
      NSInteger index = 0;
      switch (state) {
        case SportsAppActivityStateInit:
          newStateString = @"Init";
          break;
        case SportsAppActivityStateRunning:
          newStateString = @"Running";
          index = 1;
          break;
        case SportsAppActivityStatePaused:
          newStateString = @"Paused";
          break;
        case SportsAppActivityStateEnd:
          newStateString = @"End";
          break;
      }
      SDDataViewController *strongSelf = weakSelf;
      UITextView *logTextView = strongSelf->_logTextView;
      logTextView.text = [logTextView.text stringByAppendingFormat:@"Activity state: %@\n", newStateString];
      UISegmentedControl *segmentedControl = strongSelf->_activityStateSegmentedControl;
      segmentedControl.selectedSegmentIndex = index;
      return YES;
    }];
  }
  self.navigationItem.prompt = _watch ? [_watch name] : @"No Pebble";
}

- (IBAction)sliderAction:(UISlider *)sender {
  NSUInteger index = [_sliders indexOfObject:sender];
  UILabel *label = [_labels objectAtIndex:index];
  switch (index) {
    case SDIndexTime: {
      int trucatedTime = ABS(truncf(sender.value));
      BOOL isNegative = (sender.value < 0.f);
      int seconds = trucatedTime % 60;
      int minutes = (trucatedTime - seconds) / 60;
      label.text = [NSString stringWithFormat:@"Time: %s%i:%02i", isNegative ? "-" : "", minutes, seconds];
      break;
    }
    case SDIndexDistance:
      label.text = [NSString stringWithFormat:@"Distance: %2.1f", sender.value];
      break;
    case SDIndexData:
      label.text = [NSString stringWithFormat:@"Data: %@", [PBSportsUpdate timeStringFromFloat:sender.value]];
      break;
    default: break;
  }
}

- (void)logIfNoWatch {
  if (_watch == nil) {
    _logTextView.text = [_logTextView.text stringByAppendingString:@"No Pebble connected!\n"];
  }
}

- (IBAction)stateAction:(UISegmentedControl *)sender {
  [self logIfNoWatch];
  SportsAppActivityState state;
  switch (sender.selectedSegmentIndex) {
    case 0:
      state = SportsAppActivityStatePaused;
      break;
    case 1:
    default:
      state = SportsAppActivityStateRunning;
      break;
  }
  NSString *log = [NSString stringWithFormat:@"Set activity state: %@\n", state == SportsAppActivityStatePaused ? @"Paused" : @"Running"];
  [_watch sportsAppSetActivityState:state onSent:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Failed sending activity state: %@\n", error];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingString:log];
    }
  }];
}

- (IBAction)updateAction:(id)sender {
  [self logIfNoWatch];

  NSUInteger count = _sliders.count;
  float value[count];
  for (int i = 0; i < count; ++i) {
    value[i] = [(UISlider*)[_sliders objectAtIndex:i] value];
  }

  // Either use the helper PBSportsUpdate class:
  /*
  PBSportsUpdate *update = [PBSportsUpdate updateWithTime:value[SDIndexTime]
                                                 distance:value[SDIndexDistance]
						     data:value[SDIndexData]];
  NSDictionary *updateDict = [update dictionary];
   */

  // Or construct a dictionary with the one or more values you want to update:

  NSDictionary *updateDict = @{ PBSportsTimeKey : [PBSportsUpdate timeStringFromFloat:value[SDIndexTime]],
				PBSportsDataKey : [PBSportsUpdate timeStringFromFloat:value[SDIndexData]],
                                PBSportsDistanceKey : [NSString stringWithFormat:@"%2.02f", value[SDIndexDistance]]};

  NSString *log = [NSString stringWithFormat:@"Sent update: t:%2.2f, d:%2.2f, p:%2.1f\n", value[SDIndexTime], value[SDIndexDistance], value[SDIndexData]];
  [_watch sportsAppUpdate:updateDict onSent:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Failed sending update: %@\n", error];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingString:log];
    }
  }];
}

- (IBAction)unitAction:(UISegmentedControl*)button {
  [self logIfNoWatch];
  
  BOOL isMetric = (button.selectedSegmentIndex == 1);
  [_watch sportsAppSetMetric:isMetric onSent:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Failed setting unit: %@\n", error];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Unit set: %@\n", isMetric ? @"Metric" : @"Imperial"];
    }
  }];
}
- (IBAction)paceAction:(UISegmentedControl*)button {
  [self logIfNoWatch];

  BOOL isPace = (button.selectedSegmentIndex == 0);
  [_watch sportsAppSetLabel:isPace onSent:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Failed setting unit: %@\n", error];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingFormat:@"Label set: %@\n", isPace ? @"PACE" : @"SPEED"];
    }
  }];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self) {
    self.navigationItem.title = @"Sports Data";
  }
  return self;
}

- (void)viewDidUnload {
  _activityStateSegmentedControl = nil;
  [super viewDidUnload];
}
@end
