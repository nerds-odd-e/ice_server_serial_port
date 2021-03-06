// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_I_H
#define TEST_I_H

#include <Test.h>

class ChecksumI : public Test::Checksum
{
public:

    ChecksumI(const Ice::ObjectAdapterPtr&);

    virtual Ice::SliceChecksumDict getSliceChecksums(const Ice::Current&) const;

    virtual void shutdown(const Ice::Current&);

private:

    Ice::ObjectAdapterPtr _adapter;
};

#endif
