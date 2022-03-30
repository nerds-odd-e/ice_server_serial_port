// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/GCRecMutex.h>

namespace IceInternal
{
GCRecMutex gcRecMutex;
}

using namespace IceInternal;

static int gcInitCount = 0;

GCRecMutex::GCRecMutex()
{
    if(gcInitCount++ == 0)
    {
	_m = new IceUtil::RecMutex;
    }
}

GCRecMutex::~GCRecMutex()
{
    if(--gcInitCount == 0)
    {
	delete _m;
    }
}

GCRecMutexInit::GCRecMutexInit()
{
    if(gcInitCount++ == 0)
    {
	gcRecMutex._m = new IceUtil::RecMutex;
    }
}

GCRecMutexInit::~GCRecMutexInit()
{
    if(--gcInitCount == 0)
    {
	delete gcRecMutex._m;
    }
}
