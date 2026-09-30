#pragma once

#include "Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include "Exceptions/NLMExceptions.hpp"
#include <cmath>
#include <stdexcept>

namespace nlm {

// Input validation constants
static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
static constexpr float MAX_MEMBRANE_POTENTIAL = 100.0f;
static constexpr float MIN_THRESHOLD = -100.0f;
static constexpr float MAX_THRESHOLD = 100.0f;
static constexpr float MIN_RESTING_POTENTIAL = -100.0f;
static constexpr float MAX_RESTING_POTENTIAL = 100.0f;
static constexpr float MIN_RESET_POTENTIAL = -100.0f;
static constexpr float MAX_RESET_POTENTIAL = 100.0f;
static constexpr float MIN_LEAK_CONDUCTANCE = 0.0f;
static constexpr float MAX_LEAK_CONDUCTANCE = 100.0f;
static constexpr uint32_t MIN_REFACTORY_PERIOD = 0;
static constexpr uint32_t MAX_REFACTORY_PERIOD = 10000;
static constexpr float MIN_SYNAPTIC_WEIGHT = -1.0f;
static constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;
static constexpr float MIN_SYNAPTIC_DELAY = 1;
static constexpr float MAX_SYNAPTIC_DELAY = 1000;

// Parameter validation helper functions
bool isValidMembranePotential(float potential) {
    return potential >= MIN_MEMBRANE_POTENTIAL && potential <= MAX_MEMBRANE_POTENTIAL;
}

bool isValidThreshold(float threshold) {
    return threshold >= MIN_THRESHOLD && threshold <= MAX_THRESHOLD;
}

bool isValidRestingPotential(float potential) {
    return potential >= MIN_RESTING_POTENTIAL && potential <= MAX_RESTING_POTENTIAL;
}

bool isValidResetPotential(float potential) {
    return potential >= MIN_RESET_POTENTIAL && potential <= MAX_RESET_POTENTIAL;
}

bool isValidLeakConductance(float conductance) {
    return conductance >= MIN_LEAK_CONDUCTANCE && conductance <= MAX_LEAK_CONDUCTANCE;
}

bool isValidRefractoryPeriod(uint32_t period) {
    return period >= MIN_REFACTORY_PERIOD && period <= MAX_REFACTORY_PERIOD;
}

bool isValidSynapticWeight(float weight) {
    return weight >= MIN_SYNAPTIC_WEIGHT && weight <= MAX_SYNAPTIC_WEIGHT;
}

bool isValidSynapseDelay(float delay) {
    return delay >= MIN_SYNAPTIC_DELAY && delay <= MAX_SYNAPTIC_DELAY;
}

float clampMembranePotential(float potential) {
    return std::max(std::min(potential, MAX_MEMBRANE_POTENTIAL), MIN_MEMBRANE_POTENTIAL);
}

float clampThreshold(float threshold) {
    return std::max(std::min(threshold, MAX_THRESHOLD), MIN_THRESHOLD);
}

// Validation and clamping functions for Neuron state
MembranePotential validateAndClampMembranePotential(MembranePotential potential) {
    if (!isValidMembranePotential(potential)) {
        NLM_THROW_SIMULATION_ERROR(
            "Invalid membrane potential: " + std::to_string(potential) + ". "
            "Valid range: [" + std::to_string(MIN_MEMBRANE_POTENTIAL) + ", " +
            std::to_string(MAX_MEMBRANE_POTENTIAL) + "]",
            "neuron"
        );
    }
    return clampMembranePotential(potential);
}

MembranePotential validateAndClampThreshold(MembranePotential threshold) {
    if (!isValidThreshold(threshold)) {
        NLM_THROW_SIMULATION_ERROR(
            "Invalid threshold: " + std::to_string(threshold) + ". "
            "Valid range: [" + std::to_string(MIN_THRESHOLD) + ", " +
            std::to_string(MAX_THRESHOLD) + "]",
            "neuron"
        );
    }
    return clampThreshold(threshold);
}

MembranePotential validateAndClampRestingPotential(MembranePotential potential) {
    if (!isValidRestingPotential(potential)) {
        NLM_THROW_SIMULATION_ERROR(
            "Invalid resting potential: " + std::to_string(potential) + ". "
            "Valid range: [" + std::to_string(MIN_RESTING_POTENTIAL) + ", " +
            std::to_string(MAX_RESTING_POTENTIAL) + "]",
            "neuron"
        );
    }
    return potential;
}

MembranePotential validateAndClampResetPotential(MembranePotential potential) {
    if (!isValidResetPotential(potential)) {
        NLM_THROW_SIMULATION_ERROR(
            "Invalid reset potential: " + std::to_string(potential) + ". "
            "Valid range: [" + std::to_string(MIN_RESET_POTENTIAL) + ", " +
            std::to_string(MAX_RESET_POTENTIAL) + "]",
            "neuron"
        );
    }
    return potential;
}

MembranePotential validateAndClampLeakConductance(MembranePotential conductance) {
    if (!isValidLeakConductance(conductance)) {
        NLM_THROW_SIMULATION_ERROR(
            "Invalid leak conductance: " + std::to_string(conductance) + ". "
            "Valid range: [" + std::to_string(MIN_LEAK_CONDUCTANCE) + ", " +
            std::to_string(MAX_LEAK_CONDUCTANCE) + "]",
            "neuron"
        );
    }
    return conductance;
}

uint32_t validateAndClampRefractoryPeriod(uint32_t period) {
    if (!isValidRefractoryPeriod(period)) {
        NLM_THROW_SIMULATION_ERROR(
            "Invalid refractory period: " + std::to_string(period) + ". "
            "Valid range: [" + std::to_string(MIN_REFACTORY_PERIOD) + ", " +
            std::to_string(MAX_REFACTORY_PERIOD) + "]",
            "neuron"
        );
    }
    return period;
}

// Enhanced Neuron setter methods with validation
void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = validateAndClampMembranePotential(potential);
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    pImpl->state.membranePotential = validateAndClampMembranePotential(newPotential);
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = validateAndClampThreshold(threshold);
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = validateAndClampRestingPotential(potential);
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = validateAndClampResetPotential(potential);
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = validateAndClampLeakConductance(conductance);
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = validateAndClampRefractoryPeriod(steps);
}

// Neuron methods with validation
void Neuron::setState(const NeuronState& state) {
    pImpl->state.membranePotential = validateAndClampMembranePotential(state.membranePotential);
    pImpl->state.threshold = validateAndClampThreshold(state.threshold);
    pImpl->state.restingPotential = validateAndClampRestingPotential(state.restingPotential);
    pImpl->state.resetPotential = validateAndClampResetPotential(state.resetPotential);
    pImpl->state.leakConductance = validateAndClampLeakConductance(state.leakConductance);
    pImpl->state.refractoryPeriod = validateAndClampRefractoryPeriod(state.refractoryPeriod);
}

} // namespace nlm
