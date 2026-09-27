#pragma once

/**
 * Integration Monitor
 * 
 * Cross-system communication performance monitoring for the NLM brain simulation project.
 * 
 * Features:
 * - Cross-system communication monitoring
 * - System integration performance tracking
 * - Memory-neuromodulation integration monitoring
 * - Plugin system integration monitoring
 * - Configuration integration monitoring
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class IntegrationMonitor {
public:
    /**
     * Integration event record
     */
    struct IntegrationEvent {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string source_system;
        std::string target_system;
        std::string integration_type; // "memory_neuromodulation", "plugin_system", "configuration"
        double communication_duration;
        double communication_efficiency; // 0.0 to 1.0
        int thread_id;
        bool success;
        std::string error_message;
    };

    /**
     * Integration system metrics
     */
    struct IntegrationMetrics {
        double communication_frequency; // communications per second
        double average_communication_duration;
        double overall_integration_efficiency;
        double system_coordination_efficiency;
        int total_communications;
        int successful_communications;
        int failed_communications;
    };

    IntegrationMonitor();
    ~IntegrationMonitor();
    
    /** Record integration event */
    void recordIntegrationEvent(const IntegrationEvent& event);
    
    /** Get all integration events */
    const std::vector<IntegrationEvent>& getIntegrationEvents() const { return events_; }
    
    /** Get integration metrics */
    const IntegrationMetrics& getIntegrationMetrics() const { return metrics_; }
    
    /** Calculate integration efficiency */
    double calculateIntegrationEfficiency() const;
    
    /** Get events by type */
    std::unordered_map<std::string, int> getEventsByType() const;
    
    /** Get events by system */
    std::unordered_map<std::string, int> getEventsBySystem() const;
    
    /** Calculate system coherence */
    double calculateSystemCoherence() const;
    
    /** Clear events */
    void clear();
    
    /** Enable/disable plugin integration tracking */
    void enablePluginIntegrationTracking(bool enable) { track_plugin_integration_ = enable; }
    
    /** Enable/disable configuration integration tracking */
    void enableConfigurationIntegrationTracking(bool enable) { track_configuration_integration_ = enable; }
    
private:
    std::vector<IntegrationEvent> events_;
    IntegrationMetrics metrics_;
    bool track_plugin_integration_;
    bool track_configuration_integration_;
    size_t max_events_;
};

} // namespace nlm
