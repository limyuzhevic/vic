#pragma once

#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <set>

namespace nlm {

Profiler::Profiler()
    : max_samples_per_function_(1000)
    , sampling_interval_ms_(100)
    , real_time_profiling_(false) {
}

Profiler::~Profiler() {
    for (auto& pair : sessions_) {
        pair.second.is_active = false;
    }
}

std::string Profiler::generateSessionId() const {
    return "profile_" + std::to_string(next_session_id_++);
}

std::string Profiler::startSession(const std::string& session_name) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    std::string session_id = generateSessionId();
    
    ProfileSession session;
    session.session_id = session_id;
    session.session_name = session_name;
    session.start_time = std::chrono::steady_clock::now();
    session.is_active = true;
    
    sessions_[session_id] = session;
    
    Logger::getInstance().logInfo("Profiler session started: " + session_id + " (" + session_name + ")");
    
    return session_id;
}

void Profiler::stopSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        it->second.is_active = false;
        it->second.end_time = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(it->second.end_time - it->second.start_time);
        Logger::getInstance().logInfo("Profiler session stopped: " + session_id + " (duration: " + std::to_string(duration.count()) + "ms)");
    }
}

void Profiler::pauseSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        it->second.is_active = false;
        Logger::getInstance().logInfo("Profiler session paused: " + session_id);
    }
}

void Profiler::resumeSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        it->second.is_active = true;
        it->second.start_time = std::chrono::steady_clock::now(); // Reset start time
        Logger::getInstance().logInfo("Profiler session resumed: " + session_id);
    }
}

bool Profiler::isSessionActive(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto it = sessions_.find(session_id);
    return (it != sessions_.end() && it->second.is_active);
}

void Profiler::profileFunctionEnter(const std::string& function_name, const std::string& category) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    if (!real_time_profiling_) {
        return;
    }
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    current_state_.current_function = function_name;
    current_state_.current_category = category;
    current_state_.enter_time = std::chrono::steady_clock::now();
    current_state_.call_stack[function_name]++;
    
    // Add to timeline
    TimelineEvent event;
    event.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    event.event_name = "ENTER: " + function_name;
    event.event_type = "function_call";
    event.thread_id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    active_session->timeline_events.push_back(event);
}

void Profiler::profileFunctionExit(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    if (!real_time_profiling_ || current_state_.current_function.empty()) {
        return;
    }
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    // Calculate duration
    auto exit_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(exit_time - current_state_.enter_time);
    double duration_ms = duration.count() / 1000.0;
    
    // Update function profile
    FunctionProfile& profile = active_session->function_profiles[function_name];
    profile.name = function_name;
    profile.category = current_state_.current_category;
    profile.total_time_ms += duration_ms;
    profile.call_count++;
    profile.average_time_ms = profile.total_time_ms / profile.call_count;
    profile.min_time_ms = std::min(profile.min_time_ms, duration_ms);
    profile.max_time_ms = std::max(profile.max_time_ms, duration_ms);
    profile.sample_times.push_back(duration_ms);
    
    // Limit samples
    if (profile.sample_times.size() > max_samples_per_function_) {
        profile.sample_times.erase(profile.sample_times.begin());
    }
    
    // Update parent profile if exists
    if (!current_state_.call_stack.empty() && current_state_.call_stack.size() > 1) {
        std::string parent_function = "";
        for (const auto& pair : current_state_.call_stack) {
            if (pair.second > 1) {
                parent_function = pair.first;
                break;
            }
        }
        if (!parent_function.empty() && parent_function != function_name) {
            active_session->function_profiles[parent_function].children[function_name] = profile;
        }
    }
    
    // Add to timeline
    TimelineEvent event;
    event.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    event.event_name = "EXIT: " + function_name;
    event.event_type = "function_call";
    event.duration_ms = duration_ms;
    event.thread_id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    active_session->timeline_events.push_back(event);
    
    // Reset current state
    current_state_.current_function.clear();
    current_state_.current_category.clear();
    current_state_.call_stack.clear();
}

