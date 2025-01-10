#pragma once
#include <iostream>
#include <vector>

// 抽象观察者接口
class Observer {
public:
    virtual void update() = 0;
};

// 抽象主题接口
class Subject {
public:
    virtual void registerObserver(Observer* observer) = 0;
    virtual void removeObserver(Observer* observer) = 0;
    virtual void notifyObservers() = 0;
};

// 具体主题
class ConcreteSubject : public Subject {
private:
    std::vector<Observer*> observers;
    int state;

public:
    ConcreteSubject() : state(0) {}

    void registerObserver(Observer* observer) override {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) override {
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end())
            observers.erase(it);
    }

    void notifyObservers() override {
        for (auto& obs : observers)
            obs->update();
    }

    void setState(int newState) {
        state = newState;
        notifyObservers();
    }

    int getState() const {
        return state;
    }
};

// 具体观察者
class ConcreteObserver : public Observer {
private:
    ConcreteSubject* subject;
    int state;

public:
    ConcreteObserver(ConcreteSubject* subj) : subject(subj), state(0) {
        subject->registerObserver(this);
    }

    ~ConcreteObserver() {
        subject->removeObserver(this);
    }

    void update() override {
        state = subject->getState();
        std::cout << "Observer state changed to: " << state << std::endl;
    }
};