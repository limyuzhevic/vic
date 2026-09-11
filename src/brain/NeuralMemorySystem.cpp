// Forward declarations for event-driven computation
struct SpikeEvent;
struct DelayedSpikeEvent;

namespace nlm {

void NeuralMemorySystem::initialize(Brain* brain) {
    if (brain) {
        // Initialize working memory
        if (auto* wm = brain->getWorkingMemory()) {
            wm->initialize(brain);
        }
        
        // Initialize episodic memory
        if (auto* em = brain->getEpisodicMemory()) {
            em->initialize(brain);
        }
        
        // Initialize associative memory
        if (auto* am = brain->getAssociativeMemory()) {
            am->initialize(brain);
        }
    }
}

void NeuralMemorySystem::storeToNeuron(Brain& brain, NeuronId neuronId, float activation) {
    if (auto* wm = brain.getWorkingMemory()) {
        wm->storeToNeuron(neuronId, activation);
    }
}

void NeuralMemorySystem::update(Brain& brain, TimestepDuration dt) {
    if (auto* wm = brain.getWorkingMemory()) {
        wm->update(dt);
    }
    
    // Update working memory traces
    updateWorkingMemoryTraces(brain, dt);
    
    // Process working memory competition
    processWorkingMemoryCompetition(brain);
}

void NeuralMemorySystem::clearWorkingMemory(Brain& brain) {
    if (auto* wm = brain.getWorkingMemory()) {
        wm->clear();
    }
}

void NeuralMemorySystem::storeEpisode(Brain& brain) {
    if (auto* em = brain.getEpisodicMemory()) {
        em->storeEpisode(captureCurrentState(brain));
    }
}

void NeuralMemorySystem::replayEpisodes(Brain& brain, size_t count) {
    if (auto* em = brain.getEpisodicMemory()) {
        auto episodes = em->getEpisodesForReplay(count);
        for (const auto* episode : *episodes) {
            replayEpisode(brain, *episode);
        }
    }
}

void NeuralMemorySystem::consolidate(Brain& brain, float threshold) {
    if (auto* em = brain.getEpisodicMemory()) {
        em->consolidate(threshold);
    }
}

void NeuralMemorySystem::clearEpisodicMemory(Brain& brain) {
    if (auto* em = brain.getEpisodicMemory()) {
        em->clear();
    }
}

void NeuralMemorySystem::storeAssociation(Brain& brain, std::vector<NeuronId> pattern1,
                                        std::vector<NeuronId> pattern2, float strength) {
    if (auto* am = brain.getAssociativeMemory()) {
        am->storeAssociation(pattern1, pattern2, strength);
    }
}

std::vector<NeuronId> NeuralMemorySystem::recallAssociation(Brain& brain,
                                                           std::vector<NeuronId> pattern1) {
    if (auto* am = brain.getAssociativeMemory()) {
        return am->recallAssociation(pattern1);
    }
    return {};
}

void NeuralMemorySystem::updateAssociativeMemory(Brain& brain, TimestepDuration dt) {
    if (auto* am = brain.getAssociativeMemory()) {
        am->update(dt);
    }
}

size_t NeuralMemorySystem::getActiveTraces(Brain& brain) const {
    if (auto* wm = brain.getWorkingMemory()) {
        return wm->getActiveTraces();
    }
    return 0;
}

size_t NeuralMemorySystem::getEpisodeCount(Brain& brain) const {
    if (auto* em = brain.getEpisodicMemory()) {
        return em->getEpisodeCount();
    }
    return 0;
}

const std::vector<EpisodicMemoryItem>* NeuralMemorySystem::getEpisodesForReplay(
    Brain& brain, size_t count) const {
    if (auto* em = brain.getEpisodicMemory()) {
        return em->getEpisodesForReplay(count);
    }
    return nullptr;
}

void NeuralMemorySystem::setWorkingMemoryCapacity(Brain& brain, size_t capacity) {
    if (auto* wm = brain.getWorkingMemory()) {
        wm->setCapacity(capacity);
    }
}

void NeuralMemorySystem::setMaxEpisodes(Brain& brain, size_t maxEpisodes) {
    if (auto* em = brain.getEpisodicMemory()) {
        em->setMaxEpisodes(maxEpisodes);
    }
}

void NeuralMemorySystem::logMemoryStatus(Brain& brain) const {
    NLM_LOG_INFO("=== Memory System Status ===");
    
    // Working memory status
    size_t activeTraces = getActiveTraces(brain);
    NLM_LOG_INFO("Working memory traces: " + std::to_string(activeTraces));
    
    // Episodic memory status
    size_t episodeCount = getEpisodeCount(brain);
    NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodeCount));
    
    // Associative memory status
    // (Would need to add method to associative memory for count)
    NLM_LOG_INFO("Associative memory: Active associations tracked");
}

