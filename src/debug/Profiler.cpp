// Profiler implementation

#include "debug/Profiler.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

Profiler::Profiler() : samplingThreadRunning(false), sampleInterval(1.0), 
                      maxSamplesPerStep(10), benchmarking(false), 
                      currentCpuUsage(0.0), currentMemoryUsage(0.0),
                      currentMemoryAllocated(0), currentMemoryPeak(0) {
    loadDefaultConfiguration();
}

void Profiler::startProfiling(const std::vector<ProfileTarget>& targets) {
    std::lock_guard<std::mutex> lock(mutex);
    
    enabledTargets = targets;
    
    if (enabledTargets.empty()) {
        enabledTargets = {ProfileTarget::CPU, ProfileTarget::Memory};
    }
    
    // Clear old samples
    samples.clear();
    
    // Start sampling thread
    if (!samplingThreadRunning) {
        samplingThreadRunning = true;
        sampleThread = std::thread(&Profiler::samplingThread, this);
    }
}

void Profiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex);
    
    samplingThreadRunning = false;
    
    if (sampleThread.joinable()) {
        sampleThread.join();
    }
    
    // Update function profiles
    updateFunctionProfiles();
}

bool Profiler::isProfiling() const {
    std::lock_guard<std::mutex> lock(mutex);
    return samplingThreadRunning && !enabledTargets.empty();
}

void Profiler::takeSample(uint64_t step, const std::string& component) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if sampling is active
    if (!isProfiling()) {
        return;
    }
    
    // Update performance metrics
    updatePerformanceMetrics();
    
    // Create sample
    ProfileSample sample(step, currentCpuUsage, currentMemoryUsage, 
                       0.0, currentMemoryAllocated, currentMemoryPeak, 
                       std::thread::hardware_concurrency(), 0.0, component);
    
    // Add custom metrics based on enabled targets
    if (std::find(enabledTargets.begin(), enabledTargets.end(), ProfileTarget::CPU) != enabledTargets.end()) {
        sample.customMetrics["cpu_usage"] = currentCpuUsage;
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), ProfileTarget::Memory) != enabledTargets.end()) {
        sample.customMetrics["memory_usage"] = currentMemoryUsage;
        sample.customMetrics["memory_allocated"] = static_cast<double>(currentMemoryAllocated);
        sample.customMetrics["memory_peak"] = static_cast<double>(currentMemoryPeak);
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), ProfileTarget::GPU) != enabledTargets.end()) {
        // GPU metrics would be collected here
        sample.customMetrics["gpu_usage"] = 0.0; // Placeholder
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), ProfileTarget::Network) != enabledTargets.end()) {
        // Network metrics would be collected here
        sample.customMetrics["network_bandwidth"] = 0.0; // Placeholder
    }
    
    samples.push_back(sample);
    
    // Cleanup if buffer is full
    if (samples.size() > maxSamplesPerStep * 1000) { // Arbitrary limit
        samples.erase(samples.begin(), samples.begin() + samples.size() - maxSamplesPerStep * 1000);
    }
    
    // Notify listeners
    for (const auto& listener : profileListeners) {
        try {
            listener(sample);
        } catch (const std::exception& e) {
            std::cerr << "Error in profile listener: " << e.what() << std::endl;
        }
    }
}

void Profiler::startFunctionProfile(const std::string& functionName, const std::string& component) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string key = functionName + "|" + component;
    
    if (activeFunctionProfiles.find(key) != activeFunctionProfiles.end()) {
        return; // Already profiling this function
    }
    
    FunctionProfileData& profileData = activeFunctionProfiles[key];
    profileData.startTime = std::chrono::steady_clock::now();
}

void Profiler::stopFunctionProfile(const std::string& functionName, const std::string& component) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string key = functionName + "|" + component;
    
    auto it = activeFunctionProfiles.find(key);
    if (it == activeFunctionProfiles.end()) {
        return; // Not currently profiling this function
    }
    
    FunctionProfileData& profileData = it->second;
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - profileData.startTime);
    
    profileData.accumulatedTime += duration;
    profileData.callCount++;
    
    if (profileData.callCount == 1 || duration < profileData.minTime) {
        profileData.minTime = duration;
    }
    if (duration > profileData.maxTime) {
        profileData.maxTime = duration;
    }
    
    activeFunctionProfiles.erase(it);
    
    // Notify function profile listeners
    FunctionProfile profile(functionName, component);
    profile.totalTime = profileData.accumulatedTime;
    profile.minTime = profileData.minTime;
    profile.maxTime = profileData.maxTime;
    profile.callCount = profileData.callCount;
    
    for (const auto& listener : functionProfileListeners) {
        try {
            listener(profile);
        } catch (const std::exception& e) {
            std::cerr << "Error in function profile listener: " << e.what() << std::endl;
        }
    }
}

ProfileStats Profiler::getProfileStats(uint64_t startStep, uint64_t endStep) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    ProfileStats stats;
    
    // Calculate statistics from samples
    for (const auto& sample : samples) {
        if (sample.step >= startStep && sample.step <= endStep) {
            stats.totalSamples++;
            stats.averageCpuUsage += sample.cpuUsage;
            stats.averageMemoryUsage += sample.memoryUsage;
            stats.peakMemoryUsage = std::max(stats.peakMemoryUsage, sample.memoryUsage);
            stats.totalMemoryAllocated += sample.memoryAllocated;
        }
    }
    
    if (stats.totalSamples > 0) {
        stats.averageCpuUsage /= stats.totalSamples;
        stats.averageMemoryUsage /= stats.totalSamples;
        stats.cpuTimePercentage = (stats.averageCpuUsage / 100.0) * 100.0; // Simplified
        stats.memoryTimePercentage = (stats.averageMemoryUsage / stats.peakMemoryUsage) * 100.0;
    }
    
    stats.stepRangeStart = startStep;
    stats.stepRangeEnd = endStep;
    
    // Add function profiles
    for (const auto& [key, profileData] : activeFunctionProfiles) {
        size_t pos = key.find("|");
        if (pos != std::string::npos) {
            std::string functionName = key.substr(0, pos);
            std::string component = key.substr(pos + 1);
            
            FunctionProfile profile(functionName, component);
            profile.totalTime = profileData.accumulatedTime;
            profile.minTime = profileData.minTime;
            profile.maxTime = profileData.maxTime;
            profile.callCount = profileData.callCount;
            
            stats.functionProfiles[key] = profile;
        }
    }
    
    return stats;
}

std::vector<ProfileSample> Profiler::getRecentSamples(size_t count, 
                                                       uint64_t startStep, 
                                                       uint64_t endStep) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<ProfileSample> result;
    
    for (auto it = samples.rbegin(); it != samples.rend() && result.size() < count; ++it) {
        if (it->step >= startStep && it->step <= endStep) {
            result.push_back(*it);
        }
    }
    
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<FunctionProfile> Profiler::getFunctionProfiles(const std::string& component) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<FunctionProfile> result;
    
    for (const auto& [key, profile] : activeFunctionProfiles) {
        size_t pos = key.find("|");
        if (pos != std::string::npos) {
            std::string comp = key.substr(pos + 1);
            if (component.empty() || comp == component) {
                FunctionProfile funcProfile(key.substr(0, pos), comp);
                funcProfile.totalTime = profile.accumulatedTime;
                funcProfile.minTime = profile.minTime;
                funcProfile.maxTime = profile.maxTime;
                funcProfile.callCount = profile.callCount;
                result.push_back(funcProfile);
            }
        }
    }
    
    return result;
}

void Profiler::setSampleInterval(double intervalSeconds) {
    std::lock_guard<std::mutex> lock(mutex);
    sampleInterval = std::chrono::duration<double>(intervalSeconds);
}

double Profiler::getSampleInterval() const {
    std::lock_guard<std::mutex> lock(mutex);
    return sampleInterval.count();
}

void Profiler::setMaxSamplesPerStep(size_t maxSamples) {
    std::lock_guard<std::mutex> lock(mutex);
    maxSamplesPerStep = maxSamples;
}

size_t Profiler::getMaxSamplesPerStep() const {
    std::lock_guard<std::mutex> lock(mutex);
    return maxSamplesPerStep;
}

