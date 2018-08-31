#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Comctl32.lib")

#include <Windows.h>
#include <CommCtrl.h>
#include <TlHelp32.h>
#include <string>
#include <algorithm>
#include "DLL_Inject.hpp"
#include "resource.h"

namespace ToxicInject
{
	// Default 2d vector struct
	struct Vector2D
	{
		int x{}, y{};
		Vector2D(const int &x, const int &y)
			: x(x), y(y)
		{}
		Vector2D()
		{}
	};

	class Application
	{
	// This section contains all private application variables
	private:
		Vector2D window_size{};				// Contain vector, of window size(width, height)
		Vector2D window_pos{};				// Contain vector, of window pos(x, y)
		std::wstring window_title{};		// Contain unicode string, with window title
		std::wstring dll_path{};			// Contain unicode string, with choosed dll path

		HWND hwnd{};						// Contain window handle

		// Button's state varialbes
		bool isMouseOnUpdateBtn{};
		bool isMouseOnInjectBtn{};
		bool isMouseOnBrowseBtn{};

	// This section contains all private controls id
	private:
		const static int UPDATE_BUTTON{ 1337 };
		const static int INJECT_BUTTON{ 1338 };
		const static int BROWSE_BUTTON{ 1339 };

	// This section contains handle of all buttons
	private:
		HWND hUpdate_Button{};				// Handle of update button
		HWND hInject_Button{};				// Handle of inject button
		HWND hBrowse_Button{};				// Handle of browse button
		HWND hProcess_ListView{};			// Handle of ListView, that contains processes

	// This section contains on-click button methods
	private:
		// On update button click method
		void OnUpdateButton_Click();
		// On inject button click method
		void OnInjectButton_Click();
		// On browse button click method
		void OnBrowseButton_Click();

	// This section contains all private application methods
	private:
		// Method register native window object
		bool RegisterNativeWindow();

		// Window message procedure
		LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// Static application message procedure
		static LRESULT CALLBACK ApplicationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// Method called on window draw
		void OnWindow_Paint();

		// Method creates all native-window controls
		void CreateNativeControls();

		// Method add row to Processes ListView
		void ListView_AddItem(const int &index, const std::wstring &process_name, const int &pID);

		// Method updates process list
		void UpdateProcesses();

	// This section contains controls paint methods
	private:
		// Method draw update button
		void DrawUpdateButton(HDC hDC);
		// Method draw inject button
		void DrawInjectButton(HDC hDC);
		// Method draw browse button
		void DrawBrowseButton(HDC hDC);

	// This section contains all public application methods
	public:
		// CTOR
		Application(const std::wstring &title, const Vector2D &window_size, const Vector2D &window_pos);
		// DTOR
		~Application();

		// Method starts native window-message loop
		void Run();
	};
}