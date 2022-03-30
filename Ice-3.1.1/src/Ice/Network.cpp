// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/StaticMutex.h>
#include <Ice/Network.h>
#include <Ice/LocalException.h>

#if defined(_WIN32)
#  include <winsock2.h>
#elif defined(__linux) || defined(__APPLE__) || defined(__FreeBSD__)
#  include <ifaddrs.h>
#else
#  include <sys/ioctl.h>
#  include <net/if.h>
#  ifdef __sun
#    include <sys/sockio.h>
#  endif
#endif

using namespace std;
using namespace Ice;
using namespace IceInternal;

#ifdef __sun
#    define INADDR_NONE (in_addr_t)0xffffffff
#endif

static IceUtil::StaticMutex inetMutex = ICE_STATIC_MUTEX_INITIALIZER;

string
IceInternal::inetAddrToString(const struct in_addr& in)
{
    //
    // inet_ntoa uses static memory on some platforms so we protect
    // access and make a copy.
    //
    IceUtil::StaticMutex::Lock lock(inetMutex);
    return string(inet_ntoa(in));
}

int
IceInternal::getSocketErrno()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool
IceInternal::interrupted()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEINTR;
#else
#   ifdef EPROTO
    return errno == EINTR || errno == EPROTO;
#   else
    return errno == EINTR;
#   endif
#endif
}

bool
IceInternal::acceptInterrupted()
{
    if(interrupted())
    {
	return true;
    }

#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAECONNABORTED ||
           error == WSAECONNRESET ||
           error == WSAETIMEDOUT;
#else
    return errno == ECONNABORTED ||
           errno == ECONNRESET ||
           errno == ETIMEDOUT;
#endif
}

bool
IceInternal::noBuffers()
{
#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAENOBUFS ||
           error == WSAEFAULT;
#else
    return errno == ENOBUFS;
#endif
}

bool
IceInternal::wouldBlock()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}

bool
IceInternal::connectFailed()
{
#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAECONNREFUSED ||
           error == WSAETIMEDOUT ||
           error == WSAENETUNREACH ||
           error == WSAEHOSTUNREACH ||
           error == WSAECONNRESET ||
           error == WSAESHUTDOWN ||
           error == WSAECONNABORTED;
#else
    return errno == ECONNREFUSED ||
           errno == ETIMEDOUT ||
           errno == ENETUNREACH ||
           errno == EHOSTUNREACH ||
           errno == ECONNRESET ||
           errno == ESHUTDOWN ||
           errno == ECONNABORTED;
#endif
}

bool
IceInternal::connectionRefused()
{
#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAECONNREFUSED;
#else
    return errno == ECONNREFUSED;
#endif
}

bool
IceInternal::connectInProgress()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EINPROGRESS;
#endif
}

bool
IceInternal::connectionLost()
{
#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAECONNRESET ||
           error == WSAESHUTDOWN ||
           error == WSAENOTCONN ||
           error == WSAECONNABORTED;
#else
    return errno == ECONNRESET ||
           errno == ENOTCONN ||
           errno == ESHUTDOWN ||
           errno == ECONNABORTED ||
	   errno == EPIPE;
#endif
}

bool
IceInternal::notConnected()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAENOTCONN;
#else
    return errno == ENOTCONN;
#endif
}

bool
IceInternal::recvTruncated()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEMSGSIZE;
#else
    // We don't get an error under Linux if a datagram is truncated.
    return false;
#endif
}

SOCKET
IceInternal::createSocket(bool udp)
{
    SOCKET fd;

    if(udp)
    {
	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    else
    {
	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    if(fd == INVALID_SOCKET)
    {
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }

    if(!udp)
    {
	setTcpNoDelay(fd);
	setKeepAlive(fd);
    }

    return fd;
}

void
IceInternal::closeSocket(SOCKET fd)
{
#ifdef _WIN32
    int error = WSAGetLastError();
    if(closesocket(fd) == SOCKET_ERROR)
    {
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
    WSASetLastError(error);
#else
    int error = errno;
    if(close(fd) == SOCKET_ERROR)
    {
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
    errno = error;
#endif
}

void
IceInternal::closeSocketNoThrow(SOCKET fd)
{
#ifdef _WIN32
    int error = WSAGetLastError();
    closesocket(fd);
    WSASetLastError(error);
#else
    int error = errno;
    close(fd);
    errno = error;
#endif
}
    
void
IceInternal::shutdownSocketWrite(SOCKET fd)
{
    if(shutdown(fd, SHUT_WR) == SOCKET_ERROR)
    {
	//
	// Ignore errors indicating that we are shutdown already.
	//
#if defined(_WIN32)
	int error = WSAGetLastError();
	if(error == WSAENOTCONN)
	{
	    return;
	}
#elif defined(__APPLE__)
	if(errno == ENOTCONN || errno == EINVAL)
	{
	    return;
	}
#else
	if(errno == ENOTCONN)
	{
	    return;
	}
#endif
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}
    
void
IceInternal::shutdownSocketReadWrite(SOCKET fd)
{
    if(shutdown(fd, SHUT_RDWR) == SOCKET_ERROR)
    {
	//
	// Ignore errors indicating that we are shutdown already.
	//
#if defined(_WIN32)
	int error = WSAGetLastError();
	if(error == WSAENOTCONN)
	{
	    return;
	}
#elif defined(__APPLE__)
	if(errno == ENOTCONN || errno == EINVAL)
	{
	    return;
	}
#else
	if(errno == ENOTCONN)
	{
	    return;
	}
#endif

	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}
    
void
IceInternal::setBlock(SOCKET fd, bool block)
{
    if(block)
    {
#ifdef _WIN32
	unsigned long arg = 0;
	if(ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR)
	{
	    closeSocketNoThrow(fd);
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = WSAGetLastError();
	    throw ex;
	}
#else
	int flags = fcntl(fd, F_GETFL);
	flags &= ~O_NONBLOCK;
	if(fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
	{
	    closeSocketNoThrow(fd);
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = errno;
	    throw ex;
	}
#endif
    }
    else
    {
#ifdef _WIN32
	unsigned long arg = 1;
	if(ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR)
	{
	    closeSocketNoThrow(fd);
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = WSAGetLastError();
	    throw ex;
	}
#else
	int flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;
	if(fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
	{
	    closeSocketNoThrow(fd);
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = errno;
	    throw ex;
	}
#endif
    }
}

void
IceInternal::setTcpNoDelay(SOCKET fd)
{
    int flag = 1;
    if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}
    
void
IceInternal::setKeepAlive(SOCKET fd)
{
    int flag = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}

void
IceInternal::setSendBufferSize(SOCKET fd, int sz)
{
    if(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&sz, int(sizeof(int))) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}

int
IceInternal::getSendBufferSize(SOCKET fd)
{
    int sz;
    socklen_t len = sizeof(sz);
    if(getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&sz, &len) == SOCKET_ERROR || len != sizeof(sz))
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
    return sz;
}

void
IceInternal::setRecvBufferSize(SOCKET fd, int sz)
{
    if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&sz, int(sizeof(int))) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}

int
IceInternal::getRecvBufferSize(SOCKET fd)
{
    int sz;
    socklen_t len = sizeof(sz);
    if(getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&sz, &len) == SOCKET_ERROR || len != sizeof(sz))
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
    return sz;
}

void
IceInternal::doBind(SOCKET fd, struct sockaddr_in& addr)
{
#ifndef _WIN32
    int flag = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
#endif

    if(bind(fd, reinterpret_cast<struct sockaddr*>(&addr), int(sizeof(addr))) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }

    socklen_t len = static_cast<socklen_t>(sizeof(addr));
#ifdef NDEBUG
    getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
#else
    int ret = getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    assert(ret != SOCKET_ERROR);
#endif
}

void
IceInternal::doListen(SOCKET fd, int backlog)
{
repeatListen:
    if(::listen(fd, backlog) == SOCKET_ERROR)
    {
	if(interrupted())
	{
	    goto repeatListen;
	}
	
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}

void
IceInternal::doConnect(SOCKET fd, struct sockaddr_in& addr, int timeout)
{
#ifdef _WIN32
    //
    // Set larger send buffer size to avoid performance problems on
    // WIN32.
    //
    setSendBufferSize(fd, 64 * 1024);

    //
    // Under WinCE its not possible to find out the connection failure
    // reason with SO_ERROR, so its necessary to use the WSAEVENT
    // mechanism. We use the same mechanism for any Winsock platform.
    //
    WSAEVENT event = WSACreateEvent();
    if(event == 0)
    {
	closeSocketNoThrow(fd);

	SocketException ex(__FILE__, __LINE__);
	ex.error = WSAGetLastError();
	throw ex;
    }

    if(WSAEventSelect(fd, event, FD_CONNECT) == SOCKET_ERROR)
    {
	int error = WSAGetLastError();

    	WSACloseEvent(event);
    	closeSocketNoThrow(fd);

	SocketException ex(__FILE__, __LINE__);
	ex.error = error;
	throw ex;
    }
#endif

repeatConnect:
    if(::connect(fd, reinterpret_cast<struct sockaddr*>(&addr), int(sizeof(addr))) == SOCKET_ERROR)
    {
	if(interrupted())
	{
	    goto repeatConnect;
	}
	
	if(connectInProgress())
	{
	    int val;
#ifdef _WIN32
	    WSAEVENT events[1];
	    events[0] = event;
	    long tout = (timeout >= 0) ? timeout : WSA_INFINITE;
	    DWORD rc = WSAWaitForMultipleEvents(1, events, FALSE, tout, FALSE);
	    if(rc == WSA_WAIT_FAILED)
	    {
    	    	int error = WSAGetLastError();

		WSACloseEvent(event);
    	    	closeSocketNoThrow(fd);

		SocketException ex(__FILE__, __LINE__);
		ex.error = error;
		throw ex;
	    }

	    if(rc == WSA_WAIT_TIMEOUT)
	    {
		WSACloseEvent(event);
    	    	closeSocketNoThrow(fd);

		assert(timeout >= 0);
		throw ConnectTimeoutException(__FILE__, __LINE__);
	    }
	    assert(rc == WSA_WAIT_EVENT_0);
	    
	    WSANETWORKEVENTS nevents;
	    if(WSAEnumNetworkEvents(fd, event, &nevents) == SOCKET_ERROR)
	    {
    	    	int error = WSAGetLastError();
		WSACloseEvent(event);
    	    	closeSocketNoThrow(fd);

		SocketException ex(__FILE__, __LINE__);
		ex.error = error;
		throw ex;
	    }

	    //
	    // Now we close the event, because we're finished and
	    // this code be repeated.
	    //
	    WSACloseEvent(event);

	    assert(nevents.lNetworkEvents & FD_CONNECT);
	    val = nevents.iErrorCode[FD_CONNECT_BIT];
#else
	repeatSelect:
	    int ret;
	    fd_set wFdSet;
	    FD_ZERO(&wFdSet);
	    FD_SET(fd, &wFdSet);
	    //
	    // Note that although we use a different mechanism for
	    // WIN32, winsock notifies about connection failures
	    // through the exception filedescriptors
	    //
	    if(timeout >= 0)
	    {
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;
		ret = ::select(fd + 1, 0, &wFdSet, 0, &tv);
	    }
	    else
	    {
		ret = ::select(fd + 1, 0, &wFdSet, 0, 0);
	    }
	    
	    if(ret == 0)
	    {
		closeSocketNoThrow(fd);
		throw ConnectTimeoutException(__FILE__, __LINE__);
	    }
	    else if(ret == SOCKET_ERROR)
	    {
		if(interrupted())
		{
		    goto repeatSelect;
		}
		
		SocketException ex(__FILE__, __LINE__);
		ex.error = getSocketErrno();
		throw ex;
	    }
	    
	    //
	    // Strange windows bug: The following call to Sleep() is
	    // necessary, otherwise no error is reported through
	    // getsockopt.
	    //
	    //Sleep(0);
	    socklen_t len = static_cast<socklen_t>(sizeof(int));
	    if(getsockopt(fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len) == SOCKET_ERROR)
	    {
		closeSocketNoThrow(fd);
		SocketException ex(__FILE__, __LINE__);
		ex.error = getSocketErrno();
		throw ex;
	    }
#endif
	    
	    if(val > 0)
	    {
		closeSocketNoThrow(fd);
#ifdef _WIN32
		WSASetLastError(val);
#else
		errno = val;
#endif
		if(connectionRefused())
		{
		    ConnectionRefusedException ex(__FILE__, __LINE__);
		    ex.error = getSocketErrno();
		    throw ex;
		}
		else if(connectFailed())
		{
		    ConnectFailedException ex(__FILE__, __LINE__);
		    ex.error = getSocketErrno();
		    throw ex;
		}
		else
		{
		    SocketException ex(__FILE__, __LINE__);
		    ex.error = getSocketErrno();
		    throw ex;
		}
	    }
	    
	    return;
	}
    
	closeSocketNoThrow(fd);
	if(connectionRefused())
	{
	    ConnectionRefusedException ex(__FILE__, __LINE__);
	    ex.error = getSocketErrno();
	    throw ex;
	}
	else if(connectFailed())
	{
	    ConnectFailedException ex(__FILE__, __LINE__);
	    ex.error = getSocketErrno();
	    throw ex;
	}
	else
	{
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = getSocketErrno();
	    throw ex;
	}
    }
}

SOCKET
IceInternal::doAccept(SOCKET fd, int timeout)
{
#ifdef _WIN32
    SOCKET ret;
#else
    int ret;
#endif

repeatAccept:
    if((ret = ::accept(fd, 0, 0)) == INVALID_SOCKET)
    {
	if(acceptInterrupted())
	{
	    goto repeatAccept;
	}

	if(wouldBlock())
	{
	repeatSelect:
	    int rs;
	    fd_set fdSet;
	    FD_ZERO(&fdSet);
	    FD_SET(fd, &fdSet);
	    if(timeout >= 0)
	    {
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;
		rs = ::select(static_cast<int>(fd + 1), &fdSet, 0, 0, &tv);
	    }
	    else
	    {
		rs = ::select(static_cast<int>(fd + 1), &fdSet, 0, 0, 0);
	    }
	    
	    if(rs == SOCKET_ERROR)
	    {
		if(interrupted())
		{
		    goto repeatSelect;
		}
		
		SocketException ex(__FILE__, __LINE__);
		ex.error = getSocketErrno();
		throw ex;
	    }
	    
	    if(rs == 0)
	    {
		throw TimeoutException(__FILE__, __LINE__);
	    }
	    
	    goto repeatAccept;
	}
	
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }

    setTcpNoDelay(ret);
    setKeepAlive(ret);

#ifdef _WIN32
    //
    // Set larger send buffer size to avoid performance problems on
    // WIN32.
    //
    setSendBufferSize(ret, 64 * 1024);
#endif
    
    return ret;
}

void
IceInternal::getAddress(const string& host, int port, struct sockaddr_in& addr)
{
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    if(addr.sin_addr.s_addr == INADDR_NONE)
    {
#ifdef _WIN32

	//
	// Windows XP has getaddrinfo(), but we don't want to require XP to run Ice.
	//
	
	//
	// gethostbyname() is thread safe on Windows, with a separate hostent per thread
	//
	struct hostent* entry;
	int retry = 5;
	do
	{
	    entry = gethostbyname(host.c_str());
	}
	while(entry == 0 && WSAGetLastError() == WSATRY_AGAIN && --retry >= 0);
	
	if(entry == 0)
	{
	    DNSException ex(__FILE__, __LINE__);

	    ex.error = WSAGetLastError();
	    ex.host = host;
	    throw ex;
	}
	memcpy(&addr.sin_addr, entry->h_addr, entry->h_length);

#else

	struct addrinfo* info = 0;
	int retry = 5;

	struct addrinfo hints = { 0 };
	hints.ai_family = PF_INET;
	
	int rs = 0;
	do
	{
	    rs = getaddrinfo(host.c_str(), 0, &hints, &info);    
	}
	while(info == 0 && rs == EAI_AGAIN && --retry >= 0);
	
	if(rs != 0)
	{
	    DNSException ex(__FILE__, __LINE__);
	    ex.error = rs;
	    ex.host = host;
	    throw ex;
	}

	assert(info->ai_family == PF_INET);
	struct sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(info->ai_addr);

	addr.sin_addr.s_addr = sin->sin_addr.s_addr;
	freeaddrinfo(info);

#endif
    }
}

bool
IceInternal::compareAddress(const struct sockaddr_in& addr1, const struct sockaddr_in& addr2)
{
    return (addr1.sin_family == addr2.sin_family) &&
           (addr1.sin_port == addr2.sin_port) &&
           (addr1.sin_addr.s_addr == addr2.sin_addr.s_addr);
}

void
IceInternal::createPipe(SOCKET fds[2])
{
#ifdef _WIN32

    SOCKET fd = createSocket(false);
    setBlock(fd, true);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    
    doBind(fd, addr);
    doListen(fd, 1);

    try
    {
	fds[0] = createSocket(false);
    }
    catch(...)
    {
	::closesocket(fd);
	throw;
    }

    try
    {
        setBlock(fds[0], true);
        doConnect(fds[0], addr, -1);
    }
    catch(...)
    {
	::closesocket(fd);
	throw;
    }

    try
    {
	fds[1] = doAccept(fd, -1);
    }
    catch(...)
    {
	::closesocket(fds[0]);
	::closesocket(fd);
	throw;
    }

    ::closesocket(fd);

    try
    {
        setBlock(fds[1], true);
    }
    catch(...)
    {
	::closesocket(fds[0]);
	::closesocket(fd);
	throw;
    }

#else

    if(::pipe(fds) != 0)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }

    try
    {
        setBlock(fds[0], true);
    }
    catch(...)
    {
    	closeSocketNoThrow(fds[1]);
	throw;
    }

    try
    {
        setBlock(fds[1], true);
    }
    catch(...)
    {
    	closeSocketNoThrow(fds[0]);
	throw;
    }

#endif
}

#ifdef _WIN32

string
IceInternal::errorToString(int error)
{
    if(error < WSABASEERR)
    {
	LPVOID lpMsgBuf = 0;
	DWORD ok = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				 FORMAT_MESSAGE_FROM_SYSTEM |
				 FORMAT_MESSAGE_IGNORE_INSERTS,
				 NULL,
				 error,
				 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				 (LPTSTR)&lpMsgBuf,
				 0,
				 NULL);
	if(ok)
	{
	    LPCTSTR msg = (LPCTSTR)lpMsgBuf;
	    assert(msg && strlen((const char*)msg) > 0);
	    string result = (const char*)msg;
	    if(result[result.length() - 1] == '\n')
	    {
	        result = result.substr(0, result.length() - 2);
	    }
	    LocalFree(lpMsgBuf);
	    return result;
	}
	else
	{
	    ostringstream os;
	    os << "unknown error: " << error;
	    return os.str();
	}
    }

    switch(error)
    {
    case WSAEINTR:
	return "WSAEINTR";
	
    case WSAEBADF:
	return "WSAEBADF";
	
    case WSAEACCES:
	return "WSAEACCES";
	
    case WSAEFAULT:
	return "WSAEFAULT";
	
    case WSAEINVAL:
	return "WSAEINVAL";
	
    case WSAEMFILE:
	return "WSAEMFILE";
	
    case WSAEWOULDBLOCK:
	return "WSAEWOULDBLOCK";
	
    case WSAEINPROGRESS:
	return "WSAEINPROGRESS";
	
    case WSAEALREADY:
	return "WSAEALREADY";
	
    case WSAENOTSOCK:
	return "WSAENOTSOCK";
	
    case WSAEDESTADDRREQ:
	return "WSAEDESTADDRREQ";
	
    case WSAEMSGSIZE:
	return "WSAEMSGSIZE";
	
    case WSAEPROTOTYPE:
	return "WSAEPROTOTYPE";
	
    case WSAENOPROTOOPT:
	return "WSAENOPROTOOPT";
	
    case WSAEPROTONOSUPPORT:
	return "WSAEPROTONOSUPPORT";
	
    case WSAESOCKTNOSUPPORT:
	return "WSAESOCKTNOSUPPORT";
	
    case WSAEOPNOTSUPP:
	return "WSAEOPNOTSUPP";
	
    case WSAEPFNOSUPPORT:
	return "WSAEPFNOSUPPORT";
	
    case WSAEAFNOSUPPORT:
	return "WSAEAFNOSUPPORT";
	
    case WSAEADDRINUSE:
	return "WSAEADDRINUSE";
	
    case WSAEADDRNOTAVAIL:
	return "WSAEADDRNOTAVAIL";
	
    case WSAENETDOWN:
	return "WSAENETDOWN";
	
    case WSAENETUNREACH:
	return "WSAENETUNREACH";
	
    case WSAENETRESET:
	return "WSAENETRESET";
	
    case WSAECONNABORTED:
	return "WSAECONNABORTED";
	
    case WSAECONNRESET:
	return "WSAECONNRESET";
	
    case WSAENOBUFS:
	return "WSAENOBUFS";
	
    case WSAEISCONN:
	return "WSAEISCONN";
	
    case WSAENOTCONN:
	return "WSAENOTCONN";
	
    case WSAESHUTDOWN:
	return "WSAESHUTDOWN";
	
    case WSAETOOMANYREFS:
	return "WSAETOOMANYREFS";
	
    case WSAETIMEDOUT:
	return "WSAETIMEDOUT";
	
    case WSAECONNREFUSED:
	return "WSAECONNREFUSED";
	
    case WSAELOOP:
	return "WSAELOOP";
	
    case WSAENAMETOOLONG:
	return "WSAENAMETOOLONG";
	
    case WSAEHOSTDOWN:
	return "WSAEHOSTDOWN";
	
    case WSAEHOSTUNREACH:
	return "WSAEHOSTUNREACH";
	
    case WSAENOTEMPTY:
	return "WSAENOTEMPTY";
	
    case WSAEPROCLIM:
	return "WSAEPROCLIM";
	
    case WSAEUSERS:
	return "WSAEUSERS";
	
    case WSAEDQUOT:
	return "WSAEDQUOT";
	
    case WSAESTALE:
	return "WSAESTALE";
	
    case WSAEREMOTE:
	return "WSAEREMOTE";
	
    case WSAEDISCON:
	return "WSAEDISCON";
	
    case WSASYSNOTREADY:
	return "WSASYSNOTREADY";
	
    case WSAVERNOTSUPPORTED:
	return "WSAVERNOTSUPPORTED";
	
    case WSANOTINITIALISED:
	return "WSANOTINITIALISED";
	
    case WSAHOST_NOT_FOUND:
	return "WSAHOST_NOT_FOUND";
	
    case WSATRY_AGAIN:
	return "WSATRY_AGAIN";
	
    case WSANO_RECOVERY:
	return "WSANO_RECOVERY";
	
    case WSANO_DATA:
	return "WSANO_DATA";

    default:
    {
	ostringstream os;
	os << "unknown socket error: " << error;
	return os.str();
    }
    }
}

string
IceInternal::errorToStringDNS(int error)
{
    return errorToString(error);
}

#else

string
IceInternal::errorToString(int error)
{
    return strerror(error);
}

string
IceInternal::errorToStringDNS(int error)
{
    return gai_strerror(error);
}

#endif

string
IceInternal::lastErrorToString()
{
#ifdef _WIN32
    return errorToString(WSAGetLastError());
#else
    return errorToString(errno);
#endif
}

std::string
IceInternal::fdToString(SOCKET fd)
{
    if(fd == INVALID_SOCKET)
    {
	return "<closed>";
    }

    struct sockaddr_in localAddr;
    fdToLocalAddress(fd, localAddr);

    struct sockaddr_in remoteAddr;
    bool peerConnected = fdToRemoteAddress(fd, remoteAddr);

    ostringstream s;
    s << "local address = " << addrToString(localAddr);
    if(peerConnected)
    {
	s << "\nremote address = " << addrToString(remoteAddr);
    }
    else
    {
	s << "\nremote address = <not connected>";
    }
    return s.str();
}

void
IceInternal::fdToLocalAddress(SOCKET fd, struct sockaddr_in& addr)
{
    socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    if(getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len) == SOCKET_ERROR)
    {
	closeSocketNoThrow(fd);
	SocketException ex(__FILE__, __LINE__);
	ex.error = getSocketErrno();
	throw ex;
    }
}

bool
IceInternal::fdToRemoteAddress(SOCKET fd, struct sockaddr_in& addr)
{
    socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    if(getpeername(fd, reinterpret_cast<struct sockaddr*>(&addr), &len) == SOCKET_ERROR)
    {
	if(notConnected())
	{
	    return false;
	}
	else
	{
	    closeSocketNoThrow(fd);
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = getSocketErrno();
	    throw ex;
	}
    }

    return true;
}

string
IceInternal::addrToString(const struct sockaddr_in& addr)
{
    ostringstream s;
    s << inetAddrToString(addr.sin_addr) << ':' << ntohs(addr.sin_port);
    return s.str();
}

vector<string>
IceInternal::getLocalHosts()
{
    vector<string> result;

#if defined(_WIN32)
    vector<struct sockaddr_in> addrs = getLocalAddresses();
    for(unsigned int i = 0; i < addrs.size(); ++i)
    {
        result.push_back(inetAddrToString(addrs[i].sin_addr));
    }
#elif defined(__linux) || defined(__APPLE__) || defined(__FreeBSD__)
    struct ifaddrs* ifap;
    if(::getifaddrs(&ifap) == SOCKET_ERROR)
    {
        SocketException ex(__FILE__, __LINE__);
        ex.error = getSocketErrno();
        throw ex;
    }

    struct ifaddrs* curr = ifap;
    while(curr != 0)
    {
        if(curr->ifa_addr && curr->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(curr->ifa_addr);
    	    if(addr->sin_addr.s_addr != 0)
    	    {
    	        result.push_back(inetAddrToString((*addr).sin_addr));
    	    }
        }
    
        curr = curr->ifa_next;
    }

    ::freeifaddrs(ifap);
#else
    SOCKET fd = createSocket(false);

#ifdef _AIX
    int cmd = CSIOCGIFCONF;
#else
    int cmd = SIOCGIFCONF;
#endif
    struct ifconf ifc;
    int numaddrs = 10;
    int old_ifc_len = 0;

    //
    // Need to call ioctl multiple times since we do not know up front
    // how many addresses there will be, and thus how large a buffer we need.
    // We keep increasing the buffer size until subsequent calls return
    // the same length, meaning we have all the addresses.
    //
    while(true)
    {
        int bufsize = numaddrs * static_cast<int>(sizeof(struct ifreq));
        ifc.ifc_len = bufsize;
        ifc.ifc_buf = (char*)malloc(bufsize);
    
        int rs = ioctl(fd, cmd, &ifc);
        if(rs == SOCKET_ERROR)
        {
	    free(ifc.ifc_buf);
    	    closeSocketNoThrow(fd);
    	    SocketException ex(__FILE__, __LINE__);
    	    ex.error = getSocketErrno();
    	    throw ex;
        }
        else if(ifc.ifc_len == old_ifc_len)
        {
            //
    	    // Returned same length twice in a row, finished.
    	    //
            break;
        }
        else
        {
            old_ifc_len = ifc.ifc_len;
        }
    
        numaddrs += 10;
        free(ifc.ifc_buf);
    }

    numaddrs = ifc.ifc_len / static_cast<int>(sizeof(struct ifreq));
    struct ifreq* ifr = ifc.ifc_req;
    for(int i = 0; i < numaddrs; ++i)
    {
        if(ifr[i].ifr_addr.sa_family == AF_INET)
        {
            struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(&ifr[i].ifr_addr);
    	    if(addr->sin_addr.s_addr != 0)
    	    {
    	        result.push_back(inetAddrToString((*addr).sin_addr));
    	    }
        }
    }

    free(ifc.ifc_buf);
    closeSocket(fd);
#endif

    return result;
}

#ifdef _WIN32
vector<struct sockaddr_in>
IceInternal::getLocalAddresses()
{
    vector<struct sockaddr_in> result;
    try
    {
        SOCKET fd = createSocket(false);

        vector<unsigned char> buffer;
        buffer.resize(1024);
        unsigned long len = 0;
        DWORD rs = WSAIoctl(fd, SIO_ADDRESS_LIST_QUERY, 0, 0, 
			    &buffer[0], static_cast<DWORD>(buffer.size()),
			    &len, 0, 0);
        if(rs == SOCKET_ERROR)
        {
            //
            // If the buffer wasn't big enough, resize it to the
            // required length and try again.
            //
            if(getSocketErrno() == WSAEFAULT)
            {
                buffer.resize(len);
                rs = WSAIoctl(fd, SIO_ADDRESS_LIST_QUERY, 0, 0, 
			      &buffer[0], static_cast<DWORD>(buffer.size()),
			      &len, 0, 0);
            }

            if(rs == SOCKET_ERROR)
            {
                closeSocketNoThrow(fd);
                SocketException ex(__FILE__, __LINE__);
                ex.error = getSocketErrno();
                throw ex;
            }
        }
        //        
        // Add the local interface addresses.
        //
        SOCKET_ADDRESS_LIST* addrs = reinterpret_cast<SOCKET_ADDRESS_LIST*>(&buffer[0]);
        for (int i = 0; i < addrs->iAddressCount; ++i)
        {
            result.push_back(*reinterpret_cast<struct sockaddr_in*>(addrs->Address[i].lpSockaddr));
        }

        //
        // Add the loopback interface address.
        //
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(0);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        result.push_back(addr);

        closeSocket(fd);
    }
    catch(const Ice::LocalException&)
    {
        //
        // TODO: Warning?
        //
    }
    return result;
}

bool
IceInternal::isLocalAddress(const struct sockaddr_in& addr)
{
    struct sockaddr_in addr0 = addr;
    addr0.sin_port = htons(0); // Local interface addresses have the port set to 0.

    try
    {
        vector<struct sockaddr_in> localAddrs = getLocalAddresses();
        for(vector<struct sockaddr_in>::const_iterator p = localAddrs.begin(); p != localAddrs.end(); ++p)
        {
	    if(compareAddress(addr0, *p))
	    {
	        return true;
	    }
        }
    }
    catch(const Ice::LocalException&)
    {
	//
	// TODO: Warning?
	//
    }
    return false;
}

bool
IceInternal::isPeerLocal(SOCKET fd)
{
    socklen_t remoteLen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    struct sockaddr_in remoteAddr;
    if(getpeername(fd, reinterpret_cast<struct sockaddr*>(&remoteAddr), &remoteLen) == SOCKET_ERROR)
    {
	if(notConnected())
	{
	    return false;
	}
	else
	{
	    closeSocketNoThrow(fd);
	    SocketException ex(__FILE__, __LINE__);
	    ex.error = getSocketErrno();
	    throw ex;
	}
    }
    return isLocalAddress(remoteAddr);
}
#endif
