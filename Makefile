# Space Variant Imaging System
#
# Copyright (C) 2006
# Center for Perceptual Systems
# University of Texas at Austin
#
# jsp Mon Aug 14 10:23:51 CDT 2006

.PHONY: build release check benchmark publish clean

MAJOR_VERSION=`grep MAJOR_VERSION version.h | cut -d' ' -f5 -`
MINOR_VERSION=`grep MINOR_VERSION version.h | cut -d' ' -f5 -`
RELEASE=6

# Pass version number to sub-makes
export VERSION=$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE)

ifeq (Linux,$(shell uname))
MAKEFILE=Makefile.linux
else
ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
MAKEFILE=Makefile.cygwin
else
$(error "Unknown OS")
endif
endif

.PHONY: build check dist publish clean

build: Makefile
	$(MAKE) -r -f $(MAKEFILE) build

release: Makefile
	$(MAKE) -r -f $(MAKEFILE) release

check:
	$(MAKE) -f $(MAKEFILE) check

benchmark:
	$(MAKE) -f $(MAKEFILE) benchmark

dist: clean release
	svn log > CHANGES
	$(MAKE) -f $(MAKEFILE) dist

tmp:
	$(MAKE) -f $(MAKEFILE) dist_src

publish:
	$(MAKE) -f $(MAKEFILE) publish

clean:
	rm -f CHANGES
	rm -Rf examples/avner.tgz.images
	rm -Rf examples/hana.tgz.images
	rm -Rf examples/katharine.tgz.images
	rm -Rf examples/caesar.tgz.images
	rm -f svismex.cpp
	rm -f sviscodec.m
	rm -f svisencode.m
	rm -f svisinit.m
	rm -f svisrelease.m
	rm -f svissetresmap.m
	rm -f svissetsrc.m
	rm -f tmp_*.pgm
	$(MAKE) -f $(MAKEFILE) clean
