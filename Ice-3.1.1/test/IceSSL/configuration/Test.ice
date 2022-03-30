// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

module Test
{

interface Server
{
    void noCert();
    void checkCert(string subjectDN, string issuerDN);
    void checkCipher(string cipher);
};

dictionary<string, string> Properties;

interface ServerFactory
{
    Server* createServer(Properties props);
    void destroyServer(Server* srv);
    void shutdown();
};

};

#endif
