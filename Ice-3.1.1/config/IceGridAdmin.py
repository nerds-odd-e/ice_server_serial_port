#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import sys, os, TestUtil
from threading import Thread

debug = 0
#debug = 1

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

iceGridPort = "12010";

nodeOptions = r' --Ice.Warn.Connections=0' + \
              r' --IceGrid.Node.Endpoints=default' + \
              r' --IceGrid.Node.WaitTime=30' + \
              r' --Ice.ProgramName=icegridnode' + \
              r' --IceGrid.Node.Trace.Activator=0' + \
              r' --IceGrid.Node.Trace.Adapter=0' + \
              r' --IceGrid.Node.Trace.Server=0' + \
              r' --IceGrid.Node.PrintServersReady=node' + \
	      r' --Ice.NullHandleAbort' + \
              r' --Ice.ThreadPool.Server.Size=0' + \
              r' --Ice.ServerIdleTime=0';

registryOptions = r' --Ice.Warn.Connections=0' + \
                  r' --IceGrid.Registry.PermissionsVerifier=IceGrid/NullPermissionsVerifier' + \
                  r' --IceGrid.Registry.AdminPermissionsVerifier=IceGrid/NullPermissionsVerifier' + \
                  r' --IceGrid.Registry.Server.Endpoints=default' + \
                  r' --IceGrid.Registry.Internal.Endpoints=default' + \
                  r' --IceGrid.Registry.Admin.Endpoints=default' + \
                  r' --IceGrid.Registry.Server.Endpoints=default' + \
                  r' --IceGrid.Registry.Internal.Endpoints=default' + \
                  r' --IceGrid.Registry.Admin.Endpoints=default' + \
                  r' --IceGrid.Registry.Trace.Session=0' + \
                  r' --IceGrid.Registry.Trace.Application=0' + \
                  r' --IceGrid.Registry.Trace.Node=0' + \
                  r' --IceGrid.Registry.Trace.Adapter=0' + \
                  r' --IceGrid.Registry.Trace.Object=0' + \
                  r' --IceGrid.Registry.Trace.Server=0' + \
                  r' --IceGrid.Registry.Trace.Locator=0' + \
                  r' --Ice.ThreadPool.Client.SizeWarn=10' + \
                  r' --Ice.ThreadPool.Server.Size=0' + \
                  r' --Ice.ServerIdleTime=0';

def startIceGridRegistry(testdir, dynamicRegistration = False):

    global iceGridPort

    iceGrid = os.path.join(toplevel, "bin", "icegridregistry")

    dataDir = os.path.join(testdir, "db", "registry")
    if not os.path.exists(dataDir):
        os.mkdir(dataDir)
    else:
        cleanDbDir(dataDir)

    print "starting icegrid registry...",
    command = iceGrid + TestUtil.clientServerOptions + ' --nowarn ' + registryOptions + \
              r' --IceGrid.Registry.Client.Endpoints="default -p ' + iceGridPort + ' -t 30000" ' + \
              r' --IceGrid.Registry.Data=' + dataDir + \
              r' --IceGrid.Registry.DefaultTemplates=' + os.path.join(toplevel, "config", "templates.xml")

    if dynamicRegistration:
        command += ' --IceGrid.Registry.DynamicRegistration'        

    if debug:
        print command

    (stdin, iceGridPipe) = os.popen4(command)
    TestUtil.getServerPid(iceGridPipe)
    TestUtil.getAdapterReady(iceGridPipe, True, 4)
    print "ok"

    return iceGridPipe

def startIceGridNode(testdir):

    iceGrid = os.path.join(toplevel, "bin", "icegridnode")

    dataDir = os.path.join(testdir, "db", "node")
    if not os.path.exists(dataDir):
        os.mkdir(dataDir)
    else:
        cleanDbDir(dataDir)

    overrideOptions = '"' + TestUtil.clientServerOptions.replace("--", "") + \
	              ' Ice.ServerIdleTime=0 Ice.PrintProcessId=0 Ice.PrintAdapterReady=0"'

    print "starting icegrid node...",
    command = iceGrid + TestUtil.clientServerOptions + ' --nowarn ' + nodeOptions + \
              r' "--Ice.Default.Locator=IceGrid/Locator:default -p ' + iceGridPort + '" ' + \
              r' --IceGrid.Node.Data=' + dataDir + \
              r' --IceGrid.Node.Name=localnode' + \
              r' --IceGrid.Node.PropertiesOverride=' + overrideOptions

    if debug:
        print command

    (stdin, iceGridPipe) = os.popen4(command)
    TestUtil.getServerPid(iceGridPipe)
    TestUtil.getAdapterReady(iceGridPipe, False)
    TestUtil.waitServiceReady(iceGridPipe, 'node')
        
    print "ok"

    return iceGridPipe

