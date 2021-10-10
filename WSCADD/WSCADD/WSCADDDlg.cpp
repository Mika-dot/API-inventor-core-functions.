
// WSCADDDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "WSCADD.h"
#include "WSCADDDlg.h"
#include "afxdialogex.h"
#include "math.h"

#define PI 4*atan(1)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma once

#import "C:\Program Files\Autodesk\Inventor 2022\Bin\RxInventor.tlb" \
rename_namespace("InventorNative") \
named_guids raw_dispinterfaces \
high_method_prefix("Method") \
rename("DeleteFile", "APIDeleteFile") \
rename("CopyFile", "APICopyFile") \
rename("MoveFile", "APIMoveFile") \
rename("SetEnvironmentVariable", "APISetEnvironmentVariable") \
rename("GetObject", "APIGetObject") \
exclude("_FILETIME", "IStream", "ISequentialStream", \
"_LARGE_INTEGER", "_ULARGE_INTEGER", "tagSTATSTG", \
"IEnumUnknown", "IPersistFile", "IPersist", "IPictureDisp")

//rename_namespace для удобства наши api будет меноваться спереди прописываться Method
//rename подменяет слова для удобство что бы небыла конфликтов с зарезервироваными
//exclude ууберает эти методы.

using namespace InventorNative;// пространства имен

CComPtr<Application> pInvApp; // приложение

PartDocumentPtr pPartDoc; // документ
PartComponentDefinitionPtr pPartComDef; // компоненты детали
TransientGeometryPtr pTransGeom; // геометрия детали
TransactionPtr pTrans; // операции

TransactionManagerPtr pTransManager; // менеджер операций

PlanarSketchesPtr sketches; // эскиз
PartFeaturesPtr ft; // элементы деталей

WorkPlanesPtr wp; // рабочая плоскость
WorkAxesPtr wax; // рабочие оси
WorkPointsPtr wpt; // рабочие точки

//CComPtr<ObjectCollection> objcollection;

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CWSCADDDlg



CWSCADDDlg::CWSCADDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WSCADD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWSCADDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWSCADDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CWSCADDDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CWSCADDDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CWSCADDDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CWSCADDDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CWSCADDDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CWSCADDDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// Обработчики сообщений CWSCADDDlg

BOOL CWSCADDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CWSCADDDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CWSCADDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CWSCADDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWSCADDDlg::OnBnClickedButton1()
{
	if (!pInvApp) { // указатель на приложение (в первый раз)

		CLSID InvAppClsid; // фамилия на программу
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid); // имя на программу // преврощения имени в фамилию



		if (FAILED(hRes)) // макрос на проверку корекности работы
		{
			pInvApp = nullptr; // обнуления указателя интентора
			return;
		}


		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk); // проверка на запуск инвентора (да,нет)

		if (FAILED(hRes))// макрос на проверку корекности работы
		{
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pInvAppUnk); // запуск инвентора // pInvAppUnk указатель на интрфейс

			if (FAILED(hRes))
			{
				pInvApp = nullptr;
				return;
			}
		}

		hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void**)&pInvApp); // добраться до всех интерфейсов  // имя интерфейса Application // pInvApp куда записываем

		if (FAILED(hRes))
		{
			return;
		}

	}

	pInvApp->put_Visible(TRUE); // вывод на экран инвентор // TRUE отображение
	
	pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);
	// создания документа // указатель pInvApp // Documents все открытые документы // MethodAdd новый документ с параметрами // kPartDocumentObject деталь // pInvApp указатель // указатель на файл FileManager // MethodGetTemplateFile характеристики документа // kPartDocumentObject деталь // kMetricSystemOfMeasure сестема си //kGOST_DraftingStandard по госту // 
	// pPartDocссылка на новый документ


	pPartDoc->DisplayName = _T("Конфиг"); // добавили документ 
	
	pPartDoc->get_ComponentDefinition(&pPartComDef); // инцилизация функции по ссылки (имя файла)




	pInvApp->get_TransientGeometry(&pTransGeom); // инцилизация функции по ссылки



	pTransManager = pInvApp->GetTransactionManager(); // инцилизация функции по ссылки

	DocumentPtr Doc = CComQIPtr <Document>(pPartDoc); // приводим к нужному типу

	pTransManager->raw_StartTransaction(Doc, _T("Конфиг"), &pTrans); // запус менеджера операций  // указатель на документ

	AfxMessageBox(L"Успешно"); // если успешно