void Profiler::enableTarget(ProfileTarget target) {
    std::lock_guard<std::mutex> lock(mutex);
    if (std::find(enabledTargets.begin(), enabledTargets.end(), target) == enabledTargets.end()) {
        enabledTargets.push_back(target);
    }
}

void Profiler::disableTarget(ProfileTarget target) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove(enabledTargets.begin(), enabledTargets.end(), target);
    enabledTargets.erase(it, enabledTargets.end());
}

bool Profiler::isTargetEnabled(ProfileTarget target) const {
    std::lock_guard<std::mutex> lock(mutex);
    return std::find(enabledTargets.begin(), enabledTargets.end(), target) != enabledTargets.end();
}

void Profiler::enableComponent(const std::string& component) {
    std::lock_guard<std::mutex> lock(mutex);
    if (std::find(enabledComponents.begin(), enabledComponents.end(), component) == enabledComponents.end()) {
        enabledComponents.push_back(component);
    }
}

void Profiler::disableComponent(const std::string& component) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove(enabledComponents.begin(), enabledComponents.end(), component);
    enabledComponents.erase(it, enabledComponents.end());
}

bool Profiler::isComponentEnabled(const std::string& component) const {
    std::lock_guard<std::mutex> lock(mutex);
    return std::find(enabledComponents.begin(), enabledComponents.end(), component) != enabledComponents.end();
}

void Profiler::addProfileListener(const ProfileListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    profileListeners.push_back(listener);
}

void Profiler::removeProfileListener(const ProfileListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    profileListeners.erase(std::remove(profileListeners.begin(), profileListeners.end(), listener), profileListeners.end());
}

void Profiler::addFunctionProfileListener(const FunctionProfileListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    functionProfileListeners.push_back(listener);
}

void Profiler::removeFunctionProfileListener(const FunctionProfileListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    functionProfileListeners.erase(std::remove(functionProfileListeners.begin(), functionProfileListeners.end(), listener), functionProfileListeners.end());
}

void Profiler::addProfileStatsListener(const ProfileStatsListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    profileStatsListeners.push_back(listener);
}

void Profiler::removeProfileStatsListener(const ProfileStatsListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    profileStatsListeners.erase(std::remove(profileStatsListeners.begin(), profileStatsListeners.end(), listener), profileStatsListeners.end());
}

std::string Profiler::exportSamples(const std::string& format, 
                                     uint64_t startStep, 
                                     uint64_t endStep) const {
    if (format == "json") {
        std::vector<ProfileSample> filteredSamples;
        for (const auto& sample : samples) {
            if (sample.step >= startStep && sample.step <= endStep) {
                filteredSamples.push_back(sample);
            }
        }
        return exportToJSON(filteredSamples);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(samples);
    }
}

std::string Profiler::exportFunctionProfiles(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<FunctionProfile> profiles;
    for (const auto& [key, profileData] : activeFunctionProfiles) {
        size_t pos = key.find("|");
        if (pos != std::string::npos) {
            std::string functionName = key.substr(0, pos);
            std::string component = key.substr(pos + 1);
            
            FunctionProfile profile(functionName, component);
            profile.totalTime = profileData.accumulatedTime;
            profile.minTime = profileData.minTime;
            profile.maxTime = profileData.maxTime;
            profile.callCount = profileData.callCount;
            
            profiles.push_back(profile);
        }
    }
    
    if (format == "json") {
        return exportToJSON(profiles);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(profiles);
    }
}

std::string Profiler::exportProfileStats(const std::string& format) const {
    ProfileStats stats = getProfileStats();
    
    if (format == "json") {
        return exportToJSON(stats);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(stats);
    }
}

void Profiler::startBenchmark(const std::string& mode) {
    std::lock_guard<std::mutex> lock(mutex);
    benchmarking = true;
    currentBenchmarkMode = mode;
    
    // Start profiling for benchmark
    startProfiling({ProfileTarget::CPU, ProfileTarget::Memory, ProfileTarget::Simulation});
}

void Profiler::stopBenchmark() {
    std::lock_guard<std::mutex> lock(mutex);
    
    benchmarking = false;
    currentBenchmarkMode = "";
    
    // Stop profiling
    stopProfiling();
}

bool Profiler::isBenchmarking() const {
    std::lock_guard<std::mutex> lock(mutex);
    return benchmarking;
}

