#include "Hyperion.h"
#include "TristGraphics.h"

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
		elUD->ShitImage(screen); elUD->PresentFrame();
		break;
	} return DefWindowProc(hWnd, message, wParam, lParam);
};

bool InitializeHyperion(void* thismod) {
	screen = (PIXEL_VERTEX*)calloc(ScrWidth*ScrHeight, sizeof(PIXEL_VERTEX));

	PIXEL_VERTEX(*scr)[ScrWidth] = (PIXEL_VERTEX(*)[ScrWidth])screen;

	for (size_t x = 0; x < ScrWidth; x++){
		for (size_t y = 0; y < ScrHeight; y++) {
			scr[x][y] = { 1.f, 1.f, 1.f, .4f };
	}	}

	elUD = new UD(ScrWidth, ScrHeight);

	elUD->SetWindowInitCallback([](UD* elUD) -> void { elUD->InitD3D(); });
	elUD->InitWindow(WindowProc);
}

void TerminateHyperion() {
	elUD->TermD3D();
	elUD->TermWindow();

	free(screen);
}