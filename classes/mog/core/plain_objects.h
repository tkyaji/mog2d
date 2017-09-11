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
        Point operator=(const Point &p);
        Point operator+(const Point &p);
        Point operator-(const Point &p);
        Point operator*(const Point &p);
        Point operator/(const Point &p);
        Point operator+(float value);
        Point operator-(float value);
        Point operator*(float value);
        Point operator/(float value);
        bool operator==(const Point &p);
        
        static Point zero;
        static Point one;
        static Point half;
    };
    
    
    class Size {
    public:
        Size();
        Size(float width, float height);
        Size(const Size &size);
        Size(const Point &point);
        float width = 0;
        float height = 0;
        Size operator=(const Size &size);
        Size operator+(const Size &size);
        Size operator-(const Size &size);
        Size operator*(const Size &size);
        Size operator/(const Size &size);
        Size operator+(float value);
        Size operator-(float value);
        Size operator*(float value);
        Size operator/(float value);
        bool operator==(const Size &size);
        
        static Size zero;
        static Size one;
    };

    
    class Color {
    public:
        Color(float r, float g, float b, float a = 1.0f);
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 0;
        
        Color operator*(const Color &c);
        Color operator*(float c);
        bool operator==(const Color &c);
        
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