//
//
//	// TODO: добавьте свой код обработчика уведомлений
}


void CWSCADDDlg::OnBnClickedButton2()
{
	// рис 1
	BeginWaitCursor(); // 

	pPartComDef->get_Features(&ft); //инцилизация на элементы детали
	pPartComDef->get_Sketches(&sketches); //инцилизация коллекция эскизов


	pPartComDef->get_WorkPlanes(&wp);//инцилизация коллекция плоскости
	pPartComDef->get_WorkAxes(&wax);//инцилизация коллекция оси
	pPartComDef->get_WorkPoints(&wpt);//инцилизация коллекция точки


	PlanarSketchPtr pSketch; // обявления указателя на плоские эскизы
	sketches->raw_Add(wp->GetItem(2), false, &pSketch); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз

	SketchPointsPtr skPoints; // обявления коллекций точек (контейнер)
	SketchLinesPtr skLines; // линий
	SketchCirclesPtr skCircles;// окружностей
	SketchArcsPtr skArcs;// дук окружности

	pSketch->get_SketchPoints(&skPoints); // иницилизация контейнера выше описаного
	pSketch->get_SketchLines(&skLines);
	pSketch->get_SketchCircles(&skCircles);
	pSketch->get_SketchArcs(&skArcs);

	SketchPointPtr point[7]; // указатели на колличество их и к ним можно обращатся дальше
	SketchLinePtr lines[4];
	SketchCirclePtr circ; // единичная если надо много то circ[5]
	SketchArcPtr arc;

	// добавления точек
	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 0), false); // skPoints эскиз // MethodAdd добавления новой точки  //(0, 0) координаты
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(9, 0), false); // это сантиметры, а не метры или милиметры
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(10, 1), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(10, 7), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 3), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(9, 1), false);
	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(6.5, 3), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]); // проводится линии 
	lines[1] = skLines->MethodAddByTwoPoints(point[2], point[3]); // skLines эскиз // MethodAddByTwoPoints соединения точек линиями // point[2], point[3] точки
	lines[2] = skLines->MethodAddByTwoPoints(point[3], point[4]);
	lines[3] = skLines->MethodAddByTwoPoints(point[4], point[0]);

	// коллекция окружностей skCircles
	circ = skCircles->MethodAddByCenterRadius(point[6], 1); // построения окружностей //MethodAddByCenterRadius добавления окружности по центру точки и значением радиуса // центр окружности point[6] // радиус 1

	// коллекция дуг skArcs
	arc = skArcs->MethodAddByCenterStartEndPoint(point[5], point[1], point[2], true);//  дуга окружности // MethodAddByCenterStartEndPoint дуга по центру с ограничением по точкам
	// point[5] центр
	// ограничения дуги point[1], point[2],
	// true направления по часовой стрелки 



	// 3d эскиз

	Sketches3DPtr p3DSketches; // 3D ЭСКИЗЫ
	Sketch3DPtr p3DSketch;// указатели на 3d
	SketchPoints3DPtr p3DPoints;// указатели на 3d точки


	SketchSplines3DPtr pSketchSplines;// указатель на коллекцию сплайов
	SketchSpline3DPtr pSketchSpline;// на конкретный сплайн


	pPartComDef->get_Sketches3D(&p3DSketches); // инициализация всех переменых
	p3DSketches->raw_Add(&p3DSketch);

	p3DSketch->get_SketchSplines3D(&pSketchSplines);
	p3DSketch->get_SketchPoints3D(&p3DPoints);

	ObjectCollectionPtr objCollection = pInvApp->TransientObjects->MethodCreateObjectCollection(); // коллекция обьектов инициализация


	for (double fi = 0, z = 0; fi < 2 * PI; fi += (PI / 20), z +=0.1f)
	{
		double R = 1 + cos(3 * fi) + sin(3 * fi) * sin(3 * fi);

		PointPtr pt = pTransGeom->MethodCreatePoint(R * cos(fi), R * sin(fi), z); // координаты по x,y,z

		SketchPoint3DPtr pt3D = p3DPoints->MethodAdd(pt, false); // заносим в коллекцию
		
		objCollection->raw_Add(pt);// добавление
	} 


	pSketchSpline = pSketchSplines->MethodAdd(objCollection, kSmoothSplineFit); // строем сплайн (видно соединяем все точки линиями)

	pTrans->MethodEnd();

	// TODO: добавьте свой код обработчика уведомлений
}


