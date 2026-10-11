#include "DebugVisualizer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <set>

namespace nlm {

struct DebugVisualizer::Impl {
    bool active;
    double updateRate;
    std::set<uint64_t> allocatedAddresses;
    std::set<uint64_t> deallocatedAddresses;
    std::unordered_map<std::string, uint64_t> eventTypeCounts;
    std::unordered_map<std::string, uint64_t> sourceCounts;
    std::unordered_set<std::string> trackedEventTypes;
    std::vector<std::string> filteredComponents;
    double timeFilterStart;
    double timeFilterEnd;
    bool timeFilterActive;
    
    Impl() : active(false), updateRate(1.0), timeFilterStart(0.0), timeFilterEnd(0.0), timeFilterActive(false) {
        trackedEventTypes = {"neuron_fire", "synaptic_update", "memory_alloc", "memory_dealloc", "system_event"};
    }
};

DebugVisualizer::DebugVisualizer() : active_(false), updateRate_(1.0) {
    pImpl = std::make_unique<Impl>();
    initialize();
}

DebugVisualizer::~DebugVisualizer() = default;

bool DebugVisualizer::initialize() {
    active_ = true;
    NLM_LOG_INFO("DebugVisualizer initialized");
    return true;
}

void DebugVisualizer::update(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!active_ || !brain) return;
    
    // Collect debug information
    collectMemoryLeaks();
    analyzeEventFlows();
    detectIntegrationProblems();
    
    // Update system state
    std::vector<std::string> activeComponents;
    std::vector<std::string> inactiveComponents;
    
    // Collect brain systems
    if (brain->getWorkingMemory()) activeComponents.push_back("working_memory");
    else inactiveComponents.push_back("working_memory");
    
    if (brain->getEpisodicMemory()) activeComponents.push_back("episodic_memory");
    else inactiveComponents.push_back("episodic_memory");
    
    if (brain->getDopamine()) activeComponents.push_back("neuromodulation");
    else inactiveComponents.push_back("neuromodulation");
    
    if (brain->getPredictionSystem()) activeComponents.push_back("prediction");
    else inactiveComponents.push_back("prediction");
    
    if (brain->getDevelopmentSystem()) activeComponents.push_back("development");
    else inactiveComponents.push_back("development");
    
    if (brain->getCuriosity()) activeComponents.push_back("curiosity");
    else inactiveComponents.push_back("curiosity");
    
    if (brain->getNovelty()) activeComponents.push_back("novelty");
    else inactiveComponents.push_back("novelty");
    
    // Collect configuration values
    std::unordered_map<std::string, std::string> configValues;
    if (config) {
        std::vector<std::string> configKeys = {
            "neuron_count", "region_count", "connection_probability",
            "working_memory_size", "memory_consolidation_interval",
            "neuromodulation_level", "development_stage"
        };
        
        for (const auto& key : configKeys) {
            if (config->has(key)) {
                if (std::is_same<decltype(config->getOr<int64_t>("", 0))>::value) {
                    configValues[key] = std::to_string(config->getOr<int64_t>(key, 0));
                } else if (std::is_same<decltype(config->getOr<double>("", 0.0))>::value) {
                    configValues[key] = std::to_string(config->getOr<double>(key, 0.0));
                } else if (std::is_same<decltype(config->getOr<std::string>("", ""))>::value) {
                    configValues[key] = config->getOr<std::string>(key, "");
                }
            }
        }
    }
    
