// Design_mode_example.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Strategy_Pattern.h"
#include "Observer_Pattern.h"
#include "Decorator_Pattern.h"
#include "Bridge_Pattern.h"
#include "Factory_Method.h"
#include "Abstract_Factory.h"
#include "Builder.h"
#include "Singleton.h"
#include "FlyWeight_Pattern.h"
#include "Facade_Pattern.h"
#include "Proxy_Pattern.h"

// Strategy 策略模式
int test_Strategy_Pattern() {
    std::function<void(Duck&)> simulate = [](Duck& duck) {
        duck.performQuack();
        duck.performFly();
    };
    auto mallardDuck = Duck(std::make_unique<Quack>(), std::make_unique<FlyWithWings>());
    auto modelDuck = Duck(std::make_unique<Squeak>(), std::make_unique<FlyNoWay>());

    std::cout << "Mallard Duck:" << std::endl;
    simulate(mallardDuck);

    std::cout << "Model Duck:" << std::endl;
    simulate(modelDuck);

    return 0;
}

// Observer 观察者模式
int test_Observer_Pattern() {
    ConcreteSubject subject;
    ConcreteObserver observer1(&subject);
    ConcreteObserver observer2(&subject);

    subject.setState(1); // 观察者1和2都应该被通知
    subject.setState(2); // 观察者1和2都应该被通知

    return 0;
}

// Decorator 装饰模式
int test_Decorator_Pattern() {

    Component* text = new SimpleText("Hello World");
    Component* boldText = new BoldDecorator(text);
    Component* italicText = new ItalicDecorator(text);

    std::cout << "Original Text:" << std::endl;
    text->print();

    std::cout << "Bold Text:" << std::endl;
    boldText->print();

    std::cout << "Italic Text:" << std::endl;
    italicText->print();

    delete text;
    delete boldText;
    delete italicText;

    return 0;
}

// Bridge 桥模式
int test_Bridge_Pattern() {
    DrawAPI* windowsAPI = new WindowsAPI();
    DrawAPI* macOSAPI = new MacOSAPI();

    Shape* shape1 = new RedCircle(windowsAPI);
    shape1->draw();

    Shape* shape2 = new GreenRectangle(macOSAPI);
    shape2->draw();

    delete shape1;
    delete shape2;
    delete windowsAPI;
    delete macOSAPI;

    return 0;
}

// Factory Method 工厂方法
int test_Factory_Method() {
    using namespace Factory_Method;
    ShapeFactory* factory;
    Factory_Method::Shape* shape;

    std::function<void(Factory_Method::Shape*)> clientCode = [](Factory_Method::Shape* shape) {
        shape->draw();
    };

    // Creating a Circle using the CircleFactory
    factory = new CircleFactory();
    shape = factory->createShape();
    clientCode(shape);
    delete shape;
    delete factory;

    // Creating a Square using the SquareFactory
    factory = new SquareFactory();
    shape = factory->createShape();
    clientCode(shape);
    delete shape;
    delete factory;

    return 0;
}

// Abstract Factory 抽象工厂
int test_Abstract_Factory() {
    std::function<void(GUIFactory*)> clientCode = [](GUIFactory* factory) {
        Button* button = factory->createButton();
        TextBox* textBox = factory->createTextBox();

        button->paint();
        textBox->paint();

        delete button;
        delete textBox;
    };

    // 使用Windows风格的控件
    GUIFactory* winFactory = new WinFactory();
    clientCode(winFactory);
    delete winFactory;

    // 使用MacOS风格的控件
    GUIFactory* macFactory = new MacFactory();
    clientCode(macFactory);
    delete macFactory;

    return 0;
}

// Builder 构建器
int test_Builder()
{
    Director director;
    ICarBuilder* economyBuilder = new EconomyCarBuilder();
    ICarBuilder* luxuryBuilder = new LuxuryCarBuilder();

    director.construct(economyBuilder);
    Car* economyCar = economyBuilder->getResult();
    economyCar->display();

    director.construct(luxuryBuilder);
    Car* luxuryCar = luxuryBuilder->getResult();
    luxuryCar->display();

    delete economyBuilder;
    delete luxuryBuilder;
    delete economyCar;
    delete luxuryCar;

    return 0;
}

IMPL_SINGLETON_CLASS(SingletonT_test);
// Singleton 单件模式
int test_Singleton() {
    Singleton& singletonInstance = Singleton::getInstance();
    singletonInstance.printMessage();

    SingletonT_test::instance()->print();
    return 0;
}

void test_Design_mode()
{
    //test_Strategy_Pattern();
    //test_Observer_Pattern();
    //test_Decorator_Pattern();
    //test_Bridge_Pattern();
    //test_Factory_Method();
    //test_Abstract_Factory();
    //test_Builder();
    //test_Singleton();
    //test_Flyweight();
    //test_Facade();
    test_Proxy();
}

int test_memory() {
#include <iostream>
#include <cstdint>
#if defined(WIN32)
    std::cout << "32-bit Windows platform" << std::endl;
#elif defined(_WIN64)
    std::cout << "64-bit Windows platform" << std::endl;
#elif defined(__x86_64__) || defined(__x86_64)
    std::cout << "64-bit AMD64 platform" << std::endl;
#elif defined(_M_X64)
    std::cout << "64-bit Microsoft platform" << std::endl;
#elif defined(_M_IX86) || defined(_X86_)
    std::cout << "32-bit x86 Microsoft platform" << std::endl;
#else
    std::cout << "Platform bitness not detected" << std::endl;
#endif

    std::cout << "Size of pointer: " << sizeof(void*) << " bytes" << std::endl;
    std::cout << "Size of int: " << sizeof(int) << " bytes" << std::endl;
    std::cout << "Size of long: " << sizeof(long) << " bytes" << std::endl;
    std::cout << "Size of long long: " << sizeof(long long) << " bytes" << std::endl;
    std::cout << "Max addressable memory: " << (std::uint64_t(1) << (sizeof(void*) - 1)) << " bytes" << std::endl;
    uintptr_t address = reinterpret_cast<uintptr_t>(new int);

    size_t size = sizeof(address);
    std::cout << "Size of uintptr_t: " << size << " bytes" << std::endl;
    return 0;
}

#include <map>
int main()
{
    test_Design_mode();
    //test_memory();

    return 0;
}