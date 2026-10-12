#include "MemoryVisualizer.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

struct MemoryVisualizer::Impl {
    // Memory data
    std::vector<WorkingMemoryActivation> currentWorkingMemories;
    std::deque<WorkingMemoryActivation> workingMemoryHistory;
    size_t workingMemoryHistorySize;
    
    std::vector<EpisodicEpisode> currentEpisodicEpisodes;
    std::deque<EpisodicEpisode> episodicMemoryHistory;
    size_t episodicMemoryHistorySize;
    
    std::vector<AssociativeMemory> currentAssociativeMemories;
    std::deque<AssociativeMemory> associativeMemoryHistory;
    size_t associativeMemoryHistorySize;
    
    MemoryStatistics currentMemoryStats;
    
    // Configuration
    bool autoUpdate;
    
    // Timing
    Timestamp lastTimestamp;
    bool initialized;
    
    Impl() : workingMemoryHistorySize(1000), episodicMemoryHistorySize(100), 
             associativeMemoryHistorySize(1000), autoUpdate(true),
             lastTimestamp(0), initialized(false) {}
};

MemoryVisualizer::MemoryVisualizer() : pImpl(std::make_unique<Impl>()) {}
MemoryVisualizer::~MemoryVisualizer() = default;

bool MemoryVisualizer::initialize() {
    try {
        pImpl->initialized = true;
        Logger::getInstance().logInfo("Memory visualizer initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize memory visualizer: " + std::string(e.what()));
        return false;
    }
}

void MemoryVisualizer::update(double deltaTime) {
    if (!pImpl->initialized) return;
    
    pImpl->lastTimestamp += static_cast<Timestamp>(deltaTime * 1000.0);
    
    // Clean old history data
    while (pImpl->workingMemoryHistory.size() > pImpl->workingMemoryHistorySize) {
        pImpl->workingMemoryHistory.pop_front();
    }
    while (pImpl->episodicMemoryHistory.size() > pImpl->episodicMemoryHistorySize) {
        pImpl->episodicMemoryHistory.pop_front();
    }
    while (pImpl->associativeMemoryHistory.size() > pImpl->associativeMemoryHistorySize) {
        pImpl->associativeMemoryHistory.pop_front();
    }
    
    // Update current data
    pImpl->currentWorkingMemories.assign(pImpl->workingMemoryHistory.begin(), pImpl->workingMemoryHistory.end());
    pImpl->currentEpisodicEpisodes.assign(pImpl->episodicMemoryHistory.begin(), pImpl->episodicMemoryHistory.end());
    pImpl->currentAssociativeMemories.assign(pImpl->associativeMemoryHistory.begin(), pImpl->associativeMemoryHistory.end());
}

void MemoryVisualizer::recordWorkingMemoryActivation(const WorkingMemoryActivation& activation) {
    if (pImpl->initialized) {
        pImpl->workingMemoryHistory.push_back(activation);
    }
}

void MemoryVisualizer::recordWorkingMemoryActivations(const std::vector<WorkingMemoryActivation>& activations) {
    for (const auto& activation : activations) {
        recordWorkingMemoryActivation(activation);
    }
}

void MemoryVisualizer::recordEpisodicEpisode(const EpisodicEpisode& episode) {
    if (pImpl->initialized) {
        pImpl->episodicMemoryHistory.push_back(episode);
    }
}

void MemoryVisualizer::recordEpisodicEpisodes(const std::vector<EpisodicEpisode>& episodes) {
    for (const auto& episode : episodes) {
        recordEpisodicEpisode(episode);
    }
}

void MemoryVisualizer::recordAssociativeMemory(const AssociativeMemory& memory) {
    if (pImpl->initialized) {
        pImpl->associativeMemoryHistory.push_back(memory);
    }
}

void MemoryVisualizer::recordAssociativeMemories(const std::vector<AssociativeMemory>& memories) {
    for (const auto& memory : memories) {
        recordAssociativeMemory(memory);
    }
}

void MemoryVisualizer::updateMemoryStatistics(const MemoryStatistics& stats) {
    pImpl->currentMemoryStats = stats;
}

void MemoryVisualizer::renderWorkingMemory(const std::vector<WorkingMemoryActivation>& activations) {
    std::cout << "=== Working Memory Activations ===" << std::endl;
    
    if (activations.empty()) {
        std::cout << "No working memory activations recorded." << std::endl;
        return;
    }
    
    for (const auto& activation : activations) {
        std::cout << "Memory ID: " << activation.id << ", "
                  << "Content: " << activation.content << ", "
                  << "Activation: " << activation.activationLevel << ", "
                  << "Stability: " << activation.stability << ", "
                  << "Origin: " << activation.originRegion << ", "
                  << "Active Neurons: " << activation.activeNeurons.size() << std::endl;
    }
}

