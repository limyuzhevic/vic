#include "DevelopmentVisualizer.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

struct DevelopmentVisualizer::Impl {
    // Developmental stages
    std::vector<DevelopmentalStage> currentDevelopmentalStages;
    std::deque<DevelopmentalStage> developmentalStageHistory;
    size_t stageHistorySize;
    
    // Structural plasticity events
    std::vector<StructuralPlasticityEvent> currentPlasticityEvents;
    std::deque<StructuralPlasticityEvent> plasticityEventHistory;
    size_t plasticityEventHistorySize;
    
    // Developmental metrics
    DevelopmentalMetrics currentDevelopmentalMetrics;
    
    // Configuration
    bool autoUpdate;
    
    // Timing
    Timestamp lastTimestamp;
    bool initialized;
    
    Impl() : stageHistorySize(100), plasticityEventHistorySize(200), autoUpdate(true),
             lastTimestamp(0), initialized(false) {}
};

DevelopmentVisualizer::DevelopmentVisualizer() : pImpl(std::make_unique<Impl>()) {}
DevelopmentVisualizer::~DevelopmentVisualizer() = default;

bool DevelopmentVisualizer::initialize() {
    try {
        pImpl->initialized = true;
        Logger::getInstance().logInfo("Development visualizer initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize development visualizer: " + std::string(e.what()));
        return false;
    }
}

void DevelopmentVisualizer::update(double deltaTime) {
    if (!pImpl->initialized) return;
    
    pImpl->lastTimestamp += static_cast<Timestamp>(deltaTime * 1000.0);
    
    // Clean old history data
    while (pImpl->developmentalStageHistory.size() > pImpl->stageHistorySize) {
        pImpl->developmentalStageHistory.pop_front();
    }
    while (pImpl->plasticityEventHistory.size() > pImpl->plasticityEventHistorySize) {
        pImpl->plasticityEventHistory.pop_front();
    }
    
    // Update current data
    pImpl->currentDevelopmentalStages.assign(pImpl->developmentalStageHistory.begin(), pImpl->developmentalStageHistory.end());
    pImpl->currentPlasticityEvents.assign(pImpl->plasticityEventHistory.begin(), pImpl->plasticityEventHistory.end());
}

void DevelopmentVisualizer::recordDevelopmentalStage(const DevelopmentalStage& stage) {
    if (pImpl->initialized) {
        pImpl->developmentalStageHistory.push_back(stage);
    }
}

void DevelopmentVisualizer::recordDevelopmentalStages(const std::vector<DevelopmentalStage>& stages) {
    for (const auto& stage : stages) {
        recordDevelopmentalStage(stage);
    }
}

void DevelopmentVisualizer::recordStructuralPlasticityEvent(const StructuralPlasticityEvent& event) {
    if (pImpl->initialized) {
        pImpl->plasticityEventHistory.push_back(event);
    }
}

void DevelopmentVisualizer::recordStructuralPlasticityEvents(const std::vector<StructuralPlasticityEvent>& events) {
    for (const auto& event : events) {
        recordStructuralPlasticityEvent(event);
    }
}

void DevelopmentVisualizer::updateDevelopmentalMetrics(const DevelopmentalMetrics& metrics) {
    pImpl->currentDevelopmentalMetrics = metrics;
}

void DevelopmentVisualizer::renderDevelopmentalStages(const std::vector<DevelopmentalStage>& stages) {
    std::cout << "=== Developmental Stages ===" << std::endl;
    
    if (stages.empty()) {
        std::cout << "No developmental stages recorded." << std::endl;
        return;
    }
    
    for (const auto& stage : stages) {
        std::cout << "Stage: " << static_cast<int>(stage.stage) << ", "
                  << "Progression: " << stage.progression << ", "
                  << "Description: " << stage.description << std::endl;
        std::cout << "  Milestones: ";
        for (size_t i = 0; i < stage.milestones.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << stage.milestones[i];
        }
        std::cout << std::endl;
        std::cout << "  Learning Rate: " << stage.globalLearningRate << ", "
                  << "Stability: " << stage.synapticStability << ", "
                  << "Memory Capacity: " << stage.memoryCapacity << std::endl;
        std::cout << "  Regional Development: ";
        for (const auto& [regionId, dev] : stage.regionalDevelopment) {
            std::cout << "Region " << regionId << "=" << dev << ", ";
        }
        std::cout << std::endl;
        std::cout << "  Active Processes: ";
        for (size_t i = 0; i < stage.activeProcesses.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << stage.activeProcesses[i];
        }
        std::cout << std::endl;
    }
}

