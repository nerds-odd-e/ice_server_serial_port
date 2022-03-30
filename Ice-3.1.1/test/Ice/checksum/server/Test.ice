// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

#include <Ice/SliceChecksumDict.ice>

module Test
{

interface Checksum
{
    nonmutating Ice::SliceChecksumDict getSliceChecksums();
    
    void shutdown();
};

};

#endif
