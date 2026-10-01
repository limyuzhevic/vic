#include "Debug.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

namespace nlm {

// MemoryTracker implementation
struct MemoryTracker::Impl {
    struct AllocationInfo {
        void* address;
        size_t size;
        std::string type;
        std::string file;
        int line;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
        bool isDeallocated;
        
        AllocationInfo(void* addr, size_t sz, const std::string& t, 
                      const std::string& f, int l)
            : address(addr), size(sz), type(t), file(f), line(l),
              timestamp(std::chrono::high_resolution_clock::now()),
              threadId(std::this_thread::get_id()), isDeallocated(false) {}
    };
    
    std::vector<AllocationInfo> allocations;
    std::unordered_map<std::string, bool> trackTypes;
    mutable std::mutex mutex;
    size_t totalAllocated;
    size_t totalDeallocated;
};

MemoryTracker::MemoryTracker() : pImpl(std::make_unique<Impl>()) {
    pImpl->totalAllocated = 0;
    pImpl->totalDeallocated = 0;
}

MemoryTracker::~MemoryTracker() = default;

void MemoryTracker::trackAllocation(void* address, size_t size, 
                                   const std::string& type,
                                   const std::string& file, int line) {
    if (file.empty()) {
        return; // Skip system allocations
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool shouldTrack = trackTypes.empty() || trackTypes.find(type) != trackTypes.end();
    if (!shouldTrack) {
        return;
    }
    
    pImpl->allocations.emplace_back(address, size, type, file, line);
    pImpl->totalAllocated += size;
}

void MemoryTracker::trackDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& alloc : pImpl->allocations) {
        if (alloc.address == address && !alloc.isDeallocated) {
            alloc.isDeallocated = true;
            pImpl->totalDeallocated += alloc.size;
            return;
        }
    }
}

std::vector<MemoryTracker::AllocationInfo> MemoryTracker::checkForLeaks() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<AllocationInfo> leaks;
    for (const auto& alloc : pImpl->allocations) {
        if (!alloc.isDeallocated) {
            leaks.push_back(alloc);
        }
    }
    
    return leaks;
}

size_t MemoryTracker::getTotalAllocated() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalAllocated;
}

size_t MemoryTracker::getTotalDeallocated() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalDeallocated;
}

size_t MemoryTracker::getCurrentUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalAllocated - pImpl->totalDeallocated;
}

size_t MemoryTracker::getPeakUsage() const {
    // Return current usage for now - in a full implementation this would track historical peaks
    return getCurrentUsage();
}

void MemoryTracker::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->allocations.clear();
    pImpl->totalAllocated = 0;
    pImpl->totalDeallocated = 0;
}

void MemoryTracker::trackType(const std::string& type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackTypes[type] = true;
}

void MemoryTracker::stopTrackingType(const std::string& type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackTypes.erase(type);
}

// TimeProfiler implementation
struct TimeProfiler::Impl {
    struct ProfileData {
        std::string functionName;
        std::chrono::duration<double> totalTime;
        std::chrono::duration<double> minTime;
        std::chrono::duration<double> maxTime;
        size_t callCount;
        std::chrono::duration<double> avgTime;
        std::chrono::duration<double> stdDevTime;
        
        ProfileData(const std::string& name)
            : functionName(name), totalTime(0), minTime(0), maxTime(0),
              callCount(0), avgTime(0), stdDevTime(0) {}
    };
    
    std::unordered_map<std::string, ProfileData> profiles;
    mutable std::mutex mutex;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> currentTimes;
    bool enabled;
};

TimeProfiler::TimeProfiler() : enabled_(true) {
    pImpl = std::make_unique<Impl>();
    pImpl->enabled = true;
}

TimeProfiler::~TimeProfiler() = default;

void TimeProfiler::startProfile(const std::string& functionName) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create or get profile data
    if (pImpl->profiles.find(functionName) == pImpl->profiles.end()) {
        pImpl->profiles[functionName] = ProfileData(functionName);
    }
    
    // Start timing
    pImpl->currentTimes[functionName] = std::chrono::high_resolution_clock::now();
}

