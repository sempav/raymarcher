#pragma once

#include "defines.h"
#include "logger.h"

#include "camera.h"
#include "keyboard.h"
#include "quad.h"
#include "shader.h"

struct WindowInfo
{
	int width;
	int height;
	WindowInfo(int w, int h) : width(w), height(h) { }
};

class App
{
	WindowInfo window;

	float last_render_time;
	bool init_ok;

    std::unique_ptr<GLProgram> program;

	Quad *quad;

	float cur_camera_acceleration;
	SmoothCamera *camera;

	bool InitGL();
	bool InitObjects();

	void ProcessInput();

    App(const App &other) = delete;
    App& operator= (const App &other) = delete;
public:
	App(void);
	~App(void);

	bool Initialize();

	void onDisplay();
	void onIdle();
	void onReshape(int width, int height);
	void onMouseMove(int x, int y);

	//void Update(float delta_time);

	KeyboardListener keys;
	const WindowInfo* GetWindowInfo() {return &window; }
};