void Profiler::profileFunctionCall(const std::string& function_name, const std::string& category, std::function<void()> func) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    profileFunctionEnter(function_name, category);
    func();
    profileFunctionExit(function_name);
}

void Profiler::profileMemoryAlloc(const std::string& alloc_site, uint64_t size, const std::string& stack_trace) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    MemoryProfile profile;
    profile.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    profile.memory_mb = size / (1024.0 * 1024.0); // Convert to MB
    profile.allocation_count++;
    profile.allocation_types[alloc_site]++;
    profile.stack_trace = stack_trace;
    
    active_session->memory_profiles.push_back(profile);
}

void Profiler::profileMemoryDealloc(const std::string& alloc_site, uint64_t size) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    MemoryProfile profile;
    profile.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    profile.memory_mb = size / (1024.0 * 1024.0); // Convert to MB
    profile.deallocation_count++;
    profile.allocation_types[alloc_site]--;
    
    active_session->memory_profiles.push_back(profile);
}

void Profiler::profileMemoryUsage(double memory_mb, const std::string& context) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    MemoryProfile profile;
    profile.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    profile.memory_mb = memory_mb;
    
    if (!context.empty()) {
        profile.allocation_types[context]++;
    }
    
    active_session->memory_profiles.push_back(profile);
}

void Profiler::profileAlgorithm(const std::string& algorithm_name, uint64_t input_size,
                                std::function<void()> algorithm_func) {
    auto start_time = std::chrono::steady_clock::now();
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        algorithm_func();
        return;
    }
    
    algorithm_func();
    
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Estimate memory usage (simplified)
    double estimated_memory_mb = (input_size * sizeof(double)) / (1024.0 * 1024.0);
    
    AlgorithmProfile profile;
    profile.algorithm_name = algorithm_name;
    profile.input_size = input_size;
    profile.execution_time_ms = duration.count();
    profile.memory_usage_mb = estimated_memory_mb;
    profile.time_complexity_estimate = log2(input_size); // Simplified
    profile.space_complexity_estimate = log2(input_size); // Simplified
    profile.optimization_suggestions = "Consider input size optimization and parallel processing";
    
    active_session->algorithm_profiles.push_back(profile);
}

void Profiler::profileAlgorithm(const std::string& algorithm_name, uint64_t input_size,
                                double execution_time_ms, double memory_usage_mb) {
    AlgorithmProfile profile;
    profile.algorithm_name = algorithm_name;
    profile.input_size = input_size;
    profile.execution_time_ms = execution_time_ms;
    profile.memory_usage_mb = memory_usage_mb;
    profile.time_complexity_estimate = log2(input_size); // Simplified
    profile.space_complexity_estimate = log2(input_size); // Simplified
    profile.optimization_suggestions = "Consider input size optimization and parallel processing";
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (active_session) {
        active_session->algorithm_profiles.push_back(profile);
    }
}

void Profiler::profileTimelineEvent(const std::string& event_name, const std::string& event_type,
                                   double duration_ms, const std::string& thread_id) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    TimelineEvent event;
    event.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    event.event_name = event_name;
    event.event_type = event_type;
    event.duration_ms = duration_ms;
    event.thread_id = thread_id.empty() ? std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) : thread_id;
    
    active_session->timeline_events.push_back(event);
}

void Profiler::profileTimelineMarker(const std::string& marker_name,
                                     const std::map<std::string, std::string>& metadata) {
    profileTimelineEvent(marker_name, "marker", 0.0);
}

void Profiler::profileSpikeEvent(uint64_t timestamp_ms, uint32_t spike_count,
                                const std::vector<double>& inter_spike_intervals) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    SpikeEventProfile profile;
    profile.timestamp_ms = timestamp_ms;
    profile.spike_count = spike_count;
    profile.spike_frequency_hz = (spike_count > 0) ? (spike_count / 1000.0) : 0.0; // Assuming timestamp in ms
    profile.inter_spike_intervals = inter_spike_intervals;
    profile.pattern_type = classifySpikePattern(inter_spike_intervals);
    profile.burst_detection_score = 0.0; // Would calculate based on inter-spike intervals
    profile.analysis_notes = "Spike event analyzed for pattern detection";
    
    active_session->spike_event_profiles.push_back(profile);
}

void Profiler::profileSpikePattern(const std::string& pattern_type, double confidence_score) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    SpikeEventProfile profile;
    profile.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - active_session->start_time).count();
    profile.pattern_type = pattern_type;
    profile.burst_detection_score = confidence_score;
    profile.analysis_notes = "Pattern detected with confidence: " + std::to_string(confidence_score);
    
    active_session->spike_event_profiles.push_back(profile);
}

void Profiler::profileSystemCallEnter(const std::string& call_name, const std::string& signature) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    // Reset timer for this system call
    // In a real implementation, this would start a timer
}

void Profiler::profileSystemCallExit(const std::string& call_name, double duration_ms, uint64_t memory_delta) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    // Find active session
    ProfileSession* active_session = nullptr;
    for (auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_session = &pair.second;
            break;
        }
    }
    
    if (!active_session) {
        return;
    }
    
    SystemCallProfile profile;
    profile.call_name = call_name;
    profile.signature = "unknown"; // Would track this during enter
    profile.call_count = 1;
    profile.total_time_ms = duration_ms;
    profile.average_time_ms = duration_ms;
    profile.memory_allocated = memory_delta;
    
    active_session->system_call_profiles[call_name] = profile;
}

ProfileSession Profiler::getSession(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        return it->second;
    }
    
    return ProfileSession{};
}

std::vector<ProfileSession> Profiler::getAllSessions() const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    std::vector<ProfileSession> result;
    for (const auto& pair : sessions_) {
        result.push_back(pair.second);
    }
    
    return result;
}

FunctionProfile Profiler::getFunctionProfile(const std::string& session_id, const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return FunctionProfile{};
    }
    
    auto func_it = session_it->second.function_profiles.find(function_name);
    if (func_it == session_it->second.function_profiles.end()) {
        return FunctionProfile{};
    }
    
    return func_it->second;
}

