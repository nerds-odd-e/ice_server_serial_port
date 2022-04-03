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

public final class SerialPortHolder {
    public SerialPort value;

    public SerialPortHolder() {
    }

    public SerialPortHolder(SerialPort value) {
        this.value = value;
    }

    public Patcher
    getPatcher() {
        return new Patcher();
    }

    public class Patcher implements IceInternal.Patcher {
        public void
        patch(Ice.Object v) {
            value = (SerialPort) v;
        }

        public String
        type() {
            return "::Server::SerialPort";
        }
    }
}
