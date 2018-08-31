#include "ToxicInjector.h"

// On update button click method
void ToxicInject::Application::OnUpdateButton_Click()
{
	this->UpdateProcesses();
}

// On inject button click method
void ToxicInject::Application::OnInjectButton_Click()
{
	std::wstring process_name{};
	process_name.resize(MAX_PATH);

	std::wstring process_id{};
	process_id.resize(MAX_PATH);

	if (int selected_index = ListView_GetNextItem(this->hProcess_ListView, -1, LVNI_SELECTED); selected_index > 0)
	{
		LVITEM lvI{};
		lvI.iItem = selected_index;
		lvI.mask = LVIF_TEXT;
		lvI.cchTextMax = MAX_PATH;
		lvI.pszText = &process_name[0];
		ListView_GetItem(this->hProcess_ListView, &lvI);
		lvI.pszText = &process_id[0];
		lvI.iSubItem = 1;
		ListView_GetItem(this->hProcess_ListView, &lvI);

		if (this->dll_path.empty())
			MessageBox(this->hwnd, L"Browse correct dll!", L"ERROR!", MB_ICONERROR);
		else
			Shkolnik::Windows_Native::DLL_Inject injector(this->dll_path, std::stoi(process_id));

		this->UpdateProcesses();
	}
}

// On browse button click method
void ToxicInject::Application::OnBrowseButton_Click()
{
	OPENFILENAME ofn{};			// common dialog box structure
	this->dll_path.resize(MAX_PATH);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->hwnd;
	ofn.lpstrFile = &this->dll_path[0];
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"DLL\0*.dll\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = L"Choose dll file:";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	GetOpenFileName(&ofn);

	this->dll_path.erase(std::remove(std::begin(this->dll_path), std::end(this->dll_path), 0), std::end(this->dll_path));
}

// Method register native window object
bool ToxicInject::Application::RegisterNativeWindow()
{
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = this->ApplicationProc;
	wc.lpszClassName = this->window_title.c_str();
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return false;	

	if (this->hwnd = CreateWindowEx(
		0,
		this->window_title.c_str(),
		this->window_title.c_str(),
		WS_POPUP,
		(GetSystemMetrics(SM_CXSCREEN) - this->window_size.x) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - this->window_size.y) / 2,
		this->window_size.x,
		this->window_size.y,
		NULL,
		NULL,
		NULL, this); !this->hwnd)
		return false;

	UpdateWindow(this->hwnd);
	ShowWindow(this->hwnd, SW_SHOW);
	return true;
}

