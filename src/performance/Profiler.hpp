#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <memory>
#include <atomic>

namespace nlm {

class PerformanceProfiler {
public:
    struct OperationStats {
        std::string name;
        uint64_t calls = 0;
        uint64_t cycles = 0;
        uint64_t bytes = 0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        double avgTime() const { return totalTime / calls; }
        
        double totalTime = 0.0;
    };
    
    PerformanceProfiler() : running(false), totalCycles(0), totalBytes(0) {}
    
    void start(const std::string& name) {
        auto now = std::chrono::high_resolution_clock::now();
        operations[name] = {name, 0, 0, 0, 0.0, 0.0, 0.0, 0.0};
        operationTimes[name] = now;
    }
    
    void stop(const std::string& name) {
        auto end = std::chrono::high_resolution_clock::now();
        auto start = operationTimes[name];
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        double timeMs = duration.count() / 1000000.0;
        
        if (operations.find(name) != operations.end()) {
            auto& stats = operations[name];
            stats.calls++;
            stats.totalTime += timeMs;
            if (timeMs < stats.minTime) stats.minTime = timeMs;
            if (timeMs > stats.maxTime) stats.maxTime = timeMs;
        }
    }
    
    void incrementCycles(const std::string& name, uint64_t cycles) {
        if (operations.find(name) != operations.end()) {
            operations[name].cycles += cycles;
        }
        totalCycles += cycles;
    }
    
    void incrementBytes(const std::string& name, uint64_t bytes) {
        if (operations.find(name) != operations.end()) {
            operations[name].bytes += bytes;
        }
        totalBytes += bytes;
    }
    
    void startOverall() { overallStart = std::chrono::high_resolution_clock::now(); }
    void stopOverall() {
        auto end = std::chrono::high_resolution_clock::now();
        overallDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - overallStart);
    }
    
    void printReport(const std::string& filename = "performance_report.txt") {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "NLM Performance Report" << std::endl;
        file << "=====================" << std::endl << std::endl;
        
        file << "Overall performance:" << std::endl;
        file << "  Total execution time: " << overallDuration.count() / 1000000.0 << " ms" << std::endl;
        file << "  Total cycles: " << totalCycles << std::endl;
        file << "  Total memory allocated: " << totalBytes << " bytes" << std::endl << std::endl;
        
        file << "Operation breakdown:" << std::endl;
        file << std::left << std::setw(30) << "Operation"
              << std::setw(10) << "Calls"
              << std::setw(15) << "Time (ms)"
              << std::setw(15) << "Cycles"
              << std::setw(15) << "Bytes" << std::endl;
        file << std::string(75, '-') << std::endl;
        
        for (const auto& pair : operations) {
            const auto& stats = pair.second;
            file << std::left << std::setw(30) << stats.name
                 << std::setw(10) << stats.calls
                 << std::setw(15) << std::fixed << std::setprecision(3) << stats.totalTime
                 << std::setw(15) << stats.cycles
                 << std::setw(15) << stats.bytes << std::endl;
        }
        
        file.close();
    }
    
private:
    std::map<std::string, OperationStats> operations;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> operationTimes;
    bool running = false;
    uint64_t totalCycles = 0;
    uint64_t totalBytes = 0;
    std::chrono::high_resolution_clock::time_point overallStart;
    std::chrono::nanoseconds overallDuration;
};

extern PerformanceProfiler profiler;

} // namespace nlm