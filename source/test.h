#pragma once

namespace test
{
    const float SCREEN_WIDTH = 1024;
    const float SCREEN_HEIGHT = 768;

    void render(void);           // Only platform::drawPoint should be used
    void update(int dt);         // dt in milliseconds
    void on_click(int x, int y); // x, y - in pixels

    void init(void);
    void term(void);
}; // namespace test