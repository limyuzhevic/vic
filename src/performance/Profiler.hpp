// Performance profiling utilities for NLM
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <algorithm>

namespace nlm {

// Profiling data structures
struct ProfileEvent {
    std::string name;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::string threadId;
    std::unordered_map<std::string, std::string> metadata;
    
    ProfileEvent(const std::string& eventName) : name(eventName) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
    }
    
    double getDurationMs() const {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
};

class Profiler {
public:
    Profiler() : enabled_(false), maxEvents_(1000) {}
    
    void enable(bool enabled = true) {
        enabled_ = enabled;
        if (enabled_) {
            clear();
            startThread();
        }
    }
    
    void disable() {
        enabled_ = false;
        stopThread();
    }
    
    void startEvent(const std::string& name, const std::unordered_map<std::string, std::string>& metadata = {}) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        events_.push_back(ProfileEvent(name));
        events_.back().metadata = metadata;
    }
    
    void stopEvent(size_t eventIndex) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        if (eventIndex < events_.size()) {
            events_[eventIndex].stop();
        }
    }
    
    size_t startScopedEvent(const std::string& name, const std::unordered_map<std::string, std::string>& metadata = {}) {
        if (!enabled_) return 0;
        
        std::lock_guard<std::mutex> lock(mutex_);
        size_t index = events_.size();
        events_.push_back(ProfileEvent(name));
        events_.back().metadata = metadata;
        return index;
    }
    
    void stopScopedEvent(size_t eventIndex) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        if (eventIndex < events_.size()) {
            events_[eventIndex].stop();
        }
    }
    
    void logMemoryUsage() {
        if (!enabled_) return;
        
        std::ostringstream oss;
        oss << "Memory usage: (placeholder - integrate with system-specific memory tracking)";
        addSystemEvent("MEMORY_USAGE", oss.str());
    }
    
    void logThreadInfo() {
        if (!enabled_) return;
        
        std::ostringstream oss;
        oss << "Thread ID: " << std::this_thread::get_id();
        addSystemEvent("THREAD_INFO", oss.str());
    }
    
    void addSystemEvent(const std::string& name, const std::string& message) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        ProfileEvent event(name);
        event.metadata["message"] = message;
        event.stop();
        events_.push_back(std::move(event));
    }
    
    std::string generateReport(bool includeDetails = true) {
        std::ostringstream oss;
        oss << "=== NLM Performance Profile ===\n";
        oss << "Total events: " << events_.size() << "\n";
        oss << "Profiling enabled: " << (enabled_ ? "yes" : "no") << "\n\n";
        
        if (events_.empty()) {
            oss << "No profiling events recorded.\n";
            return oss.str();
        }
        
        // Group events by type and calculate statistics
        std::unordered_map<std::string, std::vector<double>> eventDurations;
        std::unordered_map<std::string, size_t> eventCounts;
        
        for (const auto& event : events_) {
            if (event.endTime.time_since_epoch().count() == 0) continue; // Ongoing event
            
            double duration = event.getDurationMs();
            eventDurations[event.name].push_back(duration);
            eventCounts[event.name]++;
        }
        
        oss << "Event Summary:\n";
        for (const auto& pair : eventDurations) {
            const std::string& name = pair.first;
            const std::vector<double>& durations = pair.second;
            
            double total = std::accumulate(durations.begin(), durations.end(), 0.0);
            double avg = total / durations.size();
            double min = *std::min_element(durations.begin(), durations.end());
            double max = *std::max_element(durations.begin(), durations.end());
            
            oss << "  " << name << ": " << eventCounts[name] << " occurrences, "
                << std::fixed << std::setprecision(3) << total << "ms total, "
                << avg << "ms avg, " << min << "ms min, " << max << "ms max\n";
        }
        
        if (includeDetails) {
            oss << "\nDetailed Events:\n";
            for (size_t i = 0; i < events_.size(); ++i) {
                const auto& event = events_[i];
                if (event.endTime.time_since_epoch().count() == 0) continue;
                
                oss << "  " << i << ". " << event.name << ": "
                    << std::fixed << std::setprecision(3) << event.getDurationMs() << "ms";
                
                if (!event.metadata.empty()) {
                    oss << " [Metadata: ";
                    bool first = true;
                    for (const auto& meta : event.metadata) {
                        if (!first) oss << ", ";
                        oss << meta.first << "=" << meta.second;
                        first = false;
                    }
                    oss << "]";
                }
                oss << "\n";
            }
        }
        
        return oss.str();
    }
    
    void saveReport(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << generateReport(true);
            file.close();
            std::cout << "Profile report saved to: " << filename << std::endl;
        } else {
            std::cerr << "ERROR: Could not open file " << filename << " for writing profile report" << std::endl;
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.clear();
    }
    
    size_t getEventCount() const {
        return events_.size();
    }
    
    bool isEnabled() const {
        return enabled_;
    }
    
private:
    void startThread() {
        // Thread management for real-time profiling
    }
    
    void stopThread() {
        // Thread cleanup
    }
    
    mutable std::mutex mutex_;
    std::vector<ProfileEvent> events_;
    bool enabled_;
    size_t maxEvents_;
};

// Global profiler instance
extern Profiler& getGlobalProfiler();

// Convenience macros for profiling
#define PROFILE_START(name) do { gProfiler.startEvent(name); } while(0)
#define PROFILE_STOP(eventIndex) do { gProfiler.stopEvent(eventIndex); } while(0)
#define PROFILE_SCOPED(name) do { static size_t _scoped_event_index = gProfiler.startScopedEvent(name); } while(0)
#define PROFILE_SCOPED_WITH_META(name, meta) do { static size_t _scoped_event_index = gProfiler.startScopedEvent(name, meta); } while(0)

// Global profiler instance declaration
extern Profiler gProfiler;

} // namespace nlm