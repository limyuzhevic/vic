#pragma once

/**
 * Neuromodulation Monitor
 * 
 * Neuromodulator performance tracking for the NLM brain simulation project.
 * 
 * Features:
 * - Neuromodulator update performance monitoring
 * - Neuromodulator release tracking
 * - Neuromodulator efficiency analysis
 * - Neuromodulator interaction monitoring
 * - Neuromodulator feedback loop tracking
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class NeuromodulationMonitor {
public:
    /**
     * Neuromodulator update record
     */
    struct NeuromodulatorUpdate {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string neuromodulator_type; // "dopamine", "serotonin", "acetylcholine"
        int target_neuron_id;
        double concentration_change;
        double update_duration;
        double effectiveness_score; // 0.0 to 1.0
        int thread_id;
        std::string release_type; // "phasic", "tonic"
    };

    /**
     * Neuromodulation system metrics
     */
    struct NeuromodulationMetrics {
        double update_frequency; // updates per second
        double average_update_duration;
        double average_effectiveness;
        double neuromodulator_efficiency;
        int total_updates;
        int positive_effects;
        int negative_effects;
    };

    NeuromodulationMonitor();
    ~NeuromodulationMonitor();
    
    /** Record neuromodulator update */
    void recordNeuromodulatorUpdate(const NeuromodulatorUpdate& update);
    
    /** Get all neuromodulator updates */
    const std::vector<NeuromodulatorUpdate>& getNeuromodulatorUpdates() const { return updates_; }
    
    /** Get neuromodulation metrics */
    const NeuromodulationMetrics& getNeuromodulationMetrics() const { return metrics_; }
    
    /** Calculate neuromodulator efficiency */
    double calculateNeuromodulatorEfficiency() const;
    
    /** Get updates by type */
    std::unordered_map<std::string, int> getUpdatesByType() const;
    
    /** Get updates by release type */
    std::unordered_map<std::string, int> getUpdatesByReleaseType() const;
    
    /** Calculate neuromodulator balance */
    double calculateNeuromodulatorBalance() const;
    
    /** Clear updates */
    void clear();
    
    /** Enable/disable phasic tracking */
    void enablePhasicTracking(bool enable) { track_phasic_ = enable; }
    
    /** Enable/disable tonic tracking */
    void enableTonicTracking(bool enable) { track_tonic_ = enable; }
    
private:
    std::vector<NeuromodulatorUpdate> updates_;
    NeuromodulationMetrics metrics_;
    bool track_phasic_;
    bool track_tonic_;
    size_t max_updates_;
};

} // namespace nlm
