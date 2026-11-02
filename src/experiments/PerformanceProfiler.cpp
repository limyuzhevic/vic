#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../sensory/Vision.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>

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
        
        PerformanceMetric(const std::string& n, const std::string& u = "ms")
            : name(n), value(0.0), unit(u), totalDuration(0), callCount(0), 
              minValue(std::numeric_limits<double>::max()), 
              maxValue(std::numeric_limits<double>::lowest()) {}
    };
    
    std::map<std::string, PerformanceMetric> metrics;
    std::shared_ptr<Config> config;
    bool profilingEnabled;
    std::ofstream profileOutputFile;
    
    // Timing helpers
    double getCurrentTimeMs() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
    
    // Neural system profiling
    void profileBrainSimulation(std::shared_ptr<Brain> brain, uint64_t steps);
    void profileMemoryOperations(std::shared_ptr<Memory> memory);
    void profileNeuromodulation(std::shared_ptr<Neuromodulator> neuromodulator);
    void profileSensoryProcessing(std::shared_ptr<Vision> vision);
    void profilePlasticity(std::shared_ptr<Brain> brain, uint64_t steps);
    
    // Configuration helpers
    void setupProfilingMetrics();
    void updateMetric(const std::string& name, double value);
    void startTiming(const std::string& name);
    void stopTiming(const std::string& name);
    void logMetric(const std::string& name, double value, const std::string& unit = "ms");
    
    // Output formatting
    void printProfileReport();
    void writeProfileToFile(const std::string& filename);
    void exportMetricsToCSV(const std::string& filename);
    
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
};

} // namespace nlm
