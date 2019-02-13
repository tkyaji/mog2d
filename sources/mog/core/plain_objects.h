#ifndef plain_objects_h
#define plain_objects_h

namespace mog {
    class Size;
    
    class Point {
    public:
        Point();
        Point(float x, float y);
        Point(const Point &point);
        Point(const Size &size);
        float x = 0;
        float y = 0;
        Point operator+(const Point &p) const;
        Point operator-(const Point &p) const;
        Point operator*(const Point &p) const;
        Point operator/(const Point &p) const;
        Point operator+(float value) const;
        Point operator-(float value) const;
        Point operator*(float value) const;
        Point operator/(float value) const;
        Point &operator+=(const Point &p);
        Point &operator-=(const Point &p);
        Point &operator*=(const Point &p);
        Point &operator/=(const Point &p);
        Point &operator+=(float value);
        Point &operator-=(float value);
        Point &operator*=(float value);
        Point &operator/=(float value);
        bool operator==(const Point &p) const;
        bool operator!=(const Point &p) const;

        static Point zero;
        static Point one;
        static Point half;
        
        static float dot(const Point &p1, const Point &p2);
        static float cross(const Point &p1, const Point &p2);
        static float length(const Point &p);
        static Point normalize(const Point &p);
    };
    
    
    class Size {
    public:
        Size();
        Size(float width, float height);
        Size(const Size &size);
        Size(const Point &point);
        float width = 0;
        float height = 0;
        Size operator+(const Size &size) const;
        Size operator-(const Size &size) const;
        Size operator*(const Size &size) const;
        Size operator/(const Size &size) const;
        Size operator+(float value) const;
        Size operator-(float value) const;
        Size operator*(float value) const;
        Size operator/(float value) const;
        Size &operator+=(const Size &p);
        Size &operator-=(const Size &p);
        Size &operator*=(const Size &p);
        Size &operator/=(const Size &p);
        Size &operator+=(float value);
        Size &operator-=(float value);
        Size &operator*=(float value);
        Size &operator/=(float value);
        bool operator==(const Size &size) const;
        bool operator!=(const Size &size) const;

        static Size zero;
    };
    
    
    class Rect {
    public:
        Rect();
        Rect(const Point &position, const Size &size);
        Rect(float x, float y, float width, float height);
        Point position = Point::zero;
        Size size = Size::zero;
        bool operator==(const Rect &rect) const;
        bool operator!=(const Rect &rect) const;

        static Rect zero;
    };

    
    class Color {
    public:
        Color();
        Color(float r, float g, float b, float a = 1.0f);
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 0;
        
        Color operator*(const Color &c) const;
        Color operator*(float c) const;
        bool operator==(const Color &c) const;
        bool operator!=(const Color &c) const;

        static Color white;
        static Color black;
        static Color red;
        static Color green;
        static Color blue;
        static Color yellow;
        static Color transparent;
    };
    
}

#endif /* plain_objects_h */