void MemoryVisualizer::renderEpisodicMemory(const std::vector<EpisodicEpisode>& episodes) {
    std::cout << "=== Episodic Memory Episodes ===" << std::endl;
    
    if (episodes.empty()) {
        std::cout << "No episodic memory episodes recorded." << std::endl;
        return;
    }
    
    for (const auto& episode : episodes) {
        std::cout << "Episode ID: " << episode.id << ", "
                  << "Description: " << episode.description << ", "
                  << "Importance: " << episode.importance << ", "
                  << "Strength: " << episode.strength << ", "
                  << "Consolidation: " << episode.consolidationLevel << ", "
                  << "Recall Count: " << episode.recallCount << std::endl;
        if (!episode.firstRecallTime) {
            std::cout << "  First Recall: Not yet" << std::endl;
        } else {
            std::cout << "  First Recall: " << episode.firstRecallTime << std::endl;
        }
    }
}

void MemoryVisualizer::renderAssociativeMemory(const std::vector<AssociativeMemory>& memories) {
    std::cout << "=== Associative Memory ===" << std::endl;
    
    if (memories.empty()) {
        std::cout << "No associative memories recorded." << std::endl;
        return;
    }
    
    for (const auto& memory : memories) {
        std::cout << "Memory ID: " << memory.id << ", "
                  << "Key: " << memory.key << ", "
                  << "Value: " << memory.value << ", "
                  << "Strength: " << memory.strength << ", "
                  << "Access Count: " << memory.accessCount << ", "
                  << "Last Accessed: " << memory.lastAccessed << std::endl;
    }
}

void MemoryVisualizer::renderMemoryStatistics(const MemoryStatistics& stats) {
    std::cout << "=== Memory System Statistics ===" << std::endl;
    std::cout << "Working Memory Utilization: " << stats.workingMemoryUtilization << std::endl;
    std::cout << "Episodic Memory Consolidation: " << stats.episodicMemoryConsolidation << std::endl;
    std::cout << "Associative Memory Activation: " << stats.associativeMemoryActivation << std::endl;
    std::cout << "Total Working Memories: " << stats.totalWorkingMemories << std::endl;
    std::cout << "Total Episodic Episodes: " << stats.totalEpisodicEpisodes << std::endl;
    std::cout << "Total Associative Memories: " << stats.totalAssociativeMemories << std::endl;
    std::cout << "Average Memory Stability: " << stats.averageMemoryStability << std::endl;
    std::cout << "Memory Cohesion: " << stats.memoryCohesion << std::endl;
    
    std::cout << "Region Contributions:" << std::endl;
    for (const auto& [regionId, contribution] : stats.regionMemoryContribution) {
        std::cout << "  Region " << regionId << ": " << contribution << std::endl;
    }
}

void MemoryVisualizer::setWorkingMemoryHistorySize(size_t size) {
    pImpl->workingMemoryHistorySize = size;
    while (pImpl->workingMemoryHistory.size() > size) {
        pImpl->workingMemoryHistory.pop_front();
    }
}

void MemoryVisualizer::setEpisodicMemoryHistorySize(size_t size) {
    pImpl->episodicMemoryHistorySize = size;
    while (pImpl->episodicMemoryHistory.size() > size) {
        pImpl->episodicMemoryHistory.pop_front();
    }
}

void MemoryVisualizer::setAssociativeMemoryHistorySize(size_t size) {
    pImpl->associativeMemoryHistorySize = size;
    while (pImpl->associativeMemoryHistory.size() > size) {
        pImpl->associativeMemoryHistory.pop_front();
    }
}

void MemoryVisualizer::setAutoUpdate(bool autoUpdate) {
    pImpl->autoUpdate = autoUpdate;
}

float MemoryVisualizer::getWorkingMemoryUtilization() const {
    return pImpl->currentMemoryStats.workingMemoryUtilization;
}

float MemoryVisualizer::getEpisodicMemoryConsolidation() const {
    return pImpl->currentMemoryStats.episodicMemoryConsolidation;
}

float MemoryVisualizer::getAssociativeMemoryActivation() const {
    return pImpl->currentMemoryStats.associativeMemoryActivation;
}

uint32_t MemoryVisualizer::getTotalMemoryItems() const {
    return pImpl->currentMemoryStats.totalWorkingMemories + 
           pImpl->currentMemoryStats.totalEpisodicEpisodes +
           pImpl->currentMemoryStats.totalAssociativeMemories;
}

