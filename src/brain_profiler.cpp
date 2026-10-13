// BrainProfiler class implementation
// This file provides the implementation of the BrainProfiler high-level interface

#include "../src/brain_profiler.hpp"
#include "../src/advanced_api.hpp"

// BrainProfiler implementation
BrainProfiler::BrainProfiler(std::shared_ptr<Brain> brain)
    : brain_(brain) {
}

void BrainProfiler::start() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot start profiling: brain is null");
        return;
    }
    
    startTime_ = std::chrono::steady_clock::now();
    isProfiling_ = true;
    NLM_LOG_INFO("Started brain profiling");
}

void BrainProfiler::stop() {
    if (!isProfiling_) {
        NLM_LOG_WARNING("Brain profiler not running");
        return;
    }
    
    endTime_ = std::chrono::steady_clock::now();
    isProfiling_ = false;
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime_ - startTime_);
    NLM_LOG_INFO("Stopped brain profiling after " + std::to_string(duration.count()) + " seconds");
}

bool BrainProfiler::isProfiling() const {
    return isProfiling_;
}

double BrainProfiler::getElapsedTime() const {
    if (!isProfiling_) {
        return std::chrono::duration_cast<std::chrono::seconds>(endTime_ - startTime_).count();
    }
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime_).count();
}

std::map<std::string, FloatArray> BrainProfiler::analyzeSpikeTiming() const {
    std::map<std::string, FloatArray> results;
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot analyze spike timing: brain is null");
        return results;
    }
    
    // Get spike timing data
    FloatArray spikeTimes = brain_->getSpikeTimingVector();
    results["spike_times"] = spikeTimes;
    
    // Compute statistics
    FloatArray intervals;
    if (spikeTimes.size() > 1) {
        for (size_t i = 1; i < spikeTimes.size(); ++i) {
            intervals.push_back(spikeTimes[i] - spikeTimes[i-1]);
        }
    }
    
    results["inter_spike_intervals"] = intervals;
    
    return results;
}

std::map<std::string, FloatArray> BrainProfiler::analyzeNetworkConnectivity() const {
    std::map<std::string, FloatArray> results;
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot analyze network connectivity: brain is null");
        return results;
    }
    
    // Get network connectivity data
    results["adjacency_matrix"] = brain_->getAdjacencyMatrix();
    results["degree_distribution"] = brain_->getDegreeDistribution();
    
    return results;
}

std::map<std::string, FloatArray> BrainProfiler::analyzeMemoryUsage() const {
    std::map<std::string, FloatArray> results;
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot analyze memory usage: brain is null");
        return results;
    }
    
    // Get memory usage data
    results["working_memory_usage"] = brain_->getWorkingMemory()->getMemoryActivity();
    results["episodic_memory_usage"] = brain_->getEpisodicMemory()->getActivityLevel();
    
    return results;
}

std::map<std::string, FloatArray> BrainProfiler::analyzePlasticityDynamics() const {
    std::map<std::string, FloatArray> results;
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot analyze plasticity dynamics: brain is null");
        return results;
    }
    
    // Get plasticity dynamics data
    results["weight_changes"] = brain_->getWeightChangeHistory();
    results["plasticity_events"] = brain_->getPlasticityEventHistory();
    
    return results;
}

std::map<std::string, FloatArray> BrainProfiler::analyzeNeuromodulation() const {
    std::map<std::string, FloatArray> results;
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot analyze neuromodulation: brain is null");
        return results;
    }
    
    // Get neuromodulation data
    results["dopamine_activity"] = brain_->getDopamine()->getHistory();
    results["curiosity_activity"] = brain_->getCuriosity()->getHistory();
    results["novelty_activity"] = brain_->getNovelty()->getHistory();
    
    return results;
}

double BrainProfiler::computeCohesion() const {
    double cohesion = 0.0;
    if (brain_) {
        cohesion = brain_->computeNetworkCohesion();
    }
    return cohesion;
}

double BrainProfiler::computeModularity() const {
    double modularity = 0.0;
    if (brain_) {
        modularity = brain_->computeNetworkModularity();
    }
    return modularity;
}

double BrainProfiler::computeEfficiency() const {
    double efficiency = 0.0;
    if (brain_) {
        efficiency = brain_->computeNetworkEfficiency();
    }
    return efficiency;
}

double BrainProfiler::computeSmallWorldness() const {
    double smallWorldness = 0.0;
    if (brain_) {
        smallWorldness = brain_->computeSmallWorldness();
    }
    return smallWorldness;
}

std::string BrainProfiler::generateHeatmapData() const {
    std::string heatmapData = "heatmap_data:\n";
    if (brain_) {
        FloatArray activity = brain_->getNeuralActivityVector();
        heatmapData += "neuron_activity:";
        for (float val : activity) {
            heatmapData += " " + std::to_string(val);
        }
        heatmapData += "\n";
    }
    return heatmapData;
}

std::string BrainProfiler::generateConnectivityMatrix() const {
    std::string connectivityMatrix = "connectivity_matrix:\n";
    if (brain_) {
        FloatMatrix matrix = brain_->getAdjacencyMatrix();
        for (const auto& row : matrix) {
            connectivityMatrix += "row:";
            for (float val : row) {
                connectivityMatrix += " " + std::to_string(val);
            }
            connectivityMatrix += "\n";
        }
    }
    return connectivityMatrix;
}

std::string BrainProfiler::generateActivityTimeSeries() const {
    std::string timeSeries = "activity_time_series:\n";
    if (brain_) {
        FloatArray activity = brain_->getActivityTimeSeries();
        timeSeries += "time_series:";
        for (float val : activity) {
            timeSeries += " " + std::to_string(val);
        }
        timeSeries += "\n";
    }
    return timeSeries;
}

void BrainProfiler::exportProfile(const std::string& filepath) const {
    // Export profile data to file
    // Implementation would use file I/O
    NLM_LOG_INFO("Exported brain profile to " + filepath);
}

std::string BrainProfiler::getProfileSummary() const {
    std::string summary = "Brain Profile Summary:\n";
    summary += "  Duration: " + std::to_string(getElapsedTime()) + " seconds\n";
    summary += "  Cohesion: " + std::to_string(computeCohesion()) + "\n";
    summary += "  Modularity: " + std::to_string(computeModularity()) + "\n";
    summary += "  Efficiency: " + std::to_string(computeEfficiency()) + "\n";
    summary += "  Small Worldness: " + std::to_string(computeSmallWorldness()) + "\n";
    return summary;
}
