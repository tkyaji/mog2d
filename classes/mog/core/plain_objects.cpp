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

Point Point::operator+(const Point &p) const {
    return Point(this->x + p.x, this->y + p.y);
}

Point Point::operator-(const Point &p) const {
    return Point(this->x - p.x, this->y - p.y);
}

Point Point::operator*(const Point &p) const {
    return Point(this->x * p.x, this->y * p.y);
}

Point Point::operator/(const Point &p) const {
    return Point(this->x / p.x, this->y / p.y);
}

Point Point::operator+(float value) const {
    return Point(this->x + value, this->y + value);
}

Point Point::operator-(float value) const {
    return Point(this->x - value, this->y - value);
}

Point Point::operator*(float value) const {
    return Point(this->x * value, this->y * value);
}

Point Point::operator/(float value) const {
    return Point(this->x / value, this->y / value);
}

bool Point::operator==(const Point &p) const {
    return this->x == p.x && this->y == p.y;
}

bool Point::operator!=(const Point &p) const {
    return !(*this == p);
}


#pragma - Size

Size Size::zero(0, 0);
Size Size::half(0.5f, 0.5f);
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

Size Size::operator+(const Size &size) const {
    return Size(this->width + size.width, this->height + size.height);
}

Size Size::operator-(const Size &size) const {
    return Size(this->width - size.width, this->height - size.height);
}

Size Size::operator*(const Size &size) const {
    return Size(this->width * size.width, this->height * size.height);
}

Size Size::operator/(const Size &size) const {
    return Size(this->width / size.width, this->height / size.height);
}

Size Size::operator+(float value) const {
    return Size(this->width + value, this->height + value);
}

Size Size::operator-(float value) const {
    return Size(this->width - value, this->height - value);
}

Size Size::operator*(float value) const {
    return Size(this->width * value, this->height * value);
}

Size Size::operator/(float value) const {
    return Size(this->width / value, this->height / value);
}

bool Size::operator==(const Size &size) const {
    return this->width == size.width && this->height == size.height;
}

bool Size::operator!=(const Size &size) const {
    return !(*this == size);
}


#pragma - Rect

Rect Rect::zero(0, 0, 0, 0);

Rect::Rect() {
}

Rect::Rect(const Point &position, const Size &size) {
    this->position = position;
    this->size = size;
}

Rect::Rect(float x, float y, float width, float height) {
    this->position = Point(x, y);
    this->size = Size(width, height);
}

bool Rect::operator==(const Rect &rect) const {
    return this->position == rect.position && this->size == rect.size;
}

bool Rect::operator!=(const Rect &rect) const {
    return !(*this == rect);
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

Color Color::operator*(const Color &c) const {
    return Color(this->r * c.r, this->g * c.g, this->b * c.b, this->a * c.a);
}

Color Color::operator*(float c) const {
    return Color(this->r * c, this->g * c, this->b * c, this->a * c);
}

bool Color::operator==(const Color &c) const {
    return (this->r == c.r && this->g == c.g && this->b == c.b && this->a == c.a);
}

bool Color::operator!=(const Color &c) const {
    return !(*this == c);
}
