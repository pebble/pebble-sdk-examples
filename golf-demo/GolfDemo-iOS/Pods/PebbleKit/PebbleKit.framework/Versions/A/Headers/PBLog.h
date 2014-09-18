//
//  PBLog.h
//  PebbleApp
//
//  Created by Martijn The on 11/29/12.
//  Copyright (c) 2012 Pebble Technology. All rights reserved.
//

#import "DDLog.h"

#ifndef PBWSLOG_CONTEXT
#define PBWSLOG_CONTEXT 1
#endif

extern int PBLogLevel;

#define LogError(frmt, ...)   LOG_OBJC_MAYBE(LOG_ASYNC_ERROR,   PBLogLevel, LOG_FLAG_ERROR,   0, frmt, ##__VA_ARGS__)
#define LogWarn(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_WARN,    PBLogLevel, LOG_FLAG_WARN,    0, frmt, ##__VA_ARGS__)
#define LogInfo(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_INFO,    PBLogLevel, LOG_FLAG_INFO,    0, frmt, ##__VA_ARGS__)
#define LogVerbose(frmt, ...) LOG_OBJC_MAYBE(LOG_ASYNC_VERBOSE, PBLogLevel, LOG_FLAG_VERBOSE, 0, frmt, ##__VA_ARGS__)

#define WSLogError(frmt, ...)   LOG_OBJC_MAYBE(LOG_ASYNC_ERROR,   PBLogLevel, LOG_FLAG_ERROR,   PBWSLOG_CONTEXT, frmt, ##__VA_ARGS__)
#define WSLogWarn(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_WARN,    PBLogLevel, LOG_FLAG_WARN,    PBWSLOG_CONTEXT, frmt, ##__VA_ARGS__)
#define WSLogInfo(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_INFO,    PBLogLevel, LOG_FLAG_INFO,    PBWSLOG_CONTEXT, frmt, ##__VA_ARGS__)
#define WSLogVerbose(frmt, ...) LOG_OBJC_MAYBE(LOG_ASYNC_VERBOSE, PBLogLevel, LOG_FLAG_VERBOSE, PBWSLOG_CONTEXT, frmt, ##__VA_ARGS__)

#define LogCError(frmt, ...)   LOG_C_MAYBE(LOG_ASYNC_ERROR,   PBLogLevel, LOG_FLAG_ERROR,   0, frmt, ##__VA_ARGS__)
#define LogCWarn(frmt, ...)    LOG_C_MAYBE(LOG_ASYNC_WARN,    PBLogLevel, LOG_FLAG_WARN,    0, frmt, ##__VA_ARGS__)
#define LogCInfo(frmt, ...)    LOG_C_MAYBE(LOG_ASYNC_INFO,    PBLogLevel, LOG_FLAG_INFO,    0, frmt, ##__VA_ARGS__)
#define LogCVerbose(frmt, ...) LOG_C_MAYBE(LOG_ASYNC_VERBOSE, PBLogLevel, LOG_FLAG_VERBOSE, 0, frmt, ##__VA_ARGS__)
