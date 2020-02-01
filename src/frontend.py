#!/usr/bin/python3

from python_tbb import *
import numpy as np

def callback(a, i, result) :
	result.__setitem__(i, int(a) * 10)

def main() :
	length = 1000
	
	result1 = backend_serial(length, callback)
	result2 = backend_tbb(length, callback)

	for i in range(length) :
		print("%d vs %d" % (result1[i], result2[i]))
	
if __name__ == "__main__" :
	main()

