#include <iostream>
#include <exception>
#include "application.h"
#include <string.h>


// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Main function that builds and runs the game
int main(int argc, char** argv){
    Application app; // Game application 

    if(argc > 1) {
        std::string arg = std::string(argv[1]);
        if( arg == "--nosound") {
            app.SetSoundEnabled(false);
        } else {
            std::cout << "unknown argument: " << arg << std::endl;
        }
    }

    try {
        app.Init();
        app.Start();
    }
    catch (std::exception &e){
        PrintException(e);
    }

    return 0;
}
