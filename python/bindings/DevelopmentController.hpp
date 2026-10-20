#pragma once

#include <string>
#include <memory>
#include "../../src/brain/Brain.hpp"
#include "../../src/core/Types/Types.hpp"

namespace nlm {

class DevelopmentController {
public:
    DevelopmentController();
    ~DevelopmentController();
    
    void setDevelopmentalStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage() const;
    std::string getStageName() const;
    void advanceStage();
    
    void enableCriticalPeriods(bool enable);
    void setCriticalPeriodEnding(DevelopmentalStage stage, double age);
    float getCriticalPeriodProgress() const;
    bool isCriticalPeriodActive() const;
    
    void setPlasticityAgeCurve(const std::string& curveType);
    float getPlasticityRate() const;
    void enableAgingSimulation(bool enable);
    void setAgingRate(double rate);
    float getAgingLevel() const;
    double getRemainingLifespan() const;
    
    std::map<std::string, float> getDevelopmentStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm