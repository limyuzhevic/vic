// Brain Checkpoint Implementation
// Refactored checkpoint persistence for Brain.cpp

#include "BrainCheckpoint.hpp"
#include "../brain/Brain.hpp"
#include "../brain/BrainHelpers.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Region.hpp"
#include "../brain/Population.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>

namespace nlm {

// Phase 5 checkpoint payload helpers for the extra sections. Format is
// deliberately simple and self-describing per value: a uint64 count
// followed by raw elements, in a fixed order per section. All readers
// bounds-check and fail the section (not the whole load) on truncation,
// so a corrupt extra section degrades to the Phase 4 behaviour instead
// of aborting a load that would otherwise succeed.
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

struct BrainCheckpoint::P5Reader {
    const uint8_t* p;
    const uint8_t* end;
    bool ok = true;
    bool u64(uint64_t& v) {
        if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
        std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
    }
    bool f32(float& v) {
        if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
        std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
    }
    bool f64(double& v) {
        if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
        std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
    }
    template <typename T>
    bool vec(std::vector<T>& v) {
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
};

void BrainCheckpoint::prepareNeuronCheckpointData(const Brain::Impl& pImpl, NeuronCheckpointData& neuronData) {
    neuronData.membranePotential.reserve(pImpl->regions.size() ? 
                                         pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.restingPotential.reserve(pImpl->regions.size() ? 
                                        pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.threshold.reserve(pImpl->regions.size() ? 
                                 pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.resetPotential.reserve(pImpl->regions.size() ? 
                                     pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.leakConductance.reserve(pImpl->regions.size() ? 
                                      pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.firingState.reserve(pImpl->regions.size() ? 
                                   pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.refractoryRemaining.reserve(pImpl->regions.size() ? 
                                          pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.refractoryPeriod.reserve(pImpl->regions.size() ? 
                                       pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.lastSpikeTime.reserve(pImpl->regions.size() ? 
                                    pImpl->regions[0]->getTotalNeuronCount() : 0);
    neuronData.firingRate.reserve(pImpl->regions.size() ? 
                                 pImpl->regions[0]->getTotalNeuronCount() : 0);
    
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
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
        }
    }
}

void BrainCheckpoint::prepareSynapseCheckpointData(const Brain::Impl& pImpl, SynapseCheckpointData& synapseData) {
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
            synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
            synapseData.weight.push_back(syn->getWeight());
            synapseData.delay.push_back(syn->getDelay());
            synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
            synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            synapseData.efficacy.push_back(syn->getEfficacy());
        }
    }
}

void BrainCheckpoint::prepareSimulationStateData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload) {
    p5AppendU64(payload, static_cast<uint64_t>(pImpl->totalSpikesTotal));
    p5AppendU64(payload, static_cast<uint64_t>(pImpl->totalSpikesThisStep));
    p5AppendFloat(payload, pImpl->learnedPreference);
    p5AppendU64(payload, static_cast<uint64_t>(pImpl->stepsSinceLastEpisode));
}

void BrainCheckpoint::prepareRandomStateData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload) {
    if (pImpl->rng) {
        std::string state = pImpl->rng->serializeState();
        std::vector<uint8_t> stateData(state.begin(), state.end());
        payload = std::move(stateData);
    }
}

void BrainCheckpoint::preparePlasticityData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload) {
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            p5AppendU64(payload, syn->getSourceNeuron().index());
            p5AppendU64(payload, syn->getDestinationNeuron().index());
            p5AppendVec<double>(payload, syn->getPreSpikeHistory());
            p5AppendVec<double>(payload, syn->getPostSpikeHistory());
        }
    }
}

void BrainCheckpoint::prepareMemoryData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload) {
    if (pImpl->workingMemory) {
        auto neurons = pImpl->workingMemory->getMemoryNeurons();
        std::vector<uint64_t> ids;
        ids.reserve(neurons.size());
        for (const auto& n : neurons) ids.push_back(n.index());
        p5AppendVec<uint64_t>(payload, ids);
        p5AppendVec<float>(payload, pImpl->workingMemory->getMemoryActivations());
        p5AppendVec<uint64_t>(payload, pImpl->workingMemory->getMemoryTimestamps());
    } else {
        p5AppendVec<uint64_t>(payload, {});
        p5AppendVec<float>(payload, {});
        p5AppendVec<uint64_t>(payload, {});
    }
    
    if (pImpl->episodicMemory) {
        const auto& eps = pImpl->episodicMemory->getEpisodes();
        p5AppendU64(payload, static_cast<uint64_t>(eps.size()));
        for (const auto& ep : eps) {
            p5AppendU64(payload, ep.timestamp);
            p5AppendFloat(payload, ep.positionX);
            p5AppendFloat(payload, ep.positionY);
            p5AppendFloat(payload, ep.orientation);
            p5AppendU64(payload, static_cast<uint64_t>(static_cast<int32_t>(ep.action)));
            p5AppendFloat(payload, ep.reward);
            p5AppendFloat(payload, ep.energy);
            p5AppendFloat(payload, ep.novelty);
            p5AppendFloat(payload, ep.resultingReward);
            p5AppendU64(payload, ep.age);
            p5AppendVec<float>(payload, ep.sensoryState);
            p5AppendVec<float>(payload, ep.resultingSensoryState);
            std::vector<uint64_t> activeIds;
            activeIds.reserve(ep.activeNeurons.size());
            for (const auto& n : ep.activeNeurons) activeIds.push_back(n.index());
            p5AppendVec<uint64_t>(payload, activeIds);
            p5AppendVec<float>(payload, ep.neuronActivations);
        }
    } else {
        p5AppendU64(payload, 0);
    }
}

bool BrainCheckpoint::saveBrain(Brain::Impl& pImpl, const std::string& filepath) {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // Create checkpoint writer (implementation would use actual checkpoint system)
        if (!pImpl->checkpointManager->create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        pImpl->checkpointManager->setMetadata(
            pImpl->regions.size() ? pImpl->regions[0]->getTotalNeuronCount() : 0,
            pImpl->regions.size() ? pImpl->regions[0]->getSynapseCount() : 0,
            pImpl->regions.size(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Prepare and write neurons
        NeuronCheckpointData neuronData;
        prepareNeuronCheckpointData(pImpl, neuronData);
        if (!pImpl->checkpointManager->writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Prepare and write synapses
        SynapseCheckpointData synapseData;
        prepareSynapseCheckpointData(pImpl, synapseData);
        if (!pImpl->checkpointManager->writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Write pending delayed spikes
        if (pImpl->spikeSystem) {
            auto pending = pImpl->spikeSystem->getPendingDelayedSpikes();
            if (!pImpl->checkpointManager->writeDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
                return false;
            }
        }
        
        // Write extra sections
        {
            std::vector<uint8_t> payload;
            prepareSimulationStateData(pImpl, payload);
            pImpl->checkpointManager->bufferSection(CheckpointSection::SimulationState, payload);
        }
        
        {
            std::vector<uint8_t> payload;
            prepareRandomStateData(pImpl, payload);
            pImpl->checkpointManager->bufferSection(CheckpointSection::RandomState, payload);
        }
        
        {
            std::vector<uint8_t> payload;
            preparePlasticityData(pImpl, payload);
            pImpl->checkpointManager->bufferSection(CheckpointSection::Plasticity, payload);
        }
        
        {
            std::vector<uint8_t> payload;
            prepareMemoryData(pImpl, payload);
            pImpl->checkpointManager->bufferSection(CheckpointSection::Memory, payload);
        }
        
        // Finalize
        if (!pImpl->checkpointManager->finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(pImpl->checkpointManager->getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::loadBrain(Brain::Impl& pImpl, const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        if (!pImpl->checkpointManager->open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!pImpl->checkpointManager->validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + pImpl->checkpointManager->getError());
            return false;
        }
        
        // Validate neuron count
        if (pImpl->checkpointManager->getHeader().neuronCount != 
            (pImpl->regions.size() ? pImpl->regions[0]->getTotalNeuronCount() : 0)) {
            NLM_LOG_ERROR("Checkpoint neuron mismatch");
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!pImpl->checkpointManager->readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        applyRestoredNeuronStates(pImpl, neuronData);
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!pImpl->checkpointManager->readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states
        applyRestoredSynapseStates(pImpl, synapseData);
        
        // Restore pending delayed spikes
        {
            std::vector<DelayedSpikeEvent> pending;
            if (!pImpl->checkpointManager->readDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to read pending spikes from checkpoint");
                return false;
            }
            if (pImpl->spikeSystem) {
                pImpl->spikeSystem->restorePendingDelayedSpikes(pending);
            }
            NLM_LOG_INFO("Restored " + std::to_string(pending.size()) + " pending delayed spikes");
        }
        
        // Restore extra sections
        restoreExtraSections(pImpl);
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainCheckpoint::applyRestoredNeuronStates(Brain::Impl& pImpl, const NeuronCheckpointData& neuronData) {
    size_t idx = 0;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
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
        }
    }
}

void BrainCheckpoint::applyRestoredSynapseStates(Brain::Impl& pImpl, const SynapseCheckpointData& synapseData) {
    // Match synapses by (source, destination) pair
    std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
    auto pairKey = [](uint64_t src, uint64_t dst) {
        return (src << 32) ^ dst;
    };
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            byPair[pairKey(syn->getSourceNeuron().index(),
                         syn->getDestinationNeuron().index())].push_back(syn.get());
        }
    }
    
    std::unordered_map<uint64_t, size_t> usedCount;
    size_t restored = 0;
    for (size_t i = 0; i < synapseData.weight.size(); ++i) {
        uint64_t key = pairKey(synapseData.sourceNeuron[i],
                             synapseData.destinationNeuron[i]);
        auto it = byPair.find(key);
        Synapse* target = nullptr;
        if (it != byPair.end()) {
            size_t used = usedCount[key]++;
            if (used < it->second.size()) target = it->second[used];
        }
        if (!target) continue;
        target->setWeight(synapseData.weight[i]);
        if (i < synapseData.delay.size()) target->setDelay(synapseData.delay[i]);
        if (i < synapseData.synapseType.size()) {
            target->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
        }
        if (i < synapseData.eligibilityTrace.size()) {
            target->setEligibilityTrace(synapseData.eligibilityTrace[i]);
        }
        if (i < synapseData.efficacy.size()) {
            target->setEfficacy(synapseData.efficacy[i]);
        }
        ++restored;
    }
    
    NLM_LOG_INFO("Restored " + std::to_string(restored) + " / " +
                 std::to_string(synapseData.weight.size()) + " synapses");
}

void BrainCheckpoint::restoreExtraSections(Brain::Impl& pImpl) {
    // Restore Phase 5 extra sections
    {
        auto payload = pImpl->checkpointManager->readSection(CheckpointSection::SimulationState);
        if (!payload.empty()) {
            P5Reader r{payload.data(), payload.data() + payload.size()};
            uint64_t total = 0, thisStep = 0, sinceEp = 0;
            float pref = 0.0f;
            if (r.u64(total) && r.u64(thisStep) && r.f32(pref) && r.u64(sinceEp) && r.ok) {
                pImpl->totalSpikesTotal = static_cast<size_t>(total);
                pImpl->totalSpikesThisStep = static_cast<size_t>(thisStep);
                pImpl->learnedPreference = std::clamp(pref, -1.0f, 1.0f);
                pImpl->stepsSinceLastEpisode = static_cast<size_t>(sinceEp);
            }
        }
    }
    
    {
        auto payload = pImpl->checkpointManager->readSection(CheckpointSection::RandomState);
        if (!payload.empty() && pImpl->rng) {
            std::string state(payload.begin(), payload.end());
            if (!pImpl->rng->restoreState(state)) {
                // Log warning
            }
        }
    }
    
    {
        // Restore plasticity data
        auto payload = pImpl->checkpointManager->readSection(CheckpointSection::Plasticity);
        if (!payload.empty()) {
            P5Reader r{payload.data(), payload.data() + payload.size()};
            std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
            auto pairKey = [](uint64_t src, uint64_t dst) { return (src << 32) ^ dst; };
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    byPair[pairKey(syn->getSourceNeuron().index(),
                                 syn->getDestinationNeuron().index())].push_back(syn.get());
                }
            }
            std::unordered_map<uint64_t, size_t> usedCount;
            size_t restored = 0;
            while (r.p < r.end && r.ok) {
                uint64_t src = 0, dst = 0;
                std::vector<double> pre, post;
                if (!r.u64(src) || !r.u64(dst) || !r.vec(pre) || !r.vec(post)) break;
                uint64_t key = pairKey(src, dst);
                auto it = byPair.find(key);
                if (it == byPair.end()) continue;
                size_t used = usedCount[key]++;
                if (used >= it->second.size()) continue;
                it->second[used]->setPreSpikeHistory(pre);
                it->second[used]->setPostSpikeHistory(post);
                ++restored;
            }
        }
    }
    
    {
        // Restore memory data
        auto payload = pImpl->checkpointManager->readSection(CheckpointSection::Memory);
        if (!payload.empty()) {
            P5Reader r{payload.data(), payload.data() + payload.size()};
            std::vector<uint64_t> wmIds;
            std::vector<float> wmActs;
            std::vector<uint64_t> wmTimes;
            if (r.vec(wmIds) && r.vec(wmActs) && r.vec(wmTimes) && r.ok &&
                pImpl->workingMemory) {
                std::vector<NeuronId> neurons;
                neurons.reserve(wmIds.size());
                for (uint64_t id : wmIds) neurons.emplace_back(id);
                pImpl->workingMemory->restoreSnapshot(neurons, wmActs, wmTimes);
            }
            uint64_t epCount = 0;
            if (r.u64(epCount) && r.ok && pImpl->episodicMemory) {
                std::vector<EpisodicMemoryItem> eps;
                eps.reserve(static_cast<size_t>(epCount));
                for (uint64_t i = 0; i < epCount && r.ok; ++i) {
                    EpisodicMemoryItem ep;
                    uint64_t ts = 0, actionRaw = 0, age = 0;
                    std::vector<uint64_t> activeIds;
                    if (!r.u64(ts) || !r.f32(ep.positionX) || !r.f32(ep.positionY) ||
                        !r.f32(ep.orientation) || !r.u64(actionRaw) ||
                        !r.f32(ep.reward) || !r.f32(ep.energy) || !r.f32(ep.novelty) ||
                        !r.f32(ep.resultingReward) || !r.u64(age) ||
                        !r.vec(ep.sensoryState) || !r.vec(ep.resultingSensoryState) ||
                        !r.vec(activeIds) || !r.vec(ep.neuronActivations)) {
                        break;
                    }
                    ep.timestamp = ts;
                    ep.action = static_cast<ActionType>(static_cast<int32_t>(actionRaw));
                    ep.age = age;
                    ep.activeNeurons.reserve(activeIds.size());
                    for (uint64_t id : activeIds) ep.activeNeurons.emplace_back(id);
                    eps.push_back(std::move(ep));
                }
                if (r.ok) {
                    pImpl->episodicMemory->restoreEpisodes(eps);
                }
            }
        }
    }
}

} // namespace nlm