def iceGridAdmin(cmd, ignoreFailure = False):

    global iceGridPort
    iceGridAdmin = os.path.join(toplevel, "bin", "icegridadmin")

    command = iceGridAdmin + TestUtil.clientOptions + \
              r' --Ice.Default.Locator="IceGrid/Locator:default -p ' + iceGridPort + '" ' + \
              r' -e "' + cmd + '" 2>&1'

    if debug:
        print command

    iceGridAdminPipe = os.popen(command)

    output = iceGridAdminPipe.readlines()
        
    iceGridAdminStatus = TestUtil.closePipe(iceGridAdminPipe)
    if not ignoreFailure and iceGridAdminStatus:
        print "icegridadmin command failed: " + cmd
        for line in output:
            print line
        TestUtil.killServers()
        sys.exit(1)

    return output
    
def killNodeServers():
    
    global iceGridPort
    
    for server in iceGridAdmin("server list"):
        server = server.strip()
        iceGridAdmin("server disable " + server, True)
        iceGridAdmin("server signal " + server + " SIGKILL", True)

def iceGridTest(name, application, additionalOptions = "", applicationOptions = ""):

    testdir = os.path.join(toplevel, "test", name)
    client = os.path.join(testdir, "client")

    clientOptions = " --Ice.Default.Locator=\"IceGrid/Locator:default -p " + iceGridPort + "\" " + additionalOptions

    iceGridRegistryPipe = startIceGridRegistry(testdir)
    iceGridNodePipe = startIceGridNode(testdir)

    if application != "":
        print "adding application...",
        iceGridAdmin('application add ' + os.path.join(testdir, application) + ' ' + \
                     '"test.dir=' + testdir + '" "ice.dir=' + toplevel + '" ' + applicationOptions)
        print "ok"

    print "starting client...",

    command = client + TestUtil.clientOptions + " " + clientOptions + " 2>&1"
    if debug:
        print command

    clientPipe = os.popen(command)
    print "ok"

    TestUtil.printOutputFromPipe(clientPipe)
    
    clientStatus = TestUtil.closePipe(clientPipe)
    if clientStatus:
        killNodeServers()
        if application != "":
            print "remove application...",
            iceGridAdmin("application remove Test", True)
            print "ok"
        TestUtil.killServers()
        sys.exit(1)

    if application != "":
        print "remove application...",
        iceGridAdmin("application remove Test")
        print "ok"

    print "shutting down icegrid node...",
    iceGridAdmin("node shutdown localnode")
    print "ok"
    print "shutting down icegrid registry...",
    iceGridAdmin("shutdown")
    print "ok"

    TestUtil.joinServers()

    if TestUtil.serverStatus():
        sys.exit(1)                

def iceGridClientServerTest(name, additionalClientOptions, additionalServerOptions):

    testdir = os.path.join(toplevel, "test", name)
    server = os.path.join(testdir, "server")
    client = os.path.join(testdir, "client")

    clientOptions = "--Ice.Default.Locator=\"IceGrid/Locator:default -p " + iceGridPort + "\" " + \
                    additionalClientOptions

    serverOptions = "--Ice.Default.Locator=\"IceGrid/Locator:default -p " + iceGridPort + "\" " + \
                    additionalServerOptions
    
    iceGridRegistryPipe = startIceGridRegistry(testdir, True)

    print "starting sever...",
    serverPipe = os.popen(server + TestUtil.clientServerOptions + " " + serverOptions + " 2>&1")
    TestUtil.getServerPid(serverPipe)
    TestUtil.getAdapterReady(serverPipe)
    print "ok"

    print "starting client...",
    command = client + TestUtil.clientOptions + " " + clientOptions + " 2>&1"
    if debug:
        print command

    clientPipe = os.popen(command)
    print "ok"

    TestUtil.printOutputFromPipe(clientPipe)
    
    clientStatus = TestUtil.closePipe(clientPipe)
    if clientStatus:
        TestUtil.killServers()
        sys.exit(1)

    print "shutting down icegrid registry...",
    iceGridAdmin("shutdown")
    print "ok"

    TestUtil.joinServers()

    if TestUtil.serverStatus():
        sys.exit(1)

def cleanDbDir(path):
    
    files = os.listdir(path)
    for filename in files:
        if filename != "CVS" and filename != ".dummy":
            fullpath = os.path.join(path, filename);
            if os.path.isdir(fullpath):
                cleanDbDir(fullpath)
		try:
		    os.rmdir(fullpath)
		except OSError:
		    # This might fail if the directory is empty (because it itself is
		    # a CVS directory).
		    pass
            else:
                os.remove(fullpath)
