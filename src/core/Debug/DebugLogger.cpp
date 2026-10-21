// DebugLogger.cpp

#include "Debug/DebugLogger.h"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

void DebugLogger::log(DebugEventType type, const std::string& message,
                      const std::string& context,
                      const std::string& file, int line) {
    if (!enabled_) return;
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    double ms = std::chrono::duration<double>(now.time_since_epoch()).count() * 1000.0;
    
    DebugEvent event(type, message, ms, file, line, context);
    events_.push_back(event);
    
    // Trim to max events
    if (events_.size() > maxEvents_) {
        events_.erase(events_.begin(), events_.begin() + (events_.size() - maxEvents_));
    }
    
    // Always log important events to main logger
    bool isImportant = (type == DebugEventType::ErrorCondition ||
                       type == DebugEventType::SystemInitialization ||
                       type == DebugEventType::SleepCycle ||
                       type == DebugEventType::MemoryConsolidation);
    
    if (isImportant || verbose_) {
        std::string eventTypeStr;
        switch (type) {
            case DebugEventType::SystemInitialization: eventTypeStr = "INIT"; break;
            case DebugEventType::BrainStep: eventTypeStr = "STEP"; break;
            case DebugEventType::MemoryUpdate: eventTypeStr = "MEMORY"; break;
            case DebugEventType::PredictionUpdate: eventTypeStr = "PREDICT"; break;
            case DebugEventType::PlanningAction: eventTypeStr = "PLAN"; break;
            case DebugEventType::MotorOutput: eventTypeStr = "MOTOR"; break;
            case DebugEventType::Neuromodulation: eventTypeStr = "NEURO"; break;
            case DebugEventType::MemoryConsolidation: eventTypeStr = "CONSOL"; break;
            case DebugEventType::SleepCycle: eventTypeStr = "SLEEP"; break;
            case DebugEventType::ErrorCondition: eventTypeStr = "ERROR"; break;
            case DebugEventType::PerformanceMetrics: eventTypeStr = "PERF"; break;
            case DebugEventType::ConceptFormation: eventTypeStr = "CONCEPT"; break;
            case DebugEventType::AttentionUpdate: eventTypeStr = "ATTN"; break;
            case DebugEventType::IntegrationEvent: eventTypeStr = "INTEG"; break;
            default: eventTypeStr = "UNKNOWN";
        }
        
        std::string logMessage = "[" + eventTypeStr + "] " + message;
        if (!context.empty()) {
            logMessage += " (" + context + ")";
        }
        
        NLM_LOG_INFO(logMessage);
    }
}

bool DebugLogger::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Debug Event Log" << std::endl;
    file << "===============" << std::endl;
    file << std::endl;
    
    for (const auto& event : events_) {
        // Format timestamp
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << event.timestamp;
        
        file << "[" << ss.str() << "] ";
        
        // Format event type
        std::string eventTypeStr;
        switch (event.type) {
            case DebugEventType::SystemInitialization: eventTypeStr = "INIT"; break;
            case DebugEventType::BrainStep: eventTypeStr = "STEP"; break;
            case DebugEventType::MemoryUpdate: eventTypeStr = "MEMORY"; break;
            case DebugEventType::PredictionUpdate: eventTypeStr = "PREDICT"; break;
            case DebugEventType::PlanningAction: eventTypeStr = "PLAN"; break;
            case DebugEventType::MotorOutput: eventTypeStr = "MOTOR"; break;
            case DebugEventType::Neuromodulation: eventTypeStr = "NEURO"; break;
            case DebugEventType::MemoryConsolidation: eventTypeStr = "CONSOL"; break;
            case DebugEventType::SleepCycle: eventTypeStr = "SLEEP"; break;
            case DebugEventType::ErrorCondition: eventTypeStr = "ERROR"; break;
            case DebugEventType::PerformanceMetrics: eventTypeStr = "PERF"; break;
            case DebugEventType::ConceptFormation: eventTypeStr = "CONCEPT"; break;
            case DebugEventType::AttentionUpdate: eventTypeStr = "ATTN"; break;
            case DebugEventType::IntegrationEvent: eventTypeStr = "INTEG"; break;
            default: eventTypeStr = "UNKNOWN";
        }
        
        file << "[" << eventTypeStr << "] " << event.message;
        
        if (!event.context.empty()) {
            file << " (" << event.context << ")";
        }
        
        if (!event.file.empty()) {
            file << " @ " << event.file;
            if (event.line > 0) {
                file << ":" << event.line;
            }
        }
        
        file << std::endl;
    }
    
    file << std::endl;
    file << "Total events: " << events_.size() << std::endl;
    
    return true;
}

