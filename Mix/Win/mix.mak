# mix.exe makefile

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Mix - Win32 Release
!MESSAGE No configuration specified.  Defaulting to Mix - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Mix - Win32 Release" && "$(CFG)" != "Mix - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mix.mak" CFG="Mix - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mix - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Mix - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "Mix - Win32 Debug"
CPP=cl.exe
RSC=rc.exe

MIXOPTS=/D "USE_SOCK" /D "WIN32" /D "_MSC" /D "WIN32SERVICE" /D "_MBCS" /D "_CONSOLE"

!IF  "$(CFG)" == "Mix - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Mix.exe"

CLEAN : 
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\service.obj"
	-@erase "$(OUTDIR)\Mix.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W2 /GX /O2 /D "NDEBUG" /Fp"$(INTDIR)/Mix.pch" /YX\
 /Fo"$(INTDIR)/" /c 

CPP_OBJS=.\Release/
CPP_SBRS=.\.

BSC32=bscmake.exe

BSC32_FLAGS=/nologo /o"$(OUTDIR)/Mix.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe

LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib C:\mixmaster-2.9.0\Src\Release\mixlib.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/Mix.pdb" /machine:I386\
 /out:"$(OUTDIR)/Mix.exe" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\service.obj"

"$(OUTDIR)\Mix.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS) C:\mixmaster-2.9.0\Src\Release\mixlib.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Mix - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Mix.exe"

CLEAN : 
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\service.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Mix.exe"
	-@erase "$(OUTDIR)\Mix.ilk"
	-@erase "$(OUTDIR)\Mix.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W2 /Gm /GX /Zi /Od /D "_DEBUG" /Fp"$(INTDIR)/Mix.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 

CPP_OBJS=.\Debug/
CPP_SBRS=.\.

BSC32=bscmake.exe

BSC32_FLAGS=/nologo /o"$(OUTDIR)/Mix.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe

LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib C:\mixmaster-2.9.0\Src\Release\mixlib.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/Mix.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/Mix.exe" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\service.obj"

"$(OUTDIR)\Mix.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS) C:\mixmaster-2.9.0\Src\Release\mixlib.lib
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $(MIXOPTS) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $(MIXOPTS) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $(MIXOPTS) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $(MIXOPTS) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $(MIXOPTS) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $(MIXOPTS) $<  

################################################################################
# Begin Source File

SOURCE=.\main.c
DEP_CPP_MAIN_=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\service.c
DEP_CPP_SERVI=\
	".\config.h"\
	".\mix.h"\
	".\mix3.h"\
	".\version.h"\
	

"$(INTDIR)\service.obj" : $(SOURCE) $(DEP_CPP_SERVI) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
