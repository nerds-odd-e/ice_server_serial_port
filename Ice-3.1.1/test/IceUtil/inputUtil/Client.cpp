// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Unicode.h>
#include <IceUtil/InputUtil.h>
#include <IceUtil/Options.h>
#include <TestCommon.h>

#include <vector>

using namespace IceUtil;
using namespace std;

#define WS " \f\n\r\t\v"

#if defined(__BCPLUSPLUS__)
Int64 Int64Min = -9223372036854775808i64;
Int64 Int64Max =  9223372036854775807i64;
#elif defined(_MSC_VER)
const Int64 Int64Min = -9223372036854775808i64;
const Int64 Int64Max =  9223372036854775807i64;
#elif defined(ICE_64)
const Int64 Int64Min = -0x7fffffffffffffffL-1L;
const Int64 Int64Max = 0x7fffffffffffffffL;
#else
const Int64 Int64Min = -0x7fffffffffffffffLL-1LL;
const Int64 Int64Max = 0x7fffffffffffffffLL;
#endif

int
main(int, char**)
{
    cout << "testing string-to-64-bit integer conversion... " << flush;

    bool b;
    Int64 result;

    b = stringToInt64("", result);
    test(!b && result == 0);
    b = stringToInt64(WS, result);
    test(!b && result == 0);

    b = stringToInt64("123", result);
    test(b && result == 123);
    b = stringToInt64("+123", result);
    test(b && result == 123);
    b = stringToInt64("-123", result);
    test(b && result == -123);

    b = stringToInt64("0123", result);
    test(b && result == 83);
    b = stringToInt64("+0123", result);
    test(b && result == 83);
    b = stringToInt64("-0123", result);
    test(b && result == -83);

    b = stringToInt64("0x123", result);
    test(b && result == 291);
    b = stringToInt64("+0x123", result);
    test(b && result == 291);
    b = stringToInt64("-0x123", result);
    test(b && result == -291);

    b = stringToInt64(WS "123", result);
    test(b && result == 123);
    b = stringToInt64("123" WS, result);
    test(b && result == 123);
    b = stringToInt64(WS "123" WS, result);
    test(b && result == 123);

    b = stringToInt64("123Q", result);
    test(b && result == 123);
    b = stringToInt64(" 123Q", result);
    test(b && result == 123);
    b = stringToInt64(" 123Q ", result);
    test(b && result == 123);
    b = stringToInt64(" 123 Q", result);
    test(b && result == 123);

    b = stringToInt64("Q", result);
    test(!b && result == 0);
    b = stringToInt64(" Q", result);
    test(!b && result == 0);

    b = stringToInt64("-9223372036854775807", result);
    test(b && result == -ICE_INT64(9223372036854775807));
    b = stringToInt64("-9223372036854775808", result);
    test(b && result == Int64Min);
    b = stringToInt64("-9223372036854775809", result);
    test(!b && result < 0);

    b = stringToInt64("9223372036854775806", result);
    test(b && result == ICE_INT64(9223372036854775806));
    b = stringToInt64("9223372036854775807", result);
    test(b && result == Int64Max);
    b = stringToInt64("9223372036854775808", result);
    test(!b && result > 0);

    b = stringToInt64("-9223372036854775807Q", result);
    test(b && result == -ICE_INT64(9223372036854775807));
    b = stringToInt64("-9223372036854775808Q", result);
    test(b && result == Int64Min);
    b = stringToInt64("-9223372036854775809Q", result);
    test(!b && result < 0);

    b = stringToInt64("-9223372036854775809999Q", result);
    test(!b && result < 0);

    cout << "ok" << endl;

    cout << "testing string to command line arguments... " << flush;
    vector<string> args;

    test(IceUtil::Options::split("").empty());
    
    args = IceUtil::Options::split("\"\"");
    test(args.size() == 1 && args[0] == "");
    args = IceUtil::Options::split("''");
    test(args.size() == 1 && args[0] == "");
    args = IceUtil::Options::split("$''");
    test(args.size() == 1 && args[0] == "");

    args = IceUtil::Options::split("-a -b -c");
    test(args.size() == 3 && args[0] == "-a" && args[1] == "-b" && args[2] == "-c");
    args = IceUtil::Options::split("\"-a\" '-b' $'-c'");
    test(args.size() == 3 && args[0] == "-a" && args[1] == "-b" && args[2] == "-c");
    args = IceUtil::Options::split("  '-b' \"-a\" $'-c' ");
    test(args.size() == 3 && args[0] == "-b" && args[1] == "-a" && args[2] == "-c");
    args = IceUtil::Options::split(" $'-c' '-b' \"-a\"  ");
    test(args.size() == 3 && args[0] == "-c" && args[1] == "-b" && args[2] == "-a");

    // Testing single quote
    args = IceUtil::Options::split("-Dir='C:\\\\test\\\\file'"); // -Dir='C:\\test\\file'
    test(args.size() == 1 && args[0] == "-Dir=C:\\\\test\\\\file"); // -Dir=C:\\test\\file
    args = IceUtil::Options::split("-Dir='C:\\test\\file'"); // -Dir='C:\test\file'
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\file"); // -Dir=C:\test\file
    args = IceUtil::Options::split("-Dir='C:\\test\\filewith\"quote'"); // -Dir='C:\test\filewith"quote'
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\filewith\"quote"); // -Dir=C:\test\filewith"quote

    // Testing double quote
    args = IceUtil::Options::split("-Dir=\"C:\\\\test\\\\file\""); // -Dir="C:\\test\\file"
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\file"); // -Dir=C:\test\file
    args = IceUtil::Options::split("-Dir=\"C:\\test\\file\""); // -Dir="C:\test\file"
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\file"); // -Dir=C:\test\file
    args = IceUtil::Options::split("-Dir=\"C:\\test\\filewith\\\"quote\""); // -Dir="C:\test\filewith\"quote"
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\filewith\"quote"); // -Dir=C:\test\filewith"quote

    // Testing ANSI quote
    args = IceUtil::Options::split("-Dir=$'C:\\\\test\\\\file'"); // -Dir=$'C:\\test\\file'
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\file"); // -Dir=C:\test\file
    args = IceUtil::Options::split("-Dir=$'C:\\oest\\oile'"); // -Dir='C:\oest\oile'
    test(args.size() == 1 && args[0] == "-Dir=C:\\oest\\oile"); // -Dir=C:\oest\oile
    args = IceUtil::Options::split("-Dir=$'C:\\oest\\oilewith\"quote'"); // -Dir=$'C:\oest\oilewith"quote'
    test(args.size() == 1 && args[0] == "-Dir=C:\\oest\\oilewith\"quote"); // -Dir=C:\oest\oilewith"quote
    args = IceUtil::Options::split("-Dir=$'\\103\\072\\134\\164\\145\\163\\164\\134\\146\\151\\154\\145'");
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\file"); // -Dir=C:\test\file
    args = IceUtil::Options::split("-Dir=$'\\x43\\x3A\\x5C\\x74\\x65\\x73\\x74\\x5C\\x66\\x69\\x6C\\x65'");
    test(args.size() == 1 && args[0] == "-Dir=C:\\test\\file"); // -Dir=C:\test\file
    args = IceUtil::Options::split("-Dir=$'\\cM\\c_'"); // Control characters
    test(args.size() == 1 && args[0] == "-Dir=\015\037");
    args = IceUtil::Options::split("-Dir=$'C:\\\\\\146\\x66\\cMi'"); // -Dir=$'C:\\\146\x66i\cMi'
    test(args.size() == 1 && args[0] == "-Dir=C:\\ff\015i");
    args = IceUtil::Options::split("-Dir=$'C:\\\\\\cM\\x66\\146i'"); // -Dir=$'C:\\\cM\x66\146i'
    test(args.size() == 1 && args[0] == "-Dir=C:\\\015ffi");

    vector<string> badQuoteCommands;
    badQuoteCommands.push_back("\"");
    badQuoteCommands.push_back("'");
    badQuoteCommands.push_back("\\$'");
    badQuoteCommands.push_back("-Dir=\"test");
    badQuoteCommands.push_back("-Dir='test");
    badQuoteCommands.push_back("-Dir=$'test");
    for(vector<string>::const_iterator p = badQuoteCommands.begin(); p != badQuoteCommands.end(); ++p)
    {
	try
	{
	    IceUtil::Options::split(*p);
	    test(false);
	}
	catch(const IceUtil::Options::BadQuote&)
	{
	}
    }
    cout << "ok" << endl;

    return EXIT_SUCCESS;
}
