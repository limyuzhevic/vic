// Enhanced NLM Profiler Implementation
// File: src/core/Profiling/Profiler.cpp

#include "Profiler.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <regex>

namespace nlm {

Profiler::Profiler(const ProfilerConfig& config)
    : config_(config)
    , running_(false)
    , start_time_(std::chrono::steady_clock::now())
    , end_time_(start_time_)
    , thread_safe_(true)
    , current_sample_index_(0)
    , last_cleanup_(start_time_)
    , total_samples_taken_(0)
    , last_update_(start_time_) {

    // Initialize sampling filters based on patterns
    for (const auto& pattern : config.include_patterns) {
        try {
            std::regex regex(pattern);
            include_regexes_.push_back(regex);
        } catch (const std::regex_error& e) {
            // Ignore invalid regex patterns
            std::cerr << "Invalid include pattern: " << pattern << std::endl;
        }
    }

    for (const auto& pattern : config.exclude_patterns) {
        try {
            std::regex regex(pattern);
            exclude_regexes_.push_back(regex);
        } catch (const std::regex_error& e) {
            std::cerr << "Invalid exclude pattern: " << pattern << std::endl;
        }
    }

    // Set up real-time monitoring thread if enabled
    if (config.enable_real_time_analysis) {
        enable_real_time_monitoring(true);
    }
}

Profiler::~Profiler() {
    stop();
}

void Profiler::configure(const ProfilerConfig& config) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    
    // Stop if currently running
    if (running_.load()) {
        stop();
    }

    config_ = config;
    
    // Update sampling filters
    include_regexes_.clear();
    exclude_regexes_.clear();
    
    for (const auto& pattern : config.include_patterns) {
        try {
            std::regex regex(pattern);
            include_regexes_.push_back(regex);
        } catch (const std::regex_error& e) {
            std::cerr << "Invalid include pattern: " << pattern << std::endl;
        }
    }

    for (const auto& pattern : config.exclude_patterns) {
        try {
            std::regex regex(pattern);
            exclude_regexes_.push_back(regex);
        } catch (const std::regex_error& e) {
            std::cerr << "Invalid exclude pattern: " << pattern << std::endl;
        }
    }

    // Clear old samples
    clear_samples();
    
    // Restart if was running
    if (running_.load()) {
        start();
    }
}

void Profiler::start() {
    if (running_.load()) {
        return;
    }

    running_.store(true);
    start_time_ = std::chrono::steady_clock::now();
    end_time_ = start_time_;
    current_sample_index_ = 0;
    total_samples_taken_ = 0;

    // Clear sampled functions and threads
    sampled_functions_.clear();
    sampled_threads_.clear();

    // Start monitoring thread
    monitoring_thread_ = std::thread(&Profiler::sampling_loop, this);
}

void Profiler::stop() {
    if (!running_.load()) {
        return;
    }

    running_.store(false);
    end_time_ = std::chrono::steady_clock::now();

    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }

    if (real_time_monitoring_) {
        enable_real_time_monitoring(false);
    }
}

void Profiler::take_sample() {
    if (!running_.load()) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    std::thread::id current_thread_id = std::this_thread::get_id();

    switch (config_.type) {
        case ProfilerType::CPU:
            sample_cpu(now, current_thread_id);
            break;
        case ProfilerType::MEMORY:
            sample_memory(now, current_thread_id);
            break;
        case ProfilerType::THREAD:
            sample_thread(now, current_thread_id);
            break;
        case ProfilerType::SYSTEM:
            sample_system(now, current_thread_id);
            break;
        case ProfilerType::FUNCTION:
            sample_function_from_stack(now, current_thread_id);
            break;
        case ProfilerType::ALLOCATION:
            sample_allocation_from_tracker(now, current_thread_id);
            break;
        case ProfilerType::GPU:
            sample_gpu(now, current_thread_id);
            break;
        case ProfilerType::NETWORK:
            sample_network(now, current_thread_id);
            break;
        case ProfilerType::DISK:
            sample_disk(now, current_thread_id);
            break;
        default:
            sample_all(now, current_thread_id);
            break;
    }

    total_samples_taken_++;
}

void Profiler::sample_function(const std::string& function_name,
                               const std::string& file_path,
                               size_t line_number,
                               std::chrono::duration<double> duration,
                               std::chrono::duration<double> cpu_duration,
                               const std::map<std::string, std::string>& parameters) {
    if (!running_.load()) {
        return;
    }

    std::lock_guard<std::mutex> lock(samples_mutex_);

    FunctionSample sample;
    sample.timestamp = std::chrono::steady_clock::now();
    sample.thread_id = std::this_thread::get_id();
    sample.function_name = function_name;
    sample.file_path = file_path;
    sample.line_number = line_number;
    sample.duration = duration;
    sample.cpu_duration = cpu_duration;
    sample.parameters = parameters;
    sample.call_count = 1;
    sample.recursive_depth = 0;

    // Get call stack
    sample.call_stack = get_call_stack();

    // Calculate CPU usage
    if (duration.count() > 0) {
        sample.cpu_usage = (cpu_duration.count() / duration.count()) * 100.0;
    }

    // Check if we should sample this function
    if (should_sample_function(function_name) && should_sample_thread(sample.thread_id)) {
        function_samples_.push_back(sample);

        // Keep only recent samples
        if (function_samples_.size() > config_.max_samples) {
            function_samples_.erase(function_samples_.begin());
        }
    }
}

void Profiler::sample_allocation(void* address, size_t size,
                                 const std::string& allocator_type,
                                 const std::string& allocation_site,
                                 const std::vector<std::string>& call_stack) {
    if (!running_.load()) {
        return;
    }

    std::lock_guard<std::mutex> lock(samples_mutex_);

    AllocationSample sample;
    sample.timestamp = std::chrono::steady_clock::now();
    sample.thread_id = std::this_thread::get_id();
    sample.allocated_address = address;
    sample.allocation_size = size;
    sample.allocator_type = allocator_type;
    sample.allocation_site = allocation_site;
    sample.call_stack = call_stack;
    sample.stack_depth = call_stack.size();
    sample.is_allocated = true;
    sample.object_type = get_allocation_type(size, allocator_type);
    sample.object_count = 1;

    allocation_samples_.push_back(sample);

    // Keep only recent samples
    if (allocation_samples_.size() > config_.max_samples) {
        allocation_samples_.erase(allocation_samples_.begin());
    }
}

void Profiler::sample_deallocation(void* address, size_t size) {
    if (!running_.load()) {
        return;
    }

    std::lock_guard<std::mutex> lock(samples_mutex_);

    // Find the corresponding allocation sample
    for (auto& sample : allocation_samples_) {
        if (sample.allocated_address == address && sample.is_allocated) {
            sample.is_allocated = false;
            sample.deallocation_time = std::chrono::steady_clock::now();
            
            if (sample.deallocation_time >= sample.allocation_time) {
                sample.time_to_live = std::chrono::duration<double>(
                    sample.deallocation_time - sample.allocation_time
                ).count();
            }
            
            break;
        }
    }
}

void Profiler::sample_thread(std::thread::id thread_id,
                             const std::string& thread_name,
                             std::chrono::duration<double> cpu_time,
                             std::chrono::duration<double> wall_time,
                             double utilization) {
    std::lock_guard<std::mutex> lock(samples_mutex_);

    ThreadSample sample;
    sample.timestamp = std::chrono::steady_clock::now();
    sample.thread_id = thread_id;
    sample.thread_name = thread_name;
    sample.cpu_time = cpu_time;
    sample.wall_time = wall_time;
    sample.utilization = utilization;

    thread_samples_.push_back(sample);

    // Keep only recent samples
    if (thread_samples_.size() > config_.max_samples) {
        thread_samples_.erase(thread_samples_.begin());
    }
}

void Profiler::sample_system() {
    std::lock_guard<std::mutex> lock(samples_mutex_);

    SystemSample sample;
    sample.timestamp = std::chrono::steady_clock::now();
    sample.thread_id = std::this_thread::get_id();

    // Get system metrics
    sample.system_load = get_system_load();
    sample.cpu_temperature = get_cpu_temperature();
    sample.disk_usage = get_disk_usage();
    sample.network_usage = get_network_usage();
    sample.network_bytes_sent = get_network_bytes_sent();
    sample.network_bytes_received = get_network_bytes_received();
    sample.battery_level = get_battery_level();
    sample.power_state = get_power_state();

    system_samples_.push_back(sample);

    // Keep only recent samples
    if (system_samples_.size() > config_.max_samples) {
        system_samples_.erase(system_samples_.begin());
    }
}

ProfiledFunction Profiler::get_function_analysis(const std::string& function_name) const {
    ProfiledFunction profiled_func;
    profiled_func.name = function_name;

    std::lock_guard<std::mutex> lock(samples_mutex_);

    // Filter function samples
    std::vector<FunctionSample> func_samples;
    for (const auto& sample : function_samples_) {
        if (sample.function_name == function_name) {
            func_samples.push_back(sample);
        }
    }

    if (func_samples.empty()) {
        return profiled_func;
    }

    // Calculate statistics
    profiled_func.call_count = func_samples.size();
    profiled_func.total_time = std::accumulate(
        func_samples.begin(), func_samples.end(), std::chrono::duration<double>(0),
        [](const std::chrono::duration<double>& sum, const FunctionSample& sample) {
            return sum + sample.duration;
        }
    );

    profiled_func.cpu_time = std::accumulate(
        func_samples.begin(), func_samples.end(), std::chrono::duration<double>(0),
        [](const std::chrono::duration<double>& sum, const FunctionSample& sample) {
            return sum + sample.cpu_duration;
        }
    );

    profiled_func.wall_time = profiled_func.total_time;

    if (profiled_func.call_count > 0) {
        profiled_func.avg_time = profiled_func.total_time / profiled_func.call_count;
        profiled_func.min_time = std::chrono::duration<double>(std::numeric_limits<double>::max());
        profiled_func.max_time = std::chrono::duration<double>(0);

        for (const auto& sample : func_samples) {
            profiled_func.min_time = std::min(profiled_func.min_time, sample.duration);
            profiled_func.max_time = std::max(profiled_func.max_time, sample.duration);
        }

        profiled_func.cpu_usage = 0.0;
        for (const auto& sample : func_samples) {
            profiled_func.cpu_usage += sample.cpu_usage;
        }
        profiled_func.cpu_usage /= func_samples.size();

        profiled_func.memory_usage = 0;
        for (const auto& sample : func_samples) {
            profiled_func.memory_usage += sample.memory_usage;
        }
        profiled_func.memory_usage /= func_samples.size();
    }

    // Collect time samples
    profiled_func.time_samples.reserve(func_samples.size());
    for (const auto& sample : func_samples) {
        profiled_func.time_samples.push_back(sample.duration);
    }

    // Collect calls by thread
    for (const auto& sample : func_samples) {
        profiled_func.call_counts_by_thread[sample.thread_id]++;
    }

    return profiled_func;
}

