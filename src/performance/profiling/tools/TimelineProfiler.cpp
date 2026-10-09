#include "TimelineProfiler.hpp"

namespace nlm {
namespace profiling {

// TimelineProfiler implementation

void TimelineProfiler::startProfiling() {
    if (profiling_) return;
    profiling_ = true;
    pImpl_->startTime = std::chrono::steady_clock::now();
}

void TimelineProfiler::stopProfiling() {
    if (!profiling_) return;
    profiling_ = false;
    pImpl_->endTime = std::chrono::steady_clock::now();
    
    double totalTime = getTotalTime();
    totalProfilingTime_ = totalTime;
}

bool TimelineProfiler::isProfiling() const {
    return profiling_;
}

void TimelineProfiler::recordEvent(const std::string& label, const std::string& category,
                                   double startTime, double duration, uint64_t callCount) {
    TimelineEvent event;
    event.label = label;
    event.category = category;
    event.startTime = startTime;
    event.duration = duration;
    event.cumulativeTime = duration * callCount;
    event.callCount = callCount;
    event.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    events_.push_back(event);
}

void TimelineProfiler::recordStackTrace(const std::string& label, const std::string& category,
                                       double startTime, double duration) {
    // Simplified stack trace - could be enhanced to capture actual stack frames
    TimelineEvent event;
    event.label = label;
    event.category = category;
    event.startTime = startTime;
    event.duration = duration;
    event.cumulativeTime = duration;
    event.callCount = 1;
    event.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    event.metadata["stack_trace"] = "simplified_stack_trace";
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    events_.push_back(event);
}

std::vector<TimelineProfiler::TimelineEvent> TimelineProfiler::getEventsByCategory(const std::string& category) const {
    std::vector<TimelineEvent> result;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    for (const auto& event : events_) {
        if (event.category == category) {
            result.push_back(event);
        }
    }
    return result;
}

std::vector<TimelineProfiler::TimelineEvent> TimelineProfiler::getEventsByLabel(const std::string& label) const {
    std::vector<TimelineEvent> result;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    for (const auto& event : events_) {
        if (event.label == label) {
            result.push_back(event);
        }
    }
    return result;
}

double TimelineProfiler::getTotalTime() const {
    if (!profiling_) return totalProfilingTime_;
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl_->startTime).count();
}

double TimelineProfiler::getAverageEventDuration(const std::string& label) const {
    auto events = getEventsByLabel(label);
    if (events.empty()) return 0.0;
    
    double totalDuration = 0.0;
    for (const auto& event : events) {
        totalDuration += event.duration;
    }
    return totalDuration / events.size();
}

std::vector<std::string> TimelineProfiler::findLongestRunningEvents(size_t limit) const {
    std::vector<std::pair<TimelineEvent, double>> eventDurations;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& event : events_) {
        eventDurations.push_back({event, event.duration});
    }
    
    std::sort(eventDurations.begin(), eventDurations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    result.reserve(std::min(limit, eventDurations.size()));
    for (size_t i = 0; i < std::min(limit, eventDurations.size()); ++i) {
        result.push_back(eventDurations[i].first.label);
    }
    
    return result;
}

std::vector<std::string> TimelineProfiler::findMostFrequentEvents(size_t limit) const {
    std::map<std::string, uint64_t> eventCounts;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& event : events_) {
        eventCounts[event.label] += event.callCount;
    }
    
    std::vector<std::pair<std::string, uint64_t>> sortedEvents(eventCounts.begin(), eventCounts.end());
    std::sort(sortedEvents.begin(), sortedEvents.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    result.reserve(std::min(limit, sortedEvents.size()));
    for (size_t i = 0; i < std::min(limit, sortedEvents.size()); ++i) {
        result.push_back(sortedEvents[i].first);
    }
    
    return result;
}

bool TimelineProfiler::exportToJSON(const std::string& filepath) const {
    // TODO: Implement JSON export
    return false;
}

bool TimelineProfiler::exportToCSV(const std::string& filepath) const {
    // TODO: Implement CSV export
    return false;
}

bool TimelineProfiler::exportToTimelineHTML(const std::string& filepath) const {
    // TODO: Implement HTML export
    return false;
}

void TimelineProfiler::integrateWithProfiler(PerformanceProfiler& profiler) {
    // TODO: Integrate with PerformanceProfiler
}

} // namespace profiling
} // namespace nlm
