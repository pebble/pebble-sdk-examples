//
//  NSJSONSerialization+ObjectWithNString.h
//  Ejecta
//
//  Created by matth on 7/19/13.
//
//

#import <Foundation/Foundation.h>

@interface NSJSONSerialization (ObjectWithNString)
+ (id)JSONObjectWithString:(NSString *)string options:(NSJSONReadingOptions)opt error:(NSError **)error;
+ (NSString*)stringWithNSDictionary:(NSDictionary*)dictionary;
@end
