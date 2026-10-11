#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <iostream>
#include <fstream>
#include <set>

namespace nlm {

/**
 * Debug visualization system for Phase 6
 * 
 * Provides comprehensive debugging tools for detecting and visualizing
 * memory leaks, event flows, integration issues, system state, and debug timelines.
 */
class DebugVisualizer {
public:
    /**
     * Structure representing a memory allocation trace
     */
    struct MemoryAllocation {
        std::string allocationId;
        uint64_t address;
        uint64_t size;
        std::string allocationSite;
        std::string allocationType; // "heap", "stack", "global"
        std::chrono::steady_clock::time_point timestamp;
        bool isLeaked;
        std::string allocatorTag;
        
        MemoryAllocation() : address(0), size(0), isLeaked(false) {}
    };
    
    /**
     * Structure representing an event trace
     */
    struct EventTrace {
        std::string eventId;
        std::string eventType; // "neuron_fire", "synaptic_update", "memory_alloc", "system_event"
        double timestamp;
        std::string source; // Component that generated the event
        std::string target; // Component that received the event
        std::string data; // Event data as JSON string
        std::string flowPath; // Complete flow path through the system
        std::string severity; // "info", "warning", "error", "critical"
        
        EventTrace() : timestamp(0.0) {}
    };
    
    /**
     * Structure representing an integration issue
     */
    struct IntegrationIssue {
        std::string issueId;
        std::string issueType; // "missing_dependency", "circular_dependency", "performance_degradation", "health_decline"
        std::string component; // Component affected
        std::string severity; // "low", "medium", "high", "critical"
        double timestamp;
        std::string description;
        double impactScore; // 0.0 to 1.0
        bool isResolved;
        std::vector<std::string> suggestedFixes;
        
        IntegrationIssue() : impactScore(0.0), isResolved(false), timestamp(0.0) {}
    };
    
    /**
     * Structure representing system state
     */
    struct SystemState {
        double timestamp;
        std::string stateId;
        std::string sessionId;
        std::vector<std::string> activeComponents;
        std::vector<std::string> inactiveComponents;
        std::unordered_map<std::string, std::string> configurationValues;
        std::unordered_map<std::string, double> systemMetrics;
        std::vector<std::string> recentEvents;
        
        SystemState() : timestamp(0.0) {}
    };
    
    /**
     * Structure representing a debug timeline event
     */
    struct TimelineEvent {
        double timestamp;
        std::string eventId;
        std::string eventType; // "step", "checkpoint", "integration", "performance", "debug"
        std::string description;
        std::vector<std::string> tags;
        std::string color; // Color for timeline visualization
        double duration; // Duration of event
        
        TimelineEvent() : timestamp(0.0), duration(0.0) {}
    };
    
    /**
     * Debug visualization options
     */
    struct DebugOptions {
        bool showMemoryLeaks; // Show memory leak detector
        bool showEventFlow; // Show event flow tracer
        bool showIntegrationIssues; // Show integration issue detector
        bool showSystemState; // Show system state inspector
        bool showTimeline; // Show debug timeline visualizer
        std::string focusArea; // "memory", "events", "integration", "performance", "all"
        std::string detailLevel; // "brief", "detailed", "comprehensive"
        double refreshInterval; // seconds
        std::vector<std::string> eventTypesToTrack;
        std::vector<std::string> componentFilter;
        bool enableLiveTracking; // Enable real-time tracking
        double maxHistorySize; // Maximum events to keep in history
    };
    
    /**
     * Memory leak detection results
     */
    struct MemoryLeakReport {
        bool hasLeaks; // Whether memory leaks were detected
        uint64_t totalAllocated; // Total allocated memory
        uint64_t totalLeaked; // Total leaked memory
        uint64_t leakCount; // Number of leaks
        std::vector<MemoryAllocation> leaks; // Detailed leak information
        std::string report; // Human-readable report
        
        MemoryLeakReport() : hasLeaks(false), totalAllocated(0), totalLeaked(0), leakCount(0) {}
    };
    
    /**
     * Event flow analysis results
     */
    struct EventFlowReport {
        uint64_t totalEvents; // Total events processed
        uint64_t eventsTracked; // Events that are being tracked
        std::vector<EventTrace> significantEvents; // Events that need attention
        std::unordered_map<std::string, uint64_t> eventTypeCounts; // Count by type
        std::unordered_map<std::string, uint64_t> sourceCounts; // Count by source
        std::string flowReport; // Human-readable report
        
