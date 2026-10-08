#pragma once

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem() {}
    ~DevelopmentSystem() = default;
    
    void initialize() {}
    void update(double timestep) {}
    
    void setSynaptogenesisRate(float rate) {}
    void setPruningRate(float rate) {}
    float getSynaptogenesisRate() const { return 0.0f; }
    float getPruningRate() const { return 0.0f; }
};

} // namespace nlm