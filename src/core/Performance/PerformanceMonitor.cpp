// Enhanced NLM Performance Monitor Implementation
// File: src/core/Performance/PerformanceMonitor.cpp

#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <unistd.h>
    #include <sys/sysinfo.h>
    #include <sys/time.h>
    #include <sys/resource.h>
#endif

namespace nlm {

PerformanceMonitor::PerformanceMonitor(const PerformanceMonitorConfig& config) 
    : config_(config)
    , running_(false)
    , start_time_(std::chrono::steady_clock::now())
    , last_cleanup_(std::chrono::steady_clock::now())
    , total_samples_(0)
    , last_update_(std::chrono::steady_clock::now()) {
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::configure(const PerformanceMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    config_ = config;
    max_history_size_ = config.max_history_size;
    cleanup_interval_ = config.sampling_interval * 10;  // Cleanup every 10 samples
}

void PerformanceMonitor::start() {
    if (running_.load()) {
        return;
    }

    running_.store(true);
    monitoring_thread_ = std::thread(&PerformanceMonitor::update, this);
}

void PerformanceMonitor::stop() {
    if (!running_.load()) {
        return;
    }

    running_.store(false);
    
    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
}

void PerformanceMonitor::update() {
    while (running_.load()) {
        try {
            monitor_cpu();
            monitor_memory();
            monitor_threads();
            monitor_network();
            monitor_disk();
            monitor_application();
            
            collect_system_data();
            
            total_samples_++;
            
            std::this_thread::sleep_for(
                std::chrono::duration<double>(config_.sampling_interval)
            );
        } catch (const std::exception& e) {
            // Log error but continue monitoring
            std::cerr << "PerformanceMonitor error: " << e.what() << std::endl;
        }
    }
}

void PerformanceMonitor::monitor_cpu() {
    if (!config_.enable_cpu_monitoring) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    auto time_diff = std::chrono::duration<double>(now - current_snapshot_.cpu.last_update).count();
    
    if (time_diff < (config_.cpu_sampling_interval / 1000.0)) {
        return;  // Skip if not enough time passed
    }

    CPUMetrics cpu;
    cpu.last_update = now;

#ifdef _WIN32
    FILETIME idle_time, kernel_time, user_time;
    if (GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
        uint64_t idle = (static_cast<uint64_t>(idle_time.dwHighDateTime) << 32) | idle_time.dwLowDateTime;
        uint64_t kernel = (static_cast<uint64_t>(kernel_time.dwHighDateTime) << 32) | kernel_time.dwLowDateTime;
        uint64_t user = (static_cast<uint64_t>(user_time.dwHighDateTime) << 32) | user_time.dwLowDateTime;
        
        uint64_t total = kernel + user;
        if (total > 0) {
            cpu.cpu_percent = 100.0 * (total - idle) / total;
        }
        
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        cpu.frequency = sys_info.dwProcessorSpeed * 1000000;  // Convert to Hz
    }
#else
    // Linux/Unix implementation
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double current_time = ts.tv_sec + ts.tv_nsec / 1e9;
    
    std::ifstream cpu_stat("/proc/stat");
    if (cpu_stat.good()) {
        std::string line;
        std::getline(cpu_stat, line);
        if (line.substr(0, 3) == "cpu") {
            std::istringstream iss(line);
            std::string cpu_str;
            uint64_t user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
            
            iss >> cpu_str >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
            
            uint64_t total_idle = idle + iowait;
            uint64_t total_non_idle = user + nice + system + irq + softirq + steal + guest + guest_nice;
            uint64_t total = total_idle + total_non_idle;
            
            static uint64_t prev_total = 0;
            static uint64_t prev_idle = 0;
            
            if (prev_total > 0) {
                uint64_t total_diff = total - prev_total;
                uint64_t idle_diff = total_idle - prev_idle;
                
                if (total_diff > 0) {
                    cpu.cpu_percent = 100.0 * (total_diff - idle_diff) / total_diff;
                }
            }
            
            prev_total = total;
            prev_idle = total_idle;
        }
    }
    
    // Get CPU frequency
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.good()) {
        std::string line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("cpu MHz") != std::string::npos) {
                std::istringstream iss(line);
                std::string key;
                double MHz;
                iss >> key >> MHz;
                cpu.frequency = MHz * 1000000;  // Convert to Hz
                break;
            }
        }
    }
#endif

    current_snapshot_.cpu = cpu;
}

