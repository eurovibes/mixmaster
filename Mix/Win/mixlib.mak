# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=mixlib - Win32 Release
!MESSAGE No configuration specified.  Defaulting to mixlib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "mixlib - Win32 Release" && "$(CFG)" != "mixlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mixlib.mak" CFG="mixlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mixlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mixlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "mixlib - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "mixlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\mixlib.dll"

CLEAN : 
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\buffers.obj"
	-@erase "$(INTDIR)\chain.obj"
	-@erase "$(INTDIR)\chain1.obj"
	-@erase "$(INTDIR)\chain2.obj"
	-@erase "$(INTDIR)\chain3.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crypto.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\keymgt.obj"
	-@erase "$(INTDIR)\mail.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menunym.obj"
	-@erase "$(INTDIR)\menusend.obj"
	-@erase "$(INTDIR)\menuutil.obj"
	-@erase "$(INTDIR)\mime.obj"
	-@erase "$(INTDIR)\mix.obj"
	-@erase "$(INTDIR)\nym.obj"
	-@erase "$(INTDIR)\pgp.obj"
	-@erase "$(INTDIR)\pgpcreat.obj"
	-@erase "$(INTDIR)\pgpdata.obj"
	-@erase "$(INTDIR)\pgpdb.obj"
	-@erase "$(INTDIR)\pgpget.obj"
	-@erase "$(INTDIR)\pool.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\rem.obj"
	-@erase "$(INTDIR)\rem1.obj"
	-@erase "$(INTDIR)\rem2.obj"
	-@erase "$(INTDIR)\rem3.obj"
	-@erase "$(INTDIR)\rfc822.obj"
	-@erase "$(INTDIR)\rndseed.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\mixlib.dll"
	-@erase "$(OUTDIR)\mixlib.exp"
	-@erase "$(OUTDIR)\mixlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W2 /GX /I "include" /I "zlib-1.1.4" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "MIXLIB_EXPORTS" /D "USE_SOCK" /D "USE_ZLIB" /D "_MSC" /D "WIN32SERVICE" /D "BROKEN_MTA" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MD /W2 /GX /I "include" /I "zlib-1.1.4" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "MIXLIB_EXPORTS" /D "USE_SOCK" /D\
 "USE_ZLIB" /D "_MSC" /D "WIN32SERVICE" /D "BROKEN_MTA" /D "_MBCS"\
 /Fp"$(INTDIR)/mixlib.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mixlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mixlib.pdb" /machine:I386 /out:"$(OUTDIR)/mixlib.dll"\
 /implib:"$(OUTDIR)/mixlib.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\buffers.obj" \
	"$(INTDIR)\chain.obj" \
	"$(INTDIR)\chain1.obj" \
	"$(INTDIR)\chain2.obj" \
	"$(INTDIR)\chain3.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crypto.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\keymgt.obj" \
	"$(INTDIR)\mail.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\menunym.obj" \
	"$(INTDIR)\menusend.obj" \
	"$(INTDIR)\menuutil.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\mix.obj" \
	"$(INTDIR)\nym.obj" \
	"$(INTDIR)\pgp.obj" \
	"$(INTDIR)\pgpcreat.obj" \
	"$(INTDIR)\pgpdata.obj" \
	"$(INTDIR)\pgpdb.obj" \
	"$(INTDIR)\pgpget.obj" \
	"$(INTDIR)\pool.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\rem.obj" \
	"$(INTDIR)\rem1.obj" \
	"$(INTDIR)\rem2.obj" \
	"$(INTDIR)\rem3.obj" \
	"$(INTDIR)\rfc822.obj" \
	"$(INTDIR)\rndseed.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\zutil.obj" \
	".\libeay32.lib"

"$(OUTDIR)\mixlib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mixlib_"
# PROP BASE Intermediate_Dir "mixlib_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mixlib_"
# PROP Intermediate_Dir "mixlib_"
# PROP Target_Dir ""
OUTDIR=.\mixlib_
INTDIR=.\mixlib_

ALL : "$(OUTDIR)\mixlib.dll"

