//
//  PBWatch+Version.h
//  PebbleKit
//
//  Created by Martijn The on 9/17/12.
//  Copyright (c) 2012 Pebble Technology. All rights reserved.
//

#import <PebbleKit/PebbleKit.h>

/**
 *  PBFirmwareVersion is a helper class that is able to parse the versionTag string
 *  into its components (os, major, minor, suffix) and facilitate comparisons
 *  between versions.
 *  Valid versions follow the following format:
 *  `vOS(.MAJOR(.MINOR(-SUFFIX)))`
 *  Where the fields OS, MAJOR, MINOR are positive, decimal integer numbers and SUFFIX
 *  an arbitrary string. MAJOR, MINOR and SUFFIX are optional (nested optionality).
 *  Examples of valid versions:
 *  `v1`
 *  `v2.1234.99-alpha-beta-gamma`
 */
@interface PBFirmwareVersion : NSObject
/**
 *  The OS version component.
 */
@property (nonatomic, readonly) NSInteger os;

/**
 *  The major version component.
 *  @note If a version tag string does not contain a major component, it will be set to 0.
 */
@property (nonatomic, readonly) NSInteger major;

/**
 *  The minor version component.
 *  @note If a version tag string does not contain a major component, it will be set to 0.
 */
@property (nonatomic, readonly) NSInteger minor;

/**
 *  The release suffix, which is the part following the dash '-'.
 *  @note If a version tag string does not contain a suffix, it will be set to an empty string.
 *  @discussion The release suffix is not used in the -compare: method.
 */
@property (nonatomic, readonly, copy) NSString *suffix;

/**
 *  The version timestamp.
 *  @note The timestamp is in the final comparison equation, in the -compare: method, after evaluating
 *  os, major and minor components.
 */
@property (nonatomic, readonly) UInt32 timestamp;

/**
 *  The version's git commit hash.
 *  @discussion The commit hash is not used in the -compare: method.
 */
@property (nonatomic, readonly, copy) NSString *commitHash;

/**
 *  The version's git tag.
 */
@property (nonatomic, readonly, copy) NSString *tag;

/**
 *  Creates a PBFirmwareVersion object given a tag string and timestamp.
 */
+ (PBFirmwareVersion*)firmwareVersionWithTag:(NSString*)tag commitHash:(NSString*)commitHash timestamp:(UInt32)timestamp;

/**
 *  Creates a PBFirmwareVersion object given its components.
 *  @discussion The tag string will be set to the canonical format vOS.MINOR.MAJOR(-SUFFIX).
 */
+ (PBFirmwareVersion*)firmwareVersionWithOS:(NSInteger)os major:(NSInteger)major minor:(NSInteger)minor suffix:(NSString*)suffix commitHash:(NSString*)commitHash timestamp:(UInt32)timestamp;

/**
 *  Compares the receiver to another version object.
 *  @param aVersion The version to compare with the receiver.
 *  @return NSOrderedAscending if the value of aVersion is greater than the receiver’s,
 *  NSOrderedSame if they’re equal, and NSOrderedDescending if the value of aVersion is less than the receiver’s.
 */
- (NSComparisonResult)compare:(PBFirmwareVersion *)aVersion;

/**
 *  Convenience wrappers around -compare:
 */
- (BOOL)isEqualOrNewer:(PBFirmwareVersion *)other;
- (BOOL)isNewer:(PBFirmwareVersion *)other;
- (BOOL)isEqualVersionOnly:(PBFirmwareVersion *)other;

@end



/**
 *  Values specifying the Pebble hardware platform variant.
 */
typedef enum {
  FirmwareMetadataPlatformUnknown = 0,
  FirmwareMetadataPlatformPebbleOneEV1 = 1,
  FirmwareMetadataPlatformPebbleOneEV2 = 2,
  FirmwareMetadataPlatformPebbleOneEV2_3 = 3,
  FirmwareMetadataPlatformPebbleOneEV2_4 = 4,
  FirmwareMetadataPlatformPebbleOnePointFive = 5,
  FirmwareMetadataPlatformPebbleTwoPointZero = 6,
  FirmwareMetadataPlatformPebbleOneBigboard = 0xff,
  FirmwareMetadataPlatformPebbleOneBigboard2 = 0xfe,
} FirmwareMetadataPlatform;

