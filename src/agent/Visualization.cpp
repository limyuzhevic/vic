// Advanced Visualization and Debugging Tools Implementation
// Implementation of the comprehensive visualization system for NLM

#include "Visualization.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <filesystem>

namespace nlm {

// NeuralActivityVisualizer Implementation
NeuralActivityVisualizer::NeuralActivityVisualizer(VisualizationTheme theme) 
    : maxSnapshots_(100), activityMin_(0.0f), activityMax_(1.0f), theme_(theme) {}

NeuralActivityVisualizer::~NeuralActivityVisualizer() {}

void NeuralActivityVisualizer::setTheme(VisualizationTheme theme) {
    theme_ = theme;
}

void NeuralActivityVisualizer::addSnapshot(const NeuralActivitySnapshot& snapshot) {
    snapshots_.push_back(snapshot);
    if (snapshots_.size() > maxSnapshots_) {
        snapshots_.erase(snapshots_.begin());
    }
}

void NeuralActivityVisualizer::clearSnapshots() {
    snapshots_.clear();
}

std::string NeuralActivityVisualizer::toText() const {
    std::stringstream ss;
    ss << "=== Neural Activity Visualization ===" << std::endl;
    ss << "Theme: " << static_cast<int>(theme_) << std::endl;
    ss << "Snapshots: " << snapshots_.size() << std::endl;
    ss << std::endl;
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        ss << "--- Snapshot " << i << " ---" << std::endl;
        ss << formatAsText(snapshots_[i]) << std::endl;
    }
    
    return ss.str();
}

std::string NeuralActivityVisualizer::toJSON() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"theme\": " << static_cast<int>(theme_) << "," << std::endl;
    ss << "  \"snapshots\": [" << std::endl;
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        ss << "    {" << formatAsJSON(snapshots_[i]) << "}";
        if (i < snapshots_.size() - 1) {
            ss << "," << std::endl;
        } else {
            ss << std::endl;
        }
    }
    
    ss << "  ]" << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

std::string NeuralActivityVisualizer::toCSV() const {
    std::stringstream ss;
    ss << "SnapshotID,NeuronCount,SynapticCount,NeuromodulationCount,Timestamp" << std::endl;
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        const auto& snapshot = snapshots_[i];
        ss << i << "," << snapshot.neuronActivities.size() << ","
           << snapshot.synapticWeights.size() << ","
           << snapshot.neuromodulationLevels.size() << ","
           << std::chrono::system_clock::to_time_t(snapshot.timestamp) << std::endl;
    }
    
    return ss.str();
}

std::string NeuralActivityVisualizer::toHTML() const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head><title>Neural Activity Visualization</title></head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "<h1>Neural Activity Visualization</h1>" << std::endl;
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        ss << "<h2>Snapshot " << i << "</h2>" << std::endl;
        ss << "<pre>" << formatAsText(snapshots_[i]) << "</pre>" << std::endl;
    }
    
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    return ss.str();
}

