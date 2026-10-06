#include "NeuromodulatorEnhancedInterface.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

NeuromodulatorEnhancedInterface::NeuromodulatorEnhancedInterface() {
    manager_ = new NeuromodulatorManagerEnhanced();
}

NeuromodulatorEnhancedInterface::~NeuromodulatorEnhancedInterface() {
    delete manager_;
}

void NeuromodulatorEnhancedInterface::initialize(class Brain* brain) {
    manager_->getDopamine()->initialize(brain);
    manager_->getCuriosity()->initialize(brain);
    manager_->getNovelty()->initialize(brain);
    manager_->getPredictionError()->initialize(brain);
}

void NeuromodulatorEnhancedInterface::update(class Brain* brain,
                                             const class Observation& currentObservation,
                                             const class Observation& expectedObservation) {
    manager_->updateEnhanced(brain, currentObservation, expectedObservation);
}

void NeuromodulatorEnhancedInterface::update(float novelty, float predictionError, double dt) {
    float currentPredictionError = manager_->getPredictionError()->getLevel();
    manager_->getCuriosity()->update(novelty, currentPredictionError, static_cast<float>(dt));
}

void NeuromodulatorEnhancedInterface::detectNovelty(const class Observation& observation,
                                                    const class Observation& previousObservation) {
    manager_->getNovelty()->detectNovelty(observation, previousObservation);
}

void NeuromodulatorEnhancedInterface::detectNovelty(const std::vector<float>& currentPattern,
                                                    const std::vector<float>& previousPattern) {
    manager_->getNovelty()->detectNovelty(currentPattern, previousPattern);
}

float NeuromodulatorEnhancedInterface::getDopamineLevel() const {
    return manager_->getDopamine()->getLevel();
}

float NeuromodulatorEnhancedInterface::getCuriosityLevel() const {
    return manager_->getCuriosity()->getLevel();
}

float NeuromodulatorEnhancedInterface::getNoveltyLevel() const {
    return manager_->getNovelty()->getLevel();
}

float NeuromodulatorEnhancedInterface::getPredictionErrorLevel() const {
    return manager_->getPredictionError()->getLevel();
}

const std::vector<float>& NeuromodulatorEnhancedInterface::getNoveltyHistory() const {
    return manager_->getNovelty()->getHistory();
}

const std::vector<float>& NeuromodulatorEnhancedInterface::getPredictionErrorHistory() const {
    return manager_->getPredictionError()->getHistory();
}

void NeuromodulatorEnhancedInterface::reset() {
    manager_->reset();
}

void NeuromodulatorEnhancedInterface::setEnhancedParameter(const std::string& neuromodulatorType,
                                                           const std::string& parameter,
                                                           float value) {
    manager_->setEnhancedParameter(neuromodulatorType, parameter, value);
}

std::string NeuromodulatorEnhancedInterface::getReport() const {
    return manager_->getEnhancedParametersReport();
}

NeuromodulationUpdate NeuromodulatorEnhancedInterface::getCurrentUpdate() const {
    return manager_->getCurrentUpdate();
}

} // namespace nlm