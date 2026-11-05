// BrainCheckpoint.cpp - Implementation of BrainCheckpoint class

#include "BrainCheckpoint.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>

namespace nlm {

bool BrainCheckpoint::save(Brain& brain, const std::string& filepath) {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            brain.getTotalNeuronCount(),
            brain.getTotalSynapseCount(),
            brain.getRegionCount(),
            brain.pImpl->currentStep,
            brain.pImpl->currentTime
        );
        
        // Write neurons
        writeNeuronState(brain, writer);
        
        // Write synapses
        writeSynapseState(brain, writer);
        
        // Write delayed spike events
        writeDelayedSpikeEvents(brain, writer);
        
        // Write Phase 5 extra sections
        writePhase5ExtraSections(brain, writer);
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::load(Brain& brain, const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
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
        
        // Verify neuron count
        if (reader.getHeader().neuronCount != brain.getTotalNeuronCount()) {
            NLM_LOG_ERROR("Checkpoint neuron mismatch");
            return false;
        }
        
        // Read neurons
        readNeuronState(brain, reader);
        
        // Read synapses
        readSynapseState(brain, reader);
        
        // Read delayed spike events
        readDelayedSpikeEvents(brain, reader);
        
        // Read Phase 5 extra sections
        readPhase5ExtraSections(brain, reader);
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainCheckpoint::saveSimulationState(Brain& brain, CheckpointWriter& writer) {
    // Save simulation state (counters, learned preference, steps-since-episode)
    std::vector<uint8_t> payload;
    p5AppendU64(payload, static_cast<uint64_t>(brain.pImpl->totalSpikesTotal));
    p5AppendU64(payload, static_cast<uint64_t>(brain.pImpl->totalSpikesThisStep));
    p5AppendFloat(payload, brain.pImpl->learnedPreference);
    p5AppendU64(payload, static_cast<uint64_t>(brain.pImpl->stepsSinceLastEpisode));
    writer.bufferSection(CheckpointSection::SimulationState, payload);
}

void BrainCheckpoint::loadSimulationState(Brain& brain, CheckpointReader& reader) {
    auto payload = reader.readSection(CheckpointSection::SimulationState);
    if (!payload.empty()) {
        P5Reader r{payload.data(), payload.data() + payload.size()};
        uint64_t total = 0, thisStep = 0, sinceEp = 0;
        float pref = 0.0f;
        if (r.u64(total) && r.u64(thisStep) && r.f32(pref) && r.u64(sinceEp) && r.ok) {
            brain.pImpl->totalSpikesTotal = static_cast<size_t>(total);
            brain.pImpl->totalSpikesThisStep = static_cast<size_t>(thisStep);
            brain.pImpl->learnedPreference = std::clamp(pref, -1.0f, 1.0f);
            brain.pImpl->stepsSinceLastEpisode = static_cast<size_t>(sinceEp);
        }
    }
}

void BrainCheckpoint::saveRandomState(Brain& brain, CheckpointWriter& writer) {
    // Save RNG engine state
    if (brain.pImpl->rng) {
        std::string state = brain.pImpl->rng->serializeState();
        std::vector<uint8_t> payload(state.begin(), state.end());
        writer.bufferSection(CheckpointSection::RandomState, payload);
    }
}

void BrainCheckpoint::loadRandomState(Brain& brain, CheckpointReader& reader) {
    auto payload = reader.readSection(CheckpointSection::RandomState);
    if (!payload.empty() && brain.pImpl->rng) {
        std::string state(payload.begin(), payload.end());
        brain.pImpl->rng->restoreState(state);
    }
}

void BrainCheckpoint::savePlasticityState(Brain& brain, CheckpointWriter& writer) {
    // Save per-synapse spike histories
    std::vector<uint8_t> payload;
    for (const auto& region : brain.pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            p5AppendU64(payload, syn->getSourceNeuron().index());
            p5AppendU64(payload, syn->getDestinationNeuron().index());
            p5AppendVec<double>(payload, syn->getPreSpikeHistory());
            p5AppendVec<double>(payload, syn->getPostSpikeHistory());
        }
    }
    writer.bufferSection(CheckpointSection::Plasticity, payload);
}

void BrainCheckpoint::loadPlasticityState(Brain& brain, CheckpointReader& reader) {
    auto payload = reader.readSection(CheckpointSection::Plasticity);
    if (!payload.empty()) {
        P5Reader r{payload.data(), payload.data() + payload.size()};
        std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
        auto pairKey = [](uint64_t src, uint64_t dst) { return (src << 32) ^ dst; };
        
        for (const auto& region : brain.pImpl->regions) {
            for (const auto& syn : region->getSynapses()) {
                byPair[pairKey(syn->getSourceNeuron().index(),
                               syn->getDestinationNeuron().index())].push_back(syn.get());
            }
        }
        
        std::unordered_map<uint64_t, size_t> usedCount;
        size_t restored = 0;
        while (r.p < r.end && r.ok) {
            uint64_t src = 0, dst = 0;
            std::vector<double> pre, post;
            if (!r.u64(src) || !r.u64(dst) || !r.vec(pre) || !r.vec(post) break;
            uint64_t key = pairKey(src, dst);
            auto it = byPair.find(key);
            if (it == byPair.end()) continue;
            size_t used = usedCount[key]++;
            if (used >= it->second.size()) continue;
            it->second[used]->setPreSpikeHistory(pre);
            it->second[used]->setPostSpikeHistory(post);
            ++restored;
        }
        NLM_LOG_INFO("Restored spike histories for " + std::to_string(restored) + " synapses");
    }
}

void BrainCheckpoint::saveMemoryState(Brain& brain, CheckpointWriter& writer) {
    // Save working memory and episodic memory
    std::vector<uint8_t> payload;
    
    if (brain.pImpl->workingMemory) {
        auto neurons = brain.pImpl->workingMemory->getMemoryNeurons();
        std::vector<uint64_t> ids;
        ids.reserve(neurons.size());
        for (const auto& n : neurons) ids.push_back(n.index());
        p5AppendVec<uint64_t>(payload, ids);
        p5AppendVec<float>(payload, brain.pImpl->workingMemory->getMemoryActivations());
        p5AppendVec<uint64_t>(payload, brain.pImpl->workingMemory->getMemoryTimestamps());
    } else {
        p5AppendVec<uint64_t>(payload, std::vector<uint64_t>{});
        p5AppendVec<float>(payload, std::vector<float>{});
        p5AppendVec<uint64_t>(payload, std::vector<uint64_t>{});
    }
    
    if (brain.pImpl->episodicMemory) {
        const auto& eps = brain.pImpl->episodicMemory->getEpisodes();
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
    
    writer.bufferSection(CheckpointSection::Memory, payload);
}

void BrainCheckpoint::loadMemoryState(Brain& brain, CheckpointReader& reader) {
    auto payload = reader.readSection(CheckpointSection::Memory);
    if (!payload.empty()) {
        P5Reader r{payload.data(), payload.data() + payload.size()};
        std::vector<uint64_t> wmIds;
        std::vector<float> wmActs;
        std::vector<uint64_t> wmTimes;
        if (r.vec(wmIds) && r.vec(wmActs) && r.vec(wmTimes) && r.ok &&
            brain.pImpl->workingMemory) {
            std::vector<NeuronId> neurons;
            neurons.reserve(wmIds.size());
            for (uint64_t id : wmIds) neurons.emplace_back(id);
            brain.pImpl->workingMemory->restoreSnapshot(neurons, wmActs, wmTimes);
        }
        uint64_t epCount = 0;
        if (r.u64(epCount) && r.ok && brain.pImpl->episodicMemory) {
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
                brain.pImpl->episodicMemory->restoreEpisodes(eps);
            }
        }
    }
}

void BrainCheckpoint::writeNeuronState(Brain& brain, CheckpointWriter& writer) {
    // Write neuron states to checkpoint
    NeuronCheckpointData neuronData;
    neuronData.membranePotential.reserve(brain.getTotalNeuronCount());
    neuronData.restingPotential.reserve(brain.getTotalNeuronCount());
    neuronData.threshold.reserve(brain.getTotalNeuronCount());
    neuronData.resetPotential.reserve(brain.getTotalNeuronCount());
    neuronData.leakConductance.reserve(brain.getTotalNeuronCount());
    neuronData.firingState.reserve(brain.getTotalNeuronCount());
    neuronData.refractoryRemaining.reserve(brain.getTotalNeuronCount());
    neuronData.refractoryPeriod.reserve(brain.getTotalNeuronCount());
    neuronData.lastSpikeTime.reserve(brain.getTotalNeuronCount());
    neuronData.firingRate.reserve(brain.getTotalNeuronCount());

    for (const auto& region : brain.pImpl->regions) {
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

    if (!writer.writeNeurons(neuronData)) {
        NLM_LOG_ERROR("Failed to write neurons to checkpoint");
    }
}

void BrainCheckpoint::writeSynapseState(Brain& brain, CheckpointWriter& writer) {
    // Write synapse states to checkpoint
    SynapseCheckpointData synapseData;
    for (const auto& region : brain.pImpl->regions) {
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

    if (!writer.writeSynapses(synapseData)) {
        NLM_LOG_ERROR("Failed to write synapses to checkpoint");
    }
}

void BrainCheckpoint::writeDelayedSpikeEvents(Brain& brain, CheckpointWriter& writer) {
    // Write pending delayed spike events
    if (brain.pImpl->spikeSystem) {
        auto pending = brain.pImpl->spikeSystem->getPendingDelayedSpikes();
        if (!writer.writeDelayedSpikes(pending)) {
            NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
        }
    }
}

void BrainCheckpoint::readNeuronState(Brain& brain, CheckpointReader& reader) {
    // Read neuron states from checkpoint
    NeuronCheckpointData neuronData;
    if (!reader.readNeurons(neuronData)) {
        NLM_LOG_ERROR("Failed to read neurons from checkpoint");
        return;
    }

    // Apply neuron states
    size_t idx = 0;
    for (auto& region : brain.pImpl->regions) {
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

void BrainCheckpoint::readSynapseState(Brain& brain, CheckpointReader& reader) {
    // Read synapse states from checkpoint
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        NLM_LOG_ERROR("Failed to read synapses from checkpoint");
        return;
    }

    // Apply synapse states (match by (source, destination) pair)
    std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
    auto pairKey = [](uint64_t src, uint64_t dst) { return (src << 32) ^ dst; };
    for (const auto& region : brain.pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
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
        if (it != byPair.end()) {
            size_t used = usedCount[key]++;
            if (used < it->second.size()) {
                it->second[used]->setWeight(synapseData.weight[i]);
                if (i < synapseData.delay.size()) {
                    it->second[used]->setDelay(synapseData.delay[i]);
                }
                if (i < synapseData.synapseType.size()) {
                    it->second[used]->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                }
                if (i < synapseData.eligibilityTrace.size()) {
                    it->second[used]->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                }
                if (i < synapseData.efficacy.size()) {
                    it->second[used]->setEfficacy(synapseData.efficacy[i]);
                }
                ++restored;
            }
        }
    }
    NLM_LOG_INFO("Restored " + std::to_string(restored) + " / " +
                 std::to_string(synapseData.weight.size()) + " synapses");
}

void BrainCheckpoint::readDelayedSpikeEvents(Brain& brain, CheckpointReader& reader) {
    // Read pending delayed spike events
    std::vector<DelayedSpikeEvent> pending;
    if (reader.readDelayedSpikes(pending)) {
        if (brain.pImpl->spikeSystem) {
            brain.pImpl->spikeSystem->restorePendingDelayedSpikes(pending);
        }
        NLM_LOG_INFO("Restored " + std::to_string(pending.size()) + " pending delayed spikes");
    }
}

void BrainCheckpoint::writePhase5ExtraSections(Brain& brain, CheckpointWriter& writer) {
    // Write Phase 5 extra sections: SimulationState, RandomState, Plasticity, Memory
    saveSimulationState(brain, writer);
    saveRandomState(brain, writer);
    savePlasticityState(brain, writer);
    saveMemoryState(brain, writer);
}

void BrainCheckpoint::readPhase5ExtraSections(Brain& brain, CheckpointReader& reader) {
    // Read Phase 5 extra sections: SimulationState, RandomState, Plasticity, Memory
    loadSimulationState(brain, reader);
    loadRandomState(brain, reader);
    loadPlasticityState(brain, reader);
    loadMemoryState(brain, reader);
}

bool BrainCheckpoint::p5AppendU64(std::vector<uint8_t>& out, uint64_t v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
    return true;
}

bool BrainCheckpoint::p5AppendFloat(std::vector<uint8_t>& out, float v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
    return true;
}

bool BrainCheckpoint::p5AppendDouble(std::vector<uint8_t>& out, double v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
    return true;
}

template <typename T>
bool BrainCheckpoint::p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v) {
    uint64_t n = static_cast<uint64_t>(v.size());
    p5AppendU64(out, n);
    if (!v.empty()) {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(v.data());
        out.insert(out.end(), b, b + v.size() * sizeof(T));
    }
    return true;
}