void NeuralMemorySystem::updateWorkingMemoryTraces(Brain& brain, TimestepDuration dt) {
    // Decay of memory traces over time
    if (auto* wm = brain.getWorkingMemory()) {
        wm->decayTraces(dt);
    }
}

void NeuralMemorySystem::processWorkingMemoryCompetition(Brain& brain) {
    if (auto* wm = brain.getWorkingMemory()) {
        // Competition among memory traces
        std::vector<NeuronId> competitors = wm->getMemoryNeurons();
        if (!competitors.empty()) {
            // Attention system processes competition
            if (auto* attention = brain.getAttention()) {
                attention->processCompetition(competitors);
            }
        }
    }
}

EpisodicMemoryItem NeuralMemorySystem::captureCurrentState(Brain& brain) const {
    EpisodicMemoryItem episode;
    episode.timestamp = static_cast<size_t>(brain.getTotalSpikeCount());
    
    // Record active neurons
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (neuron->isFiring() ||
                    std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                }
            }
        }
    }
    
    // Record reward level
    if (auto* dopamine = brain.getDopamine()) {
        episode.reward = dopamine->getLevel();
    }
    
    return episode;
}

void NeuralMemorySystem::replayEpisode(Brain& brain, const EpisodicMemoryItem& episode) const {
    // Reactivate neurons based on episode
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        NeuronId neuronId = episode.activeNeurons[i];
        float activation = episode.neuronActivations[i] * 10.0f;  // Scale up for replay
        
        // Inject current to reactivate neuron
        brain.injectCurrent(neuronId, activation);
    }
}

std::vector<const EpisodicMemoryItem*> NeuralMemorySystem::findSimilarEpisodes(
    Brain& brain, const EpisodicMemoryItem& current) const {
    std::vector<const EpisodicMemoryItem*> similar;
    
    if (auto* em = brain.getEpisodicMemory()) {
        auto allEpisodes = em->getAllEpisodes();
        for (const auto& episode : allEpisodes) {
            if (compareEpisodes(episode, current) > 0.8f) {
                similar.push_back(&episode);
            }
        }
    }
    
    return similar;
}

void NeuralMemorySystem::compressMemoryRepresentation(EpisodicMemoryItem& episode) const {
    // Reduce dimensionality while preserving key features
    if (episode.neuronActivations.size() > 100) {
        // Sample key activations
        std::vector<size_t> indices;
        size_t step = episode.neuronActivations.size() / 100;
        for (size_t i = 0; i < 100; ++i) {
            indices.push_back(i * step);
        }
        
        std::vector<NeuronId> compressedNeurons;
        std::vector<float> compressedActivations;
        
        for (size_t idx : indices) {
            if (idx < episode.activeNeurons.size()) {
                compressedNeurons.push_back(episode.activeNeurons[idx]);
                compressedActivations.push_back(episode.neuronActivations[idx]);
            }
        }
        
        episode.activeNeurons = compressedNeurons;
        episode.neuronActivations = compressedActivations;
    }
}

float NeuralMemorySystem::compareEpisodes(const EpisodicMemoryItem& episode1,
                                        const EpisodicMemoryItem& episode2) const {
    // Simple similarity metric based on overlap and activation patterns
    float similarity = 0.0f;
    
    // Compare active neurons (simplified - in real system would use embeddings)
    std::unordered_map<NeuronId, bool> neurons1;
    for (const auto& id : episode1.activeNeurons) {
        neurons1[id] = true;
    }
    
    float overlap = 0.0f;
    for (const auto& id : episode2.activeNeurons) {
        if (neurons1.find(id) != neurons1.end()) {
            overlap++;
        }
    }
    
    if (!episode1.activeNeurons.empty() && !episode2.activeNeurons.empty()) {
        similarity = overlap / std::min(episode1.activeNeurons.size(),
                                      episode2.activeNeurons.size());
    }
    
    return similarity;
}

} // namespace nlm
