#pragma once

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

namespace VisualizationHelpers {

// Update visualization state from Brain class
void updateBrainVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain);

// Update visualization state from SpikeSystem class
void updateSpikeVisualization(VisualizationInterface::Impl::BrainState& state, const SpikeSystem& spikeSystem);

// Update visualization state from memory systems
void updateMemoryVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain);

// Update visualization state from neuromodulation systems
void updateNeuromodulationVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain);

// Update visualization state from development system
void updateDevelopmentVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain);

// Update visualization state from prediction system
void updatePredictionVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain);

// Update visualization state from cognition systems
void updateCognitionVisualization(VisualizationInterface::Impl::BrainState& state, const Brain& brain);

// Calculate average synaptic weight
float calculateAverageSynapticWeight(const Brain& brain);

} // namespace VisualizationHelpers

} // namespace nlm