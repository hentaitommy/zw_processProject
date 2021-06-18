// processProject.h : main header file for the processProject DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CprocessProjectApp
// See processProject.cpp for the implementation of this class
//

class CprocessProjectApp : public CWinApp
{
public:
	CprocessProjectApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