void PerformanceMonitor::monitor_memory() {
    if (!config_.enable_memory_monitoring) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    auto time_diff = std::chrono::duration<double>(now - current_snapshot_.memory.last_update).count();
    
    if (time_diff < (config_.memory_sampling_interval / 1000.0)) {
        return;  // Skip if not enough time passed
    }

    MemoryMetrics memory;
    memory.last_update = now;

#ifdef _WIN32
    MEMORYSTATUSEX mem_status;
    mem_status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&mem_status)) {
        memory.total_memory = mem_status.ullTotalPhys;
        memory.used_memory = mem_status.ullTotalPhys - mem_status.ullAvailPhys;
        memory.free_memory = mem_status.ullAvailPhys;
        memory.allocated_objects = mem_status.dwObjects - mem_status.dwObjects;  // Simplified
        memory.peak_memory = mem_status.ullTotalPageFile - mem_status.ullAvailPageFile;
    }
#else
    // Linux/Unix implementation
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        memory.total_memory = info.totalram * info.mem_unit;
        memory.free_memory = info.freeram * info.mem_unit;
        memory.used_memory = memory.total_memory - memory.free_memory;
        memory.peak_memory = info.totalsw * info.mem_unit;  // Swap space
    }
    
    // Get process-specific memory info
    std::ifstream status("/proc/self/status");
    if (status.good()) {
        std::string line;
        while (std::getline(status, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                std::istringstream iss(line);
                std::string key;
                uint64_t rss;
                iss >> key >> rss;
                memory.used_memory = rss * 1024;  // Convert KB to bytes
            } else if (line.find("VmSize:") != std::string::npos) {
                std::istringstream iss(line);
                std::string key;
                uint64_t size;
                iss >> key >> size;
                memory.total_memory = size * 1024;  // Convert KB to bytes
            }
        }
    }
#endif

    memory.free_memory = memory.total_memory - memory.used_memory;
    memory.allocated_objects = total_samples_ / 1000;  // Approximation
    memory.current_allocation_rate = static_cast<uint64_t>(memory.allocated_objects / time_diff);
    
    // Calculate memory fragmentation (simplified)
    if (!current_snapshot_.memory.allocation_sizes.empty()) {
        double avg_size = std::accumulate(
            current_snapshot_.memory.allocation_sizes.begin(),
            current_snapshot_.memory.allocation_sizes.end(),
            0.0
        ) / current_snapshot_.memory.allocation_sizes.size();
        
        double variance = 0.0;
        for (uint64_t size : current_snapshot_.memory.allocation_sizes) {
            variance += (size - avg_size) * (size - avg_size);
        }
        variance /= current_snapshot_.memory.allocation_sizes.size();
        
        memory.memory_fragmentation = std::sqrt(variance) / avg_size;
    }

    current_snapshot_.memory = memory;
}

void PerformanceMonitor::monitor_threads() {
    if (!config_.enable_thread_monitoring) {
        return;
    }

    ThreadMetrics threads;
    threads.last_update = std::chrono::steady_clock::now();

    // Get current thread count
    threads.active_threads = static_cast<size_t>(std::thread::hardware_concurrency());
    threads.total_threads = threads.active_threads * 2;  // Estimate
    threads.peak_threads = threads.total_threads / 2;    // Simplified

    current_snapshot_.threads = threads;
}

void PerformanceMonitor::monitor_network() {
    if (!config_.enable_network_monitoring) {
        return;
    }

    NetworkMetrics network;
    network.last_update = std::chrono::steady_clock::now();

    // Simplified network metrics - would need platform-specific implementation
    // For Linux: read /proc/net/dev
    // For Windows: use GetIfTable
    // For macOS: use getifaddrs

    current_snapshot_.network = network;
}

void PerformanceMonitor::monitor_disk() {
    if (!config_.enable_disk_monitoring) {
        return;
    }

    DiskMetrics disk;
    disk.last_update = std::chrono::steady_clock::now();

    // Simplified disk metrics - would need platform-specific implementation
    // For Linux: read /proc/diskstats
    // For Windows: use GetDiskFreeSpaceEx
    // For macOS: use disk_usage

    current_snapshot_.disk = disk;
}

void PerformanceMonitor::monitor_application() {
    ApplicationMetrics application;
    application.last_update = std::chrono::steady_clock::now();

    // Get current process metrics
    auto now = application.last_update;
    auto start_diff = std::chrono::duration<double>(now - start_time_).count();
    
    application.wall_time = start_diff;
    application.cpu_time = start_diff * current_snapshot_.cpu.cpu_percent / 100.0;
    application.memory_usage = current_snapshot_.memory.used_memory;
    application.peak_memory = current_snapshot_.memory.peak_memory;
    
    if (start_diff > 0) {
        application.operations_per_second = total_samples_ / start_diff;
    }

    current_snapshot_.application = application;
}