// Window message procedure
LRESULT ToxicInject::Application::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// Executes when window creates
		case WM_CREATE:
		{
			this->CreateNativeControls();
			this->UpdateProcesses();
		}
		break;

		// Need for paint GUI
		case WM_PAINT:
		{
			this->OnWindow_Paint();
		}
		break;

		// Need for draw custom controls
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pdis = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
			switch (pdis->CtlID)
			{
				case UPDATE_BUTTON:
				{
					if (pdis->itemState & ODS_SELECTED)
						this->isMouseOnUpdateBtn = true;
					else
						this->isMouseOnUpdateBtn = false;
					this->DrawUpdateButton(pdis->hDC);

					//return TRUE;
				}
				break;

				case INJECT_BUTTON:
				{
					if (pdis->itemState & ODS_SELECTED)
						this->isMouseOnInjectBtn = true;
					else
						this->isMouseOnInjectBtn = false;
					this->DrawInjectButton(pdis->hDC);

					//return TRUE;
				}
				break;

				case BROWSE_BUTTON:
				{
					if (pdis->itemState & ODS_SELECTED)
						this->isMouseOnBrowseBtn = true;
					else
						this->isMouseOnBrowseBtn = false;
					this->DrawBrowseButton(pdis->hDC);

					//return TRUE;
				}
				break;
			}
		}
		break;

		// Need for drag window without title bar and borders
		case WM_LBUTTONDOWN:
		{
			ReleaseCapture();
			SendMessage(hWnd, WM_SYSCOMMAND, 61458, 0);
		}
		break;

		// Need for change cursor on custom buttons
		case WM_SETCURSOR:
		{
			HWND wind = reinterpret_cast<HWND>(wParam);
			if (wind == this->hUpdate_Button || wind == this->hInject_Button || wind == this->hBrowse_Button)
			{
				SetCursor(LoadCursor(NULL, IDC_HAND));
				return(TRUE);
			}
		}
		break;

		// Executes when user press button(button clicks)
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case UPDATE_BUTTON:
				{
					this->OnUpdateButton_Click();
				}
				break;

				case INJECT_BUTTON:
				{
					this->OnInjectButton_Click();
				}
				break;

				case BROWSE_BUTTON:
				{
					this->OnBrowseButton_Click();
				}
				break;
			}
		}
		break;

		// Executes when window is closed/terminated...
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Static application message procedure
LRESULT ToxicInject::Application::ApplicationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Application* pApp;
	if (uMsg == WM_NCCREATE)
	{
		pApp = static_cast<Application*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp)))
		{
			if (GetLastError() != 0)
				return false;
		}
	}
	else
	{
		pApp = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	 
	if (pApp)
	{
		pApp->hwnd = hWnd;
		return pApp->WindowProc(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Method called on window draw
void ToxicInject::Application::OnWindow_Paint()
{
	RECT rc{ 220, 0, 440, 200 };
	RECT rc1{ 220, 0, 440, 30 };
	
	HFONT hTahoma_Bold = CreateFont(23, 0, 0, 0, FW_BOLD,0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Tahoma"));
	HFONT hTahoma_Regular = CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Tahoma"));
	HFONT hArial_Small = CreateFont(13, 0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	HBRUSH section_head_bg_color{ CreateSolidBrush(RGB(45, 45, 45)) };
	HBRUSH section_bg_color{ CreateSolidBrush(RGB(60, 60, 60)) };
	PAINTSTRUCT ps{};
	BeginPaint(this->hwnd, &ps);

	/*DRAWING RIGHT SECTION*/
	FillRect(ps.hdc, &rc, section_bg_color);
	FillRect(ps.hdc, &rc1, section_head_bg_color);

	/*DRAWING HEADER TEXT*/
	SetTextColor(ps.hdc, RGB(255, 255, 255));
	SetBkColor(ps.hdc, RGB(45, 45, 45));
	SelectObject(ps.hdc, hTahoma_Bold);
	TextOut(ps.hdc, 260, 3, this->window_title.c_str(), this->window_title.length());

	/*DRAWING COPYRIGHT BOTTOM TEXT*/
	SetTextColor(ps.hdc, RGB(128, 128, 128));
	SetBkColor(ps.hdc, RGB(60, 60, 60));
	SelectObject(ps.hdc, hArial_Small);
	TextOut(ps.hdc, 260, 180, L"C0D3D by: Shkolnik Prahramist", 30);

	/*DELETING GDI-OBJECTS! FUCK-THIS-SHIT!!!*/
	DeleteObject(section_bg_color);
	DeleteObject(section_head_bg_color);
	DeleteObject(hTahoma_Bold);
	DeleteObject(hTahoma_Regular);
	DeleteObject(hArial_Small);
	EndPaint(this->hwnd, &ps);
}

// Method creates all native-window controls
void ToxicInject::Application::CreateNativeControls()
{
	INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	this->hProcess_ListView = CreateWindow(WC_LISTVIEW, L"", 
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 0, 0, 220, 200, this->hwnd, 0, 0, 0);

	SendMessage(this->hProcess_ListView, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	std::wstring col_1{ L"Process Name:" }, col_2{ L"Process ID:" };	

	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text,
	// and subitem members of the structure are valid.
	LVCOLUMN lvc{};
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.pszText = (LPWSTR)col_1.c_str();
	
	// Insert the columns into the list view.
	ListView_InsertColumn(this->hProcess_ListView, 0, &lvc);
	lvc.pszText = (LPWSTR)col_2.c_str();
	ListView_InsertColumn(this->hProcess_ListView, 1, &lvc);

	this->hUpdate_Button = CreateWindow(L"BUTTON", L"", WS_VISIBLE| WS_CHILD | BS_OWNERDRAW, 224, 50, 212, 26, this->hwnd, reinterpret_cast<HMENU>(this->UPDATE_BUTTON), 0, 0);
	this->hInject_Button = CreateWindow(L"BUTTON", L"", WS_VISIBLE| WS_CHILD | BS_OWNERDRAW, 224, 76 + 15, 212, 26, this->hwnd, reinterpret_cast<HMENU>(this->INJECT_BUTTON), 0, 0);
	this->hBrowse_Button = CreateWindow(L"BUTTON", L"", WS_VISIBLE| WS_CHILD | BS_OWNERDRAW, 224, 91 + 26 + 15, 212, 26, this->hwnd, reinterpret_cast<HMENU>(this->BROWSE_BUTTON), 0, 0);
}

// Method add row to Processes ListView
void ToxicInject::Application::ListView_AddItem(const int &index, const std::wstring &process_name, const int &pID)
{
	// Initialize LVITEM members that are common to all items.
	LVITEM lvI{};
	lvI.mask = LVIF_TEXT | LVIF_STATE;
	lvI.pszText = (LPWSTR)process_name.c_str();
	lvI.iItem = index;
	ListView_InsertItem(this->hProcess_ListView, &lvI);
	
	std::wstring num = std::to_wstring(pID);
	lvI.iSubItem = 1;
	lvI.pszText = (LPWSTR)num.c_str();
	ListView_SetItem(this->hProcess_ListView, &lvI);
}

// Method updates process list
void ToxicInject::Application::UpdateProcesses()
{
	// Need for display correct process list
	Sleep(10);
	
	ListView_DeleteAllItems(this->hProcess_ListView);
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };
	Process32First(hSnap, &pe32);
	int index{};
	do
	{
		this->ListView_AddItem(index, pe32.szExeFile, pe32.th32ProcessID);
		index++;
	} while (Process32Next(hSnap, &pe32));
	CloseHandle(hSnap);
}

// Method draw update button
void ToxicInject::Application::DrawUpdateButton(HDC hDC)
{
	RECT rc{ 0, 0, 212, 23 };
	HBRUSH bg{};
	HFONT hTahoma_Regular = CreateFont(19, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Tahoma"));
	HPEN hPen = CreatePen(PS_SOLID, 7, RGB(44, 97, 46));

	if (this->isMouseOnUpdateBtn)
	{
		bg = CreateSolidBrush(RGB(44, 97, 46));

		/*DRAW BACKGROUND*/
		FillRect(hDC, &rc, bg);

		/*DRAW BOTTOM LINE*/
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, 26, 0);
		LineTo(hDC, 212, 26);

		/*DRAW TEXT*/
		SetBkColor(hDC, RGB(44, 97, 46));
		SetTextColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hTahoma_Regular);

		TextOut(hDC, 83, 3, L"Update", 7);

		/*FREE GDI MEMORY*/
		DeleteObject(bg);
		DeleteObject(hTahoma_Regular);
		DeleteObject(hPen);
	}
	else
	{
		bg = CreateSolidBrush(RGB(67, 156, 70));

		/*DRAW BACKGROUND*/
		FillRect(hDC, &rc, bg);

		/*DRAW BOTTOM LINE*/
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, 26, 0);
		LineTo(hDC, 212, 26);

		/*DRAW TEXT*/
		SetBkColor(hDC, RGB(67, 156, 70));
		SetTextColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hTahoma_Regular);

		TextOut(hDC, 83, 2, L"Update", 7);

		/*FREE GDI MEMORY*/
		DeleteObject(bg);
		DeleteObject(hTahoma_Regular);
		DeleteObject(hPen);
	}
}

// Method draw inject button
void ToxicInject::Application::DrawInjectButton(HDC hDC)
{
	RECT rc{ 0, 0, 212, 23 };
	HBRUSH bg{};
	HFONT hTahoma_Regular = CreateFont(19, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Tahoma"));
	HPEN hPen = CreatePen(PS_SOLID, 7, RGB(44, 97, 46));

	if (this->isMouseOnInjectBtn)
	{
		bg = CreateSolidBrush(RGB(44, 97, 46));

		/*DRAW BACKGROUND*/
		FillRect(hDC, &rc, bg);

		/*DRAW BOTTOM LINE*/
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, 26, 0);
		LineTo(hDC, 212, 26);

		/*DRAW TEXT*/
		SetBkColor(hDC, RGB(44, 97, 46));
		SetTextColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hTahoma_Regular);

		TextOut(hDC, 87, 3, L"Inject", 7);

		/*FREE GDI MEMORY*/
		DeleteObject(bg);
		DeleteObject(hTahoma_Regular);
		DeleteObject(hPen);
	}
	else
	{
		bg = CreateSolidBrush(RGB(67, 156, 70));

		/*DRAW BACKGROUND*/
		FillRect(hDC, &rc, bg);

		/*DRAW BOTTOM LINE*/
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, 26, 0);
		LineTo(hDC, 212, 26);

		/*DRAW TEXT*/
		SetBkColor(hDC, RGB(67, 156, 70));
		SetTextColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hTahoma_Regular);

		TextOut(hDC, 87, 2, L"Inject", 7);

		/*FREE GDI MEMORY*/
		DeleteObject(bg);
		DeleteObject(hTahoma_Regular);
		DeleteObject(hPen);
	}
}

