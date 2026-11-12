// Brain checkpoint functionality - extracted from monolithic Brain.cpp
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Extract save/load checkpoint functionality from Brain.cpp

// Additional checkpoint helper methods
void Brain::saveSystemStates(CheckpointWriter& writer) {
    // Save brain system states for checkpointing
    
    // Save memory systems
    if (pImpl->workingMemory) {
        std::vector<uint64_t> wmIds;
        std::vector<float> wmActs;
        std::vector<uint64_t> wmTimes;
        
        const auto& neurons = pImpl->workingMemory->getMemoryNeurons();
        const auto& activations = pImpl->workingMemory->getMemoryActivations();
        const auto& timestamps = pImpl->workingMemory->getMemoryTimestamps();
        
        wmIds.reserve(neurons.size());
        wmActs.reserve(activations.size());
        wmTimes.reserve(timestamps.size());
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            wmIds.push_back(neurons[i].index());
            wmActs.push_back(activations[i]);
            wmTimes.push_back(static_cast<uint64_t>(timestamps[i]));
        }
        
        std::vector<uint8_t> payload;
        P5Writer pw{payload};
        
        if (pw.vec(wmIds) && pw.vec(wmActs) && pw.vec(wmTimes)) {
            writer.writeSection(CheckpointSection::Memory, payload);
        }
    }
    
    // Save episodic memory
    if (pImpl->episodicMemory) {
        uint64_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
        std::vector<uint8_t> payload;
        P5Writer pw{payload};
        
        if (pw.u64(episodeCount)) {
            for (uint64_t i = 0; i < episodeCount; ++i) {
                const auto& episode = pImpl->episodicMemory->retrieveEpisode(i);
                
                uint64_t ts = static_cast<uint64_t>(episode.timestamp);
                uint64_t actionRaw = 0;  // Placeholder for action
                uint64_t age = 0;        // Placeholder for age
                
                if (pw.u64(ts) && pw.u64(actionRaw) && pw.u64(age)) {
                    // Store episode data
                    // In full implementation, would serialize episode details
                }
            }
            
            writer.writeSection(CheckpointSection::Memory, payload);
        }
    }
    
    // Save spike system history
    if (pImpl->spikeSystem) {
        std::vector<uint8_t> payload;
        P5Writer pw{payload};
        
        // Save spike histories for plasticity
        std::vector<std::pair<uint64_t, std::vector<double>>> spikeHistories;
        
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->hasPreSpikeHistory() && syn->hasPostSpikeHistory()) {
                    uint64_t src = syn->getSourceNeuron().index();
                    uint64_t dst = syn->getDestinationNeuron().index();
                    
                    std::vector<double> pre = syn->getPreSpikeHistory();
                    std::vector<double> post = syn->getPostSpikeHistory();
                    
                    if (!pre.empty() && !post.empty()) {
                        spikeHistories.push_back({(src << 32) | dst, pre});
                        spikeHistories.push_back({(src << 32) | dst, post});
                    }
                }
            }
        }
        
        if (!spikeHistories.empty()) {
            if (pw.vec(spikeHistories)) {
                writer.writeSection(CheckpointSection::Plasticity, payload);
            }
        }
    }
}

void Brain::loadSystemStates(CheckpointReader& reader) {
    // Load brain system states from checkpoint
    
    // Load working memory
    auto memoryPayload = reader.readSection(CheckpointSection::Memory);
    if (!memoryPayload.empty()) {
        P5Reader pr{memoryPayload.data(), memoryPayload.data() + memoryPayload.size()};
        
        std::vector<uint64_t> wmIds;
        std::vector<float> wmActs;
        std::vector<uint64_t> wmTimes;
        
        if (pr.vec(wmIds) && pr.vec(wmActs) && pr.vec(wmTimes) && pr.ok &&
            pImpl->workingMemory) {
            
            std::vector<NeuronId> neurons;
            neurons.reserve(wmIds.size());
            for (uint64_t id : wmIds) neurons.emplace_back(id);
            
            pImpl->workingMemory->restoreSnapshot(neurons, wmActs, wmTimes);
        }
    }
    
    // Load episodic memory
    auto episodicPayload = reader.readSection(CheckpointSection::Memory);
    if (!episodicPayload.empty()) {
        P5Reader pr{episodicPayload.data(), episodicPayload.data() + episodicPayload.size()};
        
        uint64_t epCount = 0;
        if (pr.u64(epCount) && pr.ok && pImpl->episodicMemory) {
            std::vector<EpisodicMemoryItem> eps;
            eps.reserve(static_cast<size_t>(epCount));
            
            for (uint64_t i = 0; i < epCount && pr.ok; ++i) {
                EpisodicMemoryItem ep;
                uint64_t ts = 0, actionRaw = 0, age = 0;
                
                if (pr.u64(ts) && pr.u64(actionRaw) && pr.u64(age)) {
                    ep.timestamp = ts;
                    // In full implementation, would deserialize full episode
                }
            }
            
            // Would load episodes into episodic memory
            // pImpl->episodicMemory->loadEpisodes(eps);
        }
    }
    
    // Load plasticity system state
    auto plasticityPayload = reader.readSection(CheckpointSection::Plasticity);
    if (!plasticityPayload.empty()) {
        P5Reader pr{plasticityPayload.data(), plasticityPayload.data() + plasticityPayload.size()};
        
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
        
        while (pr.p < pr.end && pr.ok) {
            uint64_t src =  DEMO, dst = 0;
            std::vector<double> pre, post;
            
            if (!pr.u64(src) || !pr.u64(dst) || !pr.vec(pre) || !pr.vec(post)) break;
            
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

// Additional checkpoint management methods
void Brain::updateCheckpointState(const std::string& checkpointPath) {
    // Update checkpoint manager state based on configuration
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->setCheckpointPath(checkpointPath);
    }
}

void Brain::scheduleCheckpoint() {
    // Schedule automatic checkpoint based on configuration
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->scheduleCheckpoint();
    }
}

} // namespace nlm