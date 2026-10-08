#pragma once

namespace nlm {

class OptimizedBrain {
public:
    OptimizedBrain(std::shared_ptr<Config> config) {}
    ~OptimizedBrain() = default;
    
    bool initialize() { return true; }
    void step(SimulationStep step) {}
    void step(SimulationStep step, Timestamp currentTime) {}
};

} // namespace nlm