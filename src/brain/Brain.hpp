#pragma once

#include "Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations for internal helper functions
class Neuron;
class Synapse;

// Brain implementation - extracted from Brain.cpp

// Initialize brain components
bool Brain::initialize();

// Update brain state for one simulation step
void Brain::step(SimulationStep currentStep);
void Brain::step(SimulationStep currentStep, Timestamp currentTime);

// Checkpoint operations
bool Brain::save(const std::string& filepath) const;
bool Brain::load(const std::string& filepath);

// Helper functions for brain operations
void Brain::receiveSensoryInput(const SensoryInput& input);
void Brain::injectCurrent(NeuronId neuron, MembranePotential current);
void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current);

// Statistics and status reporting
void Brain::logStatus() const;

// Memory system access
size_t Brain::getTotalNeuronCount() const;
size_t Brain::getTotalSynapseCount() const;
size_t Brain::getActiveNeuronCount() const;
size_t Brain::getFiringNeuronCount() const;

// Brain system access
SpikeSystem* Brain::getSpikeSystem();
const SpikeSystem* Brain::getSpikeSystem() const;
STDP* Brain::getSTDP();
Hebbian* Brain::getHebbian();
StructuralPlasticity* Brain::getStructuralPlasticity();

// Action and behavior
std::unique_ptr<Action> Brain::produceAction();

// Neuromodulation
void Brain::applyNeuromodulation(const Neuromodulator& signal);

// Plasticity and development
void Brain::updatePlasticity();
void Brain::develop();

// Brain lifecycle
void Brain::reset();

} // namespace nlm