std::vector<FunctionProfile> Profiler::getAllFunctionProfiles(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    std::vector<FunctionProfile> result;
    for (const auto& pair : session_it->second.function_profiles) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<MemoryProfile> Profiler::getMemoryProfiles(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    return session_it->second.memory_profiles;
}

std::vector<AlgorithmProfile> Profiler::getAlgorithmProfiles(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    return session_it->second.algorithm_profiles;
}

std::vector<TimelineEvent> Profiler::getTimelineEvents(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    return session_it->second.timeline_events;
}

HotPathData Profiler::getHotPathData(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return HotPathData{};
    }
    
    HotPathData hot_path_data;
    hot_path_data.total_time_ms = 0.0;
    hot_path_data.total_calls = 0;
    
    for (const auto& pair : session_it->second.function_profiles) {
        const auto& profile = pair.second;
        hot_path_data.hot_functions.push_back({profile.name, profile.total_time_ms});
        hot_path_data.hot_calls.push_back({profile.name, profile.call_count});
        hot_path_data.total_time_ms += profile.total_time_ms;
        hot_path_data.total_calls += profile.call_count;
    }
    
    if (hot_path_data.total_time_ms > 0) {
        hot_path_data.bottleneck_percentage = (hot_path_data.hot_functions.front().second / hot_path_data.total_time_ms) * 100.0;
    }
    
    // Sort by time and call count (descending)
    std::sort(hot_path_data.hot_functions.begin(), hot_path_data.hot_functions.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    std::sort(hot_path_data.hot_calls.begin(), hot_path_data.hot_calls.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return hot_path_data;
}

std::vector<SpikeEventProfile> Profiler::getSpikeEventProfiles(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    return session_it->second.spike_event_profiles;
}

std::map<std::string, SystemCallProfile> Profiler::getSystemCallProfiles(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    return session_it->second.system_call_profiles;
}

Profiler::HotPathData Profiler::analyzeHotPaths(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return HotPathData{};
    }
    
    HotPathData hot_path_data;
    hot_path_data.total_time_ms = 0.0;
    hot_path_data.total_calls = 0;
    
    for (const auto& pair : session_it->second.function_profiles) {
        const auto& profile = pair.second;
        hot_path_data.hot_functions.push_back({profile.name, profile.total_time_ms});
        hot_path_data.hot_calls.push_back({profile.name, profile.call_count});
        hot_path_data.total_time_ms += profile.total_time_ms;
        hot_path_data.total_calls += profile.call_count;
    }
    
    // Analyze bottlenecks
    analyzeFunctionHotPath(hot_path_data.hot_functions.front(), hot_path_data, hot_path_data.total_time_ms, 5.0);
    
    // Sort by time and call count (descending)
    std::sort(hot_path_data.hot_functions.begin(), hot_path_data.hot_functions.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    std::sort(hot_path_data.hot_calls.begin(), hot_path_data.hot_calls.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return hot_path_data;
}

std::vector<std::string> Profiler::identifyBottlenecks(const std::string& session_id, double threshold_percent) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    std::vector<std::string> bottlenecks;
    double total_time = 0.0;
    
    // Calculate total time
    for (const auto& pair : session_it->second.function_profiles) {
        total_time += pair.second.total_time_ms;
    }
    
    // Identify bottlenecks
    for (const auto& pair : session_it->second.function_profiles) {
        const auto& profile = pair.second;
        double percentage = (profile.total_time_ms / total_time) * 100.0;
        
        if (percentage > threshold_percent) {
            bottlenecks.push_back(profile.name + " (" + std::to_string(percentage) + "%)");
        }
    }
    
    return bottlenecks;
}

Profiler::SpikeEventProfile Profiler::analyzeSpikePattern(const std::string& session_id, uint64_t step) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return SpikeEventProfile{};
    }
    
    // Find spike event at the specified step (simplified)
    for (const auto& profile : session_it->second.spike_event_profiles) {
        if (profile.timestamp_ms == step) {
            return profile;
        }
    }
    
    return SpikeEventProfile{};
}

std::vector<Profiler::SpikeEventProfile> Profiler::analyzeSpikePatternAcrossSteps(const std::string& session_id, uint64_t steps) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    std::vector<SpikeEventProfile> results;
    
    for (const auto& profile : session_it->second.spike_event_profiles) {
        if (profile.timestamp_ms <= steps) {
            results.push_back(profile);
        }
    }
    
    return results;
}

std::string Profiler::classifySpikePattern(const std::vector<double>& inter_spike_intervals) const {
    if (inter_spike_intervals.size() < 3) {
        return "unknown";
    }
    
    double mean_isi = std::accumulate(inter_spike_intervals.begin(), inter_spike_intervals.end(), 0.0) / inter_spike_intervals.size();
    double std_isi = std::sqrt(std::inner_product(inter_spike_intervals.begin(), inter_spike_intervals.end(), inter_spike_intervals.begin(), 0.0) / inter_spike_intervals.size() - mean_isi * mean_isi);
    double cv = (mean_isi > 0) ? (std_isi / mean_isi) : 0.0;
    
    // Classify based on coefficient of variation
    if (cv < 0.1) {
        return "regular";
    } else if (cv < 0.5) {
        return "irregular";
    } else {
        return "burst";
    }
}

std::map<std::string, SystemCallProfile> Profiler::analyzeSystemCalls(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    return session_it->second.system_call_profiles;
}

std::string Profiler::getMostExpensiveSystemCall(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return "";
    }
    
    double max_time = 0.0;
    std::string most_expensive;
    
    for (const auto& pair : session_it->second.system_call_profiles) {
        const auto& profile = pair.second;
        if (profile.total_time_ms > max_time) {
            max_time = profile.total_time_ms;
            most_expensive = profile.call_name;
        }
    }
    
    return most_expensive;
}