float NeuralActivityVisualizer::getAverageActivity() const {
    if (snapshots_.empty()) return 0.0f;
    
    float total = 0.0f;
    size_t count = 0;
    
    for (const auto& snapshot : snapshots_) {
        for (float activity : snapshot.neuronActivities) {
            total += activity;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0.0f;
}

float NeuralActivityVisualizer::getMaxActivity() const {
    if (snapshots_.empty()) return 0.0f;
    
    float max = 0.0f;
    for (const auto& snapshot : snapshots_) {
        for (float activity : snapshot.neuronActivities) {
            max = std::max(max, activity);
        }
    }
    
    return max;
}

float NeuralActivityVisualizer::getMinActivity() const {
    if (snapshots_.empty()) return 0.0f;
    
    float min = std::numeric_limits<float>::max();
    for (const auto& snapshot : snapshots_) {
        for (float activity : snapshot.neuronActivities) {
            min = std::min(min, activity);
        }
    }
    
    return min;
}

size_t NeuralActivityVisualizer::getActivityVariance() const {
    if (snapshots_.empty()) return 0;
    
    std::vector<float> activities;
    for (const auto& snapshot : snapshots_) {
        activities.insert(activities.end(), 
                         snapshot.neuronActivities.begin(), 
                         snapshot.neuronActivities.end());
    }
    
    if (activities.size() < 2) return 0;
    
    float mean = getAverageActivity();
    float sumSqDiff = 0.0f;
    for (float activity : activities) {
        sumSqDiff += (activity - mean) * (activity - mean);
    }
    
    return static_cast<size_t>(sumSqDiff / activities.size());
}

NeuralActivitySnapshot NeuralActivityVisualizer::getAverageSnapshot() const {
    NeuralActivitySnapshot avgSnapshot;
    
    if (snapshots_.empty()) return avgSnapshot;
    
    size_t totalNeurons = 0;
    for (const auto& snapshot : snapshots_) {
        totalNeurons += snapshot.neuronActivities.size();
    }
    
    if (totalNeurons == 0) return avgSnapshot;
    
    // Calculate average activities
    for (const auto& snapshot : snapshots_) {
        for (size_t i = 0; i < snapshot.neuronActivities.size(); ++i) {
            if (i < avgSnapshot.neuronActivities.size()) {
                avgSnapshot.neuronActivities[i] += snapshot.neuronActivities[i];
            }
        }
    }
    
    // Normalize
    for (float& activity : avgSnapshot.neuronActivities) {
        activity /= snapshots_.size();
    }
    
    return avgSnapshot;
}

NeuralActivitySnapshot NeuralActivityVisualizer::getPeakSnapshot() const {
    if (snapshots_.empty()) return NeuralActivitySnapshot();
    
    NeuralActivitySnapshot peakSnapshot;
    float maxTotalActivity = 0.0f;
    
    for (const auto& snapshot : snapshots_) {
        float totalActivity = 0.0f;
        for (float activity : snapshot.neuronActivities) {
            totalActivity += activity;
        }
        
        if (totalActivity > maxTotalActivity) {
            maxTotalActivity = totalActivity;
            peakSnapshot = snapshot;
        }
    }
    
    return peakSnapshot;
}

NeuralActivitySnapshot NeuralActivityVisualizer::getLowActivitySnapshot() const {
    if (snapshots_.empty()) return NeuralActivitySnapshot();
    
    NeuralActivitySnapshot lowSnapshot;
    float minTotalActivity = std::numeric_limits<float>::max();
    
    for (const auto& snapshot : snapshots_) {
        float totalActivity = 0.0f;
        for (float activity : snapshot.neuronActivities) {
            totalActivity += activity;
        }
        
        if (totalActivity < minTotalActivity) {
            minTotalActivity = totalActivity;
            lowSnapshot = snapshot;
        }
    }
    
    return lowSnapshot;
}

std::string NeuralActivityVisualizer::formatAsText(const NeuralActivitySnapshot& snapshot) const {
    std::stringstream ss;
    ss << "  Timestamp: " << std::chrono::system_clock::to_time_t(snapshot.timestamp) << std::endl;
    ss << "  Neuron Activities: [";
    for (size_t i = 0; i < std::min(snapshot.neuronActivities.size(), size_t(10)); ++i) {
        ss << snapshot.neuronActivities[i];
        if (i < snapshot.neuronActivities.size() - 1 && i < 9) ss << ", ";
    }
    if (snapshot.neuronActivities.size() > 10) ss << " ...]";
    else ss << "]";
    ss << std::endl;
    ss << "  Synaptic Weights: [";
    for (size_t i = 0; i < std::min(snapshot.synapticWeights.size(), size_t(10)); ++i) {
        ss << snapshot.synapticWeights[i];
        if (i < snapshot.synapticWeights.size() - 1 && i < 9) ss << ", ";
    }
    if (snapshot.synapticWeights.size() > 10) ss << " ...]";
    else ss << "]";
    ss << std::endl;
    ss << "  Neuromodulation Levels: [";
    for (size_t i = 0; i < std::min(snapshot.neuromodulationLevels.size(), size_t(10)); ++i) {
        ss << snapshot.neuromodulationLevels[i];
        if (i < snapshot.neuromodulationLevels.size() - 1 && i < 9) ss << ", ";
    }
    if (snapshot.neuromodulationLevels.size() > 10) ss << " ...]";
    else ss << "]";
    ss << std::endl;
    return ss.str();
}

std::string NeuralActivityVisualizer::formatAsJSON(const NeuralActivitySnapshot& snapshot) const {
    std::stringstream ss;
    ss << "\"timestamp\": \"" << std::chrono::system_clock::to_time_t(snapshot.timestamp) << "\", "
       << "\"neuronActivities\": [";
    
    for (size_t i = 0; i < snapshot.neuronActivities.size(); ++i) {
        ss << snapshot.neuronActivities[i];
        if (i < snapshot.neuronActivities.size() - 1) ss << ", ";
    }
    
    ss << "], \"synapticWeights\": [";
    for (size_t i = 0; i < snapshot.synapticWeights.size(); ++i) {
        ss << snapshot.synapticWeights[i];
        if (i < snapshot.synapticWeights.size() - 1) ss << ", ";
    }
    
    ss << "], \"neuromodulationLevels\": [";
    for (size_t i = 0; i < snapshot.neuromodulationLevels.size(); ++i) {
        ss << snapshot.neuromodulationLevels[i];
        if (i < snapshot.neuromodulationLevels.size() - 1) ss << ", ";
    }
    
    ss << "]";
    return ss.str();
}

std::string NeuralActivityVisualizer::formatAsCSV(const NeuralActivitySnapshot& snapshot) const {
    std::stringstream ss;
    ss << std::chrono::system_clock::to_time_t(snapshot.timestamp) << ",";
    
    for (size_t i = 0; i < snapshot.neuronActivities.size(); ++i) {
        ss << snapshot.neuronActivities[i];
        if (i < snapshot.neuronActivities.size() - 1) ss << ";";
    }
    
    ss << ";";
    for (size_t i = 0; i < snapshot.synapticWeights.size(); ++i) {
        ss << snapshot.synapticWeights[i];
        if (i < snapshot.synapticWeights.size() - 1) ss << ";";
    }
    
    ss << ";";
    for (size_t i = 0; i < snapshot.neuromodulationLevels.size(); ++i) {
        ss << snapshot.neuromodulationLevels[i];
        if (i < snapshot.neuromodulationLevels.size() - 1) ss << ";";
    }
    
    return ss.str();
}

std::string NeuralActivityVisualizer::formatAsHTML(const NeuralActivitySnapshot& snapshot) const {
    std::stringstream ss;
    ss << "<div class=\"neural-snapshot\">" << std::endl;
    ss << "<h3>Neural Activity Snapshot</h3>" << std::endl;
    ss << "<p><strong>Timestamp:</strong> " 
       << std::chrono::system_clock::to_time_t(snapshot.timestamp) 
       << "</p>" << std::endl;
    ss << "<p><strong>Neuron Activities:</strong> [";
    
    for (size_t i = 0; i < std::min(snapshot.neuronActivities.size(), size_t(10)); ++i) {
        ss << snapshot.neuronActivities[i];
        if (i < snapshot.neuronActivities.size() - 1 && i < 9) ss << ", ";
    }
    if (snapshot.neuronActivities.size() > 10) ss << "...";
    ss << "]</p>" << std::endl;
    ss << "</div>" << std::endl;
    return ss.str();
}

// MemorySystemVisualizer Implementation
MemorySystemVisualizer::MemorySystemVisualizer(VisualizationTheme theme) 
    : workingMemThreshold_(0.8f), episodicMemThreshold_(0.7f), semanticMemThreshold_(0.6f),
      showDetailedStats_(true), theme_(theme) {}

MemorySystemVisualizer::~MemorySystemVisualizer() {}

void MemorySystemVisualizer::setTheme(VisualizationTheme theme) {
    theme_ = theme;
}

std::string MemorySystemVisualizer::toText() const {
    std::stringstream ss;
    ss << "=== Memory System Visualization ===" << std::endl;
    ss << "Theme: " << static_cast<int>(theme_) << std::endl;
    ss << std::endl;
    
    ss << formatWorkingMemoryStats() << std::endl;
    ss << formatEpisodicMemoryStats() << std::endl;
    ss << formatSemanticMemoryStats() << std::endl;
    ss << formatProceduralMemoryStats() << std::endl;
    ss << formatAssociativeMemoryStats() << std::endl;
    
    return ss.str();
}

std::string MemorySystemVisualizer::toJSON() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"workingMemory\": " << formatWorkingMemoryStats() << "," << std::endl;
    ss << "  \"episodicMemory\": " << formatEpisodicMemoryStats() << ", " << std::endl;
    ss << "  \"semanticMemory\": " << formatSemanticMemoryStats() << ", " << std::endl;
    ss << "  \"proceduralMemory\": " << formatProceduralMemoryStats() << ", " << std::endl;
    ss << "  \"associativeMemory\": " << formatAssociativeMemoryStats() << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

std::string MemorySystemVisualizer::toHTML() const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head><title>Memory System Visualization</title></head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "<h1>Memory System Visualization</h1>" << std::endl;
    ss << "<div class=\"memory-stats\">" << std::endl;
    ss << formatWorkingMemoryStats() << std::endl;
    ss << formatEpisodicMemoryStats() << std::endl;
    ss << formatSemanticMemoryStats() << std::endl;
    ss << formatProceduralMemoryStats() << std::endl;
    ss << formatAssociativeMemoryStats() << std::endl;
    ss << "</div>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    return ss.str();
}

void MemorySystemVisualizer::analyzeMemoryHealth() const {
    NLM_LOG_INFO("=== Memory Health Analysis ===");
    
    // Check working memory health
    if (workingMemory_) {
        size_t currentSize = workingMemory_->getCurrentSize();
        size_t capacity = workingMemory_->getCapacity();
        float utilization = static_cast<float>(currentSize) / capacity;
        
        if (utilization > workingMemThreshold_) {
            NLM_LOG_WARN("Working memory utilization high: " + std::to_string(utilization * 100) + "%");
        } else {
            NLM_LOG_INFO("Working memory utilization normal: " + std::to_string(utilization * 100) + "%");
        }
    }
    
    // Check episodic memory health
    if (episodicMemory_) {
        size_t episodeCount = episodicMemory_->getEpisodeCount();
        
        if (episodeCount > 1000) {
            NLM_LOG_INFO("Episodic memory contains many episodes: " + std::to_string(episodeCount));
        } else if (episodeCount < 10) {
            NLM_LOG_WARN("Episodic memory contains few episodes: " + std::to_string(episodeCount));
        }
    }
    
    // Check semantic memory health
    if (semanticMemory_) {
        auto facts = semanticMemory_->getAllFacts();
        
        if (facts.size() > 100) {
            NLM_LOG_INFO("Semantic memory contains many facts: " + std::to_string(facts.size()));
        } else if (facts.size() < 5) {
            NLM_LOG_WARN("Semantic memory contains few facts: " + std::to_string(facts.size()));
        }
    }
}

void MemorySystemVisualizer::detectMemoryGaps() const {
    NLM_LOG_INFO("=== Memory Gap Detection ===");
    
    // Detect gaps in episodic memory
    if (episodicMemory_) {
        auto recentEpisodes = episodicMemory_->getRecentEpisodes(10);
        
        if (recentEpisodes.size() < 5) {
            NLM_LOG_WARN("Recent episodic memory gap: only " + std::to_string(recentEpisodes.size()) + " episodes in last 10");
        }
    }
    
    // Detect gaps in semantic memory
    if (semanticMemory_) {
        auto facts = semanticMemory_->getAllFacts();
        
        if (facts.empty()) {
            NLM_LOG_WARN("Semantic memory gap: no facts stored");
        }
    }
    
    // Detect gaps in procedural memory
    if (proceduralMemory_) {
        auto skillNames = proceduralMemory_->getAllSkillNames();
        
        if (skillNames.empty()) {
            NLM_LOG_WARN("Procedural memory gap: no skills learned");
        }
    }
}

void MemorySystemVisualizer::assessMemoryEfficiency() const {
    NLM_LOG_INFO("=== Memory Efficiency Assessment ===");
    
    float totalEfficiency = 0.0f;
    
    // Assess working memory efficiency
    if (workingMemory_) {
        size_t currentSize = workingMemory_->getCurrentSize();
        size_t capacity = workingMemory_->getCapacity();
        
        if (capacity > 0) {
            float efficiency = 1.0f - (static_cast<float>(currentSize) / capacity);
            NLM_LOG_INFO("Working memory efficiency: " + std::to_string(efficiency * 100) + "%");
            totalEfficiency += efficiency;
        }
    }
    
    // Assess episodic memory efficiency
    if (episodicMemory_) {
        size_t episodeCount = episodicMemory_->getEpisodeCount();
        
        // Simple efficiency metric based on episode count
        float efficiency = std::min(1.0f, static_cast<float>(episodeCount) / 1000.0f);
        NLM_LOG_INFO("Episodic memory efficiency: " + std::to_string(efficiency * 100) + "%");
        totalEfficiency += efficiency;
    }
    
    // Assess semantic memory efficiency
    if (semanticMemory_) {
        auto facts = semanticMemory_->getAllFacts();
        
        float efficiency = std::min(1.0f, static_cast<float>(facts.size()) / 100.0f);
        NLM_LOG_INFO("Semantic memory efficiency: " + std::to_string(efficiency * 100) + "%");
        totalEfficiency += efficiency;
    }
    
    // Calculate average efficiency
    int validMetrics = 0;
    if (workingMemory_) validMetrics++;
    if (episodicMemory_) validMetrics++;
    if (semanticMemory_) validMetrics++;
    
    if (validMetrics > 0) {
        float avgEfficiency = totalEfficiency / validMetrics;
        NLM_LOG_INFO("Average memory system efficiency: " + std::to_string(avgEfficiency * 100) + "%");
    }
}

std::string MemorySystemVisualizer::formatWorkingMemoryStats() const {
    std::stringstream ss;
    ss << "--- Working Memory ---" << std::endl;
    
    if (!workingMemory_) {
        ss << "Not initialized" << std::endl;
        return ss.str();
    }
    
    size_t currentSize = workingMemory_->getCurrentSize();
    size_t capacity = workingMemory_->getCapacity();
    
    ss << "Size: " << currentSize << "/" << capacity << std::endl;
    ss << "Utilization: " << (capacity > 0 ? (currentSize * 100.0f / capacity) : 0.0f) << "%" << std::endl;
    
    if (showDetailedStats_) {
        // Try to get some additional stats if available
        ss << "Decay rate: 0.01f (default)" << std::endl;
    }
    
    return ss.str();
}

std::string MemorySystemVisualizer::formatEpisodicMemoryStats() const {
    std::stringstream ss;
    ss << "--- Episodic Memory ---" << std::endl;
    
    if (!episodicMemory_) {
        ss << "Not initialized" << std::endl;
        return ss.str();
    }
    
    size_t episodeCount = episodicMemory_->getEpisodeCount();
    ss << "Episode Count: " << episodeCount << std::endl;
    
    if (showDetailedStats_) {
        auto recentEpisodes = episodicMemory_->getRecentEpisodes(5);
        ss << "Recent Episodes: " << recentEpisodes.size() << std::endl;
        ss << "Max Episodes: 1000 (default capacity)" << std::endl;
    }
    
    return ss.str();
}

std::string MemorySystemVisualizer::formatSemanticMemoryStats() const {
    std::stringstream ss;
    ss << "--- Semantic Memory ---" << std::endl;
    
    if (!semanticMemory_) {
        ss << "Not initialized" << std::endl;
        return ss.str();
    }
    
    auto facts = semanticMemory_->getAllFacts();
    ss << "Facts Count: " << facts.size() << std::endl;
    
    if (showDetailedStats_ && !facts.empty()) {
        ss << "Sample Facts:" << std::endl;
        size_t sampleSize = std::min(facts.size(), size_t(3));
        for (size_t i = 0; i < sampleSize; ++i) {
            ss << "  - " << facts[i].first << " -> " << facts[i].second << std::endl;
        }
    }
    
    return ss.str();
}

std::string MemorySystemVisualizer::formatProceduralMemoryStats() const {
    std::stringstream ss;
    ss << "--- Procedural Memory ---" << std::endl;
    
    if (!proceduralMemory_) {
        ss << "Not initialized" << std::endl;
        return ss.str();
    }
    
    auto skillNames = proceduralMemory_->getAllSkillNames();
    ss << "Skills Count: " << skillNames.size() << std::endl;
    
    if (showDetailedStats_ && !skillNames.empty()) {
        ss << "Skill Proficiency:" << std::endl;
        for (const auto& skillName : skillNames) {
            float proficiency = proceduralMemory_->getSkillProficiency(skillName);
            ss << "  - " << skillName << ": " << (proficiency * 100) << "%" << std::endl;
        }
    }
    
    return ss.str();
}

std::string MemorySystemVisualizer::formatAssociativeMemoryStats() const {
    std::stringstream ss;
    ss << "--- Associative Memory ---" << std::endl;
    
    if (!associativeMemory_) {
        ss << "Not initialized" << std::endl;
        return ss.str();
    }
    
    auto associations = associativeMemory_->getAllAssociations();
    ss << "Associations Count: " << associations.size() << std::endl;
    
    if (showDetailedStats_ && !associations.empty()) {
        ss << "Sample Associations:" << std::endl;
        size_t sampleSize = std::min(associations.size(), size_t(3));
        for (size_t i = 0; i < sampleSize; ++i) {
            ss << "  - (" << std::get<0>(associations[i]) << ", " 
               << std::get<1>(associations[i]) << ") : "
               << std::get<2>(associations[i]) << std::endl;
        }
    }
    
    return ss.str();
}

// PerformanceMonitor Implementation
PerformanceMonitor::PerformanceMonitor() 
    : reportingInterval_(100), enableAlerting_(true) {}

PerformanceMonitor::~PerformanceMonitor() {}

void PerformanceMonitor::startTiming(const std::string& operation) {
    auto it = timingData_.find(operation);
    if (it == timingData_.end()) {
        timingData_[operation] = TimingInfo{std::chrono::system_clock::now(), 
                                           std::chrono::duration<double>(0.0), 1};
    } else {
        it->second.startTime = std::chrono::system_clock::now();
        it->second.accumulatedTime = std::chrono::duration<double>(0.0);
        it->second.callCount = 1;
    }
}

void PerformanceMonitor::stopTiming(const std::string& operation) {
    auto it = timingData_.find(operation);
    if (it != timingData_.end()) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double>(endTime - it->second.startTime);
        it->second.accumulatedTime += duration;
        it->second.callCount++;
        
        updateMetrics();
    }
}

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    metricData_[name].push_back(value);
    
    // Keep only recent metrics for memory efficiency
    if (metricData_[name].size() > reportingInterval_) {
        metricData_[name].erase(metricData_[name].begin());
    }
    
    updateMetrics();
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    return currentMetrics_;
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::stringstream ss;
    ss << "=== Performance Report ===" << std::endl;
    ss << "Total Reward: " << currentMetrics_.totalReward << std::endl;
    ss << "Average Step Reward: " << currentMetrics_.averageStepReward << std::endl;
    ss << "Total Steps: " << currentMetrics_.totalSteps << std::endl;
    ss << "Successful Actions: " << currentMetrics_.successfulActions << std::endl;
    ss << "Exploration Rate: " << currentMetrics_.explorationRate << std::endl;
    ss << "Learning Rate: " << currentMetrics_.learningRate << std::endl;
    ss << "Final Memory Usage: " << currentMetrics_.finalMemoryUsage << std::endl;
    ss << "Final Skill Proficiency: " << currentMetrics_.finalSkillProficiency << std::endl;
    ss << "Time Per Step: " << currentMetrics_.timePerStep << std::endl;
    
    return ss.str();
}

