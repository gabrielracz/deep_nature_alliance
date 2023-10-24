#ifndef APPLICATION_H
#define APPLICATION_H
#include "view.h"
#include "game.h"
#include <memory>

class Application {
private:
    View view;
    Game game;
public:
    Application();
    void Init();
    void Start();

};

#endif