std::string Profiler::getMostFrequentSystemCall(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return "";
    }
    
    uint64_t max_calls = 0;
    std::string most_frequent;
    
    for (const auto& pair : session_it->second.system_call_profiles) {
        const auto& profile = pair.second;
        if (profile.call_count > max_calls) {
            max_calls = profile.call_count;
            most_frequent = profile.call_name;
        }
    }
    
    return most_frequent;
}

double Profiler::calculateOverallPerformanceScore(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return 0.0;
    }
    
    double score = 0.0;
    
    // Calculate score based on multiple metrics
    for (const auto& pair : session_it->second.function_profiles) {
        const auto& profile = pair.second;
        score += (1000.0 / (profile.average_time_ms + 1.0));  // Higher score for faster execution
    }
    
    // Add memory efficiency score
    double total_memory = 0.0;
    for (const auto& memory_profile : session_it->second.memory_profiles) {
        total_memory += memory_profile.memory_mb;
    }
    
    if (!session_it->second.memory_profiles.empty()) {
        score += (10000.0 / (total_memory / session_it->second.memory_profiles.size() + 1.0));
    }
    
    return score;
}

std::map<std::string, double> Profiler::getPerformanceMetrics(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return {};
    }
    
    std::map<std::string, double> metrics;
    
    // Calculate various performance metrics
    double total_time = 0.0;
    uint64_t total_calls = 0;
    
    for (const auto& pair : session_it->second.function_profiles) {
        const auto& profile = pair.second;
        total_time += profile.total_time_ms;
        total_calls += profile.call_count;
    }
    
    metrics["total_execution_time_ms"] = total_time;
    metrics["total_function_calls"] = static_cast<double>(total_calls);
    metrics["average_function_time_ms"] = (total_calls > 0) ? (total_time / total_calls) : 0.0;
    
    // Memory metrics
    if (!session_it->second.memory_profiles.empty()) {
        double total_memory = 0.0;
        for (const auto& profile : session_it->second.memory_profiles) {
            total_memory += profile.memory_mb;
        }
        
        metrics["average_memory_mb"] = total_memory / session_it->second.memory_profiles.size();
        metrics["peak_memory_mb"] = std::max_element(session_it->second.memory_profiles.begin(), session_it->second.memory_profiles.end(),
                     [](const MemoryProfile& a, const MemoryProfile& b) { return a.memory_mb < b.memory_mb; })->memory_mb;
    }
    
    return metrics;
}

std::string Profiler::getPerformanceReport(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return "Session not found";
    }
    
    std::ostringstream oss;
    oss << "Performance Report for Session: " << session_id << std::endl;
    oss << "==========================================" << std::endl;
    oss << "Total Functions Profiled: " << session_it->second.function_profiles.size() << std::endl;
    oss << "Total Memory Samples: " << session_it->second.memory_profiles.size() << std::endl;
    oss << "Total Algorithm Profiles: " << session_it->second.algorithm_profiles.size() << std::endl;
    oss << "Total Timeline Events: " << session_it->second.timeline_events.size() << std::endl;
    
    double total_time = 0.0;
    for (const auto& pair : session_it->second.function_profiles) {
        total_time += pair.second.total_time_ms;
    }
    
    oss << "Total Execution Time: " << std::fixed << std::setprecision(2) << total_time << " ms" << std::endl;
    
    // Generate hot path summary
    HotPathData hot_path_data = analyzeHotPaths(session_id);
    oss << "\nTop 5 Hot Functions:" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(5, hot_path_data.hot_functions.size()); ++i) {
        oss << "  " << i + 1 << ". " << hot_path_data.hot_functions[i].first 
            << " - " << std::fixed << std::setprecision(2) << hot_path_data.hot_functions[i].second << " ms" << std::endl;
    }
    
    return oss.str();
}

