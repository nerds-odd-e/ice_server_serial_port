// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <IceUtil/Options.h>
#include <Slice/Preprocessor.h>
#include <Slice/CPlusPlusUtil.h>
#include <IceUtil/OutputUtil.h>
#include <cstring>

using namespace std;
using namespace IceUtil;
using namespace Slice;

static string ICE_ENCODING_COMPARE = "Freeze::IceEncodingCompare";

struct DictIndex
{
    string member;
    bool caseSensitive;
    bool sort;
    string userCompare;

    bool operator==(const DictIndex& rhs) const
    {
	return member == rhs.member;
    }

    bool operator!=(const DictIndex& rhs) const
    {
	return member != rhs.member;
    }
};

struct Dict
{
    string name;
    string key;
    StringList keyMetaData;
    string value;
    StringList valueMetaData;
    bool sort;
    string userCompare;
  
    vector<DictIndex> indices;
};

struct Index
{
    string name;
    string type;
    string member;
    bool caseSensitive;
};

struct IndexType
{
    TypePtr type;
    StringList metaData;
};

void
usage(const char* n)
{
    cerr << "Usage: " << n << " [options] file-base [slice-files...]\n";
    cerr <<
        "Options:\n"
        "-h, --help            Show this message.\n"
        "-v, --version         Display the Ice version.\n"
        "--header-ext EXT      Use EXT instead of the default `h' extension.\n"
        "--source-ext EXT      Use EXT instead of the default `cpp' extension.\n"
	"--add-header HDR[,GUARD]\n"
	"                      Add #include for HDR (with guard GUARD) to generated source file.\n"
        "-DNAME                Define NAME as 1.\n"
        "-DNAME=DEF            Define NAME as DEF.\n"
        "-UNAME                Remove any definition for NAME.\n"
        "-IDIR                 Put DIR in the include file search path.\n"
	"-E                    Print preprocessor output on stdout.\n"
        "--include-dir DIR     Use DIR as the header include directory in source files.\n"
        "--dll-export SYMBOL   Use SYMBOL for DLL exports.\n"
        "--dict NAME,KEY,VALUE[,sort[,COMPARE]]\n"
	"                      Create a Freeze dictionary with the name NAME,\n"
        "                      using KEY as key, and VALUE as value. This\n"
        "                      option may be specified multiple times for\n"
        "                      different names. NAME may be a scoped name.\n"
	"                      By default, keys are sorted using their binary\n"
	"                      Ice-encoding representation. Use 'sort' to sort\n"
	"                      with the COMPARE functor class. COMPARE's default\n"
	"                      value is std::less<KEY>\n"
	"--index NAME,TYPE,MEMBER[,{case-sensitive|case-insensitive}]\n" 
        "                      Create a Freeze evictor index with the name\n"
        "                      NAME for member MEMBER of class TYPE. This\n"
        "                      option may be specified multiple times for\n"
        "                      different names. NAME may be a scoped name.\n"
        "                      When member is a string, the case can be\n"
        "                      sensitive or insensitive (default is sensitive).\n"
	"--dict-index DICT[,MEMBER][,{case-sensitive|case-insensitive}]\n"
	"                 [,sort[,COMPARE]]\n"
	"                      Add an index to dictionary DICT. If MEMBER is \n"
        "                      specified, then DICT's VALUE must be a class or\n"
	"                      a struct, and MEMBER must designate a member of\n"
	"                      VALUE. Otherwise, the entire VALUE is used for \n"
	"                      indexing. When the secondary key is a string, \n"
	"                      the case can be sensitive or insensitive (default\n"
	"                      is sensitive).\n"
	"                      By default, keys are sorted using their binary\n"
	"                      Ice-encoding representation. Use 'sort' to sort\n"
	"                      with the COMPARE functor class. COMPARE's default\n"
	"                      value is std::less<secondary key type>.\n"
        "--output-dir DIR      Create files in the directory DIR.\n"
        "-d, --debug           Print debug messages.\n"
        "--ice                 Permit `Ice' prefix (for building Ice source code only)\n"
        ;
    // Note: --case-sensitive is intentionally not shown here!
}

bool
checkIdentifier(string n, string t, string s)
{
    if(s.empty() || (!isalpha(s[0]) && s[0] != '_'))
    {
	cerr << n << ": `" << t << "' is not a valid type name" << endl;
	return false;
    }
    
    for(unsigned int i = 1; i < s.size(); ++i)
    {
	if(!isalnum(s[i]) && s[i] != '_')
	{
	    cerr << n << ": `" << t << "' is not a valid type name" << endl;
	    return false;
	}
    }

    return true;
}

void
printFreezeTypes(Output& out, const vector<Dict>& dicts, const vector<Index>& indices)
{
    out << '\n';
    out << "\n// Freeze types in this file:";
    for(vector<Dict>::const_iterator p = dicts.begin(); p != dicts.end(); ++p)
    {
	out << "\n// name=\"" << p->name << "\", key=\"" 
	    << p->key << "\", value=\"" << p->value << "\"";
    }
    
    for(vector<Index>::const_iterator q = indices.begin(); q != indices.end(); ++q)
    {
	out << "\n// name=\"" << q->name << "\", type=\"" << q->type 
	    << "\", member=\"" << q->member << "\"";
	if(q->caseSensitive == false)
	{
	    out << " (case insensitive)";
	}
    }
    out << '\n';
}

template<class T>
inline string 
getCompare(const T& t, const string& keyType)
{
    if(t.sort)
    {
	if(t.userCompare == "")
	{
	    return "std::less< " + keyType + ">";
	}
	else
	{
	    return t.userCompare;
	}
    }
    else
    {
	return ICE_ENCODING_COMPARE;
    }
}

void
writeCodecH(const TypePtr& type, const StringList& metaData, const string& name, const string& freezeType, Output& H,
	    const string& dllExport)
{
    H << sp << nl << "class " << dllExport << name;
    H << sb;
    H.dec();
    H << sp << nl << "public:";
    H << sp;
    H.inc();
    H << nl << "static void write(" << inputTypeToString(type, false, metaData)
      << ", Freeze::" << freezeType << "&, const ::Ice::CommunicatorPtr&);";
    H << nl << "static void read(" << typeToString(type, false, metaData) << "&, const Freeze::" << freezeType << "&, "
      << "const ::Ice::CommunicatorPtr&);";
    H << nl << "static const std::string& typeId();";
    H << eb << ';';
}

