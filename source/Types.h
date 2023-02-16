#pragma once

struct Color
{
    float r, g, b;
};

struct Vector2f
{
    float x, y;

    Vector2f operator*(float scalar) const
    {
        return { x * scalar, y * scalar };
    }

    void operator+=(const Vector2f& other)
    {
        x += other.x;
        y += other.y;
    }
};

struct Bounds
{
    Vector2f topLeft;
    Vector2f bottomRight;

    inline bool test(Vector2f pos) const
    {
        return (pos.x >= topLeft.x && pos.y >= topLeft.y && pos.x <= bottomRight.x && pos.y <= bottomRight.y);
    }
};