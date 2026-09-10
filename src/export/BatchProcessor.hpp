#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"

namespace nlm {

// Batch processing system for running multiple simulations
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Add simulation to batch
    struct BatchSimulation {
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Environment> environment;
        std::string name;
        std::map<std::string, std::string> parameters;
        size_t steps;
        bool enabled;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double executionTime;
        float finalReward;
        std::map<std::string, float> metrics;
    };
    
    void addSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                      size_t steps, const std::string& name = "");
    
    // Run batch processing
    bool runBatch(const std::string& outputDir = "batch_results");
    void stopBatch();
    bool isRunning() const;
    
    // Get batch results
    std::vector<BatchSimulation> getResults() const;
    BatchSimulation getResult(size_t index) const;
    size_t getSimulationCount() const;
    
    // Configuration
    void setParallelProcessing(bool enable, size_t threadCount = 0);
    void setCheckpointInterval(size_t interval);
    void setOutputFormat(const std::string& format);
    void setMetricsCollection(const std::vector<std::string>& metrics);
    
    // Progress tracking
    double getProgress() const;
    std::string getStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
