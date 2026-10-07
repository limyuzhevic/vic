#pragma once

#include "CommandBase.hpp"
#include "../brain/Brain.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

namespace nlm {

class BatchCommand : public CommandBase {
public:
    BatchCommand();
    ~BatchCommand();
    
    std::string getName() const override;
    std::string getDescription() const override;
    std::string getUsage() const override;
    std::string getHelp() const override;
    
    CommandStatus getStatus() const override;
    double getProgress() const override;
    std::string getProgressText() const override;
    
    bool execute() override;
    bool cancel() override;
    bool isCancellable() const override;
    
    CommandResult getResult() const override;
    std::vector<std::string> getArguments() const override;
    void setArguments(const std::vector<std::string>& args) override;
    
    bool validateArguments() const override;
    std::string getLastError() const override;
    
    std::chrono::milliseconds getEstimatedExecutionTime() const override;
    
    Category getCategory() const override;
    
    // Batch-specific functionality
    bool addSimulationConfig(const std::string& configName, const std::shared_ptr<Config>& config);
    bool addSimulationFile(const std::string& filepath);
    bool clearSimulations();
    
    std::vector<std::string> getSimulationConfigs() const;
    std::vector<std::string> getSimulationFiles() const;
    
    // Parallel processing settings
    void setMaxParallelSimulations(size_t maxParallel);
    void setThreadPoolSize(size_t poolSize);
    void enableGpuAcceleration(bool enable);
    
    // Progress tracking
    struct BatchProgress {
        std::string simulationName;
        double progress;
        std::string statusText;
        std::chrono::milliseconds elapsedTime;
        size_t simulationIndex;
        size_t totalSimulations;
    };
    
    std::vector<BatchProgress> getBatchProgress() const;
    
    // Performance optimization
    void setPerformanceMode(const std::string& mode); // "balanced", "fast", "accurate"
    void setMemoryLimit(size_t limitBytes);
    void setTimeLimit(std::chrono::seconds limit);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm