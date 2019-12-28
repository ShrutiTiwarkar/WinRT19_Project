#include <Windows.h>
#include <CommCtrl.h>	// creating List View
#include <sstream>		// convert String to Integer
#include "SetIcon.h"
#include "RecordFile.h"

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogDisplayBox(HWND, UINT, WPARAM, LPARAM);

void PrintMainMenu(HWND, HDC, long, long, long, long);
//void DisplayAll();
void InsertPerson(HWND);
void CreatePerson(HWND);
BOOL ValidateName(HWND, TCHAR*);
BOOL ValidateDate(HWND, TCHAR*, TCHAR*, TCHAR*);
BOOL ValidatePhone(HWND, TCHAR*);
BOOL ValidateEmail(HWND, TCHAR*);
void AddToFormatedFile(HWND);
void AddToTempFile();
void TokenizeAndAddMember();
void TokenPerson(int*, TCHAR*);
void TokenRegNumber(TCHAR[512], TCHAR *);
void TokenName(TCHAR person[512], TCHAR*, TCHAR*, TCHAR*);
void TokenBirthDate(TCHAR person[512], TCHAR*, TCHAR*, TCHAR*);
void TokenGender(TCHAR person[512], TCHAR*);
void TokenEmail(TCHAR person[512], TCHAR *);
void TokenPhone(TCHAR person[512], TCHAR *);
void InsertNode();

/*void CreateForm(HWND, HDC, long, long, RECT);
void CreateNameTextBox(HWND, HDC, HWND *TextBoxName, HWND *TextBoxMiddleName, HWND *TextBoxSurname, long, long, RECT);
void CreateDOBText(HWND hwnd, HDC hdc, HWND *,long, long, RECT rc);
*/

static char RegistrationNumber = '0';

static BOOL nameFLAG = FALSE;
static BOOL middleFLAG = FALSE;
static BOOL surnameFLAG = FALSE;
static BOOL phoneFlag = FALSE;
static BOOL dateFlag = FALSE;
static BOOL emailFlag = FALSE;
static BOOL displayFlag = FALSE;
static TCHAR newNodeString[1024];
static TCHAR tempNewNodeString[1024];


struct  PersonDetails
{
	char Reg_No;
	TCHAR FirstName[20];
	TCHAR MiddleName[20];
	TCHAR Surname[20];
	TCHAR DOB[3];
	TCHAR MOB[3];
	TCHAR YOB[5];
	TCHAR phone_Number[15];
	TCHAR Gender[10];
	TCHAR Mail_id[30];
	struct PersonDetails *Next;
	struct PersonDetails *Prev;
};

struct PersonDetails *Head = NULL;
struct PersonDetails *newNode = NULL;

static int pageNumber = 4;// page 4 is flash Screen
IReadRecordFile* pIRead = NULL;
IWriteRecordFile* pIWrite = NULL;
TCHAR buffer[10240]; // Read whole file in array

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpszCmdLine, int iCmdShow) {

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	HRESULT hr;

	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		MessageBox(NULL, TEXT("COM Library cannot be initialized.Program will EXIT"), TEXT("Program Error"), MB_ICONERROR);
		exit(0);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, (LPWSTR)MYICON);		//Customized ICON
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(hInstance, (LPWSTR)MYICON);				// Customized ICON
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = szAppName;

	RegisterClassEx(&wndclass);
	
	hwnd = CreateWindow(szAppName,
		TEXT("Data Entry"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	TCHAR str[] = TEXT("In token display = FALSE");
	HRESULT hr, hr1;
	static HBITMAP hBitmap = NULL, hBitmap1 = NULL;
	static HINSTANCE hInst, hInst1;
	static HDC hdc, hMemdc;
	PAINTSTRUCT ps;																		// Used by Os internally
	RECT rc;
	BITMAP bmp, bmp1;
	static long xPos, yPos;
	static long cxClient, cyClient, xHalf, yHalf, xQuad, yQuad, xHalfOfQuad, yHalfOfQuad, xPixel, yPixel, iScale, cxRadius, cyRadius, cxMove, cyMove, cxTotal, cyTotal;
	static INITCOMMONCONTROLSEX icex;
	HANDLE hFile2;	

	switch (iMsg) {

	case WM_CREATE:
		
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		hBitmap = (HBITMAP)LoadBitmap(hInst, (LPCWSTR)MYBITMAP);
		hBitmap1 = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(MYBITMAPS));
		
		hr = CoCreateInstance(CLSID_ReadWrite, NULL, CLSCTX_INPROC_SERVER, IID_IWriteRecordFile, (void **)&pIWrite);
		if (FAILED(hr))
		{
			MessageBox(hwnd, TEXT("IWrite interface can not be obtained"), TEXT("Error"), MB_OK);
			DestroyWindow(hwnd);
			break;
		}

		hr1 = CoCreateInstance(CLSID_ReadWrite, NULL, CLSCTX_INPROC_SERVER, IID_IReadRecordFile, (void **)&pIRead);
		if (FAILED(hr))
		{
			MessageBox(hwnd, TEXT("IRead interface can not be obtained"), TEXT("Error"), MB_OK);
			DestroyWindow(hwnd);
			break;
		}

		hFile2 = CreateFile(TEXT("Temp.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		if (hFile2 == INVALID_HANDLE_VALUE) {
			hFile2 = CreateFile(TEXT("Temp.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		if (hFile2 == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, TEXT("FILE Temp not open in Create"), TEXT("Message"), MB_OK);
			return -1;
		}

		buffer[10240] = pIRead->ReadRecord(buffer, hFile2);
		CloseHandle(hFile2);

		// This reads the previous exsiting file for getting previous records and add to structure
		if (displayFlag == FALSE) {
			TokenizeAndAddMember();
			displayFlag = TRUE;
		}

		hdc = GetDC(hwnd);
		ReleaseDC(hwnd, hdc);

		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;

		InitCommonControlsEx(&icex); // class registeration
		break;

	case WM_CHAR:

		switch (LOWORD(wParam))
		{
		case 'A':
		case 'a':
			if (pageNumber == 0) {
				pageNumber = 1;
			}
			break;
		case 'B':
		case 'b':
			if (pageNumber == 0) {
				pageNumber = 2;
			}
			else if (pageNumber == 2) {
				pageNumber = 0;
			}
			break;
		case 'C':
		case 'c':
			if (pageNumber == 0) {
				pageNumber = 3;
			}
			break;
		case 'S':
		case 's':
			pageNumber = 0;
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		break;

	case WM_SIZE:

		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		xHalf = cxClient / 2;
		yHalf = cyClient / 2;

		xQuad = xHalf / 2;
		yQuad = yHalf / 2;

		xHalfOfQuad = xQuad / 2;
		yHalfOfQuad = yQuad / 2;

		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_PAINT:
		GetClientRect(hwnd, &rc);
		hdc = BeginPaint(hwnd, &ps);
		//StretchBlt(hdc, 0, 0, 1024, 820, hMemdc, 0, 0, 800, 600, SRCCOPY);
		

		switch (pageNumber) {
		case 0:
			// Main Page : Menu
			hMemdc = CreateCompatibleDC(NULL);
			SelectObject(hMemdc, (HGDIOBJ)hBitmap);
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			StretchBlt(hdc, 0, 0, rc.right, rc.bottom, hMemdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			PrintMainMenu(hwnd, hdc, cxClient, cyClient, xQuad, yQuad);
			DeleteDC(hMemdc);
			break;
		case 1:
			// Dialog Box to fill Details
			hMemdc = CreateCompatibleDC(NULL);
			SelectObject(hMemdc, (HGDIOBJ)hBitmap);
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			StretchBlt(hdc, 0, 0, rc.right, rc.bottom, hMemdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);			
			PrintMainMenu(hwnd, hdc, cxClient, cyClient, xQuad, yQuad);
			DialogBox(hInst, TEXT("DialogBox"), hwnd, (DLGPROC)AboutDlgProc);
			DeleteDC(hMemdc);

			break;
		case 2:
			// Dialog box to Display
			hMemdc = CreateCompatibleDC(NULL);
			SelectObject(hMemdc, (HGDIOBJ)hBitmap);
			GetObject(hBitmap, sizeof(BITMAP), &bmp);
			StretchBlt(hdc, 0, 0, rc.right, rc.bottom, hMemdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			PrintMainMenu(hwnd, hdc, cxClient, cyClient, xQuad, yQuad);
			DialogBox(hInst, TEXT("DialogDisplay"), hwnd, (DLGPROC)DialogDisplayBox);
			DeleteDC(hMemdc); 
			break;
		case 3:
			DestroyWindow(hwnd);
			break;
		case 4:
			hMemdc = CreateCompatibleDC(NULL);
			SelectObject(hMemdc, (HGDIOBJ)hBitmap1);
			GetObject(hBitmap1, sizeof(BITMAP), &bmp1);
			//StretchBlt(hdc, 0, 0, 1024, 820, hMemdc, 0, 0, 800, 600, SRCCOPY);
			StretchBlt(hdc, 0, 0, rc.right, rc.bottom, hMemdc, 0, 0, bmp1.bmWidth, bmp1.bmHeight, SRCCOPY);
			DeleteDC(hMemdc);
			break;
		}

		UpdateWindow(hwnd);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:

		pIRead->Release();
		pIWrite->Release();
		
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


/************************************************************************
*		Function Name	:			PrintMainMenu						*
*		Working			:	Displays the Menu on the screen				*
*							in different font styles and color			*
*																		*
*************************************************************************/
void PrintMainMenu(HWND hwnd, HDC hdc, long cxClient, long cyClient, long xQuad, long yQuad) {

	TCHAR szTitle[] = TEXT("Main Menu.");
	TCHAR szStartUpLine[] = TEXT("Select the Option...!");
	TCHAR Menu1[] = TEXT("a. Add New Member");
	TCHAR Menu2[] = TEXT("b. Display All");
	TCHAR Menu3[] = TEXT("c. Exit");

	HFONT hFont, hFont2, hFont3;
	SIZE size;
	int xTemp = 0, yTemp = 0;
	int x = 50;

	hFont = CreateFont(100, 60, 0, 10, FW_EXTRABOLD, TRUE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Comic Sans MS"));
	hFont2 = CreateFont(60, 40, 0, 10, FW_BOLD, TRUE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Comic Sans MS"));
	hFont3 = CreateFont(50, 25, 0, 10, FW_BOLD, TRUE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Comic Sans MS"));
	//Mistral

	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);

	GetTextExtentPoint32(hdc, szTitle,(int)strlen((char *)szTitle), &size);

	BeginPath(hdc);
	TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad/2)/2, szTitle, lstrlen(szTitle));
	DeleteObject(hFont);

	SelectObject(hdc, hFont2);
	SetBkMode(hdc, TRANSPARENT);

	TextOut(hdc, (3 * xQuad)/2,cyClient - (yQuad/2), szStartUpLine, lstrlen(szStartUpLine));
	DeleteObject(hFont2);

	SelectObject(hdc, hFont3);
	SetBkMode(hdc, TRANSPARENT);

	TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad), Menu1, lstrlen(Menu1));
	TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad + x), Menu2, lstrlen(Menu2));
	TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad + (2 * x)), Menu3, lstrlen(Menu3));

	EndPath(hdc);

	SelectObject(hdc, CreateHatchBrush(HS_BDIAGONAL, RGB(255, 0, 0)));
	SetBkColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, OPAQUE);

	StrokeAndFillPath(hdc);
	DeleteObject(SelectObject(hdc, GetStockObject(WHITE_BRUSH)));
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));
	DeleteObject(hFont3);

}