void
writeCodecC(const TypePtr& type, const StringList& metaData, const string& name, const string& freezeType, bool encaps,
            Output& C)
{
    string quotedFreezeType = "\"" + freezeType + "\"";

    C << sp << nl << "void" << nl << name << "::write(" << inputTypeToString(type, false, metaData) << " v, "
      << "Freeze::" << freezeType << "& bytes, const ::Ice::CommunicatorPtr& communicator)";
    C << sb;
    C << nl << "IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);";
    C << nl << "IceInternal::BasicStream stream(instance.get());";
    if(encaps)
    {
        C << nl << "stream.startWriteEncaps();";
    }
    writeMarshalUnmarshalCode(C, type, "v", true, "stream", false, metaData);
    if(type->usesClasses())
    {
        C << nl << "stream.writePendingObjects();";
    }
    if(encaps)
    {
        C << nl << "stream.endWriteEncaps();";
    }
    C << nl << "::std::vector<Ice::Byte>(stream.b.begin(), stream.b.end()).swap(bytes);";
    C << eb;

    C << sp << nl << "void" << nl << name << "::read(" << typeToString(type, false, metaData) << "& v, "
      << "const Freeze::" << freezeType << "& bytes, const ::Ice::CommunicatorPtr& communicator)";
    C << sb;
    C << nl << "IceInternal::InstancePtr instance = IceInternal::getInstance(communicator);";
    C << nl << "IceInternal::BasicStream stream(instance.get());";
    if(type->usesClasses())
    {
        C << nl << "stream.sliceObjects(false);";
    }
    C << nl << "stream.b.resize(bytes.size());";
    C << nl << "::memcpy(&stream.b[0], &bytes[0], bytes.size());";
    C << nl << "stream.i = stream.b.begin();";
    if(encaps)
    {
        C << nl << "stream.startReadEncaps();";
    }
    writeMarshalUnmarshalCode(C, type, "v", false, "stream", false, metaData);
    if(type->usesClasses())
    {
        C << nl << "stream.readPendingObjects();";
    }
    if(encaps)
    {
        C << nl << "stream.endReadEncaps();";
    }
    C << eb;

    string staticName = "__";
    for(string::const_iterator p = name.begin(); p != name.end(); ++p)
    {
	if((*p) == ':')
	{
	    staticName += '_';
	}
	else
	{
	    staticName += *p;
	}
    }
    staticName += "_typeId";

    string typeId = type->typeId();
    BuiltinPtr builtInType = BuiltinPtr::dynamicCast(type);
    if(builtInType &&  builtInType->kind() == Builtin::KindString && metaData.size() != 0 && 
       metaData.front() == "cpp:type:wstring")
    {
        typeId = "wstring";
    }
    C << sp << nl << "static const ::std::string " << staticName << " = \"" << typeId << "\";";

    C << sp << nl << "const ::std::string&" << nl << name << "::typeId()";
    C << sb;
    C << nl << "return " << staticName << ";";

    C << eb;
}

void
writeDictWithIndicesH(const string& name, const Dict& dict, 
		      const vector<IndexType> indexTypes, 
		      const TypePtr& keyType, const StringList& keyMetaData, const TypePtr& valueType,
		      const StringList& valueMetaData, Output& H, const string& dllExport)
{
    string compare = getCompare(dict, typeToString(keyType, false, keyMetaData));
    
    string templateParams = string("< ") + typeToString(keyType, false, keyMetaData) + ", "
	+ typeToString(valueType, false, valueMetaData) + ", " + name + "KeyCodec, " 
	+ name + "ValueCodec, " + compare + " >";

    string keyCompareParams =
	string("< ") + typeToString(keyType, false, keyMetaData) + ", "
	+ name + "KeyCodec, " + compare + " >";

    vector<string> capitalizedMembers;
    size_t i;
    for(i = 0; i < dict.indices.size(); ++i)
    {
	const string& member = dict.indices[i].member;
	if(!member.empty())
	{
	    string capitalizedMember = member;
	    capitalizedMember[0] = toupper(capitalizedMember[0]);
	    capitalizedMembers.push_back(capitalizedMember);
	}
	else
	{
	    capitalizedMembers.push_back("Value");
	}
    }

    H << sp << nl << "class " << dllExport << name 
      << " : public Freeze::Map" << templateParams;
    H << sb;
    H.dec();
    H << sp << nl << "public:";
    H << sp;
    H.inc();

    //
    // Typedefs
    //
    /*
    H << nl << "typedef std::pair<const " << typeToString(keyType, false, keyMetaData)
      << ", const" << typeToString(valueType, false, valueMetaData) << "> value_type;";

    H << nl << "typedef Freeze::Iterator" << templateParams << " iterator;";
    H << nl << "typedef Freeze::ConstIterator" << templateParams << " const_iterator;";
    H << nl << "typedef size_t size_type;";
    H << nl << "typedef ptrdiff_t difference_type;";
    */

    //
    // Nested index classes
    //

    for(i = 0; i < capitalizedMembers.size(); ++i)
    {
	string className = capitalizedMembers[i] + "Index";

	string indexCompare = 
	    getCompare(dict.indices[i], typeToString(indexTypes[i].type, false, indexTypes[i].metaData));
	
	string indexCompareParams =
	    string("< ") + typeToString(indexTypes[i].type, false, indexTypes[i].metaData) + ", "
	    + className + ", " + indexCompare + " >";

	H << sp << nl << "class " << dllExport << className
	  << " : public Freeze::MapIndex" << indexCompareParams;	
	H << sb;

	H.dec();
	H << sp << nl << "public:";
	H << sp;
	H.inc();
	H << nl << capitalizedMembers[i] << "Index(const std::string&, const " 
	  << indexCompare << "& = " << indexCompare << "());";
	
	H << sp;

	//
	// Codec
	//
	H << nl << "static void write(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << ", Freeze::Key&, const Ice::CommunicatorPtr&);";

	H << nl << "static void read(" 
	  << typeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << "&, const Freeze::Key&, const ::Ice::CommunicatorPtr&);";

	H.dec();
	H << sp << nl << "protected:";
	H << sp;
	H.inc();

	H << nl << "virtual void marshalKey(const Freeze::Value&, Freeze::Key&) const;";
	
	H << eb << ';';
    }

    //
    // Constructors
    //
    H << sp;
    H << nl << name << "(const Freeze::ConnectionPtr&, const std::string&, "
      << "bool = true, const " << compare << "& = " << compare << "());";
    H << sp;
    H << nl << "template <class _InputIterator>"
      << nl << name << "(const Freeze::ConnectionPtr& __connection, "
      << "const std::string& __dbName, bool __createDb, "
      << "_InputIterator __first, _InputIterator __last, "
      << "const " << compare << "& __compare = " << compare << "())";
    H.inc();
    H << nl << ": Freeze::Map" << templateParams <<"(__connection->getCommunicator())";
    H.dec();
    H << sb;
    H << nl << "Freeze::KeyCompareBasePtr __keyCompare = "
      << "new Freeze::KeyCompare" << keyCompareParams << "(__compare, this->_communicator);";
    H << nl << "std::vector<Freeze::MapIndexBasePtr> __indices;";
    for(i = 0; i < capitalizedMembers.size(); ++i)
    {
	string indexName = dict.indices[i].member;
	if(indexName.empty())
	{
	    indexName = "index";
	}
	indexName = string("\"") + indexName + "\"";

	H << nl << "__indices.push_back(new " << capitalizedMembers[i] << "Index(" << indexName << "));";
    }
    H << nl << "this->_helper.reset(Freeze::MapHelper::create(__connection, __dbName, "
      << name + "KeyCodec::typeId(), "
      << name + "ValueCodec::typeId(), __keyCompare, __indices, __createDb));";
    H << nl << "while(__first != __last)";
    H << sb;
    H << nl << "put(*__first);";
    H << nl << "++__first;";
    H << eb;
    H << eb;

    //
    // Find, lowerBound, upperBound, equalRange and count functions
    //
    for(i = 0; i < capitalizedMembers.size(); ++i)
    {
	H << sp;
	H << nl << "iterator findBy" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ", bool = true);";
	H << nl << "const_iterator findBy" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ", bool = true) const;";
	
	H << nl << "iterator lowerBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ");";
	H << nl << "const_iterator lowerBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ") const;";
	
	H << nl << "iterator upperBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ");";
	H << nl << "const_iterator upperBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ") const;";

	H << nl << "std::pair<iterator, iterator> equalRangeFor" 
	  << capitalizedMembers[i] << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) 
	  << ");";
	
	H << nl << "std::pair<const_iterator, const_iterator> equalRangeFor" 
	  << capitalizedMembers[i] << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << ") const;";

	string countFunction = dict.indices[i].member.empty() ? string("valueCount")
	    : dict.indices[i].member + "Count";

	H << nl << "int " << countFunction
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << ") const;";
    }
    
    H << eb << ';';
}

