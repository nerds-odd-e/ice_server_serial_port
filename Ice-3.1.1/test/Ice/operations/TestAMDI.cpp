// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestAMDI.h>
#include <TestCommon.h>
#include <functional>
#ifdef __BCPLUSPLUS__
#  include <iterator>
#endif

class Thread_opVoid : public IceUtil::Thread
{
public:

    Thread_opVoid(const Test::AMD_MyClass_opVoidPtr& cb) :
	_cb(cb)
    {
    }

    virtual void run()
    {
	_cb->ice_response();
    }

private:

    Test::AMD_MyClass_opVoidPtr _cb;
};

MyDerivedClassI::MyDerivedClassI(const Ice::ObjectAdapterPtr& adapter, const Ice::Identity& identity) :
    _adapter(adapter),
    _identity(identity)
{
}

void
MyDerivedClassI::shutdown_async(const Test::AMD_MyClass_shutdownPtr& cb, const Ice::Current&)
{
    if(_opVoidThread)
    {
	_opVoidThread->getThreadControl().join();
	_opVoidThread = 0;
    }

    _adapter->getCommunicator()->shutdown();
    cb->ice_response();
}

void
MyDerivedClassI::opVoid_async(const Test::AMD_MyClass_opVoidPtr& cb, const Ice::Current&)
{
    if(_opVoidThread)
    {
	_opVoidThread->getThreadControl().join();
	_opVoidThread = 0;
    }

    _opVoidThread = new Thread_opVoid(cb);
    _opVoidThread->start();
}

void
MyDerivedClassI::opSleep_async(const Test::AMD_MyClass_opSleepPtr& cb, 
			       int duration, 
			       const Ice::Current&)
{
    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(duration));
    cb->ice_response();
}

void
MyDerivedClassI::opByte_async(const Test::AMD_MyClass_opBytePtr& cb,
			      Ice::Byte p1,
			      Ice::Byte p2,
			      const Ice::Current&)
{
    cb->ice_response(p1, p1 ^ p2);
}

void
MyDerivedClassI::opBool_async(const Test::AMD_MyClass_opBoolPtr& cb,
			      bool p1,
			      bool p2,
			      const Ice::Current&)
{
    cb->ice_response(p2, p1);
}

void
MyDerivedClassI::opShortIntLong_async(const Test::AMD_MyClass_opShortIntLongPtr& cb,
				      Ice::Short p1,
				      Ice::Int p2,
				      Ice::Long p3,
				      const Ice::Current&)
{
    cb->ice_response(p3, p1, p2, p3);
}

void
MyDerivedClassI::opFloatDouble_async(const Test::AMD_MyClass_opFloatDoublePtr& cb,
				     Ice::Float p1,
				     Ice::Double p2,
				     const Ice::Current&)
{
    cb->ice_response(p2, p1, p2);
}

void
MyDerivedClassI::opString_async(const Test::AMD_MyClass_opStringPtr& cb,
				const std::string& p1,
				const std::string& p2,
				const Ice::Current&)
{
    cb->ice_response(p1 + " " + p2, p2 + " " + p1);
}

void
MyDerivedClassI::opMyEnum_async(const Test::AMD_MyClass_opMyEnumPtr& cb,
				Test::MyEnum p1,
				const Ice::Current&)
{
    cb->ice_response(Test::enum3, p1);
}

void
MyDerivedClassI::opMyClass_async(const Test::AMD_MyClass_opMyClassPtr& cb,
				 const Test::MyClassPrx& p1,
				 const Ice::Current&)
{
    Test::MyClassPrx p2 = p1;
    Test::MyClassPrx p3 = Test::MyClassPrx::uncheckedCast(_adapter->createProxy(
    					_adapter->getCommunicator()->stringToIdentity("noSuchIdentity")));
    cb->ice_response(Test::MyClassPrx::uncheckedCast(_adapter->createProxy(_identity)), p2, p3);
}

