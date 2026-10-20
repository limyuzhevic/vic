#include "PerformanceMonitor.hpp"

namespace nlm {

PerformanceMonitor::PerformanceMonitor()
    : timeSinceLastUpdate(0.0)
    , totalSteps(0)
    , startTime(std::chrono::steady_clock::now())
    , lastUpdateTime(std::chrono::steady_clock::now())
    , updateInterval(1.0) // 1 second
    , currentFPS(0.0)
    , currentTPS(0.0)
    , totalMemoryUsage(0)
    , peakMemoryUsage(0)
    , cpuUsage(0.0)
    , activeTimers(std::map<std::string, TimerData>())
    , isMonitoring(false)
{
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

void PerformanceMonitor::startMonitoring() {
    if (!isMonitoring) {
        isMonitoring = true;
        startTime = std::chrono::steady_clock::now();
        lastUpdateTime = startTime;
        NLM_LOG_INFO("Performance monitoring started");
    }
}

void PerformanceMonitor::stopMonitoring() {
    if (isMonitoring) {
        isMonitoring = false;
        NLM_LOG_INFO("Performance monitoring stopped");
    }
}

void PerformanceMonitor::update(double timestep) {
    if (!isMonitoring) return;
    
    currentTime = std::chrono::steady_clock::now();
    timeSinceLastUpdate += timestep;
    totalSteps++;
    
    // Update FPS every updateInterval seconds
    std::chrono::duration<double> elapsedSinceUpdate = currentTime - lastUpdateTime;
    if (elapsedSinceUpdate.count() >= updateInterval) {
        double updateTime = elapsedSinceUpdate.count();
        currentFPS = static_cast<double>(framesSinceLastUpdate) / updateTime;
        framesSinceLastUpdate = 0;
        lastUpdateTime = currentTime;
        
        // Reset memory tracking
        size_t currentMemUsage = getCurrentMemoryUsage();
        if (currentMemUsage > peakMemoryUsage) {
            peakMemoryUsage = currentMemUsage;
        }
        
        NLM_LOG_DEBUG("Performance: " + std::to_string(currentFPS) + " FPS, " + 
                     std::to_string(currentTPS) + " TPS, " +
                     std::to_string(currentMemUsage) + " bytes memory");
    }
    
    framesSinceLastUpdate++;
    currentTPS = static_cast<double>(totalSteps) / timeSinceLastUpdate;
    
    // Update active timers
    for (auto& pair : activeTimers) {
        TimerData& timer = pair.second;
        if (timer.isRunning) {
            timer.elapsedTime += timestep;
        }
    }
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Report ===\n";
    ss << "Total Steps: " << totalSteps << "\n";
    ss << "Total Time: " << timeSinceLastUpdate << " seconds\n";
    ss << "Current FPS: " << currentFPS << "\n";
    ss << "Current TPS: " << currentTPS << "\n";
    ss << "Memory Usage: " << totalMemoryUsage << " bytes\n";
    ss << "Peak Memory Usage: " << peakMemoryUsage << " bytes\n";
    ss << "CPU Usage: " << cpuUsage << "%\n";
    ss << "\nActive Timers:\n";
    
    for (const auto& pair : activeTimers) {
        const TimerData& timer = pair.second;
        if (timer.isRunning) {
            ss << "  " << pair.first << ": " << timer.elapsedTime << "s (running)\n";
        } else {
            ss << "  " << pair.first << ": " << timer.elapsedTime << "s (paused)\n";
        }
    }
    
    return ss.str();
}

void PerformanceMonitor::startTimer(const std::string& name) {
    activeTimers[name].isRunning = true;
    activeTimers[name].startTime = std::chrono::steady_clock::now();
}

void PerformanceMonitor::stopTimer(const std::string& name) {
    if (activeTimers.find(name) != activeTimers.end()) {
        TimerData& timer = activeTimers[name];
        if (timer.isRunning) {
            timer.isRunning = false;
            std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - timer.startTime;
            timer.elapsedTime += elapsed.count();
        }
    }
}

void PerformanceMonitor::resetTimer(const std::string& name) {
    if (activeTimers.find(name) != activeTimers.end()) {
        activeTimers[name].elapsedTime = 0.0;
        activeTimers[name].isRunning = false;
    }
}

double PerformanceMonitor::getTimerTime(const std::string& name) const {
    if (activeTimers.find(name) != activeTimers.end()) {
        return activeTimers.at(name).elapsedTime;
    }
    return 0.0;
}

bool PerformanceMonitor::isTimerRunning(const std::string& name) const {
    if (activeTimers.find(name) != activeTimers.end()) {
        return activeTimers.at(name).isRunning;
    }
    return false;
}

size_t PerformanceMonitor::getCurrentMemoryUsage() {
    // Implementation varies by platform
    // For Linux/macOS, we can use /proc/self/status or getrusage
    // For Windows, use GetProcessMemoryInfo
    // For cross-platform, use a portable approach
    
#ifdef __linux__
    std::ifstream statusFile("/proc/self/status");
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") == 0) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string memoryKB = line.substr(colonPos + 1);
                size_t pos = 0;
                while ((pos = memoryKB.find(' ')) != std::string::npos) {
                    memoryKB.erase(pos, 1);
                }
                return std::stoul(memoryKB) * 1024; // Convert KB to bytes
            }
        }
    }
