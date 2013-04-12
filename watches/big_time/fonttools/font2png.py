#
# Used to create image tiles suitable for use by the "Big Watch" watch
# face.
#
# The meta data for the images produced is output to the terminal for
# copy pasta into `resource_map.json`.
#
#
# This script should be run from the root of the watch project
# directory, like this:
#
#    python fonttools/font2png.py
#
# On Mac OS X you can use it like this to have the meta data copied
# directly to the clipboard, ready for pasting:
#
#     python fonttools/font2png.py | pbcopy
#
#
# These image tiles are needed because Pebble can't handle the 100
# point font size required by the watch. Each image tile is a quarter
# of the display.
#
# The generation of tiles is more complicated than it needs to be due to:
#
#   a) For Nevis (at least) the digits seem to return a fixed height
#      of 115 pixels even though the digits don't take up that much
#      vertical space. This means we can't use `getsize()` for
#      dimensions.
#
#   b) The bottom of the digits seem to get clipped (by PIL) unless
#      the canvas has a bunch of extra space. (This may be related to
#      <http://stackoverflow.com/questions/1933766/fonts-clipping-with-pil>
#      or
#      <http://stackoverflow.com/questions/13821882/pil-cut-off-letters/13831117#13831117>.)
#
# Because of these complications, we have to center the digit
# manually.
#
# We first plot the digit on a large canvas, then crop to just the
# bounds of the character and then paste the result centered on a
# canvas of the correct size.
#

import ImageFont, ImageDraw, Image

FONT_SIZE = 100
FONT_FILE_PATH = "resources/src/fonts/nevis.ttf"

OUTPUT_IMAGE_FILEPATH_TEMPLATE = "resources/src/images/num_%d.png"


TILE_WIDTH_PIXELS = 144/2
TILE_HEIGHT_PIXELS = 168/2

LARGE_SCRATCH_CANVAS_DIMENSIONS = (100, 100)
FINAL_TILE_CANVAS_DIMENSIONS = (TILE_WIDTH_PIXELS, TILE_HEIGHT_PIXELS)

META_DATA_TEMPLATE = \
"""
        {
        "type": "png",
        "defName": "IMAGE_NUM_%d",
        "file": "images/num_%d.png"
        }"""


meta_data_entries = []

font = ImageFont.truetype(FONT_FILE_PATH, FONT_SIZE)


if __name__ == "__main__":
    # Generate the image tile file for each digit.
    for digit in range(0, 10):
        # Draw the digit on a large canvas so PIL doesn't crop it.
        scratch_canvas_image = Image.new("RGB", LARGE_SCRATCH_CANVAS_DIMENSIONS)
        draw = ImageDraw.Draw(scratch_canvas_image)

        draw.text((0,0), str(digit), font=font)

        # Discard all the padding
        cropped_digit_image = scratch_canvas_image.crop(scratch_canvas_image.getbbox())

        # Center the digit within the final image tile and save it
        digit_width, digit_height = cropped_digit_image.size

        tile_image = Image.new("RGB", FINAL_TILE_CANVAS_DIMENSIONS)

        tile_image.paste(cropped_digit_image, ((TILE_WIDTH_PIXELS-digit_width)/2, (TILE_HEIGHT_PIXELS-digit_height)/2))

        tile_image.save(OUTPUT_IMAGE_FILEPATH_TEMPLATE % digit)

        meta_data_entries.append(META_DATA_TEMPLATE % (digit, digit))


    # Display the meta data which needs to be included in `resource_map.json`.
    print ",\n".join(meta_data_entries)




