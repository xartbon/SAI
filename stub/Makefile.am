# Makefile.am -- Process this file with automake to produce Makefile.in

# note that order matters: make the lib first then use it

SUBDIRS = src
DIST_SUBDIRS = src

.PHONY: unit_test lag_unit_test
unit_test: lag_unit_test
lag_unit_test:
	cd src/test && $(MAKE) $(AM_MAKEFLAGS) test
