# Microsoft Developer Studio Project File - Name="mixlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mixlib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mixlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mixlib.mak" CFG="mixlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mixlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "mixlib_intermediate/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MIXLIB_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O2 /I "../src/pcre-2.08" /I "../src/zlib-1.1.4" /D "_USRDLL" /D "MIXLIB_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32" /D "IBMPC_SYSTEM" /D "_MSC" /D "MSC" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib advapi32.lib libeay32.lib /nologo /dll /pdb:none /machine:I386
# Begin Special Build Tool
IntDir=.\mixlib_intermediate/
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Desc=Build static library
PostBuild_Cmds=lib.exe /nologo /out:$(OutDir)\mixlib_static.lib $(IntDir)\*.obj
# End Special Build Tool
# Begin Target

# Name "mixlib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Src\buffers.c
# End Source File
# Begin Source File

SOURCE=..\Src\chain.c
# End Source File
# Begin Source File

SOURCE=..\Src\chain1.c
# End Source File
# Begin Source File

SOURCE=..\Src\chain2.c
# End Source File
# Begin Source File

SOURCE=..\Src\compress.c
# End Source File
# Begin Source File

SOURCE=..\Src\crypto.c
# End Source File
# Begin Source File

SOURCE=..\Src\dllmain.c
# End Source File
# Begin Source File

SOURCE=..\Src\keymgt.c
# End Source File
# Begin Source File

SOURCE=..\Src\mail.c
# End Source File
# Begin Source File

SOURCE=..\Src\maildir.c
# End Source File
# Begin Source File

SOURCE=..\Src\menu.c
# End Source File
# Begin Source File

SOURCE=..\Src\menunym.c
# End Source File
# Begin Source File

SOURCE=..\Src\menusend.c
# End Source File
# Begin Source File

SOURCE=..\Src\menuutil.c
# End Source File
# Begin Source File

SOURCE=..\Src\mime.c
# End Source File
# Begin Source File

SOURCE=..\Src\mix.c
# End Source File
# Begin Source File

SOURCE=..\Src\mixlib.def
# End Source File
# Begin Source File

SOURCE=..\Src\nym.c
# End Source File
# Begin Source File

SOURCE=.\parsedate.tab.c
# End Source File
# Begin Source File

SOURCE=..\Src\pgp.c
# End Source File
# Begin Source File

SOURCE=..\Src\pgpcreat.c
# End Source File
# Begin Source File

SOURCE=..\Src\pgpdata.c
# End Source File
# Begin Source File

SOURCE=..\Src\pgpdb.c
# End Source File
# Begin Source File

SOURCE=..\Src\pgpget.c
# End Source File
# Begin Source File

SOURCE=..\Src\pool.c
# End Source File
# Begin Source File

SOURCE=..\Src\random.c
# End Source File
# Begin Source File

SOURCE=..\Src\rem.c
# End Source File
# Begin Source File

SOURCE=..\Src\rem1.c
# End Source File
# Begin Source File

SOURCE=..\Src\rem2.c
# End Source File
# Begin Source File

SOURCE=..\Src\rfc822.c
# End Source File
# Begin Source File

SOURCE=..\Src\rndseed.c
# End Source File
# Begin Source File

SOURCE=..\Src\stats.c
# End Source File
# Begin Source File

SOURCE=..\Src\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Src\config.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