std::vector<ProfiledFunction> Profiler::get_function_analysis(
    const std::string& pattern,
    AnalysisMode mode) const {

    std::vector<ProfiledFunction> results;
    std::lock_guard<std::mutex> lock(samples_mutex_);

    std::regex regex_pattern(pattern);

    for (const auto& sample : function_samples_) {
        if (std::regex_search(sample.function_name, regex_pattern)) {
            ProfiledFunction profiled_func = get_function_analysis(sample.function_name);
            results.push_back(profiled_func);

            // Limit results for performance
            if (results.size() >= 1000) {
                break;
            }
        }
    }

    // Sort based on analysis mode
    switch (mode) {
        case AnalysisMode::SUMMARY:
            std::sort(results.begin(), results.end(),
                     [](const ProfiledFunction& a, const ProfiledFunction& b) {
                         return a.call_count > b.call_count;
                     });
            break;
        case AnalysisMode::DETAILED:
            std::sort(results.begin(), results.end(),
                     [](const ProfiledFunction& a, const ProfiledFunction& b) {
                         return a.total_time > b.total_time;
                     });
            break;
        case AnalysisMode::TIMELINE:
            // Sort by first occurrence
            // This would require tracking first occurrence time
            break;
        case AnalysisMode::COMPARATIVE:
            // Sort by multiple metrics
            std::sort(results.begin(), results.end(),
                     [](const ProfiledFunction& a, const ProfiledFunction& b) {
                         return a.call_count * a.avg_time.count() >
                                b.call_count * b.avg_time.count();
                     });
            break;
        case AnalysisMode::ROOT_CAUSE:
            std::sort(results.begin(), results.end(),
                     [](const ProfiledFunction& a, const ProfiledFunction& b) {
                         return a.call_count > b.call_count &&
                                a.total_time > b.total_time;
                     });
            break;
    }

    return results;
}

ProfileAnalysis Profiler::analyze_profile() const {
    ProfileAnalysis analysis;
    analysis.profile_name = "Profiler Analysis";
    analysis.total_duration = std::chrono::duration<double>(
        end_time_ - start_time_
    );
    analysis.total_samples = total_samples_taken_;

    if (analysis.total_samples == 0) {
        return analysis;
    }

    // Calculate average metrics
    double total_cpu_time = 0.0;
    double total_memory = 0.0;
    size_t total_allocations = 0;
    size_t total_deallocations = 0;

    std::lock_guard<std::mutex> lock(samples_mutex_);

    for (const auto& sample : cpu_samples_) {
        total_cpu_time += sample.cpu_usage;
        total_memory += sample.memory_usage;
    }

    analysis.cpu_usage = total_cpu_time / cpu_samples_.size();
    analysis.memory_usage = total_memory / cpu_samples_.size();

    // Analyze allocations
    for (const auto& sample : allocation_samples_) {
        if (sample.is_allocated) {
            total_allocations++;
        } else {
            total_deallocations++;
        }
    }

    analysis.total_allocations = total_allocations;
    analysis.total_deallocations = total_deallocations;

    // Detect memory leaks
    analysis.memory_leak_count = std::count_if(
        allocation_samples_.begin(), allocation_samples_.end(),
        [](const AllocationSample& sample) {
            return sample.is_allocated;
        }
    );

    // Get hot functions
    analysis.hot_functions = get_hot_functions(10);

    // Get cold functions
    analysis.cold_functions = get_cold_functions(10);

    // Get bottleneck functions
    analysis.bottleneck_functions = get_bottleneck_functions(10);

    // Calculate efficiency score
    if (analysis.total_duration.count() > 0) {
        analysis.efficiency_score = analysis.total_samples / analysis.total_duration.count();
        analysis.throughput = analysis.total_samples / analysis.total_duration.count();
    }

    // Calculate cache efficiency (simplified)
    analysis.cache_efficiency = 0.8;  // Placeholder

    // Generate recommendations
    analysis.recommendations = generate_recommendations(analysis);

    // Generate optimization opportunities
    analysis.optimization_opportunities = generate_optimization_opportunities(analysis);

    return analysis;
}

std::vector<std::string> Profiler::get_hot_functions(size_t count) const {
    std::vector<std::string> hot_functions;

    std::lock_guard<std::mutex> lock(samples_mutex_);

    // Count calls per function
    std::unordered_map<std::string, size_t> function_calls;
    std::unordered_map<std::string, double> function_time;

    for (const auto& sample : function_samples_) {
        function_calls[sample.function_name]++;
        function_time[sample.function_name] += sample.duration.count();
    }

    // Sort by time (descending)
    std::vector<std::pair<std::string, double>> sorted_functions;
    for (const auto& pair : function_time) {
        sorted_functions.emplace_back(pair.first, pair.second);
    }

    std::sort(sorted_functions.begin(), sorted_functions.end(),
             [](const std::pair<std::string, double>& a,
                const std::pair<std::string, double>& b) {
                 return a.second > b.second;
             });

    // Take top N functions
    hot_functions.reserve(count);
    for (size_t i = 0; i < std::min(count, sorted_functions.size()); ++i) {
        hot_functions.push_back(sorted_functions[i].first);
    }

    return hot_functions;
}

std::vector<std::string> Profiler::get_cold_functions(size_t count) const {
    std::vector<std::string> cold_functions;

    std::lock_guard<std::mutex> lock(samples_mutex_);

    // Count calls per function
    std::unordered_map<std::string, double> function_time;

    for (const auto& sample : function_samples_) {
        function_time[sample.function_name] += sample.duration.count();
    }

    // Sort by time (ascending)
    std::vector<std::pair<std::string, double>> sorted_functions;
    for (const auto& pair : function_time) {
        sorted_functions.emplace_back(pair.first, pair.second);
    }

    std::sort(sorted_functions.begin(), sorted_functions.end(),
             [](const std::pair<std::string, double>& a,
                const std::pair<std::string, double>& b) {
                 return a.second < b.second;
             });

    // Take bottom N functions
    cold_functions.reserve(count);
    for (size_t i = 0; i < std::min(count, sorted_functions.size()); ++i) {
        cold_functions.push_back(sorted_functions[i].first);
    }

    return cold_functions;
}

std::vector<std::string> Profiler::get_bottleneck_functions(size_t count) const {
    std::vector<std::string> bottleneck_functions;

    std::lock_guard<std::mutex> lock(samples_mutex_);

    // Analyze function samples for bottlenecks
    std::vector<std::pair<std::string, double>> bottleneck_scores;

    for (const auto& sample : function_samples_) {
        double score = sample.duration.count() * sample.call_count;

        // Check if function is called by many threads
        size_t unique_threads = 0;
        for (const auto& other_sample : function_samples_) {
            if (other_sample.function_name == sample.function_name) {
                unique_threads++;
            }
        }

        score *= std::pow(unique_threads, 0.5);  // Weight by thread diversity
        bottleneck_scores.emplace_back(sample.function_name, score);
    }

    // Sort by bottleneck score (descending)
    std::sort(bottleneck_scores.begin(), bottleneck_scores.end(),
             [](const std::pair<std::string, double>& a,
                const std::pair<std::string, double>& b) {
                 return a.second > b.second;
             });

    // Take top N functions
    bottleneck_functions.reserve(count);
    for (size_t i = 0; i < std::min(count, bottleneck_scores.size()); ++i) {
        bottleneck_functions.push_back(bottleneck_scores[i].first);
    }

    return bottleneck_functions;
}

std::vector<std::string> Profiler::get_recommendation() const {
    return generate_recommendations(analyze_profile());
}

std::vector<CPUSample> Profiler::query_cpu_samples(double time_window) const {
    std::vector<CPUSample> result;
    auto cutoff_time = std::chrono::steady_clock::now() - std::chrono::duration<double>(time_window);

    std::lock_guard<std::mutex> lock(samples_mutex_);

    for (const auto& sample : cpu_samples_) {
        if (sample.timestamp >= cutoff_time) {
            result.push_back(sample);
        }
    }

    return result;
}

std::vector<MemorySample> Profiler::query_memory_samples(double time_window) const {
    std::vector<MemorySample> result;
    auto cutoff_time = std::chrono::steady_clock::now() - std::chrono::duration<double>(time_window);

    std::lock_guard<std::mutex> lock(samples_mutex_);

    for (const auto& sample : memory_samples_) {
        if (sample.timestamp >= cutoff_time) {
            result.push_back(sample);
        }
    }

    return result;
}

std::vector<ThreadSample> Profiler::query_thread_samples(double time_window) const {
    std::vector<ThreadSample> result;
    auto cutoff_time = std::chrono::steady_clock::now() - std::chrono::duration<double>(time_window);

    std::lock_guard<std::mutex> lock(samples_mutex_);

    for (const auto& sample : thread_samples_) {
        if (sample.timestamp >= cutoff_time) {
            result.push_back(sample);
        }
    }

    return result;
}

std::vector<FunctionSample> Profiler::query_function_samples(double time_window) const {
    std::vector<FunctionSample> result;
    auto cutoff_time = std::chrono::steady_clock::now() - std::chrono::duration<double>(time_window);

    std::lock_guard<std::mutex> lock(samples_mutex_);

    for (const auto& sample : function_samples_) {
        if (sample.timestamp >= cutoff_time) {
            result.push_back(sample);
        }
    }

    return result;
}

std::vector<AllocationSample> Profiler::query_allocation_samples(double time_window) const {
    std::vector<AllocationSample> result;
    auto cutoff_time = std::chrono::steady_clock::now() - std::chrono::duration<double>(time_window);

    std::lock_guard<std::mutex> lock(samples_mutex_);

    for (const auto& sample : allocation_samples_) {
        if (sample.timestamp >= cutoff_time) {
            result.push_back(sample);
        }
    }

    return result;
}

