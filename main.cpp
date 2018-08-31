#include "ToxicInjector.h"

int CALLBACK wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	using namespace ToxicInject;
	
	try
	{
		Application app(L"Toxic - Injector", Vector2D(440, 200), Vector2D(0, 0));
		app.Run();
	}
	catch (std::wstring &e)
	{
		MessageBox(HWND_DESKTOP, e.c_str(), L"Exception!", MB_ICONERROR);
	}

	return EXIT_SUCCESS;
}