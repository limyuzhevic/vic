#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class StatisticsManager {
public:
    StatisticsManager(Brain* brain);
    ~StatisticsManager();
    
    bool initialize();
    void step(SimulationStep currentStep, Timestamp currentTime);
    void reset();
    
    // Statistics operations
    void logStatus() const;
    void updateStepStatistics(SimulationStep currentStep, Timestamp currentTime);
    
private:
    Brain* brain_;
};

} // namespace nlm
