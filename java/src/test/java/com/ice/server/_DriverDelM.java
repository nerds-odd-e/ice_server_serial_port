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

public final class _DriverDelM extends Ice._ObjectDelM implements _DriverDel {
    @Override
	public String
    readInfoFromDevice(java.util.Map __ctx)
            throws IceInternal.LocalExceptionWrapper {
        IceInternal.Outgoing __og = __connection.getOutgoing(__reference, "readInfoFromDevice", Ice.OperationMode.Normal, __ctx, __compress);
        try {
            boolean __ok = __og.invoke();
            try {
                IceInternal.BasicStream __is = __og.is();
                if (!__ok) {
                    try {
                        __is.throwException();
                    } catch (Ice.UserException __ex) {
                        throw new Ice.UnknownUserException(__ex.ice_name());
                    }
                }
                String __ret;
                __ret = __is.readString();
                return __ret;
            } catch (Ice.LocalException __ex) {
                throw new IceInternal.LocalExceptionWrapper(__ex, false);
            }
        } finally {
            __connection.reclaimOutgoing(__og);
        }
    }
}
