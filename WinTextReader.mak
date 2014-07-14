# Microsoft Developer Studio Generated NMAKE File, Based on WinTextReader.dsp
!IF "$(CFG)" == ""
CFG=WinTextReader - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WinTextReader - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WinTextReader - Win32 Release" && "$(CFG)" != "WinTextReader - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinTextReader.mak" CFG="WinTextReader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinTextReader - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WinTextReader - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "WinTextReader - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\WinTextReader.exe"


CLEAN :
	-@erase "$(INTDIR)\dlgtxtctrl.obj"
	-@erase "$(INTDIR)\txtread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\voiceclass.obj"
	-@erase "$(INTDIR)\WinTextReader.obj"
	-@erase "$(INTDIR)\WinTextReader.res"
	-@erase "$(OUTDIR)\WinTextReader.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "D:\Program Files\Microsoft Speech SDK 5.1\include" /I "aspell-dev-0-50-3-3\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\WinTextReader.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinTextReader.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\WinTextReader.pdb" /machine:I386 /out:"$(OUTDIR)\WinTextReader.exe" /libpath:"D:\Program Files\Microsoft Speech SDK 5.1\Lib\i386" 
LINK32_OBJS= \
	"$(INTDIR)\dlgtxtctrl.obj" \
	"$(INTDIR)\txtread.obj" \
	"$(INTDIR)\voiceclass.obj" \
	"$(INTDIR)\WinTextReader.obj" \
	"$(INTDIR)\WinTextReader.res"

"$(OUTDIR)\WinTextReader.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinTextReader - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\WinTextReader.exe" "$(OUTDIR)\WinTextReader.bsc"


CLEAN :
	-@erase "$(INTDIR)\dlgtxtctrl.obj"
	-@erase "$(INTDIR)\dlgtxtctrl.sbr"
	-@erase "$(INTDIR)\txtread.obj"
	-@erase "$(INTDIR)\txtread.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\voiceclass.obj"
	-@erase "$(INTDIR)\voiceclass.sbr"
	-@erase "$(INTDIR)\WinTextReader.obj"
	-@erase "$(INTDIR)\WinTextReader.res"
	-@erase "$(INTDIR)\WinTextReader.sbr"
	-@erase "$(OUTDIR)\WinTextReader.bsc"
	-@erase "$(OUTDIR)\WinTextReader.exe"
	-@erase "$(OUTDIR)\WinTextReader.ilk"
	-@erase "$(OUTDIR)\WinTextReader.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "D:\Program Files\Microsoft Speech SDK 5.1\include" /I "aspell-dev-0-50-3-3\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\WinTextReader.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinTextReader.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dlgtxtctrl.sbr" \
	"$(INTDIR)\txtread.sbr" \
	"$(INTDIR)\voiceclass.sbr" \
	"$(INTDIR)\WinTextReader.sbr"

"$(OUTDIR)\WinTextReader.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\WinTextReader.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WinTextReader.exe" /pdbtype:sept /libpath:"D:\Program Files\Microsoft Speech SDK 5.1\Lib\i386" 
LINK32_OBJS= \
	"$(INTDIR)\dlgtxtctrl.obj" \
	"$(INTDIR)\txtread.obj" \
	"$(INTDIR)\voiceclass.obj" \
	"$(INTDIR)\WinTextReader.obj" \
	"$(INTDIR)\WinTextReader.res"

"$(OUTDIR)\WinTextReader.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("WinTextReader.dep")
!INCLUDE "WinTextReader.dep"
!ELSE 
!MESSAGE Warning: cannot find "WinTextReader.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinTextReader - Win32 Release" || "$(CFG)" == "WinTextReader - Win32 Debug"
SOURCE=.\dlgtxtctrl.cpp

!IF  "$(CFG)" == "WinTextReader - Win32 Release"


"$(INTDIR)\dlgtxtctrl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTextReader - Win32 Debug"


"$(INTDIR)\dlgtxtctrl.obj"	"$(INTDIR)\dlgtxtctrl.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\txtread.cpp

!IF  "$(CFG)" == "WinTextReader - Win32 Release"


"$(INTDIR)\txtread.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTextReader - Win32 Debug"


"$(INTDIR)\txtread.obj"	"$(INTDIR)\txtread.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\voiceclass.cpp

!IF  "$(CFG)" == "WinTextReader - Win32 Release"


"$(INTDIR)\voiceclass.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTextReader - Win32 Debug"


"$(INTDIR)\voiceclass.obj"	"$(INTDIR)\voiceclass.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\WinTextReader.cpp

!IF  "$(CFG)" == "WinTextReader - Win32 Release"


"$(INTDIR)\WinTextReader.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTextReader - Win32 Debug"


"$(INTDIR)\WinTextReader.obj"	"$(INTDIR)\WinTextReader.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\WinTextReader.rc

"$(INTDIR)\WinTextReader.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

