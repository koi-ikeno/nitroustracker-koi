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

bits = {}

im = Image.open(sys.argv[1]).convert("RGBA")

has_transparency = False

for iy in range(0, im.height):
	for ix in range(0, im.width):
		pxl = im.getpixel((ix, iy))
		if (pxl[0] > 128) and (pxl[1] < 128):
			bits[im.width*iy + ix] = -1
			has_transparency = True
		else:
			bits[im.width*iy + ix] = 1 if (pxl[0] < 128) else 0

bit_count = im.width * im.height * (2 if has_transparency else 1)
byte_count = int((bit_count + 7) / 8)

print("Converting %dx%d image to %d bits (%d bytes)" % (im.width, im.height, bit_count, byte_count))

with open(sys.argv[2], "wb") as fp:
	for i in range(0, byte_count):
		v = 0
		if has_transparency:
			for ib in range(0, 4):
				idx = i * 4 + ib
				if idx in bits:
					v = v | ((bits[idx] + 1) << (ib * 2))
		else:
			for ib in range(0, 8):
				idx = i * 8 + ib
				if idx in bits:
					v = v | (bits[idx] << ib)
		fp.write(struct.pack("<B", v))
