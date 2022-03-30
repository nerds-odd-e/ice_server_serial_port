// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_ACCEPTOR_F_H
#define ICE_ACCEPTOR_F_H

#include <Ice/Handle.h>

namespace IceInternal
{

class Acceptor;
ICE_API void incRef(Acceptor*);
ICE_API void decRef(Acceptor*);
typedef Handle<Acceptor> AcceptorPtr;

}

#endif
