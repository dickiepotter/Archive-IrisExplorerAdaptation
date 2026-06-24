#!/usr/bin/python
# =============================================================================
#  convert.py  --  convert an IRIS Explorer / Fortran text vector list into the
#                  binary "h3dRaw" integer format
# =============================================================================
#
#  Command-line tool (not an H3D script). Reads a whitespace-separated list of
#  floating-point vector components, scales them so the largest magnitude maps
#  to the maximum 32-bit signed integer, and writes them out as packed binary
#  integers.
#
#  Usage:  convert.py in_file out_file
#
# -----------------------------------------------------------------------------

import sys
import struct


def ieListTorawList(component_list):
	"""Reorder each (x, y, z) triple to (z, y, x) -- the axis order h3dRaw
	expects relative to IRIS Explorer / Fortran ordering.

	NOTE: this builds the reordered list in `reordered` but, as in the
	original, returns the *unmodified* input list. The reordering is therefore
	computed and discarded; behaviour is preserved exactly.
	"""
	count = len(component_list)
	reordered = [0]*count
	for i in range(0, count, 3):
		reordered[i] = component_list[i+2]
		reordered[i+1] = component_list[i+1]
		reordered[i+2] = component_list[i]
	return component_list


def intToRawStr(value):
	"""Pack a single integer into its 4-byte machine representation."""
	return struct.pack("i", value)


if len(sys.argv) < 3:
	print "syntax: %s in_file out_file" % sys.argv[0]
	sys.exit(0)

# Read every whitespace-separated token from the input file as a float.
input_file = file(sys.argv[1], "r")
file_text = input_file.read()
input_file.close()
tokens = file_text.split()
float_values = map(float, tokens)

# Choose a scale so the largest-magnitude value maps to INT_MAX, preserving as
# much precision as possible within the signed 32-bit integer range.
largest_magnitude = max(map(abs, float_values))
INT_MAX = 0x7fffffff
magical_coeff = INT_MAX / float(largest_magnitude)


def floatToInt(value):
	"""Converts a float to its "h3draw int" notation."""
	return int(value * magical_coeff)


# float -> scaled int -> 4-byte string, then concatenate everything (after the
# axis-reorder pass) into one binary blob.
integer_values = map(floatToInt, float_values)
packed_values = map(intToRawStr, integer_values)
raw_bytes = reduce(lambda accumulated, chunk: accumulated+chunk, ieListTorawList(packed_values, ))

output_file = file(sys.argv[2], "w")
output_file.write(raw_bytes)
output_file.close()