CLEAN : 
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\buffers.obj"
	-@erase "$(INTDIR)\chain.obj"
	-@erase "$(INTDIR)\chain1.obj"
	-@erase "$(INTDIR)\chain2.obj"
	-@erase "$(INTDIR)\chain3.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crypto.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\keymgt.obj"
	-@erase "$(INTDIR)\mail.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menunym.obj"
	-@erase "$(INTDIR)\menusend.obj"
	-@erase "$(INTDIR)\menuutil.obj"
	-@erase "$(INTDIR)\mime.obj"
	-@erase "$(INTDIR)\mix.obj"
	-@erase "$(INTDIR)\nym.obj"
	-@erase "$(INTDIR)\pgp.obj"
	-@erase "$(INTDIR)\pgpcreat.obj"
	-@erase "$(INTDIR)\pgpdata.obj"
	-@erase "$(INTDIR)\pgpdb.obj"
	-@erase "$(INTDIR)\pgpget.obj"
	-@erase "$(INTDIR)\pool.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\rem.obj"
	-@erase "$(INTDIR)\rem1.obj"
	-@erase "$(INTDIR)\rem2.obj"
	-@erase "$(INTDIR)\rem3.obj"
	-@erase "$(INTDIR)\rfc822.obj"
	-@erase "$(INTDIR)\rndseed.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\mixlib.dll"
	-@erase "$(OUTDIR)\mixlib.exp"
	-@erase "$(OUTDIR)\mixlib.ilk"
	-@erase "$(OUTDIR)\mixlib.lib"
	-@erase "$(OUTDIR)\mixlib.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /I "include" /I "zlib-1.1.4" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "MIXLIB_EXPORTS" /D "USE_SOCK" /D "USE_ZLIB" /D "_MSC" /D "WIN32SERVICE" /D "BROKEN_MTA" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /I "include" /I "zlib-1.1.4" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "MIXLIB_EXPORTS" /D "USE_SOCK" /D\
 "USE_ZLIB" /D "_MSC" /D "WIN32SERVICE" /D "BROKEN_MTA" /D "_MBCS"\
 /Fp"$(INTDIR)/mixlib.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\mixlib_/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mixlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/mixlib.pdb" /debug /machine:I386 /out:"$(OUTDIR)/mixlib.dll"\
 /implib:"$(OUTDIR)/mixlib.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\buffers.obj" \
	"$(INTDIR)\chain.obj" \
	"$(INTDIR)\chain1.obj" \
	"$(INTDIR)\chain2.obj" \
	"$(INTDIR)\chain3.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crypto.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\keymgt.obj" \
	"$(INTDIR)\mail.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\menunym.obj" \
	"$(INTDIR)\menusend.obj" \
	"$(INTDIR)\menuutil.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\mix.obj" \
	"$(INTDIR)\nym.obj" \
	"$(INTDIR)\pgp.obj" \
	"$(INTDIR)\pgpcreat.obj" \
	"$(INTDIR)\pgpdata.obj" \
	"$(INTDIR)\pgpdb.obj" \
	"$(INTDIR)\pgpget.obj" \
	"$(INTDIR)\pool.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\rem.obj" \
	"$(INTDIR)\rem1.obj" \
	"$(INTDIR)\rem2.obj" \
	"$(INTDIR)\rem3.obj" \
	"$(INTDIR)\rfc822.obj" \
	"$(INTDIR)\rndseed.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\zutil.obj" \
	".\libeay32.lib"

"$(OUTDIR)\mixlib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "mixlib - Win32 Release"
# Name "mixlib - Win32 Debug"

!IF  "$(CFG)" == "mixlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\buffers.c
DEP_CPP_BUFFE=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\buffers.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\version.h

!IF  "$(CFG)" == "mixlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix.h

!IF  "$(CFG)" == "mixlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix3.h

!IF  "$(CFG)" == "mixlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\config.h