void TimeProfiler::stopProfile(const std::string& functionName) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->currentTimes.find(functionName);
    if (it == pImpl->currentTimes.end()) {
        return; // No active profile
    }
    
    // Calculate elapsed time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - it->second);
    
    auto& profile = pImpl->profiles[functionName];
    
    // Update profile data
    profile.totalTime += duration;
    if (profile.callCount == 0 || duration < profile.minTime) {
        profile.minTime = duration;
    }
    if (duration > profile.maxTime) {
        profile.maxTime = duration;
    }
    profile.callCount++;
    
    // Calculate average and standard deviation
    profile.avgTime = profile.totalTime / profile.callCount;
    if (profile.callCount > 1) {
        double variance = 0.0;
        // In a full implementation, we would collect all sample times
        profile.stdDevTime = std::chrono::duration<double>(std::sqrt(variance));
    }
    
    // Remove from current times
    pImpl->currentTimes.erase(it);
}

const TimeProfiler::ProfileData* TimeProfiler::getProfile(const std::string& functionName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->profiles.find(functionName);
    if (it == pImpl->profiles.end()) {
        return nullptr;
    }
    
    return &it->second;
}

std::vector<TimeProfiler::ProfileData> TimeProfiler::getAllProfiles() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ProfileData> result;
    result.reserve(pImpl->profiles.size());
    
    for (const auto& pair : pImpl->profiles) {
        result.push_back(pair.second);
    }
    
    return result;
}

void TimeProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->profiles.clear();
    pImpl->currentTimes.clear();
}

double TimeProfiler::getTotalTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    double total = 0.0;
    for (const auto& pair : pImpl->profiles) {
        total += pair.second.totalTime.count();
    }
    
    return total;
}

void TimeProfiler::printReport(std::ostream& out) const {
    out << "=== TIME PROFILING REPORT ===" << std::endl;
    
    if (pImpl->profiles.empty()) {
        out << "No profile data available." << std::endl;
        return;
    }
    
    // Sort profiles by total time
    std::vector<std::pair<std::string, ProfileData>> sorted;
    for (const auto& pair : pImpl->profiles) {
        sorted.emplace_back(pair.first, pair.second);
    }
    
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) {
                  return a.second.totalTime > b.second.totalTime;
              });
    
    out << std::left << std::setw(40) << "Function Name"
        << std::right << std::setw(15) << "Calls"
        << std::right << std::setw(15) << "Total Time (s)"
        << std::right << std::setw(15) << "Avg Time (ms)"
        << std::right << std::setw(15) << "Min Time (ms)"
        << std::right << std::setw(15) << "Max Time (ms)"
        << std::endl;
    
    out << std::string(120, '-') << std::endl;
    
    for (const auto& pair : sorted) {
        const auto& profile = pair.second;
        
        out << std::left << std::setw(40) << pair.first
            << std::right << std::setw(15) << profile.callCount
            << std::right << std::setw(15) << std::fixed << std::setprecision(6)
                << profile.totalTime.count()
            << std::right << std::setw(15) << std::fixed << std::setprecision(3)
                << profile.avgTime.count() * 1000.0
            << std::right << std::setw(15) << std::fixed << std::setprecision(3)
                << profile.minTime.count() * 1000.0
            << std::right << std::setw(15) << std::fixed << std::setprecision(3)
                << profile.maxTime.count() * 1000.0
            << std::endl;
    }
    
    out << std::endl;
    out << "Total time profiled: " << std::fixed << std::setprecision(3)
        << getTotalTime() << " seconds" << std::endl;
    out << "=============================" << std::endl;
}

void TimeProfiler::exportToJson(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n  \"profiles\": [" << std::endl;
    
    bool first = true;
    for (const auto& pair : pImpl->profiles) {
        if (!first) {
            file << ",\n";
        }
        
        const auto& profile = pair.second;
        
        file << "    {\n"
             << "      \"function_name\": \"" << pair.first << "\",
"
             << "      \"call_count\": " << profile.callCount << ",\n"
             << "      \"total_time\": " << std::fixed << std::setprecision(6)
                 << profile.totalTime.count() << ",\n"
             << "      \"min_time\": " << std::fixed << std::setprecision(6)
                 << profile.minTime.count() << ",\n"
             << "      \"max_time\": " << std::fixed << std::setprecision(6)
                 << profile.maxTime.count() << ",\n"
             << "      \"avg_time\": " << std::fixed << std::setprecision(6)
                 << profile.avgTime.count() << "\n"
             << "    }";
        
        first = false;
    }
    
    file << "\n  ]\n}" << std::endl;
}

