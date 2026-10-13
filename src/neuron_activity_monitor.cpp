// NeuronActivityMonitor class implementation
// This file provides the implementation of the NeuronActivityMonitor high-level interface

#include "../src/neuron_activity_monitor.hpp"
#include "../src/advanced_api.hpp"

// NeuronActivityMonitor implementation
NeuronActivityMonitor::NeuronActivityMonitor(std::shared_ptr<Brain> brain)
    : brain_(brain) {
    startMonitoring();
}

void NeuronActivityMonitor::startMonitoring() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot start monitoring: brain is null");
        return;
    }
    
    startTime_ = std::chrono::steady_clock::now();
    activityHistory_.clear();
    
    NLM_LOG_INFO("Started neuron activity monitoring");
}

void NeuronActivityMonitor::stopMonitoring() {
    if (activityHistory_.empty()) {
        NLM_LOG_WARNING("No activity data to stop monitoring");
        return;
    }
    
    NLM_LOG_INFO("Stopped neuron activity monitoring");
}

bool NeuronActivityMonitor::isMonitoring() const {
    return !activityHistory_.empty();
}

FloatArray NeuronActivityMonitor::getActivityStatistics(size_t neuronIndex) const {
    FloatArray stats(4, 0.0f); // mean, variance, min, max
    
    if (neuronIndex >= activityHistory_.size()) {
        return stats;
    }
    
    const FloatArray& activity = activityHistory_[neuronIndex];
    
    if (activity.empty()) {
        return stats;
    }
    
    // Calculate statistics
    float sum = std::accumulate(activity.begin(), activity.end(), 0.0f);
    float mean = sum / activity.size();
    stats[0] = mean; // mean
    
    float sqSum = 0.0f;
    float min = activity[0];
    float max = activity[0];
    
    for (float val : activity) {
        sqSum += (val - mean) * (val - mean);
        if (val < min) min = val;
        if (val > max) max = val;
    }
    
    float variance = sqSum / activity.size();
    stats[1] = variance; // variance
    stats[2] = min; // min
    stats[3] = max; // max
    
    return stats;
}

FloatMatrix NeuronActivityMonitor::getActivityTimeSeries(size_t neuronIndex) const {
    FloatMatrix timeSeries;
    
    if (neuronIndex >= activityHistory_.size()) {
        return timeSeries;
    }
    
    timeSeries.push_back(activityHistory_[neuronIndex]);
    return timeSeries;
}

std::map<size_t, double> NeuronActivityMonitor::getFiringRates() const {
    std::map<size_t, double> firingRates;
    
    // Calculate firing rates from activity history
    for (size_t i = 0; i < activityHistory_.size(); ++i) {
        const FloatArray& activity = activityHistory_[i];
        if (!activity.empty()) {
            double firingRate = std::accumulate(activity.begin(), activity.end(), 0.0) / activity.size();
            firingRates[i] = firingRate;
        }
    }
    
    return firingRates;
}

std::map<size_t, double> NeuronActivityMonitor::getBursts() const {
    std::map<size_t, double> burstCounts;
    
    // Detect bursts in activity
    for (size_t i = 0; i < activityHistory_.size(); ++i) {
        const FloatArray& activity = activityHistory_[i];
        
        double burstCount = 0.0;
        size_t burstLength = 0;
        
        for (float val : activity) {
            if (val > 0.5) { // Threshold for burst detection
                burstLength++;
            } else {
                if (burstLength > 3) { // Minimum burst length
                    burstCount++;
                }
                burstLength = 0;
            }
        }
        
        if (burstLength > 3) {
            burstCount++;
        }
        
        burstCounts[i] = burstCount;
    }
    
    return burstCounts;
}