void
MyDerivedClassI::opStruct_async(const Test::AMD_MyClass_opStructPtr& cb,
				const Test::Structure& p1,
				const Test::Structure& p2,
				const Ice::Current&)
{
    Test::Structure p3 = p1;
    p3.s.s = "a new string";
    cb->ice_response(p2, p3);
}

void
MyDerivedClassI::opByteS_async(const Test::AMD_MyClass_opByteSPtr& cb,
			       const Test::ByteS& p1,
			       const Test::ByteS& p2,
			       const Ice::Current&)
{
    Test::ByteS p3;
    p3.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), p3.begin());
    Test::ByteS r = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(r));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opBoolS_async(const Test::AMD_MyClass_opBoolSPtr& cb,
			       const Test::BoolS& p1,
			       const Test::BoolS& p2,
			       const Ice::Current&)
{
    Test::BoolS p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::BoolS r;
    r.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), r.begin());
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opShortIntLongS_async(const Test::AMD_MyClass_opShortIntLongSPtr& cb,
				       const Test::ShortS& p1,
				       const Test::IntS& p2,
				       const Test::LongS& p3,
				       const Ice::Current&)
{
    Test::ShortS p4 = p1;
    Test::IntS p5;
    p5.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p5.begin());
    Test::LongS p6 = p3;
    std::copy(p3.begin(), p3.end(), std::back_inserter(p6));
    cb->ice_response(p3, p4, p5, p6);
}

void
MyDerivedClassI::opFloatDoubleS_async(const Test::AMD_MyClass_opFloatDoubleSPtr& cb,
				      const Test::FloatS& p1,
				      const Test::DoubleS& p2,
				      const Ice::Current&)
{
    Test::FloatS p3 = p1;
    Test::DoubleS p4;
    p4.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p4.begin());
    Test::DoubleS r = p2;
    std::copy(p1.begin(), p1.end(), std::back_inserter(r));
    cb->ice_response(r, p3, p4);
}

void
MyDerivedClassI::opStringS_async(const Test::AMD_MyClass_opStringSPtr& cb,
				 const Test::StringS& p1,
				 const Test::StringS& p2,
				 const Ice::Current&)
{
    Test::StringS p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::StringS r;
    r.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), r.begin());
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opByteSS_async(const Test::AMD_MyClass_opByteSSPtr& cb,
				const Test::ByteSS& p1,
				const Test::ByteSS& p2,
				const Ice::Current&)
{
    Test::ByteSS p3;
    p3.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), p3.begin());
    Test::ByteSS r = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(r));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opBoolSS_async(const Test::AMD_MyClass_opBoolSSPtr& cb,
				const Test::BoolSS& p1,
				const Test::BoolSS& p2,
				const Ice::Current&)
{
    Test::BoolSS p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::BoolSS r;
    r.resize(p1.size());
    std::reverse_copy(p1.begin(), p1.end(), r.begin());
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opShortIntLongSS_async(const Test::AMD_MyClass_opShortIntLongSSPtr& cb,
					const Test::ShortSS& p1,
					const Test::IntSS& p2,
					const Test::LongSS& p3,
					const Ice::Current&)
{
    Test::ShortSS p4 = p1;
    Test::IntSS p5;
    p5.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p5.begin());
    Test::LongSS p6 = p3;
    std::copy(p3.begin(), p3.end(), std::back_inserter(p6));
    cb->ice_response(p3, p4, p5, p6);
}

void
MyDerivedClassI::opFloatDoubleSS_async(const Test::AMD_MyClass_opFloatDoubleSSPtr& cb,
				       const Test::FloatSS& p1,
				       const Test::DoubleSS& p2,
				       const Ice::Current&)
{
    Test::FloatSS p3 = p1;
    Test::DoubleSS p4;
    p4.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), p4.begin());
    Test::DoubleSS r = p2;
    std::copy(p2.begin(), p2.end(), std::back_inserter(r));
    cb->ice_response(r, p3, p4);
}

void
MyDerivedClassI::opStringSS_async(const Test::AMD_MyClass_opStringSSPtr& cb,
				  const Test::StringSS& p1,
				  const Test::StringSS& p2,
				  const Ice::Current&)
{
    Test::StringSS p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::StringSS r;
    r.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), r.begin());
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opStringSSS_async(const Test::AMD_MyClass_opStringSSSPtr& cb,
				  const Test::StringSSS& p1,
				  const Test::StringSSS& p2,
				  const Ice::Current&)
{
    Test::StringSSS p3 = p1;
    std::copy(p2.begin(), p2.end(), std::back_inserter(p3));
    Test::StringSSS r;
    r.resize(p2.size());
    std::reverse_copy(p2.begin(), p2.end(), r.begin());
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opByteBoolD_async(const Test::AMD_MyClass_opByteBoolDPtr& cb,
				   const Test::ByteBoolD& p1,
				   const Test::ByteBoolD& p2,
				   const Ice::Current&)
{
    Test::ByteBoolD p3 = p1;
    Test::ByteBoolD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opShortIntD_async(const Test::AMD_MyClass_opShortIntDPtr& cb,
				   const Test::ShortIntD& p1,
				   const Test::ShortIntD& p2,
				   const Ice::Current&)
{
    Test::ShortIntD p3 = p1;
    Test::ShortIntD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opLongFloatD_async(const Test::AMD_MyClass_opLongFloatDPtr& cb,
				    const Test::LongFloatD& p1,
				    const Test::LongFloatD& p2,
				    const Ice::Current&)
{
    Test::LongFloatD p3 = p1;
    Test::LongFloatD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opStringStringD_async(const Test::AMD_MyClass_opStringStringDPtr& cb,
				       const Test::StringStringD& p1,
				       const Test::StringStringD& p2,
				       const Ice::Current&)
{
    Test::StringStringD p3 = p1;
    Test::StringStringD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opStringMyEnumD_async(const Test::AMD_MyClass_opStringMyEnumDPtr& cb,
				       const Test::StringMyEnumD& p1,
				       const Test::StringMyEnumD& p2,
				       const Ice::Current&)
{
    Test::StringMyEnumD p3 = p1;
    Test::StringMyEnumD r = p1;
    std::set_union(p1.begin(), p1.end(), p2.begin(), p2.end(), std::inserter(r, r.end()));
    cb->ice_response(r, p3);
}

void
MyDerivedClassI::opIntS_async(const Test::AMD_MyClass_opIntSPtr& cb, const Test::IntS& s, const Ice::Current&)
{
    Test::IntS r;
    std::transform(s.begin(), s.end(), std::back_inserter(r), std::negate<int>());
    cb->ice_response(r);
}

void
MyDerivedClassI::opByteSOneway_async(const Test::AMD_MyClass_opByteSOnewayPtr& cb, const Test::ByteS& s,
				     const Ice::Current&)
{
    cb->ice_response();
}

void
MyDerivedClassI::opContext_async(const Test::AMD_MyClass_opContextPtr& cb, const Ice::Current& c)
{
    Test::StringStringD r = c.ctx;
    cb->ice_response(r);
}

void 
MyDerivedClassI::opDoubleMarshaling_async(const Test::AMD_MyClass_opDoubleMarshalingPtr& cb,
					  Ice::Double p1, const Test::DoubleS& p2, const Ice::Current&)
{
    Ice::Double d = 1278312346.0 / 13.0;
    test(p1 == d);
    for(unsigned int i = 0; i < p2.size(); ++i)
    {
        test(p2[i] == d);
    }
    cb->ice_response();
}

void
MyDerivedClassI::opDerived_async(const Test::AMD_MyDerivedClass_opDerivedPtr& cb, const Ice::Current&)
{
    cb->ice_response();
}

void
TestCheckedCastI::getContext_async(const Test::AMD_TestCheckedCast_getContextPtr& cb, const Ice::Current&)
{
    cb->ice_response(_ctx);
}

bool
TestCheckedCastI::ice_isA(const std::string& s, const Ice::Current& current) const
{
    _ctx = current.ctx;
#ifdef __BCPLUSPLUS__
    return Test::TestCheckedCast::ice_isA(s, current);
#else
    return TestCheckedCast::ice_isA(s, current);
#endif
}
