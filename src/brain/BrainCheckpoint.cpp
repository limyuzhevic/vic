#include "BrainCheckpoint.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <algorithm>
#include <unordered_map>

namespace nlm {

bool BrainCheckpoint::save(Brain* brain, const std::string& filepath) const {
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        NLM_LOG_INFO("Saving brain state to " + filepath);

        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            stats.saveSuccess = false;
            return false;
        }

        // Set metadata
        writer.setMetadata(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getRegionCount(),
            brain->pImpl->currentStep,
            brain->pImpl->currentTime
        );

        // Write neurons
        if (!writeNeurons(brain, writer)) {
            stats.saveSuccess = false;
            return false;
        }

        // Write synapses
        if (!writeSynapses(brain, writer)) {
            stats.saveSuccess = false;
            return false;
        }

        // Write pending delayed spikes
        if (brain->pImpl->spikeSystem) {
            auto pending = brain->pImpl->spikeSystem->getPendingDelayedSpikes();
            if (!writer.writeDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
                stats.saveSuccess = false;
                return false;
            }
            stats.spikesSaved = pending.size();
        }

        // Phase 5 extra sections
        if (!writePhase5ExtraSections(brain, writer)) {
            stats.saveSuccess = false;
            return false;
        }

        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            stats.saveSuccess = false;
            return false;
        }

        stats.neuronsSaved = brain->getTotalNeuronCount();
        stats.synapsesSaved = brain->getTotalSynapseCount();

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        stats.saveTime = duration;

        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        stats.saveSuccess = true;
        return true;

    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        stats.saveSuccess = false;
        return false;
    }
}

bool BrainCheckpoint::load(Brain* brain, const std::string& filepath) {
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        NLM_LOG_INFO("Loading brain state from " + filepath);

        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            stats.loadSuccess = false;
            return false;
        }

        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            stats.loadSuccess = false;
            return false;
        }

        // Neuron count must match exactly
        if (reader.getHeader().neuronCount != brain->getTotalNeuronCount()) {
            NLM_LOG_ERROR("Checkpoint neuron mismatch: header has " +
                         std::to_string(reader.getHeader().neuronCount) + " neurons, this brain has " +
                         std::to_string(brain->getTotalNeuronCount()));
            stats.loadSuccess = false;
            return false;
        }

        // Read neurons
        if (!readNeurons(brain, reader)) {
            stats.loadSuccess = false;
            return false;
        }

        // Read synapses
        if (!readSynapses(brain, reader)) {
            stats.loadSuccess = false;
            return false;
        }

        // Read delayed spikes
        if (!readDelayedSpikes(brain, reader)) {
            stats.loadSuccess = false;
            return false;
        }

        // Read Phase 5 extra sections
        if (!readPhase5ExtraSections(brain, reader)) {
            stats.loadSuccess = false;
            return false;
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        stats.loadTime = duration;

        NLM_LOG_INFO("Brain state loaded successfully");
        stats.loadSuccess = true;
        return true;

    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        stats.loadSuccess = false;
        return false;
    }
}

bool BrainCheckpoint::writeNeurons(Brain* brain, CheckpointWriter& writer) const {
    NeuronCheckpointData neuronData;
    neuronData.membranePotential.reserve(brain->getTotalNeuronCount());
    neuronData.restingPotential.reserve(brain->getTotalNeuronCount());
    neuronData.threshold.reserve(brain->getTotalNeuronCount());
    neuronData.resetPotential.reserve(brain->getTotalNeuronCount());
    neuronData.leakConductance.reserve(brain->getTotalNeuronCount());
    neuronData.firingState.reserve(brain->getTotalNeuronCount());
    neuronData.refractoryRemaining.reserve(brain->getTotalNeuronCount());
    neuronData.refractoryPeriod.reserve(brain->getTotalNeuronCount());
    neuronData.lastSpikeTime.reserve(brain->getTotalNeuronCount());
    neuronData.firingRate.reserve(brain->getTotalNeuronCount());

    for (const auto& region : brain->pImpl->regions) {
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
        return false;
    }
    return true;
}

bool BrainCheckpoint::writeSynapses(Brain* brain, CheckpointWriter& writer) const {
    SynapseCheckpointData synapseData;
    for (const auto& region : brain->pImpl->regions) {
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
        return false;
    }
    return true;
}

bool BrainCheckpoint::writeDelayedSpikes(Brain* brain, CheckpointWriter& writer) const {
    if (brain->pImpl->spikeSystem) {
        auto pending = brain->pImpl->spikeSystem->getPendingDelayedSpikes();
        if (!writer.writeDelayedSpikes(pending)) {
            NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
            return false;
        }
        stats.spikesSaved = pending.size();
    }
    return true;
}

