// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_PARSER_H
#define ICE_GRID_PARSER_H

#include <IceUtil/Handle.h>
#include <IceGrid/Admin.h>
#include <IceGrid/Query.h>
#include <list>
#include <stdio.h>

#ifdef _WIN32
#   include <io.h>
#   ifdef _MSC_VER
#     define isatty _isatty
#     define fileno _fileno
// '_isatty' : inconsistent dll linkage.  dllexport assumed.
#       pragma warning( disable : 4273 )
#   endif
#endif

//
// Stuff for flex and bison
//

#define YYSTYPE std::list<std::string>
#define YY_DECL int yylex(YYSTYPE* yylvalp)
YY_DECL;
int yyparse();

//
// I must set the initial stack depth to the maximum stack depth to
// disable bison stack resizing. The bison stack resizing routines use
// simple malloc/alloc/memcpy calls, which do not work for the
// YYSTYPE, since YYSTYPE is a C++ type, with constructor, destructor,
// assignment operator, etc.
//
#define YYMAXDEPTH  10000 // 10000 should suffice. Bison default is 10000 as maximum.
#define YYINITDEPTH YYMAXDEPTH // Initial depth is set to max depth, for the reasons described above.

//
// Newer bison versions allow to disable stack resizing by defining
// yyoverflow.
//
#define yyoverflow(a, b, c, d, e, f) yyerror(a)

namespace IceGrid
{

class Parser;
typedef ::IceUtil::Handle<Parser> ParserPtr;

}

namespace IceGrid
{

class Parser : public ::IceUtil::SimpleShared
{
public:

    static ParserPtr createParser(const Ice::CommunicatorPtr&, const IceGrid::AdminPrx&, const IceGrid::QueryPrx&);

    void usage();

    void addApplication(const std::list<std::string>&);
    void removeApplication(const std::list<std::string>&);
    void describeApplication(const std::list<std::string>&);
    void diffApplication(const std::list<std::string>&);
    void updateApplication(const std::list<std::string>&);
    void patchApplication(const std::list<std::string>&);
    void listAllApplications();

    void describeServerTemplate(const std::list<std::string>&);
    void describeServiceTemplate(const std::list<std::string>&);
    void instantiateServerTemplate(const std::list<std::string>&);

    void describeNode(const std::list<std::string>&);
    void pingNode(const std::list<std::string>&);
    void printLoadNode(const std::list<std::string>&);
    void shutdownNode(const std::list<std::string>&);
    void removeNode(const std::list<std::string>&);
    void listAllNodes();

    void removeServer(const std::list<std::string>&);
    void startServer(const std::list<std::string>&);
    void stopServer(const std::list<std::string>&);
    void patchServer(const std::list<std::string>&);
    void signalServer(const std::list<std::string>&);
    void writeMessage(const std::list<std::string>&, int fd);
    void describeServer(const std::list<std::string>&);
    void stateServer(const std::list<std::string>&);
    void enableServer(const std::list<std::string>&, bool);
    void pidServer(const std::list<std::string>&);
    void listAllServers();

    void endpointsAdapter(const std::list<std::string>&);
    void removeAdapter(const std::list<std::string>&);
    void listAllAdapters();

    void addObject(const std::list<std::string>&);
    void removeObject(const std::list<std::string>&);
    void findObject(const std::list<std::string>&);
    void describeObject(const std::list<std::string>&);
    void listObject(const std::list<std::string>&);

    void shutdown();

    void showBanner();
    void showCopying();
    void showWarranty();

    void getInput(char*, int&, int);
    void nextLine();
    void continueLine();
    const char* getPrompt();
    void scanPosition(const char*);

    void invalidCommand(const char*);
    void invalidCommand(const std::string&);
    void patchFailed(const Ice::StringSeq&);
    void error(const char*);
    void error(const std::string&);

    void warning(const char*);
    void warning(const std::string&);

    int parse(FILE*, bool);
    int parse(const std::string&, bool);

private:

    Parser(const Ice::CommunicatorPtr&, const IceGrid::AdminPrx&, const IceGrid::QueryPrx&);
    void exception(const Ice::Exception&);

    std::string _commands;
    Ice::CommunicatorPtr _communicator;
    IceGrid::AdminPrx _admin;
    IceGrid::QueryPrx _query;
    bool _continue;
    int _errors;
    int _currentLine;
    std::string _currentFile;
};

extern Parser* parser; // The current parser for bison/flex

}

#endif