    // Collect system metrics
    std::unordered_map<std::string, double> systemMetrics;
    systemMetrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
    systemMetrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
    systemMetrics["avg_firing_rate"] = brain->getAverageFiringRate();
    systemMetrics["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
    
    updateSystemState(activeComponents, inactiveComponents, configValues, systemMetrics);
    
    // Update debug reports
    updateDebugReports();
    
    if (updateCallback_) {
        updateCallback_(currentSystemState_);
    }
}

void DebugVisualizer::renderMemoryLeakDetector() const {
    std::cout << "\n=== Memory Leak Detector ===" << std::endl;
    
    generateMemoryLeakReport();
    
    std::cout << "\nMemory Allocation Statistics:" << std::endl;
    std::cout << "  Total Allocations: " << memoryAllocations_.size() << std::endl;
    std::cout << "  Active Allocations: " << pImpl->allocatedAddresses.size() << std::endl;
    std::cout << "  Deallocated: " << pImpl->deallocatedAddresses.size() << std::endl;
    
    std::cout << "\nAllocation Types:" << std::endl;
    std::unordered_map<std::string, uint64_t> typeCounts;
    for (const auto& pair : memoryAllocations_) {
        typeCounts[pair.second.allocationType]++;
    }
    
    for (const auto& pair : typeCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    // Show leaked allocations
    std::cout << "\nLeaked Allocations:" << std::endl;
    uint64_t totalLeakedSize = 0;
    for (const auto& pair : memoryAllocations_) {
        if (pair.second.isLeaked) {
            std::cout << "  Address: " << formatMemoryAddress(pair.first) << std::endl;
            std::cout << "    Size: " << pair.second.size << " bytes" << std::endl;
            std::cout << "    Site: " << pair.second.allocationSite << std::endl;
            std::cout << "    Type: " << pair.second.allocationType << std::endl;
            std::cout << "    Allocator: " << pair.second.allocatorTag << std::endl;
            std::cout << std::endl;
            
            totalLeakedSize += pair.second.size;
        }
    }
    
    std::cout << "Total Leaked Memory: " << totalLeakedSize << " bytes ("
              << (totalLeakedSize / 1024.0) << " KB)" << std::endl;
    
    if (memoryLeakReport_.hasLeaks) {
        std::cout << "\n[ALERT] Memory leaks detected!" << std::endl;
    }
}

void DebugVisualizer::renderEventFlowTracer() const {
    std::cout << "\n=== Event Flow Tracer ===" << std::endl;
    
    generateEventFlowReport();
    
    std::cout << "\nEvent Processing Statistics:" << std::endl;
    std::cout << "  Total Events: " << eventFlowReport_.totalEvents << std::endl;
    std::cout << "  Events Tracked: " << eventFlowReport_.eventsTracked << std::endl;
    
    std::cout << "\nEvent Types by Frequency:" << std::endl;
    for (const auto& pair : eventFlowReport_.eventTypeCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << " ("
                  << (eventFlowReport_.totalEvents > 0 ? 
                      static_cast<double>(pair.second) * 100.0 / eventFlowReport_.totalEvents : 0.0)
                  << "%)" << std::endl;
    }
    
    std::cout << "\nSources by Frequency:" << std::endl;
    for (const auto& pair : eventFlowReport_.sourceCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    std::cout << "\nSignificant Events (Needs Attention):" << std::endl;
    for (const auto& event : eventFlowReport_.significantEvents) {
        std::cout << "  " << event.eventType << " at " << event.timestamp << "s";
        std::cout << " (" << event.severity << ")" << std::endl;
        std::cout << "    From: " << event.source << " To: " << event.target << std::endl;
        std::cout << "    Flow: " << event.flowPath << std::endl;
        std::cout << std::endl;
    }
}

void DebugVisualizer::renderIntegrationIssueDetector() const {
    std::cout << "\n=== Integration Issue Detector ===" << std::endl;
    
    if (integrationIssues_.empty()) {
        std::cout << "No integration issues detected." << std::endl;
        return;
    }
    
    std::cout << "Integration Issues Found: " << integrationIssues_.size() << std::endl;
    
    for (size_t i = 0; i < integrationIssues_.size(); ++i) {
        const auto& issue = integrationIssues_[i];
        
        std::cout << "\n" << (i + 1) << ". " << issue.issueType << std::endl;
        std::cout << "   Component: " << issue.component << std::endl;
        std::cout << "   Severity: " << issue.severity << " (Impact: " 
                  << std::fixed << std::setprecision(1) << (issue.impactScore * 100) << "%)" << std::endl;
        std::cout << "   Description: " << issue.description << std::endl;
        std::cout << "   Status: " << (issue.isResolved ? "RESOLVED" : "UNRESOLVED") << std::endl;
        
        if (!issue.suggestedFixes.empty()) {
            std::cout << "   Suggested Fixes:" << std::endl;
            for (const auto& fix : issue.suggestedFixes) {
                std::cout << "     - " << fix << std::endl;
            }
        }
        
        std::string severityColor = getIssueSeverityColor(issue.severity);
        std::cout << "   " << severityColor << "-> " << issue.severity << " alert" << std::endl;
    }
}

void DebugVisualizer::renderSystemStateInspector() const {
    std::cout << "\n=== System State Inspector ===" << std::endl;
    
    std::cout << "System State ID: " << currentSystemState_.stateId << std::endl;
    std::cout << "Session ID: " << currentSystemState_.sessionId << std::endl;
    std::cout << "Timestamp: " << std::fixed << std::setprecision(2) 
              << currentSystemState_.timestamp << "s ago" << std::endl;
    
    std::cout << "\nActive Components: " << currentSystemState_.activeComponents.size() << std::endl;
    for (const auto& component : currentSystemState_.activeComponents) {
        std::cout << "  • " << component << std::endl;
    }
    
    std::cout << "\nInactive Components: " << currentSystemState_.inactiveComponents.size() << std::endl;
    for (const auto& component : currentSystemState_.inactiveComponents) {
        std::cout << "  ✗ " << component << std::endl;
    }
    
    std::cout << "\nConfiguration Values:" << std::endl;
    for (const auto& pair : currentSystemState_.configurationValues) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    std::cout << "\nSystem Metrics:" << std::endl;
    for (const auto& pair : currentSystemState_.systemMetrics) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    std::cout << "\nRecent Events:" << std::endl;
    for (const auto& event : currentSystemState_.recentEvents) {
        std::cout << "  " << event << std::endl;
    }
}

void DebugVisualizer::renderDebugTimeline() const {
    std::cout << "\n=== Debug Timeline Visualizer ===" << std::endl;
    
    if (debugTimeline_.empty()) {
        std::cout << "No timeline events recorded." << std::endl;
        return;
    }
    
    std::cout << "Timeline Events: " << debugTimeline_.size() << std::endl;
    
    // Find time range
    double minTime = std::numeric_limits<double>::max();
    double maxTime = std::numeric_limits<double>::lowest();
    for (const auto& event : debugTimeline_) {
        minTime = std::min(minTime, event.timestamp);
        maxTime = std::max(maxTime, event.timestamp + event.duration);
    }
    
    double timeRange = maxTime - minTime;
    if (timeRange < 1e-6) timeRange = 1.0; // Avoid division by zero
    
    std::cout << "Time Range: " << std::fixed << std::setprecision(2) 
              << minTime << " - " << maxTime << "s (" << timeRange << "s total)" << std::endl;
    
    std::cout << "\nTimeline Events (chronological):" << std::endl;
    for (const auto& event : debugTimeline_) {
        // Calculate position on timeline
        double position = (event.timestamp - minTime) / timeRange * 60.0; // Scale to 60 chars
        double duration = event.duration / timeRange * 60.0;
        
        std::cout << "  ";
        
        // Print timeline markers
        for (int i = 0; i < 60; ++i) {
            if (i >= position && i < position + duration) {
                std::cout << "█";
            } else {
                std::cout << "─";
            }
        }
        
        std::cout << " " << event.eventType << " at " << std::fixed << std::setprecision(2) 
                  << event.timestamp << "s";
        
        if (!event.description.empty()) {
            std::cout << " - " << event.description;
        }
        
        std::cout << " " << getEventTypeIcon(event.eventType);
        
        std::cout << std::endl;
    }
}

void DebugVisualizer::generateDebugReport(const std::string& filename) const {
    std::cout << "Generating debug report to " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create debug report file: " << filename << std::endl;
        return;
    }
    
    file << "=== Phase 6 Debug Report ===" << std::endl;
    file << "Generated at: " << std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count() << "s ago" << std::endl;
    file << "\n=== Memory Leak Analysis ===" << std::endl;
    file << "Memory Leaks Detected: " << (memoryLeakReport_.hasLeaks ? "YES" : "NO") << std::endl;
    file << "Total Allocated: " << memoryLeakReport_.totalAllocated << " bytes" << std::endl;
    file << "Total Leaked: " << memoryLeakReport_.totalLeaked << " bytes" << std::endl;
    file << "Leak Count: " << memoryLeakReport_.leakCount << std::endl;
    
    file << "\n=== Event Flow Analysis ===" << std::endl;
    file << "Total Events: " << eventFlowReport_.totalEvents << std::endl;
    file << "Events Tracked: " << eventFlowReport_.eventsTracked << std::endl;
    
    file << "\n=== Integration Issues ===" << std::endl;
    file << "Total Issues: " << integrationIssues_.size() << std::endl;
    
    int criticalCount = 0, highCount = 0, mediumCount = 0, lowCount = 0;
    for (const auto& issue : integrationIssues_) {
        if (issue.severity == "critical") criticalCount++;
        else if (issue.severity == "high") highCount++;
        else if (issue.severity == "medium") mediumCount++;
        else lowCount++;
    }
    
    file << "Critical: " << criticalCount << std::endl;
    file << "High: " << highCount << std::endl;
    file << "Medium: " << mediumCount << std::endl;
    file << "Low: " << lowCount << std::endl;
    
    file << "\n=== System State Summary ===" << std::endl;
    file << "Active Components: " << currentSystemState_.activeComponents.size() << std::endl;
    file << "Inactive Components: " << currentSystemState_.inactiveComponents.size() << std::endl;
    file << "Configuration Keys: " << currentSystemState_.configurationValues.size() << std::endl;
    file << "Metrics Collected: " << currentSystemState_.systemMetrics.size() << std::endl;
    
    file << "\n=== Timeline Events ===" << std::endl;
    file << "Timeline Events: " << debugTimeline_.size() << std::endl;
    
    if (debugTimeline_.size() > 0) {
        double earliest = debugTimeline_[0].timestamp;
        double latest = debugTimeline_.back().timestamp;
        file << "Time Range: " << std::fixed << std::setprecision(2) 
             << earliest << " - " << latest << "s" << std::endl;
    }
    
    file << "\n=== End Debug Report ===" << std::endl;
    file.close();
}

bool DebugVisualizer::exportEventTraces(const std::string& filename) const {
    std::cout << "Exporting event traces to " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create event trace file: " << filename << std::endl;
        return false;
    }
    
    file << "event_id,event_type,timestamp,source,target,data,severity" << std::endl;
    
    for (const auto& event : eventTraces_) {
        file << event.eventId << "," << event.eventType << ","
             << std::fixed << std::setprecision(6) << event.timestamp << ","
             << event.source << "," << event.target << ","
             << event.data << "," << event.severity << std::endl;
    }
    
    file.close();
    return true;
}

bool DebugVisualizer::exportMemoryAllocations(const std::string& filename) const {
    std::cout << "Exporting memory allocations to " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create memory allocation file: " << filename << std::endl;
        return false;
    }
    