!IF  "$(CFG)" == "mixlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.c
DEP_CPP_UTIL_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\random.c
DEP_CPP_RANDO=\
	".\config.h"\
	".\crypto.h"\
	".\include\openssl/aes.h"\
	".\include\openssl/asn1.h"\
	".\include\openssl/bio.h"\
	".\include\openssl/blowfish.h"\
	".\include\openssl/bn.h"\
	".\include\openssl/cast.h"\
	".\include\openssl/crypto.h"\
	".\include\openssl/des.h"\
	".\include\openssl/des_old.h"\
	".\include\openssl/dh.h"\
	".\include\openssl/dsa.h"\
	".\include\openssl/e_os2.h"\
	".\include\openssl/ebcdic.h"\
	".\include\openssl/idea.h"\
	".\include\openssl/md5.h"\
	".\include\openssl/opensslconf.h"\
	".\include\openssl/opensslv.h"\
	".\include\openssl/ossl_typ.h"\
	".\include\openssl/rand.h"\
	".\include\openssl/ripemd.h"\
	".\include\openssl/rsa.h"\
	".\include\openssl/safestack.h"\
	".\include\openssl/sha.h"\
	".\include\openssl/stack.h"\
	".\include\openssl/symhacks.h"\
	".\include\openssl/ui.h"\
	".\include\openssl/ui_compat.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\random.obj" : $(SOURCE) $(DEP_CPP_RANDO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix.c
DEP_CPP_MIX_C=\
	".\config.h"\
	".\menu.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\mix.obj" : $(SOURCE) $(DEP_CPP_MIX_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\menuutil.c
DEP_CPP_MENUU=\
	".\config.h"\
	".\menu.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\menuutil.obj" : $(SOURCE) $(DEP_CPP_MENUU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libeay32.lib

!IF  "$(CFG)" == "mixlib - Win32 Release"

!ELSEIF  "$(CFG)" == "mixlib - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rndseed.c
DEP_CPP_RNDSE=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\rndseed.obj" : $(SOURCE) $(DEP_CPP_RNDSE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mail.c
DEP_CPP_MAIL_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\mail.obj" : $(SOURCE) $(DEP_CPP_MAIL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pool.c
DEP_CPP_POOL_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_POOL_=\
	".\pcre.h"\
	

"$(INTDIR)\pool.obj" : $(SOURCE) $(DEP_CPP_POOL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\keymgt.c
DEP_CPP_KEYMG=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\keymgt.obj" : $(SOURCE) $(DEP_CPP_KEYMG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\stats.c
DEP_CPP_STATS=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\stats.obj" : $(SOURCE) $(DEP_CPP_STATS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rem.c
DEP_CPP_REM_C=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\rem.obj" : $(SOURCE) $(DEP_CPP_REM_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rfc822.c
DEP_CPP_RFC82=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\rfc822.obj" : $(SOURCE) $(DEP_CPP_RFC82) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mime.c
DEP_CPP_MIME_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\mime.obj" : $(SOURCE) $(DEP_CPP_MIME_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\crypto.c
DEP_CPP_CRYPT=\
	".\config.h"\
	".\crypto.h"\
	".\include\openssl/aes.h"\
	".\include\openssl/asn1.h"\
	".\include\openssl/bio.h"\
	".\include\openssl/blowfish.h"\
	".\include\openssl/bn.h"\
	".\include\openssl/cast.h"\
	".\include\openssl/crypto.h"\
	".\include\openssl/des.h"\
	".\include\openssl/des_old.h"\
	".\include\openssl/dh.h"\
	".\include\openssl/dsa.h"\
	".\include\openssl/e_os2.h"\
	".\include\openssl/ebcdic.h"\
	".\include\openssl/idea.h"\
	".\include\openssl/md5.h"\
	".\include\openssl/opensslconf.h"\
	".\include\openssl/opensslv.h"\
	".\include\openssl/ossl_typ.h"\
	".\include\openssl/rand.h"\
	".\include\openssl/ripemd.h"\
	".\include\openssl/rsa.h"\
	".\include\openssl/safestack.h"\
	".\include\openssl/sha.h"\
	".\include\openssl/stack.h"\
	".\include\openssl/symhacks.h"\
	".\include\openssl/ui.h"\
	".\include\openssl/ui_compat.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\crypto.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\chain2.c
DEP_CPP_CHAIN=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\chain2.obj" : $(SOURCE) $(DEP_CPP_CHAIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgpdb.c
DEP_CPP_PGPDB=\
	".\config.h"\
	".\include\openssl/opensslv.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	

"$(INTDIR)\pgpdb.obj" : $(SOURCE) $(DEP_CPP_PGPDB) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\chain1.c
DEP_CPP_CHAIN1=\
	".\config.h"\
	".\include\openssl/opensslv.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	

"$(INTDIR)\chain1.obj" : $(SOURCE) $(DEP_CPP_CHAIN1) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rem1.c
DEP_CPP_REM1_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\rem1.obj" : $(SOURCE) $(DEP_CPP_REM1_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgpget.c
DEP_CPP_PGPGE=\
	".\config.h"\
	".\crypto.h"\
	".\include\openssl/aes.h"\
	".\include\openssl/asn1.h"\
	".\include\openssl/bio.h"\
	".\include\openssl/blowfish.h"\
	".\include\openssl/bn.h"\
	".\include\openssl/cast.h"\
	".\include\openssl/crypto.h"\
	".\include\openssl/des.h"\
	".\include\openssl/des_old.h"\
	".\include\openssl/dh.h"\
	".\include\openssl/dsa.h"\
	".\include\openssl/e_os2.h"\
	".\include\openssl/ebcdic.h"\
	".\include\openssl/idea.h"\
	".\include\openssl/md5.h"\
	".\include\openssl/opensslconf.h"\
	".\include\openssl/opensslv.h"\
	".\include\openssl/ossl_typ.h"\
	".\include\openssl/rand.h"\
	".\include\openssl/ripemd.h"\
	".\include\openssl/rsa.h"\
	".\include\openssl/safestack.h"\
	".\include\openssl/sha.h"\
	".\include\openssl/stack.h"\
	".\include\openssl/symhacks.h"\
	".\include\openssl/ui.h"\
	".\include\openssl/ui_compat.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\pgpget.obj" : $(SOURCE) $(DEP_CPP_PGPGE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rem2.c
DEP_CPP_REM2_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\rem2.obj" : $(SOURCE) $(DEP_CPP_REM2_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rem3.c
DEP_CPP_REM3_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\rem3.obj" : $(SOURCE) $(DEP_CPP_REM3_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgpdata.c
DEP_CPP_PGPDA=\
	".\config.h"\
	".\crypto.h"\
	".\include\openssl/aes.h"\
	".\include\openssl/asn1.h"\
	".\include\openssl/bio.h"\
	".\include\openssl/blowfish.h"\
	".\include\openssl/bn.h"\
	".\include\openssl/cast.h"\
	".\include\openssl/crypto.h"\
	".\include\openssl/des.h"\
	".\include\openssl/des_old.h"\
	".\include\openssl/dh.h"\
	".\include\openssl/dsa.h"\
	".\include\openssl/e_os2.h"\
	".\include\openssl/ebcdic.h"\
	".\include\openssl/idea.h"\
	".\include\openssl/md5.h"\
	".\include\openssl/opensslconf.h"\
	".\include\openssl/opensslv.h"\
	".\include\openssl/ossl_typ.h"\
	".\include\openssl/rand.h"\
	".\include\openssl/ripemd.h"\
	".\include\openssl/rsa.h"\
	".\include\openssl/safestack.h"\
	".\include\openssl/sha.h"\
	".\include\openssl/stack.h"\
	".\include\openssl/symhacks.h"\
	".\include\openssl/ui.h"\
	".\include\openssl/ui_compat.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\pgpdata.obj" : $(SOURCE) $(DEP_CPP_PGPDA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgpcreat.c
DEP_CPP_PGPCR=\
	".\config.h"\
	".\crypto.h"\
	".\include\openssl/aes.h"\
	".\include\openssl/asn1.h"\
	".\include\openssl/bio.h"\
	".\include\openssl/blowfish.h"\
	".\include\openssl/bn.h"\
	".\include\openssl/cast.h"\
	".\include\openssl/crypto.h"\
	".\include\openssl/des.h"\
	".\include\openssl/des_old.h"\
	".\include\openssl/dh.h"\
	".\include\openssl/dsa.h"\
	".\include\openssl/e_os2.h"\
	".\include\openssl/ebcdic.h"\
	".\include\openssl/idea.h"\
	".\include\openssl/md5.h"\
	".\include\openssl/opensslconf.h"\
	".\include\openssl/opensslv.h"\
	".\include\openssl/ossl_typ.h"\
	".\include\openssl/rand.h"\
	".\include\openssl/ripemd.h"\
	".\include\openssl/rsa.h"\
	".\include\openssl/safestack.h"\
	".\include\openssl/sha.h"\
	".\include\openssl/stack.h"\
	".\include\openssl/symhacks.h"\
	".\include\openssl/ui.h"\
	".\include\openssl/ui_compat.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\pgpcreat.obj" : $(SOURCE) $(DEP_CPP_PGPCR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\compress.c
DEP_CPP_COMPR=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgp.c
DEP_CPP_PGP_C=\
	".\config.h"\
	".\include\openssl/opensslv.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	

"$(INTDIR)\pgp.obj" : $(SOURCE) $(DEP_CPP_PGP_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\deflate.c"
DEP_CPP_DEFLA=\
	".\zlib-1.1.4\deflate.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\infblock.c"
DEP_CPP_INFBL=\
	".\zlib-1.1.4\infblock.h"\
	".\zlib-1.1.4\infcodes.h"\
	".\zlib-1.1.4\inftrees.h"\
	".\zlib-1.1.4\infutil.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\infblock.obj" : $(SOURCE) $(DEP_CPP_INFBL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\inftrees.c"
DEP_CPP_INFTR=\
	".\zlib-1.1.4\inffixed.h"\
	".\zlib-1.1.4\inftrees.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\inftrees.obj" : $(SOURCE) $(DEP_CPP_INFTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\infutil.c"
DEP_CPP_INFUT=\
	".\zlib-1.1.4\infblock.h"\
	".\zlib-1.1.4\infcodes.h"\
	".\zlib-1.1.4\inftrees.h"\
	".\zlib-1.1.4\infutil.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\infutil.obj" : $(SOURCE) $(DEP_CPP_INFUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\infcodes.c"
DEP_CPP_INFCO=\
	".\zlib-1.1.4\infblock.h"\
	".\zlib-1.1.4\infcodes.h"\
	".\zlib-1.1.4\inffast.h"\
	".\zlib-1.1.4\inftrees.h"\
	".\zlib-1.1.4\infutil.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\infcodes.obj" : $(SOURCE) $(DEP_CPP_INFCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\inffast.c"
DEP_CPP_INFFA=\
	".\zlib-1.1.4\infblock.h"\
	".\zlib-1.1.4\infcodes.h"\
	".\zlib-1.1.4\inffast.h"\
	".\zlib-1.1.4\inftrees.h"\
	".\zlib-1.1.4\infutil.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\inffast.obj" : $(SOURCE) $(DEP_CPP_INFFA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\adler32.c"
DEP_CPP_ADLER=\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\adler32.obj" : $(SOURCE) $(DEP_CPP_ADLER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\zutil.c"
DEP_CPP_ZUTIL=\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\zutil.obj" : $(SOURCE) $(DEP_CPP_ZUTIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\inflate.c"
DEP_CPP_INFLA=\
	".\zlib-1.1.4\infblock.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\inflate.obj" : $(SOURCE) $(DEP_CPP_INFLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\chain.c
DEP_CPP_CHAIN_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\chain.obj" : $(SOURCE) $(DEP_CPP_CHAIN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=".\zlib-1.1.4\trees.c"
DEP_CPP_TREES=\
	".\zlib-1.1.4\deflate.h"\
	".\zlib-1.1.4\trees.h"\
	".\zlib-1.1.4\zconf.h"\
	".\zlib-1.1.4\zlib.h"\
	".\zlib-1.1.4\zutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\chain3.c
DEP_CPP_CHAIN3=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\chain3.obj" : $(SOURCE) $(DEP_CPP_CHAIN3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\menu.c
DEP_CPP_MENU_=\
	".\config.h"\
	".\menu.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\menusend.c
DEP_CPP_MENUS=\
	".\config.h"\
	".\menu.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\menusend.obj" : $(SOURCE) $(DEP_CPP_MENUS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\nym.c
DEP_CPP_NYM_C=\
	".\config.h"\
	".\include\openssl/opensslv.h"\
	".\mix.h"\
	".\mix3.h"\
	".\pgp.h"\
	".\version.h"\
	

"$(INTDIR)\nym.obj" : $(SOURCE) $(DEP_CPP_NYM_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\menunym.c
DEP_CPP_MENUN=\
	".\config.h"\
	".\menu.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\menunym.obj" : $(SOURCE) $(DEP_CPP_MENUN) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
