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

font_width = 3
font_height = 5

bits = {}

im = Image.open(sys.argv[1]).convert("RGBA")
char_count = int(im.width / (font_width + 1))
bit_count = font_width * font_height * char_count
byte_count = int((bit_count + 7) / 8)

print("Building %dx%d font of %d characters" % (font_width, font_height, char_count))
print("-> %d bits (%d bytes)" % (bit_count, byte_count))

for char_id in range(0, char_count):
	for iy in range(0, font_height):
		for ix in range(0, font_width):
			pxl = im.getpixel((ix + (char_id * (font_width + 1)), iy))
			bits[font_width*char_count*iy + font_width*char_id + ix] = (pxl[0] > 128)

with open(sys.argv[2], "wb") as fp:
	for i in range(0, byte_count):
		v = 0
		for ib in range(0, 8):
			idx = i * 8 + ib
			if idx in bits and bits[idx] == True:
				v = v | (1 << ib)
		fp.write(struct.pack("<B", v))
