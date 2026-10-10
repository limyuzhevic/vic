// NLM Performance Profiler Tool
// Performance monitoring and profiling capabilities for NLM simulations

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <numeric>
#include <sstream>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

using namespace nlm;

void printProfileBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Performance Profiler                    ║
║                                                               ║
║  Monitor and profile simulation performance                  ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

class NLMProfiler {
public:
    struct ProfileSession {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        bool isRunning;
        double totalCpuUsage;
        double maxCpuUsage;
        double avgCpuUsage;
        double totalMemoryUsage;
        double maxMemoryUsage;
        double avgMemoryUsage;
        size_t totalNeuronsProcessed;
        size_t totalSpikesGenerated;
        double totalSimulationTime;
        std::vector<double> cpuUsageHistory;
        std::vector<double> memoryUsageHistory;
        std::vector<double> simulationTimeHistory;
        std::vector<size_t> neuronCountHistory;
        std::vector<size_t> spikeCountHistory;
        std::map<std::string, std::string> systemInfo;
    };

    struct ProfileMetrics {
        double cpuUtilization;
        double memoryEfficiency;
        double simulationSpeed;
        double learningEfficiency;
        double stabilityScore;
        std::vector<std::string> performanceIssues;
        std::vector<std::string> optimizationSuggestions;
        std::map<std::string, double> resourceBreakdown;
    };

