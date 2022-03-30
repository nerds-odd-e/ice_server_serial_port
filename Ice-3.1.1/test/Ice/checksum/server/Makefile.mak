# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..\..

SERVER		= server.exe

TARGETS		= $(SERVER)

SOBJS		= Test.obj \
		  Types.obj \
		  TestI.obj \
		  Server.obj

SRCS		= $(SOBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. -I../../../include $(CPPFLAGS)
SLICE2CPPFLAGS	= --checksum $(SLICE2CPPFLAGS)

$(SERVER): $(SOBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(SOBJS), $@,, $(LIBS)

clean::
	del /q Test.cpp Test.h
	del /q Types.cpp Types.h

!include .depend
