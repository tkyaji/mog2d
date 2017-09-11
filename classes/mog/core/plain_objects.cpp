#include "mog/core/plain_objects.h"

using namespace mog;

#pragma - Point

Point Point::zero(0, 0);
Point Point::one(1, 1);
Point Point::half(0.5f, 0.5f);

Point::Point() {
}

Point::Point(float x, float y) {
    this->x = x;
    this->y = y;
}

Point::Point(const Point &point) {
    this->x = point.x;
    this->y = point.y;
}

Point::Point(const Size &size) {
    this->x = size.width;
    this->y = size.height;
}

Point Point::operator=(const Point &p) {
    this->x = p.x;
    this->y = p.y;
    return *this;
}

Point Point::operator+(const Point &p) {
    return Point(this->x + p.x, this->y + p.y);
}

Point Point::operator-(const Point &p) {
    return Point(this->x - p.x, this->y - p.y);
}

Point Point::operator*(const Point &p) {
    return Point(this->x * p.x, this->y * p.y);
}

Point Point::operator/(const Point &p) {
    return Point(this->x / p.x, this->y / p.y);
}

Point Point::operator+(float value) {
    return Point(this->x + value, this->y + value);
}

Point Point::operator-(float value) {
    return Point(this->x - value, this->y - value);
}

Point Point::operator*(float value) {
    return Point(this->x * value, this->y * value);
}

Point Point::operator/(float value) {
    return Point(this->x / value, this->y / value);
}

bool Point::operator==(const Point &p) {
    return this->x == p.x && this->y == p.y;
}


#pragma - Size

Size Size::zero(0, 0);
Size Size::one(1, 1);

Size::Size() {
}

Size::Size(float width, float height) {
    this->width = width;
    this->height = height;
}

Size::Size(const Size &size) {
    this->width = size.width;
    this->height = size.height;
}

Size::Size(const Point &point) {
    this->width = point.x;
    this->height = point.y;
}

Size Size::operator=(const Size &size) {
    this->width = size.width;
    this->height = size.height;
    return *this;
}

Size Size::operator+(const Size &size) {
    return Size(this->width + size.width, this->height + size.height);
}

Size Size::operator-(const Size &size) {
    return Size(this->width - size.width, this->height - size.height);
}

Size Size::operator*(const Size &size) {
    return Size(this->width * size.width, this->height * size.height);
}

Size Size::operator/(const Size &size) {
    return Size(this->width / size.width, this->height / size.height);
}

Size Size::operator+(float value) {
    return Size(this->width + value, this->height + value);
}

Size Size::operator-(float value) {
    return Size(this->width - value, this->height - value);
}

Size Size::operator*(float value) {
    return Size(this->width * value, this->height * value);
}

Size Size::operator/(float value) {
    return Size(this->width / value, this->height / value);
}

bool Size::operator==(const Size &size) {
    return this->width == size.width && this->height == size.height;
}


#pragma - Color

Color Color::white(1, 1, 1, 1);
Color Color::black(0, 0, 0, 1);
Color Color::red(1, 0, 0, 1);
Color Color::green(0, 1, 0, 1);
Color Color::blue(0, 0, 1, 1);
Color Color::yellow(1, 1, 0, 1);
Color Color::transparent(1, 1, 1, 0);

Color::Color(float r, float g, float b, float a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Color Color::operator*(const Color &c) {
    return Color(this->r * c.r, this->g * c.g, this->b * c.b, this->a * c.a);
}

Color Color::operator*(float c) {
    return Color(this->r * c, this->g * c, this->b * c, this->a * c);
}

bool Color::operator==(const Color &c) {
    return (this->r == c.r && this->g == c.g && this->b == c.b && this->a == c.a);
}
