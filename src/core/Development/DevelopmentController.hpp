#pragma once

#include <string>
#include <map>
#include <memory>
#include "../../src/core/Config/Config.hpp"
#include "../../src/core/Logger/Logger.hpp"

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
    void setPlasticityRateAtAge(double age, float rate);
    float getPlasticityRate() const;
    
    void setMaturationSchedule(const std::map<std::string, double>& schedule);
    float getMaturationProgress() const;
    double getMaturationalAge() const;
    
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