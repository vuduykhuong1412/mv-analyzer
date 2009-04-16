; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMVAnalyzerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MVAnalyzer.h"

ClassCount=3
Class1=CMVAnalyzerApp
Class2=CMVAnalyzerDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MVANALYZER_DIALOG
Resource4=IDD_SETTING

[CLS:CMVAnalyzerApp]
Type=0
HeaderFile=MVAnalyzer.h
ImplementationFile=MVAnalyzer.cpp
Filter=N

[CLS:CMVAnalyzerDlg]
Type=0
HeaderFile=MVAnalyzerDlg.h
ImplementationFile=MVAnalyzerDlg.cpp
Filter=D
LastObject=IDC_PLAYBACK_LARGE
BaseClass=cdxCSizingDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=MVAnalyzerDlg.h
ImplementationFile=MVAnalyzerDlg.cpp
Filter=D
LastObject=IDOK

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MVANALYZER_DIALOG]
Type=1
Class=CMVAnalyzerDlg
ControlCount=31
Control1=IDC_PLAYBACK_A,static,1350701068
Control2=IDC_PLAY,button,1342373889
Control3=IDC_PROGRESS,msctls_trackbar32,1342177304
Control4=IDC_PLAYBACK_B,static,1350701068
Control5=IDC_FRAME_NUM,static,1342312449
Control6=IDC_SETTING,button,1342308352
Control7=IDC_MV,button,1342177283
Control8=IDC_GRID,button,1342177283
Control9=IDC_YUV,button,1342177280
Control10=IDC_PLAYBACK_AA,static,1350696972
Control11=IDC_PLAYBACK_BB,static,1350696972
Control12=IDC_PLAYBACK_BB_UP,button,1342210048
Control13=IDC_PLAYBACK_BB_DOWN,button,1342210048
Control14=IDC_PLAYBACK_BB_RIGHT,button,1342210048
Control15=IDC_PLAYBACK_BB_LEFT,button,1342210048
Control16=IDC_PREV,button,1342177280
Control17=IDC_NEXT,button,1342177280
Control18=IDC_FILE_OPEN,button,1342177280
Control19=IDC_SIGN,button,1342177283
Control20=IDC_STATIC_MINSAD,static,1342308352
Control21=IDC_OPT_SAD,static,1342308353
Control22=IDC_STATIC_CURRSAD,static,1342308352
Control23=IDC_Curr_SAD,static,1342308353
Control24=IDC_STATIC_SADDIFF,static,1342308352
Control25=IDC_SAD_DIFF,static,1342308353
Control26=IDC_STATIC_CURRBLK,static,1342308352
Control27=IDC_CURR_BLK_X,static,1342308353
Control28=IDC_CURR_BLK_Y,static,1342308353
Control29=IDC_MV_SCALE,button,1342308352
Control30=IDC_FULL_SCREEN,button,1342308352
Control31=IDC_PLAYBACK_LARGE,static,1350701068

[DLG:IDD_SETTING]
Type=1
Class=?
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SIZE_LIST,combobox,1344339970
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_SIZE_WIDTH,edit,1350631552
Control8=IDC_SIZE_HEIGHT,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_FPS_LIST,combobox,1344339971