    NLMProfiler() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        currentSession_.isRunning = false;
    }

    void startProfile(const std::string& name, const std::map<std::string, std::string>& systemInfo = {}) {
        currentSession_.name = name;
        currentSession_.startTime = std::chrono::high_resolution_clock::now();
        currentSession_.isRunning = true;
        currentSession_.totalCpuUsage = 0.0;
        currentSession_.maxCpuUsage = 0.0;
        currentSession_.avgCpuUsage = 0.0;
        currentSession_.totalMemoryUsage = 0.0;
        currentSession_.maxMemoryUsage = 0.0;
        currentSession_.avgMemoryUsage = 0.0;
        currentSession_.totalNeuronsProcessed = 0;
        currentSession_.totalSpikesGenerated = 0;
        currentSession_.totalSimulationTime = 0.0;
        currentSession_.cpuUsageHistory.clear();
        currentSession_.memoryUsageHistory.clear();
        currentSession_.simulationTimeHistory.clear();
        currentSession_.neuronCountHistory.clear();
        currentSession_.spikeCountHistory.clear();
        currentSession_.systemInfo = systemInfo;
        
        NLM_LOG_INFO("Starting performance profile: " + name);
    }

    void updateMetrics(double cpuUsage, double memoryUsage, size_t neuronsProcessed, size_t spikesGenerated, double simulationTime) {
        if (!currentSession_.isRunning) return;
        
        currentSession_.cpuUsageHistory.push_back(cpuUsage);
        currentSession_.memoryUsageHistory.push_back(memoryUsage);
        currentSession_.simulationTimeHistory.push_back(simulationTime);
        currentSession_.neuronCountHistory.push_back(neuronsProcessed);
        currentSession_.spikeCountHistory.push_back(spikesGenerated);
        
        currentSession_.totalCpuUsage += cpuUsage;
        currentSession_.totalMemoryUsage += memoryUsage;
        currentSession_.totalNeuronsProcessed += neuronsProcessed;
        currentSession_.totalSpikesGenerated += spikesGenerated;
        currentSession_.totalSimulationTime += simulationTime;
        
        currentSession_.maxCpuUsage = std::max(currentSession_.maxCpuUsage, cpuUsage);
        currentSession_.maxMemoryUsage = std::max(currentSession_.maxMemoryUsage, memoryUsage);
        
        NLM_LOG_INFO("Profile update: CPU=" + std::to_string(cpuUsage) + "%, " +
                    "Memory=" + std::to_string(memoryUsage) + "MB, " +
                    "Neurons=" + std::to_string(neuronsProcessed) + ", " +
                    "Spikes=" + std::to_string(spikesGenerated));
    }

    void stopProfile() {
        if (!currentSession_.isRunning) {
            NLM_LOG_WARNING("No active profile session to stop");
            return;
        }
        
        currentSession_.endTime = std::chrono::high_resolution_clock::now();
        currentSession_.isRunning = false;
        
        // Calculate averages
        size_t historySize = currentSession_.cpuUsageHistory.size();
        if (historySize > 0) {
            currentSession_.avgCpuUsage = currentSession_.totalCpuUsage / historySize;
            currentSession_.avgMemoryUsage = currentSession_.totalMemoryUsage / historySize;
        }
        
        double duration = getDuration();
        if (duration > 0) {
            currentSession_.avgCpuUsage = currentSession_.totalCpuUsage / duration;
            currentSession_.avgMemoryUsage = currentSession_.totalMemoryUsage / duration;
        }
        
        NLM_LOG_INFO("Profile completed: " + currentSession_.name);
        NLM_LOG_INFO("  Duration: " + std::to_string(duration) + "s");
        NLM_LOG_INFO("  Average CPU: " + std::to_string(currentSession_.avgCpuUsage) + "%");
        NLM_LOG_INFO("  Average Memory: " + std::to_string(currentSession_.avgMemoryUsage) + "MB");
        NLM_LOG_INFO("  Neurons processed: " + std::to_string(currentSession_.totalNeuronsProcessed));
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(currentSession_.totalSpikesGenerated));
    }

    double getDuration() const {
        if (!currentSession_.isRunning) return 0.0;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - currentSession_.startTime);
        return duration.count();
    }

    ProfileMetrics analyzePerformance() {
        ProfileMetrics metrics;
        
        size_t historySize = currentSession_.cpuUsageHistory.size();
        if (historySize == 0) return metrics;
        
        // Calculate performance metrics
        metrics.cpuUtilization = currentSession_.avgCpuUsage;
        metrics.memoryEfficiency = calculateMemoryEfficiency();
        metrics.simulationSpeed = calculateSimulationSpeed();
        metrics.learningEfficiency = calculateLearningEfficiency();
        metrics.stabilityScore = calculateStabilityScore();
        
        // Detect performance issues
        detectPerformanceIssues(metrics);
        
        // Generate optimization suggestions
        generateOptimizationSuggestions(metrics);
        
        // Resource breakdown
        metrics.resourceBreakdown["cpu"] = currentSession_.avgCpuUsage;
        metrics.resourceBreakdown["memory"] = currentSession_.avgMemoryUsage;
        metrics.resourceBreakdown["neurons_per_second"] = currentSession_.totalNeuronsProcessed / std::max(1.0, getDuration());
        metrics.resourceBreakdown["spikes_per_second"] = currentSession_.totalSpikesGenerated / std::max(1.0, getDuration());
        
        return metrics;
    }

    void exportProfile(const std::string& filename, const std::string& format = "json") {
        if (format == "json") {
            exportAsJSON(filename);
        } else if (format == "csv") {
            exportAsCSV(filename);
        } else if (format == "txt") {
            exportAsTXT(filename);
        }
        
        NLM_LOG_INFO("Profile exported to " + filename);
    }

    void printProfileSummary() {
        std::cout << "=== NLM Performance Profile Summary ===" << std::endl;
        std::cout << "Profile Name: " << currentSession_.name << std::endl;
        std::cout << "Duration: " << getDuration() << "s" << std::endl;
        std::cout << std::endl;
        
        if (currentSession_.cpuUsageHistory.size() > 0) {
            std::cout << "Performance Metrics:" << std::endl;
            std::cout << "  CPU Usage:" << std::endl;
            std::cout << "    Average: " << std::fixed << std::setprecision(1) 
                      << currentSession_.avgCpuUsage << "%" << std::endl;
            std::cout << "    Maximum: " << std::fixed << std::setprecision(1) 
                      << currentSession_.maxCpuUsage << "%" << std::endl;
            std::cout << "  Memory Usage:" << std::endl;
            std::cout << "    Average: " << std::fixed << std::setprecision(1) 
                      << currentSession_.avgMemoryUsage << "MB" << std::endl;
            std::cout << "    Maximum: " << std::fixed << std::setprecision(1) 
                      << currentSession_.maxMemoryUsage << "MB" << std::endl;
            std::cout << "  Processing Speed:" << std::endl;
            std::cout << "    Neurons/sec: " << currentSession_.totalNeuronsProcessed / std::max(1.0, getDuration()) << std::endl;
            std::cout << "    Spikes/sec: " << currentSession_.totalSpikesGenerated / std::max(1.0, getDuration()) << std::endl;
            std::cout << std::endl;
            
            // Generate analysis
            ProfileMetrics metrics = analyzePerformance();
            
            std::cout << "Performance Analysis:" << std::endl;
            std::cout << "  CPU Utilization: " << std::fixed << std::setprecision(1) 
                      << metrics.cpuUtilization << "%" << std::endl;
            std::cout << "  Memory Efficiency: " << std::fixed << std::setprecision(3) 
                      << metrics.memoryEfficiency << std::endl;
            std::cout << "  Simulation Speed: " << std::fixed << std::setprecision(1) 
                      << metrics.simulationSpeed << " steps/sec" << std::endl;
            std::cout << "  Learning Efficiency: " << std::fixed << std::setprecision(3) 
                      << metrics.learningEfficiency << std::endl;
            std::cout << "  Stability Score: " << std::fixed << std::setprecision(3) 
                      << metrics.stabilityScore << std::endl;
            
            if (!metrics.performanceIssues.empty()) {
                std::cout << "  Performance Issues:" << std::endl;
                for (const auto& issue : metrics.performanceIssues) {
                    std::cout << "    - " << issue << std::endl;
                }
            }
            
            if (!metrics.optimizationSuggestions.empty()) {
                std::cout << "  Optimization Suggestions:" << std::endl;
                for (const auto& suggestion : metrics.optimizationSuggestions) {
                    std::cout << "    - " << suggestion << std::endl;
                }
            }
        }
    }

