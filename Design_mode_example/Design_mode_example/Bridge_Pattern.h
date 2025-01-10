#pragma once
#include <iostream>

// ����
class Shape {
protected:
    class DrawAPI* api;
public:
    Shape(class DrawAPI* drawAPI) : api(drawAPI) {}
    virtual void draw() const = 0;
};

// ʵ�ֻ�
class DrawAPI {
public:
    virtual void draw() const = 0;
};

class RedCircle : public Shape {
public:
    RedCircle(class DrawAPI* drawAPI) : Shape(drawAPI) {}
    void draw() const override {
        api->draw();
        std::cout << "Drawing Red Circle\n";
    }
};

class GreenRectangle : public Shape {
public:
    GreenRectangle(class DrawAPI* drawAPI) : Shape(drawAPI) {}
    void draw() const override {
        api->draw();
        std::cout << "Drawing Green Rectangle\n";
    }
};

// ��ͬ��ʵ�ֻ�
class WindowsAPI : public DrawAPI {
public:
    void draw() const override {
        std::cout << "Using Windows API to draw\n";
    }
};

class MacOSAPI : public DrawAPI {
public:
    void draw() const override {
        std::cout << "Using MacOS API to draw\n";
    }
};