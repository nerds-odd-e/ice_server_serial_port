// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_SERVER_I_H
#define ICE_GRID_SERVER_I_H

#include <IceUtil/Mutex.h>
#include <Freeze/EvictorF.h>
#include <IceGrid/Activator.h>
#include <IceGrid/WaitQueue.h>
#include <IceGrid/Internal.h>

#ifndef _WIN32
#   include <sys/types.h> // for uid_t, gid_t
#endif

namespace IceGrid
{

class NodeI;
typedef IceUtil::Handle<NodeI> NodeIPtr;
class ServerAdapterI;
typedef IceUtil::Handle<ServerAdapterI> ServerAdapterIPtr;
class ServerCommand;
typedef IceUtil::Handle<ServerCommand> ServerCommandPtr;
class DestroyCommand;
typedef IceUtil::Handle<DestroyCommand> DestroyCommandPtr;
class StopCommand;
typedef IceUtil::Handle<StopCommand> StopCommandPtr;
class StartCommand;
typedef IceUtil::Handle<StartCommand> StartCommandPtr;
class PatchCommand;
typedef IceUtil::Handle<PatchCommand> PatchCommandPtr;
class LoadCommand;
typedef IceUtil::Handle<LoadCommand> LoadCommandPtr;

class ServerI : public Server, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    enum InternalServerState
    {
	Inactive,
	Activating,
	WaitForActivation,
	ActivationTimeout,
	Active,
	Deactivating,
	DeactivatingWaitForProcess,
	Destroying,
	Destroyed,
	Loading,
	Patching
    };

    enum ServerActivation
    {
	Always,
	Session,
	OnDemand,
	Manual,
	Disabled
    };

    ServerI(const NodeIPtr&, const ServerPrx&, const std::string&, const std::string&, int);
    virtual ~ServerI();

    virtual void start_async(const AMD_Server_startPtr&, const ::Ice::Current& = Ice::Current());
    virtual void stop_async(const AMD_Server_stopPtr&, const ::Ice::Current& = Ice::Current());
    virtual void sendSignal(const std::string&, const ::Ice::Current&);
    virtual void writeMessage(const std::string&, Ice::Int, const ::Ice::Current&);

    virtual ServerState getState(const ::Ice::Current& = Ice::Current()) const;
    virtual Ice::Int getPid(const ::Ice::Current& = Ice::Current()) const;

    virtual void setEnabled(bool, const ::Ice::Current&);
    virtual bool isEnabled(const ::Ice::Current& = Ice::Current()) const;
    virtual void setProcess_async(const AMD_Server_setProcessPtr&, const ::Ice::ProcessPrx&, const ::Ice::Current&);

    std::string getApplication() const;
    bool canActivateOnDemand() const;
    const std::string& getId() const;
    DistributionDescriptor getDistribution() const;
    bool hasApplicationDistribution() const;
    void getDynamicInfo(ServerDynamicInfoSeq&, AdapterDynamicInfoSeq&) const;

    void start(ServerActivation, const AMD_Server_startPtr& = AMD_Server_startPtr());
    void load(const AMD_Node_loadServerPtr&, const std::string&, const ServerDescriptorPtr&, const std::string&);
    void destroy(const AMD_Node_destroyServerPtr&);
    bool startPatch(bool);
    bool waitForPatch();
    void finishPatch();

    void adapterActivated(const std::string&);
    void adapterDeactivated(const std::string&);
    void activationFailed(bool);
    void deactivationFailed();

    void activate();
    void kill();
    void deactivate();
    void update();
    void destroy();
    void terminated(const std::string&, int);

private:
    
    void updateImpl(const std::string&, const ServerDescriptorPtr&, const std::string&);
    void checkActivation();
    void checkDestroyed();
    void disableOnFailure();
    void enableAfterFailure(bool);

    void setState(InternalServerState, const std::string& = std::string());
    ServerCommandPtr nextCommand();
    void setStateNoSync(InternalServerState, const std::string& = std::string());
    
    std::string addAdapter(const AdapterDescriptor&, const CommunicatorDescriptorPtr&);
    void updateConfigFile(const std::string&, const CommunicatorDescriptorPtr&);
    void updateDbEnv(const std::string&, const DbEnvDescriptor&);
    PropertyDescriptor createProperty(const std::string&, const std::string& = std::string());
    void createOrUpdateDirectory(const std::string&);
    ServerState toServerState(InternalServerState) const;
    ServerActivation toServerActivation(const std::string&) const;
    ServerDynamicInfo getDynamicInfo() const;

