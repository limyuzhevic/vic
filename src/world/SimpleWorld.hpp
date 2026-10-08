#pragma once

namespace nlm {

class SimpleWorld {
public:
    SimpleWorld() {}
    ~SimpleWorld() = default;
    
    void initialize(int width, int height) {}
    void applyAction(const Brain* brain, const MotorCommand& cmd) {}
    float computeReward(const Brain* brain) const { return 0.0f; }
    
    int getVisionWidth() const { return 16; }
    int getVisionHeight() const { return 16; }
};

class Observation {
public:
    Observation() {}
    ~Observation() = default;
};

} // namespace nlm