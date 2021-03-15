
#include <windows.h>

#undef max
#undef min

#include "utils.h" // утилиты

#include "Objects/objects.h" // объекты

#include "Sprites/sprite.cpp" // спрайты

bool running = true;

const bool debug_mode = true;
const bool Camera_mod = true; // подвижна ли у нас камера?

Camera camera;

Render_state render_state;

dot arena_half_size; // половина размера игрового окна

#include "Render/render.cpp" // рендер

#include "Game/game.cpp" // игра


// обновление ввода
void update_controls(HWND& window, Input& input) {

	// обнуляем значение нажатия кнопки
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input.buttons[i].changed = false;
	}

	bool isPeekMessage = false; // прочитали ли мы хоть раз сообщение

	MSG message;
	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
		isPeekMessage = true;

		auto set_button = [&](button_t b, bool is_down, bool changed) {
			input.buttons[b].is_down = is_down;
			input.buttons[b].changed = changed;
		};

		switch (message.message) {

		case WM_LBUTTONUP: {

			set_button(BUTTON_MOUSE_L, false, true);

		}break;

		case WM_LBUTTONDOWN: {

			set_button(BUTTON_MOUSE_L, true, true);

		}break;

		case WM_RBUTTONUP: {
			set_button(BUTTON_MOUSE_R, false, true);

		}break;

		case WM_RBUTTONDOWN: {

			set_button(BUTTON_MOUSE_R, true, true);

		}break;

		case WM_KEYUP:
		case WM_KEYDOWN: {

			u64 vk_code = message.wParam; //button
			bool is_down = ((message.lParam & (static_cast<u64>(1) << 31)) == 0); // нажата ли она

#define update_button(b, vk)\
case vk: {\
	set_button(b, is_down, (is_down != input.buttons[b].is_down));\
} break;

			switch (vk_code) {
				update_button(BUTTON_UP, VK_UP);
				update_button(BUTTON_DOWN, VK_DOWN);
				update_button(BUTTON_W, 'W');
				update_button(BUTTON_S, 'S');
				update_button(BUTTON_D, 'D');
				update_button(BUTTON_A, 'A');
				update_button(BUTTON_B, 'B');
				update_button(BUTTON_C, 'C');
				update_button(BUTTON_E, 'E');
				update_button(BUTTON_F, 'F');
				update_button(BUTTON_J, 'J');
				update_button(BUTTON_K, 'K');
				update_button(BUTTON_DEL, VK_DELETE);
				update_button(BUTTON_LEFT, VK_LEFT);
				update_button(BUTTON_RIGHT, VK_RIGHT);
				update_button(BUTTON_ENTER, VK_RETURN);
				update_button(BUTTON_ESC, VK_ESCAPE);
				update_button(BUTTON_TAB, VK_TAB);
				update_button(BUTTON_SPACE, VK_SPACE);
				update_button(BUTTON_0, VK_NUMPAD0);
				update_button(BUTTON_1, VK_NUMPAD1);
				update_button(BUTTON_2, VK_NUMPAD2);
				update_button(BUTTON_3, VK_NUMPAD3);
				update_button(BUTTON_4, VK_NUMPAD4);
				update_button(BUTTON_5, VK_NUMPAD5);
				update_button(BUTTON_6, VK_NUMPAD6);
				update_button(BUTTON_7, VK_NUMPAD7);
				update_button(BUTTON_8, VK_NUMPAD8);
				update_button(BUTTON_9, VK_NUMPAD9);
			}

#undef update_button

		}break;

		default: {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		}
	}

	// если в message есть положение мыши
	if (isPeekMessage) {
		// mouse update
		mouse.pos = dot(message.pt.x, message.pt.y) * 0.097;

		mouse.pos.x = clamp<point_t>(0, mouse.pos.x, 2 * arena_half_size.x) - arena_half_size.x;
		mouse.pos.y = -clamp<point_t>(0, mouse.pos.y, 2 * arena_half_size.y) + arena_half_size.y;
	}
}

// функция вызова события для окна
LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false;
	} break;

	case WM_SIZE: {

		// взятие размеров окна
		{
			RECT rect;
			GetClientRect(hwnd, &rect);

			render_state.resize(rect.right - rect.left, rect.bottom - rect.top);
		}

		// релаксация масштабирования
		scale_factor = render_state.height * render_scale;

		// релаксация размеров окна
		arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, 1.0 / render_scale) * 0.5;

	} break;

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
}


int main() {

	//скрывает консоль
	ShowWindow(GetConsoleWindow(), HIDE_WINDOW); 
	//ShowWindow(GetConsoleWindow(), SW_SHOW);

	// скрывает курсор
	ShowCursor(false);

	init_sprites();

	// Create Window class
	WNDCLASS window_class = {};
	{
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpszClassName = L"Game Window Class";
		window_class.lpfnWndProc = window_callback;
		//window_class.hIcon = static_cast<HICON>(LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	}

	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Register class
	RegisterClass(&window_class);

	// Create window
	HWND window = CreateWindow(window_class.lpszClassName, L"C++ Game Engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, 0, 0, hInstance, 0);
	//Fullscreen!!!!!!
	{
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}


	HDC hdc = GetDC(window);

	Input input = {};

	point_t delta_time = 1.0 / 60;

	u32 frame_count = 0; // колво кадров
	point_t frame_time_accum = 0; // время накопления кадров

	while (running) {
		update_controls(window, input);

		simulate_game(input, delta_time);

		// update fps
		{
			static s32 fps = 0;

			frame_count++;
			frame_time_accum += delta_time;
			if (frame_time_accum > 0.5) {
				fps = frame_count * 2;
				frame_time_accum = 0;
				frame_count = 0;
			}

			if (debug_mode) {
				draw_number(fps, dot(5, 5) - arena_half_size, 0.5, 0xffffff);
				draw_number(delta_time * 1000, dot(15, 5) - arena_half_size, 0.5, 0xffffff);
			}
		}

		// render screen
		{

			StretchDIBits(hdc, 0, 0, render_state.width, render_state.height,
				0, 0, render_state.width, render_state.height,
				reinterpret_cast<void*>(render_state.render_memory), &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		}

		// update delta time
		{
			static Timer timer;
			delta_time = timer.time();
			timer.reset();
		}
	}

	return 0;
}