    file << "address,size,allocation_site,allocation_type,timestamp,is_leaked,allocator_tag" << std::endl;
    
    for (const auto& pair : memoryAllocations_) {
        const auto& alloc = pair.second;
        file << formatMemoryAddress(alloc.address) << ","
             << alloc.size << ","
             << alloc.allocationSite << ","
             << alloc.allocationType << ","
             << std::fixed << std::setprecision(6) << 
                std::chrono::duration<double>(alloc.timestamp.time_since_epoch()).count() << ","
             << (alloc.isLeaked ? "true" : "false") << ","
             << alloc.allocatorTag << std::endl;
    }
    
    file.close();
    return true;
}

void DebugVisualizer::recordMemoryAllocation(uint64_t address, uint64_t size,
                                           const std::string& site, const std::string& type,
                                           const std::string& allocatorTag) {
    MemoryAllocation alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.allocationSite = site;
    alloc.allocationType = type;
    alloc.timestamp = std::chrono::steady_clock::now();
    alloc.isLeaked = true; // Assume leaked until deallocated
    alloc.allocatorTag = allocatorTag;
    alloc.allocationId = "alloc_" + std::to_string(address);
    
    memoryAllocations_[address] = alloc;
    pImpl->allocatedAddresses.insert(address);
    
    NLM_LOG_DEBUG("Memory allocation recorded: address=" + std::to_string(address) + 
                 " size=" + std::to_string(size) + " site=" + site);
}

void DebugVisualizer::recordMemoryDeallocation(uint64_t address) {
    auto it = memoryAllocations_.find(address);
    if (it != memoryAllocations_.end()) {
        it->second.isLeaked = false;
        pImpl->deallocatedAddresses.insert(address);
        
        NLM_LOG_DEBUG("Memory deallocation recorded: address=" + std::to_string(address));
    }
}

void DebugVisualizer::recordEvent(const std::string& type, double timestamp,
                                 const std::string& source, const std::string& target,
                                 const std::string& data, const std::string& severity) {
    EventTrace event;
    event.eventId = "event_" + std::to_string(eventTraces_.size());
    event.eventType = type;
    event.timestamp = timestamp;
    event.source = source;
    event.target = target;
    event.data = data;
    event.severity = severity;
    
    // Calculate flow path
    event.flowPath = source + " -> " + target;
    
    eventTraces_.push_back(event);
    
    pImpl->eventTypeCounts[type]++;
    pImpl->sourceCounts[source]++;
    
    // Check if event is significant
    if (severity == "critical" || severity == "error" || type == "memory_alloc" || 
        type == "memory_dealloc") {
        eventFlowReport_.significantEvents.push_back(event);
    }
    
    NLM_LOG_DEBUG("Event recorded: type=" + type + " source=" + source + " severity=" + severity);
}

