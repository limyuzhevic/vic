#pragma once

/**
 * Spike Processing Monitor
 * 
 * Spike processing performance monitoring for the NLM brain simulation project.
 * 
 * Features:
 * - Spike processing throughput monitoring
 * - Spike latency measurement
 * - Spike pattern analysis
 * - Network-level spike monitoring
 * - Real-time spike performance tracking
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class SpikeProcessingMonitor {
public:
    /**
     * Spike event record
     */
    struct SpikeEvent {
        std::chrono::high_resolution_clock::time_point timestamp;
        int neuron_id;
        int source_region;
        int target_region;
        double synaptic_weight;
        double propagation_delay;
        double membrane_potential_change;
        int thread_id;
        std::string spike_type; // "excitatory", "inhibitory"
    };

    /**
     * Network metrics
     */
    struct NetworkMetrics {
        double spike_rate; // spikes per second
        double network_efficiency; // 0.0 to 1.0
        double synchronization_level; // 0.0 to 1.0
        double burst_frequency;
        double average_synaptic_strength;
    };

    SpikeProcessingMonitor();
    ~SpikeProcessingMonitor();
    
    /** Record spike event */
    void recordSpikeEvent(const SpikeEvent& spike_event);
    
    /** Get all spike events */
    const std::vector<SpikeEvent>& getSpikeEvents() const { return spike_events_; }
    
    /** Get network metrics */
    const NetworkMetrics& getNetworkMetrics() const { return network_metrics_; }
    
    /** Calculate spike throughput */
    double calculateSpikeThroughput() const;
    
    /** Calculate average spike latency */
    double calculateAverageSpikeLatency() const;
    
    /** Get spike count by region */
    std::unordered_map<std::string, int> getSpikeCountByRegion() const;
    
    /** Get spike count by type */
    std::unordered_map<std::string, int> getSpikeCountByType() const;
    
    /** Calculate network synchronization */
    double calculateNetworkSynchronization() const;
    
    /** Clear spike events */
    void clear();
    
    /** Enable/disable network monitoring */
    void enableNetworkMonitoring(bool enable) { track_network_ = enable; }
    
    /** Enable/disable real-time monitoring */
    void enableRealTimeMonitoring(bool enable) { track_real_time_ = enable; }
    
private:
    std::vector<SpikeEvent> spike_events_;
    NetworkMetrics network_metrics_;
    bool track_network_;
    bool track_real_time_;
    size_t max_spike_events_;
};

} // namespace nlm