// EventTracer implementation
struct EventTracer::Impl {
    struct Event {
        Event::Type type;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string description;
        std::unordered_map<std::string, std::string> metadata;
        std::thread::id threadId;
        std::string sourceFile;
        int sourceLine;
        
        Event(Event::Type t, const std::string& desc)
            : type(t), timestamp(std::chrono::high_resolution_clock::now()),
              description(desc), threadId(std::this_thread::get_id()),
              sourceFile(""), sourceLine(0) {}
    };
    
    std::vector<Event> events;
    mutable std::mutex mutex;
    bool enabled;
    bool running;
};

EventTracer::EventTracer() : enabled_(true), running_(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->enabled = true;
    pImpl->running = false;
}

EventTracer::~EventTracer() = default;

void EventTracer::recordEvent(Event::Type type, const std::string& description) {
    if (!enabled_) return;
    
    Event event(type, description);
    recordEventInternal(event);
}

void EventTracer::recordEventInternal(const Event& event) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!running_) {
        pImpl->events.push_back(event);
        return;
    }
    
    // For running tracers, we might batch events or process them immediately
    pImpl->events.push_back(event);
}

std::vector<EventTracer::Event> EventTracer::getEventsByType(Event::Type type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Event> result;
    for (const auto& event : pImpl->events) {
        if (event.type == type) {
            result.push_back(event);
        }
    }
    
    return result;
}

std::vector<EventTracer::Event> EventTracer::getEventsInRange(
    const std::chrono::high_resolution_clock::time_point& start,
    const std::chrono::high_resolution_clock::time_point& end
) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Event> result;
    for (const auto& event : pImpl->events) {
        if (event.timestamp >= start && event.timestamp <= end) {
            result.push_back(event);
        }
    }
    
    return result;
}

std::vector<EventTracer::Event> EventTracer::getEventsByThread(std::thread::id threadId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Event> result;
    for (const auto& event : pImpl->events) {
        if (event.threadId == threadId) {
            result.push_back(event);
        }
    }
    
    return result;
}

size_t EventTracer::getEventCount(Event::Type type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    size_t count = 0;
    for (const auto& event : pImpl->events) {
        if (event.type == type) {
            count++;
        }
    }
    
    return count;
}

double EventTracer::getEventFrequency(Event::Type type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->events.empty()) {
        return 0.0;
    }
    
    size_t count = getEventCount(type);
    
    // Calculate time span
    auto minTime = std::min_element(pImpl->events.begin(), pImpl->events.end(),
        [](const Event& a, const Event& b) {
            return a.timestamp < b.timestamp;
        })->timestamp;
    
    auto maxTime = std::max_element(pImpl->events.begin(), pImpl->events.end(),
        [](const Event& a, const Event& b) {
            return a.timestamp < b.timestamp;
        })->timestamp;
    
    double timeSpan = std::chrono::duration<double>(maxTime - minTime).count();
    if (timeSpan == 0.0) {
        return count;
    }
    
    return static_cast<double>(count) / timeSpan;
}

void EventTracer::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->events.clear();
}

void EventTracer::start() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->running = true;
}

void EventTracer::stop() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->running = false;
}

void EventTracer::exportToJson(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n  \"events\": [" << std::endl;
    
    bool first = true;
    for (const auto& event : pImpl->events) {
        if (!first) {
            file << ",\n";
        }
        
        // Convert timestamp to ISO format
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            event.timestamp.time_since_epoch()
        );
        
        file << "    {\n"
             << "      \"type\": \"" << static_cast<int>(event.type) << "\",
"
             << "      \"timestamp\": " << duration.count() << ",\n"
             << "      \"description\": \"" << event.description << "\",
"
             << "      \"thread_id\": \"" << event.threadId << "\",
"
             << "      \"source_file\": \"" << event.sourceFile << "\",
"
             << "      \"source_line\": " << event.sourceLine << ",\n"
             << "      \"metadata\": {\n";
        
        bool firstMeta = true;
        for (const auto& meta : event.metadata) {
            if (!firstMeta) {
                file << ",\n";
            }
            file << "        \"" << meta.first << "\": \"" << meta.second << "\"";
            firstMeta = false;
        }
        
        file << "\n      }\n    }";
        
        first = false;
    }
    
    file << "\n  ]\n}" << std::endl;
}