void PerformanceMonitor::collect_system_data() {
    // Collect system load average
#ifdef _WIN32
    // Windows implementation
    DOUBLE load = 0.0;
    if (GetSystemLoadAverage(&load)) {
        current_snapshot_.system_load = load;
    }
#else
    // Unix/Linux implementation
    std::ifstream loadavg("/proc/loadavg");
    if (loadavg.good()) {
        std::string line;
        std::getline(loadavg, line);
        std::istringstream iss(line);
        iss >> current_snapshot_.system_load;
    }
#endif

    // Get CPU temperature (Linux)
#ifdef __linux__
    std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
    if (temp_file.good()) {
        int temp_celsius;
        temp_file >> temp_celsius;
        current_snapshot_.temperature = temp_celsius / 10.0;  // Convert to Celsius
    }
#endif
}

PerformanceSnapshot PerformanceMonitor::create_snapshot() const {
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::steady_clock::now();
    snapshot.cpu = current_snapshot_.cpu;
    snapshot.memory = current_snapshot_.memory;
    snapshot.threads = current_snapshot_.threads;
    snapshot.network = current_snapshot_.network;
    snapshot.disk = current_snapshot_.disk;
    snapshot.application = current_snapshot_.application;
    snapshot.system_load = current_snapshot_.system_load;
    snapshot.temperature = current_snapshot_.temperature;
    snapshot.process_name = "nlm";
    snapshot.process_id = getpid();
    
    return snapshot;
}

void PerformanceMonitor::add_snapshot_to_history(const PerformanceSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    history_.push_back(snapshot);
    
    // Remove oldest snapshots if exceeding limit
    if (history_.size() > max_history_size_) {
        history_.erase(history_.begin());
    }
    
    cleanup_old_data();
}

void PerformanceMonitor::check_alerts(const PerformanceSnapshot& snapshot) {
    if (alert_callback_) {
        bool alert_triggered = false;
        
        // CPU alert
        if (snapshot.cpu.cpu_percent > cpu_threshold_) {
            alert_callback_(snapshot);
            alert_triggered = true;
        }
        
        // Memory alert
        if (snapshot.memory.used_memory > (memory_threshold_ / 100.0 * snapshot.memory.total_memory)) {
            alert_callback_(snapshot);
            alert_triggered = true;
        }
        
        // Thread alert
        if (snapshot.threads.active_threads > thread_threshold_) {
            alert_callback_(snapshot);
            alert_triggered = true;
        }
        
        // Log alerts
        if (alert_triggered) {
            std::cerr << "PerformanceMonitor alert: Resource threshold exceeded!" << std::endl;
        }
    }
}

void PerformanceMonitor::update_performance_counters() {
    // Update performance counters for analysis
    // This would include things like cache hit rates, context switches, etc.
}

void PerformanceMonitor::cleanup_old_data() {
    auto now = std::chrono::steady_clock::now();
    auto time_diff = std::chrono::duration<double>(now - last_cleanup_).count();
    
    if (time_diff < cleanup_interval_) {
        return;
    }
    
    // Keep only recent data (last hour)
    auto cutoff_time = now - std::chrono::hours(1);
    
    std::lock_guard<std::mutex> lock(data_mutex_);
    auto new_end = std::remove_if(history_.begin(), history_.end(),
        [cutoff_time](const PerformanceSnapshot& snapshot) {
            return snapshot.timestamp < cutoff_time;
        });
    
    history_.erase(new_end, history_.end());
    last_cleanup_ = now;
}

std::vector<PerformanceSnapshot> PerformanceMonitor::get_history(double time_window) const {
    auto cutoff_time = std::chrono::steady_clock::now() - std::chrono::duration<double>(time_window);
    
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    std::vector<PerformanceSnapshot> result;
    for (const auto& snapshot : history_) {
        if (snapshot.timestamp >= cutoff_time) {
            result.push_back(snapshot);
        }
    }
    
    return result;
}

