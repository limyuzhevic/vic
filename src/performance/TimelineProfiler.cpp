// TimelineProfiler.cpp implementation
#include "TimelineProfiler.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>

using json = nlohmann::json;

namespace nlm {

TimelineProfiler::TimelineProfiler() : running_(false), startTime_(0), currentThreadId_(std::hash<std::thread::id>{}(std::this_thread::get_id())) {}
TimelineProfiler::~TimelineProfiler() = default;

void TimelineProfiler::recordEvent(const std::string& eventType, const std::string& name,
                                 const std::string& file, int line, double duration,
                                 uint64_t threadId) {
    double timestamp = running_ ? (std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::time_point(std::chrono::duration<double>(startTime_))
    ).count()) : 0.0;
    
    TimelineEvent event(timestamp, "main", eventType, name, file, line, duration, threadId);
    
    std::lock_guard<std::mutex> lock(mutex_);
    events_.push_back(event);
}

void TimelineProfiler::enterFunction(const std::string& functionName, const std::string& file,
                                    int line) {
    double timestamp = running_ ? (std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::time_point(std::chrono::duration<double>(startTime_))
    ).count()) : 0.0;
    
    TimelineEvent event(timestamp, "main", "function_enter", functionName, file, line, 0.0,
                       std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    std::lock_guard<std::mutex> lock(mutex_);
    callStack_.push_back(event);
}

void TimelineProfiler::exitFunction(const std::string& functionName, double duration) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!callStack_.empty()) {
        TimelineEvent& event = callStack_.back();
        event.duration = duration;
        event.eventType = "function_exit";
        
        events_.push_back(event);
        callStack_.pop_back();
    }
}

void TimelineProfiler::recordMemoryAllocation(size_t size, const std::string& context) {
    recordEvent("memory", "allocation", "", 0, 0.0,
               std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

void TimelineProfiler::recordMemoryDeallocation(size_t size, const std::string& context) {
    recordEvent("memory", "deallocation", "", 0, 0.0,
               std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

void TimelineProfiler::recordIOEvent(const std::string& operation, size_t bytes,
                                    double duration, const std::string& target) {
    recordEvent("io", operation, "", 0, duration,
               std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

void TimelineProfiler::start() {
    if (!running_) {
        running_ = true;
        startTime_ = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
}

void TimelineProfiler::stop() {
    running_ = false;
}

std::string TimelineProfiler::generateTimelineReport() const {
    std::stringstream ss;
    ss << "Timeline Report\n";
    ss << "===============\n\n";
    ss << "Total events: " << events_.size() << "\n";
    ss << "Timeline duration: " << (events_.empty() ? 0 : events_.back().timestamp) << "s\n\n";
    
    // Group events by type
    std::unordered_map<std::string, size_t> eventCounts;
    for (const auto& event : events_) {
        eventCounts[event.eventType]++;
    }
    
    ss << "Event breakdown:\n";
    for (const auto& pair : eventCounts) {
        ss << "  " << pair.first << ": " << pair.second << "\n";
    }
    ss << "\n";
    
    // Sort events by timestamp
    std::vector<TimelineEvent> sortedEvents = events_;
    std::sort(sortedEvents.begin(), sortedEvents.end(),
              [](const TimelineEvent& a, const TimelineEvent& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Show recent events
    ss << "Recent events:\n";
    size_t recentCount = std::min(size_t(10), sortedEvents.size());
    for (size_t i = 0; i < recentCount; ++i) {
        size_t idx = sortedEvents.size() - 1 - i;
        const auto& event = sortedEvents[idx];
        ss << "  " << event.timestamp << "s - " << event.threadName << " - " 
           << event.eventType << " - " << event.name;
        if (!event.file.empty()) ss << " (" << event.file << ":" << event.line << ")";
        if (event.duration > 0) ss << " - Duration: " << event.duration << "s";
        ss << "\n";
    }
    
    return ss.str();
}

std::string TimelineProfiler::generateFlameGraph() const {
    std::stringstream ss;
    ss << "Flame Graph Format\n";
    ss << "=================\n\n";
    
    // Sort events by timestamp
    std::vector<TimelineEvent> sortedEvents = events_;
    std::sort(sortedEvents.begin(), sortedEvents.end(),
              [](const TimelineEvent& a, const TimelineEvent& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Generate flame graph data
    for (const auto& event : sortedEvents) {
        if (event.eventType == "function_enter" || event.eventType == "function_exit") {
            ss << "entry " << event.name << " " << event.timestamp << " " 
               << (event.duration > 0 ? event.duration : 0.0) << " "
               << event.threadId << "\n";
        }
    }
    
    return ss.str();
}

bool TimelineProfiler::saveTimeline(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    // Convert to JSON
    json j;
    j["events"] = json::array();
    
    for (const auto& event : events_) {
        json jEvent;
        jEvent["timestamp"] = event.timestamp;
        jEvent["threadName"] = event.threadName;
        jEvent["eventType"] = event.eventType;
        jEvent["name"] = event.name;
        jEvent["file"] = event.file;
        jEvent["line"] = event.line;
        jEvent["duration"] = event.duration;
        jEvent["threadId"] = event.threadId;
        
        // Add metadata
        if (!event.metadata.empty()) {
            for (const auto& pair : event.metadata) {
                jEvent["metadata"][pair.first] = pair.second;
            }
        }
        
        j["events"].push_back(jEvent);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool TimelineProfiler::loadTimeline(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearEvents();
    
    for (const auto& jEvent : j["events"]) {
        TimelineEvent event(
            jEvent["timestamp"].get<double>(),
            jEvent["threadName"].get<std::string>(),
            jEvent["eventType"].get<std::string>(),
            jEvent["name"].get<std::string>(),
            jEvent.value("file", ""),
            jEvent.value("line", 0),
            jEvent.value("duration", 0.0),
            jEvent.value("threadId", 0ULL)
        );
        
        // Load metadata
        if (jEvent.contains("metadata")) {
            for (const auto& pair : jEvent["metadata"].items()) {
                event.metadata[pair.key()] = pair.value().get<std::string>();
            }
        }
        
        events_.push_back(event);
    }
    
    file.close();
    return true;
}

void TimelineProfiler::clearEvents() {
    std::lock_guard<std::mutex> lock(mutex_);
    events_.clear();
    callStack_.clear();
}

} // namespace nlm