std::string EventTracer::analyzePatterns() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->events.empty()) {
        return "No events to analyze.";
    }
    
    std::stringstream ss;
    
    // Count event types
    std::unordered_map<Event::Type, size_t> typeCounts;
    for (const auto& event : pImpl->events) {
        typeCounts[event.type]++;
    }
    
    ss << "=== EVENT PATTERN ANALYSIS ===" << std::endl;
    ss << "Total events: " << pImpl->events.size() << std::endl;
    
    for (const auto& pair : typeCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second
           << " events (" 
           << std::fixed << std::setprecision(2)
           << (static_cast<double>(pair.second) / pImpl->events.size() * 100.0)
           << "%)" << std::endl;
    }
    
    // Calculate time between events of same type
    ss << std::endl << "Time analysis by event type:" << std::endl;
    
    for (const auto& pair : typeCounts) {
        ss << "  Type " << static_cast<int>(pair.first) << ": ";
        
        std::vector<Event> typeEvents;
        for (const auto& event : pImpl->events) {
            if (event.type == pair.first) {
                typeEvents.push_back(event);
            }
        }
        
        if (typeEvents.size() < 2) {
            ss << "Not enough events for timing analysis." << std::endl;
            continue;
        }
        
        std::sort(typeEvents.begin(), typeEvents.end(),
                  [](const Event& a, const Event& b) {
                      return a.timestamp < b.timestamp;
                  });
        
        std::vector<double> intervals;
        for (size_t i = 1; i < typeEvents.size(); ++i) {
            double interval = std::chrono::duration<double>(
                typeEvents[i].timestamp - typeEvents[i-1].timestamp
            ).count();
            intervals.push_back(interval);
        }
        
        if (!intervals.empty()) {
            double minInterval = *std::min_element(intervals.begin(), intervals.end());
            double maxInterval = *std::max_element(intervals.begin(), intervals.end());
            double avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
            
            ss << "Min: " << minInterval << "s, Max: " << maxInterval << "s, Avg: " << avgInterval << "s" << std::endl;
        }
    }
    
    return ss.str();
}

// Debugger implementation
struct Debugger::Impl {
    std::vector<Breakpoint> breakpoints;
    std::vector<Watchpoint> watchpoints;
    std::mutex mutex;
    bool stepOver;
    bool stepInto;
    bool stepOut;
    bool paused;
    std::string currentState;
};

Debugger::Debugger() : paused_(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->stepOver = false;
    pImpl->stepInto = false;
    pImpl->stepOut = false;
    pImpl->paused = false;
    pImpl->currentState = "running";
}

Debugger::~Debugger() = default;

void Debugger::addBreakpoint(const std::string& condition, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->breakpoints.emplace_back(condition, description);
}

void Debugger::removeBreakpoint(const std::string& condition) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::remove_if(pImpl->breakpoints.begin(), pImpl->breakpoints.end(),
        [&condition](const Breakpoint& bp) {
            return bp.condition == condition;
        });
    
    pImpl->breakpoints.erase(it, pImpl->breakpoints.end());
}

void Debugger::enableBreakpoint(const std::string& condition) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& bp : pImpl->breakpoints) {
        if (bp.condition == condition) {
            bp.active = true;
            break;
        }
    }
}

void Debugger::disableBreakpoint(const std::string& condition) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& bp : pImpl->breakpoints) {
        if (bp.condition == condition) {
            bp.active = false;
            break;
        }
    }
}

void Debugger::addWatchpoint(const std::string& expression, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->watchpoints.emplace_back(expression, description);
}

void Debugger::removeWatchpoint(const std::string& expression) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::remove_if(pImpl->watchpoints.begin(), pImpl->watchpoints.end(),
        [&expression](const Watchpoint& wp) {
            return wp.expression == expression;
        });
    
    pImpl->watchpoints.erase(it, pImpl->watchpoints.end());
}

void Debugger::enableWatchpoint(const std::string& expression) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& wp : pImpl->watchpoints) {
        if (wp.expression == expression) {
            wp.active = true;
            break;
        }
    }
}

void Debugger::disableWatchpoint(const std::string& expression) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& wp : pImpl->watchpoints) {
        if (wp.expression == expression) {
            wp.active = false;
            break;
        }
    }
}