void CWSCADDDlg::OnBnClickedButton3()
{
	BeginWaitCursor(); // 

	pPartComDef->get_Features(&ft); //инцилизация на элементы детали
	pPartComDef->get_Sketches(&sketches); //инцилизация коллекция эскизов


	pPartComDef->get_WorkPlanes(&wp);//инцилизация коллекция плоскости
	pPartComDef->get_WorkAxes(&wax);//инцилизация коллекция оси
	pPartComDef->get_WorkPoints(&wpt);//инцилизация коллекция точки

	SketchPointsPtr skPoints; // обявления коллекций точек (контейнер)
	SketchLinesPtr skLines; // линий
	ProfilesPtr skProfiles;

	PlanarSketchPtr pSketch; // обявления указателя на плоские эскизы
	sketches->raw_Add(wp->GetItem(2), false, &pSketch); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз

	pSketch->get_SketchPoints(&skPoints); // иницилизация контейнера выше описаного
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);

	SketchPointPtr point[8]; // указатели на колличество их и к ним можно обращатся дальше
	SketchLinePtr lines[8];


	// добавления точек
	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0.7, 0), false); // skPoints эскиз // MethodAdd добавления новой точки  //(0, 0) координаты
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0.7, 1.8f), false); // это сантиметры, а не метры или милиметры
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(2.2f, 1.8f), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(2.2f, 6.6f), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-2.2f, 6.6f), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-2.2f, 1.8f), false);
	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.7, 1.8f), false);
	point[7] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.7, 0), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]); // проводится линии 
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]); // skLines эскиз // MethodAddByTwoPoints соединения точек линиями // point[2], point[3] точки
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[4]);
	lines[4] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[5] = skLines->MethodAddByTwoPoints(point[5], point[6]);
	lines[6] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[7] = skLines->MethodAddByTwoPoints(point[7], point[0]);


	// проверка на правильность для операции выдавливания
	ProfilePtr pProfile; 
	try {
		pProfile = skProfiles->MethodAddForSolid(true); // MethodAddForSolid операция выдавливания // если вызвала програмное прерывания => эскиз на верен
		// true поглощаются контуа друг в друге 
		// цилинр в цилиндре => одна окружность
		// false  цилинр в цилиндре => окружность с дыркой из малой окружности
	}
	catch (...) { // отлавливания програмной прирывания
		AfxMessageBox(L"Ошибочный контур!");
		return;
	}

	ExtrudeFeaturesPtr ftExtrude; // операция выдавливания
	ft->get_ExtrudeFeatures(&ftExtrude); // инициализация колекции 

	// ftExtrude колекция этих операторов
	// метод выдавливания
	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile, 4.6f, kPositiveExtentDirection, kJoinOperation);
	// pProfile профиль (он зазивал)
	//  4.6f растояние 46 мм
	// kPositiveExtentDirection направления выдавливания есть kNegativeExtentDirection  // в обе стороны kSymmetricExtentDirection
	// kJoinOperation добавления матерьяла 

	//------------------------------------------------------------------------------------все норм сверху

	PlanarSketchPtr pSketch1; // обявления указателя на плоские эскизы
	sketches->raw_Add(wp->GetItem(1), false, &pSketch1); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз

	SketchPointsPtr skPoints1; // обявления коллекций точек (контейнер)
	SketchLinesPtr skLines1; // линий
	ProfilesPtr skProfiles1;

	pSketch1->get_SketchPoints(&skPoints1); // иницилизация контейнера выше описаного
	pSketch1->get_SketchLines(&skLines1);
	pSketch1->get_Profiles(&skProfiles1);

	SketchPointPtr point1[6]; // указатели на колличество их и к ним можно обращатся дальше
	SketchLinePtr lines1[6];


	// добавления точек
	point1[0] = skPoints1->MethodAdd(pTransGeom->MethodCreatePoint2d(2.5f, 0.f), false); // skPoints эскиз // MethodAdd добавления новой точки  //(0, 0) координаты
	point1[1] = skPoints1->MethodAdd(pTransGeom->MethodCreatePoint2d(2.5f, 3.2f), false); // это сантиметры, а не метры или милиметры
	point1[2] = skPoints1->MethodAdd(pTransGeom->MethodCreatePoint2d(1.2f, 3.2f), false);
	point1[3] = skPoints1->MethodAdd(pTransGeom->MethodCreatePoint2d(1.2f, 5.4f), false);
	point1[4] = skPoints1->MethodAdd(pTransGeom->MethodCreatePoint2d(4.6f, 5.4f), false);
	point1[5] = skPoints1->MethodAdd(pTransGeom->MethodCreatePoint2d(4.6f, 0.f), false);
	

	lines1[0] = skLines1->MethodAddByTwoPoints(point1[0], point1[1]); // проводится линии 
	lines1[1] = skLines1->MethodAddByTwoPoints(point1[1], point1[2]); // skLines эскиз // MethodAddByTwoPoints соединения точек линиями // point[2], point[3] точки
	lines1[2] = skLines1->MethodAddByTwoPoints(point1[2], point1[3]);
	lines1[3] = skLines1->MethodAddByTwoPoints(point1[3], point1[4]);
	lines1[4] = skLines1->MethodAddByTwoPoints(point1[4], point1[5]);
	lines1[5] = skLines1->MethodAddByTwoPoints(point1[5], point1[0]);



	// проверка на правильность для операции выдавливания
	ProfilePtr pProfile1;
	try {
		pProfile1 = skProfiles1->MethodAddForSolid(true); // MethodAddForSolid операция выдавливания // если вызвала програмное прерывания => эскиз на верен
		// true поглощаются контуа друг в друге 
		// цилинр в цилиндре => одна окружность
		// false  цилинр в цилиндре => окружность с дыркой из малой окружности
	}
	catch (...) { // отлавливания програмной прирывания
		AfxMessageBox(L"Ошибочный контур!");
		return;
	}

	// ftExtrude колекция этих операторов
	// метод выдавливания
	ExtrudeFeaturePtr extrude1 = ftExtrude->MethodAddByThroughAllExtent(pProfile1, kSymmetricExtentDirection, kCutOperation);
	// MethodAddByThroughAllExtent вырез на сквозь до конца
	// pProfile профиль (он зазивал)
	//  4.6f растояние 46 мм
	// kPositiveExtentDirection направления выдавливания есть kNegativeExtentDirection  // в обе стороны kSymmetricExtentDirection
	// kCutOperation убовляет матерьяла 



	pTrans->MethodEnd();
}