void DevelopmentVisualizer::renderStructuralPlasticity(const std::vector<StructuralPlasticityEvent>& events) {
    std::cout << "=== Structural Plasticity Events ===" << std::endl;
    
    if (events.empty()) {
        std::cout << "No structural plasticity events recorded." << std::endl;
        return;
    }
    
    for (const auto& event : events) {
        std::cout << "Event: " << static_cast<int>(event.type) << ", "
                  << "Description: " << event.description << ", "
                  << "Magnitude: " << event.magnitude << ", "
                  << "Efficiency Gain: " << event.efficiencyGain << ", "
                  << "Completed: " << event.isCompleted << std::endl;
        std::cout << "  Source: Region " << event.sourceRegion << ", "
                  << "Target: Region " << event.targetRegion << std::endl;
        std::cout << "  Involved Neurons: ";
        for (size_t i = 0; i < event.involvedNeurons.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << event.involvedNeurons[i];
        }
        std::cout << std::endl;
        std::cout << "  Influencing Factors: " << event.influencingFactors << std::endl;
    }
}

void DevelopmentVisualizer::renderDevelopmentalMetrics(const DevelopmentalMetrics& metrics) {
    std::cout << "=== Developmental Metrics ===" << std::endl;
    std::cout << "Average Maturation Level: " << metrics.averageMaturationLevel << std::endl;
    std::cout << "Synaptogenesis Activity: " << metrics.averageSynaptogenesisActivity << std::endl;
    std::cout << "Pruning Activity: " << metrics.averagePruningActivity << std::endl;
    std::cout << "Connections Created: " << metrics.newConnectionsCreated << std::endl;
    std::cout << "Connections Eliminated: " << metrics.connectionsEliminated << std::endl;
    std::cout << "Total Structural Change: " << metrics.totalStructuralChange << std::endl;
    std::cout << "Network Efficiency: " << metrics.networkEfficiency << std::endl;
    std::cout << "Learning Speed: " << metrics.learningSpeed << std::endl;
    std::cout << "Memory Retention: " << metrics.memoryRetention << std::endl;
    std::cout << "Adaptability: " << metrics.adaptability << std::endl;
    
    std::cout << "Regional Efficiency:" << std::endl;
    for (const auto& [regionId, efficiency] : metrics.regionalEfficiency) {
        std::cout << "  Region " << regionId << ": " << efficiency << std::endl;
    }
    
    std::cout << "Regional Learning Rates:" << std::endl;
    for (const auto& [regionId, rate] : metrics.regionalLearningRate) {
        std::cout << "  Region " << regionId << ": " << rate << std::endl;
    }
    
    std::cout << "Active Developmental Processes:" << std::endl;
    for (const auto& process : metrics.activeDevelopmentalProcesses) {
        std::cout << "  " << process << std::endl;
    }
}

void DevelopmentVisualizer::setStageHistorySize(size_t size) {
    pImpl->stageHistorySize = size;
    while (pImpl->developmentalStageHistory.size() > size) {
        pImpl->developmentalStageHistory.pop_front();
    }
}

void DevelopmentVisualizer::setPlasticityEventHistorySize(size_t size) {
    pImpl->plasticityEventHistorySize = size;
    while (pImpl->plasticityEventHistory.size() > size) {
        pImpl->plasticityEventHistory.pop_front();
    }
}

void DevelopmentVisualizer::setAutoUpdate(bool autoUpdate) {
    pImpl->autoUpdate = autoUpdate;
}

float DevelopmentVisualizer::getAverageMaturationLevel() const {
    if (pImpl->currentDevelopmentalStages.empty()) return 0.0f;
    
    float totalLevel = 0.0f;
    for (const auto& stage : pImpl->currentDevelopmentalStages) {
        totalLevel += stage.progression;
    }
    
    return totalLevel / pImpl->currentDevelopmentalStages.size();
}

float DevelopmentVisualizer::getCurrentSynaptogenesisActivity() const {
    return pImpl->currentDevelopmentalMetrics.averageSynaptogenesisActivity;
}

float DevelopmentVisualizer::getCurrentPruningActivity() const {
    return pImpl->currentDevelopmentalMetrics.averagePruningActivity;
}

