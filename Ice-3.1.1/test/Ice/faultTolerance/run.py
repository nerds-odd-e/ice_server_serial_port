#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "config"))
import TestUtil

name = os.path.join("Ice", "faultTolerance")
testdir = os.path.join(toplevel, "test", name)

server = os.path.join(testdir, "server")
client = os.path.join(testdir, "client")

num = 12
base = 12340

for i in range(0, num):
    print "starting server #%d..." % (i + 1),
    serverPipe = os.popen(server + TestUtil.serverOptions + " %d" % (base + i) + " 2>&1")
    TestUtil.getServerPid(serverPipe)
    TestUtil.getAdapterReady(serverPipe)
    print "ok"

ports = ""
for i in range(0, num):
    ports = "%s %d" % (ports, base + i)

print "starting client...",
clientPipe = os.popen(client + TestUtil.clientOptions + " " + ports + " 2>&1")
print "ok"

TestUtil.printOutputFromPipe(clientPipe)
clientStatus = TestUtil.closePipe(clientPipe)
if clientStatus:
    TestUtil.killServers()

if clientStatus or TestUtil.serverStatus():
    sys.exit(1)

sys.exit(0)