        EventFlowReport() : totalEvents(0), eventsTracked(0) {}
    };
    
public:
    DebugVisualizer();
    ~DebugVisualizer();
    
    /**
     * Initialize the debug visualizer
     */
    bool initialize();
    
    /**
     * Update debug information
     */
    void update(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    /**
     * Render memory leak detector
     */
    void renderMemoryLeakDetector() const;
    
    /**
     * Render event flow tracer
     */
    void renderEventFlowTracer() const;
    
    /**
     * Render integration issue detector
     */
    void renderIntegrationIssueDetector() const;
    
    /**
     * Render system state inspector
     */
    void renderSystemStateInspector() const;
    
    /**
     * Render debug timeline visualizer
     */
    void renderDebugTimeline() const;
    
    /**
     * Get memory leak report
     */
    MemoryLeakReport getMemoryLeakReport() const { return memoryLeakReport_; }
    
    /**
     * Get event flow report
     */
    EventFlowReport getEventFlowReport() const { return eventFlowReport_; }
    
    /**
     * Get integration issues
     */
    std::vector<IntegrationIssue> getIntegrationIssues() const { return integrationIssues_; }
    
    /**
     * Get current system state
     */
    SystemState getSystemState() const { return currentSystemState_; }
    
    /**
     * Get debug timeline
     */
    std::vector<TimelineEvent> getDebugTimeline() const { return debugTimeline_; }
    
    /**
     * Record memory allocation
     */
    void recordMemoryAllocation(uint64_t address, uint64_t size, 
                               const std::string& site, const std::string& type,
                               const std::string& allocatorTag);
    
    /**
     * Record memory deallocation
     */
    void recordMemoryDeallocation(uint64_t address);
    
    /**
     * Record event trace
     */
    void recordEvent(const std::string& type, double timestamp,
                    const std::string& source, const std::string& target,
                    const std::string& data, const std::string& severity);
    
    /**
     * Add integration issue
     */
    void addIntegrationIssue(const std::string& type, const std::string& component,
                           const std::string& severity, const std::string& description,
                           double impact);
    
    /**
     * Update system state
     */
    void updateSystemState(const std::vector<std::string>& activeComponents,
                         const std::unordered_map<std::string, std::string>& configValues,
                         const std::unordered_map<std::string, double>& metrics);
    
    /**
     * Add timeline event
     */
    void addTimelineEvent(double timestamp, const std::string& type,
                         const std::string& description,
                         const std::vector<std::string>& tags,
                         const std::string& color, double duration);
    
    /**
     * Generate debug report
     */
    void generateDebugReport(const std::string& filename) const;
    
    /**
     * Export event traces
     */
    bool exportEventTraces(const std::string& filename) const;
    
    /**
     * Export memory allocations
     */
    bool exportMemoryAllocations(const std::string& filename) const;
    
    /**
     * Is visualization active
     */
    bool isActive() const { return active_; }
    
    /**
     * Set update callback
     */
    void setUpdateCallback(std::function<void(const SystemState&)> callback) {
        updateCallback_ = callback;
    }
    
    /**
     * Set brain reference
     */
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    /**
     * Set config reference
     */
    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    /**
     * Clear all debug data
     */
    void clearDebugData();
    
    /**
     * Filter debug data by component
     */
    void filterByComponent(const std::vector<std::string>& components);
    
    /**
     * Filter debug data by time range
     */
    void filterByTimeRange(double startTime, double endTime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool active_;
    double updateRate_;
    
    // Debug data
    std::unordered_map<uint64_t, MemoryAllocation> memoryAllocations_;
    std::vector<EventTrace> eventTraces_;
    std::vector<IntegrationIssue> integrationIssues_;
    SystemState currentSystemState_;
    std::vector<TimelineEvent> debugTimeline_;
    
    // Reports
    MemoryLeakReport memoryLeakReport_;
    EventFlowReport eventFlowReport_;
    
    // Callbacks and references
    std::function<void(const SystemState&)> updateCallback_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
    // Helper methods
    void collectMemoryLeaks();
    void analyzeEventFlows();
    void detectIntegrationProblems();
    void updateDebugReports();
    void analyzeTimelineData();
    std::string formatMemoryAddress(uint64_t address) const;
    std::string getEventTypeIcon(const std::string& type) const;
    std::string getIssueSeverityColor(const std::string& severity) const;
};

} // namespace nlm