void DebugVisualizer::addIntegrationIssue(const std::string& type, const std::string& component,
                                         const std::string& severity, const std::string& description,
                                         double impact) {
    IntegrationIssue issue;
    issue.issueId = "issue_" + std::to_string(integrationIssues_.size());
    issue.issueType = type;
    issue.component = component;
    issue.severity = severity;
    issue.description = description;
    issue.impactScore = impact;
    issue.timestamp = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    integrationIssues_.push_back(issue);
    
    NLM_LOG_WARNING("Integration issue detected: " + type + " in " + component + ": " + description);
}

void DebugVisualizer::updateSystemState(const std::vector<std::string>& activeComponents,
                                       const std::vector<std::string>& inactiveComponents,
                                       const std::unordered_map<std::string, std::string>& configValues,
                                       const std::unordered_map<std::string, double>& metrics) {
    currentSystemState_.timestamp = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    currentSystemState_.stateId = "state_" + std::to_string(currentSystemState_.timestamp);
    currentSystemState_.sessionId = "session_" + std::to_string(rand());
    
    currentSystemState_.activeComponents = activeComponents;
    currentSystemState_.inactiveComponents = inactiveComponents;
    currentSystemState_.configurationValues = configValues;
    currentSystemState_.systemMetrics = metrics;
    
    // Add recent events
    currentSystemState_.recentEvents.push_back(
        "System state updated at " + std::to_string(currentSystemState_.timestamp) + "s");
    
    // Keep only recent events
    if (currentSystemState_.recentEvents.size() > 100) {
        currentSystemState_.recentEvents.erase(
            currentSystemState_.recentEvents.begin());
    }
}

void DebugVisualizer::addTimelineEvent(double timestamp, const std::string& type,
                                      const std::string& description,
                                      const std::vector<std::string>& tags,
                                      const std::string& color, double duration) {
    TimelineEvent event;
    event.timestamp = timestamp;
    event.eventId = "timeline_" + std::to_string(debugTimeline_.size());
    event.eventType = type;
    event.description = description;
    event.tags = tags;
    event.color = color;
    event.duration = duration;
    
    debugTimeline_.push_back(event);
}

