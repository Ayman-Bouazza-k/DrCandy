/**
 * Virtual controller that keeps track of key presses and mouse buttons.
 */
#ifndef CONTROLLER_H
#define CONTROLLER_H

/// Keep track of controller inputs (one True at one frame per key press)
class Controller
{
public:
    Controller();

    /// Update the pressed states; should be called once per frame
    void update();

    /// @return whether the "up" arrow has been pressed this frame
    bool isUpPressed() const;
    /// @return whether the "down" arrow has been pressed this frame
    bool isDownPressed() const;
    /// @return whether the "left" arrow has been pressed this frame
    bool isLeftPressed() const;
    /// @return whether the "right" arrow has been pressed this frame
    bool isRightPressed() const;
    /// @return whether action button 1 has been pressed this frame
    bool isKey1Pressed() const;
    /// @return whether action button 2 has been pressed this frame
    bool isKey2Pressed() const;
    /// @return whether action button 3 has been pressed this frame
    bool isKey3Pressed() const;
    /// @return the current mouse x position, relative to the game window
    int getMouseX() const;
    /// @return the current mouse y position, relative to the game window
    int getMouseY() const;
    /// @return whether the left mouse button has been pressed this frame
    bool isMouseLeftPressed() const;
    /// @return whether the right mouse button has been pressed this frame
    bool isMouseRightPressed() const;

private:
    // Limit the options for controller buttons
    enum Index
    {
        UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3,
        KEY1 = 4, KEY2 = 5, KEY3 = 6,
        MOUSE_LEFT = 7, MOUSE_RIGHT = 8,
        COUNT = 9
    };

    // Change mapping for an index to a different keyboard constant
    void setMapping(int index, int keycode);

    // Get mapping for an index
    int getMapping(int index) const;

    // Internal storage for pressed states and mapping
    bool m_pressed[COUNT];
    int m_map[COUNT];

    // Track raw previous hardware state per-index so we can detect "just pressed" edges
    bool m_rawPrevious[COUNT];

    // Frame counter incremented on each update()
    int m_frame;

    // Mouse coordinates stored here and updated on update()
    int mouseX;
    int mouseY;
};
#endif