void Profiler::export_data(const std::string& filename, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filename);
        if (!file.good()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }

        file << "{\n";
        file << "  \"profile_name\": \"" << "Profiler" << "\",\n";
        file << "  \"start_time\": \"" << std::chrono::system_clock::to_time_t(start_time_) << "\",\n";
        file << "  \"end_time\": \"" << std::chrono::system_clock::to_time_t(end_time_) << "\",\n";
        file << "  \"total_samples\": " << total_samples_taken_ << ",\n";
        file << "  \"cpu_samples\": [\n";

        for (size_t i = 0; i < cpu_samples_.size(); ++i) {
            const auto& sample = cpu_samples_[i];
            file << "    {\n";
            file << "      \"timestamp\": \"" << std::chrono::duration<double>(sample.timestamp - start_time_).count() << "\",\n";
            file << "      \"cpu_usage\": " << sample.cpu_usage << ",\n";
            file << "      \"user_time\": " << sample.user_time << ",\n";
            file << "      \"system_time\": " << sample.system_time << ",\n";
            file << "      \"frequency\": " << sample.frequency << ",\n";
            file << "      \"thread_id\": \"" << sample.thread_id << "\"\n";
            if (i < cpu_samples_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }

        file << "  ],\n";
        file << "  \"function_samples\": [\n";

        for (size_t i = 0; i < function_samples_.size(); ++i) {
            const auto& sample = function_samples_[i];
            file << "    {\n";
            file << "      \"timestamp\": \"" << std::chrono::duration<double>(sample.timestamp - start_time_).count() << "\",\n";
            file << "      \"thread_id\": \"" << sample.thread_id << "\",\n";
            file << "      \"function_name\": \"" << sample.function_name << "\",\n";
            file << "      \"file_path\": \"" << sample.file_path << "\",\n";
            file << "      \"line_number\": " << sample.line_number << ",\n";
            file << "      \"duration\": " << sample.duration.count() << ",\n";
            file << "      \"cpu_duration\": " << sample.cpu_duration.count() << ",\n";
            file << "      \"call_count\": " << sample.call_count << ",\n";
            if (i < function_samples_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }

        file << "  ],\n";
        file << "  \"allocation_samples\": [\n";

        for (size_t i = 0; i < allocation_samples_.size(); ++i) {
            const auto& sample = allocation_samples_[i];
            file << "    {\n";
            file << "      \"timestamp\": \"" << std::chrono::duration<double>(sample.timestamp - start_time_).count() << "\",\n";
            file << "      \"thread_id\": \"" << sample.thread_id << "\",\n";
            file << "      \"allocated_address\": \"" << sample.allocated_address << "\",\n";
            file << "      \"allocation_size\": " << sample.allocation_size << ",\n";
            file << "      \"allocator_type\": \"" << sample.allocator_type << "\",\n";
            file << "      \"allocation_site\": \"" << sample.allocation_site << "\",\n";
            file << "      \"is_allocated\": " << (sample.is_allocated ? "true" : "false") << "\n";
            if (i < allocation_samples_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }

        file << "  ]\n";
        file << "}\n";
    }
}

void Profiler::export_samples(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.good()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << "NLM Profiler Samples\n";
    file << "====================\n\n";
    file << "Profile Duration: " << analyze_profile().total_duration.count() << " seconds\n";
    file << "Total Samples: " << total_samples_taken_ << "\n";
    file << "CPU Samples: " << cpu_samples_.size() << "\n";
    file << "Memory Samples: " << memory_samples_.size() << "\n";
    file << "Thread Samples: " << thread_samples_.size() << "\n";
    file << "Function Samples: " << function_samples_.size() << "\n";
    file << "Allocation Samples: " << allocation_samples_.size() << "\n\n";

    // Export CPU samples
    file << "CPU Samples:\n";
    for (const auto& sample : cpu_samples_) {
        file << "  Time: " << std::chrono::duration<double>(sample.timestamp - start_time_).count() << "s, ";
        file << "CPU: " << sample.cpu_usage << "%, ";
        file << "Thread: " << sample.thread_id << "\n";
    }
    file << "\n";

    // Export function samples
    file << "Function Samples:\n";
    for (const auto& sample : function_samples_) {
        file << "  Time: " << std::chrono::duration<double>(sample.timestamp - start_time_).count() << "s, ";
        file << "Function: " << sample.function_name << ", ";
        file << "Duration: " << sample.duration.count() << "s, ";
        file << "Thread: " << sample.thread_id << "\n";
    }
}

void Profiler::export_analysis(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.good()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    ProfileAnalysis analysis = analyze_profile();

    file << "NLM Profiler Analysis\n";
    file << "====================\n\n";

    file << "Profile Analysis:\n";
    file << "  Profile Name: " << analysis.profile_name << "\n";
    file << "  Total Duration: " << analysis.total_duration.count() << " seconds\n";
    file << "  Total Samples: " << analysis.total_samples << "\n";
    file << "  CPU Usage: " << analysis.cpu_usage << "%\n";
    file << "  Memory Usage: " << analysis.memory_usage << " bytes\n";
    file << "  Throughput: " << analysis.throughput << " samples/second\n";
    file << "  Efficiency Score: " << analysis.efficiency_score << "\n\n";

    file << "Allocations and Deallocations:\n";
    file << "  Total Allocations: " << analysis.total_allocations << "\n";
    file << "  Total Deallocations: " << analysis.total_deallocations << "\n";
    file << "  Memory Leaks: " << analysis.memory_leak_count << "\n\n";

    file << "Hot Functions (Top 10):\n";
    for (size_t i = 0; i < analysis.hot_functions.size() && i < 10; ++i) {
        file << "  " << (i + 1) << ". " << analysis.hot_functions[i] << "\n";
    }
    file << "\n";

    file << "Cold Functions (Top 10):\n";
    for (size_t i = 0; i < analysis.cold_functions.size() && i < 10; ++i) {
        file << "  " << (i + 1) << ". " << analysis.cold_functions[i] << "\n";
    }
    file << "\n";

    file << "Bottleneck Functions (Top 10):\n";
    for (size_t i = 0; i < analysis.bottleneck_functions.size() && i < 10; ++i) {
        file << "  " << (i + 1) << ". " << analysis.bottleneck_functions[i] << "\n";
    }
    file << "\n";

    file << "Recommendations:\n";
    for (const auto& recommendation : analysis.recommendations) {
        file << "  " << recommendation << "\n";
    }
    file << "\n";

    file << "Optimization Opportunities:\n";
    for (const auto& opportunity : analysis.optimization_opportunities) {
        file << "  " << opportunity << "\n";
    }
}

void Profiler::cleanup_old_samples() {
    auto now = std::chrono::steady_clock::now();
    auto time_diff = std::chrono::duration<double>(now - last_cleanup_).count();n
    if (time_diff < cleanup_interval_) {
        return;
    }

    // Remove samples older than 1 hour
    auto cutoff_time = now - std::chrono::hours(1);

    std::lock_guard<std::mutex> lock(samples_mutex_);

    auto new_end_cpu = std::remove_if(cpu_samples_.begin(), cpu_samples_.end(),
        [cutoff_time](const CPUSample& sample) {
            return sample.timestamp < cutoff_time;
        });
    cpu_samples_.erase(new_end_cpu, cpu_samples_.end());

    auto new_end_memory = std::remove_if(memory_samples_.begin(), memory_samples_.end(),
        [cutoff_time](const MemorySample& sample) {
            return sample.timestamp < cutoff_time;
        });
    memory_samples_.erase(new_end_memory, memory_samples_.end());

    auto new_end_thread = std::remove_if(thread_samples_.begin(), thread_samples_.end(),
        [cutoff_time](const ThreadSample& sample) {
            return sample.timestamp < cutoff_time;
        });
    thread_samples_.erase(new_end_thread, thread_samples_.end());

    auto new_end_function = std::remove_if(function_samples_.begin(), function_samples_.end(),
        [cutoff_time](const FunctionSample& sample) {
            return sample.timestamp < cutoff_time;
        });
    function_samples_.erase(new_end_function, function_samples_.end());

    auto new_end_allocation = std::remove_if(allocation_samples_.begin(), allocation_samples_.end(),
        [cutoff_time](const AllocationSample& sample) {
            return sample.timestamp < cutoff_time;
        });
    allocation_samples_.erase(new_end_allocation, allocation_samples_.end());

    auto new_end_system = std::remove_if(system_samples_.begin(), system_samples_.end(),
        [cutoff_time](const SystemSample& sample) {
            return sample.timestamp < cutoff_time;
        });
    system_samples_.erase(new_end_system, system_samples_.end());

    last_cleanup_ = now;
}

void Profiler::clear_samples() {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    cpu_samples_.clear();
    memory_samples_.clear();
    thread_samples_.clear();
    function_samples_.clear();
    allocation_samples_.clear();
    system_samples_.clear();
}

void Profiler::reset() {
    std::lock_guard<std::mutex> lock(samples_mutex_);

    running_.store(false);
    start_time_ = std::chrono::steady_clock::now();
    end_time_ = start_time_;
    current_sample_index_ = 0;
    total_samples_taken_ = 0;

    sampled_functions_.clear();
    sampled_threads_.clear();

    clear_samples();

    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
}

void Profiler::enable_real_time_monitoring(bool enable) {
    if (enable && !real_time_monitoring_) {
        real_time_monitoring_ = true;
        real_time_thread_ = std::thread(&Profiler::real_time_monitoring_loop, this);
    } else if (!enable && real_time_monitoring_) {
        real_time_monitoring_ = false;
        if (real_time_thread_.joinable()) {
            real_time_thread_.join();
        }
    }
}

void Profiler::set_sample_callback(std::function<void(const CPUSample&)> callback) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    sample_callback_ = std::move(callback);
}

