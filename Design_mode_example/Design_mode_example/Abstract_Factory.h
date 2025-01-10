#pragma once

#include <iostream>

// �����Ʒ�ӿ�
class Button {
public:
    virtual void paint() = 0;
    virtual ~Button() {}
};

class TextBox {
public:
    virtual void paint() = 0;
    virtual ~TextBox() {}
};

// �����Ʒʵ��
class WinButton : public Button {
public:
    void paint() override {
        std::cout << "Painting Windows button.\n";
    }
};

class WinTextBox : public TextBox {
public:
    void paint() override {
        std::cout << "Painting Windows text box.\n";
    }
};

class MacButton : public Button {
public:
    void paint() override {
        std::cout << "Painting MacOS button.\n";
    }
};

class MacTextBox : public TextBox {
public:
    void paint() override {
        std::cout << "Painting MacOS text box.\n";
    }
};

// ���󹤳��ӿ�
class GUIFactory {
public:
    virtual Button* createButton() = 0;
    virtual TextBox* createTextBox() = 0;
    virtual ~GUIFactory() {}
};

// ���幤��ʵ��
class WinFactory : public GUIFactory {
public:
    Button* createButton() override {
        return new WinButton();
    }

    TextBox* createTextBox() override {
        return new WinTextBox();
    }
};

class MacFactory : public GUIFactory {
public:
    Button* createButton() override {
        return new MacButton();
    }

    TextBox* createTextBox() override {
        return new MacTextBox();
    }
};