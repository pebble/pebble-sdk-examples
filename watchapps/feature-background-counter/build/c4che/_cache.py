AR = 'arm-none-eabi-ar'
ARFLAGS = 'rcs'
AS = 'arm-none-eabi-gcc'
BINDIR = '/usr/local/bin'
CC = ['arm-none-eabi-gcc']
CCLNK_SRC_F = []
CCLNK_TGT_F = ['-o']
CC_NAME = 'gcc'
CC_SRC_F = []
CC_TGT_F = ['-c', '-o']
CC_VERSION = ('4', '8', '4')
CFLAGS = ['-std=c99', '-mcpu=cortex-m3', '-mthumb', '-ffunction-sections', '-fdata-sections', '-g', '-Os', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', '-Wno-error=unused-function', '-Wno-error=unused-variable']
CFLAGS_MACBUNDLE = ['-fPIC']
CFLAGS_cshlib = ['-fPIC']
CPPPATH_ST = '-I%s'
DEFINES = ['RELEASE']
DEFINES_ST = '-D%s'
DEST_BINFMT = 'elf'
DEST_CPU = 'arm'
DEST_OS = 'darwin'
LD = 'arm-none-eabi-ld'
LIBDIR = '/usr/local/lib'
LIBPATH_ST = '-L%s'
LIB_ST = '-l%s'
LINKFLAGS = ['-mcpu=cortex-m3', '-mthumb', '-Wl,--gc-sections', '-Wl,--warn-common', '-Os']
LINKFLAGS_MACBUNDLE = ['-bundle', '-undefined', 'dynamic_lookup']
LINKFLAGS_cshlib = ['-shared']
LINKFLAGS_cstlib = ['-Wl,-Bstatic']
LINK_CC = ['arm-none-eabi-gcc']
PEBBLE_SDK = '/Users/cherie/pebble-dev/PebbleSDK-2.6/Pebble'
PREFIX = '/usr/local'
RPATH_ST = '-Wl,-rpath,%s'
SHLIB_MARKER = None
SIZE = 'arm-none-eabi-size'
SONAME_ST = '-Wl,-h,%s'
STLIBPATH_ST = '-L%s'
STLIB_MARKER = None
STLIB_ST = '-l%s'
cprogram_PATTERN = '%s'
cshlib_PATTERN = 'lib%s.so'
cstlib_PATTERN = 'lib%s.a'
macbundle_PATTERN = '%s.bundle'
