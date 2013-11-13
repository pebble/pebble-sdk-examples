//
//  SDAppViewController.m
//  SportsDemo
//
//  Created by Martijn The on 2/7/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "SDAppViewController.h"

@interface SDAppViewController ()

@end

@implementation SDAppViewController {
  PBWatch *_watch;
  __weak IBOutlet UITextView *_logTextView;
}

- (void)logIfNoWatch {
  if (_watch == nil) {
    _logTextView.text = [_logTextView.text stringByAppendingString:@"No Pebble connected!\n"];
  }
}

- (IBAction)launchAction:(id)sender {
  [self logIfNoWatch];

  [_watch sportsAppLaunch:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingString:@"Failed sending launch command.\n"];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingString:@"Launch command sent.\n"];
    }
  }];
}

- (IBAction)killAction:(id)sender {
  [self logIfNoWatch];

  [_watch sportsAppKill:^(PBWatch *watch, NSError *error) {
    if (error) {
      _logTextView.text = [_logTextView.text stringByAppendingString:@"Failed sending kill command.\n"];
    } else {
      _logTextView.text = [_logTextView.text stringByAppendingString:@"Kill command sent.\n"];
    }
  }];

}

- (void)setTargetWatch:(PBWatch*)watch {
  _watch = watch;
  self.navigationItem.prompt = _watch ? [_watch name] : @"No Pebble";
}

- (void)closeSession {
  [_watch closeSession:^{
    NSLog(@"Session closed.");
  }];
}

- (void)dealloc {
  [self setTargetWatch:nil];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self) {
    self.navigationItem.title = @"Sports App";
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Close Session" style:UIBarButtonItemStylePlain target:self action:@selector(closeSession)];
  }
  return self;
}
							
@end
