#include "BrainCheckpoint.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cstring>

namespace nlm {

BrainCheckpoint::BrainCheckpoint()
    : bytesWritten(0), checkpointCount(0)
    , checkpointDir("./checkpoints"), maxCheckpoints(10), compressionLevel(0.5f) {}

BrainCheckpoint::~BrainCheckpoint() = default;

bool BrainCheckpoint::initialize(BrainCore* core) {
    this->core = core;
    
    if (!core) {
        NLM_LOG_ERROR("Cannot initialize checkpoint manager without core brain");
        return false;
    }
    
    // Initialize checkpoint manager
    checkpointManager = std::make_unique<CheckpointManager>();
    checkpointManager->configure(checkpointDir, maxCheckpoints, maxCheckpoints, true);
    
    NLM_LOG_INFO("Checkpoint manager initialized: " + checkpointDir + 
                 ", max checkpoints: " + std::to_string(maxCheckpoints));
    
    return true;
}

bool BrainCheckpoint::save(const std::string& filepath, const std::string& format) const {
    if (!core || !checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not properly initialized");
        return false;
    }
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            core->getTotalNeuronCount(),
            core->getTotalSynapseCount(),
            0,  // Region count - would need core to provide this
            core->getSpikesThisStep(),
            core->currentTime()
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(core->getTotalNeuronCount());
        neuronData.restingPotential.reserve(core->getTotalNeuronCount());
        neuronData.threshold.reserve(core->getTotalNeuronCount());
        neuronData.resetPotential.reserve(core->getTotalNeuronCount());
        neuronData.leakConductance.reserve(core->getTotalNeuronCount());
        
        for (auto* neuron : core->getSensoryNeurons()) {
            const auto& state = neuron->getState();
            neuronData.membranePotential.push_back(state.membranePotential);
            neuronData.restingPotential.push_back(state.restingPotential);
            neuronData.threshold.push_back(state.threshold);
            neuronData.resetPotential.push_back(state.resetPotential);
            neuronData.leakConductance.push_back(state.leakConductance);
            neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
            neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
            neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
            neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
            neuronData.firingRate.push_back(state.firingRate);
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses (simplified)
        SynapseCheckpointData synapseData;
        for (auto& region : core->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Write pending delayed spikes (simplified)
        if (core->getSpikeSystem()) {
            auto pending = core->getSpikeSystem()->getPendingDelayedSpikes();
            if (!writer.writeDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
                return false;
            }
        }
        
        // Write Phase 5 extra sections
        savePhase5Data(filepath);
        saveMemoryData(filepath);
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        bytesWritten = writer.getBytesWritten();
        checkpointCount++;
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(bytesWritten) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::load(const std::string& filepath, const std::string& format) {
    if (!core || !checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not properly initialized");
        return false;
    }
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto* neuron : core->getSensoryNeurons()) {
            if (idx < neuronData.membranePotential.size()) {
                neuron->setMembranePotential(neuronData.membranePotential[idx]);
                neuron->setRestingPotential(neuronData.restingPotential[idx]);
                neuron->setThreshold(neuronData.threshold[idx]);
                neuron->setResetPotential(neuronData.resetPotential[idx]);
                neuron->setLeakConductance(neuronData.leakConductance[idx]);
                
                auto& state = neuron->getState();
                if (idx < neuronData.firingState.size()) {
                    state.firingState = static_cast<FiringState>(neuronData.firingState[idx]);
                }
                if (idx < neuronData.refractoryRemaining.size()) {
                    state.refractoryRemaining = neuronData.refractoryRemaining[idx];
                }
                if (idx < neuronData.refractoryPeriod.size()) {
                    state.refractoryPeriod = neuronData.refractoryPeriod[idx];
                }
                if (idx < neuronData.lastSpikeTime.size()) {
                    state.lastSpikeTime = neuronData.lastSpikeTime[idx];
                }
                if (idx < neuronData.firingRate.size()) {
                    state.firingRate = neuronData.firingRate[idx];
                }
            }
            idx++;
        }
        
        // Read synapses (simplified)
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Read pending delayed spikes
        std::vector<DelayedSpikeEvent> pending;
        if (!reader.readDelayedSpikes(pending)) {
            NLM_LOG_ERROR("Failed to read pending spikes from checkpoint");
            return false;
        }
        if (core->getSpikeSystem()) {
            core->getSpikeSystem()->restorePendingDelayedSpikes(pending);
        }
        
        // Update simulation clock
        core->currentStep = reader.getHeader().step;
        core->currentTime = reader.getHeader().simulationTime;
        
        // Read Phase 5 extra sections
        loadPhase5Data(filepath);
        loadMemoryData(filepath);
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainCheckpoint::update(SimulationStep currentStep, Timestamp currentTime) {
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

void BrainCheckpoint::p5AppendU64(std::vector<uint8_t>& out, uint64_t v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
}

void BrainCheckpoint::p5AppendFloat(std::vector<uint8_t>& out, float v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
}

void BrainCheckpoint::p5AppendDouble(std::vector<uint8_t>& out, double v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
}

template <typename T>
void BrainCheckpoint::p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v) {
    p5AppendU64(out, static_cast<uint64_t>(v.size()));
    if (!v.empty()) {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(v.data());
        out.insert(out.end(), b, b + v.size() * sizeof(T));
    }
}

bool BrainCheckpoint::P5Reader::u64(uint64_t& v) {
    if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
    std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
}

bool BrainCheckpoint::P5Reader::f32(float& v) {
    if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
    std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
}

bool BrainCheckpoint::P5Reader::f64(double& v) {
    if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
    std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
}

template <typename T>
bool BrainCheckpoint::P5Reader::vec(std::vector<T>& v) {
    uint64_t n = 0;
    if (!u64(n)) return false;
    if (n > 10000000) { ok = false; return false; }
    if (end - p < static_cast<std::ptrdiff_t>(n * sizeof(T))) { ok = false; return false; }
    v.resize(static_cast<size_t>(n));
    if (n > 0) {
        std::memcpy(v.data(), p, static_cast<size_t>(n) * sizeof(T));
        p += static_cast<std::ptrdiff_t>(n * sizeof(T));
    }
    return true;
}

bool BrainCheckpoint::savePhase5Data(const std::string& filepath) const {
    // Placeholder implementation
    return true;
}

bool BrainCheckpoint::saveMemoryData(const std::string& filepath) const {
    // Placeholder implementation
    return true;
}

bool BrainCheckpoint::loadPhase5Data(const std::string& filepath) {
    // Placeholder implementation
    return true;
}

bool BrainCheckpoint::loadMemoryData(const std::string& filepath) {
    // Placeholder implementation
    return true;
}

} // namespace nlm
