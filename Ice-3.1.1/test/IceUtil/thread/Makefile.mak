# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

CLIENT		= client.exe

TARGETS		= $(CLIENT)

OBJS		= TestBase.obj \
		  CreateTest.obj \
		  AliveTest.obj \
		  StartTest.obj \
		  RWRecMutexTest.obj \
		  RecMutexTest.obj \
		  StaticMutexTest.obj \
		  MutexTest.obj \
		  MonitorMutexTest.obj \
		  MonitorRecMutexTest.obj \
                  CountDownLatchTest.obj \
		  TestSuite.obj \
		  Client.obj


SRCS		= $(OBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. -I../../include $(CPPFLAGS)

$(CLIENT): $(OBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(OBJS), $@,, $(BASELIBS)

!include .depend
