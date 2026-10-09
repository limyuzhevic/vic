#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <filesystem>

namespace nlm {

struct PerformanceProfiler::Impl {
    std::unordered_map<std::string, ProfileResult> profiles;
    std::unordered_map<std::string, BenchmarkResult> benchmarks;
    std::string currentProfile;
};

void PerformanceProfiler::startBrainProfiling(const std::shared_ptr<Brain>& brain, const std::string& profileName) {
    if (!brain) {
        NLM_LOG_ERROR("PerformanceProfiler::startBrainProfiling: Cannot profile null brain");
        return;
    }
    
    pImpl->currentProfile = profileName;
    
    if (pImpl->profiles.find(profileName) == pImpl->profiles.end()) {
        ProfileResult profile;
        profile.profileName = profileName;
        profile.totalTime = 0.0;
        profile.cpuUsage = 0.0;
        profile.memoryUsage = 0.0;
        profile.neuronActivity = 0.0;
        profile.networkEfficiency = 0.0;
        profile.learningProgress = 0.0;
        pImpl->profiles[profileName] = profile;
    }
    
    // Record initial measurements
    Measurement initialMeasurement("initialization", 0.0, 0, 0, 0.0, 0.0, 0.0, 0.0);
    recordMeasurement(profileName, initialMeasurement);
    
    NLM_LOG_INFO("Started performance profiling for brain: " + profileName);
}

ProfileResult PerformanceProfiler::stopBrainProfiling(const std::shared_ptr<Brain>& brain) {
    if (!brain || pImpl->currentProfile.empty()) {
        return ProfileResult();
    }
    
    std::string profileName = pImpl->currentProfile;
    pImpl->currentProfile.clear();
    
    ProfileResult& profile = pImpl->profiles[profileName];
    
    // Calculate final metrics
    if (!profile.measurements.empty()) {
        double totalDuration = 0.0;
        size_t maxMemory = 0;
        size_t totalNeurons = 0;
        double totalSpikes = 0.0;
        double totalLearning = 0.0;
        double totalPlasticity = 0.0;
        
        for (const auto& measurement : profile.measurements) {
            totalDuration += measurement.duration;
            maxMemory = std::max(maxMemory, measurement.memoryUsage);
            totalNeurons += measurement.neuronCount;
            totalSpikes += measurement.spikeCount;
            totalLearning += measurement.learningRate;
            totalPlasticity += measurement.plasticityLevel;
        }
        
        profile.totalTime = totalDuration;
        profile.memoryUsage = static_cast<double>(maxMemory) / (1024 * 1024); // Convert to MB
        profile.neuronActivity = totalNeurons > 0 ? totalSpikes / totalNeurons : 0.0;
        profile.learningProgress = totalLearning / profile.measurements.size();
        profile.networkEfficiency = totalNeurons > 0 ? totalSpikes / totalDuration : 0.0;
        
        // Calculate CPU usage (simplified approximation)
        profile.cpuUsage = std::min(100.0, profile.neuronActivity * 100.0);
    }
    
    NLM_LOG_INFO("Stopped performance profiling for brain: " + profileName);
    NLM_LOG_INFO("Profile results: " + profile.toString());
    
    return profile;
}

void PerformanceProfiler::startBenchmark(const std::string& benchmarkName, const std::shared_ptr<Brain>& brain) {
    if (pImpl->benchmarks.find(benchmarkName) == pImpl->benchmarks.end()) {
        BenchmarkResult benchmark;
        benchmark.benchmarkName = benchmarkName;
        benchmark.totalTime = 0.0;
        benchmark.avgTimePerStep = 0.0;
        benchmark.minTime = std::numeric_limits<double>::max();
        benchmark.maxTime = 0.0;
        benchmark.stdDev = 0.0;
        benchmark.totalNeurons = 0;
        benchmark.totalSynapses = 0;
        benchmark.totalSpikes = 0.0;
        benchmark.finalLearningRate = 0.0;
        benchmark.finalPlasticity = 0.0;
        benchmark.performanceScore = 0.0;
        benchmark.efficiencyScore = 0.0;
        benchmark.scalabilityScore = 0.0;
        pImpl->benchmarks[benchmarkName] = benchmark;
    }
    
    NLM_LOG_INFO("Started benchmark: " + benchmarkName);
}

BenchmarkResult PerformanceProfiler::stopBenchmark(const std::string& benchmarkName, const std::shared_ptr<Brain>& brain) {
    if (pImpl->benchmarks.find(benchmarkName) == pImpl->benchmarks.end()) {
        return BenchmarkResult();
    }
    
    BenchmarkResult& benchmark = pImpl->benchmarks[benchmarkName];
    
    // Calculate statistics
    if (!benchmark.measurements.empty()) {
        std::vector<double> durations;
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        double totalSpikes = 0.0;
        
        for (const auto& measurement : benchmark.measurements) {
            durations.push_back(measurement.duration);
            totalNeurons += measurement.neuronCount;
            totalSynapses += measurement.neuronCount * 10; // Approximate synapses per neuron
            totalSpikes += measurement.spikeCount;
        }
        
        benchmark.totalTime = std::accumulate(durations.begin(), durations.end(), 0.0);
        benchmark.avgTimePerStep = durations.size() > 0 ? benchmark.totalTime / durations.size() : 0.0;
        
        if (!durations.empty()) {
            benchmark.minTime = *std::min_element(durations.begin(), durations.end());
            benchmark.maxTime = *std::max_element(durations.begin(), durations.end());
            benchmark.stdDev = calculateStandardDeviation(durations);
        }
        
        benchmark.totalNeurons = totalNeurons / benchmark.measurements.size();
        benchmark.totalSynapses = totalSynapses / benchmark.measurements.size();
        benchmark.totalSpikes = totalSpikes;
        
        // Calculate scores
        benchmark.performanceScore = calculatePerformanceScore(benchmark.measurements.back());
        benchmark.efficiencyScore = calculateEfficiencyScore(benchmark.measurements.back());
        benchmark.scalabilityScore = calculateScalabilityScore(benchmark.measurements.back());
    }
    
    NLM_LOG_INFO("Stopped benchmark: " + benchmarkName);
    
    return benchmark;
}

void PerformanceProfiler::recordMeasurement(const std::string& profileName, const Measurement& measurement) {
    if (pImpl->profiles.find(profileName) != pImpl->profiles.end()) {
        pImpl->profiles[profileName].measurements.push_back(measurement);
    }
}

ProfileResult PerformanceProfiler::getCurrentProfile() const {
    if (pImpl->currentProfile.empty() || pImpl->profiles.find(pImpl->currentProfile) == pImpl->profiles.end()) {
        return ProfileResult();
    }
    return pImpl->profiles.at(pImpl->currentProfile);
}

BenchmarkResult PerformanceProfiler::getBenchmark(const std::string& benchmarkName) const {
    if (pImpl->benchmarks.find(benchmarkName) == pImpl->benchmarks.end()) {
        return BenchmarkResult();
    }
    return pImpl->benchmarks.at(benchmarkName);
}

std::vector<ProfileResult> PerformanceProfiler::getAllProfiles() const {
    std::vector<ProfileResult> profiles;
    for (const auto& pair : pImpl->profiles) {
        profiles.push_back(pair.second);
    }
    return profiles;
}

std::vector<BenchmarkResult> PerformanceProfiler::getAllBenchmarks() const {
    std::vector<BenchmarkResult> benchmarks;
    for (const auto& pair : pImpl->benchmarks) {
        benchmarks.push_back(pair.second);
    }
    return benchmarks;
}

void PerformanceProfiler::clear() {
    pImpl->profiles.clear();
    pImpl->benchmarks.clear();
    pImpl->currentProfile.clear();
    
    NLM_LOG_INFO("Cleared all performance profiling data");
}

bool PerformanceProfiler::saveProfileToFile(const std::string& filename, const std::string& profileName) const {
    if (pImpl->profiles.find(profileName) == pImpl->profiles.end()) {
        NLM_LOG_ERROR("PerformanceProfiler::saveProfileToFile: Profile not found: " + profileName);
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    const ProfileResult& profile = pImpl->profiles.at(profileName);
    
    file << "# NLM Performance Profile File\n";
    file << "# Profile: " << profile.profileName << "\n";
    file << "# Generated on " << __TIMESTAMP__ << "\n\n";
    
    file << "ProfileName = \"" << profile.profileName << "\"\n";
    file << "TotalTime = " << profile.totalTime << "\n";
    file << "CPUUsage = " << profile.cpuUsage << "\n";
    file << "MemoryUsage = " << profile.memoryUsage << "\n";
    file << "NeuronActivity = " << profile.neuronActivity << "\n";
    file << "NetworkEfficiency = " << profile.networkEfficiency << "\n";
    file << "LearningProgress = " << profile.learningProgress << "\n\n";
    
    file << "Measurements:\n";
    for (const auto& measurement : profile.measurements) {
        file << "  - Operation = \"" << measurement.operation << "\"\n";
        file << "    Duration = " << measurement.duration << "\n";
        file << "    MemoryUsage = " << measurement.memoryUsage << "\n";
        file << "    NeuronCount = " << measurement.neuronCount << "\n";
        file << "    SpikeCount = " << measurement.spikeCount << "\n";
        file << "    LearningRate = " << measurement.learningRate << "\n";
        file << "    PlasticityLevel = " << measurement.plasticityLevel << "\n";
        file << "    Temperature = " << measurement.temperature << "\n";
        file << "\n";
    }
    
    file.close();
    return true;
}

bool PerformanceProfiler::loadProfileFromFile(const std::string& filename, std::string& profileName) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentKey;
    ProfileResult profile;
    Measurement currentMeasurement("", 0.0);
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            currentKey = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Remove quotes
            if (value.size() >= 2 && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on key
            if (currentKey == "ProfileName") {
                profileName = value;
                profile.profileName = value;
            } else if (currentKey == "TotalTime") {
                profile.totalTime = std::stod(value);
            } else if (currentKey == "CPUUsage") {
                profile.cpuUsage = std::stod(value);
            } else if (currentKey == "MemoryUsage") {
                profile.memoryUsage = std::stod(value);
            } else if (currentKey == "NeuronActivity") {
                profile.neuronActivity = std::stod(value);
            } else if (currentKey == "NetworkEfficiency") {
                profile.networkEfficiency = std::stod(value);
            } else if (currentKey == "LearningProgress") {
                profile.learningProgress = std::stod(value);
            }
        } else if (line.find("- Operation =") != std::string::npos) {
            // Start of a new measurement
            if (!currentMeasurement.operation.empty()) {
                profile.measurements.push_back(currentMeasurement);
            }
            
            currentMeasurement = Measurement("", 0.0);
            size_t quotesStart = line.find("\"");
            if (quotesStart != std::string::npos) {
                size_t quotesEnd = line.find("\"", quotesStart + 1);
                if (quotesEnd != std::string::npos) {
                    currentMeasurement.operation = line.substr(quotesStart + 1, quotesEnd - quotesStart - 1);
                }
            }
        }
    }
    
