clean:
	rm -rf build/

test: test_array

test_array: test_array_one_proc test_array_simple

test_array_one_proc:
	mkdir -p build/
	mpic++ -std=c++17 -o build/test_array_one_proc tests/array/test_array_one_proc.cpp
	mpiexec -np 1 --mca btl_base_warn_component_unused 0 build/test_array_one_proc

test_array_simple:
	mkdir -p build/
	mpic++ -std=c++17 -o build/test_array_simple tests/array/test_array_simple.cpp
	mpiexec -np 4 --mca btl_base_warn_component_unused 0 build/test_array_simple
