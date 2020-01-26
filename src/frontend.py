#!/usr/bin/python3

from python_tbb import *
import numpy as np

def callback(a) :
	return int(a) * 10

def main() :
	length = 10
	result1 = stdvectorint(np.zeros(length, np.int32))
	result2 = stdvectorint(np.zeros(length, np.int32))
	
	backend_serial(result1, callback)
	backend_tbb(result2, callback)

	for i in range(length) :
		print("%d vs %d" % (result1[i], result2[i]))
	
if __name__ == "__main__" :
	main()

