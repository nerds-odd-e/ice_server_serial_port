// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_AMD_ICE
#define TEST_AMD_ICE

module Test
{

interface Thrower;

exception A
{
    int aMem;
};

exception B extends A
{
    int bMem;
};

exception C extends B
{
    int cMem;
};

exception D
{
    int dMem;
};

module Mod
{
    exception A extends ::Test::A
    {
	int a2Mem;
    };
};


["ami", "amd"] interface Thrower
{
    void shutdown();
    bool supportsUndeclaredExceptions();
    bool supportsAssertException();

    void throwAasA(int a) throws A;
    void throwAorDasAorD(int a) throws A, D;
    void throwBasA(int a, int b) throws A;
    void throwCasA(int a, int b, int c) throws A;
    void throwBasB(int a, int b) throws B;
    void throwCasB(int a, int b, int c) throws B;
    void throwCasC(int a, int b, int c) throws C;

    void throwModA(int a, int a2) throws Mod::A;

    void throwUndeclaredA(int a);
    void throwUndeclaredB(int a, int b);
    void throwUndeclaredC(int a, int b, int c);
    void throwLocalException();
    void throwNonIceException();
    void throwAssertException();
};

["ami", "amd"] interface WrongOperation
{
    void noSuchOperation();
};

};

#endif