    // Add the last measurement
    if (!currentMeasurement.operation.empty()) {
        profile.measurements.push_back(currentMeasurement);
    }
    
    if (!profileName.empty()) {
        pImpl->profiles[profileName] = profile;
    }
    
    return true;
}

bool PerformanceProfiler::saveAllProfilesToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Performance Profiles File\n";
    file << "# Generated on " << __TIMESTAMP__ << "\n\n";
    
    file << "ProfileCount = " << pImpl->profiles.size() << "\n\n";
    
    for (const auto& pair : pImpl->profiles) {
        file << "--- Profile Start ---\n";
        
        const ProfileResult& profile = pair.second;
        
        file << "ProfileName = \"" << profile.profileName << "\"\n";
        file << "TotalTime = " << profile.totalTime << "\n";
        file << "CPUUsage = " << profile.cpuUsage << "\n";
        file << "MemoryUsage = " << profile.memoryUsage << "\n";
        file << "NeuronActivity = " << profile.neuronActivity << "\n";
        file << "NetworkEfficiency = " << profile.networkEfficiency << "\n";
        file << "LearningProgress = " << profile.learningProgress << "\n\n";
        
        file << "Measurements:\n";
        for (const auto& measurement : profile.measurements) {
            file << "  - Operation = \"" << measurement.operation << "\"\n";
            file << "    Duration = " << measurement.duration << "\n";
            file << "    MemoryUsage = " << measurement.memoryUsage << "\n";
            file << "    NeuronCount = " << measurement.neuronCount << "\n";
            file << "    SpikeCount = " << measurement.spikeCount << "\n";
            file << "    LearningRate = " << measurement.learningRate << "\n";
            file << "    PlasticityLevel = " << measurement.plasticityLevel << "\n";
            file << "    Temperature = " << measurement.temperature << "\n";
            file << "\n";
        }
        
        file << "--- Profile End ---\n\n";
    }
    
    file.close();
    return true;
}

bool PerformanceProfiler::loadAllProfilesFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->profiles.clear();
    
    std::string line;
    std::string currentProfile;
    ProfileResult currentProfileData;
    Measurement currentMeasurement("", 0.0);
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        if (line == "--- Profile Start ---") {
            // Save previous profile
            if (!currentProfile.empty()) {
                pImpl->profiles[currentProfile] = currentProfileData;
                currentProfileData.clear();
                currentMeasurement = Measurement("", 0.0);
            }
            continue;
        }
        
        if (line == "--- Profile End ---") {
            currentProfile.clear();
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Remove quotes
            if (value.size() >= 2 && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on key
            if (key == "ProfileName") {
                currentProfile = value;
                currentProfileData.profileName = value;
            } else if (key == "TotalTime") {
                currentProfileData.totalTime = std::stod(value);
            } else if (key == "CPUUsage") {
                currentProfileData.cpuUsage = std::stod(value);
            } else if (key == "MemoryUsage") {
                currentProfileData.memoryUsage = std::stod(value);
            } else if (key == "NeuronActivity") {
                currentProfileData.neuronActivity = std::stod(value);
            } else if (key == "NetworkEfficiency") {
                currentProfileData.networkEfficiency = std::stod(value);
            } else if (key == "LearningProgress") {
                currentProfileData.learningProgress = std::stod(value);
            }
        } else if (line.find("- Operation =") != std::string::npos) {
            // Start of a new measurement
            if (!currentMeasurement.operation.empty()) {
                currentProfileData.measurements.push_back(currentMeasurement);
            }
            
            currentMeasurement = Measurement("", 0.0);
            size_t quotesStart = line.find("\"");
            if (quotesStart != std::string::npos) {
                size_t quotesEnd = line.find("\"", quotesStart + 1);
                if (quotesEnd != std::string::npos) {
                    currentMeasurement.operation = line.substr(quotesStart + 1, quotesEnd - quotesStart - 1);
                }
            }
        }
    }
    
    // Save the last profile
    if (!currentProfile.empty() && !currentProfileData.measurements.empty()) {
        pImpl->profiles[currentProfile] = currentProfileData;
    }
    
    return true;
}

std::string PerformanceProfiler::getPerformanceStatistics() const {
    std::ostringstream oss;
    
    oss << "=== NLM Performance Statistics ===\n";
    oss << "Profiles: " << pImpl->profiles.size() << "\n";
    oss << "Benchmarks: " << pImpl->benchmarks.size() << "\n\n";
    
    if (!pImpl->profiles.empty()) {
        oss << "Profile Summary:\n";
        for (const auto& pair : pImpl->profiles) {
            const ProfileResult& profile = pair.second;
            oss << "  " << profile.profileName << ":\n";
            oss << "    Total Time: " << std::fixed << std::setprecision(2) << profile.totalTime << " ms\n";
            oss << "    CPU Usage: " << std::fixed << std::setprecision(1) << profile.cpuUsage << "%\n";
            oss << "    Memory Usage: " << std::fixed << std::setprecision(2) << profile.memoryUsage << " MB\n";
            oss << "    Neuron Activity: " << std::fixed << std::setprecision(3) << profile.neuronActivity << "\n";
            oss << "    Network Efficiency: " << std::fixed << std::setprecision(3) << profile.networkEfficiency << "\n";
            oss << "    Learning Progress: " << std::fixed << std::setprecision(3) << profile.learningProgress << "\n\n";
        }
    }
    
    if (!pImpl->benchmarks.empty()) {
        oss << "Benchmark Summary:\n";
        for (const auto& pair : pImpl->benchmarks) {
            const BenchmarkResult& benchmark = pair.second;
            oss << "  " << benchmark.benchmarkName << ":\n";
            oss << "    Total Time: " << std::fixed << std::setprecision(2) << benchmark.totalTime << " ms\n";
            oss << "    Avg Time Per Step: " << std::fixed << std::setprecision(2) << benchmark.avgTimePerStep << " ms\n";
            oss << "    Min Time: " << std::fixed << std::setprecision(2) << benchmark.minTime << " ms\n";
            oss << "    Max Time: " << std::fixed << std::setprecision(2) << benchmark.maxTime << " ms\n";
            oss << "    Performance Score: " << std::fixed << std::setprecision(3) << benchmark.performanceScore << "\n";
            oss << "    Efficiency Score: " << std::fixed << std::setprecision(3) << benchmark.efficiencyScore << "\n";
            oss << "    Scalability Score: " << std::fixed << std::setprecision(3) << benchmark.scalabilityScore << "\n\n";
        }
    }
    
    return oss.str();
}