std::string Profiler::getCurrentBenchmarkMode() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentBenchmarkMode;
}

double Profiler::getCurrentCpuUsage() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentCpuUsage;
}

double Profiler::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentMemoryUsage;
}

uint64_t Profiler::getCurrentMemoryAllocated() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentMemoryAllocated;
}

uint64_t Profiler::getCurrentMemoryPeak() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentMemoryPeak;
}

void Profiler::clearSamples() {
    std::lock_guard<std::mutex> lock(mutex);
    samples.clear();
}

void Profiler::clearFunctionProfiles() {
    std::lock_guard<std::mutex> lock(mutex);
    activeFunctionProfiles.clear();
}

bool Profiler::saveConfiguration(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string filename = filepath.empty() ? "profiler_config.json" : filepath;
    
    try {
        nlohmann::json jsonData;
        
        // Export configuration
        jsonData["sampleInterval"] = sampleInterval.count();
        jsonData["maxSamplesPerStep"] = maxSamplesPerStep;
        
        // Export enabled targets
        std::vector<int> targetsJson;
        for (ProfileTarget target : enabledTargets) {
            targetsJson.push_back(static_cast<int>(target));
        }
        jsonData["enabledTargets"] = targetsJson;
        
        // Export enabled components
        jsonData["enabledComponents"] = enabledComponents;
        
        // Export samples (limited for file size)
        if (samples.size() > 1000) {
            std::vector<ProfileSample> recentSamples(samples.end() - 1000, samples.end());
            jsonData["samples"] = exportToJSON(recentSamples);
        } else {
            jsonData["samples"] = exportToJSON(samples);
        }
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << jsonData.dump(2);
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving profiler configuration: " << e.what() << std::endl;
    }
    
    return false;
}

bool Profiler::loadConfiguration(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string filename = filepath.empty() ? "profiler_config.json" : filepath;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // Load configuration
        sampleInterval = std::chrono::duration<double>(jsonData.value("sampleInterval", 1.0));
        maxSamplesPerStep = jsonData.value("maxSamplesPerStep", 10);
        
        // Load enabled targets
        enabledTargets.clear();
        if (jsonData.contains("enabledTargets")) {
            for (int target : jsonData["enabledTargets"]) {
                enabledTargets.push_back(static_cast<ProfileTarget>(target));
            }
        }
        
        // Load enabled components
        enabledComponents = jsonData.value("enabledComponents", std::vector<std::string>());
        
        // Clear existing samples
        samples.clear();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading profiler configuration: " << e.what() << std::endl;
    }
    
    return false;
}

void Profiler::cleanupOldSamples() {
    // Remove old samples to maintain memory usage
    while (samples.size() > maxSamplesPerStep * 1000) {
        samples.erase(samples.begin());
    }
}

void Profiler::updateFunctionProfiles() {
    // Update function profiles from active data
    for (auto& [key, profileData] : activeFunctionProfiles) {
        // Function profiles are updated in real-time
    }
}

void Profiler::updatePerformanceMetrics() {
    // Simulate performance metrics update
    // In a real implementation, this would collect actual system metrics
    currentCpuUsage = 50.0 + (rand() % 50); // Simulated: 50-100%
    currentMemoryUsage = 100.0 + (rand() % 900); // Simulated: 100-1000 MB
    currentMemoryAllocated = static_cast<uint64_t>(currentMemoryUsage * 1024 * 1024); // Convert to bytes
    currentMemoryPeak = std::max(currentMemoryPeak, currentMemoryAllocated);
}