#endif
    
    // Fallback to a simple approximation
    return 0;
}

void PerformanceMonitor::setUpdateInterval(double interval) {
    updateInterval = interval;
}

std::string PerformanceMonitor::getSystemInfo() {
    std::stringstream ss;
    ss << "=== NLM System Information ===\n";
    ss << "Architecture: " << getArchitectureString() << "\n";
    ss << "Compiler: " << getCompilerString() << "\n";
    ss << "Build Type: " << getBuildTypeString() << "\n";
    ss << "Git Hash: " << getGitHash() << "\n";
    ss << "Build Date: " << getBuildDate() << "\n";
    return ss.str();
}

std::string PerformanceMonitor::getArchitectureString() {
#ifdef __x86_64__
    return "x86_64";
#elif __i386__
    return "x86";
#elif __aarch64__
    return "ARM64";
#elif __arm__
    return "ARM";
#else
    return "Unknown";
#endif
}

std::string PerformanceMonitor::getCompilerString() {
#ifdef __GNUC__
    return "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__);
#elif __clang__
    return "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__);
#elif _MSC_VER
    return "MSVC " + std::to_string(_MSC_VER);
#else
    return "Unknown compiler";
#endif
}

std::string PerformanceMonitor::getBuildTypeString() {
#ifdef NDEBUG
    return "Release";
#else
    return "Debug";
#endif
}

std::string PerformanceMonitor::getGitHash() {
    // Try to get git hash from version control info
    // This is a simple implementation - could be more robust
    std::string gitFile = ".git/refs/heads/main";
    std::ifstream gitFileStream(gitFile);
    if (gitFileStream.is_open()) {
        std::string hash((std::istreambuf_iterator<char>(gitFileStream)), std::istreambuf_iterator<char>());
        return "Git: " + hash.substr(0, 8); // Short hash
    }
    
    return "Git: unknown";
}

std::string PerformanceMonitor::getBuildDate() {
    return __DATE__;
}

void PerformanceMonitor::exportPerformanceData(const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << getPerformanceReport();
        file.close();
        NLM_LOG_INFO("Performance data exported to " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to export performance data to " + filepath);
    }
}

void PerformanceMonitor::saveCheckpoint(const std::string& filepath) {
    // This could integrate with the existing CheckpointSystem
    // For now, just log that we're attempting to save performance data
    NLM_LOG_INFO("Attempting to save checkpoint to " + filepath);
    // TODO: Implement actual checkpoint saving with PerformanceMonitor data
}

void PerformanceMonitor::loadCheckpoint(const std::string& filepath) {
    // This could integrate with the existing CheckpointSystem
    // For now, just log that we're attempting to load performance data
    NLM_LOG_INFO("Attempting to load checkpoint from " + filepath);
    // TODO: Implement actual checkpoint loading with PerformanceMonitor data
}

void PerformanceMonitor::logPerformanceData() {
    NLM_LOG_INFO(getPerformanceReport());
}

void PerformanceMonitor::logSystemInfo() {
    NLM_LOG_INFO(getSystemInfo());
}

std::vector<TimerData> PerformanceMonitor::getAllTimers() const {
    std::vector<TimerData> timers;
    for (const auto& pair : activeTimers) {
        timers.push_back(pair.second);
    }
    return timers;
}

void PerformanceMonitor::clearTimers() {
    activeTimers.clear();
}

void PerformanceMonitor::setCPUUsage(double usage) {
    cpuUsage = usage;
}

void PerformanceMonitor::setMemoryUsage(size_t usage) {
    totalMemoryUsage = usage;
}

PerformanceMonitor::PerformanceMonitor(const PerformanceMonitor& other) {
    *this = other;
}

PerformanceMonitor& PerformanceMonitor::operator=(const PerformanceMonitor& other) {
    if (this != &other) {
        timeSinceLastUpdate = other.timeSinceLastUpdate;
        totalSteps = other.totalSteps;
        startTime = other.startTime;
        lastUpdateTime = other.lastUpdateTime;
        updateInterval = other.updateInterval;
        currentFPS = other.currentFPS;
        currentTPS = other.currentTPS;
        totalMemoryUsage = other.totalMemoryUsage;
        peakMemoryUsage = other.peakMemoryUsage;
        cpuUsage = other.cpuUsage;
        activeTimers = other.activeTimers;
        isMonitoring = other.isMonitoring;
        framesSinceLastUpdate = other.framesSinceLastUpdate;
    }
    return *this;
}

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&& other) noexcept {
    *this = std::move(other);
}

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&& other) noexcept {
    if (this != &other) {
        timeSinceLastUpdate = other.timeSinceLastUpdate;
        totalSteps = other.totalSteps;
        startTime = std::move(other.startTime);
        lastUpdateTime = std::move(other.lastUpdateTime);
        updateInterval = other.updateInterval;
        currentFPS = other.currentFPS;
        currentTPS = other.currentTPS;
        totalMemoryUsage = other.totalMemoryUsage;
        peakMemoryUsage = other.peakMemoryUsage;
        cpuUsage = other.cpuUsage;
        activeTimers = std::move(other.activeTimers);
        isMonitoring = other.isMonitoring;
        framesSinceLastUpdate = other.framesSinceLastUpdate;
    }
    return *this;
}

} // namespace nlm
