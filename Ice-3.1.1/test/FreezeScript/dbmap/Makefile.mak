# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

CLIENT		= makedb.exe

TARGETS		= $(CLIENT)

OBJS		= TestOld.obj \
                  makedb.obj \
                  IntSMap.obj

SRCS		= $(OBJS:.obj=.cpp)

SLICE_SRCS	= TestOld.ice

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. $(CPPFLAGS)

$(CLIENT): $(OBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(OBJS), $@,, $(LIBS) freeze$(LIBSUFFIX).lib

IntSMap.h IntSMap.cpp: $(SLICE2FREEZE)
	del /q IntSMap.h IntSMap.cpp
	$(SLICE2FREEZE) --dict IntSMap,int,::Test::S IntSMap TestOld.ice

clean::
	del /q IntSMap.h IntSMap.cpp

clean::
	del /q TestOld.cpp TestOld.h

clean::
	del /q db\*.db db\log.* db\__catalog

!include .depend