FloatArray NeuronActivityMonitor::computeCoherence() const {
    FloatArray coherence(activityHistory_.size(), 0.0f);
    
    if (activityHistory_.empty()) {
        return coherence;
    }
    
    // Calculate coherence between neurons
    for (size_t i = 0; i < activityHistory_.size(); ++i) {
        float coherenceValue = 0.0f;
        
        for (size_t j = i + 1; j < activityHistory_.size(); ++j) {
            // Calculate correlation
            float correlation = 0.0f;
            const FloatArray& actI = activityHistory_[i];
            const FloatArray& actJ = activityHistory_[j];
            
            if (actI.size() == actJ.size() && !actI.empty()) {
                float sumI = std::accumulate(actI.begin(), actI.end(), 0.0f);
                float sumJ = std::accumulate(actJ.begin(), actJ.end(), 0.0f);
                
                float meanI = sumI / actI.size();
                float meanJ = sumJ / actJ.size();
                
                float numerator = 0.0f;
                float denomI = 0.0f;
                float denomJ = 0.0f;
                
                for (size_t k = 0; k < actI.size(); ++k) {
                    float diffI = actI[k] - meanI;
                    float diffJ = actJ[k] - meanJ;
                    numerator += diffI * diffJ;
                    denomI += diffI * diffI;
                    denomJ += diffJ * diffJ;
                }
                
                if (denomI > 0 && denomJ > 0) {
                    correlation = numerator / std::sqrt(denomI * denomJ);
                }
            }
            
            coherenceValue += std::abs(correlation);
        }
        
        coherence[i] = coherenceValue / std::max(1.0f, static_cast<float>(activityHistory_.size() - i - 1));
    }
    
    return coherence;
}

std::vector<size_t> NeuronActivityMonitor::findSynchronyClusters() const {
    std::vector<size_t> clusters;
    
    // Find neuron clusters with similar activity patterns
    FloatArray coherence = computeCoherence();
    
    // Simple clustering based on coherence threshold
    double threshold = 0.7; // Coherence threshold
    
    for (size_t i = 0; i < coherence.size(); ++i) {
        if (coherence[i] > threshold) {
            clusters.push_back(i);
        }
    }
    
    return clusters;
}

std::map<size_t, FloatArray> NeuronActivityMonitor::getSpatialDistribution() const {
    std::map<size_t, FloatArray> spatialDistribution;
    
    // Map neuron activity to spatial coordinates
    for (size_t i = 0; i < activityHistory_.size(); ++i) {
        FloatArray spatial(3, 0.0f); // x, y, z coordinates
        
        // Calculate spatial position based on activity
        const FloatArray& activity = activityHistory_[i];
        
        if (!activity.empty()) {
            float sum = std::accumulate(activity.begin(), activity.end(), 0.0f);
            float meanActivity = sum / activity.size();
            
            // Simple spatial mapping
            spatial[0] = i % 10; // x coordinate
            spatial[1] = (i / 10) % 10; // y coordinate
            spatial[2] = meanActivity; // z coordinate (activity level)
        }
        
        spatialDistribution[i] = spatial;
    }
    
    return spatialDistribution;
}

void NeuronActivityMonitor::exportActivityData(const std::string& filepath) const {
    // Export activity data to file
    // Implementation would use file I/O
    NLM_LOG_INFO("Exported neuron activity data to " + filepath);
}

std::string NeuronActivityMonitor::getActivitySummary() const {
    std::string summary = "Neuron Activity Monitor Summary:\n";
    summary += "  Monitored neurons: " + std::to_string(activityHistory_.size()) + "\n";
    summary += "  Data points: " + std::to_string(activityHistory_.empty() ? 0 : activityHistory_[0].size()) + "\n";
    
    if (!activityHistory_.empty()) {
        double totalActivity = 0.0;
        for (const auto& activity : activityHistory_) {
            totalActivity += std::accumulate(activity.begin(), activity.end(), 0.0);
        }
        summary += "  Total activity: " + std::to_string(totalActivity) + "\n";
    }
    
    return summary;
}
