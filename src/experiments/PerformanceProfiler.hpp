#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <memory>
#include <chrono>

namespace nlm {

class PerformanceProfiler {
private:
    struct PerformanceMetric {
        std::string name;
        double value;
        std::string unit;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::duration totalDuration;
        int callCount;
        double minValue;
        double maxValue;
        
        PerformanceMetric(const std::string& n, const std::string& u = "ms");
    };
    
    std::map<std::string, PerformanceMetric> metrics;
    std::shared_ptr<Config> config;
    bool profilingEnabled;
    
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void run(int argc, char* argv[]);
    void printBanner();
    void showHelp();
    void startProfiling();
    void stopProfiling();
    void profileSystem(std::shared_ptr<Brain> brain, std::shared_ptr<Memory> memory,
                      std::shared_ptr<Neuromodulator> neuromodulator, std::shared_ptr<Vision> vision);
    void generateProfileReport(const std::string& outputFile = "");
    void resetMetrics();
    
private:
    void setupProfilingMetrics();
    void updateMetric(const std::string& name, double value);
    void startTiming(const std::string& name);
    void stopTiming(const std::string& name);
    void logMetric(const std::string& name, double value, const std::string& unit = "ms");
    void printProfileReport();
    void writeProfileToFile(const std::string& filename);
    void exportMetricsToCSV(const std::string& filename);
    void profileBrainSimulation(std::shared_ptr<Brain> brain, uint64_t steps);
    void profileMemoryOperations(std::shared_ptr<Memory> memory);
    void profileNeuromodulation(std::shared_ptr<Neuromodulator> neuromodulator);
    void profileSensoryProcessing(std::shared_ptr<Vision> vision);
    void profilePlasticity(std::shared_ptr<Brain> brain, uint64_t steps);
};

} // namespace nlm
