#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

// Flyweight interface
class Character {
public:
    virtual ~Character() {}
    virtual void draw(int x, int y) = 0;
};

// Concrete Flyweight
class CharacterImpl : public Character {
private:
    std::string appearance;

public:
    CharacterImpl(const std::string& appearance) : appearance(appearance) {}

    void draw(int x, int y) override {
        std::cout << "Drawing character at (" << x << "," << y << ") with appearance: " << appearance << std::endl;
    }
};

// Flyweight Factory
class CharacterFactory {
private:
    std::unordered_map<std::string, std::shared_ptr<Character>> pool;

public:
    std::shared_ptr<Character> getCharacter(const std::string& appearance) {
        auto it = pool.find(appearance);
        if (it == pool.end()) {
            std::shared_ptr<Character> newCharacter(new CharacterImpl(appearance));
            pool[appearance] = newCharacter;
            return newCharacter;
        }
        else {
            return it->second;
        }
    }
};

// Flyweight ÏíÔªÄ£Ê½
int test_Flyweight() {
    CharacterFactory factory;

    // Get shared characters
    auto redCharacter = factory.getCharacter("red");
    auto blueCharacter = factory.getCharacter("blue");
    auto greenCharacter = factory.getCharacter("green");

    // Draw characters at different positions
    redCharacter->draw(10, 20);
    blueCharacter->draw(30, 40);
    greenCharacter->draw(50, 60);

    // Even though we request the same character again, it will be reused
    auto anotherRedCharacter = factory.getCharacter("red");
    anotherRedCharacter->draw(70, 80);

    return 0;
}