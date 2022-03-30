# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME		= $(top_srcdir)\lib\icepatch2$(LIBSUFFIX).lib
DLLNAME		= $(top_srcdir)\bin\icepatch2$(SOVERSION)$(LIBSUFFIX).dll

SERVER		= $(top_srcdir)\bin\icepatch2server.exe
CLIENT		= $(top_srcdir)\bin\icepatch2client.exe
CALC		= $(top_srcdir)\bin\icepatch2calc.exe

!ifdef BUILD_UTILS

TARGETS         = $(SERVER) $(CLIENT) $(CALC)

!else

TARGETS         = $(LIBNAME) $(DLLNAME)

!endif

OBJS		= Util.obj \
		  ClientUtil.obj \
		  FileInfo.obj \
		  FileServer.obj \
		  OS.obj

SOBJS		= Server.obj \
		  FileServerI.obj

COBJS		= Client.obj

CALCOBJS	= Calc.obj

SRCS		= $(OBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp) \
		  $(COBJS:.obj=.cpp) \
		  $(CALCOBJS:.obj=.cpp)

HDIR		= $(includedir)\IcePatch2
SDIR		= $(slicedir)\IcePatch2

!include $(top_srcdir)\config\Make.rules.mak

!ifdef BUILD_UTILS

CPPFLAGS	= -I. -I.. $(CPPFLAGS)

!else

CPPFLAGS	= -I. -I.. $(CPPFLAGS) -DICE_PATCH2_API_EXPORTS

!endif

SLICE2CPPFLAGS	= --ice --include-dir IcePatch2 --dll-export ICE_PATCH2_API $(SLICE2CPPFLAGS)

$(LIBNAME): $(DLLNAME)

$(DLLNAME): $(OBJS)
	del /q $@
	$(LINK) $(LD_DLLFLAGS) $(OBJS), $(DLLNAME),, $(LIBS) $(BZIP2_LIBS) $(OPENSSL_LIBS)
	move $(DLLNAME:.dll=.lib) $(LIBNAME)

$(SERVER): $(SOBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(SOBJS), $@,, $(LIBS) icepatch2$(LIBSUFFIX).lib

$(CLIENT): $(COBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(COBJS), $@,, $(LIBS) icepatch2$(LIBSUFFIX).lib

$(CALC): $(CALCOBJS)
	del /q $@
	$(LINK) $(LD_EXEFLAGS) $(CALCOBJS), $@,, $(LIBS) icepatch2$(LIBSUFFIX).lib

!ifdef BUILD_UTILS

clean::
	del /q FileInfo.cpp $(HDIR)\FileInfo.h
	del /q FileServer.cpp $(HDIR)\FileServer.h

install:: all
	copy $(LIBNAME) $(install_libdir)
	copy $(DLLNAME) $(install_bindir)
	copy $(DLLNAME:.dll=.tds) $(install_bindir)
	copy $(SERVER) $(install_bindir)
	copy $(CLIENT) $(install_bindir)
	copy $(CALC) $(install_bindir)

!else

install:: all

$(EVERYTHING)::
	$(MAKE) -nologo /f Makefile.mak BUILD_UTILS=1 $@ || exit 1

!endif

!include .depend