void
writeDictWithIndicesC(const string& name, const string& absolute, const Dict& dict, 
		      const vector<IndexType> indexTypes, 
		      const TypePtr& keyType, const StringList& keyMetaData, const TypePtr& valueType,
		      const StringList& valueMetaData, Output& C)
{ 
    string compare = getCompare(dict, typeToString(keyType, false, keyMetaData));
    
    string templateParams = string("< ") + typeToString(keyType, false, keyMetaData) + ", "
	+ typeToString(valueType, false, valueMetaData) + ", " + name + "KeyCodec, " 
	+ name + "ValueCodec, " + compare + " >";

    string keyCompareParams =
	string("< ") + typeToString(keyType, false, keyMetaData) + ", "
	+ name + "KeyCodec, " + compare + " >";
    
    vector<string> capitalizedMembers;
    size_t i;
    for(i = 0; i < dict.indices.size(); ++i)
    {
	const string& member = dict.indices[i].member;
	if(!member.empty())
	{
	    string capitalizedMember = member;
	    capitalizedMember[0] = toupper(capitalizedMember[0]);
	    capitalizedMembers.push_back(capitalizedMember);
	}
	else
	{
	    capitalizedMembers.push_back("Value");
	}
    }
    

    //
    // Nested index classes
    //
    for(i = 0; i < capitalizedMembers.size(); ++i)
    {
	string className = capitalizedMembers[i] + "Index";

	string indexCompare = 
	    getCompare(dict.indices[i], typeToString(indexTypes[i].type, false, indexTypes[i].metaData));

	string indexCompareParams =
	    string("< ") + typeToString(indexTypes[i].type, false, indexTypes[i].metaData) + ", "
	    + className + ", " + indexCompare + " >";

	C << sp << nl << absolute << "::" << className << "::" << className
	  << "(const std::string& __name, "
	  << "const " << indexCompare << "& __compare)";

	C.inc();
	C << nl << ": Freeze::MapIndex"
	  << indexCompareParams << "(__name, __compare)";
	C.dec();
	C << sb;
	C << eb;

	C << sp << nl << "void" 
	  << nl << absolute << "::" << className << "::" 
	  << "marshalKey(const Freeze::Value& __v, Freeze::Key& __k) const";
	C << sb;
	
	bool optimize = false;

	if(dict.indices[i].member.empty() && dict.indices[i].caseSensitive)
	{
	    optimize = true;
	    C << nl << "__k = __v;";
	}
	else
	{
	    //
	    // Can't optimize
	    //
	    C << nl << typeToString(valueType, false, valueMetaData) << " __x;";
	    C << nl << absolute << "ValueCodec::read(__x, __v, _communicator);";
	    string param = "__x";
	    
	    if(!dict.indices[i].member.empty())
	    {
		if(ClassDeclPtr::dynamicCast(valueType) != 0)
		{
		    param += "->" + dict.indices[i].member;
		}
		else
		{
		    param += "." + dict.indices[i].member;
		}
	    }
	    C << nl << "write(" << param << ", __k, _communicator);";
	}
	C << eb;
	
	C << sp << nl << "void" 
	  << nl << absolute << "::" << className << "::" 
	  << "write(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << " __index, Freeze::Key& __bytes, const Ice::CommunicatorPtr& __communicator)";
	C << sb;
	
	if(optimize)
	{
	    C << nl << absolute << "ValueCodec::write(__index, __bytes, __communicator);";
	}
	else
	{
	    assert(!indexTypes[i].type->usesClasses());

	    C << nl << "IceInternal::InstancePtr __instance = IceInternal::getInstance(__communicator);";
	    C << nl << "IceInternal::BasicStream __stream(__instance.get());";
	    
	    string valueS;
	    if(dict.indices[i].caseSensitive)
	    {
		valueS = "__index";
	    }
	    else
	    {
		C << nl << typeToString(indexTypes[i].type, false, indexTypes[i].metaData) 
		  << " __lowerCaseIndex = __index;";
		C << nl << "std::transform(__lowerCaseIndex.begin(), __lowerCaseIndex.end(), __lowerCaseIndex.begin(), tolower);";
		valueS = "__lowerCaseIndex";
	    }
	    
	    writeMarshalUnmarshalCode(C, indexTypes[i].type, valueS, true, "__stream", false, indexTypes[i].metaData);
	    C << nl << "::std::vector<Ice::Byte>(__stream.b.begin(), __stream.b.end()).swap(__bytes);";
	}
	C << eb;

	C << sp << nl << "void" 
	  << nl << absolute << "::" << className << "::" 
	  << "read(" << typeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << "& __index, const Freeze::Key& __bytes, const Ice::CommunicatorPtr& __communicator)";
	C << sb;
	
	if(optimize)
	{
	    C << nl << absolute << "ValueCodec::read(__index, __bytes, __communicator);";
	}
	else
	{
	    C << nl << "IceInternal::InstancePtr __instance = IceInternal::getInstance(__communicator);";
	    C << nl << "IceInternal::BasicStream __stream(__instance.get());";
	    
	    C << nl << "__stream.b.resize(__bytes.size());";
	    C << nl << "::memcpy(&__stream.b[0], &__bytes[0], __bytes.size());";
	    C << nl << "__stream.i = __stream.b.begin();";
	    writeMarshalUnmarshalCode(C, indexTypes[i].type, "__index", false, "__stream", false, 
	    			      indexTypes[i].metaData);
	}
	C << eb;
    }

    //
    // Constructor
    //
    C << sp << nl << absolute << "::" << name
      << "(const Freeze::ConnectionPtr& __connection, const std::string& __dbName ,"
      << "bool __createDb, const " << compare << "& __compare)";
    
    C.inc();
    C << nl << ": Freeze::Map" << templateParams <<"(__connection->getCommunicator())";
    C.dec();
    C << sb;
    C << nl << "Freeze::KeyCompareBasePtr __keyCompare = "
      << "new Freeze::KeyCompare" << keyCompareParams << "(__compare, _communicator);";
    C << nl << "std::vector<Freeze::MapIndexBasePtr> __indices;";
    for(i = 0; i < capitalizedMembers.size(); ++i)
    {
	string indexName = dict.indices[i].member;
	if(indexName.empty())
	{
	    indexName = "index";
	}
	indexName = string("\"") + indexName + "\"";

	C << nl << "__indices.push_back(new " << capitalizedMembers[i] << "Index(" << indexName << "));";
    }
    C << nl << "_helper.reset(Freeze::MapHelper::create(__connection, __dbName, "
      << absolute + "KeyCodec::typeId(), "
      << absolute + "ValueCodec::typeId(), __keyCompare, __indices, __createDb));";
    C << eb;

    //
    // Find and count functions
    //
    for(i = 0; i < capitalizedMembers.size(); ++i)
    {	
	string indexClassName = capitalizedMembers[i] + "Index";
	
	string indexName = dict.indices[i].member;
	if(indexName.empty())
	{
	    indexName = "index";
	}
	indexName = string("\"") + indexName + "\"";

	C << sp << nl << absolute << "::iterator"
	  << nl << absolute << "::" << "findBy" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << " __index, bool __onlyDups)";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return iterator(_helper->index(" << indexName 
	  << ")->untypedFind(__bytes, false, __onlyDups), _communicator);";
	C << eb;

	C << sp << nl << absolute << "::const_iterator"
	  << nl << absolute << "::" << "findBy" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData)
	  << " __index, bool __onlyDups) const";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return const_iterator(_helper->index(" << indexName 
	  << ")->untypedFind(__bytes, true, __onlyDups), _communicator);";
	C << eb;

	C << sp << nl << absolute << "::iterator"
	  << nl << absolute << "::" << "lowerBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index)";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return iterator(_helper->index(" << indexName 
	  << ")->untypedLowerBound(__bytes, false), _communicator);";
	C << eb;

	C << sp << nl << absolute << "::const_iterator"
	  << nl << absolute << "::" << "lowerBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index) const";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return const_iterator(_helper->index(" << indexName 
	  << ")->untypedLowerBound(__bytes, true), _communicator);";
	C << eb;
	
	C << sp << nl << absolute << "::iterator"
	  << nl << absolute << "::" << "upperBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index)";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return iterator(_helper->index(" << indexName 
	  << ")->untypedUpperBound(__bytes, false), _communicator);";
	C << eb;

	C << sp << nl << absolute << "::const_iterator"
	  << nl << absolute << "::" << "upperBoundFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index) const";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return const_iterator(_helper->index(" << indexName 
	  << ")->untypedUpperBound(__bytes, true), _communicator);";
	C << eb;
	
	C << sp << nl << "std::pair<" << absolute << "::iterator, "
	  << absolute << "::iterator>"
	  << nl << absolute << "::" << "equalRangeFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index)";
	C << sb;
	C << nl << "return std::make_pair(lowerBoundFor" << capitalizedMembers[i]
	  << "(__index), upperBoundFor" << capitalizedMembers[i] << "(__index));";
	C << eb;

	C << sp << nl << "std::pair<" << absolute << "::const_iterator, "
	  << absolute << "::const_iterator>"
	  << nl << absolute << "::" << "equalRangeFor" << capitalizedMembers[i]
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index) const";
	C << sb;
	C << nl << "return std::make_pair(lowerBoundFor" << capitalizedMembers[i]
	  << "(__index), upperBoundFor" << capitalizedMembers[i] << "(__index));";
	C << eb;

	string countFunction = dict.indices[i].member.empty() ? string("valueCount") 
	    : dict.indices[i].member + "Count";

	C << sp << nl << "int"
	  << nl << absolute << "::" << countFunction
	  << "(" << inputTypeToString(indexTypes[i].type, false, indexTypes[i].metaData) << " __index) const";
	C << sb;
	C << nl << "Freeze::Key __bytes;";
	C << nl << indexClassName << "::" << "write(__index, __bytes, _communicator);";
	C << nl << "return _helper->index(" << indexName 
	  << ")->untypedCount(__bytes);";
	C << eb;
    }
}


bool
writeDict(const string& n, UnitPtr& u, const Dict& dict, Output& H, Output& C, const string& dllExport)
{
    string absolute = dict.name;
    if(absolute.find("::") == 0)
    {
	absolute.erase(0, 2);
    }
    string name = absolute;
    vector<string> scope;
    string::size_type pos;
    while((pos = name.find("::")) != string::npos)
    {
	string s = name.substr(0, pos);
	name.erase(0, pos + 2);
	
	if(!checkIdentifier(n, absolute, s))
	{
	    return false;
	}
	
	scope.push_back(s);
    }
    
    if(!checkIdentifier(n, absolute, name))
    {
	return false;
    }

    TypeList keyTypes = u->lookupType(dict.key, false);
    if(keyTypes.empty())
    {
	cerr << n << ": `" << dict.key << "' is not a valid type" << endl;
	return false;
    }
    TypePtr keyType = keyTypes.front();
    
    TypeList valueTypes = u->lookupType(dict.value, false);
    if(valueTypes.empty())
    {
	cerr << n << ": `" << dict.value << "' is not a valid type" << endl;
	return false;
    }
    TypePtr valueType = valueTypes.front();
    
    vector<string>::const_iterator q;
    
    for(q = scope.begin(); q != scope.end(); ++q)
    {
	H << sp;
	H << nl << "namespace " << *q << nl << '{';
    }

    writeCodecH(keyType, dict.keyMetaData, name + "KeyCodec", "Key", H, dllExport);
    writeCodecH(valueType, dict.valueMetaData, name + "ValueCodec", "Value", H, dllExport);

    vector<IndexType> indexTypes;

    if(dict.indices.size() == 0)
    {
	string compare = getCompare(dict, typeToString(keyType, false, dict.keyMetaData));

	H << sp << nl << "typedef Freeze::Map< " << typeToString(keyType, false, dict.keyMetaData) 
	  << ", " << typeToString(valueType, false, dict.valueMetaData) << ", "
	  << name << "KeyCodec, " << name << "ValueCodec, " << compare
	  << " > " << name << ";";
    }
    else
    {
	for(vector<DictIndex>::const_iterator p = dict.indices.begin();
	    p != dict.indices.end(); ++p)
	{
	    const DictIndex& index = *p;
	    if(index.member.empty())
	    {
		if(dict.indices.size() > 1)
		{
		    cerr << n << ": bad index for dictionary `" << dict.name << "'" << endl;
		    return false;
		}
		
		if(!Dictionary::legalKeyType(valueType))
		{
		    cerr << n << ": `" << dict.value << "' is not a valid index type" << endl;
		    return false; 
		}


		if(index.caseSensitive == false)
		{
		    //
		    // Let's check value is a string
		    //
		
		    BuiltinPtr builtInType = BuiltinPtr::dynamicCast(valueType);
		    
		    if(builtInType == 0 || builtInType->kind() != Builtin::KindString)
		    {
			cerr << n << ": VALUE is a `" << dict.value << "', not a string" << endl;
			return false; 
		    }
		}
		IndexType iType;
		iType.type = valueType;
		iType.metaData = dict.valueMetaData;
		indexTypes.push_back(iType);
	    }
	    else
	    {
		DataMemberPtr dataMember = 0;
		DataMemberList dataMembers;
		
		ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(valueType);
		if(classDecl != 0)
		{
		    dataMembers = classDecl->definition()->allDataMembers();
		}
		else
		{
		    StructPtr structDecl = StructPtr::dynamicCast(valueType);
		    if(structDecl == 0)
		    {
			cerr << n << ": `" << dict.value << "' is neither a class nor a struct." << endl;
			return false;
		    }
		    dataMembers = structDecl->dataMembers();
		}
		DataMemberList::const_iterator d = dataMembers.begin();
		while(d != dataMembers.end() && dataMember == 0)
		{
		    if((*d)->name() == index.member)
		    {
			dataMember = *d;
		    }
		    else
		    {
			++d;
		    }
		}
		
		if(dataMember == 0)
		{
		    cerr << n << ": The value of `" << dict.name 
			 << "' has no data member named `" << index.member << "'" << endl;
		    return false;
		}
		
		TypePtr dataMemberType = dataMember->type();

		if(!Dictionary::legalKeyType(dataMemberType))
		{
		    cerr << n << ": `" << index.member << "' cannot be used as an index" << endl;
		    return false; 
		}

		if(index.caseSensitive == false)
		{
		    //
		    // Let's check member is a string
		    //
		    BuiltinPtr memberType = BuiltinPtr::dynamicCast(dataMemberType);
		    if(memberType == 0 || memberType->kind() != Builtin::KindString)
		    {
			cerr << n << ": `" << index.member << "' is not a string " << endl;
			return false;
		    }
		}
		IndexType iType;
		iType.type = dataMemberType;
		indexTypes.push_back(iType);
	    }
	}
	writeDictWithIndicesH(name, dict, indexTypes, keyType, dict.keyMetaData, valueType, dict.valueMetaData, H, 
			      dllExport);
    }


    for(q = scope.begin(); q != scope.end(); ++q)
    {
	H << sp;
	H << nl << '}';
    }

    writeCodecC(keyType, dict.keyMetaData, absolute + "KeyCodec", "Key", false, C);
    writeCodecC(valueType, dict.valueMetaData, absolute + "ValueCodec", "Value", true, C);
    
    if(indexTypes.size() > 0)
    {
	writeDictWithIndicesC(name, absolute, dict, indexTypes, keyType, dict.keyMetaData, valueType,
			      dict.valueMetaData, C);
    }

    return true;
}


void
writeIndexH(const string& memberTypeString, const string& name, Output& H, const string& dllExport)
{
    H << sp << nl << "class " << dllExport << name
      << " : public Freeze::Index";
    H << sb;
    H.dec();
    H << sp << nl << "public:";
    H << sp;
    H.inc();
    
    H << nl << name << "(const std::string&, const std::string& = \"\");";
    H << sp << nl << "std::vector<Ice::Identity>";
    H << nl << "findFirst(" << memberTypeString << ", Ice::Int) const;";

    H << sp << nl << "std::vector<Ice::Identity>";
    H << nl << "find(" << memberTypeString << ") const;";

    H << sp << nl << "Ice::Int";
    H << nl << "count(" << memberTypeString << ") const;";
    H.dec();
    H << sp << nl << "private:";
    H << sp;
    H.inc();
    
    H << nl << "virtual bool";
    H << nl << "marshalKey(const Ice::ObjectPtr&, Freeze::Key&) const;";
    
    H << sp << nl << "void";
    H << nl << "marshalKey(" << memberTypeString << ", Freeze::Key&) const;";
    
    H << eb << ';';
    H << sp;
    H << nl << "typedef IceUtil::Handle<" << name << "> " << name << "Ptr;";
}

void
writeIndexC(const TypePtr& type, const TypePtr& memberType, const string& memberName,
	    bool caseSensitive, const string& fullName, const string& name, Output& C)
{
    string inputType = inputTypeToString(memberType, false);

    C << sp << nl << fullName << "::" << name 
      << "(const ::std::string& __name, const ::std::string& __facet)";
    C.inc();
    C << nl << ": Freeze::Index(__name, __facet)";
    C.dec();
    C << sb;
    C << eb;

    C << sp << nl << "std::vector<Ice::Identity>";
    C << nl << fullName << "::" << "findFirst(" << inputType << " __index, ::Ice::Int __firstN) const";
    C << sb;
    C << nl << "Freeze::Key __bytes;";
    C << nl << "marshalKey(__index, __bytes);";
    C << nl << "return untypedFindFirst(__bytes, __firstN);";
    C << eb;

    C << sp << nl << "std::vector<Ice::Identity>";
    C << nl << fullName << "::" << "find(" << inputType << " __index) const";
    C << sb;
    C << nl << "Freeze::Key __bytes;";
    C << nl << "marshalKey(__index, __bytes);";
    C << nl << "return untypedFind(__bytes);";
    C << eb;

    C << sp << nl << "Ice::Int";
    C << nl << fullName << "::" << "count(" << inputType << " __index) const";
    C << sb;
    C << nl << "Freeze::Key __bytes;";
    C << nl << "marshalKey(__index, __bytes);";
    C << nl << "return untypedCount(__bytes);";
    C << eb;

    string typeString = typeToString(type, false);
    
    C << sp << nl << "bool";
    C << nl << fullName << "::" << "marshalKey(const Ice::ObjectPtr& __servant, Freeze::Key& __bytes) const";
    C << sb;
    C << nl << typeString << " __s = " << typeString << "::dynamicCast(__servant);";
    C << nl << "if(__s != 0)";
    C << sb;
    C << nl << "marshalKey(__s->" << memberName << ", __bytes);";
    C << nl << "return true;";
    C << eb;
    C << nl << "else";
    C << sb;
    C << nl << "return false;";
    C << eb;
    C << eb;
    
    C << sp << nl << "void";
    C << nl << fullName << "::" << "marshalKey(" << inputType << " __index, Freeze::Key& __bytes) const";
    C << sb;
    C << nl << "IceInternal::InstancePtr __instance = IceInternal::getInstance(_communicator);";
    C << nl << "IceInternal::BasicStream __stream(__instance.get());";
    
    string valueS;
    if(caseSensitive)
    {
	valueS = "__index";
    }
    else
    {
	C << nl << typeToString(memberType, false) << " __lowerCaseIndex = __index;";
	C << nl << "std::transform(__lowerCaseIndex.begin(), __lowerCaseIndex.end(), __lowerCaseIndex.begin(), tolower);";
	valueS = "__lowerCaseIndex";
    }

    writeMarshalUnmarshalCode(C, memberType, valueS, true, "__stream", false);
    if(memberType->usesClasses())
    {
        C << nl << "__stream.writePendingObjects();";
    }
    C << nl << "::std::vector<Ice::Byte>(__stream.b.begin(), __stream.b.end()).swap(__bytes);";
    C << eb;
}

bool
writeIndex(const string& n, UnitPtr& u, const Index& index, Output& H, Output& C, const string& dllExport)
{
    string absolute = index.name;
    if(absolute.find("::") == 0)
    {
	absolute.erase(0, 2);
    }
    string name = absolute;
    vector<string> scope;
    string::size_type pos;
    while((pos = name.find("::")) != string::npos)
    {
	string s = name.substr(0, pos);
	name.erase(0, pos + 2);
	
	if(!checkIdentifier(n, absolute, s))
	{
	    return false;
	}
	
	scope.push_back(s);
    }
    
    if(!checkIdentifier(n, absolute, name))
    {
	return false;
    }

    TypeList types = u->lookupType(index.type, false);
    if(types.empty())
    {
	cerr << n << ": `" << index.type << "' is not a valid type" << endl;
	return false;
    }
    TypePtr type = types.front();
    
    ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(type);
    if(classDecl == 0)
    {
	cerr << n << ": `" << index.type << "' is not a class" << endl;
        return false;
    }

    DataMemberList dataMembers = classDecl->definition()->allDataMembers();
    DataMemberPtr dataMember = 0;
    DataMemberList::const_iterator p = dataMembers.begin();
    while(p != dataMembers.end() && dataMember == 0)
    {
	if((*p)->name() == index.member)
	{
	    dataMember = *p;
	}
	else
	{
	    ++p;
	}
    }

    if(dataMember == 0)
    {
	cerr << n << ": `" << index.type << "' has no data member named `" << index.member << "'" << endl;
        return false;
    }
    
    if(index.caseSensitive == false)
    {
	//
	// Let's check member is a string
	//
	BuiltinPtr memberType = BuiltinPtr::dynamicCast(dataMember->type());
	if(memberType == 0 || memberType->kind() != Builtin::KindString)
	{
	    cerr << n << ": `" << index.member << "'is not a string " << endl;
	    return false; 
	}
    }
   
    vector<string>::const_iterator q;
    
    for(q = scope.begin(); q != scope.end(); ++q)
    {
	H << sp;
	H << nl << "namespace " << *q << nl << '{';
    }

    writeIndexH(inputTypeToString(dataMember->type(), false), name, H, dllExport);
    
    for(q = scope.begin(); q != scope.end(); ++q)
    {
	H << sp;
	H << nl << '}';
    }

    writeIndexC(type, dataMember->type(), index.member, index.caseSensitive, absolute, name, C);
    return true;
}

int
main(int argc, char* argv[])
{
    string cppArgs;
    string headerExtension;
    string sourceExtension;
    vector<string> includePaths;
    bool preprocess;
    string include;
    string dllExport;
    vector<Dict> dicts;
    vector<Index> indices;
    string output;
    bool debug = false;
    bool ice = false;
    bool caseSensitive = false;

    IceUtil::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("", "header-ext", IceUtil::Options::NeedArg, "h");
    opts.addOpt("", "source-ext", IceUtil::Options::NeedArg, "cpp");
    opts.addOpt("", "add-header", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("D", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("U", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("I", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("E");
    opts.addOpt("", "include-dir", IceUtil::Options::NeedArg);
    opts.addOpt("", "dll-export", IceUtil::Options::NeedArg);
    opts.addOpt("", "dict", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "index", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "dict-index", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "output-dir", IceUtil::Options::NeedArg);
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("", "case-sensitive");
     
    vector<string> args;
    try
    {
        args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtil::Options::BadOpt& e)
    {
	cerr << argv[0] << ": " << e.reason << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    if(opts.isSet("h") || opts.isSet("help"))
    {
	usage(argv[0]);
	return EXIT_SUCCESS;
    }
    if(opts.isSet("v") || opts.isSet("version"))
    {
	cout << ICE_STRING_VERSION << endl;
	return EXIT_SUCCESS;
    }

    headerExtension = opts.optArg("header-ext");
    sourceExtension = opts.optArg("source-ext");

    vector<string> extraHeaders = opts.argVec("add-header");

    if(opts.isSet("D"))
    {
	vector<string> optargs = opts.argVec("D");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cppArgs += " -D\"" + *i + "\"";
	}
    }
    if(opts.isSet("U"))
    {
	vector<string> optargs = opts.argVec("U");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cppArgs += " -U\"" + *i + "\"";
	}
    }
    if(opts.isSet("I"))
    {
	includePaths = opts.argVec("I");
	for(vector<string>::const_iterator i = includePaths.begin(); i != includePaths.end(); ++i)
	{
	    cppArgs += " -I\"" + *i + "\"";
	}
    }
    preprocess= opts.isSet("E");
    if(opts.isSet("include-dir"))
    {
	include = opts.optArg("include-dir");
    }
    if(opts.isSet("dll-export"))
    {
	dllExport = opts.optArg("dll-export");
    }
    if(opts.isSet("dict"))
    {
	vector<string> optargs = opts.argVec("dict");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    string s = *i;
	    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
	    
	    Dict dict;

	    string::size_type pos;
	    pos = s.find(',');
	    if(pos != string::npos)
	    {
		dict.name = s.substr(0, pos);
		s.erase(0, pos + 1);
	    }
	    pos = s.find(',');
	    if(pos != string::npos)
	    {
	        if(s.find("[\"") == 0)
		{
		    string::size_type end = s.find("\"]");
		    if(end != string::npos && end < pos)
		    {
		        dict.key = s.substr(end + 2, pos - end - 2);
			dict.keyMetaData.push_back(s.substr(2, end - 2));
		    }
		    else
		    {
		        dict.key = s.substr(0, pos);
		    }
		}
		else
		{
		    dict.key = s.substr(0, pos);
		}
		s.erase(0, pos + 1);
	    }
	    pos = s.find(',');
	    if(pos == string::npos)
	    {
	        if(s.find("[\"") == 0)
		{
		    string::size_type end = s.find("\"]");
		    if(end != string::npos)
		    {
		        dict.value = s.substr(end + 2);
			dict.valueMetaData.push_back(s.substr(2, end - 2));
		    }
		    else
		    {
		        dict.value = s;
		    }
		}
		else
		{
		    dict.value = s;
		}
		dict.sort = false;
	    }
	    else
	    {
	        if(s.find("[\"") == 0)
		{
		    string::size_type end = s.find("\"]");
		    if(end != string::npos && end < pos)
		    {
		        dict.value = s.substr(end + 2, pos - end - 2);
			dict.valueMetaData.push_back(s.substr(2, end - 2));
		    }
		    else
		    {
		        dict.value = s.substr(0, pos);
		    }
		}
		else
		{
		    dict.value = s.substr(0, pos);
		}
		s.erase(0, pos + 1);

		pos = s.find(',');
		if(pos == string::npos)
		{
		    if(s != "sort")
		    {
			cerr << argv[0] << ": " << *i 
			     << ": nothing or ',sort' expected after value-type" << endl;
			usage(argv[0]);
			return EXIT_FAILURE;
		    }
		    dict.sort = true;
		}
		else
		{
		    string sort = s.substr(0, pos);
		    s.erase(0, pos + 1);
		    if(sort != "sort")
		    {
			cerr << argv[0] << ": " << *i 
			     << ": nothing or ',sort' expected after value-type" << endl;
			usage(argv[0]);
			return EXIT_FAILURE;
		    }
		    dict.sort = true;
		    dict.userCompare = s;
		}
	    }

	    if(dict.name.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no name specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }

	    if(dict.key.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no key specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }

	    if(dict.value.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no value specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }

	    dicts.push_back(dict);
	}
    }
    if(opts.isSet("index"))
    {
	vector<string> optargs = opts.argVec("index");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    string s = *i;
	    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
	    
	    Index index;

	    string::size_type pos;
	    pos = s.find(',');
	    if(pos != string::npos)
	    {
		index.name = s.substr(0, pos);
		s.erase(0, pos + 1);
	    }
	    pos = s.find(',');
	    if(pos != string::npos)
	    {
		index.type = s.substr(0, pos);
		s.erase(0, pos + 1);
	    }
	    pos = s.find(',');
	    string caseString;
	    if(pos != string::npos)
	    {
		index.member = s.substr(0, pos);
		s.erase(0, pos + 1);
		caseString = s;
	    }
	    else
	    {
		index.member = s;
		caseString = "case-sensitive";
	    }

	    if(index.name.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no name specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }

	    if(index.type.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no type specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }

	    if(index.member.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no member specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }
	    
	    if(caseString != "case-sensitive" && caseString != "case-insensitive")
	    {
		cerr << argv[0] << ": " << *i << ": the case can be `case-sensitive' or `case-insensitive'" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }
	    index.caseSensitive = (caseString == "case-sensitive");

	    indices.push_back(index);
	}
    }
    if(opts.isSet("dict-index"))
    {
	vector<string> optargs = opts.argVec("dict-index");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    string s = *i;
	    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
	    
	    string dictName;
	    DictIndex index;
	    index.sort = false;
	    index.caseSensitive = true;

	    string::size_type pos = s.find(',');
	    if(pos == string::npos)
	    {
		dictName = s;
	    }
	    else
	    {
		dictName = s.substr(0, pos);
		s.erase(0, pos + 1);

		bool done = false;
		while(!done)
		{
		    pos = s.find(',');
		    if(pos == string::npos)
		    {
			if(s == "sort")
			{
			    index.sort = true;
			}
			else if(s == "case-sensitive")
			{
			    index.caseSensitive = true;
			}
			else if(s == "case-insensitive")
			{
			    index.caseSensitive = false;
			}
			else if(index.member.empty())
			{
			    if(s == "\\sort")
			    {
				index.member = "sort";
			    }
			    else
			    {
				index.member = s;
			    }
			}
			else
			{
			    cerr << argv[0] << ": " << *i << ": syntax error" << endl;
			    usage(argv[0]);
			    return EXIT_FAILURE;
			}
			done = true;
		    }
		    else
		    {
			string subs = s.substr(0, pos);
			s.erase(0, pos + 1);
			
			if(subs == "sort")
			{
			    index.sort = true;
			    index.userCompare = s;
			    done = true;
			}
			else if(subs == "case-sensitive")
			{
			    index.caseSensitive = true;
			}
			else if(subs == "case-insensitive")
			{
			    index.caseSensitive = false;
			}
			else if(index.member.empty())
			{
			    if(subs == "\\sort")
			    {
				index.member = "sort";
			    }
			    else
			    {
				index.member = subs;
			    }
			}
			else
			{
			    cerr << argv[0] << ": " << *i << ": syntax error" << endl;
			    usage(argv[0]);
			    return EXIT_FAILURE;
			}
		    }
		}
	    }
		
	    if(dictName.empty())
	    {
		cerr << argv[0] << ": " << *i << ": no dictionary specified" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }

	    bool found = false;
	    for(vector<Dict>::iterator p = dicts.begin(); p != dicts.end(); ++p)
	    {
		if(p->name == dictName)
		{
		    if(find(p->indices.begin(), p->indices.end(), index) != p->indices.end())
		    {
			cerr << argv[0] << ": --dict-index " << *i 
			     << ": this dict-index is defined twice" << endl;
			return EXIT_FAILURE;
		    }
		    p->indices.push_back(index);
		    found = true;
		    break;
		}
	    }
	    if(!found)
	    {
		cerr << argv[0] << ": " << *i << ": unknown dictionary" << endl;
		usage(argv[0]);
		return EXIT_FAILURE;
	    }
	}
    }
    if(opts.isSet("output-dir"))
    {
	output = opts.optArg("output-dir");
    }
    debug = opts.isSet("d") || opts.isSet("debug");
    ice = opts.isSet("ice");
    caseSensitive = opts.isSet("case-sensitive");

    if(dicts.empty() && indices.empty())
    {
	cerr << argv[0] << ": no Freeze types specified" << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    if(args.empty())
    {
	cerr << argv[0] << ": no file name base specified" << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    string fileH = args[0];
    fileH += "." + headerExtension;
    string includeH = fileH;
    string fileC = args[0];
    fileC += "." + sourceExtension;
    if(!output.empty())
    {
	fileH = output + '/' + fileH;
	fileC = output + '/' + fileC;
    }

    UnitPtr u = Unit::createUnit(true, false, ice, caseSensitive);

    StringList includes;

    int status = EXIT_SUCCESS;

    for(vector<string>::size_type idx = 1; idx < args.size(); ++idx)
    {
	Preprocessor icecpp(argv[0], args[idx], cppArgs);

        //
        // Add an include file for each Slice file. Note that the .h extension
        // is replaced with headerExtension later.
        //
	includes.push_back(icecpp.getBaseName() + ".h");

	FILE* cppHandle = icecpp.preprocess(false);

	if(cppHandle == 0)
	{
	    u->destroy();
	    return EXIT_FAILURE;
	}

	if(preprocess)
	{
	    char buf[4096];
	    while(fgets(buf, static_cast<int>(sizeof(buf)), cppHandle) != NULL)
	    {
		if(fputs(buf, stdout) == EOF)
		{
		    u->destroy();
		    return EXIT_FAILURE;
		}
	    }
	}
	else
	{
	    status = u->parse(cppHandle, debug);
	}

	if(!icecpp.close())
	{
	    u->destroy();
	    return EXIT_FAILURE;	    
	}
    }

    if(status == EXIT_SUCCESS && !preprocess)
    {
	u->mergeModules();
	u->sort();

	{
	    for(vector<string>::iterator p = includePaths.begin(); p != includePaths.end(); ++p)
	    {
		if(p->length() && (*p)[p->length() - 1] != '/')
		{
		    *p += '/';
		}
	    }
	}

	Output H;
	H.open(fileH.c_str());
	if(!H)
	{
	    cerr << argv[0] << ": can't open `" << fileH << "' for writing: " << strerror(errno) << endl;
	    u->destroy();
	    return EXIT_FAILURE;
	}
	printHeader(H);
	printFreezeTypes(H, dicts, indices);

	Output C;
	C.open(fileC.c_str());
	if(!C)
	{
	    cerr << argv[0] << ": can't open `" << fileC << "' for writing: " << strerror(errno) << endl;
	    u->destroy();
	    return EXIT_FAILURE;
	}
	printHeader(C);
	printFreezeTypes(C, dicts, indices);

	for(vector<string>::const_iterator i = extraHeaders.begin(); i != extraHeaders.end(); ++i)
	{
	    string hdr = *i;
	    string guard;
	    string::size_type pos = hdr.rfind(',');
	    if(pos != string::npos)
	    {
		hdr = i->substr(0, pos);
		guard = i->substr(pos + 1);
	    }
	    if(!guard.empty())
	    {
		C << "\n#ifndef " << guard;
		C << "\n#define " << guard;
	    }
	    C << "\n#include <";
	    if(!include.empty())
	    {
		C << include << '/';
	    }
	    C << hdr << '>';
	    if(!guard.empty())
	    {
		C << "\n#endif";
	    }
	}

	string s = fileH;
	transform(s.begin(), s.end(), s.begin(), ToIfdef());
	H << "\n#ifndef __" << s << "__";
	H << "\n#define __" << s << "__";
	H << '\n';
	
	if(dicts.size() > 0)
	{
	    H << "\n#include <Freeze/Map.h>";
	}

	if(indices.size() > 0)
	{
	    H << "\n#include <Freeze/Index.h>";
	}

	
	{
	    for(StringList::const_iterator p = includes.begin(); p != includes.end(); ++p)
	    {
		H << "\n#include <" << changeInclude(*p, includePaths) << "." + headerExtension + ">";
	    }
	}

        C << "\n#include <Ice/BasicStream.h>";
	C << "\n#include <";
	if(include.size())
	{
	    C << include << '/';
	}
	C << includeH << '>';

	printVersionCheck(H);
	printVersionCheck(C);

	printDllExportStuff(H, dllExport);
	if(dllExport.size())
	{
	    dllExport += " ";
	}

	{
	    for(vector<Dict>::const_iterator p = dicts.begin(); p != dicts.end(); ++p)
	    {
		try
		{
		    if(!writeDict(argv[0], u, *p, H, C, dllExport))
		    {
			u->destroy();
			return EXIT_FAILURE;
		    }
		}
		catch(...)
		{
		    cerr << argv[0] << ": unknown exception" << endl;
		    u->destroy();
		    return EXIT_FAILURE;
		}
	    } 


	    for(vector<Index>::const_iterator q = indices.begin(); q != indices.end(); ++q)
	    {
		try
		{
		    if(!writeIndex(argv[0], u, *q, H, C, dllExport))
		    {
			u->destroy();
			return EXIT_FAILURE;
		    }
		}
		catch(...)
		{
		    cerr << argv[0] << ": unknown exception" << endl;
		    u->destroy();
		    return EXIT_FAILURE;
		}
	    }

	}

	H << "\n\n#endif\n";
	C << '\n';
    }
    
    u->destroy();

    return status;
}