Profiler::MemoryLeakReport Profiler::detectMemoryLeaks(const std::string& session_id) const {
    MemoryLeakReport report;
    
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end() || session_it->second.memory_profiles.size() < 2) {
        return report;
    }
    
    // Simple memory leak detection
    double first_memory = session_it->second.memory_profiles.front().memory_mb;
    double last_memory = session_it->second.memory_profiles.back().memory_mb;
    size_t num_samples = session_it->second.memory_profiles.size();
    
    report.leak_rate_mb_per_step = (last_memory - first_memory) / (num_samples - 1);
    
    if (report.leak_rate_mb_per_step > 0.01) { // 1MB per step
        report.leak_detected = true;
        report.leak_size_mb = (last_memory - first_memory);
        report.leak_source_function = "Multiple allocations in memory profile";
        report.recommendation = "Review memory allocation and deallocation patterns";
    }
    
    return report;
}

bool Profiler::exportProfile(const std::string& session_id, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Export as JSON (simplified)
    file << "{\"session_id\": \"" << session_id << "\"," << std::endl;
    file << "  \"function_profiles\": [";
    
    for (size_t i = 0; i < session_it->second.function_profiles.size(); ++i) {
        file << functionProfileToJSON(session_it->second.function_profiles.at(i));
        if (i < session_it->second.function_profiles.size() - 1) {
            file << ",";
        }
    }
    
    file << "]" << std::endl;
    file << "}";
    
    file.close();
    Logger::getInstance().logInfo("Profile exported to: " + filepath);
    return true;
}

bool Profiler::exportHotPathReport(const std::string& session_id, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return false;
    }
    
    HotPathData hot_path_data = analyzeHotPaths(session_id);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Hot Path Analysis Report for Session: " << session_id << std::endl;
    file << "==========================================" << std::endl;
    file << "Total Execution Time: " << std::fixed << std::setprecision(2) << hot_path_data.total_time_ms << " ms" << std::endl;
    file << "Total Function Calls: " << hot_path_data.total_calls << std::endl;
    file << "Bottleneck Percentage: " << std::fixed << std::setprecision(2) << hot_path_data.bottleneck_percentage << "%" << std::endl;
    
    file << "\nTop 10 Hot Functions (by cumulative time):" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(10, hot_path_data.hot_functions.size()); ++i) {
        file << "  " << i + 1 << ". " << hot_path_data.hot_functions[i].first 
             << " - " << std::fixed << std::setprecision(2) << hot_path_data.hot_functions[i].second 
             << " ms (" << hot_path_data.hot_calls[i].second << " calls)" << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Hot path report exported to: " + filepath);
    return true;
}

