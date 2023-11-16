#include <iostream>
#include <iomanip>

#include "application.h"
#include "defines.h"

Application::Application() : view(*this, resman), game(*this, resman){}

void Application::Init() {
    view.Init("view test", window_width_g, window_height_g);
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
	while(running){
		//Get frame rate
		frame_counter++;
		current_time = glfwGetTime();
		dt = current_time - last_time;
		acc_delta_time += dt;
		if(frame_counter % frame_window == 0){
            fps = frame_window/acc_delta_time;
            // std::cout << "fps: " << PRINT_FIXED_FLOAT(fps) << std::endl;
			acc_delta_time = 0;
		}
		last_time = current_time;

        game.Update(dt, view.GetKeys());
        view.Render(game.ActiveScene());
    }

}

void Application::Quit() {
    running = false;
}

void Application::Pause() {
    view.ToggleMouseCapture();
}

void Application::ToggleRenderMode() {
    view.ToggleRenderMode();
}


void Application::ToggleMouseCapture() {
    view.ToggleMouseCapture();
}