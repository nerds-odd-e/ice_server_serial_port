// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_IDENTITY_ICE
#define ICE_IDENTITY_ICE

module Ice
{

/**
 *
 * The identity of an &Ice; object. An empty [name] denotes a null
 * object.
 *
 **/
struct Identity
{
    /**
     *
     * The name of the &Ice; object.
     *
     **/
    string name;

    /**
     *
     * The &Ice; object category.
     *
     * @see ServantLocator
     * @see ObjectAdapter::addServantLocator
     *
     **/
    string category;
};

/**
 *
 * A mapping between identities and &Ice; objects.
 *
 **/
local dictionary<Identity, Object> ObjectDict;

/**
 *
 * A sequence of identities.
 *
 **/
sequence<Identity> IdentitySeq;

};

#endif
