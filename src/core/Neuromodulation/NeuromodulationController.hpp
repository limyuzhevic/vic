#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "../../src/neuromodulation/Neuromodulator.hpp"
#include "../../src/core/Config/Config.hpp"

namespace nlm {

enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    High
};

class NeuromodulationController {
public:
    NeuromodulationController();
    ~NeuromodulationController();
    
    void enableRewardModulation(bool enable);
    void setRewardFunction(std::function<float(const class Brain&, float)> rewardFunc);
    float getRewardPredictionError() const;
    void setRewardScale(float scale);
    
    void enableCuriosity(bool enable);
    void setCuriosityStrength(float strength);
    void setCuriosityDecay(float decay);
    float getCuriosityLevel() const;
    
    void enableNoveltyDetection(bool enable);
    void setNoveltyThreshold(float threshold);
    void setNoveltyWindow(std::chrono::milliseconds window);
    float getNoveltyLevel() const;
    
    void enableDopamineSystem(bool enable);
    void setDopamineBaseline(float baseline);
    float getDopamineLevel() const;
    
    void setMotivationSystem(std::map<std::string, float> motivation);
    float getMotivationLevel() const;
    
    void setModulationInteractions(std::map<std::string, std::map<std::string, float>> interactions);
    void updateNeuromodulators(double dt);
    
    std::map<std::string, float> getNeuromodulationStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm