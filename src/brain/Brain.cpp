#include "Brain.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neuron states
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        neuronData.firingState.reserve(getTotalNeuronCount());
        neuronData.refractoryRemaining.reserve(getTotalNeuronCount());
        neuronData.refractoryPeriod.reserve(getTotalNeuronCount());
        neuronData.lastSpikeTime.reserve(getTotalNeuronCount());
        neuronData.neuronType.reserve(getTotalNeuronCount());
        neuronData.regionId.reserve(getTotalNeuronCount());
        neuronData.populationId.reserve(getTotalNeuronCount());
        
        size_t idx = 0;
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    neuronData.membranePotential.push_back(neuron->getState().membranePotential);
                    neuronData.restingPotential.push_back(neuron->getState().restingPotential);
                    neuronData.threshold.push_back(neuron->getState().thresholdPotential);
                    neuronData.resetPotential.push_back(neuron->getState().resetPotential);
                    neuronData.leakConductance.push_back(neuron->getState().leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(neuron->getState().firingState));
                    neuronData.refractoryRemaining.push_back(0); // Simplified - actual refractory state would need more complex tracking
                    neuronData.refractoryPeriod.push_back(0); // Simplified
                    neuronData.lastSpikeTime.push_back(0.0f); // Simplified - actual spike time would need tracking
                    neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                    neuronData.regionId.push_back(region->getId().index());
                    neuronData.populationId.push_back(pop->getId().index());
                    idx++;
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapse states
        SynapseCheckpointData synapseData;
        synapseData.sourceNeuron.reserve(getTotalSynapseCount());
        synapseData.destinationNeuron.reserve(getTotalSynapseCount());
        synapseData.weight.reserve(getTotalSynapseCount());
        synapseData.delay.reserve(getTotalSynapseCount());
        synapseData.synapseType.reserve(getTotalSynapseCount());
        synapseData.plasticityFlags.reserve(getTotalSynapseCount());
        synapseData.eligibilityTrace.reserve(getTotalSynapseCount());
        synapseData.efficacy.reserve(getTotalSynapseCount());
        synapseData.shortTermDepression.reserve(getTotalSynapseCount());
        synapseData.shortTermFacilitation.reserve(getTotalSynapseCount());
        
        idx = 0;
        for (const auto& region : pImpl->regions) {
            for (const auto& synapse : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(synapse->getSourceNeuron());
                synapseData.destinationNeuron.push_back(synapse->getDestinationNeuron());
                synapseData.weight.push_back(synapse->getWeight());
                synapseData.delay.push_back(synapse->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(synapse->getType()));
                synapseData.plasticityFlags.push_back(synapse->getPlasticityFlags());
                synapseData.eligibilityTrace.push_back(0.0f); // Placeholder - would track eligibility traces
                synapseData.efficacy.push_back(1.0f); // Placeholder
                synapseData.shortTermDepression.push_back(0.0f); // Placeholder
                synapseData.shortTermFacilitation.push_back(0.0f); // Placeholder
                idx++;
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Write plasticity state (STDP, Hebbian, Structural)
        auto plasticityData = std::make_unique<std::vector<float>>();
        if (!writer.writeSection(CheckpointSection::Plasticity, plasticityData->data(), plasticityData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write plasticity data to checkpoint");
            return false;
        }
        
        // Write development state
        auto developmentData = std::make_unique<std::vector<float>>();
        developmentData->push_back(static_cast<float>(pImpl->developmentalStage));
        if (!writer.writeSection(CheckpointSection::Development, developmentData->data(), developmentData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write development data to checkpoint");
            return false;
        }
        
        // Write neuromodulation state
        auto neuromodulationData = std::make_unique<std::vector<float>>();
        if (pImpl->dopamine) neuromodulationData->push_back(pImpl->dopamine->getLevel());
        if (pImpl->serotonin) neuromodulationData->push_back(pImpl->serotonin->getLevel());
        if (pImpl->norepinephrine) neuromodulationData->push_back(pImpl->norepinephrine->getLevel());
        if (pImpl->acetylcholine) neuromodulationData->push_back(pImpl->acetylcholine->getLevel());
        if (pImpl->curiosity) neuromodulationData->push_back(pImpl->curiosity->getLevel());
        if (pImpl->novelty) neuromodulationData->push_back(pImpl->novelty->getLevel());
        if (pImpl->predictionError) neuromodulationData->push_back(pImpl->predictionError->getLevel());
        
        if (!writer.writeSection(CheckpointSection::Neuromodulation, neuromodulationData->data(), neuromodulationData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write neuromodulation data to checkpoint");
            return false;
        }
        
        // Write memory state
        auto memoryData = std::make_unique<std::vector<float>>();
        if (pImpl->workingMemory) {
            auto workingMem = pImpl->workingMemory->retrieve();
            memoryData->insert(memoryData->end(), workingMem.begin(), workingMem.end());
        }
        if (pImpl->episodicMemory) {
            size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
            memoryData->push_back(static_cast<float>(episodeCount));
        }
        
        if (!writer.writeSection(CheckpointSection::Memory, memoryData->data(), memoryData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write memory data to checkpoint");
            return false;
        }
        
        // Write spike history (for STDP)
        auto spikeHistoryData = std::make_unique<std::vector<float>>();
        // Would collect recent spike history for each neuron
        if (!writer.writeSection(CheckpointSection::SpikeHistory, spikeHistoryData->data(), spikeHistoryData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write spike history to checkpoint");
            return false;
        }
        
        // Write random state
        auto randomData = std::make_unique<std::vector<float>>();
        if (pImpl->rng) {
            // Would collect random generator state
            randomData->push_back(0.0f); // Placeholder
        }
        
        if (!writer.writeSection(CheckpointSection::RandomState, randomData->data(), randomData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write random state to checkpoint");
            return false;
        }
        
        // Write simulation state
        auto simulationData = std::make_unique<std::vector<float>>();
        simulationData->push_back(static_cast<float>(pImpl->currentStep));
        simulationData->push_back(static_cast<float>(pImpl->currentTime));
        simulationData->push_back(pImpl->timestep);
        
        if (!writer.writeSection(CheckpointSection::SimulationState, simulationData->data(), simulationData->size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
            return false;
        }
        
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain checkpoint saved successfully to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}