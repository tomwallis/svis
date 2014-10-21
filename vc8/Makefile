# Space Variant Imaging
#
# Copyright (C) 2006
# Center for Perceptual Systems
# University of Texas at Austin
#
# jsp Sun Aug 20 18:25:46 CDT 2006

PROJECT=vc8.sln

.PHONY: all clean check benchmark

all:
	devenv $(PROJECT) /build debug
	devenv $(PROJECT) /build release

clean:
	devenv $(PROJECT) /clean debug
	devenv $(PROJECT) /clean release

check: all
	cd .. && ./vc8/debug/test_ecc.exe
	cd .. && ./vc8/release/test_ecc.exe
	cd .. && ./vc8/debug/test_filter.exe
	cd .. && ./vc8/release/test_filter.exe
	cd .. && ./vc8/debug/test_foveate.exe
	cd .. && ./vc8/release/test_foveate.exe
	cd .. && ./vc8/debug/test_mask.exe
	cd .. && ./vc8/release/test_mask.exe
	cd .. && ./vc8/debug/test_region.exe
	cd .. && ./vc8/release/test_region.exe
	cd .. && ./vc8/debug/test_svis.exe
	cd .. && ./vc8/release/test_svis.exe

benchmark: all
	./debug/benchmark_ecc.exe
	./release/benchmark_ecc.exe
	./debug/benchmark_filter.exe
	./release/benchmark_filter.exe
	./debug/benchmark_foveate.exe
	./release/benchmark_foveate.exe
	./debug/benchmark_mask.exe
	./release/benchmark_mask.exe
	./debug/benchmark_region.exe
	./release/benchmark_region.exe
	./debug/benchmark_svis.exe
	./release/benchmark_svis.exe
