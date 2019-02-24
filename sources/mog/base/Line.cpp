#include "mog/base/Line.h"
#include <array>

using namespace mog;

static inline Point alignDirection(Point v, const Point &directionV) {
    if (v.x == 0 && v.y == 0) {
        v = directionV * Point(1.0f, -1.0f);
    }
    float c = directionV.x * v.y - v.x * directionV.y;
    if (c == 0) {
        v = Point(v.y, v.x);
        c = directionV.x * v.y - v.x * directionV.y;
    }
    if (c > 0) {
        v = v * -1.0f;
    }
    return v;
}

std::shared_ptr<Line> Line::create(const std::vector<Point> &points, float lineWidth, LineType lineType) {
    auto line = std::shared_ptr<Line>(new Line());
    line->init(points, lineWidth, lineType);
    return line;
}

Line::Line() {
}

void Line::init(const std::vector<Point> &points, float lineWidth, LineType lineType) {
    this->points = points;
    this->lineWidth = lineWidth;
    this->lineType = lineType;
    
    if (points.size() < 2) return;
    if (points.size() == 2) lineType = LineType::Lines;

    std::vector<Point> vertexPoints;
    switch (lineType) {
        case LineType::Lines:
            vertexPoints = this->createLinesVertexPoints(points, lineWidth);
            break;
        case LineType::LineStrip:
            vertexPoints = this->createLineStripVertexPoints(points, lineWidth);
            break;
        case LineType::LineLoop:
            vertexPoints = this->createLineLoopVertexPoints(points, lineWidth);
            break;
    }

    Polygon::init(vertexPoints);
}

std::vector<Point> Line::createLinesVertexPoints(const std::vector<Point> &points, float lineWidth) {
    auto _points = points;
    
    std::vector<Point> vertexPoints;
    vertexPoints.reserve(_points.size() * 2);
    
    int len = (int)_points.size();
    if (len % 2 == 1) len--;
    for (int i = 0; i < len; i=i+2) {
        Point p0 = _points[i];
        Point p1 = _points[i + 1];
        
        Point l = p1 - p0;
        Point n = Point::normalize(Point(-l.y, l.x));
        Point pp0a = p0 + n * lineWidth * 0.5f;
        Point pp0b = p0 - n * lineWidth * 0.5f;

        Point pp1a = p1 + n * lineWidth * 0.5f;
        Point pp1b = p1 - n * lineWidth * 0.5f;
        
        if (i > 0) {
            vertexPoints.emplace_back(vertexPoints[vertexPoints.size()-1]);
            vertexPoints.emplace_back(pp0a);
        }
        vertexPoints.emplace_back(pp0a);
        vertexPoints.emplace_back(pp0b);
        vertexPoints.emplace_back(pp1a);
        vertexPoints.emplace_back(pp1b);
    }
    
    return vertexPoints;
}

std::vector<Point> Line::createLineStripVertexPoints(const std::vector<Point> &points, float lineWidth) {
    auto _points = points;
    std::vector<Point> vertexPoints;
    vertexPoints.reserve(_points.size() * 2);
    
    for (int i = 0; i < _points.size() - 2; i++) {
        Point p0 = _points[i];
        Point p1 = _points[i + 1];
        Point p2 = _points[i + 2];
        
        Point l0 = p1 - p0;
        Point n0 = Point::normalize(Point(-l0.y, l0.x));
        Point t = Point::normalize(Point::normalize(p2 - p1) + Point::normalize(p1 - p0));
        Point m = Point(-t.y, t.x);
        float d = lineWidth * 0.5f / Point::dot(m, n0);
        
        Point v1 = Point::normalize(p1 - p0);
        Point v2 = Point::normalize(p1 - p2);
        Point v = Point::normalize(v1 + v2);
        v = alignDirection(v, v1);

        Point pp1a = p1 + v * d;
        Point pp1b = p1 - v * d;
        
        if (i == 0) {
            n0 = alignDirection(n0, v1);
            Point pp0a = p0 + n0 * lineWidth * 0.5f;
            Point pp0b = p0 - n0 * lineWidth * 0.5f;
            vertexPoints.emplace_back(pp0a);
            vertexPoints.emplace_back(pp0b);
        }
        
        vertexPoints.emplace_back(pp1a);
        vertexPoints.emplace_back(pp1b);
        
        if (i == _points.size() -3) {
            Point l2 = p2 - p1;
            Point n2 = Point::normalize(Point(-l2.y, l2.x));
            n2 = alignDirection(n2, v2 * -1.0f);
            Point pp2a = p2 + n2 * lineWidth * 0.5f;
            Point pp2b = p2 - n2 * lineWidth * 0.5f;
            vertexPoints.emplace_back(pp2a);
            vertexPoints.emplace_back(pp2b);
        }
    }
    
    return vertexPoints;
}

std::vector<Point> Line::createLineLoopVertexPoints(const std::vector<Point> &points, float lineWidth) {
    auto _points = points;
    _points.insert(_points.begin(), points[points.size()-1]);
    _points.emplace_back(points[0]);
    _points.emplace_back(points[1]);

    std::vector<Point> vertexPoints;
    vertexPoints.reserve(_points.size() * 2);
    
    for (int i = 0; i < _points.size() - 2; i++) {
        Point p0 = _points[i];
        Point p1 = _points[i + 1];
        Point p2 = _points[i + 2];
        
        Point l0 = p1 - p0;
        Point n0 = Point::normalize(Point(-l0.y, l0.x));
        Point t = Point::normalize(Point::normalize(p2 - p1) + Point::normalize(p1 - p0));
        float d = lineWidth * 0.5f;
        if (t.x != 0 || t.y != 0) {
            Point m = Point(-t.y, t.x);
            d = lineWidth * 0.5f / Point::dot(m, n0);
        }
        Point v1 = Point::normalize(p1 - p0);
        Point v2 = Point::normalize(p1 - p2);
        Point v = Point::normalize(v1 + v2);
        v = alignDirection(v, v1);
        
        Point pp1a = p1 + v * d;
        Point pp1b = p1 - v * d;
        
        vertexPoints.emplace_back(pp1a);
        vertexPoints.emplace_back(pp1b);
    }
    
    return vertexPoints;
}

std::vector<Point> Line::getPoints() {
    return this->points;
}

float Line::getLineWidth() {
    return this->lineWidth;
}
