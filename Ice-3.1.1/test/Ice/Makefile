# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

include $(top_srcdir)/config/Make.rules

SUBDIRS		= operations \
		  exceptions \
		  inheritance \
		  facets \
		  objects \
		  faultTolerance \
		  location \
		  adapterDeactivation \
		  slicing \
		  gc \
		  checksum \
		  stream \
		  hold \
		  custom \
		  binding \
		  retry \
		  timeout

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done
