

#
# *** Iris Explorer Release 5.0 ***
#
# NMAKE makefile for InsertInteractor - generated using module_make.awk
#

# System specific options for Alpha architecture:
!IF "$(PROCESSOR_ARCHITECTURE)" == "ALPHA"
COPTIONS=/nologo /W2 /GX /YX /QAieee /c
FOPTIONS=/compile_only
FL=f77.exe
LINK32_FOR=Dfordll.lib
!IF "$(DEBUG)" == "1"
CDEBUGFLAGS=/Zi /Od /D "_DEBUG"
FDEBUGFLAGS=/debug
NTLIBSUBDIR=AlphaDebug
!ELSE
CDEBUGFLAGS=/O2 /D "NDEBUG"
FDEBUGFLAGS=/optimize:5
NTLIBSUBDIR=AlphaRelease
!ENDIF
MACHINE=ALPHA
!ENDIF

# System specific options for Intel (x86) architecture:
!IF "$(PROCESSOR_ARCHITECTURE)" == "x86"
COPTIONS=/nologo /W2 /GX /YX /c
FOPTIONS=/nologo /c /W0 /libs:dll /threads /I $(EXPLORERHOME)\include\cx /I $(EXPLORERUSERHOME)\include\cx
FL=df.exe
!IF "$(DEBUG)" == "1"
NTLIBSUBDIR=IntelDebug
CDEBUGFLAGS=/Zi /D "_DEBUG"
FDEBUGFLAGS=/Zi
!ELSE
NTLIBSUBDIR=IntelRelease
CDEBUGFLAGS=/O2 /D "NDEBUG"
FDEBUGFLAGS=
!ENDIF
MACHINE=I386
!ENDIF

# Add Explorer Include and Lib directories to search paths
CXSYSINCLUDE=.
CXSRCINCLUDE=.
INCLUDE=$(EXPLORERUSERHOME)\include;$(EXPLORERHOME)\include;$(INCLUDE)
LIB    =$(EXPLORERUSERHOME)\lib;$(EXPLORERHOME)\lib;$(LIB);c:\usr\local\lib

# C Compiler defines
STD_DEFINES=/D "WIN32"

# C++ Compiler defines
CPPDEFINES=

# C/C++ code generation
!IF "$(DEBUG)" == "1" && "$(EXPLORERDEVELOPER)" == "1"
RTLFLAG=/MDd
LINK32_OPTIONS=/nodefaultlib:"libcd.lib" /nodefaultlib:"MSVCRT.lib"
!ELSE
RTLFLAG=/MD
LINK32_OPTIONS=/nodefaultlib:"libc.lib" /nodefaultlib:"MSVCRTD.lib"
!ENDIF

# Libraries for using OpenGL
LIBGLW = glu32.lib opengl32.lib

# Libraries need to build Geometry Modules
!IF "$(DEBUG)" == "1" && "$(EXPLORERDEVELOPER)" == "1"
GEOMETRYLIBS= geometryD.lib inv252D.lib $(LIBGLW)
NAGGRAPHICSLIBG=naggl04D.lib naghnsGD.lib

# Libraries need to build fortran modules
FORTRANLIBS=fortranApiD.lib $(LINK32_FOR)
!ELSE
NAGGRAPHICSLIBG=naggl04.lib naghnsG.lib
GEOMETRYLIBS= geometry.lib inv252.lib $(LIBGLW)

# Libraries need to build fortran modules
FORTRANLIBS=fortranApi.lib $(LINK32_FOR)
!ENDIF
NAGGRAPHICSLIB=$(NAGGRAPHICSLIBG)

# Header files need to build Inventor or Geometry Modules
GEOMETRYINCS=.

# Libraries needed to build ImageVision modules
ILLIB = libil.lib
ILINC = $(ILROOT)\include

# Flags used by IRIS Explorer
NAGCINC=.
NAGCLIB=
LIBSUBDIR=$(NTLIBSUBDIR)

CPP=cl.exe
LINK32=link.exe
TOOLSDIR1=$(EXPLORERHOME)\bin
TOOLSDIR2=$(EXPLORERHOME)\lib
TOOLSDIR3=$(EXPLORERHOME)\lib\cygwin
ALL:: "InsertInteractor.exe" "InsertInteractor.help"
CPP_FLAGS=$(COPTIONS) $(STD_DEFINES) $(CPPDEFINES) $(CDEBUGFLAGS) $(DLL_FLAGS) $(RTLFLAG)
FL_FLAGS=$(FOPTIONS) $(FDEBUGFLAGS)

CPP_INCLUDE = 

SUBSYSTEM   = console
CX_MAIN     = "$(EXPLORERHOME)\lib\main50.obj"
CX_MAIN_DBG = "$(EXPLORERHOME)\lib\main50D.obj"
EXTRA_LIBS=
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
             advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
             odbc32.lib odbccp32.lib wsock32.lib\
             $(LINK32_OPTIONS) $(EXTRA_LIBS)\
             /nologo /subsystem:$(SUBSYSTEM) /incremental:yes /debug /machine:$(MACHINE)\
             /out:"InsertInteractor.nxe"

LINK32_OBJS =  InsertInteractor.cx.obj SoFotis3Dragger.obj draggers_gen.obj

!IF "$(DEBUG)" == "1" && "$(EXPLORERDEVELOPER)" == "1"
LINK32_DLL = "$(EXPLORERHOME)\lib\mcw50D.lib"
MAIN_LIB   = $(CX_MAIN_DBG)
!ELSE
LINK32_DLL = "$(EXPLORERHOME)\lib\mcw50.lib"
MAIN_LIB   = $(CX_MAIN)
!ENDIF

LINK32_DEFAULT = $(MAIN_LIB) $(GEOMETRYLIBS)

"InsertInteractor.exe" : $(LINK32_OBJS)
     $(LINK32) @<< 
     $(LINK32_FLAGS) $(LINK32_DEFAULT) $(LINK32_OBJS) $(LINK32_DLL) 
<<
     -@ erase InsertInteractor.exe 
     -@ move InsertInteractor.nxe InsertInteractor.exe 
     @if exist InsertInteractor.nxe echo Module executable is in use so cannot be overwritten.
     @if exist InsertInteractor.nxe echo It will be replaced by the new version at the next available opportunity.
     @if exist InsertInteractor.nxe echo E.g. if a single module is running, when it is replaced in the Map Editor.

"InsertInteractor.doc" : 
   if not exist "InsertInteractor.doc" "$(TOOLSDIR2)\cxMbDoc" InsertInteractor

"InsertInteractor.help" : InsertInteractor.doc
   "$(TOOLSDIR2)\cxMbDoc" -n InsertInteractor | "$(TOOLSDIR3)\groff" -mtty-char -Tascii -man | "$(TOOLSDIR2)\col" > InsertInteractor.help
"InsertInteractor.cx.c" : InsertInteractor.mres
   "$(TOOLSDIR2)\cxMdw" -cx InsertInteractor

# Special rule for .C files: 
draggers_gen.obj: draggers_gen.C
      copy draggers_gen.C draggers_gen.cxx
      $(CPP) $(CPP_FLAGS) $(CPP_INCLUDE) draggers_gen.cxx


# Special rule for .c++ files: 

.c.obj:
    $(CPP) $(CPP_FLAGS) $(CPP_INCLUDE) $<

.cxx.obj:
    $(CPP) $(CPP_FLAGS) $(CPP_INCLUDE) $<

.cpp.obj:
    $(CPP) $(CPP_FLAGS) $(CPP_INCLUDE) $<

.f.obj:
    $(CPP) /EP /C /DWIN32 $< > $*.tmp.f 
    $(FL) $(FL_FLAGS) $(FL_INCLUDE) $*.tmp.f
    IF EXIST $*.obj erase $*.obj
    move $*.tmp.obj $*.obj

.for.obj:
    $(FL) $(FL_FLAGS) $(CPP_INCLUDE) $<

.rc.res:
    rc -r $<

CLEAN::
         -@IF EXIST *.obj erase *.obj
         -@IF EXIST *.res erase *.res
         -@IF EXIST InsertInteractor.ilk  erase  InsertInteractor.ilk
         -@IF EXIST InsertInteractor.pdb  erase  InsertInteractor.pdb
         -@IF EXIST InsertInteractor.nxe  erase  InsertInteractor.nxe
         -@IF EXIST InsertInteractor.exe  erase  InsertInteractor.exe
         -@IF EXIST InsertInteractor.cx.c erase  InsertInteractor.cx.c
         -@IF EXIST df60.pch erase df60.pch
         -@IF EXIST df60.pdb erase df60.pdb
         -@IF EXIST vc60.pch erase vc60.pch
         -@IF EXIST vc60.pdb erase vc60.pdb
         -@IF EXIST draggers_gen.cxx erase draggers_gen.cxx
INSTALL::ALL
         if not exist "$(EXPLORERUSERHOME)" mkdir "$(EXPLORERUSERHOME)"
         if not exist "$(EXPLORERUSERHOME)\modules" mkdir "$(EXPLORERUSERHOME)\modules"
         echo installing InsertInteractor
         copy InsertInteractor.mres "$(EXPLORERUSERHOME)\modules"
         copy InsertInteractor.help "$(EXPLORERUSERHOME)\modules"
         if exist InsertInteractor.credit copy InsertInteractor.credit "$(EXPLORERUSERHOME)\modules"
         if not exist InsertInteractor.credit if not "$(CXCREDITS)" == "" "$(TOOLSDIR3)\ln" -sf $(CXCREDITS) "$(EXPLORERUSERHOME)\modules\InsertInteractor.credit"
         call $(EXPLORERHOME)/lib/cx_install_module.bat InsertInteractor.nxe InsertInteractor.exe