/*
void DisplayAll(HWND hwnd, HDC hdc, long cxClient, long cyClient, long xQuad, long yQuad) {

	
	
	//TokenizeAndAddMember(hwnd);

	
	TCHAR szTitle[] = TEXT("All Records");
	TCHAR szStartUpLine[] = TEXT("Press B for BACK...!");
	TCHAR EmptyFile[] = TEXT("No Records Found...");

	HFONT hFont, hFont2;
	SIZE size;
	int y = 0;

	hFont = CreateFont(100, 40, 0, 10, FW_EXTRABOLD, TRUE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Comic Sans MS"));
	hFont2 = CreateFont(20, 10, 0, 10, FW_EXTRABOLD, TRUE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Comic Sans MS"));
	//Mistral

	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);

	GetTextExtentPoint32(hdc, szTitle, (int)strlen((char *)szTitle), &size);
	
	BeginPath(hdc);
	TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad / 2) / 2, szTitle, lstrlen(szTitle));
	TextOut(hdc, (3 * xQuad) / 2, cyClient - (yQuad / 2), szStartUpLine, lstrlen(szStartUpLine));
	
	if (Head == NULL) {
		TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad), EmptyFile, lstrlen(EmptyFile));
		DeleteObject(hFont);
	}
	else
	{
		SelectObject(hdc, hFont2);
		SetBkMode(hdc, TRANSPARENT);
		BeginPath(hdc);
		
		struct PersonDetails *temp = Head;
		while (temp -> Next != NULL)
		{	
			TextOut(hdc, (2 * xQuad) - (xQuad / 2) , (yQuad / 2) / 2 + y, temp ->DOB,(int)lstrlen(temp -> DOB));
			y = y + 50;
			temp = temp->Next;
		}
		TextOut(hdc, (2 * xQuad) - (xQuad / 2), (yQuad / 2) / 2 + y, temp->DOB, (int)lstrlen(temp->DOB));

	}
	
	EndPath(hdc);
	SelectObject(hdc, CreateHatchBrush(HS_BDIAGONAL, RGB(255, 0, 0)));
	SetBkColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, OPAQUE);

	StrokeAndFillPath(hdc);
	DeleteObject(SelectObject(hdc, GetStockObject(WHITE_BRUSH)));
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));

	DeleteObject(hFont2);
	
}
*/

/************************************************************************
*		Function Name	:			DialogDisplayBox					*
*		Working			:	Displays the List on the screen				*
*							Displays the Personal Details of 			*
*							each person in rows.						*	
*																		*
*************************************************************************/
static HWND hOptionDialogDisplay = NULL;
BOOL CALLBACK DialogDisplayBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	
	HWND hlv = NULL;
	LVITEM lvi;
	LVCOLUMN lvc;
	RECT rc;
	GetClientRect(hDlg, &rc);
	HANDLE hFile2 = NULL;
	static TCHAR OptionName[10];

	// Array of Columns in List View

	TCHAR str[][20] = { 
					TEXT("ALL"),
					TEXT("NAME"),
					TEXT("SURNAME"),
					TEXT("DATE OF BIRTH"),
					TEXT("MONTH OF BIRTH"),
					TEXT("YEAR OF BIRTH")
					};
	struct PersonDetails *temp = Head;
	int i=0, j=0, count = 0;
	TCHAR strCount[5];
	//TCHAR BirthDate[15];

	switch (message) {
	case WM_INITDIALOG:

		// Token each value from our file
		TokenizeAndAddMember();
		//ComboBox in Dialog 
		/*
		GetClientRect(hDlg, &rc);
		hOptionDialogDisplay = CreateWindow(TEXT("ComboBox"),
			NULL,
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_VSCROLL,
			rc.right / 2 - 50,
			rc.bottom / 9,
			150,
			130,
			hDlg,
			NULL,
			NULL,
			NULL);
		
		for (int i = 0; i < 6; i++) {

			//wsprintf(temp, TEXT("%s"),str[i]);
			SendMessage(hOptionDialogDisplay, CB_ADDSTRING, (WPARAM)0, (LPARAM)str[i]);
			SendMessage(hOptionDialogDisplay, CB_SETCURSEL, (WPARAM)0, (LPARAM)i);
		}
		*/
		
		hlv = CreateWindow(
			WC_LISTVIEW,
			NULL,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | WS_BORDER ,
			rc.right/12,
			rc.bottom/6,
			800,
			380,
			hDlg,
			NULL,
			NULL,
			NULL
		);
				
		// Inserting Columns
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 2;
		lvc.cx = 60;
		lvc.pszText = (LPWSTR)TEXT("Number");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvc);

		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 1;
		lvc.cx = 60;
		lvc.pszText = (LPWSTR)TEXT("Name");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvc);

		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 110;
		lvc.pszText = (LPWSTR)TEXT("Middle Name");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)2, (LPARAM)&lvc);


		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 70;
		lvc.pszText = (LPWSTR)TEXT("Surname");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)3, (LPARAM)&lvc);


		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 60;
		lvc.pszText = (LPWSTR)TEXT("Gender");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)4, (LPARAM)&lvc);

		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 40;
		lvc.pszText = (LPWSTR)TEXT("Day");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)5, (LPARAM)&lvc);


		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 60;
		lvc.pszText = (LPWSTR)TEXT("Month");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)6, (LPARAM)&lvc);


		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 40;
		lvc.pszText = (LPWSTR)TEXT("Year");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)7, (LPARAM)&lvc);


		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 150;
		lvc.pszText = (LPWSTR)TEXT("Email-Id");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)8, (LPARAM)&lvc);


		lvc.fmt = LVCFMT_CENTER;
		lvc.iSubItem = 0;
		lvc.cx = 150;
		lvc.pszText = (LPWSTR)TEXT("Phone No");
		SendMessage(hlv, LVM_INSERTCOLUMN, (WPARAM)9, (LPARAM)&lvc);
		
		// Inserting Items
		memset(&lvi, 0, sizeof(LVITEM));

		lvi.mask = LVIF_TEXT;// | LVIF_PARAM | LVIF_STATE;
	
		if (temp == NULL) {

			lvi.pszText = (LPWSTR)TEXT("No Records Found...");	//name
			lvi.iItem = 0;	//row number
			lvi.iSubItem = 0; // column number
			SendMessage(hlv, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&lvi);
		}
		else {

			while (temp != NULL) {
				j = 0;
				wsprintf(strCount, TEXT("%d"), ++count);
				lvi.pszText = (LPWSTR)strCount;	//name
				lvi.iItem = i;	//row number
				lvi.iSubItem = j; // column number
				SendMessage(hlv, LVM_INSERTITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->FirstName;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->MiddleName;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);


				lvi.pszText = (LPWSTR)temp->Surname;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->Gender;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp ->DOB;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->MOB;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->YOB;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->Mail_id;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				lvi.pszText = (LPWSTR)temp->phone_Number;
				lvi.iSubItem = ++j;
				SendMessage(hlv, LVM_SETITEM, (WPARAM)i, (LPARAM)&lvi);

				++i;
				temp = temp-> Next;
			}
		}
		return TRUE;

	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			pageNumber = 0;
			EndDialog(hDlg, 0);
			return TRUE;
		}
	}
	return FALSE;
}


