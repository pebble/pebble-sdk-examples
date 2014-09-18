//
//  gtypes.h
//  PebbleKit
//
//  Created by Martijn The on 3/24/13.
//  Copyright (c) 2013 Pebble Technology. All rights reserved.
//

#ifndef PebbleKit_gtypes_h
#define PebbleKit_gtypes_h

typedef struct GSize {
  int16_t w;
  int16_t h;
} GSize;

typedef struct GPoint {
  int16_t x;
  int16_t y;
} GPoint;

typedef struct GRect {
  GPoint origin;
  GSize size;
} GRect;

#endif
