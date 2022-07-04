// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.1.1

package com.ice.server;

public final class DriverHolder {
    public DriverHolder() {
    }

    public DriverHolder(Driver value) {
        this.value = value;
    }

    public class Patcher implements IceInternal.Patcher {
        @Override
        public void
        patch(Ice.Object v) {
            value = (Driver) v;
        }

        @Override
        public String
        type() {
            return "::Server::Driver";
        }
    }

    public Patcher
    getPatcher() {
        return new Patcher();
    }

    public Driver value;
}
