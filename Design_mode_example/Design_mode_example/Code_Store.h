#pragma once
//
//#include <iostream>
//
//// Ŀ��ӿ�
//class Target {
//public:
//    virtual void request() const = 0;
//};
//
//// ��Ҫ�������
//class Adaptee {
//public:
//    void specificRequest() const {
//        std::cout << "Adaptee's specific request.\n";
//    }
//};
//
//// ������
//class Adapter : public Target {
//private:
//    Adaptee* adaptee;
//public:
//    Adapter(Adaptee* adaptee) : adaptee(adaptee) {}
//    void request() const override {
//        adaptee->specificRequest();
//    }
//};
//
//int main() {
//    Adaptee adaptee;
//    Target* target = new Adapter(&adaptee);
//    target->request();
//    delete target;
//
//    return 0;
//}
//
//#include <iostream>
//#include <vector>
//
//// �������
//class Component {
//public:
//    virtual void add(Component* component) = 0;
//    virtual void remove(Component* component) = 0;
//    virtual void display(int depth) const = 0;
//};
//
//// Ҷ�����
//class Leaf : public Component {
//public:
//    void add(Component*) override {}
//    void remove(Component*) override {}
//    void display(int depth) const override {
//        for (int i = 0; i < depth; i++) std::cout << "-";
//        std::cout << "Leaf\n";
//    }
//};
//
//// �������
//class Composite : public Component {
//private:
//    std::vector<Component*> children;
//public:
//    void add(Component* component) override {
//        children.push_back(component);
//    }
//    void remove(Component* component) override {
//        children.erase(std::remove(children.begin(), children.end(), component), children.end());
//    }
//    void display(int depth) const override {
//        for (int i = 0; i < depth; i++) std::cout << "-";
//        std::cout << "Composite\n";
//        for (auto child : children) {
//            child->display(depth + 2);
//        }
//    }
//};
//
//int main() {
//    Component* composite = new Composite();
//    composite->add(new Leaf());
//    composite->add(new Leaf());
//    composite->display(0);
//    delete composite;
//
//    return 0;
//}