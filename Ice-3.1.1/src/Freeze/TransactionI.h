// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef FREEZE_TRANSACTIONI_H
#define FREEZE_TRANSACTIONI_H

#include <Freeze/Transaction.h>
#include <db_cxx.h>

namespace Freeze
{

class ConnectionI;
typedef IceUtil::Handle<ConnectionI> ConnectionIPtr;

class TransactionI : public Transaction
{
public:

    virtual void
    commit();

    virtual void
    rollback();
    
    TransactionI(ConnectionI*);
    
    ~TransactionI();
    
    DbTxn*
    dbTxn() const
    {
	return _txn;
    }

private:
    
    void
    cleanup();

    ConnectionIPtr _connection;
    Ice::Int _txTrace;
    DbTxn* _txn;
};

typedef IceUtil::Handle<TransactionI> TransactionIPtr;

}
#endif