std::string DebugLogger::generateSummary() const {
    std::stringstream ss;
    
    ss << "=== Debug Summary ===" << std::endl;
    ss << std::endl;
    
    // Count events by type
    std::unordered_map<DebugEventType, size_t> counts;
    for (const auto& event : events_) {
        counts[event.type]++;
    }
    
    ss << "Event Breakdown:" << std::endl;
    for (const auto& pair : counts) {
        std::string eventTypeStr;
        switch (pair.first) {
            case DebugEventType::SystemInitialization: eventTypeStr = "System Initialization"; break;
            case DebugEventType::BrainStep: eventTypeStr = "Brain Steps"; break;
            case DebugEventType::MemoryUpdate: eventTypeStr = "Memory Updates"; break;
            case DebugEventType::PredictionUpdate: eventTypeStr = "Prediction Updates"; break;
            case DebugEventType::PlanningAction: eventTypeStr = "Planning Actions"; break;
            case DebugEventType::MotorOutput: eventTypeStr = "Motor Outputs"; break;
            case DebugEventType::Neuromodulation: eventTypeStr = "Neuromodulation"; break;
            case DebugEventType::MemoryConsolidation: eventTypeStr = "Memory Consolidation"; break;
            case DebugEventType::SleepCycle: eventTypeStr = "Sleep Cycles"; break;
            case DebugEventType::ErrorCondition: eventTypeStr = "Errors"; break;
            case DebugEventType::PerformanceMetrics: eventTypeStr = "Performance Metrics"; break;
            case DebugEventType::ConceptFormation: eventTypeStr = "Concept Formation"; break;
            case DebugEventType::AttentionUpdate: eventTypeStr = "Attention Updates"; break;
            case DebugEventType::IntegrationEvent: eventTypeStr = "Integration Events"; break;
            default: eventTypeStr = "Unknown";
        }
        
        ss << "  " << eventTypeStr << ": " << pair.second << std::endl;
    }
    
    ss << std::endl;
    ss << "Time Range: " << events_.front().timestamp << " - " << events_.back().timestamp << " ms" << std::endl;
    ss << "Total Duration: " << (events_.back().timestamp - events_.front().timestamp) << " ms" << std::endl;
    
    // Highlight important events
    ss << std::endl;
    ss << "Key Observations:" << std::endl;
    
    if (counts.find(DebugEventType::ErrorCondition) != counts.end() && counts[DebugEventType::ErrorCondition] > 0) {
        ss << "  - System experienced " << counts[DebugEventType::ErrorCondition] 
           << " error condition(s)" << std::endl;
    }
    
    if (counts.find(DebugEventType::SleepCycle) != counts.end()) {
        ss << "  - System entered rest/rest cycle " << counts[DebugEventType::SleepCycle] 
           << " time(s)" << std::endl;
    }
    
    if (counts.find(DebugEventType::MemoryConsolidation) != counts.end()) {
        ss << "  - Memory consolidation occurred " << counts[DebugEventType::MemoryConsolidation] 
           << " time(s)" << std::endl;
    }
    
    if (counts.find(DebugEventType::ConceptFormation) != counts.end()) {
        ss << "  - " << counts[DebugEventType::ConceptFormation] 
           << " concepts were formed" << std::endl;
    }
    
    ss << std::endl;
    ss << "Debug logging completed successfully.";
    
    return ss.str();
}

} // namespace nlm
