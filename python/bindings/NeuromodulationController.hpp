#pragma once

#include "../../src/core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

class NeuromodulationController {
public:
    NeuromodulationController();
    ~NeuromodulationController();
    
    void enableRewardModulation(bool enable);
    void enableCuriosity(bool enable);
    void enableNoveltyDetection(bool enable);
    void enableDopamineSystem(bool enable);
    
    void setCuriosityStrength(float strength);
    void setNoveltyThreshold(float threshold);
    void setRewardScale(float scale);
    
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getDopamineLevel() const;
    float getRewardPredictionError() const;
    
    void updateNeuromodulators(double dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm