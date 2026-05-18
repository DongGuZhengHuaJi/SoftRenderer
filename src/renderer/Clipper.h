#pragma once
#include "math/Vec2.h"
#include <vector>

class Clipper {
public:
    // --- Line clipping (Cohen-Sutherland) ---
    static bool clipLine(Vec2& a, Vec2& b, const Rect& rect);

    // --- Polygon clipping (Sutherland-Hodgman) ---
    static std::vector<Vec2> clipPolygon(
        const std::vector<Vec2>& subject,
        const std::vector<Vec2>& clipPoly);

private:
    enum OutCode {
        INSIDE = 0,
        LEFT   = 1 << 0,
        RIGHT  = 1 << 1,
        BOTTOM = 1 << 2,
        TOP    = 1 << 3,
    };
    static int computeOutCode(const Vec2& p, const Rect& rect);

    static Vec2 intersectLine(const Vec2& inside, const Vec2& outside,
                               const Vec2& edgeStart, const Vec2& edgeEnd);
    static std::vector<Vec2> clipPolygonEdge(
        const std::vector<Vec2>& vertices,
        const Vec2& edgeStart, const Vec2& edgeEnd);
};
