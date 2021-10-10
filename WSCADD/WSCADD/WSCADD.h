
// WSCADD.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CWSCADDApp:
// Сведения о реализации этого класса: WSCADD.cpp
//

class CWSCADDApp : public CWinApp
{
public:
	CWSCADDApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CWSCADDApp theApp;