    const NodeIPtr _node;
    const ServerPrx _this;
    const std::string _id;
    const Ice::Int _waitTime;
    const std::string _serverDir;
    const int _disableOnFailure;

    std::string _application;
    ServerDescriptorPtr _desc;
    std::string _sessionId;
#ifndef _WIN32
    uid_t _uid;
    gid_t _gid;
#endif
    InternalServerState _state;
    ServerActivation _activation;
    int _activationTimeout;
    int _deactivationTimeout;
    typedef std::map<std::string, ServerAdapterIPtr> ServerAdapterDict;
    ServerAdapterDict _adapters;
    bool _processRegistered;
    Ice::ProcessPrx _process;
    std::set<std::string> _activeAdapters;
    IceUtil::Time _failureTime;
    ServerActivation _previousActivation;
    WaitItemPtr _timer;

    DestroyCommandPtr _destroy;
    StopCommandPtr _stop;
    LoadCommandPtr _load;
    PatchCommandPtr _patch;
    StartCommandPtr _start;
    
    int _pid;
};
typedef IceUtil::Handle<ServerI> ServerIPtr;

class ServerCommand : public IceUtil::SimpleShared
{
public:

    ServerCommand(const ServerIPtr&);
    virtual void execute() = 0;
    virtual ServerI::InternalServerState nextState() = 0;

protected:

    const ServerIPtr _server;
};
typedef IceUtil::Handle<ServerCommand> ServerCommandPtr;

class TimedServerCommand : public ServerCommand
{
public:

    TimedServerCommand(const ServerIPtr&, const WaitQueuePtr&, int);
    virtual void timeout(bool) = 0;

    void startTimer();
    void stopTimer();

private:

    WaitQueuePtr _waitQueue;
    WaitItemPtr _timer;
    int _timeout;
};
typedef IceUtil::Handle<TimedServerCommand> TimedServerCommandPtr;

class DestroyCommand : public ServerCommand
{
public:

    DestroyCommand(const ServerIPtr&, bool);

    bool canExecute(ServerI::InternalServerState state);
    ServerI::InternalServerState nextState();
    void execute();

    void addCallback(const AMD_Node_destroyServerPtr& amdCB);
    void finished();

private:

    const bool _kill;
    std::vector<AMD_Node_destroyServerPtr> _destroyCB;
};

class StopCommand : public TimedServerCommand
{
public:

    StopCommand(const ServerIPtr&, const WaitQueuePtr&, int);

    static bool isStopped(ServerI::InternalServerState state);

    bool canExecute(ServerI::InternalServerState state);
    ServerI::InternalServerState nextState();
    void execute();
    void timeout(bool destroyed);

    void addCallback(const AMD_Server_stopPtr&);
    void failed(const std::string& reason);
    void finished();

private:

    std::vector<AMD_Server_stopPtr> _stopCB;
};

class StartCommand : public TimedServerCommand
{
public:

    StartCommand(const ServerIPtr&, const WaitQueuePtr&, int);

    bool canExecute(ServerI::InternalServerState state);
    ServerI::InternalServerState nextState();
    void execute();
    void timeout(bool destroyed);

    void addCallback(const AMD_Server_startPtr&);
    void failed(const std::string& reason);
    void finished();

private:

    std::vector<AMD_Server_startPtr> _startCB;
};

class PatchCommand : public ServerCommand, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    PatchCommand(const ServerIPtr&);

    bool canExecute(ServerI::InternalServerState state);
    ServerI::InternalServerState nextState();
    void execute();

    bool waitForPatch();
    void destroyed();
    void finished();

private:

    bool _notified;
    bool _destroyed;
};

class LoadCommand : public ServerCommand
{
public:

    LoadCommand(const ServerIPtr&);

    bool canExecute(ServerI::InternalServerState state);
    ServerI::InternalServerState nextState();
    void execute();

    void setUpdate(const std::string&, const ServerDescriptorPtr&, const std::string&, bool clearDir);
    bool clearDir() const;
    std::string sessionId() const;
    std::string getApplication() const;
    ServerDescriptorPtr getDescriptor() const;
    void addCallback(const AMD_Node_loadServerPtr&);
    void failed(const Ice::Exception&);
    void finished(const ServerPrx&, const AdapterPrxDict&, int, int);

private:

    std::vector<AMD_Node_loadServerPtr> _loadCB;
    bool _clearDir;
    std::string _application;
    ServerDescriptorPtr _desc;
    std::string _sessionId;
    std::auto_ptr<DeploymentException> _exception;
};

}

#endif