bool Profiler::exportTimelineReport(const std::string& session_id, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Timeline Report for Session: " << session_id << std::endl;
    file << "=======================================" << std::endl;
    file << "Total Events: " << session_it->second.timeline_events.size() << std::endl;
    
    for (const auto& event : session_it->second.timeline_events) {
        file << event.timestamp_ms << "ms - " << event.event_name << " (" << event.event_type << ")" << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Timeline report exported to: " + filepath);
    return true;
}

bool Profiler::exportMemoryReport(const std::string& session_id, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    MemoryLeakReport leak_report = detectMemoryLeaks(session_id);
    
    file << "Memory Report for Session: " << session_id << std::endl;
    file << "===================================== " << std::endl;
    file << "Total Memory Samples: " << session_it->second.memory_profiles.size() << std::endl;
    
    double total_memory = 0.0;
    for (const auto& profile : session_it->second.memory_profiles) {
        total_memory += profile.memory_mb;
    }
    
    if (!session_it->second.memory_profiles.empty()) {
        file << "Average Memory Usage: " << std::fixed << std::setprecision(2) 
             << (total_memory / session_it->second.memory_profiles.size()) << " MB" << std::endl;
    }
    
    file << "Memory Leak Detected: " << (leak_report.leak_detected ? "Yes" : "No") << std::endl;
    if (leak_report.leak_detected) {
        file << "Leak Rate: " << std::fixed << std::setprecision(4) << leak_report.leak_rate_mb_per_step << " MB per step" << std::endl;
        file << "Recommendation: " << leak_report.recommendation << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Memory report exported to: " + filepath);
    return true;
}

bool Profiler::exportSpikeEventReport(const std::string& session_id, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Spike Event Report for Session: " << session_id << std::endl;
    file << "=========================================" << std::endl;
    file << "Total Spike Events: " << session_it->second.spike_event_profiles.size() << std::endl;
    
    double total_spikes = 0.0;
    for (const auto& profile : session_it->second.spike_event_profiles) {
        total_spikes += profile.spike_count;
    }
    
    if (!session_it->second.spike_event_profiles.empty()) {
        file << "Average Spike Count: " << std::fixed << std::setprecision(2) 
             << (total_spikes / session_it->second.spike_event_profiles.size()) << std::endl;
    }
    
    // Pattern distribution
    std::map<std::string, uint64_t> pattern_counts;
    for (const auto& profile : session_it->second.spike_event_profiles) {
        pattern_counts[profile.pattern_type]++;
    }
    
    file << "\nPattern Distribution:" << std::endl;
    for (const auto& pair : pattern_counts) {
        file << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Spike event report exported to: " + filepath);
    return true;
}

bool Profiler::exportSystemCallReport(const std::string& session_id, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "System Call Report for Session: " << session_id << std::endl;
    file << "==========================================" << std::endl;
    file << "Total System Calls: " << session_it->second.system_call_profiles.size() << std::endl;
    
    double total_time = 0.0;
    for (const auto& pair : session_it->second.system_call_profiles) {
        total_time += pair.second.total_time_ms;
    }
    
    if (!session_it->second.system_call_profiles.empty()) {
        file << "Total Time in System Calls: " << std::fixed << std::setprecision(2) << total_time << " ms" << std::endl;
    }
    
    // Most expensive system call
    std::string most_expensive = getMostExpensiveSystemCall(session_id);
    if (!most_expensive.empty()) {
        file << "\nMost Expensive System Call: " << most_expensive << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("System call report exported to: " + filepath);
    return true;
}

void Profiler::cleanupSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    sessions_.erase(session_id);
    Logger::getInstance().logInfo("Profiler session cleaned up: " + session_id);
}

void Profiler::cleanupOldSessions(uint64_t max_age_ms) {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto current_time = std::chrono::steady_clock::now();
    auto it = sessions_.begin();
    
    while (it != sessions_.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - it->second.start_time);
        
        if (duration.count() > max_age_ms) {
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t Profiler::getActiveSessionCount() const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    size_t count = 0;
    for (const auto& pair : sessions_) {
        if (pair.second.is_active) {
            count++;
        }
    }
    
    return count;
}

std::vector<std::string> Profiler::getActiveSessionIds() const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    std::vector<std::string> active_ids;
    
    for (const auto& pair : sessions_) {
        if (pair.second.is_active) {
            active_ids.push_back(pair.first);
        }
    }
    
    return active_ids;
}

uint64_t Profiler::getTotalFunctionCalls(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return 0;
    }
    
    uint64_t total_calls = 0;
    for (const auto& pair : session_it->second.function_profiles) {
        total_calls += pair.second.call_count;
    }
    
    return total_calls;
}

double Profiler::getTotalExecutionTime(const std::string& session_id) const {
    std::lock_guard<std::mutex> lock(profiling_mutex_);
    
    auto session_it = sessions_.find(session_id);
    if (session_it == sessions_.end()) {
        return 0.0;
    }
    
    double total_time = 0.0;
    for (const auto& pair : session_it->second.function_profiles) {
        total_time += pair.second.total_time_ms;
    }
    
    return total_time;
}

std::string Profiler::functionProfileToJSON(const FunctionProfile& profile) {
    std::ostringstream oss;
    oss << "{\"name\": \"" << profile.name << "\",\"category\": \"" << profile.category << "\",\"total_time_ms\": " << profile.total_time_ms << ",\"average_time_ms\": " << profile.average_time_ms << ",\"call_count\": " << profile.call_count << "}";
    return oss.str();
}

} // namespace nlm
