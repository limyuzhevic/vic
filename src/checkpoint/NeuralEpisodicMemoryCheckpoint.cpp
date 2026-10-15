// NeuralEpisodicMemory save/load implementation

#include "NeuralEpisodicMemoryCheckpoint.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include <fstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

namespace {

// NeuralEpisodicMemorySaverLoader implementation
class NeuralEpisodicMemorySaverLoaderImpl : public NeuralEpisodicMemorySaverLoader {
public:
    bool save(const NeuralEpisodicMemoryCheckpoint& checkpoint) const override {
        try {
            // Create a simple file-based save
            std::ofstream file("episodic_memory_checkpoint.tmp", std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write magic number for validation
            const uint64_t magic = 0x4E454D43524B00ULL; // "NEMCK"
            file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            
            // Write checkpoint version
            const uint32_t version = 1;
            file.write(reinterpret_cast<const char*>(&version), sizeof(version));
            
            // Write number of episodes
            uint64_t episodesSize = checkpoint.episodes.size();
            file.write(reinterpret_cast<const char*>(&episodesSize), sizeof(episodesSize));
            
            if (episodesSize > 0) {
                // Write each episode
                for (const auto& episode : checkpoint.episodes) {
                    // Write episode data (simplified serialization)
                    file.write(reinterpret_cast<const char*>(&episode.timestamp), sizeof(episode.timestamp));
                    
                    // Write sensory state
                    uint64_t sensorySize = episode.sensoryState.size();
                    file.write(reinterpret_cast<const char*>(&sensorySize), sizeof(sensorySize));
                    if (sensorySize > 0) {
                        file.write(reinterpret_cast<const char*>(episode.sensoryState.data()),
                                 sensorySize * sizeof(float));
                    }
                    
                    // Write other fields
                    file.write(reinterpret_cast<const char*>(&episode.positionX), sizeof(episode.positionX));
                    file.write(reinterpret_cast<const char*>(&episode.positionY), sizeof(episode.positionY));
                    file.write(reinterpret_cast<const char*>(&episode.orientation), sizeof(episode.orientation));
                    file.write(reinterpret_cast<const char*>(&episode.action), sizeof(episode.action));
                    file.write(reinterpret_cast<const char*>(&episode.reward), sizeof(episode.reward));
                    file.write(reinterpret_cast<const char*>(&episode.energy), sizeof(episode.energy));
                    file.write(reinterpret_cast<const char*>(&episode.novelty), sizeof(episode.novelty));
                    file.write(reinterpret_cast<const char*>(&episode.resultingReward), sizeof(episode.resultingReward));
                    
                    // Write neurons and activations
                    uint64_t neuronsSize = episode.activeNeurons.size();
                    file.write(reinterpret_cast<const char*>(&neuronsSize), sizeof(neuronsSize));
                    if (neuronsSize > 0) {
                        file.write(reinterpret_cast<const char*>(episode.activeNeurons.data()),
                                 neuronsSize * sizeof(NeuronId));
                    }
                    
                    uint64_t activationsSize = episode.neuronActivations.size();
                    file.write(reinterpret_cast<const char*>(&activationsSize), sizeof(activationsSize));
                    if (activationsSize > 0) {
                        file.write(reinterpret_cast<const char*>(episode.neuronActivations.data()),
                                 activationsSize * sizeof(float));
                    }
                    
                    // Write other fields
                    file.write(reinterpret_cast<const char*>(&episode.age), sizeof(episode.age));
                    file.write(reinterpret_cast<const char*>(&episode.predictionError), sizeof(episode.predictionError));
                    file.write(reinterpret_cast<const char*>(&episode.confidence), sizeof(episode.confidence));
                    file.write(reinterpret_cast<const char*>(&episode.sociallyRelevant), sizeof(episode.sociallyRelevant));
                    file.write(reinterpret_cast<const char*>(&episode.socialReward), sizeof(episode.socialReward));
                }
            }
            
            // Write other checkpoint data
            file.write(reinterpret_cast<const char*>(&checkpoint.maxEpisodes), sizeof(checkpoint.maxEpisodes));
            file.write(reinterpret_cast<const char*>(&checkpoint.replayEnabled), sizeof(checkpoint.replayEnabled));
            file.write(reinterpret_cast<const char*>(&checkpoint.consolidationEfficiency), sizeof(checkpoint.consolidationEfficiency));
            file.write(reinterpret_cast<const char*>(&checkpoint.consolidationCount), sizeof(checkpoint.consolidationCount));
            
            // Write derived sizes
            uint64_t recentEpisodesSize = checkpoint.recentEpisodes.size();
            file.write(reinterpret_cast<const char*>(&recentEpisodesSize), sizeof(recentEpisodesSize));
            
            uint64_t episodicBufferSize = checkpoint.episodicBuffer.size();
            file.write(reinterpret_cast<const char*>(&episodicBufferSize), sizeof(episodicBufferSize));
            
            file.close();
            
            // Atomic save: move temp to final file
            std::rename("episodic_memory_checkpoint.tmp", "episodic_memory_checkpoint.bin");
            
            return true;
        } catch (const std::exception& e) {
            // Clean up temp file if it exists
            std::remove("episodic_memory_checkpoint.tmp");
            return false;
        }
    }
    
    bool load(NeuralEpisodicMemoryCheckpoint& checkpoint) override {
        try {
            std::ifstream file("episodic_memory_checkpoint.bin", std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Read magic number for validation
            uint64_t magic;
            file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
            if (magic != 0x4E454D43524B00ULL) {
                return false;
            }
            
            // Read version
            uint32_t version;
            file.read(reinterpret_cast<char*>(&version), sizeof(version));
            if (version != 1) {
                return false;
            }
            
            // Clear current checkpoint
            checkpoint.episodes.clear();
            checkpoint.recentEpisodes.clear();
            checkpoint.episodicBuffer.clear();
            
            // Read number of episodes
            uint64_t episodesSize;
            file.read(reinterpret_cast<char*>(&episodesSize), sizeof(episodesSize));
            
            // Read each episode
            for (uint64_t i = 0; i < episodesSize; ++i) {
                EpisodicMemoryItemCheckpoint episode;
                
                // Read episode data
                file.read(reinterpret_cast<char*>(&episode.timestamp), sizeof(episode.timestamp));
                
                // Read sensory state
                uint64_t sensorySize;
                file.read(reinterpret_cast<char*>(&sensorySize), sizeof(sensorySize));
                episode.sensoryState.resize(sensorySize);
                if (sensorySize > 0) {
                    file.read(reinterpret_cast<char*>(episode.sensoryState.data()),
                             sensorySize * sizeof(float));
                }
                
                // Read other fields
                file.read(reinterpret_cast<char*>(&episode.positionX), sizeof(episode.positionX));
                file.read(reinterpret_cast<char*>(&episode.positionY), sizeof(episode.positionY));
                file.read(reinterpret_cast<char*>(&episode.orientation), sizeof(episode.orientation));
                file.read(reinterpret_cast<char*>(&episode.action), sizeof(episode.action));
                file.read(reinterpret_cast<char*>(&episode.reward), sizeof(episode.reward));
                file.read(reinterpret_cast<char*>(&episode.energy), sizeof(episode.energy));
                file.read(reinterpret_cast<char*>(&episode.novelty), sizeof(episode.novelty));
                file.read(reinterpret_cast<char*>(&episode.resultingReward), sizeof(episode.resultingReward));
                
                // Read neurons and activations
                uint64_t neuronsSize;
                file.read(reinterpret_cast<char*>(&neuronsSize), sizeof(neuronsSize));
                episode.activeNeurons.resize(neuronsSize);
                if (neuronsSize > 0) {
                    file.read(reinterpret_cast<char*>(episode.activeNeurons.data()),
                             neuronsSize * sizeof(NeuronId));
                }
                
                uint64_t activationsSize;
                file.read(reinterpret_cast<char*>(&activationsSize), sizeof(activationsSize));
                episode.neuronActivations.resize(activationsSize);
                if (activationsSize > 0) {
                    file.read(reinterpret_cast<char*>(episode.neuronActivations.data()),
                             activationsSize * sizeof(float));
                }
                
                // Read other fields
                file.read(reinterpret_cast<char*>(&episode.age), sizeof(episode.age));
                file.read(reinterpret_cast<char*>(&episode.predictionError), sizeof(episode.predictionError));
                file.read(reinterpret_cast<char*>(&episode.confidence), sizeof(episode.confidence));
                file.read(reinterpret_cast<char*>(&episode.sociallyRelevant), sizeof(episode.sociallyRelevant));
                file.read(reinterpret_cast<char*>(&episode.socialReward), sizeof(episode.socialReward));
                
                checkpoint.episodes.push_back(episode);
            }
            
            // Read other checkpoint data
            file.read(reinterpret_cast<char*>(&checkpoint.maxEpisodes), sizeof(checkpoint.maxEpisodes));
            file.read(reinterpret_cast<char*>(&checkpoint.replayEnabled), sizeof(checkpoint.replayEnabled));
            file.read(reinterpret_cast<char*>(&checkpoint.consolidationEfficiency), sizeof(checkpoint.consolidationEfficiency));
            file.read(reinterpret_cast<char*>(&checkpoint.consolidationCount), sizeof(checkpoint.consolidationCount));
            
            // Read derived sizes
            uint64_t recentEpisodesSize;
            file.read(reinterpret_cast<char*>(&recentEpisodesSize), sizeof(recentEpisodesSize));
            
            uint64_t episodicBufferSize;
            file.read(reinterpret_cast<char*>(&episodicBufferSize), sizeof(episodicBufferSize));
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    NeuralEpisodicMemoryCheckpoint createEmptyCheckpoint() const override {
        NeuralEpisodicMemoryCheckpoint checkpoint;
        checkpoint.maxEpisodes = 1000;
        checkpoint.replayEnabled = true;
        checkpoint.consolidationEfficiency = 1.0f;
        return checkpoint;
    }
    
    bool convertToMemoryItem(const EpisodicMemoryItemCheckpoint& checkpoint,
                            EpisodicMemoryItem& item) const override {
        try {
            item.timestamp = checkpoint.timestamp;
            item.sensoryState = checkpoint.sensoryState;
            item.positionX = checkpoint.positionX;
            item.positionY = checkpoint.positionY;
            item.orientation = checkpoint.orientation;
            item.action = checkpoint.action;
            item.reward = checkpoint.reward;
            item.energy = checkpoint.energy;
            item.novelty = checkpoint.novelty;
            item.resultingSensoryState = checkpoint.resultingSensoryState;
            item.resultingReward = checkpoint.resultingReward;
            item.activeNeurons = checkpoint.activeNeurons;
            item.neuronActivations = checkpoint.neuronActivations;
            item.age = checkpoint.age;
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
};

NeuralEpisodicMemorySaverLoader& getEpisodicMemorySaverLoader() {
    static NeuralEpisodicMemorySaverLoaderImpl instance;
    return instance;
}

} // namespace

// NeuralEpisodicMemory save/load functions
bool NeuralEpisodicMemory::save(NeuralEpisodicMemoryCheckpoint& checkpoint) {
    try {
        // Create checkpoint from current state
        checkpoint.episodes.clear();
        checkpoint.recentEpisodes.clear();
        checkpoint.episodicBuffer.clear();
        
        // Copy all episodes
        for (const auto& episode : episodes_) {
            EpisodicMemoryItemCheckpoint epCheckpoint;
            
            epCheckpoint.timestamp = episode.timestamp;
            epCheckpoint.sensoryState = episode.sensoryState;
            epCheckpoint.positionX = episode.positionX;
            epCheckpoint.positionY = episode.positionY;
            epCheckpoint.orientation = episode.orientation;
            epCheckpoint.action = episode.action;
            epCheckpoint.reward = episode.reward;
            epCheckpoint.energy = episode.energy;
            epCheckpoint.novelty = episode.novelty;
            epCheckpoint.resultingSensoryState = episode.resultingSensoryState;
            epCheckpoint.resultingReward = episode.resultingReward;
            epCheckpoint.activeNeurons = episode.activeNeurons;
            epCheckpoint.neuronActivations = episode.neuronActivations;
            epCheckpoint.age = episode.age;
            
            // Integration fields (would be computed from other systems)
            epCheckpoint.predictionError = 0.0f;
            epCheckpoint.confidence = 1.0f;
            epCheckpoint.sociallyRelevant = false;
            epCheckpoint.socialReward = 0.0f;
            
            checkpoint.episodes.push_back(epCheckpoint);
        }
        
        // Copy parameters
        checkpoint.maxEpisodes = maxEpisodes_;
        checkpoint.replayEnabled = replayEnabled_;
        checkpoint.consolidationEfficiency = 1.0f;
        checkpoint.consolidationCount = 0;
        
        return getEpisodicMemorySaverLoader().save(checkpoint);
    } catch (const std::exception& e) {
        return false;
    }
}

bool NeuralEpisodicMemory::load(const NeuralEpisodicMemoryCheckpoint& checkpoint) {
    try {
        // Clear current state
        clear();
        
        // Convert checkpoint episodes to memory items
        for (const auto& epCheckpoint : checkpoint.episodes) {
            EpisodicMemoryItem episode;
            
            if (getEpisodicMemorySaverLoader().convertToMemoryItem(epCheckpoint, episode)) {
                episodes_.push_back(episode);
            }
        }
        
        // Set parameters
        maxEpisodes_ = checkpoint.maxEpisodes;
        replayEnabled_ = checkpoint.replayEnabled;
        
        return getEpisodicMemorySaverLoader().load(const_cast<NeuralEpisodicMemoryCheckpoint&>(checkpoint));
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace nlm
