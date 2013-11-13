//
//  GDIconViewController.m
//  GolfDemo
//
//  Created by Martijn The on 3/24/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "GDIconViewController.h"

@interface GDIconViewController () <UITextFieldDelegate>

@end

@implementation GDIconViewController {
  PBWatch *_watch;
  __weak IBOutlet UITextField *_titleTextField;
  __weak IBOutlet UISegmentedControl *_iconSegmentedControl;
  __weak IBOutlet UITextView *_logTextView;
}

- (void)setTargetWatch:(PBWatch*)watch {
  _watch = watch;
  self.navigationItem.prompt = _watch ? [_watch name] : @"No Pebble";
}

- (void)dealloc {
  [self setTargetWatch:nil];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
  if (self) {
    self.navigationItem.title = @"Golf App";
  }
  return self;
}

- (IBAction)assignAction:(id)sender {
  NSString *fileName = (_iconSegmentedControl.selectedSegmentIndex == 0) ? @"balltee.png" : @"green.png";
  UIImage *icon = [UIImage imageNamed:fileName];
  __weak GDIconViewController *weakSelf = self;
  [_watch golfSetTitle:_titleTextField.text icon:icon onSent:^(PBWatch *watch, NSError *error) {
    GDIconViewController *strongSelf = weakSelf;
    NSString *log = error ? [error description] : @"Icon + Title set!";
    [strongSelf->_logTextView setText:[strongSelf->_logTextView.text stringByAppendingFormat:@"\n%@", log]];
  }];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
  [textField resignFirstResponder];
  return YES;
}

- (void)viewDidUnload {
  _titleTextField = nil;
  _iconSegmentedControl = nil;
  _logTextView = nil;
  [super viewDidUnload];
}
@end
