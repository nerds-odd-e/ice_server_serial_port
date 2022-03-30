// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_PROXY_VERIFIER_H
#define ICE_PROXY_VERIFIER_H

#include <Ice/Ice.h>
#include <vector>

namespace Glacier2
{

//
// Base class for proxy rule implementations. 
//

class ProxyRule
{
public:
    virtual ~ProxyRule() {}

    //
    // Checks to see if the proxy passes.
    //
    virtual bool check(const Ice::ObjectPrx&) const = 0;
};

class ProxyVerifier
{
public:
    ProxyVerifier(const Ice::CommunicatorPtr&, const char*);
    ~ProxyVerifier();

    //
    // Verifies that the proxy is permissible under the configured
    // rules.
    //
    bool verify(const Ice::ObjectPrx&);

private:
    const Ice::CommunicatorPtr _communicator;

    const int _traceLevel;

    std::vector<ProxyRule*> _acceptRules;
    std::vector<ProxyRule*> _rejectRules;
};

}
#endif
