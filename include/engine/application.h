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
public:
    Application();
    void Init();
    void Start();
    void Quit();
    void Pause();


    void ToggleMouseCapture();
    Camera& GetCamera() {return view.GetCamera();}
    void SetMouseHandler(MouseHandler h) {view.SetMouseHandler(h);}

};

#endif