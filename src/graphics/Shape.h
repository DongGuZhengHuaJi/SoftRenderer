#pragma once
class Shape {
public:
    virtual ~Shape() = default;

    int getType() const { return m_type; }

protected:
    Shape(int type) : m_type(type) {}
    
    int m_type = -1;
};
