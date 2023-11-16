#ifndef APPLICATION_H
#define APPLICATION_H
#include "resource_manager.h"
#include "view.h"
#include "game.h"
#include <memory>


class Application {
private:
    View view;
    Game game;
    ResourceManager resman;
    bool running = false;
    float fps = 0.0f;
public:
    Application();
    void Init();
    void Start();
    void Quit();
    void Pause();


    void ToggleMouseCapture() { view.ToggleMouseCapture(); }
    void ToggleRenderMode();
    Camera& GetCamera() {return view.GetCamera();}
    void SetMouseHandler(MouseHandler h) {view.SetMouseHandler(h);}
    void SetFirstPersonView() { view.InitFirstPersonView(); }
    void SetShipView() { view.InitShipView(); }

    int GetWinWidth() {return view.GetWidth();}
    int GetWinHeight() {return view.GetHeight();}
    View::Window GetWindow() { return view.GetWindow(); }
    float GetFPS() {return fps;}

};

#endif