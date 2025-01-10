#pragma once

#include <iostream>
#include <string>

// Subject interface
class Image {
public:
    virtual void display() = 0;
    virtual ~Image() {}
};

// RealSubject
class RealImage : public Image {
private:
    std::string fileName;

public:
    RealImage(const std::string& fileName) : fileName(fileName) {
        loadFromDisk();
    }

    void display() override {
        std::cout << "Displaying image: " << fileName << std::endl;
    }

private:
    void loadFromDisk() {
        std::cout << "Loading image from disk: " << fileName << std::endl;
    }
};

// Proxy
class ProxyImage : public Image {
private:
    std::string fileName;
    RealImage* realImage;

public:
    ProxyImage(const std::string& fileName) : fileName(fileName), realImage(nullptr) {}

    void display() override {
        if (!realImage) {
            realImage = new RealImage(fileName);
        }
        realImage->display();
    }

    ~ProxyImage() {
        delete realImage;
    }
};

// Proxy ����ģʽ
int test_Proxy() {
    Image* image = new ProxyImage("test.jpg");

    // ��һ�ε���display�ᴥ����ʵͼ��ļ���
    image->display();

    // �ڶ��ε���display��ֱ��ʹ���Ѽ��ص�ͼ��
    image->display();

    delete image;

    return 0;
}