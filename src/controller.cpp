#include "controller.h"
#include "keyboard.h"
#include "mouse.h"
#include <cstring>

Controller::Controller()
{
    // Zero pressed states
    std::memset(m_pressed, 0, sizeof(m_pressed));

    // Zero previous raw states
    std::memset(m_rawPrevious, 0, sizeof(m_rawPrevious));

    // initialize frame counter
    m_frame = 0;

    // Keyboard mapping defaults
    m_map[UP] = KEYBOARD_UP;
    m_map[DOWN] = KEYBOARD_DOWN;
    m_map[LEFT] = KEYBOARD_LEFT;
    m_map[RIGHT] = KEYBOARD_RIGHT;
    m_map[KEY1] = KEYBOARD_Q;
    m_map[KEY2] = KEYBOARD_W;
    m_map[KEY3] = KEYBOARD_E;

    // Mouse mappings are unused for keyboard-based mapping API, initialize defensively
    m_map[MOUSE_LEFT] = -1;
    m_map[MOUSE_RIGHT] = -1;

    // initialize mouse coords
    mouseX = 0;
    mouseY = 0;
}

void Controller::update()
{
    // increment frame counter
    ++m_frame;

    // Update each index by reading the raw hardware state, detect rising-edge (just pressed)
    for (int i = 0; i < Index::COUNT; ++i)
    {
        bool raw = false;
        switch (i)
        {
        case MOUSE_LEFT:
            raw = (Mouse_getBtnLeft() != 0);
            break;
        case MOUSE_RIGHT:
            {
                int mx, my;
                Uint32 mask = SDL_GetMouseState(&mx, &my);
                raw = (mask & SDL_BUTTON(BUTTON_RIGHT)) != 0;
                break;
            }
        default:
            raw = (Keyboard_GetKeyCnt(m_map[i]) != 0);
            break;
        }
        // m_pressed should be true only on the first frame the button becomes pressed
        m_pressed[i] = (raw && !m_rawPrevious[i]);

        // store raw for next-frame comparison
        m_rawPrevious[i] = raw;
    }

    // Update mouse position using provided mouse API
    mouseX = Mouse_getX();
    mouseY = Mouse_getY();
}

bool Controller::isUpPressed() const { return m_pressed[UP]; }
bool Controller::isDownPressed() const { return m_pressed[DOWN]; }
bool Controller::isLeftPressed() const { return m_pressed[LEFT]; }
bool Controller::isRightPressed() const { return m_pressed[RIGHT]; }
bool Controller::isKey1Pressed() const { return m_pressed[KEY1]; }
bool Controller::isKey2Pressed() const { return m_pressed[KEY2]; }
bool Controller::isKey3Pressed() const { return m_pressed[KEY3]; }

int Controller::getMouseX() const { return mouseX; }
int Controller::getMouseY() const { return mouseY; }
bool Controller::isMouseLeftPressed() const { return m_pressed[MOUSE_LEFT]; }
bool Controller::isMouseRightPressed() const { return m_pressed[MOUSE_RIGHT]; }

void Controller::setMapping(int index, int keycode)
{
    if (index >= 0 && index < Index::COUNT)
    {
        m_map[index] = keycode;
    }
}

int Controller::getMapping(int index) const
{
    int mapping = -1;
    if (index >= 0 && index < Index::COUNT)
    {
        mapping = m_map[index];
    }
    return mapping;
}
