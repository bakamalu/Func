#pragma once
#include <iostream>

// Subsystem class 1
class CDPlayer {
public:
    void on() {
        std::cout << "CD Player is on." << std::endl;
    }

    void off() {
        std::cout << "CD Player is off." << std::endl;
    }

    void play() {
        std::cout << "Playing CD." << std::endl;
    }

    void stop() {
        std::cout << "Stopping CD." << std::endl;
    }
};

// Subsystem class 2
class Amplifier {
public:
    void on() {
        std::cout << "Amplifier is on." << std::endl;
    }

    void off() {
        std::cout << "Amplifier is off." << std::endl;
    }

    void setVolume(int volume) {
        std::cout << "Setting volume to " << volume << "." << std::endl;
    }
};

// Subsystem class 3
class Speaker {
public:
    void on() {
        std::cout << "Speaker is on." << std::endl;
    }

    void off() {
        std::cout << "Speaker is off." << std::endl;
    }
};

// Facade class
class StereoSystem {
private:
    CDPlayer cdPlayer;
    Amplifier amp;
    Speaker speaker;

public:
    void playCD() {
        cdPlayer.on();
        cdPlayer.play();
        amp.on();
        amp.setVolume(10);
        speaker.on();
    }

    void stopCD() {
        cdPlayer.stop();
        cdPlayer.off();
        amp.off();
        speaker.off();
    }
};

// Facade 门面模式
int test_Facade() {
    StereoSystem stereo;
    stereo.playCD();
    stereo.stopCD();

    return 0;
}