void Profiler::set_alert_callback(std::function<void(const ProfilerConfig&)> callback) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    alert_callback_ = std::move(callback);
}

void Profiler::sampling_loop() {
    while (running_.load()) {
        try {
            take_sample();
            update_real_time_data();
            process_sample_callbacks(cpu_samples_.empty() ? CPUSample{} : cpu_samples_.back());

            std::this_thread::sleep_for(
                std::chrono::duration<double>(config_.sampling_interval)
            );
        } catch (const std::exception& e) {
            std::cerr << "Profiler sampling loop error: " << e.what() << std::endl;
        }
    }
}

void Profiler::real_time_monitoring_loop() {
    while (real_time_monitoring_.load()) {
        try {
            update_real_time_data();

            std::this_thread::sleep_for(
                std::chrono::milliseconds(config_.real_time_update_interval)
            );
        } catch (const std::exception& e) {
            std::cerr << "Profiler real-time monitoring loop error: " << e.what() << std::endl;
        }
    }
}

void Profiler::sample_cpu(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    CPUSample sample;
    sample.timestamp = timestamp;
    sample.thread_id = thread_id;

    // Get CPU metrics
    sample.cpu_usage = get_cpu_usage();
    sample.user_time = get_user_time();
    sample.system_time = get_system_time();
    sample.frequency = get_cpu_frequency();
    sample.cache_misses = get_cache_misses();
    sample.cache_hits = get_cache_hits();
    sample.context_switches = get_context_switches();
    sample.total_instructions = get_total_instructions();
    sample.total_cycles = get_total_cycles();
    sample.instructions_per_cycle = get_instructions_per_cycle();

    add_cpu_sample(sample);
}

void Profiler::sample_memory(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    MemorySample sample;
    sample.timestamp = timestamp;
    sample.thread_id = thread_id;

    // Get memory metrics
    sample.total_memory = get_total_memory();
    sample.used_memory = get_used_memory();
    sample.free_memory = get_free_memory();
    sample.allocated_objects = get_allocated_objects();
    sample.deallocated_objects = get_deallocated_objects();
    sample.peak_memory = get_peak_memory();
    sample.current_allocation_rate = get_allocation_rate();
    sample.current_deallocation_rate = get_deallocation_rate();
    sample.memory_fragmentation = get_memory_fragmentation();

    add_memory_sample(sample);
}