void DebugVisualizer::collectMemoryLeaks() {
    generateMemoryLeakReport();
    
    // Count leaked allocations
    uint64_t totalAllocated = 0;
    uint64_t totalLeaked = 0;
    uint64_t leakCount = 0;
    
    for (const auto& pair : memoryAllocations_) {
        totalAllocated += pair.second.size;
        if (pair.second.isLeaked) {
            totalLeaked += pair.second.size;
            leakCount++;
        }
    }
    
    memoryLeakReport_.hasLeaks = (leakCount > 0);
    memoryLeakReport_.totalAllocated = totalAllocated;
    memoryLeakReport_.totalLeaked = totalLeaked;
    memoryLeakReport_.leakCount = leakCount;
    
    // Generate human-readable report
    std::stringstream report;
    report << "Memory Leak Analysis Report" << std::endl;
    report << "===========================" << std::endl;
    report << "Total Allocations: " << memoryAllocations_.size() << std::endl;
    report << "Leaked Allocations: " << leakCount << std::endl;
    report << "Total Leaked Memory: " << totalLeaked << " bytes ("
           << (totalLeaked / 1024.0) << " KB)" << std::endl;
    
    if (leakCount > 0) {
        report << "\n[ALERT] Memory leaks detected!" << std::endl;
        for (const auto& pair : memoryAllocations_) {
            if (pair.second.isLeaked) {
                report << "  Leak at " << formatMemoryAddress(pair.first) 
                       << " size: " << pair.second.size << " bytes" << std::endl;
            }
        }
    }
    
    memoryLeakReport_.report = report.str();
}

void DebugVisualizer::analyzeEventFlows() {
    generateEventFlowReport();
    
    // Count events
    uint64_t totalEvents = static_cast<uint64_t>(eventTraces_.size());
    eventFlowReport_.totalEvents = totalEvents;
    eventFlowReport_.eventsTracked = static_cast<uint64_t>(eventTraces_.size());
    
    // Count by type and source
    eventFlowReport_.eventTypeCounts.clear();
    eventFlowReport_.sourceCounts.clear();
    
    for (const auto& event : eventTraces_) {
        eventFlowReport_.eventTypeCounts[event.eventType]++;
        eventFlowReport_.sourceCounts[event.source]++;
    }
    
    // Generate report
    std::stringstream report;
    report << "Event Flow Analysis Report" << std::endl;
    report << "==========================" << std::endl;
    report << "Total Events Processed: " << totalEvents << std::endl;
    report << "Events Tracked: " << eventFlowReport_.eventsTracked << std::endl;
    report << "Event Types: " << eventFlowReport_.eventTypeCounts.size() << std::endl;
    report << "Source Components: " << eventFlowReport_.sourceCounts.size() << std::endl;
    
    if (eventFlowReport_.significantEvents.size() > 0) {
        report << "\n[ALERT] Significant events detected:" << std::endl;
        for (const auto& event : eventFlowReport_.significantEvents) {
            report << "  - " << event.eventType << " at " << event.timestamp 
                   << "s (" << event.severity << ")" << std::endl;
        }
    }
    
    eventFlowReport_.flowReport = report.str();
}

void DebugVisualizer::detectIntegrationProblems() {
    // Check for integration issues based on current state
    for (const auto& component : currentSystemState_.activeComponents) {
        if (component == "working_memory" || component == "episodic_memory") {
            // Check for memory system issues
            addIntegrationIssue("memory_system", component, "warning",
                               "Memory system active but may need consolidation",
                               0.3);
        } else if (component == "neuromodulation") {
            // Check for neuromodulation issues
            addIntegrationIssue("neuromodulation", component, "info",
                               "Neuromodulation system active",
                               0.1);
        } else if (component == "prediction") {
            // Check for prediction issues
            addIntegrationIssue("prediction", component, "info",
                               "Prediction system active",
                               0.1);
        } else if (component == "development") {
            // Check for development issues
            addIntegrationIssue("development", component, "info",
                               "Development system active",
                               0.1);
        }
    }
    
    // Check for missing dependencies
    std::set<std::string> allComponents(
        currentSystemState_.activeComponents.begin(),
        currentSystemState_.activeComponents.end()
    );
    allComponents.insert(currentSystemState_.inactiveComponents.begin(),
                         currentSystemState_.inactiveComponents.end());
    
    // Check for circular dependencies (simple check)
    std::vector<std::string> componentList(allComponents.begin(), allComponents.end());
    for (size_t i = 0; i < componentList.size(); ++i) {
        for (size_t j = i + 1; j < componentList.size(); ++j) {
            if (componentList[i] == componentList[j]) continue;
            
            // Simple dependency check - if both are active and depend on each other
            addIntegrationIssue("dependency_check", componentList[i], "info",
                               "Dependency check for " + componentList[i] + " with " + componentList[j],
                               0.05);
        }
    }
}

