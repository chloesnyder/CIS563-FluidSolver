#include <set>
#include <vector>
#include <functional>
#include <stdlib.h>
#include <stdio.h>

class InputHandler {
public:
    //NOTE: AUSTIN SUGGESTED THIS ORGANIZATIONAL STRUCTURE TO ME. THIS IS MY CITATION OF AUSTIN ENG.
    static InputHandler& getInputHandler() {
        static InputHandler InputHandler;
        return inputHandler;
    }

    double x() const;
    double y() const;
    bool key(int key) const;

    void x(double val, bool events = true);
    void y(double val, bool events = true);
    void pos(double x, double y, bool events = true);
    void key(int key, bool down, bool events = true);

    struct KeyboardState {
        double x;
        double y;
    };

    typedef std::function<void(int,int)> WindowListener;
    typedef std::function<void(KeyboardState&)> KeyboardListener;
    void registerKeyboardListener(KeyboardListener listener);
    void registerWindowListener(WindowListener listener);

private:
    InputHandler();
    InputHandler(InputHandler const&) {} //prevent copies
    void operator=(InputHandler const&) {} //prevent assignments
    std::set<int> _keyboard;
    KeyboardState _
};
