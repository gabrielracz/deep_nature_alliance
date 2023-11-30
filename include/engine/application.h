#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "resource_manager.h"
#include "view.h"
#include "game.h"
#include "defines.h"



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
    void SetMouseHandler(MouseHandler h) {view.SetMouseHandler(h);}
    void SetResizeHandler(ResizeHandler h) {view.SetResizeHandler(h);}

    int GetWinWidth() {return view.GetWidth();}
    int GetWinHeight() {return view.GetHeight();}
    Window GetWindow() { return view.GetWindow(); }
    float GetFPS() {return fps;}

};

#endif