std::string Debugger::evaluateExpression(const std::string& expression) {
    // In a real implementation, this would parse and evaluate the expression
    // For now, return a placeholder
    return "DEBUG: Expression evaluation not implemented: " + expression;
}

std::string Debugger::inspectVariable(const std::string& name) {
    // In a real implementation, this would inspect the variable
    // For now, return a placeholder
    return "DEBUG: Variable inspection not implemented: " + name;
}

std::string Debugger::inspectObject(const std::string& type, size_t id) {
    // In a real implementation, this would inspect the object
    // For now, return a placeholder
    return "DEBUG: Object inspection not implemented: " + type + " " + std::to_string(id);
}

std::string Debugger::listVariables() {
    // In a real implementation, this would list all variables
    // For now, return a placeholder
    return "DEBUG: Variable listing not implemented.";
}

std::string Debugger::listBreakpoints() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "=== BREAKPOINTS ===" << std::endl;
    
    for (const auto& bp : pImpl->breakpoints) {
        ss << "Condition: " << bp.condition << std::endl;
        ss << "Description: " << bp.description << std::endl;
        ss << "Active: " << (bp.active ? "yes" : "no") << std::endl;
        ss << "Hit Count: " << bp.hitCount << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->breakpoints.empty()) {
        ss << "No breakpoints set." << std::endl;
    }
    
    return ss.str();
}

std::string Debugger::listWatchpoints() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "=== WATCHPOINTS ===" << std::endl;
    
    for (const auto& wp : pImpl->watchpoints) {
        ss << "Expression: " << wp.expression << std::endl;
        ss << "Description: " << wp.description << std::endl;
        ss << "Active: " << (wp.active ? "yes" : "no") << std::endl;
        ss << "Trigger Count: " << wp.triggerCount << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->watchpoints.empty()) {
        ss << "No watchpoints set." << std::endl;
    }
    
    return ss.str();
}

std::string Debugger::saveState() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "DEBUG_STATE" << std::endl;
    
    // Save breakpoints
    ss << pImpl->breakpoints.size() << std::endl;
    for (const auto& bp : pImpl->breakpoints) {
        ss << bp.condition << "|" << bp.description << "|" 
           << bp.active << "|" << bp.hitCount << std::endl;
    }
    
    // Save watchpoints
    ss << pImpl->watchpoints.size() << std::endl;
    for (const auto& wp : pImpl->watchpoints) {
        ss << wp.expression << "|" << wp.description << "|" 
           << wp.active << "|" << wp.triggerCount << std::endl;
    }
    
    // Save debugging state
    ss << pImpl->stepOver << "|" << pImpl->stepInto << "|" 
       << pImpl->stepOut << "|" << pImpl->paused << std::endl;
    
    return ss.str();
}

bool Debugger::loadState(const std::string& state) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss(state);
    std::string line;
    
    if (!std::getline(ss, line) || line != "DEBUG_STATE") {
        return false;
    }
    
    // Load breakpoints
    size_t breakpointCount;
    if (!(ss >> breakpointCount)) {
        return false;
    }
    ss.ignore(); // ignore newline
    
    pImpl->breakpoints.clear();
    for (size_t i = 0; i < breakpointCount; ++i) {
        std::string condition, description;
        bool active;
        size_t hitCount;
        
        if (!std::getline(ss, line, '|')) {
            return false;
        }
        condition = line.substr(0, line.find('|'));
        line.erase(0, line.find('|') + 1);
        description = line.substr(0, line.find('|'));
        line.erase(0, line.find('|') + 1);
        active = (line.substr(0, line.find('|')) == "1");
        line.erase(0, line.find('|') + 1);
        hitCount = std::stoul(line);
        
        pImpl->breakpoints.emplace_back(condition, description);
        pImpl->breakpoints.back().active = active;
        pImpl->breakpoints.back().hitCount = hitCount;
    }
    
    // Load watchpoints
    size_t watchpointCount;
    if (!(ss >> watchpointCount)) {
        return false;
    }
    ss.ignore(); // ignore newline
    
    pImpl->watchpoints.clear();
    for (size_t i = 0; i < watchpointCount; ++i) {
        std::string expression, description;
        bool active;
        size_t triggerCount;
        
        if (!std::getline(ss, line, '|')) {
            return false;
        }
        expression = line.substr(0, line.find('|'));
        line.erase(0, line.find('|') + 1);
        description = line.substr(0, line.find('|'));
        line.erase(0, line.find('|') + 1);
        active = (line.substr(0, line.find('|')) == "1");
        line.erase(0, line.find('|') + 1);
        triggerCount = std::stoul(line);
        
        pImpl->watchpoints.emplace_back(expression, description);
        pImpl->watchpoints.back().active = active;
        pImpl->watchpoints.back().triggerCount = triggerCount;
    }
    
    // Load debugging state
    std::string stepOverStr, stepIntoStr, stepOutStr, pausedStr;
    if (!std::getline(ss, stepOverStr, '|') ||
        !std::getline(ss, stepIntoStr, '|') ||
        !std::getline(ss, stepOutStr, '|') ||
        !std::getline(ss, pausedStr, '|')) {
        return false;
    }
    
    pImpl->stepOver = (stepOverStr == "1");
    pImpl->stepInto = (stepIntoStr == "1");
    pImpl->stepOut = (stepOutStr == "1");
    pImpl->paused = (pausedStr == "1");
    
    return true;
}

