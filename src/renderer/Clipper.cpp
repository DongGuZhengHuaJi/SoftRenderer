#include "renderer/Clipper.h"

int Clipper::computeOutCode(const Vec2& p, const Rect& rect) {
    int code = INSIDE;
    if (p.x < rect.xMin) code |= LEFT;
    if (p.x > rect.xMax) code |= RIGHT;
    if (p.y < rect.yMin) code |= BOTTOM;
    if (p.y > rect.yMax) code |= TOP;
    return code;
}

bool Clipper::clipLine(Vec2& a, Vec2& b, const Rect& rect) {
    int codeA = computeOutCode(a, rect);
    int codeB = computeOutCode(b, rect);

    while (true) {
        if (!(codeA | codeB)) {
            // Both inside
            return true;
        }
        if (codeA & codeB) {
            // Both outside same edge — trivially reject
            return false;
        }

        int codeOut = codeA ? codeA : codeB;
        Vec2 intersect;

        if (codeOut & TOP) {
            // rect.yMax
            float t = (rect.yMax - a.y) / (b.y - a.y);
            intersect.x = a.x + t * (b.x - a.x);
            intersect.y = rect.yMax;
        } else if (codeOut & BOTTOM) {
            float t = (rect.yMin - a.y) / (b.y - a.y);
            intersect.x = a.x + t * (b.x - a.x);
            intersect.y = rect.yMin;
        } else if (codeOut & RIGHT) {
            float t = (rect.xMax - a.x) / (b.x - a.x);
            intersect.x = rect.xMax;
            intersect.y = a.y + t * (b.y - a.y);
        } else { // LEFT
            float t = (rect.xMin - a.x) / (b.x - a.x);
            intersect.x = rect.xMin;
            intersect.y = a.y + t * (b.y - a.y);
        }

        if (codeOut == codeA) {
            a = intersect;
            codeA = computeOutCode(a, rect);
        } else {
            b = intersect;
            codeB = computeOutCode(b, rect);
        }
    }
}

Vec2 Clipper::intersectLine(const Vec2& inside, const Vec2& outside,
                              const Vec2& edgeStart, const Vec2& edgeEnd) {
    Vec2 edge = edgeEnd - edgeStart;
    Vec2 seg = outside - inside;

    float cross = edge.cross(seg);
    if (std::abs(cross) < 1e-6f) {
        return inside; // parallel, degenerate
    }

    Vec2 toInside = inside - edgeStart;
    float t = toInside.cross(edge) / cross;

    return inside + seg * t;
}

std::vector<Vec2> Clipper::clipPolygonEdge(
    const std::vector<Vec2>& vertices,
    const Vec2& edgeStart, const Vec2& edgeEnd) {

    std::vector<Vec2> output;
    if (vertices.empty()) return output;

    Vec2 edge = edgeEnd - edgeStart;

    for (size_t i = 0; i < vertices.size(); i++) {
        const Vec2& current = vertices[i];
        const Vec2& next = vertices[(i + 1) % vertices.size()];

        Vec2 toCurrent = current - edgeStart;
        Vec2 toNext = next - edgeStart;

        bool curInside = edge.cross(toCurrent) >= 0;
        bool nextInside = edge.cross(toNext) >= 0;

        if (curInside && nextInside) {
            output.push_back(next);
        } else if (curInside && !nextInside) {
            output.push_back(intersectLine(current, next, edgeStart, edgeEnd));
        } else if (!curInside && nextInside) {
            output.push_back(intersectLine(current, next, edgeStart, edgeEnd));
            output.push_back(next);
        }
        // else both outside: add nothing
    }
    return output;
}

std::vector<Vec2> Clipper::clipPolygon(
    const std::vector<Vec2>& subject,
    const std::vector<Vec2>& clipPoly) {

    std::vector<Vec2> result = subject;

    for (size_t i = 0; i < clipPoly.size(); i++) {
        const Vec2& start = clipPoly[i];
        const Vec2& end = clipPoly[(i + 1) % clipPoly.size()];
        result = clipPolygonEdge(result, start, end);
        if (result.empty()) break;
    }
    return result;
}
