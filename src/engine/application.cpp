#include "application.h"

Application::Application() : view("[] Lost Horizon", 800, 600, this) {}

void Application::Init() {
    game.Init();
    // Setup the main resources and scene in the game
    game.SetupResources();
    game.SetupScene();
}

void Application::Start() {
    // Run game
    game.MainLoop();
}