#include "DevelopmentController.hpp"
#include <iostream>

namespace nlm {

struct DevelopmentController::Impl {
    DevelopmentalStage currentStage = DevelopmentalStage::Initial;
    bool criticalPeriodsEnabled = true;
    double maturationalAge = 0.0;
    bool agingSimulationEnabled = false;
    double agingRate = 0.001;
};

DevelopmentController::DevelopmentController() : pImpl(std::make_unique<Impl>()) {}
DevelopmentController::~DevelopmentController() {}

void DevelopmentController::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->currentStage = stage;
}

DevelopmentalStage DevelopmentController::getDevelopmentalStage() const {
    return pImpl->currentStage;
}

std::string DevelopmentController::getStageName() const {
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentController::advanceStage() {
    if (pImpl->currentStage == DevelopmentalStage::Initial) {
        pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
    } else if (pImpl->currentStage == DevelopmentalStage::CriticalPeriod) {
        pImpl->currentStage = DevelopmentalStage::Maturation;
    } else if (pImpl->currentStage == DevelopmentalStage::Maturation) {
        pImpl->currentStage = DevelopmentalStage::Adult;
    } else if (pImpl->currentStage == DevelopmentalStage::Adult) {
        pImpl->currentStage = DevelopmentalStage::Aging;
    }
}

void DevelopmentController::enableCriticalPeriods(bool enable) {
    pImpl->criticalPeriodsEnabled = enable;
}

void DevelopmentController::setCriticalPeriodEnding(DevelopmentalStage stage, double age) {}
float DevelopmentController::getCriticalPeriodProgress() const {
    return 0.0f;
}
bool DevelopmentController::isCriticalPeriodActive() const {
    return false;
}

void DevelopmentController::setPlasticityAgeCurve(const std::string& curveType) {}
float DevelopmentController::getPlasticityRate() const {
    return 1.0f;
}
void DevelopmentController::enableAgingSimulation(bool enable) {
    pImpl->agingSimulationEnabled = enable;
}

void DevelopmentController::setAgingRate(double rate) {
    pImpl->agingRate = rate;
}

float DevelopmentController::getAgingLevel() const {
    if (pImpl->agingSimulationEnabled) {
        return std::min(1.0f, static_cast<float>(pImpl->maturationalAge * pImpl->agingRate));
    }
    return 0.0f;
}

double DevelopmentController::getRemainingLifespan() const {
    return 100.0 - pImpl->maturationalAge;
}

std::map<std::string, float> DevelopmentController::getDevelopmentStatistics() const {
    std::map<std::string, float> stats;
    stats["current_stage"] = static_cast<float>(pImpl->currentStage);
    stats["critical_period_progress"] = getCriticalPeriodProgress();
    stats["plasticity_rate"] = getPlasticityRate();
    stats["aging_level"] = getAgingLevel();
    stats["maturational_age"] = static_cast<float>(pImpl->maturationalAge);
    return stats;
}

} // namespace nlm