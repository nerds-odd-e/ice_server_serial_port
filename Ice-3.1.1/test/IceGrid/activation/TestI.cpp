// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestI.h>

using namespace Test;

TestI::TestI(const Ice::ObjectAdapterPtr& adapter) :
    _adapter(adapter),
    _failed(false)
{
}

void
TestI::fail(const Ice::Current&)
{
    _failed = true;
    _adapter->getCommunicator()->shutdown();
}

void
TestI::shutdown(const Ice::Current&)
{
    _adapter->getCommunicator()->shutdown();
}

bool
TestI::isFailed() const
{
    return _failed;
}
