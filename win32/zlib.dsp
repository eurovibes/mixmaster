# Microsoft Developer Studio Project File - Name="zlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zlib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "zlib_intermediate/"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Target

# Name "zlib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\adler32.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\compress.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\crc32.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\deflate.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\gzio.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\infblock.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\infcodes.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\inffast.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\inflate.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\inftrees.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\infutil.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\trees.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\uncompr.c"
# End Source File
# Begin Source File

SOURCE="..\Src\zlib-1.1.4\zutil.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
