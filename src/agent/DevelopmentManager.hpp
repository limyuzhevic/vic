#pragma once

#include "../brain/Brain.hpp"
#include <algorithm>

namespace nlm {

class DevelopmentManager {
public:
    DevelopmentManager(std::shared_ptr<Brain> brain);
    
    void update(double timestep, bool developmentEnabled, bool structuralPlasticityEnabled);
    DevelopmentalStage getStage() const;
    float getPlasticityModifier() const { return plasticityModifier_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    // Enable/disable development systems
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
private:
    std::shared_ptr<Brain> brain_;
    double developmentalAge_;
    float plasticityModifier_;
    bool developmentEnabled_;
    bool structuralPlasticityEnabled_;
};

} // namespace nlm
