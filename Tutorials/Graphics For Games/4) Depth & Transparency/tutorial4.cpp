#pragma comment(lib, "nclgl.lib")

#include "../../nclGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Depth and Transparency!", 800 , 600, false);
	if(!w.HasInitialised()) { return -1; }

	Renderer renderer(w);
	if(!renderer.HasInitialised()) { return -1; }

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			renderer.ToggleObject();
		}
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.ToggleDepth();
		}
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			renderer.ToggleAlphaBlend();
		}
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
			renderer.ToggleBlendMode();
		}

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
			renderer.MoveObject(0.1f);
		}
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
			renderer.MoveObject(-0.1f);
		}

		renderer.RenderScene();
	}

	return 0;
}