// Method draw browse button
void ToxicInject::Application::DrawBrowseButton(HDC hDC)
{
	RECT rc{ 0, 0, 212, 23 };
	HBRUSH bg{};
	HFONT hTahoma_Regular = CreateFont(19, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Tahoma"));
	HPEN hPen = CreatePen(PS_SOLID, 7, RGB(44, 97, 46));

	if (this->isMouseOnBrowseBtn)
	{
		bg = CreateSolidBrush(RGB(44, 97, 46));

		/*DRAW BACKGROUND*/
		FillRect(hDC, &rc, bg);

		/*DRAW BOTTOM LINE*/
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, 26, 0);
		LineTo(hDC, 212, 26);

		/*DRAW TEXT*/
		SetBkColor(hDC, RGB(44, 97, 46));
		SetTextColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hTahoma_Regular);

		TextOut(hDC, 83, 3, L"Browse", 7);

		/*FREE GDI MEMORY*/
		DeleteObject(bg);
		DeleteObject(hTahoma_Regular);
		DeleteObject(hPen);
	}
	else
	{
		bg = CreateSolidBrush(RGB(67, 156, 70));

		/*DRAW BACKGROUND*/
		FillRect(hDC, &rc, bg);

		/*DRAW BOTTOM LINE*/
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, 26, 0);
		LineTo(hDC, 212, 26);

		/*DRAW TEXT*/
		SetBkColor(hDC, RGB(67, 156, 70));
		SetTextColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hTahoma_Regular);

		TextOut(hDC, 83, 2, L"Browse", 7);

		/*FREE GDI MEMORY*/
		DeleteObject(bg);
		DeleteObject(hTahoma_Regular);
		DeleteObject(hPen);
	}
}

// CTOR
ToxicInject::Application::Application(const std::wstring &title, const Vector2D &window_size, const Vector2D &window_pos)
	: window_title(title), window_size(window_size), window_pos(window_pos)
{
	if (!this->RegisterNativeWindow())
		throw std::wstring(L"ToxicInject Application: can't register native window!");
}

// DTOR
ToxicInject::Application::~Application()
{
}

// Method starts native window-message loop
void ToxicInject::Application::Run()
{
	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