std::string Debugger::getCurrentStateHash() {
    // Simple hash based on current state
    std::hash<std::string> hasher;
    return std::to_string(hasher(pImpl->currentState));
}

void Debugger::stepOver() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stepOver = true;
    pImpl->paused = true;
}

void Debugger::stepInto() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stepInto = true;
    pImpl->paused = true;
}

void Debugger::stepOut() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stepOut = true;
    pImpl->paused = true;
}

void Debugger::continueExecution() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stepOver = false;
    pImpl->stepInto = false;
    pImpl->stepOut = false;
    pImpl->paused = false;
}

void Debugger::print(std::ostream& out) {
    out << "=== DEBUGGER STATUS ===" << std::endl;
    out << "Paused: " << (paused_ ? "yes" : "no") << std::endl;
    out << "Step Over: " << (pImpl->stepOver ? "yes" : "no") << std::endl;
    out << "Step Into: " << (pImpl->stepInto ? "yes" : "no") << std::endl;
    out << "Step Out: " << (pImpl->stepOut ? "yes" : "no") << std::endl;
    out << std::endl;
    
    out << "Breakpoints: " << pImpl->breakpoints.size() << std::endl;
    for (const auto& bp : pImpl->breakpoints) {
        out << "  - " << bp.description << " (condition: " << bp.condition
            << ", active: " << (bp.active ? "yes" : "no") << ")" << std::endl;
    }
    
    out << std::endl << "Watchpoints: " << pImpl->watchpoints.size() << std::endl;
    for (const auto& wp : pImpl->watchpoints) {
        out << "  - " << wp.description << " (expression: " << wp.expression
            << ", active: " << (wp.active ? "yes" : "no") << ")" << std::endl;
    }
}

bool Debugger::evaluateBreakpointCondition(const std::string& condition) {
    // In a real implementation, this would evaluate the condition
    // For now, return a simple evaluation
    return condition == "true";
}

bool Debugger::evaluateWatchpointExpression(const std::string& expression) {
    // In a real implementation, this would evaluate the expression
    // For now, return a simple evaluation
    return expression == "true";
}

// VisualDebugger implementation
struct VisualDebugger::Impl {
    // Placeholder for visual debugging state
    std::string lastNetworkVisualization;
    std::string lastActivityHeatmap;
    std::string lastConnectivityMatrix;
    std::string lastTimeSeries;
    std::string lastMemoryUsageGraph;
    std::string lastPerformanceTimeline;
};

VisualDebugger::VisualDebugger() {
    pImpl = std::make_unique<Impl>();
}

VisualDebugger::~VisualDebugger() = default;

std::string VisualDebugger::generateNetworkVisualization(const Brain& brain) const {
    // Generate a simple text-based network visualization
    std::stringstream ss;
    ss << "=== NETWORK VISUALIZATION ===" << std::endl;
    ss << "Neuron count: " << brain.getTotalNeuronCount() << std::endl;
    ss << "Synapse count: " << brain.getTotalSynapseCount() << std::endl;
    
    // Simple representation of the network
    ss << "Network structure:" << std::endl;
    ss << "  Regions: " << brain.getRegionCount() << std::endl;
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        ss << "    Region " << i << ": "
           << brain.getRegions()[i]->getPopulations().size() << " populations" << std::endl;
    }
    
    return ss.str();
}