/**
 *  Instances of PBFirmwareMetadata describe a firmware in terms of
 *  its version, whether it is a recovery firmware and its intended hardware
 *  platform.
 *  @see PBVersionInfo
 */

@interface PBFirmwareMetadata : NSObject

/**
 *  The version of the firmware.
 *  @see PBFirmwareVersion
 */
@property (nonatomic, readonly) PBFirmwareVersion *version;

/**
 *  YES if the firmware is a recovery firmware, NO if it is a regular firmware.
 */
@property (nonatomic, readonly) BOOL isRecoveryFirmware;

/**
 *  The hardware platform variant with which the firmware is compatible.
 */
@property (nonatomic, readonly) FirmwareMetadataPlatform hardwarePlatform;

/**
 *  Converts a <FirmwareMetadataPlatform> value to a string.
 *  @param hardwarePlatform The hardware platform value to convert
 *  @return A string of the hardware platform
 */
+ (NSString*)hardwarePlatformToString:(FirmwareMetadataPlatform)hardwarePlatform;

/**
 *  Converts an NSString hardware platform string to the corresponding
 *  FirmwareMetadataPlatform value.
 *  @param hardwarePlatformString The string representing the hardware platform.
 *  @return The hardware platform enum value
 */
+ (FirmwareMetadataPlatform)stringToHardwarePlatform:(NSString *)hardwarePlatformString;

@end

/**
 *  Instances of PBResourceMetadata describe (system) resources.
 *  @see PBVersionInfo
 */

@interface PBResourceMetadata : NSObject

/**
 *  The version of the resources as the Epoch timestamp of the Git commit.
 */
@property (nonatomic, readonly) UInt32 timestamp;

/**
 *  The checksum of the resources.
 */
@property (nonatomic, readonly) UInt32 crc;

/**
 *  The human-friendly string version of the resources.
 */
@property (nonatomic, readonly, strong) NSString *friendlyVersion;

/**
 *  YES if the metadata represents valid resources, NO if it does not.
 */
- (BOOL)isValid;

@end

/**
 *  An instance of PBVersionInfo describes all the different system software (Pebble OS)
 *  on a Pebble watch and other auxiliary information.
 *  @see PBWatch
 */

@interface PBVersionInfo : NSObject

/**
 *  The metadata of the firmware that is running.
 */
@property (nonatomic, readonly, strong) PBFirmwareMetadata *runningFirmwareMetadata;

/**
 *  The metadata of the recovery firmware that is in storage.
 */
@property (nonatomic, readonly, strong) PBFirmwareMetadata *recoveryFirmwareMetadata;

/**
 *  The metadata of the system resources.
 */
@property (nonatomic, readonly, strong) PBResourceMetadata *systemResources;

/**
 *  The version of the bootloader.
 */
@property (nonatomic, readonly) UInt32 bootloaderVersion;

/**
 *  The version of the hardware.
 */
@property (nonatomic, readonly, strong) NSString *hardwareVersion;

/**
 *  The serial number.
 */
@property (nonatomic, readonly, strong) NSString *serialNumber;

/**
 *  The current Bluetooth device address.
 */
@property (nonatomic, readonly, strong) NSData *deviceAddress;

/**
 *  YES if valid recovery firmware is installed, NO if not.
 */
- (BOOL)hasRecoveryFirmware;

/**
 *  YES if valid system resources are installed, NO if not.
 */
- (BOOL)hasSystemResources;

@end

@interface PBWatch (Version)

/**
 *  Requests the version info from the receiver.
 *  @param handler The handler that will be called when the version info response has been received.
 *  @param onTimeout The handler that will be called if the watch did not respond within 5 seconds.
 *  @param watch The watch for which the version info was requested.
 *  @param versionInfo The version info response.
 *  @discussion The <PBVersionInfo> response will be cached. The latest version info can be
 *  accessed through the <versionInfo> property of <PBWatch>.
 */
- (void)getVersionInfo:(void (^)(PBWatch *watch, PBVersionInfo *versionInfo))handler onTimeout:(void (^)(PBWatch *watch))onTimeout;

@end
