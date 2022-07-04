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

public final class DriverPrxHelper extends Ice.ObjectPrxHelperBase implements DriverPrx {
    @Override
	public String
    readInfoFromDevice() {
        return readInfoFromDevice(__defaultContext());
    }

    @Override
	public String
    readInfoFromDevice(java.util.Map __ctx) {
        int __cnt = 0;
        while (true) {
            try {
                __checkTwowayOnly("readInfoFromDevice");
                Ice._ObjectDel __delBase = __getDelegate();
                _DriverDel __del = (_DriverDel) __delBase;
                return __del.readInfoFromDevice(__ctx);
            } catch (IceInternal.LocalExceptionWrapper __ex) {
                __handleExceptionWrapper(__ex);
            } catch (Ice.LocalException __ex) {
                __cnt = __handleException(__ex, __cnt);
            }
        }
    }

    public static DriverPrx
    checkedCast(Ice.ObjectPrx __obj) {
        DriverPrx __d = null;
        if (__obj != null) {
            try {
                __d = (DriverPrx) __obj;
            } catch (ClassCastException ex) {
                if (__obj.ice_isA("::Server::Driver")) {
                    DriverPrxHelper __h = new DriverPrxHelper();
                    __h.__copyFrom(__obj);
                    __d = __h;
                }
            }
        }
        return __d;
    }

    public static DriverPrx
    checkedCast(Ice.ObjectPrx __obj, java.util.Map __ctx) {
        DriverPrx __d = null;
        if (__obj != null) {
            try {
                __d = (DriverPrx) __obj;
            } catch (ClassCastException ex) {
                if (__obj.ice_isA("::Server::Driver", __ctx)) {
                    DriverPrxHelper __h = new DriverPrxHelper();
                    __h.__copyFrom(__obj);
                    __d = __h;
                }
            }
        }
        return __d;
    }

    public static DriverPrx
    checkedCast(Ice.ObjectPrx __obj, String __facet) {
        DriverPrx __d = null;
        if (__obj != null) {
            Ice.ObjectPrx __bb = __obj.ice_facet(__facet);
            try {
                if (__bb.ice_isA("::Server::Driver")) {
                    DriverPrxHelper __h = new DriverPrxHelper();
                    __h.__copyFrom(__bb);
                    __d = __h;
                }
            } catch (Ice.FacetNotExistException ex) {
            }
        }
        return __d;
    }

    public static DriverPrx
    checkedCast(Ice.ObjectPrx __obj, String __facet, java.util.Map __ctx) {
        DriverPrx __d = null;
        if (__obj != null) {
            Ice.ObjectPrx __bb = __obj.ice_facet(__facet);
            try {
                if (__bb.ice_isA("::Server::Driver", __ctx)) {
                    DriverPrxHelper __h = new DriverPrxHelper();
                    __h.__copyFrom(__bb);
                    __d = __h;
                }
            } catch (Ice.FacetNotExistException ex) {
            }
        }
        return __d;
    }

    public static DriverPrx
    uncheckedCast(Ice.ObjectPrx __obj) {
        DriverPrx __d = null;
        if (__obj != null) {
            DriverPrxHelper __h = new DriverPrxHelper();
            __h.__copyFrom(__obj);
            __d = __h;
        }
        return __d;
    }

    public static DriverPrx
    uncheckedCast(Ice.ObjectPrx __obj, String __facet) {
        DriverPrx __d = null;
        if (__obj != null) {
            Ice.ObjectPrx __bb = __obj.ice_facet(__facet);
            DriverPrxHelper __h = new DriverPrxHelper();
            __h.__copyFrom(__bb);
            __d = __h;
        }
        return __d;
    }

    @Override
	protected Ice._ObjectDelM
    __createDelegateM() {
        return new _DriverDelM();
    }

    @Override
	protected Ice._ObjectDelD
    __createDelegateD() {
        return new _DriverDelD();
    }

    public static void
    __write(IceInternal.BasicStream __os, DriverPrx v) {
        __os.writeProxy(v);
    }

    public static DriverPrx
    __read(IceInternal.BasicStream __is) {
        Ice.ObjectPrx proxy = __is.readProxy();
        if (proxy != null) {
            DriverPrxHelper result = new DriverPrxHelper();
            result.__copyFrom(proxy);
            return result;
        }
        return null;
    }
}