std::string VisualDebugger::generateActivityHeatmap(const Brain& brain) const {
    std::stringstream ss;
    ss << "=== ACTIVITY HEATMAP ===" << std::endl;
    ss << "This would display a graphical heatmap of neural activity." << std::endl;
    ss << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    return ss.str();
}

std::string VisualDebugger::generateConnectivityMatrix(const Brain& brain) const {
    std::stringstream ss;
    ss << "=== CONNECTIVITY MATRIX ===" << std::endl;
    ss << "This would display a matrix showing connections between regions." << std::endl;
    ss << "Matrix size would be " << brain.getRegionCount() << " x " << brain.getRegionCount()
       << std::endl;
    
    return ss.str();
}

std::string VisualDebugger::generateTimeSeries(const Brain& brain, const std::string& metric) const {
    std::stringstream ss;
    ss << "=== TIME SERIES: " << metric << " ===" << std::endl;
    ss << "This would display a time series graph of the specified metric." << std::endl;
    ss << "Data would be plotted over time." << std::endl;
    
    return ss.str();
}

std::string VisualDebugger::generateMemoryUsageGraph() const {
    std::stringstream ss;
    ss << "=== MEMORY USAGE GRAPH ===" << std::endl;
    ss << "This would display a graph of memory usage over time." << std::endl;
    ss << "Memory pools would be shown with their current usage." << std::endl;
    
    return ss.str();
}

std::string VisualDebugger::generatePerformanceTimeline() const {
    std::stringstream ss;
    ss << "=== PERFORMANCE TIMELINE ===" << std::endl;
    ss << "This would display a timeline of performance metrics." << std::endl;
    ss << "Events, steps, and timing would be shown." << std::endl;
    
    return ss.str();
}

void VisualDebugger::generateInteractiveReport(const Brain& brain, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>NLM Debugging Report</title>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "        h1 { color: #333; }" << std::endl;
    file << "        .section { margin: 20px 0; padding: 20px; border: 1px solid #ddd; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>NLM Debugging Report</h1>" << std::endl;
    
    // Add various sections
    file << "    <div class=\"section\">" << std::endl;
    file << "        <h2>Network Summary</h2>" << std::endl;
    file << "        <p>Neurons: " << brain.getTotalNeuronCount() << "</p>" << std::endl;
    file << "        <p>Synapses: " << brain.getTotalSynapseCount() << "</p>" << std::endl;
    file << "    </div>" << std::endl;
    
    // Add more sections as needed
    
    file << "    <div class=\"section\">" << std::endl;
    file << "        <h2>Generated by NLM Debug System</h2>" << std::endl;
    file << "    </div>" << std::endl;
    
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
}

// LeakDetector implementation
std::vector<LeakDetector::LeakInfo> LeakDetector::detectLeaks() const {
    // Get memory tracker instance
    auto& tracker = MemoryTracker::getInstance();
    auto leaks = tracker.checkForLeaks();
    
    std::vector<LeakInfo> leakInfos;
    for (const auto& alloc : leaks) {
        leakInfos.emplace_back(
            alloc.type, alloc.file, alloc.line, alloc.size,
            alloc.timestamp, alloc.threadId
        );
    }
    
    return leakInfos;
}

void LeakDetector::generateLeakReport(std::ostream& out) const {
    auto leaks = detectLeaks();
    
    if (leaks.empty()) {
        out << "No memory leaks detected." << std::endl;
        return;
    }
    
    out << "=== MEMORY LEAK REPORT ===" << std::endl;
    out << "Total leaks: " << leaks.size() << std::endl;
    out << std::endl;
    
    // Group leaks by type
    std::unordered_map<std::string, std::vector<LeakInfo>> leaksByType;
    for (const auto& leak : leaks) {
        leaksByType[leak.type].push_back(leak);
    }
    
    for (const auto& pair : leaksByType) {
        out << "Type: " << pair.first << " (" << pair.second.size() << " leaks)" << std::endl;
        
        size_t totalSize = 0;
        for (const auto& leak : pair.second) {
            totalSize += leak.size;
        }
        
        out << "  Total leaked memory: " << totalSize << " bytes" << std::endl;
        
        if (pair.second.size() <= 10) {
            // Show details for small number of leaks
            for (const auto& leak : pair.second) {
                out << "    File: " << leak.file << ", Line: " << leak.line
                    << ", Size: " << leak.size << " bytes" << std::endl;
            }
        } else {
            out << "    (Too many leaks to list individually)" << std::endl;
        }
        
        out << std::endl;
    }
}