void PerformanceMonitor::updateMetrics() {
    currentMetrics_.totalSteps = 0;
    currentMetrics_.successfulActions = 0;
    currentMetrics_.totalReward = 0.0;
    currentMetrics_.averageStepReward = 0.0;
    currentMetrics_.explorationRate = 0.0;
    currentMetrics_.learningRate = 0.0;
    
    // Simple metric updates based on recorded data
    // In a real implementation, this would be more sophisticated
    for (const auto& pair : metricData_) {
        if (pair.first.find("reward") != std::string::npos) {
            double sum = 0.0;
            for (double value : pair.second) {
                sum += value;
            }
            if (!pair.second.empty()) {
                currentMetrics_.totalReward = sum;
                currentMetrics_.averageStepReward = sum / pair.second.size();
                currentMetrics_.totalSteps = pair.second.size();
            }
        } else if (pair.first.find("exploration") != std::string::npos) {
            double sum = 0.0;
            for (double value : pair.second) {
                sum += value;
            }
            currentMetrics_.explorationRate = !pair.second.empty() ? sum / pair.second.size() : 0.0;
        }
    }
    
    if (currentMetrics_.totalSteps > 0) {
        currentMetrics_.learningRate = currentMetrics_.totalReward / currentMetrics_.totalSteps;
    }
}

