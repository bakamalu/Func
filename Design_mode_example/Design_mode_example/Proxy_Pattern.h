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

// Proxy 代理模式
int test_Proxy() {
    Image* image = new ProxyImage("test.jpg");

    // 第一次调用display会触发真实图像的加载
    image->display();

    // 第二次调用display将直接使用已加载的图像
    image->display();

    delete image;

    return 0;
}