#include "Graphics/Window.h"
#include "Graphics/GraphicsEngine.h"
#include <Windows.h>


int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
	Graphics::Window win(L"Hello, World");
	Graphics::GraphicsEngine eng(win);

	bool run = true;

	while (run)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				run = false;
		}
		eng.ClearScreen();
		eng.Present();
	}
	return 0;
}