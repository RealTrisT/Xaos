#include "Hyperion.h"
#include "TristGraphics.h"

#include <stdio.h>

#define ScrWidth 1920
#define ScrHeight 1080

UD* elUD = 0;
PIXEL_VERTEX* screen = 0;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_PAINT:
		elUD->PresentFrame();
		break;
	case WM_MOUSEMOVE:
		//elUD->ShitImage(screen); elUD->PresentFrame();
		break;
	} return DefWindowProc(hWnd, message, wParam, lParam);
};

bool InitializeHyperion(unsigned char* thismod) {
	screen = (PIXEL_VERTEX*)malloc(ScrWidth*ScrHeight*sizeof(PIXEL_VERTEX));

	PIXEL_VERTEX(*scr)[ScrWidth] = (PIXEL_VERTEX(*)[ScrWidth])screen;
	puts("started initing");
	for (unsigned y = 0; y < ScrHeight; y++) {
		for (unsigned x = 0; x < ScrWidth; x++) {
			scr[y][x] = { (float)x, (float)y, 0, {1.f, 0.f, 1.f, .1f} };
	}	}
	puts("finished initing screen buffer");

	elUD = new UD(ScrWidth, ScrHeight);

	elUD->SetWindowInitCallback([](UD* elUD) -> void { 
//		SetWindowLong(elUD->hWnd, GWL_EXSTYLE, GetWindowLong(elUD->hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(elUD->hWnd, RGB(0, 0, 0), 0, LWA_ALPHA);
		elUD->InitD3D(); 
	});
	elUD->InitWindow(WindowProc);

	puts("finished initing windowshit");
	
	/*unsigned i = 1080;
	while (getchar() != EOF) {
		screen[i++] = { 1.f, 0.f, 0.f, 1.f };
		elUD->ShitImage(screen); elUD->PresentFrame();
	}*/

	elUD->ShitImage(screen); elUD->PresentFrame();

	puts("finished shitting image and presenting frame");
	return true;
}

void TerminateHyperion() {
	elUD->TermD3D();
	elUD->TermWindow();
	delete elUD;
	free(screen);
}

extern "C" __declspec(dllexport) Xaos_ModuleInfo ModuleInfo = {
	"----------------------------- HYPERION LOADED -----------------------------",
	"Hyperion: God of Watchfulness, Wisdom and the Light.\nFather of: Helios (the sun), Selene (the moon) and Eos (dawn)\nThis module manages UI.\n",
	0,
	InitializeHyperion,
	TerminateHyperion
};