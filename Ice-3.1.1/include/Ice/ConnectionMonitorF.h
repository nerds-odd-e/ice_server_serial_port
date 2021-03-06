// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_CONNECTION_MONITOR_F_H
#define ICE_CONNECTION_MONITOR_F_H

#include <Ice/Handle.h>

namespace IceInternal
{

class ConnectionMonitor;
ICE_API void incRef(ConnectionMonitor*);
ICE_API void decRef(ConnectionMonitor*);
typedef IceInternal::Handle<ConnectionMonitor> ConnectionMonitorPtr;

}

#endif