float MemoryVisualizer::getMemoryCohesion() const {
    return pImpl->currentMemoryStats.memoryCohesion;
}

bool MemoryVisualizer::exportWorkingMemoryData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "WorkingMemoryData:Timestamp,MemoryId,Content,ActivationLevel,Stability,DecayRate,OriginRegion,ActiveNeurons\n";
    for (const auto& activation : pImpl->workingMemoryHistory) {
        file << activation.timestamp << "," << activation.id << "," 
             << "\"" << activation.content << "\"" << "," << activation.activationLevel << "," 
             << activation.stability << "," << activation.decayRate << "," 
             << activation.originRegion << ",";
        
        for (size_t i = 0; i < activation.activeNeurons.size(); ++i) {
            if (i > 0) file << ";";
            file << activation.activeNeurons[i];
        }
        file << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Working memory data exported to: " + filename);
    return true;
}

bool MemoryVisualizer::exportEpisodicMemoryData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "EpisodicMemoryData:Timestamp,EpisodeId,Description,Importance,Strength,RelatedMemories,EncodingNeurons,RecallNeurons,FirstRecallTime,LastRecallTime,RecallCount,ConsolidationLevel,Context\n";
    for (const auto& episode : pImpl->episodicMemoryHistory) {
        file << episode.timestamp << "," << episode.id << "," 
             << "\"" << episode.description << "\"" << "," << episode.importance << "," 
             << episode.strength << ",";
        
        for (size_t i = 0; i < episode.relatedMemories.size(); ++i) {
            if (i > 0) file << ";";
            file << episode.relatedMemories[i];
        }
        file << ",";
        
        for (size_t i = 0; i < episode.encodingNeurons.size(); ++i) {
            if (i > 0) file << ";";
            file << episode.encodingNeurons[i];
        }
        file << ",";
        
        for (size_t i = 0; i < episode.recallNeurons.size(); ++i) {
            if (i > 0) file << ";";
            file << episode.recallNeurons[i];
        }
        file << "," << episode.firstRecallTime << "," << episode.lastRecallTime << "," 
             << episode.recallCount << "," << episode.consolidationLevel << "," 
             << "\"" << episode.context << "\"" << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Episodic memory data exported to: " + filename);
    return true;
}

bool MemoryVisualizer::exportAssociativeMemoryData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "AssociativeMemoryData:MemoryId,Key,Value,Strength,Elasticity,ActivationThreshold,KeyNeurons,ValueNeurons,AccessCount,LastAccessed,DecayFactor\n";
    for (const auto& memory : pImpl->associativeMemoryHistory) {
        file << memory.id << "," << memory.key << "," << memory.value << "," 
             << memory.strength << "," << memory.elasticity << "," 
             << memory.activationThreshold << ",";
        
        for (size_t i = 0; i < memory.keyNeurons.size(); ++i) {
            if (i > 0) file << ";";
            file << memory.keyNeurons[i];
        }
        file << ",";
        
        for (size_t i = 0; i < memory.valueNeurons.size(); ++i) {
            if (i > 0) file << ";";
            file << memory.valueNeurons[i];
        }
        file << "," << memory.accessCount << "," << memory.lastAccessed << "," 
             << memory.decayFactor << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Associative memory data exported to: " + filename);
    return true;
}

bool MemoryVisualizer::exportMemoryStatistics(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "MemoryStatistics:Timestamp,WorkingMemoryUtilization,EpisodicMemoryConsolidation,AssociativeMemoryActivation,TotalWorkingMemories,TotalEpisodicEpisodes,TotalAssociativeMemories,AverageMemoryStability,MemoryCohesion\n";
    file << pImpl->currentMemoryStats.timestamp << "," << pImpl->currentMemoryStats.workingMemoryUtilization << "," 
         << pImpl->currentMemoryStats.episodicMemoryConsolidation << "," << pImpl->currentMemoryStats.associativeMemoryActivation << "," 
         << pImpl->currentMemoryStats.totalWorkingMemories << "," << pImpl->currentMemoryStats.totalEpisodicEpisodes << "," 
         << pImpl->currentMemoryStats.totalAssociativeMemories << "," << pImpl->currentMemoryStats.averageMemoryStability << "," 
         << pImpl->currentMemoryStats.memoryCohesion << "\n";
    
    file << "\nRegionContributionData:Timestamp,RegionId,Contribution\n";
    for (const auto& [regionId, contribution] : pImpl->currentMemoryStats.regionMemoryContribution) {
        file << pImpl->currentMemoryStats.timestamp << "," << regionId << "," << contribution << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Memory statistics exported to: " + filename);
    return true;
}

} // namespace nlm