std::string PerformanceProfiler::compareProfiles(const std::vector<std::string>& profileNames) const {
    std::ostringstream oss;
    
    oss << "=== Profile Comparison ===\n\n";
    
    std::vector<const ProfileResult*> profiles;
    for (const auto& name : profileNames) {
        auto it = pImpl->profiles.find(name);
        if (it != pImpl->profiles.end()) {
            profiles.push_back(&it->second);
        }
    }
    
    if (profiles.size() < 2) {
        oss << "Need at least 2 profiles for comparison.\n";
        return oss.str();
    }
    
    // Compare key metrics
    oss << "Metric Comparison:\n";
    oss << "  Profile Name" << std::string(20, ' ') << "Time (ms)" << std::string(12, ' ') << "CPU (%) ";
    oss << "Memory (MB)" << std::string(12, ' ') << "Activity" << std::string(12, ' ') << "Efficiency";
    oss << std::string(12, ' ') << "Learning\n";
    
    for (const auto* profile : profiles) {
        oss << "  " << std::left << std::setw(20) << profile->profileName;
        oss << std::fixed << std::setprecision(2) << profile->totalTime;
        oss << std::string(12 - std::to_string(static_cast<int>(profile->totalTime)).length(), ' ') << " ";
        oss << std::fixed << std::setprecision(1) << profile->cpuUsage;
        oss << std::string(11 - std::to_string(static_cast<int>(profile->cpuUsage)).length(), ' ') << " ";
        oss << std::fixed << std::setprecision(2) << profile->memoryUsage;
        oss << std::string(12 - std::to_string(static_cast<int>(profile->memoryUsage)).length(), ' ') << " ";
        oss << std::fixed << std::setprecision(3) << profile->neuronActivity;
        oss << std::string(12 - std::to_string(static_cast<int>(profile->neuronActivity * 1000)).length(), ' ') << " ";
        oss << std::fixed << std::setprecision(3) << profile->networkEfficiency;
        oss << std::string(12 - std::to_string(static_cast<int>(profile->networkEfficiency * 1000)).length(), ' ') << " ";
        oss << std::fixed << std::setprecision(3) << profile->learningProgress;
        oss << "\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::getOptimizationRecommendations() const {
    std::ostringstream oss;
    
    oss << "=== Performance Optimization Recommendations ===\n\n";
    
    // Analyze profiles for optimization opportunities
    for (const auto& pair : pImpl->profiles) {
        const ProfileResult& profile = pair.second;
        
        // Check for long execution times
        if (profile.totalTime > 1000.0) { // > 1 second
            oss << "Profile: " << profile.profileName << "\n";
            oss << "  Recommendation: Execution time is high (" << std::fixed << std::setprecision(2) << profile.totalTime << " ms)\n";
            oss << "    Consider optimizing neural network configuration or reducing simulation steps.\n\n";
        }
        
        // Check for low efficiency
        if (profile.networkEfficiency < 0.1) { // < 0.1 spikes/ms
            oss << "Profile: " << profile.profileName << "\n";
            oss << "  Recommendation: Network efficiency is low (" << std::fixed << std::setprecision(3) << profile.networkEfficiency << " spikes/ms)\n";
            oss << "    Consider adjusting neuron parameters or connectivity patterns.\n\n";
        }
        
        // Check for high memory usage
        if (profile.memoryUsage > 100.0) { // > 100 MB
            oss << "Profile: " << profile.profileName << "\n";
            oss << "  Recommendation: Memory usage is high (" << std::fixed << std::setprecision(2) << profile.memoryUsage << " MB)\n";
            oss << "    Consider optimizing memory pool sizes or enabling compression.\n\n";
        }
        
        // Check for low learning progress
        if (profile.learningProgress < 0.1) { // < 0.1
            oss << "Profile: " << profile.profileName << "\n";
            oss << "  Recommendation: Learning progress is low (" << std::fixed << std::setprecision(3) << profile.learningProgress << ")\n";
            oss << "    Consider adjusting learning rates or plasticity parameters.\n\n";
        }
    }
    
    return oss.str();
}

void PerformanceProfiler::resetProfile(const std::string& profileName) {
    if (pImpl->profiles.find(profileName) != pImpl->profiles.end()) {
        pImpl->profiles.erase(profileName);
        NLM_LOG_INFO("Reset profile: " + profileName);
    }
}

std::string PerformanceProfiler::exportProfileData(const std::string& profileName) const {
    if (pImpl->profiles.find(profileName) == pImpl->profiles.end()) {
        return ""; // Return empty string if profile not found
    }
    
    const ProfileResult& profile = pImpl->profiles.at(profileName);
    
    // Simple CSV export
    std::ostringstream oss;
    oss << "Profile Name,Total Time (ms),CPU Usage (%),Memory Usage (MB),Neuron Activity,Network Efficiency,Learning Progress\n";
    
    for (size_t i = 0; i < profile.measurements.size(); ++i) {
        oss << profile.profileName << ",";
        oss << std::fixed << std::setprecision(2) << profile.totalTime << ",";
        oss << std::fixed << std::setprecision(1) << profile.cpuUsage << ",";
        oss << std::fixed << std::setprecision(2) << profile.memoryUsage << ",";
        oss << std::fixed << std::setprecision(3) << profile.neuronActivity << ",";
        oss << std::fixed << std::setprecision(3) << profile.networkEfficiency << ",";
        oss << std::fixed << std::setprecision(3) << profile.learningProgress;
        oss << "\n";
    }
    
    return oss.str();
}

// Static helper implementations
double PerformanceProfiler::calculateStandardDeviation(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    double mean = calculateMean(values);
    double sum = 0.0;
    
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / values.size());
}

double PerformanceProfiler::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceProfiler::calculateEfficiencyScore(const Measurement& measurement) {
    return measurement.spikeCount / std::max(measurement.duration, 1.0);
}

double PerformanceProfiler::calculateScalabilityScore(const Measurement& measurement) {
    return measurement.neuronCount / std::max(measurement.duration, 1.0);
}

double PerformanceProfiler::calculatePerformanceScore(const Measurement& measurement) {
    double efficiency = calculateEfficiencyScore(measurement);
    double scalability = calculateScalabilityScore(measurement);
    return (efficiency + scalability) / 2.0;
}

