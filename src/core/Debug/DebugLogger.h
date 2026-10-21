// DebugEvent.h
#pragma once

#include <string>
#include <chrono>
#include <iostream>

namespace nlm {

// Debug event types for tracking important system activities
enum class DebugEventType {
    SystemInitialization,
    BrainStep,
    MemoryUpdate,
    PredictionUpdate,
    PlanningAction,
    MotorOutput,
    Neuromodulation,
    MemoryConsolidation,
    SleepCycle,
    ErrorCondition,
    PerformanceMetrics,
    ConceptFormation,
    AttentionUpdate,
    IntegrationEvent
};

// Debug event structure
struct DebugEvent {
    DebugEventType type;
    std::string message;
    double timestamp;
    std::string file;
    int line;
    std::string context;
    
    DebugEvent(DebugEventType t, const std::string& msg, double ts, 
               const std::string& file = "", int line = 0, 
               const std::string& context = "")
        : type(t), message(msg), timestamp(ts), file(file), line(line), context(context) {}
};

// Simple debug logger for tracking system events
class DebugLogger {
public:
    DebugLogger() : enabled_(false), verbose_(false), maxEvents_(100) {}
    
    void enable(bool enabled = true) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    void verbose(bool verbose = true) { verbose_ = verbose; }
    bool isVerbose() const { return verbose_; }
    
    void setMaxEvents(size_t max) { maxEvents_ = max; }
    size_t getMaxEvents() const { return maxEvents_; }
    
    // Log a debug event
    void log(DebugEventType type, const std::string& message,
             const std::string& context = "",
             const std::string& file = "", int line = 0);
    
    // Convenience methods for common event types
    void logSystemInit(const std::string& message) {
        log(DebugEventType::SystemInitialization, message, "System", __FILE__, __LINE__);
    }
    
    void logBrainStep(double timestamp, int step) {
        log(DebugEventType::BrainStep, 
            "Brain step " + std::to_string(step) + " at t=" + std::to_string(timestamp),
            "Brain", __FILE__, __LINE__);
    }
    
    void logMemoryUpdate(const std::string& memoryType, size_t traces, size_t capacity) {
        log(DebugEventType::MemoryUpdate,
            memoryType + " traces: " + std::to_string(traces) + "/" + std::to_string(capacity),
            "Memory", __FILE__, __LINE__);
    }
    
    void logPrediction(float error, float confidence) {
        log(DebugEventType::PredictionUpdate,
            "Prediction error: " + std::to_string(error) + ", confidence: " + std::to_string(confidence),
            "Prediction", __FILE__, __LINE__);
    }
    
    void logPlanning(const std::string& action, size_t depth) {
        log(DebugEventType::PlanningAction,
            "Planned action: " + action + ", depth: " + std::to_string(depth),
            "Planner", __FILE__, __LINE__);
    }
    
    void logMotor(const std::string& output) {
        log(DebugEventType::MotorOutput,
            "Motor output: " + output,
            "Motor", __FILE__, __LINE__);
    }
    
    void logNeuromodulation(float dopamine, float curiosity) {
        log(DebugEventType::Neuromodulation,
            "Neuromodulation - DA: " + std::to_string(dopamine) + ", Cur: " + std::to_string(curiosity),
            "Neuromodulation", __FILE__, __LINE__);
    }
    
    void logConsolidation(size_t episodes, size_t newEpisodes) {
        log(DebugEventType::MemoryConsolidation,
            "Consolidation - " + std::to_string(episodes) + " total, " + std::to_string(newEpisodes) + " new",
            "Memory", __FILE__, __LINE__);
    }
    
    void logSleep(bool entering, bool exiting, double duration) {
        std::string state = "REST";
        if (entering && !exiting) state = "Entering REST";
        else if (exiting && !entering) state = "Exiting REST";
        else if (entering && exiting) state = "REST->ACTIVE";
        
        log(DebugEventType::SleepCycle,
            "Sleep cycle: " + state + " (duration: " + std::to_string(duration) + ")",
            "Sleep", __FILE__, __LINE__);
    }
    
    void logConcept(const std::string& concept, size_t conceptId) {
        log(DebugEventType::ConceptFormation,
            "Concept formed: " + concept + " (ID: " + std::to_string(conceptId) + ")",
            "Concepts", __FILE__, __LINE__);
    }
    
    void logAttention(const std::vector<std::string>& winners) {
        std::string winnerList = "";
        for (size_t i = 0; i < winners.size(); ++i) {
            if (i > 0) winnerList += ", ";
            winnerList += winners[i];
        }
        log(DebugEventType::AttentionUpdate,
            "Attention winners: " + winnerList,
            "Attention", __FILE__, __LINE__);
    }
    
    void logError(const std::string& error, const std::string& context) {
        log(DebugEventType::ErrorCondition,
            "ERROR: " + error,
            context, __FILE__, __LINE__);
    }
    
    void logPerformance(const std::string& metric, double value, const std::string& unit = "") {
        std::string valueStr = std::to_string(value);
        if (!unit.empty()) valueStr += " " + unit;
        log(DebugEventType::PerformanceMetrics,
            "Performance: " + metric + " = " + valueStr,
            "Metrics", __FILE__, __LINE__);
    }
    
    void logIntegration(const std::string& system1, const std::string& system2, const std::string& description) {
        log(DebugEventType::IntegrationEvent,
            "Integration: " + system1 + " + " + system2 + " -> " + description,
            "Integration", __FILE__, __LINE__);
    }
    
    // Get all events
    const std::vector<DebugEvent>& getEvents() const { return events_; }
    
    // Clear events
    void clearEvents() { events_.clear(); }
    
    // Filter events by type
    std::vector<DebugEvent> getEventsByType(DebugEventType type) const {
        std::vector<DebugEvent> result;
        for (const auto& event : events_) {
            if (event.type == type) {
                result.push_back(event);
            }
        }
        return result;
    }
    
    // Export events to file for analysis
    bool exportToFile(const std::string& filename) const;
    
    // Generate summary report
    std::string generateSummary() const;
    
private:
    std::vector<DebugEvent> events_;
    bool enabled_;
    bool verbose_;
    size_t maxEvents_;
    double startTime_;
};

} // namespace nlm
