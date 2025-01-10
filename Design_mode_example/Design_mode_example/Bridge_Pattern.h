#pragma once
#include <iostream>

// 抽象化
class Shape {
protected:
    class DrawAPI* api;
public:
    Shape(class DrawAPI* drawAPI) : api(drawAPI) {}
    virtual void draw() const = 0;
};

// 实现化
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

// 不同的实现化
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