std::string PerformanceMonitor::generateAlertReport() const {
    std::stringstream ss;
    ss << "=== Performance Alerts ===" << std::endl;
    
    for (const auto& pair : metricThresholds_) {
        auto it = metricData_.find(pair.first);
        if (it != metricData_.end() && !it->second.empty()) {
            double lastValue = it->second.back();
            if (lastValue < pair.second) {
                ss << "ALERT: Metric '" << pair.first << "' is below threshold: " 
                   << lastValue << " < " << pair.second << std::endl;
            }
        }
    }
    
    return ss.str();
}

double PerformanceMonitor::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }
    
    return sum / values.size();
}

double PerformanceMonitor::calculateVariance(const std::vector<double>& values) const {
    if (values.size() < 2) return 0.0;
    
    double mean = calculateMean(values);
    double sumSqDiff = 0.0;
    for (double value : values) {
        sumSqDiff += (value - mean) * (value - mean);
    }
    
    return sumSqDiff / values.size();
}

// DebugConsole Implementation
DebugConsole::DebugConsole() : monitoring_(false), updateInterval_(1000) {}

DebugConsole::~DebugConsole() {}

void DebugConsole::logMessage(const std::string& category, const std::string& message, 
                             const std::string& level) {
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = level;
    entry.message = message;
    
    logEntries_.push_back(entry);
    
    // Keep log size manageable
    if (logEntries_.size() > 10000) {
        logEntries_.erase(logEntries_.begin(), logEntries_.begin() + 1000);
    }
    
    // Output to console for real-time monitoring
    if (monitoring_) {
        std::cout << "[" << formatTimestamp(entry.timestamp) << "] " 
                  << level << " " << category << ": " 
                  << message << std::endl;
    }
}