uint32_t DevelopmentVisualizer::getTotalConnectionsCreated() const {
    return pImpl->currentDevelopmentalMetrics.newConnectionsCreated;
}

uint32_t DevelopmentVisualizer::getTotalConnectionsEliminated() const {
    return pImpl->currentDevelopmentalMetrics.connectionsEliminated;
}

float DevelopmentVisualizer::getAverageEfficiency() const {
    if (pImpl->currentDevelopmentalStages.empty()) return 0.0f;
    
    float totalEfficiency = 0.0f;
    for (const auto& [regionId, efficiency] : pImpl->currentDevelopmentalMetrics.regionalEfficiency) {
        totalEfficiency += efficiency;
    }
    
    return totalEfficiency / pImpl->currentDevelopmentalMetrics.regionalEfficiency.size();
}

float DevelopmentVisualizer::getLearningSpeed() const {
    return pImpl->currentDevelopmentalMetrics.learningSpeed;
}

float DevelopmentVisualizer::getAdaptability() const {
    return pImpl->currentDevelopmentalMetrics.adaptability;
}

bool DevelopmentVisualizer::exportDevelopmentalStageData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "DevelopmentalStageData:Timestamp,Stage,Progression,Description,Milestones,GlobalLearningRate,SynapticStability,MemoryCapacity,ExplorationDrive\n";
    for (const auto& stage : pImpl->developmentalStageHistory) {
        file << stage.timestamp << "," << static_cast<int>(stage.stage) << "," 
             << stage.progression << "," << "\"" << stage.description << "\"" << ",";
        
        for (size_t i = 0; i < stage.milestones.size(); ++i) {
            if (i > 0) file << ";";
            file << "\"" << stage.milestones[i] << "\"";
        }
        file << "," << stage.globalLearningRate << "," << stage.synapticStability << "," 
             << stage.memoryCapacity << "," << stage.explorationDrive << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Developmental stage data exported to: " + filename);
    return true;
}

bool DevelopmentVisualizer::exportStructuralPlasticityData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "StructuralPlasticityData:Timestamp,Type,Description,SourceRegion,TargetRegion,Magnitude,EfficiencyGain,IsCompleted,InfluencingFactors\n";
    for (const auto& event : pImpl->plasticityEventHistory) {
        file << event.timestamp << "," << static_cast<int>(event.type) << "," 
             << "\"" << event.description << "\"" << "," << event.sourceRegion << "," 
             << event.targetRegion << "," << event.magnitude << "," 
             << event.efficiencyGain << "," << event.isCompleted << "," 
             << "\"" << event.influencingFactors << "\"" << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Structural plasticity data exported to: " + filename);
    return true;
}

bool DevelopmentVisualizer::exportDevelopmentalMetrics(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "DevelopmentalMetricsData:Timestamp,AverageMaturationLevel,AverageSynaptogenesisActivity,AveragePruningActivity,NewConnectionsCreated,ConnectionsEliminated,TotalStructuralChange,NetworkEfficiency,LearningSpeed,MemoryRetention,Adaptability\n";
    file << pImpl->currentDevelopmentalMetrics.timestamp << "," << pImpl->currentDevelopmentalMetrics.averageMaturationLevel << "," 
         << pImpl->currentDevelopmentalMetrics.averageSynaptogenesisActivity << "," << pImpl->currentDevelopmentalMetrics.averagePruningActivity << "," 
         << pImpl->currentDevelopmentalMetrics.newConnectionsCreated << "," << pImpl->currentDevelopmentalMetrics.connectionsEliminated << "," 
         << pImpl->currentDevelopmentalMetrics.totalStructuralChange << "," << pImpl->currentDevelopmentalMetrics.networkEfficiency << "," 
         << pImpl->currentDevelopmentalMetrics.learningSpeed << "," << pImpl->currentDevelopmentalMetrics.memoryRetention << "," 
         << pImpl->currentDevelopmentalMetrics.adaptability << "\n";
    
    file << "\nRegionalEfficiencyData:Timestamp,RegionId,Efficiency\n";
    for (const auto& [regionId, efficiency] : pImpl->currentDevelopmentalMetrics.regionalEfficiency) {
        file << pImpl->currentDevelopmentalMetrics.timestamp << "," << regionId << "," << efficiency << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Developmental metrics exported to: " + filename);
    return true;
}

} // namespace nlm