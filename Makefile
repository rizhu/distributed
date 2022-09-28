clean:
	rm -rf build/

test: test_array

test_array: test_array_one_proc

test_array_one_proc:
	mkdir -p build/
	mpic++ -std=c++17 -o build/test_array_one_proc.out tests/array/test_array_one_proc.cpp
	mpiexec -np 1 build/test_array_one_proc.out
