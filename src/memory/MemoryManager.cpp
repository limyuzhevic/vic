#include "MemoryManager.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

// MemoryManager implementation
struct MemoryManager::Impl {
    Brain* brain = nullptr;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Record keeping
    std::vector<float> lastRecordedEpisodicActivity;
};

MemoryManager::MemoryManager()
    : pImpl_(new Impl)
    , consolidationInterval_(1000)
    , replayInterval_(100)
    , episodicMemoryInterval_(10)
    , stepsSinceLastEpisodic_(0)
    , stepsSinceLastConsolidation_(0)
    , stepsSinceLastReplay_(0)
    , workingMemoryEnabled_(true)
    , episodicMemoryEnabled_(true)
    , replayEnabled_(true)
    , consolidationEnabled_(true)
{
}

MemoryManager::~MemoryManager() = default;

void MemoryManager::initialize(Brain* brain,
                              std::unique_ptr<NeuralWorkingMemory> workingMemory,
                              std::unique_ptr<NeuralEpisodicMemory> episodicMemory,
                              std::unique_ptr<NeuralAssociativeMemory> associativeMemory) {
    if (!brain) {
        NLM_LOG_ERROR("MemoryManager: Cannot initialize with null brain pointer");
        return;
    }
    
    pImpl_->brain = brain;
    pImpl_->workingMemory = std::move(workingMemory);
    pImpl_->episodicMemory = std::move(episodicMemory);
    pImpl_->associativeMemory = std::move(associativeMemory);
    
    // Initialize memory systems if they exist
    if (pImpl_->workingMemory) {
        pImpl_->workingMemory->initialize(brain);
    }
    
    if (pImpl_->episodicMemory) {
        pImpl_->episodicMemory->initialize(brain);
    }
    
    if (pImpl_->associativeMemory) {
        pImpl_->associativeMemory->initialize(brain);
    }
    
    NLM_LOG_INFO("MemoryManager initialized successfully");
}

void MemoryManager::update(SimulationStep currentStep, Timestamp currentTime) {
    // Update all memory systems
    if (workingMemoryEnabled_ && pImpl_->workingMemory) {
        updateWorkingMemory();
    }
    
    if (episodicMemoryEnabled_ && pImpl_->episodicMemory) {
        updateEpisodicMemory(currentStep);
    }
    
    if (replayEnabled_ && pImpl_->episodicMemory) {
        handleReplay(currentStep);
    }
    
    if (consolidationEnabled_ && pImpl_->episodicMemory) {
        handleConsolidation(currentStep);
    }
    
    // Update statistics
    updateStatistics();
}

void MemoryManager::updateWorkingMemory() {
    if (!pImpl_->brain || !pImpl_->workingMemory) return;
    
    // Perform maintenance and competition
    performMemoryMaintenance();
    runMemoryCompetition();
    
    // Update working memory state
    if (pImpl_->brain) {
        // Access timestep from brain's pImpl
        for (auto& region : pImpl_->brain->getRegions()) {
            // For now, just use a default timestep for working memory update
            // In a full implementation, we would access the timestep from brain's pImpl
            pImpl_->workingMemory->update(0.001);
            break;
        }
    }
    
    stats_.totalMemoryMaintenance++;
}

void MemoryManager::updateEpisodicMemory(SimulationStep currentStep) {
    if (!pImpl_->episodicMemory) return;
    
    // Update episodic memory interval counter
    stepsSinceLastEpisodic_++;
    
    if (stepsSinceLastEpisodic_ >= episodicMemoryInterval_) {
        // Store current brain state as an episode
        recordEpisodicExperience(currentStep);
        stats_.lastEpisodicStep = currentStep;
        stepsSinceLastEpisodic_ = 0;
        stats_.episodicMemoryCount = pImpl_->episodicMemory->getEpisodeCount();
    }
}

void MemoryManager::handleConsolidation(SimulationStep currentStep) {
    if (!pImpl_->episodicMemory) return;
    
    // Check if consolidation is needed
    stepsSinceLastConsolidation_++;
    
    if (stepsSinceLastConsolidation_ >= consolidationInterval_) {
        pImpl_->episodicMemory->consolidate(0.3f);
        stats_.consolidationCount++;
        stats_.lastConsolidationStep = currentStep;
        stepsSinceLastConsolidation_ = 0;
        
        NLM_LOG_INFO("MemoryManager: Completed memory consolidation");
    }
}

void MemoryManager::handleReplay(SimulationStep currentStep) {
    if (!pImpl_->episodicMemory) return;
    
    // Check if replay is needed
    stepsSinceLastReplay_++;
    
    if (stepsSinceLastReplay_ >= replayInterval_) {
        auto episodesToReplay = pImpl_->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl_->episodicMemory->replayEpisode(episode);
        }
        
        stats_.replayCount++;
        stats_.lastReplayStep = currentStep;
        stepsSinceLastReplay_ = 0;
        
        NLM_LOG_INFO("MemoryManager: Completed memory replay");
    }
}

