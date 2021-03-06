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

public abstract class _SerialPortDisp extends Ice.ObjectImpl implements SerialPort {
	public static final String[] __ids =
			{
					"::Ice::Object",
					"::Server::SerialPort"
			};
	private final static String[] __all =
			{
					"ice_id",
					"ice_ids",
					"ice_isA",
					"ice_ping",
					"readSerialPort",
					"writeSerialPort"
			};

	public boolean
	ice_isA(String s) {
		return java.util.Arrays.binarySearch(__ids, s) >= 0;
    }

    public boolean
    ice_isA(String s, Ice.Current __current) {
        return java.util.Arrays.binarySearch(__ids, s) >= 0;
    }

    public String[]
    ice_ids() {
        return __ids;
    }

    public String[]
    ice_ids(Ice.Current __current) {
        return __ids;
    }

    public String
    ice_id() {
        return __ids[1];
    }

    public String
    ice_id(Ice.Current __current) {
        return __ids[1];
    }

	public static String
	ice_staticId() {
		return __ids[1];
	}

	public final String
	readSerialPort() {
		return readSerialPort(null);
	}

	public final void
	writeSerialPort(String message) {
		writeSerialPort(message, null);
	}

	public static IceInternal.DispatchStatus
	___readSerialPort(SerialPort __obj, IceInternal.Incoming __inS, Ice.Current __current) {
		__checkMode(Ice.OperationMode.Normal, __current.mode);
		IceInternal.BasicStream __os = __inS.os();
		String __ret = __obj.readSerialPort(__current);
		__os.writeString(__ret);
		return IceInternal.DispatchStatus.DispatchOK;
	}

	public static IceInternal.DispatchStatus
	___writeSerialPort(SerialPort __obj, IceInternal.Incoming __inS, Ice.Current __current) {
		__checkMode(Ice.OperationMode.Normal, __current.mode);
		IceInternal.BasicStream __is = __inS.is();
		String message;
		message = __is.readString();
		__obj.writeSerialPort(message, __current);
		return IceInternal.DispatchStatus.DispatchOK;
	}

	public IceInternal.DispatchStatus
	__dispatch(IceInternal.Incoming in, Ice.Current __current) {
		int pos = java.util.Arrays.binarySearch(__all, __current.operation);
		if (pos < 0) {
			return IceInternal.DispatchStatus.DispatchOperationNotExist;
		}

		switch (pos) {
			case 0: {
                return ___ice_id(this, in, __current);
            }
            case 1: {
                return ___ice_ids(this, in, __current);
			}
			case 2: {
				return ___ice_isA(this, in, __current);
			}
			case 3: {
				return ___ice_ping(this, in, __current);
			}
			case 4: {
				return ___readSerialPort(this, in, __current);
			}
			case 5: {
				return ___writeSerialPort(this, in, __current);
			}
		}

        assert (false);
        return IceInternal.DispatchStatus.DispatchOperationNotExist;
    }

	protected void
	ice_copyStateFrom(Ice.Object __obj)
			throws java.lang.CloneNotSupportedException {
		throw new java.lang.CloneNotSupportedException();
	}
}
