# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..

!include $(top_srcdir)/config/Make.rules.mak

SUBDIRS		= ca

install::
	@if not exist $(prefix)\config \
	    @echo "Creating $(prefix)\config..." & \
	    mkdir $(prefix)\config
	copy Make.rules.mak $(prefix)\config
	copy Make.rules.bcc $(prefix)\config
	copy templates.xml $(prefix)\config
	copy convertssl.py $(prefix)\config
	copy upgradeicegrid.py $(prefix)\config

$(EVERYTHING)::
	@for %i in ( $(SUBDIRS) ) do \
	    @echo "making $@ in %i" & \
	    cmd /c "cd %i & $(MAKE) -nologo -f Makefile.mak $@" || exit 1
