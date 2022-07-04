// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.1.1

package Server;

public final class SerialPortPrxHolder
{
    public
    SerialPortPrxHolder()
    {
    }

    public
    SerialPortPrxHolder(SerialPortPrx value)
    {
	this.value = value;
    }

    public SerialPortPrx value;
}