void CWSCADDDlg::OnBnClickedButton4()
{
	
	BeginWaitCursor(); // 

	SketchPointsPtr skPoints; // обявления коллекций точек (контейнер)
	SketchLinesPtr skLines; // линий
	ProfilesPtr skProfiles;

	pPartComDef->get_Features(&ft); //инцилизация на элементы детали
	pPartComDef->get_Sketches(&sketches); //инцилизация коллекция эскизов

	pPartComDef->get_WorkPlanes(&wp);//инцилизация коллекция плоскости
	pPartComDef->get_WorkAxes(&wax);//инцилизация коллекция оси
	pPartComDef->get_WorkPoints(&wpt);//инцилизация коллекция точки


	PlanarSketchPtr pSketch; // обявления указателя на плоские эскизы
	sketches->raw_Add(wp->GetItem(3), false, &pSketch); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз

	pSketch->get_SketchPoints(&skPoints); // иницилизация контейнера выше описаного
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);

	SketchPointPtr point[10]; // указатели на колличество их и к ним можно обращатся дальше
	SketchLinePtr lines[10];


	// добавления точек
	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 0.3f), false); // skPoints эскиз // MethodAdd добавления новой точки  //(0, 0) координаты
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 1.f), false); // это сантиметры, а не метры или милиметры
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(2.f, 1.5f), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(2.f, 2.f), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.5f, 2.f), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.5f, 0.8f), false);
	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.55f, 0.8f), false);
	point[7] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.55f, 0.85f), false);
	point[8] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(7.55f, 0.85f), false);
	point[9] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(7.55f, 0.3f), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]); // проводится линии 
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]); // skLines эскиз // MethodAddByTwoPoints соединения точек линиями // point[2], point[3] точки
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[4]);
	lines[4] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[5] = skLines->MethodAddByTwoPoints(point[5], point[6]);
	lines[6] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[7] = skLines->MethodAddByTwoPoints(point[7], point[8]);
	lines[8] = skLines->MethodAddByTwoPoints(point[8], point[9]);
	lines[9] = skLines->MethodAddByTwoPoints(point[9], point[0]);


	// проверка на правильность для операции выдавливания
	ProfilePtr pProfile;
	try {
		pProfile = skProfiles->MethodAddForSolid(true); // MethodAddForSolid операция выдавливания // если вызвала програмное прерывания => эскиз на верен
		// true поглощаются контуа друг в друге 
		// цилинр в цилиндре => одна окружность
		// false  цилинр в цилиндре => окружность с дыркой из малой окружности
	}
	catch (...) { // отлавливания програмной прирывания
		AfxMessageBox(L"Ошибочный контур!");
		return;
	}

	RevolveFeaturesPtr ftRefolve; // операция выдавливания
	ft->get_RevolveFeatures(&ftRefolve); // инициализация колекции // get_RevolveFeatures вращение

	// ftExtrude колекция этих операторов
	// метод выдавливания
	RevolveFeaturePtr кefolve = ftRefolve->MethodAddFull(pProfile, wax->GetItem(1), kJoinOperation);
	// wax->GetItem(1) индекс оси вращения
	// pProfile контур
	// MethodAddFull вращение на 360
	// pProfile профиль (он зазивал)
	//  4.6f растояние 46 мм
	// kPositiveExtentDirection направления выдавливания есть kNegativeExtentDirection  // в обе стороны kSymmetricExtentDirection
	// kJoinOperation добавления матерьяла 


	
	pTrans->MethodEnd();



}


void CWSCADDDlg::OnBnClickedButton5()
{
	BeginWaitCursor(); // 

	SketchPointPtr point[4]; // обявления коллекций точек (контейнер)
	SketchLinePtr lines[4]; // линий
	SketchPointsPtr skPoints;
	SketchLinesPtr skLines;
	PlanarSketchPtr pSketch; // обявления указателя на плоские эскизы
	ProfilesPtr skProfiles;

	pPartComDef->get_Features(&ft); //инцилизация на элементы детали
	pPartComDef->get_Sketches(&sketches); //инцилизация коллекция эскизов

	pPartComDef->get_WorkPlanes(&wp);//инцилизация коллекция плоскости
	pPartComDef->get_WorkAxes(&wax);//инцилизация коллекция оси
	pPartComDef->get_WorkPoints(&wpt);//инцилизация коллекция точки

	sketches->raw_Add(wp->GetItem(3), false, &pSketch); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз

	pSketch->get_SketchPoints(&skPoints); // иницилизация контейнера выше описаного
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);


	// добавления точек
	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5f, 0.f), false); // skPoints эскиз // MethodAdd добавления новой точки  //(0, 0) координаты
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5f, 3.f), false); // это сантиметры, а не метры или милиметры
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5f, 3.f), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5f, 0.f), false);
	
	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]); // проводится линии 
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]); // skLines эскиз // MethodAddByTwoPoints соединения точек линиями // point[2], point[3] точки
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	// проверка на правильность для операции выдавливания
	ProfilePtr pProfile;
	try {
		pProfile = skProfiles->MethodAddForSolid(true); // MethodAddForSolid операция выдавливания // если вызвала програмное прерывания => эскиз на верен
		// true поглощаются контуа друг в друге 
		// цилинр в цилиндре => одна окружность
		// false  цилинр в цилиндре => окружность с дыркой из малой окружности
	}
	catch (...) { // отлавливания програмной прирывания
		AfxMessageBox(L"Ошибочный контур!");
		return;
	}

	ExtrudeFeaturesPtr ftExtrude; // операция выдавливания
	ft->get_ExtrudeFeatures(&ftExtrude); // инициализация колекции // get_RevolveFeatures вращение

	// ftExtrude колекция этих операторов
	// метод выдавливания
	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile, 0.5f, kPositiveExtentDirection, kJoinOperation);
	// wax->GetItem(1) индекс оси вращения
	// pProfile контур
	// MethodAddFull вращение на 360
	// pProfile профиль (он зазивал)
	//  4.6f растояние 46 мм
	// kPositiveExtentDirection направления выдавливания есть kNegativeExtentDirection  // в обе стороны kSymmetricExtentDirection
	// kJoinOperation добавления матерьяла 


	SketchCirclePtr Circ;
	SketchCirclesPtr skCircles;

	sketches->raw_Add(wp->GetItem(3), false, &pSketch); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз
	pSketch->get_SketchCircles(&skCircles);
	Circ = skCircles->MethodAddByCenterRadius(pTransGeom->MethodCreatePoint2d(0.f, 0.2f), 0.05f); // построения окружностей //MethodAddByCenterRadius добавления окружности по центру точки и значением радиуса // центр окружности point[6] // радиус 1

	ProfilesPtr skProfiles1;
	ProfilePtr pProfile1;
	
	pSketch->get_Profiles(&skProfiles1);

	// проверка на правильность для операции выдавливания
	try {
		pProfile1 = skProfiles1->MethodAddForSolid(true); // MethodAddForSolid операция выдавливания // если вызвала програмное прерывания => эскиз на верен
		// true поглощаются контуа друг в друге 
		// цилинр в цилиндре => одна окружность
		// false  цилинр в цилиндре => окружность с дыркой из малой окружности
	}
	catch (...) { // отлавливания програмной прирывания
		AfxMessageBox(L"Ошибочный контур!");
		return;
	}

	ExtrudeFeaturePtr extrude1 = ftExtrude->MethodAddByDistanceExtent(pProfile1, 1.5f, kPositiveExtentDirection, kJoinOperation);
	




	ObjectCollection* pCollection;// указатель на коллекцию

	pInvApp->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection); // инициализирую

	pCollection->MethodAdd(extrude1);// элемент отресовки

	RectangularPatternFeaturesPtr pRectPatFeat;

	ft->get_RectangularPatternFeatures(&pRectPatFeat);

	// описания самого массива
	// сначал  данные потом элемент
	RectangularPatternFeatureDefinitionPtr pp = pRectPatFeat->MethodCreateDefinition(pCollection, wax->GetItem(1), true, 7, 0.13f, kDefault);
	// pCollection коллекция обьектов
	//wax->GetItem(1) оси
	//true направление оси
	//0.13f растояние
	// 7 колличесто элементов


	// второе напровление
	pp->PutYDirectionEntity(wax->GetItem(2));// оси 
	pp->PutYCount(10);//колл элементов
	pp->PutYSpacing(0.3f);// растояние
	pp->PutYDirectionSpacingType(kDefault);//тип промежутков
	pp->PutNaturalYDirection(true);// направление
	pp->PutXDirectionMidPlanePattern(true);// режим семетричности в обе стороны

	RectangularPatternFeaturePtr rectFeat = pRectPatFeat->MethodAddByDefinition(pp);// создание массива по описаню

	pCollection->MethodClear(); // коллекция обектов
	pCollection->MethodAdd(rectFeat); // новые элементы
	pCollection->MethodAdd(extrude); // основания под массивам

	CircularPatternFeaturesPtr pCircPatFeat; // указатель на коллекцию

	ft->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeaturePtr circFeat = pCircPatFeat->MethodAdd(pCollection, wax->GetItem(3), true, 8, "360 град", true, kIdenticalCompute); // новый вращения
	// тут все понятно
	// kIdenticalCompute ничего не изменяется (в тупую)

	pTrans->MethodEnd();

}


void CWSCADDDlg::OnBnClickedButton6()
{
	
	BeginWaitCursor(); // 

	SketchPointsPtr skPoints; // обявления коллекций точек (контейнер)
	SketchLinesPtr skLines; // линий
	ProfilesPtr skProfiles;

	pPartComDef->get_Features(&ft); //инцилизация на элементы детали
	pPartComDef->get_Sketches(&sketches); //инцилизация коллекция эскизов

	pPartComDef->get_WorkPlanes(&wp);//инцилизация коллекция плоскости
	pPartComDef->get_WorkAxes(&wax);//инцилизация коллекция оси
	pPartComDef->get_WorkPoints(&wpt);//инцилизация коллекция точки


	PlanarSketchPtr pSketch; // обявления указателя на плоские эскизы
	sketches->raw_Add(wp->GetItem(3), false, &pSketch); // иницилизация // raw_Add новый эскиз // wp->GetItem(2) плоскость эскиза (2) индекс элемента -- 1=yz 2=xz 3=xy // &pSketch указатель на эскиз

	pSketch->get_SketchPoints(&skPoints); // иницилизация контейнера выше описаного
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);

	SketchPointPtr point[10]; // указатели на колличество их и к ним можно обращатся дальше
	SketchLinePtr lines[10];


	// добавления точек
	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 0.3f), false); // skPoints эскиз // MethodAdd добавления новой точки  //(0, 0) координаты
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 1.f), false); // это сантиметры, а не метры или милиметры
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(2.f, 1.5f), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(2.f, 2.f), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.5f, 2.f), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.5f, 0.8f), false);
	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.55f, 0.8f), false);
	point[7] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(3.55f, 0.85f), false);
	point[8] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(7.55f, 0.85f), false);
	point[9] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(7.55f, 0.3f), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]); // проводится линии 
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]); // skLines эскиз // MethodAddByTwoPoints соединения точек линиями // point[2], point[3] точки
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[4]);
	lines[4] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[5] = skLines->MethodAddByTwoPoints(point[5], point[6]);
	lines[6] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[7] = skLines->MethodAddByTwoPoints(point[7], point[8]);
	lines[8] = skLines->MethodAddByTwoPoints(point[8], point[9]);
	lines[9] = skLines->MethodAddByTwoPoints(point[9], point[0]);


	// проверка на правильность для операции выдавливания
	ProfilePtr pProfile;
	try {
		pProfile = skProfiles->MethodAddForSolid(true); // MethodAddForSolid операция выдавливания // если вызвала програмное прерывания => эскиз на верен
		// true поглощаются контуа друг в друге 
		// цилинр в цилиндре => одна окружность
		// false  цилинр в цилиндре => окружность с дыркой из малой окружности
	}
	catch (...) { // отлавливания програмной прирывания
		AfxMessageBox(L"Ошибочный контур!");
		return;
	}

	RevolveFeaturesPtr ftRefolve; // операция выдавливания
	ft->get_RevolveFeatures(&ftRefolve); // инициализация колекции // get_RevolveFeatures вращение

	// ftExtrude колекция этих операторов
	// метод выдавливания
	RevolveFeaturePtr кefolve = ftRefolve->MethodAddFull(pProfile, wax->GetItem(1), kJoinOperation);
	// wax->GetItem(1) индекс оси вращения
	// pProfile контур
	// MethodAddFull вращение на 360
	// pProfile профиль (он зазивал)
	//  4.6f растояние 46 мм
	// kPositiveExtentDirection направления выдавливания есть kNegativeExtentDirection  // в обе стороны kSymmetricExtentDirection
	// kJoinOperation добавления матерьяла 

	//***************************

	FilletFeaturesPtr pFilletFt;
	ft->get_FilletFeatures(&pFilletFt);

	EdgeCollectionPtr edgeColl;

	pInvApp->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);

	SurfaceBodyPtr SurfBody;// указатели на все твердые тела
	SurfaceBodiesPtr SurfBodies;

	pPartComDef->get_SurfaceBodies(&SurfBodies);

	SurfBodies->get_Item(1, &SurfBody);// так как у нас только одно тело так что берем 1 его же 





	EdgesPtr edges;

	SurfBody->get_Edges(&edges);// колекция ребер  //---------------------------------------------------------------------

	EdgePtr ed;

	edges->get_Item(9,&ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(10, &ed);
	edgeColl->MethodAdd(ed);

	FilletFeaturePtr fillFeat = pFilletFt->MethodAddSimple(edgeColl, 0.1f, false, false, false, false, false, false); // скругления с радиусом


	ChamferFeaturesPtr pChamferFt;
	ft->get_ChamferFeatures(&pChamferFt);

	edgeColl->MethodClear();

	pPartComDef->get_SurfaceBodies(&SurfBodies);

	SurfBodies->get_Item(1, &SurfBody);// так как у нас только одно тело так что берем 1 его же 

	SurfBody->get_Edges(&edges);// колекция ребер

	edges->get_Item(6, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(12, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeaturePtr chamFearture = pChamferFt->MethodAddUsingDistance(edgeColl, 0.2f, false, false, false); // фаска 45 град




	pTrans->MethodEnd();

}
