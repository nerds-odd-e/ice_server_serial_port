// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>

#include <ClockI.h>

using namespace std;

void
ClockI::tick(const Ice::Current&)
{
    cout << "tick" << endl;
}
