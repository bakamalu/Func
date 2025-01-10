#pragma once

#include <iostream>

namespace Factory_Method
{
    // Shape ½Ó¿Ú
    class Shape {
    public:
        virtual void draw() = 0;
        virtual ~Shape() {}
    };

    // Concrete Shapes
    class Circle : public Shape {
    public:
        void draw() override {
            std::cout << "Drawing Circle\n";
        }
    };

    class Square : public Shape {
    public:
        void draw() override {
            std::cout << "Drawing Square\n";
        }
    };

    // Creator with Factory Method
    class ShapeFactory {
    public:
        virtual Shape* createShape() = 0;
        virtual ~ShapeFactory() {}
    };

    // Concrete Creators
    class CircleFactory : public ShapeFactory {
    public:
        Shape* createShape() override {
            return new Circle();
        }
    };

    class SquareFactory : public ShapeFactory {
    public:
        Shape* createShape() override {
            return new Square();
        }
    };
};