#pragma once

#include "../core/Types/Types.hpp"
#include "development/DevelopmentSystem.hpp"

namespace nlm {

class DevelopmentModule {
public:
    DevelopmentModule(std::shared_ptr<DevelopmentSystem> developmentSystem);
    
    DevelopmentSystem* getDevelopmentSystem();
    
    void update(double dt);
    void stageChanged(DevelopmentalStage oldStage, DevelopmentalStage newStage);
    bool isInSensitivePeriod() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