/************************************************************************
*		Function Name	:			AboutDlgProc						*
*		Working			:	Displays the dialog Box						*
*							In this Dialog Box user enter his details	*
*							after submiting validation takes place		*
*																		*
*************************************************************************/
static HWND hDay, hMonth, hYear;
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam){
	
	HANDLE hFile1 = NULL, hFile2 = NULL;
	TCHAR str[31];
	int startYear = 1990;
	int endYear = 2001;
	
	RECT rc;
	static long cxClient, cyClient, xHalf, yHalf;

	switch (message)
	{
	case WM_INITDIALOG:
	
		GetClientRect(hDlg, &rc);

	hDay = CreateWindow(TEXT("ComboBox"), 
			NULL, 
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_VSCROLL, 
			rc.right/6 -20,
			rc.bottom/4 + 20,
			80,
			130,
			hDlg, 
			NULL, 
			NULL, 
			NULL);
	
	for (int i = 1; i <= 31; i++) {

		wsprintf(str, TEXT("%d"), i);
		SendMessage(hDay, CB_ADDSTRING, (WPARAM)0, (LPARAM)str);
		SendMessage(hDay, CB_SETCURSEL, (WPARAM)0, (LPARAM)i);
	}

	hMonth = CreateWindow(TEXT("ComboBox"),
		NULL,
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_VSCROLL,
		rc.right / 4 + 50,
		rc.bottom / 4 + 20,
		80,
		130,
		hDlg,
		NULL,
		NULL,
		NULL);
	for (int i = 1; i <= 12; i++) {

		wsprintf(str, TEXT("%d"), i);
		SendMessage(hMonth, CB_ADDSTRING, (WPARAM)0, (LPARAM)str);
		SendMessage(hMonth, CB_SETCURSEL, (WPARAM)0, (LPARAM)i);
	}

	
	hYear = CreateWindow(TEXT("ComboBox"),
		NULL,
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_VSCROLL,
		rc.right / 2 - 20,
		rc.bottom / 4 + 20,
		80,
		130,
		hDlg,
		NULL,
		NULL,
		NULL);
	for (int i = startYear; i <= endYear; i++) {

		wsprintf(str, TEXT("%d"), i);
		SendMessage(hYear, CB_ADDSTRING, (WPARAM)0, (LPARAM)str);
		SendMessage(hYear, CB_SETCURSEL, (WPARAM)0, (LPARAM)i);
	}

		return TRUE;
	
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		xHalf = cxClient / 2;
		yHalf = cyClient / 2;

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{

		case IDC_MALE:
		case IDC_FEMALE:
			// Radio Button check which is checked : Male or Female
			CheckRadioButton(hDlg, (int)IDC_MALE, (int)IDC_FEMALE, LOWORD(wParam));			
			break;
		case IDOK:
			// Insert a person details into structure
			InsertPerson(hDlg);
			pageNumber = 0;
			// get Handle of file : Record.txt & Temp.txt
			if (nameFLAG == TRUE && middleFLAG == TRUE && surnameFLAG == TRUE && phoneFlag == TRUE && emailFlag == TRUE && dateFlag == TRUE) {
				hFile1 = CreateFile(TEXT("Record.txt"), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				if (hFile1 == INVALID_HANDLE_VALUE) {
					hFile1 = CreateFile(TEXT("Record.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				}
				if (hFile1 == INVALID_HANDLE_VALUE) {
					return -1;
				}

				hFile2 = CreateFile(TEXT("Temp.txt"), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				if (hFile2 == INVALID_HANDLE_VALUE) {
					hFile2 = CreateFile(TEXT("Temp.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				}
				if (hFile2 == INVALID_HANDLE_VALUE) {
					return -1;
				}
				//MessageBox(NULL, tempNewNodeString, TEXT("All String before Write"), MB_OK);
				pIWrite->WriteRecord(newNodeString, hFile1);
				pIWrite->WriteRecord(tempNewNodeString, hFile2);
				
				//buffer[1024] = pIRead->ReadRecord(buffer, hFile2);
				CloseHandle(hFile1);
				CloseHandle(hFile2);
				MessageBox(NULL, TEXT("Details Added in file named with Record.txt \nCreated in same folder"), TEXT("MESSAGE"), MB_OK | MB_ICONINFORMATION);
				EndDialog(hDlg, 0);
			}
			else {
				--RegistrationNumber;
				newNode->Prev->Next = NULL;
				free(newNode);
			}
			return TRUE;
		
		case IDCANCEL:
			pageNumber = 0;
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/************************************************************************
*		Function Name	:			CreatePerson						*
*		Working			:	A node of doublly LinkedList is created		*
*																		*
*																		*
*************************************************************************/
void CreatePerson(HWND hwnd) {

	struct PersonDetails *temp = NULL;
	newNode = (struct PersonDetails*)malloc(sizeof(struct PersonDetails));
	
	newNode->Reg_No = ++RegistrationNumber;
	AddToFormatedFile(hwnd);
	AddToTempFile();

	newNode->Next = NULL;
	newNode->Prev = NULL;
}

/************************************************************************
*		Function Name	:			InsertPerson						*
*		Working			:	A node of doublly LinkedList is Inserting	*
*																		*
*************************************************************************/
void InsertPerson(HWND hwnd) {

	CreatePerson(hwnd);
	
	if (Head == NULL) {
		Head = newNode;
	}
	else {
		struct PersonDetails *temp = Head;
		while (temp ->Next != NULL) {
			temp = temp->Next;
		}
		temp->Next = newNode;
		newNode->Prev = temp;
	}
}

/************************************************************************
*		Function Name	:			ValidateName						*
*		Working			:	- Checks the name string is not NULL		*
*							- Name should not contain symbols 			*
*							- String should not contain numbers			*
*																		*
*************************************************************************/
BOOL ValidateName(HWND hwnd, TCHAR *str) {

	BOOL flag = FALSE;

	if (*str == NULL) {
		// String is NULL
		MessageBox(hwnd, TEXT("Name cannot be empty"), TEXT("Message..."), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	else
	{	
		// string not NULL
		while (*str != '\0') {

			if ((*str >= 'A' || *str >= 'a') && (*str <= 'Z' || *str <= 'z')) {
				// character in string in alphabet
				flag = TRUE;
			}
			else
			{
				// character in string is symbol or number
				flag =  FALSE;
			}
			if (flag == FALSE) {
				MessageBox(hwnd, TEXT("Name must be in String"), TEXT("Message..."), MB_OK | MB_ICONWARNING);
				return FALSE;
			}
			str++;
		}
		if (flag == TRUE) {
			//MessageBox(hwnd, TEXT("Name is written"), TEXT("Message"), MB_OK);
			return TRUE;
		}
		return FALSE;
	}

}


/************************************************************************
*		Function Name	:			ValidatePhone						*
*		Working			:	- Not Complursory Field						*
*							- Must not contain Symbols and alphabets	*
*							- String must be of 10 digits				*
*																		*
*************************************************************************/
BOOL ValidatePhone(HWND hwnd, TCHAR *PhoneNumber) {
	
	BOOL flag = FALSE;
	int count = 0;

	while (*PhoneNumber != '\0') {
		if (*PhoneNumber >= '0' && *PhoneNumber <= '9') {
			// Check character is only number
			//MessageBox(hwnd, TEXT("Valid Number"), TEXT("Message..."), MB_OK);
			flag = TRUE;
		}
		else {
			MessageBox(hwnd, TEXT("Please Enter Valid Number"), TEXT("Message..."), MB_OK | MB_ICONWARNING); 
			flag = FALSE;
			break;
		}
		PhoneNumber++;
		count++;
	}
	if (count > 10 && flag == TRUE) {
		// check the count 
		MessageBox(hwnd, TEXT("Please Enter Valid Number"), TEXT("Message..."),MB_OK | MB_ICONWARNING);
		flag = FALSE;
	}
	else if (count < 10 && flag == TRUE) {
		MessageBox(hwnd, TEXT("Please Enter Valid Number"), TEXT("Message..."),MB_OK | MB_ICONWARNING);
		flag = FALSE;
	}
	else if (flag == TRUE) {
		//MessageBox(hwnd, TEXT("Valid Number"), TEXT("Message..."), MB_OK);
		return TRUE;
	}
	return flag;
	
}

/************************************************************************
*		Function Name	:			ValidateEmail						*
*		Working			:	- Checks the name string is not NULL		*
*							- Name should not contain symbols 			*
*							- String should not contain numbers			*
*							- exapmple : NAME@gmail.com					*
*																		*
*************************************************************************/
BOOL ValidateEmail(HWND hwnd, TCHAR *email) {

	BOOL flag1 = FALSE;
	BOOL flag2 = FALSE;
	
	if (*email == NULL) {
		flag1 = FALSE;
		flag2 = FALSE;
	}
	else {
		while (*email != '\0') {

			if (*email == '@') {
				flag1 = TRUE;
			}
			else if (*email == '.') {
				flag2 = TRUE;
			}
			email++;
		}
	}
	if (flag1 == TRUE && flag2 == TRUE) {
		return TRUE;
	}
	else {
		MessageBox(hwnd, TEXT("Please Enter Valid Email id"), TEXT("Message..."),MB_OK | MB_ICONWARNING);
		return FALSE;
	}
}

/************************************************************************
*		Function Name	:			ValidateDate						*
*		Working			:	- Checks the month does not excced days		*
*							- Check for leap year						*
*							- Validate Date, month, year				*
*************************************************************************/
BOOL ValidateDate(HWND hwnd, TCHAR *DOB, TCHAR *MOB, TCHAR *YOB) {
	
	BOOL flag1 = FALSE;	// dof
	//BOOL flag2 = FALSE; // mob
	//BOOL flag3 = FALSE; // yob
	BOOL leapFlag = FALSE;
	int  year;
	int  month;
	int  day;
	
	year = stoi(YOB);
	month= stoi(MOB);
	day = stoi(DOB);

	//wsprintf(temp, TEXT("%d"), day);
	//MessageBox(NULL, temp, TEXT("Year in Int"), MB_OK);


	//MessageBox(NULL, TEXT("In VAlidate DAte"), TEXT("Message"), MB_OK);
	if (year % 4 == 0) {
		if (year % 100 == 0) {
			if (year % 400 == 0) {
				leapFlag = TRUE;
			}
			else {
				leapFlag = FALSE;
			}
		}
		else {
			leapFlag = FALSE;
		}
	}
	else {
		leapFlag = FALSE;
	}
	/*
	if (leapFlag == TRUE) {
		MessageBox(NULL, TEXT("Leap Year"), TEXT("Message"), MB_OK);
	}
	*/
	if (month == 1) {
		if (day <= 31) {
			flag1 = TRUE;
		}
	}
	else if (month == 2) {
		if (leapFlag == TRUE) {
			if (day <= 29) {
				flag1 = TRUE;
			}
			else {
				flag1 = FALSE;
			}
		}
		else {
			if (day <= 28) {
				flag1 = TRUE;
			}
			else {
				flag1 = FALSE;
			}
		}
	}
	else if (month == 3) {
		if (day <= 31) {
			flag1 = TRUE;
		}
	}
	else if (month == 4) {
		if (day <= 30) {
			flag1 = TRUE;
		}
		else {
			flag1 = FALSE;
		}
	}
	else if (month == 5) {
		if (day <= 31) {
			flag1 = TRUE;
		}
	}
	else if (month == 6) {
		if (day <= 30) {
			flag1 = TRUE;
		}
		else {
			flag1 = FALSE;
		}
	}
	else if (month == 7) {
		if (day <= 31) {
			flag1 = TRUE;
		}
	}
	else if (month == 8) {
		if (day <= 31) {
			flag1 = TRUE;
		}
	}
	else if (month == 9) {
		if (day <= 30) {
			flag1 = TRUE;
		}
		else
		{
			flag1 = FALSE;
		}
	}
	else if (month == 10) {
		if (day <= 31) {
			flag1 = TRUE;
		}
	}
	else if (month == 11) {
		if (day <= 30) {
			flag1 = TRUE;
		}
		else {
			flag1 = FALSE;
		}
	}
	else if (month == 12)
	{
		if (day <= 31) {
			flag1 = TRUE;
		}
	}

	
	if (flag1 == FALSE) {
		MessageBox(NULL, TEXT("Invalid Date"), TEXT("MESSAGE"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	
	return flag1;
}

/********************************************************************************
*		Function Name	:			AddToFormatedFile							*
*		Working			:	This function get the values from the fileds		*
*							check the values are valid and add to Record.txt	*
*																				*
*********************************************************************************/
void AddToFormatedFile(HWND hwnd) {

	HWND hFirst;
	HWND hMiddle;
	HWND hSurname;
	HWND hEmail;
	HWND hPhoneNo;
	char startsymbol = '<';
	char endsymbol = '>';
	BOOL flag = TRUE;
	
	hFirst = GetDlgItem(hwnd, (int)IDC_FIRSTNAME);
	GetWindowText(hFirst, (LPWSTR)newNode->FirstName, 20);
	//GetDlgItemText(hwnd, (int)IDC_FIRSTNAME, (LPWSTR)newNode->FirstName,20);
	nameFLAG = ValidateName(hFirst, newNode->FirstName);

	hMiddle = GetDlgItem(hwnd, (int)IDC_MIDDLENAME);
	GetWindowText(hMiddle, (LPWSTR)newNode->MiddleName, 20);
	//GetDlgItemText(hwnd, (int)IDC_MIDDLENAME, (LPWSTR)newNode->MiddleName, 20);
	middleFLAG = ValidateName(hwnd, newNode->MiddleName);

	hSurname = GetDlgItem(hwnd, (int)IDC_LASTNAME);
	GetWindowText(hSurname, (LPWSTR)newNode->Surname, 20);
	//GetDlgItemText(hwnd, (int)IDC_LASTNAME, (LPWSTR)newNode->Surname, 20);
	surnameFLAG = ValidateName(hwnd, newNode->Surname);

	if (IsDlgButtonChecked(hwnd, IDC_MALE)) {
		//MessageBox(hwnd, TEXT("MALE"), TEXT("Message") , MB_OK);
		//newNode->Gender = TEXT("MALE");
		wsprintf(newNode->Gender, TEXT("Male"));
	}
	else if (IsDlgButtonChecked(hwnd, IDC_FEMALE)) {
		//MessageBox(hwnd, TEXT("FEMALE"), TEXT("Message"), MB_OK);
		//newNode->Gender = TEXT("FEMALE");
		wsprintf(newNode->Gender, TEXT("Female"));
	}

	GetWindowText(hDay, (LPWSTR)newNode->DOB, 3);
	//GetDlgItemText(hwnd, (int)IDC_DOB, (LPWSTR)newNode->DOB, 3);
	GetWindowText(hMonth, (LPWSTR)newNode->MOB, 3);
	//GetDlgItemText(hwnd, (int)IDC_MOB, (LPWSTR)newNode->MOB, 3);
	GetWindowText(hYear, (LPWSTR)newNode->YOB, 5);
	//GetDlgItemText(hwnd, (int)IDC_YOB, (LPWSTR)newNode->YOB, 5);
	dateFlag =  ValidateDate(hwnd, newNode->DOB, newNode->MOB, newNode->YOB);

	hEmail = GetDlgItem(hwnd, (int)IDC_EMAIL);
	GetWindowText(hEmail, (LPWSTR)newNode->Mail_id, 30);
	//GetDlgItemText(hwnd, (int)IDC_EMAIL, (LPWSTR)newNode->Mail_id, 30);
	emailFlag = ValidateEmail(hwnd, newNode->Mail_id);

	hPhoneNo = GetDlgItem(hwnd, (int)IDC_PHONENO);
	GetWindowText(hPhoneNo, (LPWSTR)newNode->phone_Number, 15);
	//GetDlgItemText(hwnd, (int)IDC_PHONENO, (LPWSTR)newNode->phone_Number, 15);
	phoneFlag = ValidatePhone(hwnd, newNode->phone_Number);

	wsprintf(newNodeString, TEXT("\n%c\nRegistration Number = %c\nFull Name :\n\t\t%s %s %s  \nDate Of Birth : \n\t\t%s/%s/%s \nGender :\n\t\t%s \nMail Id : \n\t\t%s \nPhone Number :\n\t\t%s\n%c\n\n"), startsymbol, newNode->Reg_No, newNode->FirstName, newNode->MiddleName, newNode->Surname, newNode->DOB, newNode->MOB, newNode->YOB, newNode->Gender, newNode->Mail_id, newNode->phone_Number, endsymbol);
	//MessageBox(NULL, newNodeString, TEXT("All String"), MB_OK);

}


/************************************************************************
*		Function Name	:			AddToTempFile						*
*		Working			:	- Temporary File created for internal		*
*							- This function adds data in form of		*
*								tokenization							*
*							- Temp.txt									*
*************************************************************************/
void AddToTempFile() {

	//[] Reg_No
	//!! FullName
	//@@ Date
	//&& Gender
	//++ Mail_Id
	//$$ Phone_Number

	char startsymbol = '<';
	char endsymbol = '>';
	char startbracket = '[';
	char endbracket = ']';
	char exclamation = '!';
	char star = '*';
	char And = '&';
	char plus = '+';
	char dolar = '$';

	wsprintf(tempNewNodeString, TEXT("%c \n %c %c %c %c%s %s %s%c %c%s/%s/%s%c %c%s%c %c%s%c %c%s%c \n %c\n"),startsymbol, startbracket, newNode->Reg_No, endbracket,exclamation, newNode->FirstName, newNode->MiddleName, newNode->Surname, exclamation,star,newNode->DOB, newNode->MOB, newNode->YOB, star, And, newNode->Gender, And, plus, newNode->Mail_id, plus, dolar, newNode->phone_Number, dolar,endsymbol);
	//MessageBox(NULL, tempNewNodeString, TEXT("All String in Temp file"), MB_OK);
}

/************************************************************************
*		Function Name	:			TokenizeAndAddMember				*
*		Working			:	- Each person record in tokenized from file	*
*							- Temp.txt									*
*************************************************************************/
void TokenizeAndAddMember() {
	
	HANDLE hFile2 = NULL;

	hFile2 = CreateFile(TEXT("Temp.txt"), GENERIC_READ , 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hFile2 == INVALID_HANDLE_VALUE) {
		hFile2 = CreateFile(TEXT("Temp.txt"), GENERIC_READ , 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (hFile2 == INVALID_HANDLE_VALUE) {
		return;
	}
	// Read whole file "Temp.txt" in this buffer
	SetFilePointer(hFile2, NULL, NULL, FILE_BEGIN);
	buffer[10240] = pIRead->ReadRecord(buffer, hFile2);


	static int i = 0;
	static TCHAR person[512];

	while (buffer[i] != '\0')
	{
		// Get a single person entry
		TokenPerson(&i, person);
		//MessageBox(NULL, (person), TEXT("Single Person after Return"), MB_OK);
		i++;
	}
	CloseHandle(hFile2);
	
}

/************************************************************************
*		Function Name	:			TokenPerson							*
*		Working			:	- Get each record of single person			*
*							- Temp.txt									*
*************************************************************************/
void TokenPerson(int *i, TCHAR *person) {

	int j = 0; 
	int k = *i;
	TCHAR RegNumber[5];
	TCHAR First[30];
	TCHAR Middle[30];
	TCHAR Last[30];
	TCHAR dob[3];
	TCHAR mob[3];
	TCHAR yob[5];
	TCHAR gender[10];
	TCHAR email[40];
	TCHAR phone[15];

	
	//Get Single person Record
	if (buffer[k] == '<') {
		k++;
		while (buffer[k] != '>') {
			person[j] = buffer[k];
			j++;
			k++;
		}
		person[j] = '\0';
		//MessageBox(NULL, (person), TEXT("Single Person before Return"), MB_OK);
		TokenRegNumber(person, RegNumber);
		//MessageBox(NULL, (RegNumber), TEXT("Reg_no after Return"), MB_OK);
		TokenName(person, First, Middle, Last);

		TokenBirthDate(person, dob, mob, yob);

		TokenGender(person, gender);

		TokenEmail(person, email);
		//MessageBox(NULL, (email), TEXT("Email after Return"), MB_OK);
		TokenPhone(person, phone);

		if (displayFlag == FALSE) {

			//struct PersonDetails *temp = NULL;
			newNode = (struct PersonDetails*)malloc(sizeof(struct PersonDetails));

			++RegistrationNumber;
			newNode->Reg_No = RegistrationNumber;
			wsprintf(newNode->FirstName, TEXT("%s"), First);
			wsprintf(newNode->MiddleName, TEXT("%s"), Middle);
			wsprintf(newNode->Surname, TEXT("%s"), Last);
			wsprintf(newNode->DOB, TEXT("%s"), dob);
			wsprintf(newNode->MOB, TEXT("%s"), mob);
			wsprintf(newNode->YOB, TEXT("%s"), yob);
			wsprintf(newNode->Gender,TEXT("%s"), gender);
			wsprintf(newNode->Mail_id, TEXT("%s"), email);
			wsprintf(newNode->phone_Number, TEXT("%s"), phone);
			newNode->Next = NULL;
			newNode->Prev = NULL;
			InsertNode();
		}
	}
	*i = k;
	k++;
	//DisplayAll(hwnd);
}

void InsertNode() {
	
	if (Head == NULL) {
		//MessageBox(NULL, newNode->FirstName, TEXT("When Head is NULL"), MB_OK);
		Head = newNode;
		//MessageBox(NULL, newNode->FirstName, TEXT("newNode assign"), MB_OK);
	}
	else {
	
		struct PersonDetails *temp = Head;
		while (temp->Next != NULL) {
			temp = temp->Next;
		}
		temp->Next = newNode;
		newNode->Prev = temp;
	}
	//MessageBox(NULL, TEXT("Insert Return"), TEXT("Message"), MB_OK);
}


/************************************************************************
*		Function Name	:			TokenRegNumber						*
*		Working			:	- Get each reg Number of single person		*
*							- Temp.txt									*
*************************************************************************/
void TokenRegNumber(TCHAR person[512], TCHAR *RegNo) {
	
	int m = 0;
	int n = 0;

	while (person[m] != '\0') {
		if (person[m] == '[') {
			m++;
			while (person[m] != ']') {
				RegNo[n] = person[m];
				n++;
				m++;
			}
			RegNo[n] = '\0';
			//MessageBox(NULL, (RegNo), TEXT("Reg_no before Return"), MB_OK);
		}
		m++;
	}
	//i++;
	//MessageBox(NULL,TEXT("Not enter in if"), TEXT("Reg_no before Return"), MB_OK);
}



/************************************************************************
*		Function Name	:			TokenName							*
*		Working			:	- Get Name of single person					*
*							- Temp.txt									*
*************************************************************************/
void TokenName(TCHAR person[512], TCHAR *First, TCHAR *Middle, TCHAR *Last) {

	TCHAR name[256];

	int m = 0, n = 0;
	while (person[m] != '\0') {
		if (person[m] == '!') {
			m++;
			while (person[m] != '!') {
				name[n] = person[m];
				n++;
				m++;
			}
			name[n] = '\0';
			//MessageBox(NULL, (name), TEXT("Name before Return"), MB_OK);
		}
		m++;
	}

	m = 0;
	n = 0;
	while (name[m] != '\0') {
		First[n] = name[m];
		m++;
		n++;
		if (name[m] == ' ') {
			First[n] = '\0';
			break;
		}
	}
	//MessageBox(NULL, First, TEXT("Message"), MB_OK);
	
	n = 0;
	while (name[m] != '\0') {
		Middle[n] = name[m];
		m++;
		n++;
		if (name[m] == ' ') {
			Middle[n] = '\0';
			break;
		}
	}
	//MessageBox(NULL, Middle, TEXT("Message"), MB_OK);
	n = 0;
	while (name[m] != '\0') {
		Last[n] = name[m];
		m++;
		n++;	
	}
	Last[n] = '\0';
	//MessageBox(NULL, Last, TEXT("Message"), MB_OK);
}



/************************************************************************
*		Function Name	:			TokenBirthDate						*
*		Working			:	- Get Day, Month, Year of single person		*
*							- Temp.txt									*
*************************************************************************/
void TokenBirthDate(TCHAR person[512], TCHAR* dob, TCHAR* mob, TCHAR* yob) {
	
	TCHAR date[512];

	int m = 0, n = 0;
	while (person[m] != '\0') {
		if (person[m] == '*') {
			m++;
			while (person[m] != '*') {
				date[n] = person[m];
				n++;
				m++;
			}
			date[n] = '\0';
			//MessageBox(NULL, (date), TEXT("Date before Return"), MB_OK);
		}
		m++;
	}
	m = 0;
	n = 0;
	while (date[m] != '\0') {
		dob[n] = date[m];
		m++;
		n++;
		if (date[m] == '/') {
			m++;
			dob[n] = '\0';
			break;
		}
	}
	//MessageBox(NULL, dob, TEXT("Message"), MB_OK);
	n = 0;
	while (date[m] != '\0') {
		mob[n] = date[m];
		m++;
		n++;
		if (date[m] == '/') {
			m++;
			mob[n] = '\0';
			break;
		}
	}
	//MessageBox(NULL, mob, TEXT("Message"), MB_OK);

	n = 0;
	while (date[m] != '\0') {
		yob[n] = date[m];
		m++;
		n++;
	}
	yob[n] = '\0';
	//MessageBox(NULL, yob, TEXT("Year"), MB_OK);
}


/************************************************************************
*		Function Name	:			TokenGender							*
*		Working			:	- Get Gender of that  person				*
*							- Temp.txt									*
*************************************************************************/
void TokenGender(TCHAR person[512], TCHAR* gender) {
	int m = 0;
	int n = 0;
	while (person[m] != '\0') {
		if (person[m] == '&') {
			m++;
			while (person[m] != '&') {
				gender[n] = person[m];
				m++;
				n++;
			}
			gender[n] = '\0';
			//MessageBox(NULL, (gender), TEXT("Gender Return"), MB_OK);
		}
		m++;
	}
}


/************************************************************************
*		Function Name	:			TokenEmail							*
*		Working			:	- Get email of single person			*
*							- Temp.txt									*
*************************************************************************/
void TokenEmail(TCHAR person[512], TCHAR * email) {

	int m = 0;
	int n = 0;
	while (person[m] != '\0') {
		if (person[m] == '+') {
			m++;
			while (person[m] != '+') {
				email[n] = person[m];
				m++;
				n++;
			}
			email[n] = '\0';
			//MessageBox(NULL, (email), TEXT("Email Return"), MB_OK);
		}
		m++;
	}

}



/************************************************************************
*		Function Name	:			TokenPhone							*
*		Working			:	- Get Phone Number of single person			*
*							- Temp.txt									*
*************************************************************************/
void TokenPhone(TCHAR person[512], TCHAR * phone) {

	int m = 0;
	int n = 0;
	while (person[m] != '\0') {
		if (person[m] == '$') {
			m++;
			while (person[m] != '$') {
				phone[n] = person[m];
				m++;
				n++;
			}
			phone[n] = '\0';
			//MessageBox(NULL, (phone), TEXT("Phone Return"), MB_OK);
		}
		m++;
	}
}


