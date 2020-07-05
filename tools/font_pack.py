#!/usr/bin/python3

# Copyright (c) 2020 Adrian Siekierka
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from PIL import Image
import struct, sys

space_width = 3
font_width = 8
font_height = 11

assert font_width == 8
im = Image.open(sys.argv[1]).convert("RGBA")
char_count = int(im.width / font_width)
char_widths = {}

print("Building %dx%d font of %d characters" % (font_width, font_height, char_count))

with open(sys.argv[2], "wb") as fp:
	for char_id in range(0, char_count):
		char_widths[char_id] = 0
		for iy in range(0, font_height):
			v = 0
			for ix in range(0, font_width):
				pxl = im.getpixel((ix + (char_id * font_width), iy))
				if pxl[0] > 128:
					v = v | (1 << ix)
					if (ix + 1) > char_widths[char_id]:
						char_widths[char_id] = ix + 1
			fp.write(struct.pack("<B", v))
		if char_widths[char_id] == 0:
			char_widths[char_id] = space_width

print("unsigned char charwidths_%dx%d[] = {%s};" % (font_width, font_height, ",".join(str(char_widths[i]) for i in range(0, char_count))));
