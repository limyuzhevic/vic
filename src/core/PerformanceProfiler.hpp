#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>

namespace nlm {

class PerformanceProfiler {
public:
    struct Measurement {
        std::string name;
        std::chrono::duration<double> duration;
        uint64_t iterations;
        double average_ns;
    };
    
    PerformanceProfiler() = default;
    
    // Start timing a section
    void startMeasurement(const std::string& name) {
        measurements_[name] = std::chrono::high_resolution_clock::now();
    }
    
    // Stop timing and record the result
    void stopMeasurement(const std::string& name, uint64_t iterations = 1) {
        auto it = measurements_.find(name);
        if (it == measurements_.end()) {
            return; // Not started
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - it->second);
        
        Measurement measurement;
        measurement.name = name;
        measurement.duration = duration;
        measurement.iterations = iterations;
        measurement.average_ns = (duration.count() * 1e9) / iterations;
        
        measurements_[name] = Measurement(); // Reset
        
        recorded_measurements_.push_back(measurement);
    }
    
    // Get measurements
    const std::vector<Measurement>& getMeasurements() const {
        return recorded_measurements_;
    }
    
    // Clear all measurements
    void clearMeasurements() {
        measurements_.clear();
        recorded_measurements_.clear();
    }
    
    // Print measurements in a formatted way
    void printMeasurements() const {
        std::cout << "=== Performance Measurements ===" << std::endl;
        
        if (recorded_measurements_.empty()) {
            std::cout << "No measurements recorded" << std::endl;
            return;
        }
        
        std::cout << std::left << std::setw(20) << "Operation"
                  << std::right << std::setw(15) << "Total Time"
                  << std::right << std::setw(15) << "Avg Time (μs)"
                  << std::right << std::setw(15) << "Iterations"
                  << std::endl;
        
        std::cout << std::string(65, '-') << std::endl;
        
        for (const auto& measurement : recorded_measurements_) {
            std::cout << std::left << std::setw(20) << measurement.name
                      << std::right << std::setw(15) << std::fixed << std::setprecision(6) << measurement.duration.count() << "s"
                      << std::right << std::setw(15) << std::fixed << std::setprecision(3) << (measurement.average_ns / 1000.0) << "μs"
                      << std::right << std::setw(15) << measurement.iterations
                      << std::endl;
        }
        
        std::cout << "===============================" << std::endl;
    }
    
    // Calculate summary statistics
    struct SummaryStatistics {
        double total_time;
        double min_time;
        double max_time;
        double mean_time;
        double std_dev_time;
    };
    
    SummaryStatistics calculateStatistics() const {
        if (recorded_measurements_.empty()) {
            return {0.0, 0.0, 0.0, 0.0, 0.0};
        }
        
        SummaryStatistics stats;
        std::vector<double> times;
        
        for (const auto& measurement : recorded_measurements_) {
            times.push_back(measurement.duration.count());
        }
        
        // Calculate statistics
        stats.total_time = std::accumulate(times.begin(), times.end(), 0.0);
        stats.min_time = *std::min_element(times.begin(), times.end());
        stats.max_time = *std::max_element(times.begin(), times.end());
        
        // Calculate mean
        stats.mean_time = stats.total_time / times.size();
        
        // Calculate standard deviation
        double variance = 0.0;
        for (double time : times) {
            variance += (time - stats.mean_time) * (time - stats.mean_time);
        }
        variance /= times.size();
        stats.std_dev_time = std::sqrt(variance);
        
        return stats;
    }
    
    // Print summary statistics
    void printSummaryStatistics() const {
        SummaryStatistics stats = calculateStatistics();
        
        std::cout << "=== Summary Statistics ===" << std::endl;
        std::cout << "Total operations: " << recorded_measurements_.size() << std::endl;
        std::cout << "Total time: " << std::fixed << std::setprecision(6) << stats.total_time << "s" << std::endl;
        std::cout << "Average time per operation: " << std::fixed << std::setprecision(6) << stats.mean_time << "s" << std::endl;
        std::cout << "Min time: " << std::fixed << std::setprecision(6) << stats.min_time << "s" << std::endl;
        std::cout << "Max time: " << std::fixed << std::setprecision(6) << stats.max_time << "s" << std::endl;
        std::cout << "Standard deviation: " << std::fixed << std::setprecision(6) << stats.std_dev_time << "s" << std::endl;
        std::cout << "==========================" << std::endl;
    }
    
private:
    std::map<std::string, std::chrono::high_resolution_clock::time_point> measurements_;
    std::vector<Measurement> recorded_measurements_;
};

// Global performance profiler
PerformanceProfiler& getGlobalProfiler() {
    static PerformanceProfiler profiler;
    return profiler;
}

// Convenience functions
void startMeasurement(const std::string& name) {
    getGlobalProfiler().startMeasurement(name);
}

void stopMeasurement(const std::string& name, uint64_t iterations = 1) {
    getGlobalProfiler().stopMeasurement(name, iterations);
}

void printPerformanceReport() {
    getGlobalProfiler().printMeasurements();
    getGlobalProfiler().printSummaryStatistics();
}

void clearPerformanceMeasurements() {
    getGlobalProfiler().clearMeasurements();
}

} // namespace nlm