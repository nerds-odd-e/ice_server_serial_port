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
SERVER		= server.exe

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Test.obj \
		  Client.obj \
		  AllTests.obj

SOBJS		= Test.obj \
		  TestI.obj \
		  ServerLocator.obj \
		  Server.obj

SRCS		= $(COBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. -I../../include $(CPPFLAGS)

$(CLIENT): $(COBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(COBJS), $@,, $(LIBS)

$(SERVER): $(SOBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(SOBJS), $@,, $(LIBS)

clean::
	del /q Test.cpp Test.h

!include .depend