std::vector<LeakDetector::LeakInfo> LeakDetector::detectLeaksForType(const std::string& type) const {
    auto leaks = detectLeaks();
    
    std::vector<LeakInfo> filtered;
    for (const auto& leak : leaks) {
        if (leak.type == type) {
            filtered.push_back(leak);
        }
    }
    
    return filtered;
}

std::vector<LeakDetector::LeakInfo> LeakDetector::filterLeaksByFile(
    const std::vector<LeakInfo>& leaks,
    const std::string& filename
) const {
    std::vector<LeakInfo> filtered;
    for (const auto& leak : leaks) {
        // Simple filename matching (can be improved)
        if (leak.file.find(filename) != std::string::npos) {
            filtered.push_back(leak);
        }
    }
    
    return filtered;
}

std::vector<LeakDetector::LeakInfo> LeakDetector::filterLeaksByThread(
    const std::vector<LeakInfo>& leaks,
    std::thread::id threadId
) const {
    std::vector<LeakInfo> filtered;
    for (const auto& leak : leaks) {
        if (leak.threadId == threadId) {
            filtered.push_back(leak);
        }
    }
    
    return filtered;
}

// DebugUtils implementation
namespace DebugUtils {
    
    std::string getStackTrace() {
        std::vector<void*> callStack;
        
        // Get current stack trace
        int maxDepth = 50;
        void* buffer[maxDepth];
        int depth = backtrace(buffer, maxDepth);
        
        for (int i = 0; i < depth; ++i) {
            callStack.push_back(buffer[i]);
        }
        
        // Convert to symbols
        char** symbols = backtrace_symbols(callStack.data(), depth);
        
        std::stringstream ss;
        for (int i = 0; i < depth; ++i) {
            ss << i << ": " << symbols[i] << std::endl;
        }
        
        free(symbols);
        
        return ss.str();
    }
    
    template<typename T>
    std::string getObjectInfo(const T& obj) {
        std::stringstream ss;
        ss << "Object type: " << typeid(T).name() << std::endl;
        ss << "Address: " << &obj << std::endl;
        ss << "Size: " << sizeof(T) << " bytes" << std::endl;
        
        return ss.str();
    }
    
    std::string getMemoryInfo() {
        std::stringstream ss;
        
        // Get system memory info (simplified)
        ss << "Memory information (simplified):" << std::endl;
        ss << "  This implementation provides basic memory tracking." << std::endl;
        ss << "  For detailed system memory info, use platform-specific APIs." << std::endl;
        
        return ss.str();
    }
    
    std::string getThreadInfo() {
        std::stringstream ss;
        
        ss << "Current thread ID: " << std::this_thread::get_id() << std::endl;
        ss << "Thread name: (not available on all platforms)" << std::endl;
        
        return ss.str();
    }
    
    std::string getSystemInfo() {
        std::stringstream ss;
        
        ss << "System information:" << std::endl;
        ss << "  OS: (not detected)" << std::endl;
        ss << "  Architecture: (not detected)" << std::endl;
        ss << "  Compiler: (not detected)" << std::endl;
        
        return ss.str();
    }
    
    double benchmarkFunction(std::function<void()> func, size_t iterations) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        return elapsed / iterations;
    }
    
    template<typename T>
    std::vector<T*> findObjects(const std::vector<std::unique_ptr<T>>& container) {
        std::vector<T*> result;
        for (const auto& ptr : container) {
            if (ptr) {
                result.push_back(ptr.get());
            }
        }
        return result;
    }
    
    template<typename T>
    void safeDelete(T*& ptr) {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
    
    void assert(bool condition, const std::string& message) {
        if (!condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
    
    void logDebug(const std::string& message) {
        if (isDebugOutputEnabled()) {
            std::cout << "[DEBUG] " << message << std::endl;
        }
    }
    
    void setDebugOutputEnabled(bool enabled) {
        // Implementation depends on global debug state
        // This would set a global flag for debug output
    }
    
    bool isDebugOutputEnabled() {
        // Implementation depends on global debug state
        // This would return the global debug flag
        return true; // Default to enabled for now
    }
}

} // namespace nlm
