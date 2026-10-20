#pragma once

#include "../../src/core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

class PlasticityController {
public:
    PlasticityController();
    ~PlasticityController();
    
    void enableSTDP(bool enable);
    void enableHebbian(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableRewardModulated(bool enable);
    
    void setSTDPParameters(float learningRate, float timeConstant, float weightScale);
    void setHebbianParameters(float learningRate, float weightScale);
    void setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate, float minSynapseWeight);
    
    void switchPlasticityRule(const std::string& ruleName);
    std::string getCurrentRule() const;
    
    void setSpatialPattern(const std::string& pattern);
    void setTemporalPattern(const std::string& pattern);
    void setInputSpecificPattern(const std::string& pattern);
    
    float getTotalWeightChange() const;
    float getAveragePlasticityStrength() const;
    void resetPlasticityStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm