# Microsoft Developer Studio Project File - Name="Jacc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Jacc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Jacc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Jacc.mak" CFG="Jacc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Jacc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Jacc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Jacc - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Jacc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\..\.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Jacc - Win32 Release"
# Name "Jacc - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\util\src\assert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grammar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grammar.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_clos.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc_clos.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_glue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc_glue.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_jamp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc_jamp.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_lexer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc_lexer.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_props.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_render_opts.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_spacer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc_spacer.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_symtype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\jacc_symtype.h
# End Source File
# Begin Source File

SOURCE=..\..\jacc_tab.c
# End Source File
# Begin Source File

SOURCE=..\..\jacc_tab.h
# End Source File
# Begin Source File

SOURCE=..\..\lalr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lalr.h
# End Source File
# Begin Source File

SOURCE=..\..\lalr_print.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lalr_print.h
# End Source File
# Begin Source File

SOURCE=..\..\prio.h
# End Source File
# End Target
# End Project
