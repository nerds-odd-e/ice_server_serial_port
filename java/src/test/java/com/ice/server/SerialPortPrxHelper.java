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

public final class SerialPortPrxHelper extends Ice.ObjectPrxHelperBase implements SerialPortPrx {
    public String
    readSerialPort() {
        return readSerialPort(__defaultContext());
    }

    public String
    readSerialPort(java.util.Map __ctx) {
        int __cnt = 0;
        while (true) {
            try {
                __checkTwowayOnly("readSerialPort");
                Ice._ObjectDel __delBase = __getDelegate();
                _SerialPortDel __del = (_SerialPortDel) __delBase;
                return __del.readSerialPort(__ctx);
            } catch (IceInternal.LocalExceptionWrapper __ex) {
                __handleExceptionWrapper(__ex);
            } catch (Ice.LocalException __ex) {
                __cnt = __handleException(__ex, __cnt);
            }
        }
    }

    public static SerialPortPrx
    checkedCast(Ice.ObjectPrx __obj) {
        SerialPortPrx __d = null;
        if (__obj != null) {
            try {
                __d = (SerialPortPrx) __obj;
            } catch (ClassCastException ex) {
                if (__obj.ice_isA("::Server::SerialPort")) {
                    SerialPortPrxHelper __h = new SerialPortPrxHelper();
                    __h.__copyFrom(__obj);
                    __d = __h;
                }
            }
        }
        return __d;
    }

    public static SerialPortPrx
    checkedCast(Ice.ObjectPrx __obj, java.util.Map __ctx) {
        SerialPortPrx __d = null;
        if (__obj != null) {
            try {
                __d = (SerialPortPrx) __obj;
            } catch (ClassCastException ex) {
                if (__obj.ice_isA("::Server::SerialPort", __ctx)) {
                    SerialPortPrxHelper __h = new SerialPortPrxHelper();
                    __h.__copyFrom(__obj);
                    __d = __h;
                }
            }
        }
        return __d;
    }

    public static SerialPortPrx
    checkedCast(Ice.ObjectPrx __obj, String __facet) {
        SerialPortPrx __d = null;
        if (__obj != null) {
            Ice.ObjectPrx __bb = __obj.ice_facet(__facet);
            try {
                if (__bb.ice_isA("::Server::SerialPort")) {
                    SerialPortPrxHelper __h = new SerialPortPrxHelper();
                    __h.__copyFrom(__bb);
                    __d = __h;
                }
            } catch (Ice.FacetNotExistException ex) {
            }
        }
        return __d;
    }

    public static SerialPortPrx
    checkedCast(Ice.ObjectPrx __obj, String __facet, java.util.Map __ctx) {
        SerialPortPrx __d = null;
        if (__obj != null) {
            Ice.ObjectPrx __bb = __obj.ice_facet(__facet);
            try {
                if (__bb.ice_isA("::Server::SerialPort", __ctx)) {
                    SerialPortPrxHelper __h = new SerialPortPrxHelper();
                    __h.__copyFrom(__bb);
                    __d = __h;
                }
            } catch (Ice.FacetNotExistException ex) {
            }
        }
        return __d;
    }

    public static SerialPortPrx
    uncheckedCast(Ice.ObjectPrx __obj) {
        SerialPortPrx __d = null;
        if (__obj != null) {
            SerialPortPrxHelper __h = new SerialPortPrxHelper();
            __h.__copyFrom(__obj);
            __d = __h;
        }
        return __d;
    }

    public static SerialPortPrx
    uncheckedCast(Ice.ObjectPrx __obj, String __facet) {
        SerialPortPrx __d = null;
        if (__obj != null) {
            Ice.ObjectPrx __bb = __obj.ice_facet(__facet);
            SerialPortPrxHelper __h = new SerialPortPrxHelper();
            __h.__copyFrom(__bb);
            __d = __h;
        }
        return __d;
    }

    public static void
    __write(IceInternal.BasicStream __os, SerialPortPrx v) {
        __os.writeProxy(v);
    }

    public static SerialPortPrx
    __read(IceInternal.BasicStream __is) {
        Ice.ObjectPrx proxy = __is.readProxy();
        if (proxy != null) {
            SerialPortPrxHelper result = new SerialPortPrxHelper();
            result.__copyFrom(proxy);
            return result;
        }
        return null;
    }

    protected Ice._ObjectDelM
    __createDelegateM() {
        return new _SerialPortDelM();
    }

    protected Ice._ObjectDelD
    __createDelegateD() {
        return new _SerialPortDelD();
    }
}
