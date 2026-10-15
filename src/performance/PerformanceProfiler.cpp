#include "PerformanceProfiler.hpp"
#include <chrono>
#include <thread>
#include <algorithm>

namespace nlm {

PerformanceProfiler::PerformanceProfiler(PerformanceMonitor* monitor) : monitor(monitor) {
}

PerformanceProfiler::~PerformanceProfiler() {
    std::lock_guard<std::mutex> lock(scopes_mutex);
    for (const auto& scope : active_scopes) {
        std::chrono::nanoseconds end_time(std::chrono::steady_clock::now().time_since_epoch().count() - scope.start_time_ns);
        ProfileEvent event;
        event.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
        event.function_name = scope.function_name;
        event.duration_ns = end_time.count();
        event.thread_id = scope.thread_id;
        event.file_name = scope.file_name;
        event.line_number = scope.line_number;
        
        if (monitor) {
            monitor->recordProfileEvent(event);
        }
    }
    active_scopes.clear();
}

void PerformanceProfiler::start(const std::string& function_name, const std::string& file_name, uint32_t line_number) {
    std::lock_guard<std::mutex> lock(scopes_mutex);
    
    uint32_t thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    ProfileScope scope;
    scope.start_time_ns = std::chrono::steady_clock::now().time_since_epoch().count();
    scope.function_name = function_name;
    scope.file_name = file_name;
    scope.line_number = line_number;
    scope.thread_id = thread_id;
    
    active_scopes.push_back(scope);
}

void PerformanceProfiler::stop(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(scopes_mutex);
    
    auto it = std::find_if(active_scopes.begin(), active_scopes.end(),
        [&function_name](const ProfileScope& scope) {
            return scope.function_name == function_name;
        });
    
    if (it != active_scopes.end()) {
        uint64_t end_time_ns = std::chrono::steady_clock::now().time_since_epoch().count();
        uint64_t duration_ns = end_time_ns - it->start_time_ns;
        
        ProfileEvent event;
        event.timestamp_ns = end_time_ns;
        event.function_name = it->function_name;
        event.duration_ns = duration_ns;
        event.thread_id = it->thread_id;
        event.file_name = it->file_name;
        event.line_number = it->line_number;
        
        if (monitor) {
            monitor->recordProfileEvent(event);
        }
        
        active_scopes.erase(it);
    }
}

PerformanceProfiler::Scope::Scope(PerformanceProfiler* profiler, const std::string& function_name, 
                                 const std::string& file_name, uint32_t line_number)
    : profiler(profiler), function_name(function_name), file_name(file_name), line_number(line_number) {
    if (profiler) {
        profiler->start(function_name, file_name, line_number);
    }
}

PerformanceProfiler::Scope::~Scope() {
    if (profiler) {
        profiler->stop(function_name);
    }
}

PerformanceProfiler::Scope PerformanceProfiler::makeScope(const std::string& function_name, 
                                                         const std::string& file_name, 
                                                         uint32_t line_number) {
    return Scope(this, function_name, file_name, line_number);
}

} // namespace nlm