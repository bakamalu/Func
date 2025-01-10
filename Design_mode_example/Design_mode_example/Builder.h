#pragma once
#include <iostream>
#include <string>
#include <vector>

// Product (Car)
class Car {
public:
    Car() {}

    void setEngine(const std::string& engine) { this->engine = engine; }
    void setSeats(int seats) { this->seats = seats; }
    void setTripComputer(bool tripComputer) { this->tripComputer = tripComputer; }
    void setGPS(bool gps) { this->gps = gps; }

    void display() const {
        std::cout << "Engine: " << engine << std::endl;
        std::cout << "Seats: " << seats << std::endl;
        std::cout << "Trip Computer: " << (tripComputer ? "Yes" : "No") << std::endl;
        std::cout << "GPS: " << (gps ? "Yes" : "No") << std::endl;
    }

private:
    std::string engine;
    int seats;
    bool tripComputer;
    bool gps;
};

// Builder interface
class ICarBuilder {
public:
    virtual void buildEngine() = 0;
    virtual void buildSeats() = 0;
    virtual void buildTripComputer() = 0;
    virtual void buildGPS() = 0;
    virtual Car* getResult() = 0;
};

// Concrete builders
class EconomyCarBuilder : public ICarBuilder {
public:
    EconomyCarBuilder() { car = new Car(); }

    void buildEngine() override { car->setEngine("Small Engine"); }
    void buildSeats() override { car->setSeats(2); }
    void buildTripComputer() override { car->setTripComputer(false); }
    void buildGPS() override { car->setGPS(false); }

    Car* getResult() override { return car; }

private:
    Car* car;
};

class LuxuryCarBuilder : public ICarBuilder {
public:
    LuxuryCarBuilder() { car = new Car(); }

    void buildEngine() override { car->setEngine("Big Engine"); }
    void buildSeats() override { car->setSeats(4); }
    void buildTripComputer() override { car->setTripComputer(true); }
    void buildGPS() override { car->setGPS(true); }

    Car* getResult() override { return car; }

private:
    Car* car;
};

// Director
class Director {
public:
    void construct(ICarBuilder* builder) {
        builder->buildEngine();
        builder->buildSeats();
        builder->buildTripComputer();
        builder->buildGPS();
    }
};