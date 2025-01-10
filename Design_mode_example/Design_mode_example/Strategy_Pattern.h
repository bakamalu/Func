#pragma once

#include <iostream>
#include <memory>
#include <functional>
// 抽象策略接口
class QuackBehavior {
public:
    virtual void quack() const = 0;
};

// 具体策略实现
class Quack : public QuackBehavior {
public:
    void quack() const override {
        std::cout << "Quack" << std::endl;
    }
};

class MuteQuack : public QuackBehavior {
public:
    void quack() const override {
        std::cout << "<<" << std::endl;
    }
};

class Squeak : public QuackBehavior {
public:
    void quack() const override {
        std::cout << "Squeak" << std::endl;
    }
};

// 抽象策略接口
class FlyBehavior {
public:
    virtual void fly() const = 0;
};

// 具体策略实现
class FlyWithWings : public FlyBehavior {
public:
    void fly() const override {
        std::cout << "I'm flying!!" << std::endl;
    }
};

class FlyNoWay : public FlyBehavior {
public:
    void fly() const override {
        std::cout << "I can't fly" << std::endl;
    }
};

// 定义上下文
class Duck {
private:
    std::unique_ptr<QuackBehavior> m_quackBehavior;
    std::unique_ptr<FlyBehavior> m_flyBehavior;

public:
    Duck(std::unique_ptr<QuackBehavior> quackBehavior, std::unique_ptr<FlyBehavior> flyBehavior)
        : m_quackBehavior(std::move(quackBehavior)), m_flyBehavior(std::move(flyBehavior)) {}

    void performQuack() const {
        m_quackBehavior->quack();
    }

    void performFly() const {
        m_flyBehavior->fly();
    }
};