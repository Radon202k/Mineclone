#ifndef INPUT_H
#define INPUT_H

typedef struct Button {
    bool down;
    bool pressed;
    bool released;
} Button;

typedef struct Mouse {
    v2 p;
    Button left;
    Button right;
} Mouse;

typedef union Keyboard {
    Button keys[9];
    struct {
        Button w;
        Button a;
        Button s;
        Button d;
        Button space;
        Button control;
        Button alt;
        Button lshift;
        Button rshift;
    };
} Keyboard;

global Mouse mouse;
global Keyboard keyboard;

#endif //INPUT_H