bool BrainCheckpoint::writePhase5ExtraSections(Brain* brain, CheckpointWriter& writer) const {
    // SimulationState: spike counters, learned preference trait,
    // steps-since-episode counter (episodic storage cadence).
    {
        std::vector<uint8_t> payload;
        p5AppendU64(payload, static_cast<uint64_t>(brain->pImpl->totalSpikesTotal));
        p5AppendU64(payload, static_cast<uint64_t>(brain->pImpl->totalSpikesThisStep));
        p5AppendFloat(payload, brain->pImpl->learnedPreference);
        p5AppendU64(payload, static_cast<uint64_t>(brain->pImpl->stepsSinceLastEpisode));
        writer.bufferSection(CheckpointSection::SimulationState, payload);
    }

    // RandomState: full RNG engine state, not just the seed.
    if (brain->pImpl->rng) {
        std::string state = brain->pImpl->rng->serializeState();
        std::vector<uint8_t> payload(state.begin(), state.end());
        writer.bufferSection(CheckpointSection::RandomState, payload);
    }

    // Plasticity: per-synapse pre/post spike histories, in the same
    // region order as the synapse section, matched by pair on load.
    {
        std::vector<uint8_t> payload;
        for (const auto& region : brain->pImpl->regions) {
            for (const auto& syn : region->getSynapses()) {
                p5AppendU64(payload, syn->getSourceNeuron().index());
                p5AppendU64(payload, syn->getDestinationNeuron().index());
                p5AppendVec<double>(payload, syn->getPreSpikeHistory());
                p5AppendVec<double>(payload, syn->getPostSpikeHistory());
            }
        }
        writer.bufferSection(CheckpointSection::Plasticity, payload);
    }

    // Memory: working-memory content + full episodic episodes.
    {
        std::vector<uint8_t> payload;
        if (brain->pImpl->workingMemory) {
            auto neurons = brain->pImpl->workingMemory->getMemoryNeurons();
            std::vector<uint64_t> ids;
            ids.reserve(neurons.size());
            for (const auto& n : neurons) ids.push_back(n.index());
            p5AppendVec<uint64_t>(payload, ids);
            p5AppendVec<float>(payload, brain->pImpl->workingMemory->getMemoryActivations());
            p5AppendVec<uint64_t>(payload, brain->pImpl->workingMemory->getMemoryTimestamps());
        } else {
            p5AppendVec<uint64_t>(payload, {});
            p5AppendVec<float>(payload, {});
            p5AppendVec<uint64_t>(payload, {});
        }

        if (brain->pImpl->episodicMemory) {
            const auto& eps = brain->pImpl->episodicMemory->getEpisodes();
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

    stats.memoryItemsSaved = brain->pImpl->episodicMemory ? brain->pImpl->episodicMemory->getEpisodes().size() : 0;
    return true;
}

bool BrainCheckpoint::readNeurons(Brain* brain, CheckpointReader& reader) {
    NeuronCheckpointData neuronData;
    if (!reader.readNeurons(neuronData)) {
        NLM_LOG_ERROR("Failed to read neurons from checkpoint");
        return false;
    }

    // Apply neuron states, including fields the old load dropped
    size_t idx = 0;
    for (auto& region : brain->pImpl->regions) {
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
                    // Firing rate is the decode signal, so a
                    // reloaded brain must resume with the saved
                    // rates, not restart its EMA from zero.
                    if (idx < neuronData.firingRate.size()) {
                        state.firingRate = neuronData.firingRate[idx];
                    }
                }
                idx++;
            }
        }
    }
    return true;
}

bool BrainCheckpoint::readSynapses(Brain* brain, CheckpointReader& reader) {
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        NLM_LOG_ERROR("Failed to read synapses from checkpoint");
        return false;
    }

    // Apply synapse states. Match by (source, destination) pair
    {
        std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
        auto pairKey = [](uint64_t src, uint64_t dst) {
            return (src << 32) ^ dst;
        };
        for (auto& region : brain->pImpl->regions) {
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
            if (it != byPair.end() && !it->second.empty()) {
                Synapse* syn = it->second.back();
                it->second.pop_back();
                usedCount[key]++;
                syn->setWeight(synapseData.weight[i]);
                restored++;
            }
        }

        // Check for any unmapped synapses
        bool allMatched = true;
        for (const auto& kv : byPair) {
            if (!kv.second.empty()) {
                allMatched = false;
                break;
            }
        }

        if (!allMatched) {
            NLM_LOG_INFO("Some checkpoint synapses were not restored (source, destination mismatch)");
        }

        stats.synapsesSaved = restored;
    }
    return true;
}

bool BrainCheckpoint::readDelayedSpikes(Brain* brain, CheckpointReader& reader) {
    std::vector<DelayedSpikeEvent> pending;
    if (!reader.readDelayedSpikes(pending)) {
        NLM_LOG_ERROR("Failed to read pending spikes from checkpoint");
        return false;
    }
    if (brain->pImpl->spikeSystem) {
        brain->pImpl->spikeSystem->queueDelayedSpikes(pending);
        stats.spikesSaved = pending.size();
    }
    return true;
}

