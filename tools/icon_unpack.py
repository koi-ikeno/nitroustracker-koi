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

out_width = int(sys.argv[3])
out_height = int(sys.argv[4])

im = Image.new("RGBA", (out_width, out_height))

ix = 0
iy = 0

with open(sys.argv[1], "rb") as fp:
	for b in fp.read():
		for i in range(0, 8):
			if iy < out_height:
				col = 255 if (b & 1) == 0 else 0
				im.putpixel((ix, iy), (col, col, col, 255))
			b = b >> 1
			ix = ix + 1
			if ix >= out_width:
				ix = 0
				iy = iy + 1

im.save(sys.argv[2])