void Profiler::samplingThread() {
    while (samplingThreadRunning) {
        try {
            std::this_thread::sleep_for(sampleInterval);
            
            // Take a sample
            updatePerformanceMetrics();
            takeSample(0, "system"); // step 0 for now
        } catch (const std::exception& e) {
            std::cerr << "Error in sampling thread: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void Profiler::loadDefaultConfiguration() {
    enabledTargets = {ProfileTarget::CPU, ProfileTarget::Memory};
    enabledComponents = {"simulation", "brain", "memory"};
    sampleInterval = std::chrono::duration<double>(1.0);
    maxSamplesPerStep = 10;
}

void Profiler::saveDefaultConfiguration() const {
    // Save default configuration to file
    nlohmann::json jsonData;
    jsonData["sampleInterval"] = sampleInterval.count();
    jsonData["maxSamplesPerStep"] = maxSamplesPerStep;
    
    std::vector<int> targetsJson;
    for (ProfileTarget target : enabledTargets) {
        targetsJson.push_back(static_cast<int>(target));
    }
    jsonData["enabledTargets"] = targetsJson;
    
    jsonData["enabledComponents"] = enabledComponents;
    
    std::ofstream file("profiler_default_config.json");
    if (file.is_open()) {
        file << jsonData.dump(2);
    }
}

std::string Profiler::exportToJSON(const std::vector<ProfileSample>& samples) const {
    nlohmann::json jsonData;
    
    for (const auto& sample : samples) {
        nlohmann::json sampleJson;
        sampleJson["step"] = sample.step;
        sampleJson["timestamp"] = sample.timestamp.time_since_epoch().count();
        sampleJson["cpuUsage"] = sample.cpuUsage;
        sampleJson["memoryUsage"] = sample.memoryUsage;
        sampleJson["gpuUsage"] = sample.gpuUsage;
        sampleJson["memoryAllocated"] = sample.memoryAllocated;
        sampleJson["memoryPeak"] = sample.memoryPeak;
        sampleJson["threadCount"] = sample.threadCount;
        sampleJson["networkBandwidth"] = sample.networkBandwidth;
        sampleJson["component"] = sample.component;
        sampleJson["customMetrics"] = sample.customMetrics;
        jsonData.push_back(sampleJson);
    }
    
    return jsonData.dump(2);
}

std::string Profiler::exportToJSON(const std::vector<FunctionProfile>& profiles) const {
    nlohmann::json jsonData;
    
    for (const auto& profile : profiles) {
        nlohmann::json profileJson;
        profileJson["functionName"] = profile.functionName;
        profileJson["component"] = profile.component;
        profileJson["totalTime"] = profile.totalTime.count();
        profileJson["minTime"] = profile.minTime.count();
        profileJson["maxTime"] = profile.maxTime.count();
        profileJson["callCount"] = profile.callCount;
        profileJson["averageTime"] = profile.averageTime();
        profileJson["minTimePercent"] = profile.minTimePercent();
        profileJson["maxTimePercent"] = profile.maxTimePercent();
        jsonData.push_back(profileJson);
    }
    
    return jsonData.dump(2);
}

std::string Profiler::exportToJSON(const ProfileStats& stats) const {
    nlohmann::json jsonData;
    
    jsonData["totalSamples"] = stats.totalSamples;
    jsonData["averageCpuUsage"] = stats.averageCpuUsage;
    jsonData["averageMemoryUsage"] = stats.averageMemoryUsage;
    jsonData["peakMemoryUsage"] = stats.peakMemoryUsage;
    jsonData["totalMemoryAllocated"] = stats.totalMemoryAllocated;
    jsonData["cpuTimePercentage"] = stats.cpuTimePercentage;
    jsonData["memoryTimePercentage"] = stats.memoryTimePercentage;
    jsonData["totalProfileDuration"] = stats.totalProfileDuration.count();
    jsonData["stepRangeStart"] = stats.stepRangeStart;
    jsonData["stepRangeEnd"] = stats.stepRangeEnd;
    
    // Export function profiles
    nlohmann::json functionsJson;
    for (const auto& [key, profile] : stats.functionProfiles) {
        nlohmann::json funcJson;
        funcJson["functionName"] = profile.functionName;
        funcJson["component"] = profile.component;
        funcJson["totalTime"] = profile.totalTime.count();
        funcJson["minTime"] = profile.minTime.count();
        funcJson["maxTime"] = profile.maxTime.count();
        funcJson["callCount"] = profile.callCount;
        funcJson["averageTime"] = profile.averageTime();
        funcJson["minTimePercent"] = profile.minTimePercent();
        funcJson["maxTimePercent"] = profile.maxTimePercent();
        functionsJson[key] = funcJson;
    }
    jsonData["functionProfiles"] = functionsJson;
    
    return jsonData.dump(2);
}

} // namespace nlm
