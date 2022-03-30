// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_SSL_ENDPOINT_I_H
#define ICE_SSL_ENDPOINT_I_H

#include <Ice/EndpointI.h>
#include <Ice/EndpointFactory.h>
#include <IceSSL/InstanceF.h>

namespace IceSSL
{

const Ice::Short EndpointType = 2;

class EndpointI : public IceInternal::EndpointI
{
public:

    EndpointI(const InstancePtr&, const std::string&, Ice::Int, Ice::Int, const std::string&, bool, bool);
    EndpointI(const InstancePtr&, const std::string&);
    EndpointI(const InstancePtr&, IceInternal::BasicStream*);

    virtual void streamWrite(IceInternal::BasicStream*) const;
    virtual std::string toString() const;
    virtual Ice::Short type() const;
    virtual Ice::Int timeout() const;
    virtual IceInternal::EndpointIPtr timeout(Ice::Int) const;
    virtual IceInternal::EndpointIPtr connectionId(const ::std::string&) const;
    virtual bool compress() const;
    virtual IceInternal::EndpointIPtr compress(bool) const;
    virtual bool datagram() const;
    virtual bool secure() const;
    virtual bool unknown() const;
    virtual IceInternal::TransceiverPtr clientTransceiver() const;
    virtual IceInternal::TransceiverPtr serverTransceiver(IceInternal::EndpointIPtr&) const;
    virtual IceInternal::ConnectorPtr connector() const;
    virtual IceInternal::AcceptorPtr acceptor(IceInternal::EndpointIPtr&, const std::string&) const;
    virtual std::vector<IceInternal::EndpointIPtr> expand(bool) const;
    virtual bool publish() const;
    virtual bool equivalent(const IceInternal::TransceiverPtr&) const;
    virtual bool equivalent(const IceInternal::AcceptorPtr&) const;

    virtual bool operator==(const IceInternal::EndpointI&) const;
    virtual bool operator!=(const IceInternal::EndpointI&) const;
    virtual bool operator<(const IceInternal::EndpointI&) const;

private:

#if defined(__SUNPRO_CC)
    //
    // COMPILERFIX: prevent the compiler from emitting a warning about
    // hidding these operators.
    //
    using LocalObject::operator==;
    using LocalObject::operator!=;
    using LocalObject::operator<;
#endif

    //
    // All members are const, because endpoints are immutable.
    //
    const InstancePtr _instance;
    const std::string _host;
    const Ice::Int _port;
    const Ice::Int _timeout;
    const std::string _connectionId;
    const bool _compress;
    const bool _publish;
};

class EndpointFactoryI : public IceInternal::EndpointFactory
{
public:

    virtual ~EndpointFactoryI();

    virtual Ice::Short type() const;
    virtual std::string protocol() const;
    virtual IceInternal::EndpointIPtr create(const std::string&) const;
    virtual IceInternal::EndpointIPtr read(IceInternal::BasicStream*) const;
    virtual void destroy();

private:

    EndpointFactoryI(const InstancePtr&);
    friend class Instance;

    InstancePtr _instance;
};

}

#endif
