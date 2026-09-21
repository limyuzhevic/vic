#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>

#include "NeuromodulationConfig.hpp"
#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct NeuromodulationController::Impl {
    NeuromodulationConfig config;
    bool enabled;
    class Brain* brain;
    
    // Registered neuromodulator factories
    std::unordered_map<std::string, std::function<std::unique_ptr<Neuromodulator>(const NeuromodulationConfig&)>> factories;
    
    // Active neuromodulator instances
    std::unordered_map<std::string, std::unique_ptr<Neuromodulator>> neuromodulators;
    
    // Historical values
    float dopamineLevel;
    float noveltyLevel;
    float curiosityLevel;
    float predictionError;
    
    Impl(const NeuromodulationConfig& cfg) : config(cfg), enabled(true), brain(nullptr),
        dopamineLevel(0.0f), noveltyLevel(0.0f), curiosityLevel(0.0f), predictionError(0.0f) {}
};

NeuromodulationController::NeuromodulationController(const NeuromodulationConfig& config)
    : pImpl(std::make_unique<Impl>(config)) {}

NeuromodulationController::~NeuromodulationController() = default;

void NeuromodulationController::registerNeuromodulator(const std::string& name,
    std::function<std::unique_ptr<Neuromodulator>(const NeuromodulationConfig&)> factory) {
    pImpl->factories[name] = std::move(factory);
}

void NeuromodulationController::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Create neuromodulator instances from factories
    for (const auto& [name, factory] : pImpl->factories) {
        auto neuromodulator = factory(pImpl->config);
        addNeuromodulator(std::move(neuromulator));
    }
}

void NeuromodulationController::update(double timestep) {
    if (!pImpl->enabled) return;
    
    // Update all neuromodulators
    for (auto& [name, neuromodulator] : pImpl->neuromodulators) {
        neuromodulator->update(static_cast<TimestepDuration>(timestep));
    }
}

void NeuromodulationController::signalReward(float reward) {
    if (!pImpl->enabled) return;
    
    for (auto& [name, neuromodulator] : pImpl->neuromodulators) {
        // Check if neuromodulator has reward signaling capability
        if (auto dopamine = dynamic_cast<Dopamine*>(neuromulator.get())) {
            dopamine->signalReward(reward);
        }
    }
}

void NeuromodulationController::signalPredictionError(float error) {
    if (!pImpl->enabled) return;
    
    // Store prediction error for other systems
    pImpl->predictionError = error;
    
    for (auto& [name, neuromodulator] : pImpl->neuromodulators) {
        if (auto dopamine = dynamic_cast<Dopamine*>(neuromodulator.get())) {
            dopamine->signalRewardPredictionError(error);
        }
    }
}

float NeuromodulationController::getNeuromodulatorLevel(const std::string& name) const {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        return it->second->getLevel();
    }
    return 0.0f;
}

float NeuromodulationController::getDopamineLevel() const {
    return getNeuromodulatorLevel("DA");
}

float NeuromodulationController::getNoveltyLevel() const {
    // Novelty is maintained separately from Neuromodulator interface
    return pImpl->noveltyLevel;
}

float NeuromodulationController::getCuriosityLevel() const {
    // Curiosity is maintained separately from Neuromodulator interface
    return pImpl->curiosityLevel;
}

float NeuromodulationController::getPredictionError() const {
    return pImpl->predictionError;
}

float NeuromodulationController::getPlasticityFactor() const {
    float totalFactor = 0.0f;
    int count = 0;
    
    for (const auto& [name, neuromodulator] : pImpl->neuromodulators) {
        totalFactor += neuromodulator->getPlasticityFactor();
        count++;
    }
    
    return count > 0 ? totalFactor / count : pImpl->config.plasticityBaseline;
}

void NeuromodulationController::reset() {
    for (auto& [name, neuromodulator] : pImpl->neuromodulators) {
        neuromodulator->setLevel(0.0f);
    }
    pImpl->dopamineLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->curiosityLevel = 0.0f;
    pImpl->predictionError = 0.0f;
}

void NeuromodulationController::enableNeuromodulation(bool enabled) {
    pImpl->enabled = enabled;
}

bool NeuromodulationController::isNeuromodulationEnabled() const {
    return pImpl->enabled;
}

const NeuromodulationConfig& NeuromodulationController::getConfig() const {
    return pImpl->config;
}

void NeuromodulationController::updateConfig(const NeuromodulationConfig& config) {
    pImpl->config = config;
    // Update neuromodulators with new config if they support it
    for (auto& [name, neuromodulator] : pImpl->neuromodulators) {
        if (auto dopamine = dynamic_cast<Dopamine*>(neuromodulator.get())) {
            // Configure dopamine with new config
            dopamine->setLevel(pImpl->config.dopamineBaseline);
        }
    }
}

bool NeuromodulationController::hasNeuromodulator(const std::string& name) const {
    return pImpl->neuromodulators.find(name) != pImpl->neuromodulators.end();
}

std::vector<std::string> NeuromodulationController::getNeuromodulatorNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : pImpl->neuromodulators) {
        names.push_back(name);
    }
    return names;
}

void NeuromodulationController::addNeuromodulator(std::unique_ptr<Neuromodulator> neuromodulator) {
    // Get name from neuromodulator
    std::string name = neuromodulator->getName();
    pImpl->neuromodulators[name] = std::move(neuromulator);
}

} // namespace nlm