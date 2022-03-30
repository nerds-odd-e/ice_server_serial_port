// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_PROPERTIES_I_H
#define ICE_PROPERTIES_I_H

#include <IceUtil/Mutex.h>
#include <Ice/Properties.h>
#include <Ice/StringConverter.h>

namespace Ice
{

class ICE_API PropertiesI : public Properties, public IceUtil::Mutex
{
public:
    
    virtual std::string getProperty(const std::string&);
    virtual std::string getPropertyWithDefault(const std::string&, const std::string&);
    virtual Ice::Int getPropertyAsInt(const std::string&);
    virtual Ice::Int getPropertyAsIntWithDefault(const std::string&, Ice::Int);
    virtual PropertyDict getPropertiesForPrefix(const std::string&);
    virtual void setProperty(const std::string&, const std::string&);
    virtual StringSeq getCommandLineOptions();
    virtual StringSeq parseCommandLineOptions(const std::string&, const StringSeq&);
    virtual StringSeq parseIceCommandLineOptions(const StringSeq&);
    virtual void load(const std::string&);
    virtual PropertiesPtr clone();

private:
    PropertiesI(const StringConverterPtr&);
    PropertiesI(StringSeq&, const PropertiesPtr&, const StringConverterPtr&);
    PropertiesI(const PropertiesI*);

    friend ICE_API PropertiesPtr createProperties(const StringConverterPtr&);
    friend ICE_API PropertiesPtr createProperties(StringSeq&, const PropertiesPtr&, const StringConverterPtr&);
    friend ICE_API PropertiesPtr createProperties(int&, char*[], const PropertiesPtr&, const StringConverterPtr&);


    void parseLine(const std::string&, const StringConverterPtr&);

    void loadConfig();

    std::map<std::string, std::string> _properties;
    const StringConverterPtr _converter;
};

}

#endif
