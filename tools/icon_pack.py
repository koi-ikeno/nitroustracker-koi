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
bit_count = im.width * im.height
byte_count = int((bit_count + 7) / 8)

print("Converting %dx%d image to %d bits (%d bytes)" % (im.width, im.height, bit_count, byte_count))

for iy in range(0, im.height):
	for ix in range(0, im.width):
		pxl = im.getpixel((ix, iy))
		bits[im.width*iy + ix] = (pxl[0] < 128)

with open(sys.argv[2], "wb") as fp:
	for i in range(0, byte_count):
		v = 0
		for ib in range(0, 8):
			idx = i * 8 + ib
			if idx in bits and bits[idx] == True:
				v = v | (1 << ib)
		fp.write(struct.pack("<B", v))
