// MVAnalyzer.h : main header file for the MVANALYZER application
//

#if !defined(AFX_MVANALYZER_H__0CACBD6F_9644_4E0E_8513_89AC5774453B__INCLUDED_)
#define AFX_MVANALYZER_H__0CACBD6F_9644_4E0E_8513_89AC5774453B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMVAnalyzerApp:
// See MVAnalyzer.cpp for the implementation of this class
//

class CMVAnalyzerApp : public CWinApp
{
public:
	CMVAnalyzerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMVAnalyzerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMVAnalyzerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MVANALYZER_H__0CACBD6F_9644_4E0E_8513_89AC5774453B__INCLUDED_)