void DebugConsole::logNeuralEvent(const std::string& eventType, double timestamp, 
                                 const std::map<std::string, std::string>& metadata) {
    std::stringstream message;
    message << "Neural Event: " << eventType << " @ " << timestamp;
    
    for (const auto& pair : metadata) {
        message << ", " << pair.first << "=" << pair.second;
    }
    
    logMessage("NEURAL", message.str(), "TRACE");
}

void DebugConsole::logMemoryEvent(const std::string& eventType, double timestamp,
                                 const std::map<std::string, std::string>& metadata) {
    std::stringstream message;
    message << "Memory Event: " << eventType << " @ " << timestamp;
    
    for (const auto& pair : metadata) {
        message << ", " << pair.first << "=" << pair.second;
    }
    
    logMessage("MEMORY", message.str(), "TRACE");
}

void DebugConsole::logBehaviorEvent(const std::string& eventType, double timestamp,
                                   const std::map<std::string, std::string>& metadata) {
    std::stringstream message;
    message << "Behavior Event: " << eventType << " @ " << timestamp;
    
    for (const auto& pair : metadata) {
        message << ", " << pair.first << "=" << pair.second;
    }
    
    logMessage("BEHAVIOR", message.str(), "TRACE");
}

std::string DebugConsole::getDebugLog() const {
    std::stringstream ss;
    ss << "=== Debug Log ===" << std::endl;
    
    for (const auto& entry : logEntries_) {
        if (shouldLogEntry(entry)) {
            ss << "[" << formatTimestamp(entry.timestamp) << "] " 
               << entry.level << " " << entry.category << ": " 
               << entry.message << std::endl;
        }
    }
    
    return ss.str();
}