void Profiler::sample_thread(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    ThreadSample sample;
    sample.timestamp = timestamp;
    sample.thread_id = thread_id;

    // Get thread metrics
    sample.thread_name = get_thread_name(thread_id);
    sample.cpu_time = get_thread_cpu_time(thread_id);
    sample.wall_time = get_thread_wall_time(thread_id);
    sample.utilization = get_thread_utilization(thread_id);
    sample.stack_size = get_thread_stack_size(thread_id);
    sample.parent_thread = get_thread_parent(thread_id);
    sample.thread_function = get_thread_function(thread_id);
    sample.creation_time = get_thread_creation_time(thread_id);

    add_thread_sample(sample);
}

void Profiler::sample_function_from_stack(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    std::vector<std::string> call_stack = get_call_stack();

    if (call_stack.empty()) {
        return;
    }

    std::string function_name = call_stack.back();
    std::string file_path = call_stack.size() >= 2 ? call_stack[call_stack.size() - 2] : "";
    size_t line_number = 0;

    // Get line number from the function
    line_number = get_function_line_number(function_name);

    // Create function sample
    FunctionSample sample;
    sample.timestamp = timestamp;
    sample.thread_id = thread_id;
    sample.function_name = function_name;
    sample.file_path = file_path;
    sample.line_number = line_number;
    sample.call_stack = call_stack;

    add_function_sample(sample);
}

void Profiler::sample_allocation_from_tracker(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    // Sample allocations from tracking system
    // This would integrate with the allocation tracking system
}

void Profiler::sample_gpu(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    // Sample GPU metrics
    // This would integrate with GPU profiling
}

void Profiler::sample_network(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    // Sample network metrics
    // This would integrate with network monitoring
}

void Profiler::sample_disk(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    // Sample disk metrics
    // This would integrate with disk monitoring
}

void Profiler::sample_all(std::chrono::steady_clock::time_point timestamp, std::thread::id thread_id) {
    // Sample all available metrics
    sample_cpu(timestamp, thread_id);
    sample_memory(timestamp, thread_id);
    sample_thread(timestamp, thread_id);
    sample_system(timestamp, thread_id);
    sample_function_from_stack(timestamp, thread_id);
}

void Profiler::add_cpu_sample(const CPUSample& sample) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    cpu_samples_.push_back(sample);

    if (cpu_samples_.size() > config_.max_samples) {
        cpu_samples_.erase(cpu_samples_.begin());
    }
}

void Profiler::add_memory_sample(const MemorySample& sample) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    memory_samples_.push_back(sample);

    if (memory_samples_.size() > config_.max_samples) {
        memory_samples_.erase(memory_samples_.begin());
    }
}

void Profiler::add_thread_sample(const ThreadSample& sample) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    thread_samples_.push_back(sample);

    if (thread_samples_.size() > config_.max_samples) {
        thread_samples_.erase(thread_samples_.begin());
    }
}

void Profiler::add_function_sample(const FunctionSample& sample) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    function_samples_.push_back(sample);

    if (function_samples_.size() > config_.max_samples) {
        function_samples_.erase(function_samples_.begin());
    }
}

void Profiler::add_allocation_sample(const AllocationSample& sample) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    allocation_samples_.push_back(sample);

    if (allocation_samples_.size() > config_.max_samples) {
        allocation_samples_.erase(allocation_samples_.begin());
    }
}

void Profiler::add_system_sample(const SystemSample& sample) {
    std::lock_guard<std::mutex> lock(samples_mutex_);
    system_samples_.push_back(sample);

    if (system_samples_.size() > config_.max_samples) {
        system_samples_.erase(system_samples_.begin());
    }
}

bool Profiler::should_sample_function(const std::string& function_name) const {
    // Check if function should be sampled based on patterns
    if (config_.include_patterns.empty()) {
        return true;  // Sample all if no patterns specified
    }

    for (const auto& regex : include_regexes_) {
        if (std::regex_search(function_name, regex)) {
            return true;
        }
    }

    return false;
}

bool Profiler::should_sample_thread(std::thread::id thread_id) const {
    // Check if thread should be sampled
    if (config_.include_patterns.empty()) {
        return true;  // Sample all if no patterns specified
    }

    // For now, sample all threads
    // Could be enhanced to check thread name patterns
    return true;
}

void Profiler::update_real_time_data() {
    // Update real-time data
    // This would update any real-time metrics
}

void Profiler::process_sample_callbacks(const CPUSample& sample) {
    if (sample_callback_) {
        sample_callback_(sample);
    }

    // Check for alerts
    if (alert_callback_) {
        double cpu_usage = sample.cpu_usage;
        double memory_usage = 0.0;  // Would need to get from memory samples

        if (cpu_usage > config_.cpu_threshold || memory_usage > config_.memory_threshold) {
            alert_callback_(config_);
        }
    }
}

// Helper function implementations

std::vector<std::string> Profiler::get_call_stack() {
    std::vector<std::string> stack;
    // Get call stack (implementation depends on platform)
    // For now, return empty vector
    return stack;
}

std::string Profiler::get_allocation_type(size_t size, const std::string& allocator_type) {
    if (allocator_type == "default") {
        if (size < 1024) return "small_object";
        else if (size < 1024 * 1024) return "medium_object";
        else return "large_object";
    } else if (allocator_type == "memory_pool") {
        return "pooled_object";
    }
    return "unknown_object";
}

double Profiler::get_system_load() {
    // Get system load average
    // Implementation depends on platform
    return 0.0;
}