void DebugVisualizer::updateDebugReports() {
    collectMemoryLeaks();
    analyzeEventFlows();
    detectIntegrationProblems();
}

void DebugVisualizer::analyzeTimelineData() {
    // Analyze timeline events for patterns
    if (debugTimeline_.size() < 2) return;
    
    // Find most frequent event types
    std::unordered_map<std::string, uint64_t> typeCounts;
    for (const auto& event : debugTimeline_) {
        typeCounts[event.eventType]++;
    }
    
    // Find time patterns
    double minTime = std::numeric_limits<double>::max();
    double maxTime = std::numeric_limits<double>::lowest();
    for (const auto& event : debugTimeline_) {
        minTime = std::min(minTime, event.timestamp);
        maxTime = std::max(maxTime, event.timestamp + event.duration);
    }
    
    double timeRange = maxTime - minTime;
    if (timeRange < 1e-6) timeRange = 1.0;
    
    // Generate summary
    NLM_LOG_INFO("Debug Timeline Analysis: " << debugTimeline_.size() << " events over " 
               << std::fixed << std::setprecision(2) << timeRange << "s");
}

std::string DebugVisualizer::formatMemoryAddress(uint64_t address) const {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << address;
    return ss.str();
}

std::string DebugVisualizer::getEventTypeIcon(const std::string& type) const {
    if (type == "neuron_fire") return "🔌";
    else if (type == "synaptic_update") return "⚡";
    else if (type == "memory_alloc") return "📝";
    else if (type == "memory_dealloc") return "🗑️";
    else if (type == "system_event") return "⚙️";
    else if (type == "step") return "⏱️";
    else if (type == "checkpoint") return "💾";
    else if (type == "integration") return "🔗";
    else if (type == "performance") return "📊";
    else return "📋";
}

std::string DebugVisualizer::getIssueSeverityColor(const std::string& severity) const {
    if (severity == "critical") return "🔴";
    else if (severity == "high") return "🟠";
    else if (severity == "medium") return "🟡";
    else if (severity == "low") return "🟢";
    else return "⚪";
}

void DebugVisualizer::clearDebugData() {
    memoryAllocations_.clear();
    eventTraces_.clear();
    integrationIssues_.clear();
    debugTimeline_.clear();
    
    NLM_LOG_INFO("Debug data cleared");
}

void DebugVisualizer::filterByComponent(const std::vector<std::string>& components) {
    pImpl->filteredComponents = components;
}

void DebugVisualizer::filterByTimeRange(double startTime, double endTime) {
    pImpl->timeFilterStart = startTime;
    pImpl->timeFilterEnd = endTime;
    pImpl->timeFilterActive = true;
}

void DebugVisualizer::generateMemoryLeakReport() {
    // This is a simplified implementation
    // In a real implementation, this would properly detect memory leaks
    memoryLeakReport_.hasLeaks = false;
    memoryLeakReport_.totalAllocated = memoryAllocations_.size() * 100; // Simulated
    memoryLeakReport_.totalLeaked = 0; // Assume no leaks for now
    memoryLeakReport_.leakCount = 0;
}

void DebugVisualizer::generateEventFlowReport() {
    // This is a simplified implementation
    // In a real implementation, this would properly analyze event flows
    eventFlowReport_.totalEvents = static_cast<uint64_t>(eventTraces_.size());
    eventFlowReport_.eventsTracked = static_cast<uint64_t>(eventTraces_.size());
    
    for (const auto& event : eventTraces_) {
        eventFlowReport_.eventTypeCounts[event.eventType]++;
        eventFlowReport_.sourceCounts[event.source]++;
    }
}

} // namespace nlm