void DebugConsole::clearLog() {
    logEntries_.clear();
}

void DebugConsole::setLogFilter(const std::string& category, const std::string& level) {
    currentFilterCategory_ = category;
    currentFilterLevel_ = level;
}

std::vector<std::string> DebugConsole::searchLog(const std::string& pattern) const {
    std::vector<std::string> results;
    
    for (const auto& entry : logEntries_) {
        if (shouldLogEntry(entry) && entry.message.find(pattern) != std::string::npos) {
            results.push_back(formatTimestamp(entry.timestamp) + " " + 
                            entry.level + " " + entry.category + ": " + 
                            entry.message);
        }
    }
    
    return results;
}

void DebugConsole::startRealTimeMonitoring(size_t updateInterval) {
    monitoring_ = true;
    updateInterval_ = updateInterval;
}

void DebugConsole::stopRealTimeMonitoring() {
    monitoring_ = false;
}

std::string DebugConsole::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

bool DebugConsole::shouldLogEntry(const LogEntry& entry) const {
    // Apply filters if set
    if (!currentFilterCategory_.empty() && entry.category != currentFilterCategory_) {
        return false;
    }
    
    if (!currentFilterLevel_.empty() && entry.level != currentFilterLevel_) {
        return false;
    }
    
    return true;
}

// VisualizationManager Implementation
VisualizationManager::VisualizationManager(VisualizationTheme theme) 
    : outputFormat_(OutputFormat::TEXT), maxReportSize_(1000000), visualizing_(false) {
    neuralVisualizer_.setTheme(theme);
    memoryVisualizer_.setTheme(theme);
    behaviorVisualizer_.setTheme(theme);
}

VisualizationManager::~VisualizationManager() {}

void VisualizationManager::updateNeuralVisualization(const NeuralActivitySnapshot& snapshot) {
    neuralVisualizer_.addSnapshot(snapshot);
    currentNeuralSnapshot_ = snapshot;
}

void VisualizationManager::updateMemoryVisualization(const MemorySystemSnapshot& snapshot) {
    memoryVisualizer_.setWorkingMemory(const_cast<WorkingMemory*>(snapshot.workingMemory_));
    memoryVisualizer_.setEpisodicMemory(const_cast<EpisodicMemory*>(snapshot.episodicMemory_));
    memoryVisualizer_.setSemanticMemory(const_cast<SemanticMemory*>(snapshot.semanticMemory_));
    memoryVisualizer_.setProceduralMemory(const_cast<ProceduralMemory*>(snapshot.proceduralMemory_));
    memoryVisualizer_.setAssociativeMemory(const_cast<AssociativeMemory*>(snapshot.associativeMemory_));
    
    currentMemorySnapshot_ = snapshot;
}

void VisualizationManager::updateBehaviorVisualization(const AgentBehaviorSnapshot& snapshot) {
    behaviorVisualizer_.setAgentBrain(const_cast<AgentBrain*>(snapshot.agentBrain_));
    behaviorVisualizer_.setAgentBody(const_cast<AgentBody*>(snapshot.agentBody_));
    behaviorVisualizer_.setWorld(const_cast<SimpleWorld*>(snapshot.world_));
    
    currentBehaviorSnapshot_ = snapshot;
}

std::string VisualizationManager::generateFullReport(OutputFormat format) const {
    std::stringstream ss;
    
    switch (format) {
        case OutputFormat::TEXT:
            ss << generateNeuralReport(OutputFormat::TEXT);
            ss << generateMemoryReport(OutputFormat::TEXT);
            ss << generateBehaviorReport(OutputFormat::TEXT);
            break;
        case OutputFormat::JSON:
            ss << "{" << std::endl;
            ss << "  \"neural\": " << generateNeuralReport(OutputFormat::JSON) << "," << std::endl;
            ss << "  \"memory\": " << generateMemoryReport(OutputFormat::JSON) << ", " << std::endl;
            ss << "  \"behavior\": " << generateBehaviorReport(OutputFormat::JSON) << std::endl;
            ss << "}" << std::endl;
            break;
        case OutputFormat::HTML:
            ss << createHTMLDashboard(generateNeuralReport(OutputFormat::HTML),
                                     generateMemoryReport(OutputFormat::HTML),
                                     generateBehaviorReport(OutputFormat::HTML));
            break;
        default:
            ss << generateFullReport(); // Recursive call with default format
            break;
    }
    
    return truncateReport(ss.str());
}

std::string VisualizationManager::generateNeuralReport(OutputFormat format) const {
    return neuralVisualizer_.toText(); // Simplified - just return text for now
}

std::string VisualizationManager::generateMemoryReport(OutputFormat format) const {
    return memoryVisualizer_.toText(); // Simplified - just return text for now
}

std::string VisualizationManager::generateBehaviorReport(OutputFormat format) const {
    return behaviorVisualizer_.toText(); // Simplified - just return text for now
}

void VisualizationManager::setTheme(VisualizationTheme theme) {
    neuralVisualizer_.setTheme(theme);
    memoryVisualizer_.setTheme(theme);
    behaviorVisualizer_.setTheme(theme);
}

void VisualizationManager::integrateWithAgent(AgentBrain* agentBrain, AgentBody* agentBody, SimpleWorld* world) {
    behaviorVisualizer_.setAgentBrain(agentBrain);
    behaviorVisualizer_.setAgentBody(agentBody);
    behaviorVisualizer_.setWorld(world);
}

void VisualizationManager::integrateWithMemory(WorkingMemory* workingMemory, EpisodicMemory* episodicMemory,
                                              SemanticMemory* semanticMemory, ProceduralMemory* proceduralMemory,
                                              AssociativeMemory* associativeMemory) {
    memoryVisualizer_.setWorkingMemory(workingMemory);
    memoryVisualizer_.setEpisodicMemory(episodicMemory);
    memoryVisualizer_.setSemanticMemory(semanticMemory);
    memoryVisualizer_.setProceduralMemory(proceduralMemory);
    memoryVisualizer_.setAssociativeMemory(associativeMemory);
}