// NLMBenchmarker implementations
BenchmarkResult NLMBenchmarker::benchmarkBrainInitialization(const AdvancedConfig& config) {
    BenchmarkResult result;
    result.benchmarkName = "brain_initialization";
    result.totalTime = 0.0;
    result.avgTimePerStep = 0.0;
    result.minTime = 0.0;
    result.maxTime = 0.0;
    result.stdDev = 0.0;
    result.totalNeurons = config.getNeuralNetworkConfig().maxNeuronCount;
    result.totalSynapses = config.getNeuralNetworkConfig().maxSynapseCount;
    result.totalSpikes = 0.0;
    result.finalLearningRate = 0.0;
    result.finalPlasticity = 0.0;
    result.performanceScore = 0.0;
    result.efficiencyScore = 0.0;
    result.scalabilityScore = 0.0;
    
    NLM_LOG_INFO("Benchmark: Brain initialization completed");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkStepExecution(const std::shared_ptr<Brain>& brain, 
                                                         size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name;
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkStepExecution: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        Measurement measurement("step_execution", timer.elapsedMs(), 0, 0, 
                               brain->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkLearningPerformance(const std::shared_ptr<Brain>& brain,
                                                                size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name + "_learning_performance";
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkLearningPerformance: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        // Simulate learning by applying some reward modulation
        float reward = (i % 10 == 0) ? 1.0f : 0.0f;
        brain->getSTDP()->addToWeight(reward * 0.01f);
        
        Measurement measurement("learning_performance", timer.elapsedMs(), 0, 0,
                               brain->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " learning performance completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkMemoryUsage(const std::shared_ptr<Brain>& brain,
                                                      size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name + "_memory_usage";
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkMemoryUsage: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        // Get memory usage (simplified approximation)
        size_t memoryUsage = brain->getTotalNeuronCount() * 100 + // Approximate neuron memory
                          brain->getTotalSynapseCount() * 50;   // Approximate synapse memory
        
        Measurement measurement("memory_usage", timer.elapsedMs(), memoryUsage, 0,
                               brain->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " memory usage completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkPlasticity(const std::shared_ptr<Brain>& brain,
                                                     size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name + "_plasticity";
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkPlasticity: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        // Get plasticity level (simplified approximation)
        double plasticity = 1.0; // Base plasticity
        if (i % 20 == 0) plasticity = 0.5; // Reduced plasticity
        if (i % 50 == 0) plasticity = 1.5; // Enhanced plasticity
        
        Measurement measurement("plasticity", timer.elapsedMs(), 0, 0,
                               brain->getTotalSpikeCount(), 0.0, plasticity, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " plasticity completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkSensoryProcessing(const std::shared_ptr<Brain>& brain,
                                                            size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name + "_sensory_processing";
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkSensoryProcessing: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        // Simulate sensory processing by injecting current into sensory neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        
        Measurement measurement("sensory_processing", timer.elapsedMs(), 0, 0,
                               brain->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " sensory processing completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkMotorOutput(const std::shared_ptr<Brain>& brain,
                                                      size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name + "_motor_output";
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkMotorOutput: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        // Get motor output (simplified approximation)
        double motorOutput = brain->getTotalSpikeCount() * 0.01; // Approximate motor output
        
        Measurement measurement("motor_output", timer.elapsedMs(), 0, 0,
                               brain->getTotalSpikeCount(), motorOutput, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " motor output completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

BenchmarkResult NLMBenchmarker::benchmarkCompleteSimulation(const std::shared_ptr<Brain>& brain,
                                                             size_t steps, const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name + "_complete_simulation";
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::benchmarkCompleteSimulation: Cannot benchmark null brain");
        return result;
    }
    
    PerformanceProfiler profiler;
    profiler.startBenchmark(name, brain);
    
    for (size_t i = 0; i < steps; ++i) {
        Timer timer;
        brain->step(i);
        
        // Complete simulation including all components
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain->getSTDP()->addToWeight(0.01f);
        
        size_t memoryUsage = brain->getTotalNeuronCount() * 100 + brain->getTotalSynapseCount() * 50;
        
        Measurement measurement("complete_simulation", timer.elapsedMs(), memoryUsage, 0,
                               brain->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    result = profiler.stopBenchmark(name, brain);
    
    NLM_LOG_INFO("Benchmark: " + name + " complete simulation completed in " + std::to_string(result.totalTime) + " ms");
    
    return result;
}

std::vector<BenchmarkResult> NLMBenchmarker::runAllBenchmarks(const std::shared_ptr<Brain>& brain, size_t steps) {
    std::vector<BenchmarkResult> results;
    
    if (!brain) {
        NLM_LOG_ERROR("NLMBenchmarker::runAllBenchmarks: Cannot run benchmarks with null brain");
        return results;
    }
    
    AdvancedConfig config = createDefaultAdvancedConfig();
    
    results.push_back(benchmarkBrainInitialization(config));
    results.push_back(benchmarkStepExecution(brain, steps, "step_execution"));
    results.push_back(benchmarkLearningPerformance(brain, steps, "learning_performance"));
    results.push_back(benchmarkMemoryUsage(brain, steps, "memory_usage"));
    results.push_back(benchmarkPlasticity(brain, steps, "plasticity"));
    results.push_back(benchmarkSensoryProcessing(brain, steps, "sensory_processing"));
    results.push_back(benchmarkMotorOutput(brain, steps, "motor_output"));
    results.push_back(benchmarkCompleteSimulation(brain, steps, "complete_simulation"));
    
    return results;
}

std::string NLMBenchmarker::compareBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::ostringstream oss;
    
    oss << "=== Benchmark Comparison ===\n\n";
    
    if (results.empty()) {
        oss << "No benchmarks to compare.\n";
        return oss.str();
    }
    
    // Create comparison table
    oss << std::left << std::setw(25) << "Benchmark";
    oss << std::fixed << std::setprecision(2);
    oss << std::setw(12) << "Total Time (ms)";
    oss << std::setw(15) << "Avg Time (ms)";
    oss << std::setw(15) << "Min Time (ms)";
    oss << std::setw(15) << "Max Time (ms)";
    oss << std::setw(15) << "Efficiency Score";
    oss << std::setw(15) << "Performance Score";
    oss << std::setw(15) << "Scalability Score\n";
    
    oss << std::string(25 + 12 + 15 + 15 + 15 + 15 + 15 + 15, '-') << "\n";
    
    for (const auto& benchmark : results) {
        oss << std::left << std::setw(25) << benchmark.benchmarkName;
        oss << std::fixed << std::setprecision(2);
        oss << std::setw(12) << benchmark.totalTime;
        oss << std::setw(15) << benchmark.avgTimePerStep;
        oss << std::setw(15) << benchmark.minTime;
        oss << std::setw(15) << benchmark.maxTime;
        oss << std::setw(15) << std::fixed << std::setprecision(3) << benchmark.efficiencyScore;
        oss << std::setw(15) << std::fixed << std::setprecision(3) << benchmark.performanceScore;
        oss << std::setw(15) << std::fixed << std::setprecision(3) << benchmark.scalabilityScore;
        oss << "\n";
    }
    
    return oss.str();
}

std::string NLMBenchmarker::getBenchmarkSummary(const std::vector<BenchmarkResult>& results) {
    std::ostringstream oss;
    
    oss << "=== Benchmark Summary ===\n\n";
    
    if (results.empty()) {
        oss << "No benchmarks completed.\n";
        return oss.str();
    }
    
    double totalTime = 0.0;
    double totalNeurons = 0.0;
    double totalSpikes = 0.0;
    double avgEfficiency = 0.0;
    double avgPerformance = 0.0;
    double avgScalability = 0.0;
    
    for (const auto& benchmark : results) {
        totalTime += benchmark.totalTime;
        totalNeurons += benchmark.totalNeurons;
        totalSpikes += benchmark.totalSpikes;
        avgEfficiency += benchmark.efficiencyScore;
        avgPerformance += benchmark.performanceScore;
        avgScalability += benchmark.scalabilityScore;
    }
    
    size_t count = results.size();
    
    oss << "Overall Statistics:\n";
    oss << "  Total Benchmarks: " << count << "\n";
    oss << "  Average Total Time: " << std::fixed << std::setprecision(2) << (totalTime / count) << " ms\n";
    oss << "  Average Neurons: " << std::fixed << std::setprecision(0) << (totalNeurons / count) << "\n";
    oss << "  Average Spikes: " << std::fixed << std::setprecision(0) << (totalSpikes / count) << "\n";
    oss << "  Average Efficiency Score: " << std::fixed << std::setprecision(3) << (avgEfficiency / count) << "\n";
    oss << "  Average Performance Score: " << std::fixed << std::setprecision(3) << (avgPerformance / count) << "\n";
    oss << "  Average Scalability Score: " << std::fixed << std::setprecision(3) << (avgScalability / count) << "\n\n";
    
    oss << "Best Performers:\n";
    
    if (!results.empty()) {
        auto bestEfficiency = std::max_element(results.begin(), results.end(),
                                               [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                   return a.efficiencyScore < b.efficiencyScore;
                                               });
        if (bestEfficiency != results.end()) {
            oss << "  Most Efficient: " << bestEfficiency->benchmarkName << " ("
                << std::fixed << std::setprecision(3) << bestEfficiency->efficiencyScore << ")\n";
        }
        
        auto bestPerformance = std::max_element(results.begin(), results.end(),
                                                [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                    return a.performanceScore < b.performanceScore;
                                                });
        if (bestPerformance != results.end()) {
            oss << "  Best Performance: " << bestPerformance->benchmarkName << " ("
                << std::fixed << std::setprecision(3) << bestPerformance->performanceScore << ")\n";
        }
        
        auto bestScalability = std::max_element(results.begin(), results.end(),
                                                [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                    return a.scalabilityScore < b.scalabilityScore;
                                                });
        if (bestScalability != results.end()) {
            oss << "  Most Scalable: " << bestScalability->benchmarkName << " ("
                << std::fixed << std::setprecision(3) << bestScalability->scalabilityScore << ")\n";
        }
    }
    
    return oss.str();
}

// AgentProfiler implementations
ProfileResult AgentProfiler::profileAgentSensoryProcessing(const std::shared_ptr<AgentBrain>& agent,
                                                            const std::shared_ptr<Brain>& brain,
                                                            size_t steps, const std::string& name) {
    ProfileResult profile;
    profile.profileName = name + "_agent_sensory_processing";
    
    if (!agent || !brain) {
        NLM_LOG_ERROR("AgentProfiler::profileAgentSensoryProcessing: Cannot profile null agent or brain");
        return profile;
    }
    
    PerformanceProfiler profiler;
    profiler.startBrainProfiling(brain, name);
    
    for (size_t i = 0; i < steps; ++i) {
        // Simulate agent sensory processing
        // In a real implementation, this would involve actual sensory input processing
        
        double memoryUsage = agent->getBrain()->getTotalNeuronCount() * 100;
        
        Measurement measurement("agent_sensory_processing", 1.0, memoryUsage, 0,
                               agent->getBrain()->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    profile = profiler.stopBrainProfiling(brain);
    
    NLM_LOG_INFO("Profile: " + name + " agent sensory processing completed");
    
    return profile;
}

ProfileResult AgentProfiler::profileAgentMotorOutput(const std::shared_ptr<AgentBrain>& agent,
                                                     const std::shared_ptr<Brain>& brain,
                                                     size_t steps, const std::string& name) {
    ProfileResult profile;
    profile.profileName = name + "_agent_motor_output";
    
    if (!agent || !brain) {
        NLM_LOG_ERROR("AgentProfiler::profileAgentMotorOutput: Cannot profile null agent or brain");
        return profile;
    }
    
    PerformanceProfiler profiler;
    profiler.startBrainProfiling(brain, name);
    
    for (size_t i = 0; i < steps; ++i) {
        // Simulate agent motor output
        // In a real implementation, this would involve actual motor output decoding
        
        double memoryUsage = agent->getBrain()->getTotalNeuronCount() * 100;
        
        Measurement measurement("agent_motor_output", 1.0, memoryUsage, 0,
                               agent->getBrain()->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    profile = profiler.stopBrainProfiling(brain);
    
    NLM_LOG_INFO("Profile: " + name + " agent motor output completed");
    
    return profile;
}

ProfileResult AgentProfiler::profileAgentLearning(const std::shared_ptr<AgentBrain>& agent,
                                                   const std::shared_ptr<Brain>& brain,
                                                   size_t steps, const std::string& name) {
    ProfileResult profile;
    profile.profileName = name + "_agent_learning";
    
    if (!agent || !brain) {
        NLM_LOG_ERROR("AgentProfiler::profileAgentLearning: Cannot profile null agent or brain");
        return profile;
    }
    
    PerformanceProfiler profiler;
    profiler.startBrainProfiling(brain, name);
    
    for (size_t i = 0; i < steps; ++i) {
        // Simulate agent learning
        // In a real implementation, this would involve actual learning processes
        
        double memoryUsage = agent->getBrain()->getTotalNeuronCount() * 100;
        
        Measurement measurement("agent_learning", 1.0, memoryUsage, 0,
                               agent->getBrain()->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    profile = profiler.stopBrainProfiling(brain);
    
    NLM_LOG_INFO("Profile: " + name + " agent learning completed");
    
    return profile;
}

ProfileResult AgentProfiler::profileAgentDevelopment(const std::shared_ptr<AgentBrain>& agent,
                                                     const std::shared_ptr<Brain>& brain,
                                                     size_t steps, const std::string& name) {
    ProfileResult profile;
    profile.profileName = name + "_agent_development";
    
    if (!agent || !brain) {
        NLM_LOG_ERROR("AgentProfiler::profileAgentDevelopment: Cannot profile null agent or brain");
        return profile;
    }
    
    PerformanceProfiler profiler;
    profiler.startBrainProfiling(brain, name);
    
    for (size_t i = 0; i < steps; ++i) {
        // Simulate agent development
        // In a real implementation, this would involve actual development processes
        
        double memoryUsage = agent->getBrain()->getTotalNeuronCount() * 100;
        
        Measurement measurement("agent_development", 1.0, memoryUsage, 0,
                               agent->getBrain()->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    profile = profiler.stopBrainProfiling(brain);
    
    NLM_LOG_INFO("Profile: " + name + " agent development completed");
    
    return profile;
}

ProfileResult AgentProfiler::profileCompleteAgentSimulation(const std::shared_ptr<AgentBrain>& agent,
                                                             const std::shared_ptr<Brain>& brain,
                                                             size_t steps, const std::string& name) {
    ProfileResult profile;
    profile.profileName = name + "_complete_agent_simulation";
    
    if (!agent || !brain) {
        NLM_LOG_ERROR("AgentProfiler::profileCompleteAgentSimulation: Cannot profile null agent or brain");
        return profile;
    }
    
    PerformanceProfiler profiler;
    profiler.startBrainProfiling(brain, name);
    
    for (size_t i = 0; i < steps; ++i) {
        // Complete agent simulation
        // In a real implementation, this would involve all agent components
        
        double memoryUsage = agent->getBrain()->getTotalNeuronCount() * 100;
        
        Measurement measurement("complete_agent_simulation", 1.0, memoryUsage, 0,
                               agent->getBrain()->getTotalSpikeCount(), 0.0, 0.0, 0.0);
        profiler.recordMeasurement(name, measurement);
    }
    
    profile = profiler.stopBrainProfiling(brain);
    
    NLM_LOG_INFO("Profile: " + name + " complete agent simulation completed");
    
    return profile;
}

std::string AgentProfiler::compareAgentProfiles(const std::vector<ProfileResult>& profiles) {
    std::ostringstream oss;
    
    oss << "=== Agent Profile Comparison ===\n\n";
    
    if (profiles.empty()) {
        oss << "No agent profiles to compare.\n";
        return oss.str();
    }
    
    // Create comparison table
    oss << std::left << std::setw(30) << "Profile Name";
    oss << std::setw(12) << "Total Time (ms)";
    oss << std::setw(12) << "CPU Usage (%)";
    oss << std::setw(12) << "Memory Usage (MB)";
    oss << std::setw(12) << "Neuron Activity";
    oss << std::setw(12) << "Network Efficiency";
    oss << std::setw(12) << "Learning Progress\n";
    
    oss << std::string(30 + 12 + 12 + 12 + 12 + 12 + 12, '-') << "\n";
    
    for (const auto& profile : profiles) {
        oss << std::left << std::setw(30) << profile.profileName;
        oss << std::fixed << std::setprecision(2) << std::setw(12) << profile.totalTime;
        oss << std::fixed << std::setprecision(1) << std::setw(12) << profile.cpuUsage;
        oss << std::fixed << std::setprecision(2) << std::setw(12) << profile.memoryUsage;
        oss << std::fixed << std::setprecision(3) << std::setw(12) << profile.neuronActivity;
        oss << std::fixed << std::setprecision(3) << std::setw(12) << profile.networkEfficiency;
        oss << std::fixed << std::setprecision(3) << std::setw(12) << profile.learningProgress;
        oss << "\n";
    }
    
    return oss.str();
}

std::string AgentProfiler::getAgentProfilingStatistics(const std::vector<ProfileResult>& profiles) {
    std::ostringstream oss;
    
    oss << "=== Agent Profiling Statistics ===\n\n";
    
    if (profiles.empty()) {
        oss << "No agent profiles available.\n";
        return oss.str();
    }
    
    double totalTime = 0.0;
    double totalCPU = 0.0;
    double totalMemory = 0.0;
    double totalActivity = 0.0;
    double totalEfficiency = 0.0;
    double totalLearning = 0.0;
    
    for (const auto& profile : profiles) {
        totalTime += profile.totalTime;
        totalCPU += profile.cpuUsage;
        totalMemory += profile.memoryUsage;
        totalActivity += profile.neuronActivity;
        totalEfficiency += profile.networkEfficiency;
        totalLearning += profile.learningProgress;
    }
    
    size_t count = profiles.size();
    
    oss << "Overall Statistics:\n";
    oss << "  Total Profiles: " << count << "\n";
    oss << "  Average Total Time: " << std::fixed << std::setprecision(2) << (totalTime / count) << " ms\n";
    oss << "  Average CPU Usage: " << std::fixed << std::setprecision(1) << (totalCPU / count) << "%\n";
    oss << "  Average Memory Usage: " << std::fixed << std::setprecision(2) << (totalMemory / count) << " MB\n";
    oss << "  Average Neuron Activity: " << std::fixed << std::setprecision(3) << (totalActivity / count) << "\n";
    oss << "  Average Network Efficiency: " << std::fixed << std::setprecision(3) << (totalEfficiency / count) << "\n";
    oss << "  Average Learning Progress: " << std::fixed << std::setprecision(3) << (totalLearning / count) << "\n\n";
    
    oss << "Best Performing Profiles:\n";
    
    // Find best profiles for each metric
    auto bestTime = std::min_element(profiles.begin(), profiles.end(),
                                     [](const ProfileResult& a, const ProfileResult& b) {
                                         return a.totalTime < b.totalTime;
                                     });
    if (bestTime != profiles.end()) {
        oss << "  Fastest: " << bestTime->profileName << " ("
            << std::fixed << std::setprecision(2) << bestTime->totalTime << " ms)\n";
    }
    
    auto bestCPU = std::min_element(profiles.begin(), profiles.end(),
                                    [](const ProfileResult& a, const ProfileResult& b) {
                                        return a.cpuUsage < b.cpuUsage;
                                    });
    if (bestCPU != profiles.end()) {
        oss << "  Lowest CPU Usage: " << bestCPU->profileName << " ("
            << std::fixed << std::setprecision(1) << bestCPU->cpuUsage << "%\n";
    }
    
    auto bestMemory = std::min_element(profiles.begin(), profiles.end(),
                                       [](const ProfileResult& a, const ProfileResult& b) {
                                           return a.memoryUsage < b.memoryUsage;
                                       });
    if (bestMemory != profiles.end()) {
        oss << "  Lowest Memory Usage: " << bestMemory->profileName << " ("
            << std::fixed << std::setprecision(2) << bestMemory->memoryUsage << " MB\n";
    }
    
    return oss.str();
}

// PerformanceAnalyzer implementations
std::string PerformanceAnalyzer::analyzePerformanceBottlenecks(const ProfileResult& profile) {
    std::ostringstream oss;
    
    oss << "=== Performance Bottleneck Analysis ===\n\n";
    oss << "Profile: " << profile.profileName << "\n\n";
    
    // Check for bottlenecks
    if (profile.totalTime > 1000.0) { // > 1 second
        oss << "1. Execution Time Bottleneck\n";
        oss << "   Total execution time is high (" << std::fixed << std::setprecision(2) << profile.totalTime << " ms)\n";
        oss << "   Recommendation: Optimize neural network configuration or reduce simulation steps.\n\n";
    }
    
    if (profile.memoryUsage > 100.0) { // > 100 MB
        oss << "2. Memory Usage Bottleneck\n";
        oss << "   Memory usage is high (" << std::fixed << std::setprecision(2) << profile.memoryUsage << " MB)\n";
        oss << "   Recommendation: Optimize memory pool sizes or enable compression.\n\n";
    }
    
    if (profile.networkEfficiency < 0.1) { // < 0.1 spikes/ms
        oss << "3. Network Efficiency Bottleneck\n";
        oss << "   Network efficiency is low (" << std::fixed << std::setprecision(3) << profile.networkEfficiency << " spikes/ms)\n";
        oss << "   Recommendation: Adjust neuron parameters or connectivity patterns.\n\n";
    }
    
    if (profile.learningProgress < 0.1) { // < 0.1
        oss << "4. Learning Progress Bottleneck\n";
        oss << "   Learning progress is low (" << std::fixed << std::setprecision(3) << profile.learningProgress << ")\n";
        oss << "   Recommendation: Adjust learning rates or plasticity parameters.\n\n";
    }
    
    return oss.str();
}

std::string PerformanceAnalyzer::analyzeScalability(const std::vector<BenchmarkResult>& benchmarks) {
    std::ostringstream oss;
    
    oss << "=== Scalability Analysis ===\n\n";
    
    if (benchmarks.size() < 2) {
        oss << "Need at least 2 benchmarks for scalability analysis.\n";
        return oss.str();
    }
    
    // Analyze scalability across benchmarks
    oss << "Scalability Analysis:\n";
    
    for (size_t i = 1; i < benchmarks.size(); ++i) {
        const BenchmarkResult& current = benchmarks[i];
        const BenchmarkResult& previous = benchmarks[i - 1];
        
        double timeRatio = current.totalTime / previous.totalTime;
        double neuronRatio = current.totalNeurons / previous.totalNeurons;
        double scalability = neuronRatio / timeRatio;
        
        oss << "  " << current.benchmarkName << ":\n";
        oss << "    Neuron Count: " << current.totalNeurons << " -> " << previous.totalNeurons << " (" << neuronRatio << "x)\n";
        oss << "    Execution Time: " << std::fixed << std::setprecision(2) << current.totalTime << " -> "
            << previous.totalTime << " ms (" << timeRatio << "x)\n";
        oss << "    Scalability Score: " << std::fixed << std::setprecision(3) << scalability << "\n\n";
    }
    
    return oss.str();
}

std::string PerformanceAnalyzer::analyzeLearningEfficiency(const ProfileResult& profile) {
    std::ostringstream oss;
    
    oss << "=== Learning Efficiency Analysis ===\n\n";
    oss << "Profile: " << profile.profileName << "\n\n";
    
    // Analyze learning efficiency
    if (profile.measurements.size() > 1) {
        oss << "Learning Progress Analysis:\n";
        
        double initialLearning = profile.measurements.front().learningRate;
        double finalLearning = profile.measurements.back().learningRate;
        double learningImprovement = finalLearning - initialLearning;
        
        oss << "  Initial Learning Rate: " << std::fixed << std::setprecision(3) << initialLearning << "\n";
        oss << "  Final Learning Rate: " << std::fixed << std::setprecision(3) << finalLearning << "\n";
        oss << "  Learning Improvement: " << std::fixed << std::setprecision(3) << learningImprovement << "\n\n";
        
        // Calculate learning efficiency score
        double learningEfficiency = profile.learningProgress / profile.totalTime;
        oss << "Learning Efficiency Score: " << std::fixed << std::setprecision(3) << learningEfficiency << "\n\n";
    }
    
    return oss.str();
}

std::string PerformanceAnalyzer::analyzeMemoryEfficiency(const ProfileResult& profile) {
    std::ostringstream oss;
    
    oss << "=== Memory Efficiency Analysis ===\n\n";
    oss << "Profile: " << profile.profileName << "\n\n";
    
    // Analyze memory efficiency
    double memoryEfficiency = profile.memoryUsage / profile.neuronActivity;
    oss << "Memory Efficiency: " << std::fixed << std::setprecision(3) << memoryEfficiency << " MB/spike\n\n";
    
    oss << "Memory Usage Analysis:\n";
    oss << "  Total Memory Usage: " << std::fixed << std::setprecision(2) << profile.memoryUsage << " MB\n";
    oss << "  Neuron Activity: " << std::fixed << std::setprecision(3) << profile.neuronActivity << " spikes/ms\n";
    oss << "  Memory per Neuron: " << std::fixed << std::setprecision(2) << (profile.memoryUsage * 1024 * 1024 / std::max(profile.neuronActivity, 0.001)) << " bytes/neuron\n\n";
    
    return oss.str();
}

std::string PerformanceAnalyzer::generatePerformanceReport(const ProfileResult& profile,
                                                           const std::vector<BenchmarkResult>& benchmarks) {
    std::ostringstream oss;
    
    oss << "=== NLM Performance Report ===\n";
    oss << "Generated on: " << __TIMESTAMP__ << "\n\n";
    
    oss << "Profile Information:\n";
    oss << "  Profile Name: " << profile.profileName << "\n";
    oss << "  Total Time: " << std::fixed << std::setprecision(2) << profile.totalTime << " ms\n";
    oss << "  CPU Usage: " << std::fixed << std::setprecision(1) << profile.cpuUsage << "%\n";
    oss << "  Memory Usage: " << std::fixed << std::setprecision(2) << profile.memoryUsage << " MB\n";
    oss << "  Neuron Activity: " << std::fixed << std::setprecision(3) << profile.neuronActivity << " spikes/ms\n";
    oss << "  Network Efficiency: " << std::fixed << std::setprecision(3) << profile.networkEfficiency << "\n";
    oss << "  Learning Progress: " << std::fixed << std::setprecision(3) << profile.learningProgress << "\n\n";
    
    oss << "Benchmark Results:\n";
    for (const auto& benchmark : benchmarks) {
        oss << "  " << benchmark.benchmarkName << ":\n";
        oss << "    Total Time: " << std::fixed << std::setprecision(2) << benchmark.totalTime << " ms\n";
        oss << "    Efficiency Score: " << std::fixed << std::setprecision(3) << benchmark.efficiencyScore << "\n";
        oss << "    Performance Score: " << std::fixed << std::setprecision(3) << benchmark.performanceScore << "\n";
        oss << "    Scalability Score: " << std::fixed << std::setprecision(3) << benchmark.scalabilityScore << "\n\n";
    }
    
    return oss.str();
}

std::vector<std::string> PerformanceAnalyzer::identifyOptimizationOpportunities(const ProfileResult& profile,
                                                                             const std::vector<BenchmarkResult>& benchmarks) {
    std::vector<std::string> opportunities;
    
    // Identify optimization opportunities
    if (profile.totalTime > 1000.0) {
        opportunities.push_back("Execution time is high - consider optimizing neural network configuration");
    }
    
    if (profile.memoryUsage > 100.0) {
        opportunities.push_back("Memory usage is high - consider enabling memory optimization features");
    }
    
    if (profile.networkEfficiency < 0.1) {
        opportunities.push_back("Network efficiency is low - consider adjusting neuron parameters");
    }
    
    if (profile.learningProgress < 0.1) {
        opportunities.push_back("Learning progress is low - consider adjusting learning rates");
    }
    
    // Analyze benchmarks for optimization opportunities
    for (const auto& benchmark : benchmarks) {
        if (benchmark.avgTimePerStep > 10.0) { // > 10 ms per step
            opportunities.push_back("Benchmark " + benchmark.benchmarkName + ": Step execution is slow - consider optimizing step processing");
        }
        
        if (benchmark.efficiencyScore < 0.5) { // < 0.5
            opportunities.push_back("Benchmark " + benchmark.benchmarkName + ": Efficiency score is low - consider optimizing algorithm implementation");
        }
    }
    
    return opportunities;
}

double PerformanceAnalyzer::calculatePerformanceScore(const ProfileResult& profile,
                                                    const std::vector<BenchmarkResult>& benchmarks) {
    double timeScore = std::min(1.0, 1000.0 / profile.totalTime); // Better performance for lower times
    double cpuScore = 1.0 - std::min(1.0, profile.cpuUsage / 100.0); // Better performance for lower CPU usage
    double memoryScore = 1.0 - std::min(1.0, profile.memoryUsage / 100.0); // Better performance for lower memory usage
    double efficiencyScore = profile.networkEfficiency / 10.0; // Normalized efficiency
    
    return (timeScore + cpuScore + memoryScore + efficiencyScore) / 4.0;
}

std::vector<std::pair<std::string, double>> PerformanceAnalyzer::rankConfigurations(
    const std::vector<std::pair<std::string, ProfileResult>>& profiles,
    const std::vector<BenchmarkResult>& benchmarks) {
    std::vector<std::pair<std::string, double>> ranked;
    
    for (const auto& pair : profiles) {
        double performanceScore = calculatePerformanceScore(pair.second, benchmarks);
        ranked.push_back(std::make_pair(pair.first, performanceScore));
    }
    
    // Sort by performance score in descending order
    std::sort(ranked.begin(), ranked.end(),
              [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                  return a.second > b.second;
              });
    
    return ranked;
}

} // namespace nlm