double Profiler::get_cpu_temperature() {
    // Get CPU temperature
    // Implementation depends on platform
    return 0.0;
}

double Profiler::get_disk_usage() {
    // Get disk usage
    // Implementation depends on platform
    return 0.0;
}

double Profiler::get_network_usage() {
    // Get network usage
    // Implementation depends on platform
    return 0.0;
}

uint64_t Profiler::get_network_bytes_sent() {
    // Get network bytes sent
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_network_bytes_received() {
    // Get network bytes received
    // Implementation depends on platform
    return 0;
}

double Profiler::get_battery_level() {
    // Get battery level
    // Implementation depends on platform
    return 0.0;
}

std::string Profiler::get_power_state() {
    // Get power state
    // Implementation depends on platform
    return "unknown";
}

double Profiler::get_cpu_usage() {
    // Get CPU usage
    // Implementation depends on platform
    return 0.0;
}

double Profiler::get_user_time() {
    // Get user time
    // Implementation depends on platform
    return 0.0;
}

double Profiler::get_system_time() {
    // Get system time
    // Implementation depends on platform
    return 0.0;
}

double Profiler::get_cpu_frequency() {
    // Get CPU frequency
    // Implementation depends on platform
    return 0.0;
}

uint64_t Profiler::get_cache_misses() {
    // Get cache misses
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_cache_hits() {
    // Get cache hits
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_context_switches() {
    // Get context switches
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_total_instructions() {
    // Get total instructions
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_total_cycles() {
    // Get total cycles
    // Implementation depends on platform
    return 0;
}

double Profiler::get_instructions_per_cycle() {
    // Get instructions per cycle
    // Implementation depends on platform
    return 0.0;
}

uint64_t Profiler::get_total_memory() {
    // Get total memory
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_used_memory() {
    // Get used memory
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_free_memory() {
    // Get free memory
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_allocated_objects() {
    // Get allocated objects
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_deallocated_objects() {
    // Get deallocated objects
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_peak_memory() {
    // Get peak memory
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_allocation_rate() {
    // Get allocation rate
    // Implementation depends on platform
    return 0;
}

uint64_t Profiler::get_deallocation_rate() {
    // Get deallocation rate
    // Implementation depends on platform
    return 0;
}

double Profiler::get_memory_fragmentation() {
    // Get memory fragmentation
    // Implementation depends on platform
    return 0.0;
}

std::string Profiler::get_thread_name(std::thread::id thread_id) {
    // Get thread name
    // Implementation depends on platform
    return "unknown";
}

std::chrono::duration<double> Profiler::get_thread_cpu_time(std::thread::id thread_id) {
    // Get thread CPU time
    // Implementation depends on platform
    return std::chrono::duration<double>(0);
}

std::chrono::duration<double> Profiler::get_thread_wall_time(std::thread::id thread_id) {
    // Get thread wall time
    // Implementation depends on platform
    return std::chrono::duration<double>(0);
}

double Profiler::get_thread_utilization(std::thread::id thread_id) {
    // Get thread utilization
    // Implementation depends on platform
    return 0.0;
}

size_t Profiler::get_thread_stack_size(std::thread::id thread_id) {
    // Get thread stack size
    // Implementation depends on platform
    return 0;
}

std::thread::id Profiler::get_thread_parent(std::thread::id thread_id) {
    // Get thread parent
    // Implementation depends on platform
    return std::thread::id();
}

std::string Profiler::get_thread_function(std::thread::id thread_id) {
    // Get thread function
    // Implementation depends on platform
    return "unknown";
}

std::chrono::steady_clock::time_point Profiler::get_thread_creation_time(std::thread::id thread_id) {
    // Get thread creation time
    // Implementation depends on platform
    return std::chrono::steady_clock::now();
}

size_t Profiler::get_function_line_number(const std::string& function_name) {
    // Get function line number
    // Implementation depends on platform
    return 0;
}

std::vector<std::string> Profiler::generate_recommendations(const ProfileAnalysis& analysis) const {
    std::vector<std::string> recommendations;

    if (analysis.cpu_usage > 80.0) {
        recommendations.push_back("Consider optimizing CPU-intensive functions");
    }

    if (analysis.memory_usage > 80.0) {
        recommendations.push_back("Consider optimizing memory usage");
    }

    if (analysis.memory_leak_count > 0) {
        recommendations.push_back("Fix memory leaks detected in allocation samples");
    }

    if (analysis.hot_functions.size() > 0) {
        recommendations.push_back("Profile hot functions for optimization opportunities");
    }

    if (analysis.efficiency_score < 0.5) {
        recommendations.push_back("Overall efficiency is low, consider architectural improvements");
    }

    return recommendations;
}

std::vector<std::string> Profiler::generate_optimization_opportunities(const ProfileAnalysis& analysis) const {
    std::vector<std::string> opportunities;

    // Analyze hot functions
    for (const auto& function : analysis.hot_functions) {
        opportunities.push_back("Optimize function: " + function);
    }

    // Analyze allocation patterns
    if (analysis.memory_leak_count > 0) {
        opportunities.push_back("Address memory leaks");
    }

    // Analyze thread utilization
    double avg_thread_utilization = 0.0;
    if (!thread_samples_.empty()) {
        double total_util = 0.0;
        for (const auto& sample : thread_samples_) {
            total_util += sample.utilization;
        }
        avg_thread_utilization = total_util / thread_samples_.size();

        if (avg_thread_utilization < 50.0) {
            opportunities.push_back("Improve thread utilization");
        }
    }

    return opportunities;
}

} // namespace nlm