bool BrainCheckpoint::readPhase5ExtraSections(Brain* brain, CheckpointReader& reader) {
    auto section = reader.getSection(CheckpointSection::SimulationState);
    if (section) {
        P5Reader p5Reader(section->data.data(), section->data.data() + section->data.size());
        uint64_t v;
        if (p5Reader.u64(v)) brain->pImpl->totalSpikesTotal = static_cast<size_t>(v);
        if (p5Reader.u64(v)) brain->pImpl->totalSpikesThisStep = static_cast<size_t>(v);
        float f;
        if (p5Reader.f32(f)) brain->pImpl->learnedPreference = f;
        if (p5Reader.u64(v)) brain->pImpl->stepsSinceLastEpisode = static_cast<size_t>(v);
    }

    section = reader.getSection(CheckpointSection::RandomState);
    if (section && brain->pImpl->rng) {
        brain->pImpl->rng->deserializeState(std::string(section->data.begin(), section->data.end()));
    }

    section = reader.getSection(CheckpointSection::Plasticity);
    if (section) {
        P5Reader p5Reader(section->data.data(), section->data.data() + section->data.size());
        std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
        auto pairKey = [](uint64_t src, uint64_t dst) { return (src << 32) ^ dst; };

        for (auto& region : brain->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                byPair[pairKey(syn->getSourceNeuron().index(),
                               syn->getDestinationNeuron().index())].push_back(syn.get());
            }
        }

        while (true) {
            uint64_t src, dst;
            std::vector<double> preHist, postHist;
            if (!p5Reader.u64(src)) break;
            if (!p5Reader.u64(dst)) break;
            if (!p5Reader.vec(preHist)) break;
            if (!p5Reader.vec(postHist)) break;

            uint64_t key = pairKey(src, dst);
            auto it = byPair.find(key);
            if (it != byPair.end() && !it->second.empty()) {
                Synapse* syn = it->second.back();
                syn->setPreSpikeHistory(preHist);
                syn->setPostSpikeHistory(postHist);
            }
        }
    }

    section = reader.getSection(CheckpointSection::Memory);
    if (section) {
        P5Reader p5Reader(section->data.data(), section->data.data() + section->data.size());
        std::vector<uint64_t> wmNeurons;
        std::vector<float> wmActivations;
        std::vector<uint64_t> wmTimestamps;
        if (p5Reader.vec(wmNeurons) && p5Reader.vec(wmActivations) && p5Reader.vec(wmTimestamps)) {
            if (brain->pImpl->workingMemory) {
                brain->pImpl->workingMemory->clear();
                for (size_t i = 0; i < wmNeurons.size() && i < wmTimestamps.size(); ++i) {
                    brain->pImpl->workingMemory->storeToNeuron(NeuronId(wmNeurons[i]), wmActivations[i]);
                    // Note: Timestamps would need to be stored if working memory supports them
                }
            }
        }

        uint64_t episodeCount;
        if (p5Reader.u64(episodeCount)) {
            if (brain->pImpl->episodicMemory) {
                brain->pImpl->episodicMemory->clear();
                for (uint64_t i = 0; i < episodeCount; ++i) {
                    EpisodicMemoryItem episode;
                    uint64_t ts;
                    float px, py, orient, reward, energy, novelty, resultingReward;
                    uint64_t age;
                    std::vector<float> sensoryState, resultingSensoryState;
                    std::vector<uint64_t> activeIds;
                    std::vector<float> activations;

                    if (p5Reader.u64(ts)) episode.timestamp = ts;
                    if (p5Reader.f32(px)) episode.positionX = px;
                    if (p5Reader.f32(py)) episode.positionY = py;
                    if (p5Reader.f32(orient)) episode.orientation = orient;
                    if (p5Reader.u64(static_cast<uint64_t&>(episode.action))) {
                        // Convert back
                    }
                    if (p5Reader.f32(reward)) episode.reward = reward;
                    if (p5Reader.f32(energy)) episode.energy = energy;
                    if (p5Reader.f32(novelty)) episode.novelty = novelty;
                    if (p5Reader.f32(resultingReward)) episode.resultingReward = resultingReward;
                    if (p5Reader.u64(age)) episode.age = age;
                    if (p5Reader.vec(sensoryState)) episode.sensoryState = sensoryState;
                    if (p5Reader.vec(resultingSensoryState)) episode.resultingSensoryState = resultingSensoryState;
                    if (p5Reader.vec(activeIds)) {
                        for (uint64_t id : activeIds) {
                            episode.activeNeurons.push_back(NeuronId(id));
                        }
                    }
                    if (p5Reader.vec(activations)) episode.neuronActivations = activations;

                    if (brain->pImpl->episodicMemory) {
                        brain->pImpl->episodicMemory->storeEpisode(episode);
                    }
                }
            }
        }
    }

    return true;
}

uint64_t BrainCheckpoint::makePairKey(uint64_t src, uint64_t dst) {
    return (src << 32) ^ dst;
}

// Phase 5 checkpoint payload helpers (legacy format compatibility)
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
