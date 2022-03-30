# Microsoft Developer Studio Project File - Name="glacier2router" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=glacier2router - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "glacier2router.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "glacier2router.mak" CFG="glacier2router - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "glacier2router - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "glacier2router - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "glacier2router - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I ".." /I "../../include" /D "_CONSOLE" /D "NDEBUG" /D "WIN32_LEAN_AND_MEAN" /D "PCRE_STATIC" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib libeay32.lib /nologo /subsystem:console /pdb:none /machine:I386 /FIXED:no
# SUBTRACT LINK32 /debug /nodefaultlib
# Begin Special Build Tool
OutDir=.\Release
TargetName=glacier2router
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\$(TargetName).exe ..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "glacier2router - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /Zi /Od /I ".." /I "../../include" /D "_CONSOLE" /D "_DEBUG" /D "WIN32_LEAN_AND_MEAN" /D "PCRE_STATIC" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib libeay32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /FIXED:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\Debug
TargetName=glacier2router
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\$(TargetName).exe ..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "glacier2router - Win32 Release"
# Name "glacier2router - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Blobject.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientBlobject.cpp
# End Source File
# Begin Source File

SOURCE=.\CryptPermissionsVerifierI.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterI.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterManager.cpp
# End Source File
# Begin Source File

SOURCE=.\glacier2router.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyVerifier.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\RouterI.cpp
# End Source File
# Begin Source File

SOURCE=.\RoutingTable.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerBlobject.cpp
# End Source File
# Begin Source File

SOURCE=.\SessionRouterI.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Blobject.h
# End Source File
# Begin Source File

SOURCE=.\ClientBlobject.h
# End Source File
# Begin Source File

SOURCE=.\CryptPermissionsVerifierI.h
# End Source File
# Begin Source File

SOURCE=.\FilterI.h
# End Source File
# Begin Source File

SOURCE=.\FilterManager.h
# End Source File
# Begin Source File

SOURCE=.\RequestQueue.h
# End Source File
# Begin Source File

SOURCE=.\RouterI.h
# End Source File
# Begin Source File

SOURCE=.\RoutingTable.h
# End Source File
# Begin Source File

SOURCE=.\ServerBlobject.h
# End Source File
# Begin Source File

SOURCE=.\SessionRouterI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
