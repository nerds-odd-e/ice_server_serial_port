# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

!include $(top_srcdir)/config/Make.rules.mak

install::
	@if not exist $(install_includedir)\Ice \
	    @echo "Creating $(install_includedir)\Ice..." & \
	    mkdir $(install_includedir)\Ice

	@for %i in ( *.h ) do \
	    @echo Installing %i & \
	    copy %i $(install_includedir)\Ice
