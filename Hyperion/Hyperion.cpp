#include "Hyperion.h"
#include "TristGraphics.h"

#include <stdio.h>

#include <dwmapi.h>
#pragma comment (lib, "Dwmapi.lib")

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
			scr[y][x] = { (float)x, (float)y, 0, { float(x) / float(ScrWidth), float(y) / float(ScrHeight), 0.f, .5f} };
	}	}
	

	for (unsigned y = 100; y < ScrHeight-100; y++) {
		for (unsigned x = 100; x < ScrWidth-100; x++) {
			scr[y][x] = { 1.f, float(x) / float(ScrWidth), float(y) / float(ScrHeight), 0.2f };
	}	}

	puts("finished initing screen buffer");

	elUD = new UD(ScrWidth, ScrHeight);

	elUD->SetWindowInitCallback([](UD* elUDp) -> void {
		SetLayeredWindowAttributes(elUD->hWnd, RGB(0, 0, 0), 0xFF, LWA_ALPHA);
		MARGINS margs = { 0, ScrWidth, 0, ScrHeight };
		DwmExtendFrameIntoClientArea(elUDp->hWnd, &margs);
		
		elUD->InitD3D(); 
	});
	elUD->InitWindow(WindowProc);

	puts("finished initing windowshit");
	

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