std::vector<PerformanceSnapshot> PerformanceMonitor::get_history(size_t max_count) const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    if (history_.size() <= max_count) {
        return history_;
    }
    
    std::vector<PerformanceSnapshot> result;
    result.reserve(max_count);
    
    auto start = history_.end() - max_count;
    for (auto it = start; it != history_.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

double PerformanceMonitor::calculate_cpu_utilization(double time_window) const {
    auto snapshots = get_history(time_window);
    if (snapshots.empty()) {
        return 0.0;
    }
    
    double total_cpu_time = 0.0;
    for (const auto& snapshot : snapshots) {
        total_cpu_time += snapshot.cpu.cpu_percent;
    }
    
    return total_cpu_time / snapshots.size();
}

double PerformanceMonitor::calculate_memory_utilization(double time_window) const {
    auto snapshots = get_history(time_window);
    if (snapshots.empty()) {
        return 0.0;
    }
    
    double total_memory_usage = 0.0;
    double total_memory_total = 0.0;
    for (const auto& snapshot : snapshots) {
        total_memory_usage += static_cast<double>(snapshot.memory.used_memory);
        total_memory_total += static_cast<double>(snapshot.memory.total_memory);
    }
    
    return total_memory_usage / total_memory_total * 100.0;
}

double PerformanceMonitor::calculate_thread_utilization(double time_window) const {
    auto snapshots = get_history(time_window);
    if (snapshots.empty()) {
        return 0.0;
    }
    
    double total_thread_usage = 0.0;
    for (const auto& snapshot : snapshots) {
        total_thread_usage += snapshot.threads.active_threads;
    }
    
    return total_thread_usage / snapshots.size();
}

std::vector<double> PerformanceMonitor::get_cpu_history(double time_window) const {
    auto snapshots = get_history(time_window);
    std::vector<double> cpu_history;
    cpu_history.reserve(snapshots.size());
    
    for (const auto& snapshot : snapshots) {
        cpu_history.push_back(snapshot.cpu.cpu_percent);
    }
    
    return cpu_history;
}

std::vector<double> PerformanceMonitor::get_memory_history(double time_window) const {
    auto snapshots = get_history(time_window);
    std::vector<double> memory_history;
    memory_history.reserve(snapshots.size());
    
    for (const auto& snapshot : snapshots) {
        memory_history.push_back(static_cast<double>(snapshot.memory.used_memory) / snapshot.memory.total_memory * 100.0);
    }
    
    return memory_history;
}

std::vector<double> PerformanceMonitor::get_thread_history(double time_window) const {
    auto snapshots = get_history(time_window);
    std::vector<double> thread_history;
    thread_history.reserve(snapshots.size());
    
    for (const auto& snapshot : snapshots) {
        thread_history.push_back(static_cast<double>(snapshot.threads.active_threads));
    }
    
    return thread_history;
}

void PerformanceMonitor::export_data(const std::string& filename, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filename);
        if (!file.good()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "{\n";
        file << "  \"samples\": " << history_.size() << ",\n";
        file << "  \"start_time\": \"" << std::chrono::system_clock::to_time_t(start_time_) << "\",\n";
        file << "  \"snapshots\": [\n";
        
        for (size_t i = 0; i < history_.size(); ++i) {
            const auto& snapshot = history_[i];
            file << "    {\n";
            file << "      \"timestamp\": \"" << std::chrono::duration<double>(snapshot.timestamp - start_time_).count() << "\",\n";
            file << "      \"cpu_percent\": " << snapshot.cpu.cpu_percent << ",\n";
            file << "      \"memory_used\": " << snapshot.memory.used_memory << ",\n";
            file << "      \"memory_total\": " << snapshot.memory.total_memory << ",\n";
            file << "      \"threads_active\": " << snapshot.threads.active_threads << ",\n";
            file << "      \"system_load\": " << snapshot.system_load << "\n";
            if (i < history_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
    }
}

void PerformanceMonitor::export_metrics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.good()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "Performance Monitor Data\n";
    file << "========================\n\n";
    file << "Start Time: " << std::chrono::system_clock::to_time_t(start_time_) << "\n";
    file << "Total Samples: " << total_samples_ << "\n";
    file << "Current Samples: " << history_.size() << "\n\n";
    
    if (!history_.empty()) {
        const auto& latest = history_.back();
        
        file << "Current Metrics:\n";
        file << "  CPU Usage: " << latest.cpu.cpu_percent << "%\n";
        file << "  Memory Usage: " << (latest.memory.used_memory / (1024 * 1024)) << " MB / "
             << (latest.memory.total_memory / (1024 * 1024)) << " MB\n";
        file << "  Active Threads: " << latest.threads.active_threads << "\n";
        file << "  System Load: " << latest.system_load << "\n";
        file << "  Uptime: " << get_uptime() << " seconds\n\n";
        
        file << "Historical Averages:\n";
        if (history_.size() >= 10) {
            auto recent_samples = get_history(10.0);
            if (!recent_samples.empty()) {
                double avg_cpu = 0.0, avg_memory = 0.0, avg_threads = 0.0;
                for (const auto& snapshot : recent_samples) {
                    avg_cpu += snapshot.cpu.cpu_percent;
                    avg_memory += static_cast<double>(snapshot.memory.used_memory) / snapshot.memory.total_memory * 100.0;
                    avg_threads += snapshot.threads.active_threads;
                }
                
                avg_cpu /= recent_samples.size();
                avg_memory /= recent_samples.size();
                avg_threads /= recent_samples.size();
                
                file << "  Average CPU Usage (last 10s): " << avg_cpu << "%\n";
                file << "  Average Memory Usage (last 10s): " << avg_memory << "%\n";
                file << "  Average Threads (last 10s): " << avg_threads << "\n";
            }
        }
    }
}

double PerformanceMonitor::get_uptime() const {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration<double>(now - start_time_).count();
    return uptime;
}

} // namespace nlm