private:
    ProfileSession currentSession_;

    double calculateMemoryEfficiency() {
        // Memory efficiency = useful work / memory used
        double memoryUsed = currentSession_.avgMemoryUsage;
        double processingPower = currentSession_.totalNeuronsProcessed / std::max(1.0, getDuration());
        
        return (processingPower / memoryUsed) * 1000.0; // Scale for readability
    }

    double calculateSimulationSpeed() {
        // Steps per second based on simulation time
        return currentSession_.totalSimulationTime / std::max(1.0, getDuration());
    }

    double calculateLearningEfficiency() {
        // Learning efficiency = spikes generated / simulation time
        return currentSession_.totalSpikesGenerated / std::max(1.0, getDuration());
    }

    double calculateStabilityScore() {
        // Stability based on consistency of metrics
        if (currentSession_.cpuUsageHistory.size() < 2) return 1.0;
        
        double cpuVariance = calculateVariance(currentSession_.cpuUsageHistory);
        double memoryVariance = calculateVariance(currentSession_.memoryUsageHistory);
        
        // Lower variance = higher stability
        double stability = 1.0 / (1.0 + cpuVariance + memoryVariance);
        return std::min(1.0, stability);
    }

    double calculateVariance(const std::vector<double>& data) {
        if (data.size() <= 1) return 0.0;
        
        double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        double variance = 0.0;
        
        for (double val : data) {
            variance += (val - mean) * (val - mean);
        }
        
        return variance / data.size();
    }

    void detectPerformanceIssues(ProfileMetrics& metrics) {
        if (currentSession_.avgCpuUsage > 90.0) {
            metrics.performanceIssues.push_back("High CPU usage (>90%)");
        }
        
        if (currentSession_.avgMemoryUsage > 2000.0) {
            metrics.performanceIssues.push_back("High memory usage (>2GB)");
        }
        
        if (currentSession_.avgCpuUsage < 20.0 && currentSession_.totalNeuronsProcessed > 1000) {
            metrics.performanceIssues.push_back("Underutilized CPU resources");
        }
        
        double spikeRate = currentSession_.totalSpikesGenerated / std::max(1.0, getDuration());
        if (spikeRate < 10.0) {
            metrics.performanceIssues.push_back("Low spike generation rate");
        }
    }

    void generateOptimizationSuggestions(ProfileMetrics& metrics) {
        if (currentSession_.avgCpuUsage > 80.0) {
            metrics.optimizationSuggestions.push_back("Consider reducing neuron count or complexity");
        }
        
        if (currentSession_.avgMemoryUsage > 1000.0) {
            metrics.optimizationSuggestions.push_back("Enable memory optimization modes");
        }
        
        double spikeRate = currentSession_.totalSpikesGenerated / std::max(1.0, getDuration());
        if (spikeRate < 50.0) {
            metrics.optimizationSuggestions.push_back("Check synaptic connectivity and learning parameters");
        }
        
        metrics.optimizationSuggestions.push_back("Use checkpointing for long simulations");
        metrics.optimizationSuggestions.push_back("Consider batch processing for multiple experiments");
    }

    void exportAsJSON(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "{" << std::endl;
        file << "  \"profile_name\": \"" << currentSession_.name << "\"," << std::endl;
        file << "  \"start_time\": \"" << getCurrentTimestamp() << "\"," << std::endl;
        file << "  \"duration_seconds\": " << getDuration() << "," << std::endl;
        file << "  \"average_cpu_usage\": " << currentSession_.avgCpuUsage << "," << std::endl;
        file << "  \"max_cpu_usage\": " << currentSession_.maxCpuUsage << "," << std::endl;
        file << "  \"average_memory_usage\": " << currentSession_.avgMemoryUsage << "," << std::endl;
        file << "  \"max_memory_usage\": " << currentSession_.maxMemoryUsage << "," << std::endl;
        file << "  \"total_neurons_processed\": " << currentSession_.totalNeuronsProcessed << "," << std::endl;
        file << "  \"total_spikes_generated\": " << currentSession_.totalSpikesGenerated << "," << std::endl;
        
        file << "  \"cpu_usage_history\": [";
        for (size_t i = 0; i < currentSession_.cpuUsageHistory.size(); ++i) {
            file << currentSession_.cpuUsageHistory[i];
            if (i < currentSession_.cpuUsageHistory.size() - 1) file << ", ";
        }
        file << "]" << std::endl;
        
        file << "  \"memory_usage_history\": [";
        for (size_t i = 0; i < currentSession_.memoryUsageHistory.size(); ++i) {
            file << currentSession_.memoryUsageHistory[i];
            if (i < currentSession_.memoryUsageHistory.size() - 1) file << ", ";
        }
        file << "]" << std::endl;
        
        file << "}" << std::endl;
        file.close();
    }

    void exportAsCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "timestamp,cpu_usage,memory_usage,neurons_processed,spikes_generated" << std::endl;
        
        auto start = currentSession_.startTime;
        for (size_t i = 0; i < currentSession_.cpuUsageHistory.size(); ++i) {
            double timestamp = i * 1.0; // 1 second intervals for simulation
            file << timestamp << ","
                 << currentSession_.cpuUsageHistory[i] << ","
                 << currentSession_.memoryUsageHistory[i] << ","
                 << currentSession_.neuronCountHistory[i] << ","
                 << currentSession_.spikeCountHistory[i] << std::endl;
        }
        
        file.close();
    }

    void exportAsTXT(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Profile" << std::endl;
        file << "=======================" << std::endl;
        file << std::endl;
        file << "Profile: " << currentSession_.name << std::endl;
        file << "Started: " << getCurrentTimestamp() << std::endl;
        file << "Duration: " << getDuration() << " seconds" << std::endl;
        file << std::endl;
        file << "Performance Summary:" << std::endl;
        file << "  Average CPU Usage: " << currentSession_.avgCpuUsage << "%" << std::endl;
        file << "  Maximum CPU Usage: " << currentSession_.maxCpuUsage << "%" << std::endl;
        file << "  Average Memory Usage: " << currentSession_.avgMemoryUsage << " MB" << std::endl;
        file << "  Maximum Memory Usage: " << currentSession_.maxMemoryUsage << " MB" << std::endl;
        file << "  Total Neurons Processed: " << currentSession_.totalNeuronsProcessed << std::endl;
        file << "  Total Spikes Generated: " << currentSession_.totalSpikesGenerated << std::endl;
        file << std::endl;
        
        file.close();
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

int main(int argc, char** argv) {
    printProfileBanner();
    
    if (argc < 2) {
        std::cout << "Usage: nlm_profile <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  start [<name>] [<duration>]     Start performance profiling" << std::endl;
        std::cout << "  stop                              Stop profiling and save results" << std::endl;
        std::cout << "  status                             Show current profiling status" << std::endl;
        std::cout << "  results [<format>]                Show profiling results" << std::endl;
        std::cout << "  export <file> [<format>]          Export profiling data" << std::endl;
        std::cout << "  analyze                           Analyze current profile" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_profile start learning_exp 3600    Start profiling for 1 hour" << std::endl;
        std::cout << "  nlm_profile stop                    Stop and save results" << std::endl;
        std::cout << "  nlm_profile results json            Show results in JSON format" << std::endl;
        std::cout << "  nlm_profile export profile.json     Export to file" << std::endl;
        std::cout << std::endl;
        return 0;
    }

    std::string command(argv[1]);
    NLMProfiler profiler;
    
    if (command == "start") {
        std::string profileName = "session_" + std::to_string(std::time(nullptr));
        int duration = 0;
        
        if (argc >= 3) {
            profileName = argv[2];
        }
        if (argc >= 4) {
            duration = std::stoi(argv[3]);
        }
        
        profiler.startProfile(profileName);
        
        if (duration > 0) {
            std::cout << "Profiling for " << duration << " seconds... (Press Ctrl+C to stop)" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(duration));
            profiler.stopProfile();
            
            // Show results
            profiler.printProfileSummary();
        }
        
    } else if (command == "stop") {
        profiler.stopProfile();
        profiler.printProfileSummary();
        
    } else if (command == "status") {
        if (profiler.isRunning()) {
            std::cout << "Profile is currently running" << std::endl;
            std::cout << "Name: " << profiler.getName() << std::endl;
            std::cout << "Duration: " << profiler.getDuration() << "s" << std::endl;
        } else {
            std::cout << "No active profile session" << std::endl;
        }
        
    } else if (command == "results") {
        std::string format = (argc >= 3) ? argv[2] : "txt";
        profiler.showResults(format);
        
    } else if (command == "export") {
        if (argc < 3) {
            std::cout << "Error: Missing file argument for export" << std::endl;
            return 1;
        }
        
        std::string filename = argv[2];
        std::string format = (argc >= 4) ? argv[3] : "json";
        
        profiler.exportProfile(filename, format);
        
    } else if (command == "analyze") {
        ProfileMetrics metrics = profiler.analyzePerformance();
        
        std::cout << "=== Performance Analysis ===" << std::endl;
        std::cout << "CPU Utilization: " << metrics.cpuUtilization << "%" << std::endl;
        std::cout << "Memory Efficiency: " << metrics.memoryEfficiency << std::endl;
        std::cout << "Simulation Speed: " << metrics.simulationSpeed << " steps/sec" << std::endl;
        std::cout << "Learning Efficiency: " << metrics.learningEfficiency << " spikes/sec" << std::endl;
        std::cout << "Stability Score: " << metrics.stabilityScore << std::endl;
        
        if (!metrics.performanceIssues.empty()) {
            std::cout << std::endl << "Performance Issues:" << std::endl;
            for (const auto& issue : metrics.performanceIssues) {
                std::cout << "  - " << issue << std::endl;
            }
        }
        
        if (!metrics.optimizationSuggestions.empty()) {
            std::cout << std::endl << "Optimization Suggestions:" << std::endl;
            for (const auto& suggestion : metrics.optimizationSuggestions) {
                std::cout << "  - " << suggestion << std::endl;
            }
        }
        
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        return 1;
    }
    
    return 0;
}

// Helper functions (simplified for this example)
bool NLMProfiler::isRunning() const {
    return currentSession_.isRunning;
}

std::string NLMProfiler::getName() const {
    return currentSession_.name;
}

void NLMProfiler::showResults(const std::string& format) {
    if (format == "json") {
        std::cout << "{\"profile_name\": \"" << currentSession_.name 
                  << "\", \"duration\": " << getDuration() << "}";
    } else {
        printProfileSummary();
    }
}