void VisualizationManager::startRealTimeVisualization(size_t updateInterval) {
    visualizing_ = true;
    // In a real implementation, this would start a background thread for periodic updates
}

void VisualizationManager::stopRealTimeVisualization() {
    visualizing_ = false;
}

std::string VisualizationManager::generateCombinedReport() const {
    std::stringstream ss;
    ss << "=== Combined Visualization Report ===" << std::endl;
    ss << generateNeuralReport() << std::endl;
    ss << generateMemoryReport() << std::endl;
    ss << generateBehaviorReport() << std::endl;
    return ss.str();
}

std::string VisualizationManager::truncateReport(const std::string& report) const {
    if (report.size() <= maxReportSize_) {
        return report;
    }
    
    return report.substr(0, maxReportSize_ - 100) + "... (truncated)";
}

// Visualization namespace functions
namespace Visualization {
    std::string generateQuickReport(AgentBrain* agentBrain, AgentBody* agentBody, 
                                   SimpleWorld* world, WorkingMemory* workingMemory,
                                   EpisodicMemory* episodicMemory,
                                   SemanticMemory* semanticMemory, ProceduralMemory* proceduralMemory,
                                   AssociativeMemory* associativeMemory) {
        std::stringstream ss;
        ss << "=== Quick NLM Agent Report ===" << std::endl;
        ss << "Agent Status: " << (agentBrain ? "Active" : "Inactive") << std::endl;
        ss << "World Size: " << (world ? "Configured" : "Not Set") << std::endl;
        ss << "Memory Systems: " << (workingMemory ? "1" : "0") + 
            "/" + std::to_string(5) << std::endl;
        ss << "Working Memory: " << (workingMemory ? std::to_string(workingMemory->getCurrentSize()) : "0") << 
            "/" << (workingMemory ? std::to_string(workingMemory->getCapacity()) : "0") << std::endl;
        ss << "Episodic Memory: " << (episodicMemory ? std::to_string(episodicMemory->getEpisodeCount()) : "0") << 
            " episodes" << std::endl;
        ss << "Semantic Memory: " << (semanticMemory ? std::to_string(semanticMemory->getAllFacts().size()) : "0") << 
            " facts" << std::endl;
        ss << "Procedural Memory: " << (proceduralMemory ? std::to_string(proceduralMemory->getAllSkillNames().size()) : "0") << 
            " skills" << std::endl;
        ss << "Associative Memory: " << (associativeMemory ? std::to_string(associativeMemory->getAllAssociations().size()) : "0") << 
            " associations" << std::endl;
        
        return ss.str();
    }
    
    void saveVisualizationToFile(const std::string& filename, const std::string& report) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << report;
            file.close();
            NLM_LOG_INFO("Visualization saved to: " + filename);
        } else {
            NLM_LOG_ERROR("Failed to save visualization to: " + filename);
        }
    }
    
    std::string createHTMLDashboard(const std::string& neuralData, 
                                   const std::string& memoryData,
                                   const std::string& behaviorData) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>" << std::endl;
        ss << "<html>" << std::endl;
        ss << "<head><title>NLM Agent Dashboard</title>" << std::endl;
        ss << "<style>body { font-family: Arial, sans-serif; margin: 20px; } "
           << "div { margin: 10px 0; padding: 10px; border: 1px solid #ccc; } "
           << ".section { background-color: #f5f5f5; } "
           << "pre { white-space: pre-wrap; word-wrap: break-word; }" 
           << "</style>" << std::endl;
        ss << "</head>" << std::endl;
        ss << "<body>" << std::endl;
        ss << "<h1>NLM Agent Dashboard</h1>" << std::endl;
        ss << "<div class=\"section\">" << std::endl;
        ss << "<h2>Neural Activity</h2>" << std::endl;
        ss << "<pre>" << neuralData << "</pre>" << std::endl;
        ss << "</div>" << std::endl;
        ss << "<div class=\"section\">" << std::endl;
        ss << "<h2>Memory Systems</h2>" << std::endl;
        ss << "<pre>" << memoryData << "</pre>" << std::endl;
        ss << "</div>" << std::endl;
        ss << "<div class=\"section\">" << std::endl;
        ss << "<h2>Agent Behavior</h2>" << std::endl;
        ss << "<pre>" << behaviorData << "</pre>" << std::endl;
        ss << "</div>" << std::endl;
        ss << "</body>" << std::endl;
        ss << "</html>" << std::endl;
        
        return ss.str();
    }
    
    void displayTerminalSummary(const std::string& title, const std::string& content) {
        std::cout << "=== " << title << " ===" << std::endl;
        std::cout << content << std::endl;
        std::cout << "=================================" << std::endl;
    }
}

} // namespace nlm

// Helper function for including the visualization header
#include "Visualization.hpp"

namespace nlm {
    VisualizationManager createVisualizationManager(VisualizationTheme theme) {
        return VisualizationManager(theme);
    }
}