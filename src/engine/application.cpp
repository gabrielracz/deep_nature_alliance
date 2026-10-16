#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "application.h"
#include "defines.h"




Application::Application() : view(*this, resman), game(*this, resman){}

void Application::Init() {
    view.Init("Deep Nature Alliance - GameDingos", window_width_g, window_height_g);
    game.Init();
}

void Application::Start() {
    // Run game
    running = true;
	float last_time = glfwGetTime();
	float current_time = 0;
	float dt = 0;
	float acc_delta_time = 0;
	unsigned int frame_counter = 0;
    int frame_window = 60;

    // DNA_CAPTURE: record every frame through the given command at a fixed 30 fps timestep
    const char* capture = std::getenv("DNA_CAPTURE");
    const float capture_dt = 1.0f / 30.0f;
    if(capture) {
        view.StartCapture(capture);
    }
    long long capture_frames = 0;
    // audio is recorded live, so only an audio capture has to run at real time.
    // The simulation is deterministic, so a slower high resolution video pass stays in sync with it.
    const bool pace_capture = std::getenv("DNA_CAPTURE_AUDIO") != nullptr;
    double capture_start = 0.0;

	while(running){
		//Get frame rate
        runtime += dt;
		frame_counter++;
		current_time = glfwGetTime();
		dt = capture ? capture_dt : current_time - last_time;
		acc_delta_time += dt;
		if(frame_counter % frame_window == 0){
            fps = frame_window/acc_delta_time;
            // std::cout << "fps: " << PRINT_FIXED_FLOAT(fps) << std::endl;
			acc_delta_time = 0;
		}
		last_time = current_time;

        game.Update(dt, view.GetKeys());
        view.Render(game.ActiveScene());

        if(capture && pace_capture) {
            // hold frames to a real time 30 fps schedule so recorded audio stays in sync
            if(capture_frames == 0) {
                capture_start = glfwGetTime();
            }
            double target = capture_start + (++capture_frames) * capture_dt;
            while(glfwGetTime() < target) {
                if(target - glfwGetTime() > 0.004) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                } else {
                    std::this_thread::yield();
                }
            }
        }
    }
}

void Application::Quit() {
    running = false;
    view.StopCapture();
}

void Application::Pause() {
    view.ToggleMouseCapture();
}

void Application::ToggleRenderMode() {
    view.ToggleRenderMode();
}