void MemoryManager::updateStatistics() {
    if (!pImpl_->brain || 
        !pImpl_->workingMemory || 
        !pImpl_->episodicMemory) {
        return;
    }
    
    // Update working memory statistics
    stats_.workingMemoryNeurons = pImpl_->workingMemory->getMemoryNeurons().size();
    stats_.workingMemoryTraces = pImpl_->workingMemory->getActiveTraces();
    stats_.workingMemoryActivity = pImpl_->workingMemory->getMemoryActivity();
    
    // Update episodic memory statistics
    stats_.episodicMemoryCount = pImpl_->episodicMemory->getEpisodeCount();
    stats_.averageEpisodicReward = pImpl_->episodicMemory->getAverageReward();
}

std::string MemoryManager::getMemoryReport() const {
    std::stringstream ss;
    ss << "=== Memory System Statistics ===\n";
    ss << std::fixed << std::setprecision(2);
    
    ss << "Working Memory:\n";
    ss << "  Neurons in memory: " << stats_.workingMemoryNeurons << "\n";
    ss << "  Active traces: " << stats_.workingMemoryTraces << "\n";
    ss << "  Memory activity: " << stats_.workingMemoryActivity << "\n";
    
    ss << "\nEpisodic Memory:\n";
    ss << "  Episode count: " << stats_.episodicMemoryCount << "\n";
    ss << "  Average reward: " << stats_.averageEpisodicReward << "\n";
    ss << "  Last episodic step: " << stats_.lastEpisodicStep << "\n";
    
    ss << "\nAssociative Memory:\n";
    ss << "  Association count: " << stats_.associativeMemoryCount << "\n";
    
    ss << "\nReplay and Consolidation:\n";
    ss << "  Replay count: " << stats_.replayCount << "\n";
    ss << "  Consolidation count: " << stats_.consolidationCount << "\n";
    ss << "  Last replay step: " << stats_.lastReplayStep << "\n";
    ss << "  Last consolidation step: " << stats_.lastConsolidationStep << "\n";
    
    ss << "\nMemory Operations:\n";
    ss << "  Maintenance operations: " << stats_.totalMemoryMaintenance << "\n";
    ss << "  Competition operations: " << stats_.totalCompetition << "\n";
    
    ss << "\n=== End Statistics ===";
    return ss.str();
}

void MemoryManager::forceConsolidation(float threshold) {
    if (pImpl_->episodicMemory) {
        pImpl_->episodicMemory->consolidate(threshold);
        stats_.consolidationCount++;
        stepsSinceLastConsolidation_ = 0;
        
        NLM_LOG_INFO("MemoryManager: Force consolidation completed");
    }
}

void MemoryManager::forceReplay(size_t count) {
    if (pImpl_->episodicMemory) {
        auto episodesToReplay = pImpl_->episodicMemory->getEpisodesForReplay(count);
        for (const auto* episode : episodesToReplay) {
            pImpl_->episodicMemory->replayEpisode(episode);
        }
        
        stats_.replayCount++;
        stepsSinceLastReplay_ = 0;
        
        NLM_LOG_INFO("MemoryManager: Force replay completed");
    }
}

void MemoryManager::clear() {
    if (pImpl_->workingMemory) pImpl_->workingMemory->clear();
    if (pImpl_->episodicMemory) pImpl_->episodicMemory->clear();
    if (pImpl_->associativeMemory) pImpl_->associativeMemory->clear();
    
    // Reset statistics
    stats_ = MemoryStats();
    stepsSinceLastEpisodic_ = 0;
    stepsSinceLastConsolidation_ = 0;
    stepsSinceLastReplay_ = 0;
    
    NLM_LOG_INFO("MemoryManager: All memories cleared");
}

void MemoryManager::createEpisodicMemoryItem(SimulationStep currentStep, float reward) {
    // This would create an EpisodicMemoryItem with current brain state
    // For now, it's a placeholder for future implementation
}

void MemoryManager::recordEpisodicExperience(SimulationStep currentStep) {
    if (!pImpl_->episodicMemory) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = reward;
    
    // Capture active neurons
    for (const auto& region : pImpl_->brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
        }
    }
    
    pImpl_->episodicMemory->storeEpisode(episode);
}

void MemoryManager::performMemoryMaintenance() {
    stats_.totalMemoryMaintenance++;
    
    if (!pImpl_->brain || !pImpl_->workingMemory) return;
    
    // Perform memory maintenance operations
    // This would include things like rehearsal, decay, etc.
    
    // Rehearse important memories
    pImpl_->workingMemory->strengthenMemory(1.0f);
}

void MemoryManager::runMemoryCompetition() {
    stats_.totalCompetition++;
    
    if (!pImpl_->brain || !pImpl_->workingMemory) return;
    
    // Run competition between memory traces
    pImpl_->workingMemory->runCompetition();
    
    // Apply attention to working memory winners
    if (!pImpl_->workingMemory->getMemoryNeurons().empty()) {
        auto competitors = pImpl_->workingMemory->getMemoryNeurons();
        // Note: In a real implementation, this would access the attention system
        // For now, we'll just log that competition occurred
    }
}

} // namespace nlm
