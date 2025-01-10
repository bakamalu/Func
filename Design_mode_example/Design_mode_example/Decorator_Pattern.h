#pragma once
#include <iostream>
#include <string>

// 抽象组件
class Component {
public:
    virtual ~Component() {}
    virtual void print() const = 0;
};

// 具体组件
class SimpleText : public Component {
private:
    std::string text;
public:
    SimpleText(const std::string& text) : text(text) {}
    void print() const override {
        std::cout << text << std::endl;
    }
};

// 抽象装饰器
class Decorator : public Component {
protected:
    Component* component;
public:
    Decorator(Component* comp) : component(comp) {}
    void print() const override {
        component->print();
    }
};

// 具体装饰器
class BoldDecorator : public Decorator {
public:
    BoldDecorator(Component* comp) : Decorator(comp) {}
    void print() const override {
        std::cout << "<b>";
        Decorator::print();
        std::cout << "</b>" << std::endl;
    }
};

class ItalicDecorator : public Decorator {
public:
    ItalicDecorator(Component* comp) : Decorator(comp) {}
    void print() const override {
        std::cout << "<i>";
        Decorator::print();
        std::cout << "</i>" << std::endl;
    }
};