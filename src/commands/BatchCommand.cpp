#include "BatchCommand.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>

namespace nlm {

struct BatchCommand::Impl {
    std::vector<std::shared_ptr<Config>> simulationConfigs;
    std::vector<std::string> simulationFiles;
    std::vector<std::string> simulationNames;
    
    // Thread pool and parallel processing
    size_t maxParallelSimulations;
    size_t threadPoolSize;
    bool gpuAcceleration;
    
    // Performance settings
    std::string performanceMode; // "balanced", "fast", "accurate"
    size_t memoryLimitBytes;
    std::chrono::seconds timeLimit;
    
    // Execution state
    CommandStatus status;
    std::atomic<size_t> completedSimulations;
    std::atomic<size_t> failedSimulations;
    mutable std::mutex progressMutex;
    std::vector<BatchProgress> batchProgress;
    
    // Control
    std::atomic<bool> cancelled;
    std::future<void> cancelFuture;
    
    // Results
    std::vector<CommandResult> simulationResults;
    std::chrono::steady_clock::time_point startTime;
    
    Impl() : maxParallelSimulations(4), threadPoolSize(4), gpuAcceleration(false),
             performanceMode("balanced"), memoryLimitBytes(1024 * 1024 * 1024), // 1GB
             timeLimit(300), status(CommandStatus::Pending),
             completedSimulations(0), failedSimulations(0),
             cancelled(false), simulationResults() {}
    
    ~Impl() {
        if (cancelFuture.valid()) {
            cancelFuture.wait();
        }
    }
};

BatchCommand::BatchCommand() : pImpl(std::make_unique<Impl>()) {
}

BatchCommand::~BatchCommand() = default;

std::string BatchCommand::getName() const {
    return "batch";
}

std::string BatchCommand::getDescription() const {
    return "Execute multiple neural network simulations in batch mode with parallel processing";
}

std::string BatchCommand::getUsage() const {
    return "batch [options] [--config file1.json] [--file sim1.nnet] [--parallel N] [--mode balanced|fast|accurate]";
}

std::string BatchCommand::getHelp() const {
    return "Execute multiple neural network simulations in batch mode.\n"
           "Options:\n"
           "  --config file    Specify configuration file for simulation\n"
           "  --file file      Specify saved simulation file to load\n"
           "  --parallel N     Number of parallel simulations (default: 4)\n"
           "  --mode MODE      Performance mode: balanced, fast, or accurate (default: balanced)\n"
           "  --memory-limit N Memory limit in MB (default: 1024)\n"
           "  --time-limit N   Time limit per simulation in seconds (default: 300)\n"
           "Examples:\n"
           "  batch --config configs/neural_net.json --file sim1.nnet --parallel 8\n"
           "  batch --mode fast --memory-limit 2048\n";
}

CommandStatus BatchCommand::getStatus() const {
    return pImpl->status;
}

double BatchCommand::getProgress() const {
    if (pImpl->simulationConfigs.empty() && pImpl->simulationFiles.empty()) {
        return 0.0;
    }
    
    size_t total = pImpl->simulationConfigs.size() + pImpl->simulationFiles.size();
    size_t completed = pImpl->completedSimulations.load();
    
    return static_cast<double>(completed) / static_cast<double>(total);
}

std::string BatchCommand::getProgressText() const {
    return std::to_string(pImpl->completedSimulations.load()) + "/" +
           std::to_string(pImpl->simulationConfigs.size() + pImpl->simulationFiles.size()) +
           " simulations completed";
}

bool BatchCommand::execute() {
    setStatus(CommandStatus::Running);
    startTiming();
    pImpl->simulationResults.clear();
    pImpl->batchProgress.clear();
    pImpl->cancelled = false;
    pImpl->completedSimulations.store(0);
    pImpl->failedSimulations.store(0);
    
    // Validate arguments
    if (!validateArguments()) {
        setStatus(CommandStatus::Failed);
        return false;
    }
    
    // Prepare simulation list
    std::vector<std::pair<std::string, std::function<bool()>>> simulations;
    
    for (size_t i = 0; i < pImpl->simulationConfigs.size(); ++i) {
        const auto& config = pImpl->simulationConfigs[i];
        std::string name = pImpl->simulationNames.empty() ? "config_" + std::to_string(i) : pImpl->simulationNames[i];
        
        simulations.emplace_back(name, [this, i, config]() {
            return executeSingleSimulation(config, i);
        });
    }
    
    for (size_t i = 0; i < pImpl->simulationFiles.size(); ++i) {
        std::string filepath = pImpl->simulationFiles[i];
        std::string name = pImpl->simulationNames.empty() ? "file_" + std::to_string(i) : pImpl->simulationNames[i + pImpl->simulationConfigs.size()];
        
        simulations.emplace_back(name, [this, filepath, i]() {
            return executeSingleFileSimulation(filepath, i);
        });
    }
    
    if (simulations.empty()) {
        setStatus(CommandStatus::Completed);
        setResult(true, "No simulations to execute", "No configuration files or saved simulations specified");
        return false;
    }
    
    // Execute simulations based on performance mode
    if (pImpl->performanceMode == "fast") {
        executeFastMode(simulations);
    } else if (pImpl->performanceMode == "accurate") {
        executeAccurateMode(simulations);
    } else { // balanced
        executeBalancedMode(simulations);
    }
    
    stopTiming();
    
    // Generate summary
    std::ostringstream oss;
    oss << "Batch execution completed!\n";
    oss << "Total simulations: " << simulations.size() << "\n";
    oss << "Completed: " << pImpl->completedSimulations.load() << "\n";
    oss << "Failed: " << pImpl->failedSimulations.load() << "\n";
    oss << "Success rate: " << (simulations.empty() ? 0.0 : 
        static_cast<double>(pImpl->completedSimulations.load()) / simulations.size() * 100.0) << "%\n";
    oss << "Total execution time: " << result.executionTime.count() << " ms\n";
    
    setStatus(CommandStatus::Completed);
    setResult(true, oss.str(), "");
    return true;
}

bool BatchCommand::cancel() {
    pImpl->cancelled = true;
    return true;
}

bool BatchCommand::isCancellable() const {
    return pImpl->status == CommandStatus::Running;
}

CommandResult BatchCommand::getResult() const {
    return result;
}

std::vector<std::string> BatchCommand::getArguments() const {
    std::vector<std::string> args;
    for (const auto& config : pImpl->simulationConfigs) {
        args.push_back("--config");
        args.push_back("config_file"); // Placeholder
    }
    for (const auto& file : pImpl->simulationFiles) {
        args.push_back("--file");
        args.push_back(file);
    }
    return args;
}

void BatchCommand::setArguments(const std::vector<std::string>& args) {
    // Parse command line arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--config" && i + 1 < args.size()) {
            std::string configFile = args[i + 1];
            i++;
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(configFile)) {
                pImpl->simulationConfigs.push_back(config);
                pImpl->simulationNames.push_back("config_" + std::to_string(pImpl->simulationConfigs.size() - 1));
            }
        } else if (args[i] == "--file" && i + 1 < args.size()) {
            std::string filepath = args[i + 1];
            i++;
            pImpl->simulationFiles.push_back(filepath);
            pImpl->simulationNames.push_back("file_" + std::to_string(pImpl->simulationFiles.size() - 1));
        } else if (args[i] == "--parallel" && i + 1 < args.size()) {
            try {
                pImpl->maxParallelSimulations = std::stoul(args[i + 1]);
                i++;
            } catch (...) {
                // Invalid number, use default
            }
        } else if (args[i] == "--mode" && i + 1 < args.size()) {
            pImpl->performanceMode = args[i + 1];
            i++;
        } else if (args[i] == "--memory-limit" && i + 1 < args.size()) {
            try {
                pImpl->memoryLimitBytes = std::stoul(args[i + 1]) * 1024 * 1024; // Convert MB to bytes
                i++;
            } catch (...) {
                // Invalid number, use default
            }
        } else if (args[i] == "--time-limit" && i + 1 < args.size()) {
            try {
                pImpl->timeLimit = std::chrono::seconds(std::stol(args[i + 1]));
                i++;
            } catch (...) {
                // Invalid number, use default
            }
        }
    }
}

bool BatchCommand::validateArguments() const {
    if (pImpl->simulationConfigs.empty() && pImpl->simulationFiles.empty()) {
        return false; // No simulations to run
    }
    
    if (pImpl->maxParallelSimulations == 0) {
        return false;
    }
    
    if (pImpl->performanceMode != "balanced" && pImpl->performanceMode != "fast" && pImpl->performanceMode != "accurate") {
        return false;
    }
    
    return true;
}

std::string BatchCommand::getLastError() const {
    return result.errorMessage;
}

std::chrono::milliseconds BatchCommand::getEstimatedExecutionTime() const {
    // Estimate based on number of simulations and performance mode
    size_t totalSimulations = pImpl->simulationConfigs.size() + pImpl->simulationFiles.size();
    
    if (pImpl->performanceMode == "fast") {
        return std::chrono::milliseconds(totalSimulations * 1000); // Fast mode: 1 second per simulation
    } else if (pImpl->performanceMode == "accurate") {
        return std::chrono::milliseconds(totalSimulations * 5000); // Accurate mode: 5 seconds per simulation
    } else { // balanced
        return std::chrono::milliseconds(totalSimulations * 2000); // Balanced mode: 2 seconds per simulation
    }
}

BatchCommand::Category BatchCommand::getCategory() const {
    return Category::Processing;
}

bool BatchCommand::addSimulationConfig(const std::string& configName, const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    
    pImpl->simulationConfigs.push_back(config);
    pImpl->simulationNames.push_back(configName);
    return true;
}

bool BatchCommand::addSimulationFile(const std::string& filepath) {
    pImpl->simulationFiles.push_back(filepath);
    pImpl->simulationNames.push_back("file_" + std::to_string(pImpl->simulationFiles.size() - 1));
    return true;
}

bool BatchCommand::clearSimulations() {
    pImpl->simulationConfigs.clear();
    pImpl->simulationFiles.clear();
    pImpl->simulationNames.clear();
    return true;
}

std::vector<std::string> BatchCommand::getSimulationConfigs() const {
    std::vector<std::string> configFiles;
    for (const auto& config : pImpl->simulationConfigs) {
        // Note: This is a simplification - in a real implementation, we would store the file path
        configFiles.push_back("config_" + std::to_string(&config - &pImpl->simulationConfigs[0]));
    }
    return configFiles;
}

std::vector<std::string> BatchCommand::getSimulationFiles() const {
    return pImpl->simulationFiles;
}

void BatchCommand::setMaxParallelSimulations(size_t maxParallel) {
    pImpl->maxParallelSimulations = maxParallel;
}

void BatchCommand::setThreadPoolSize(size_t poolSize) {
    pImpl->threadPoolSize = poolSize;
}

void BatchCommand::enableGpuAcceleration(bool enable) {
    pImpl->gpuAcceleration = enable;
}

std::vector<BatchCommand::BatchProgress> BatchCommand::getBatchProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    return pImpl->batchProgress;
}

void BatchCommand::setPerformanceMode(const std::string& mode) {
    if (mode == "balanced" || mode == "fast" || mode == "accurate") {
        pImpl->performanceMode = mode;
    }
}

void BatchCommand::setMemoryLimit(size_t limitBytes) {
    pImpl->memoryLimitBytes = limitBytes;
}

void BatchCommand::setTimeLimit(std::chrono::seconds limit) {
    pImpl->timeLimit = limit;
}

// Private helper methods
bool BatchCommand::executeSingleSimulation(const std::shared_ptr<Config>& config, size_t index) {
    std::string name = pImpl->simulationNames[index];
    
    // Create a new Brain instance with the configuration
    auto brain = std::make_shared<Brain>(config);
    
    try {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Run a number of simulation steps
        size_t steps = config->getOr<size_t>("steps", 100);
        
        for (size_t step = 0; step < steps && !pImpl->cancelled; ++step) {
            brain->step(step);
            
            // Update progress
            {
                std::lock_guard<std::mutex> lock(pImpl->progressMutex);
                BatchProgress progress;
                progress.simulationName = name;
                progress.progress = static_cast<double>(step) / static_cast<double>(steps);
                progress.statusText = "Running simulation steps";
                progress.elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - pImpl->startTime);
                progress.simulationIndex = index;
                progress.totalSimulations = pImpl->simulationConfigs.size() + pImpl->simulationFiles.size();
                pImpl->batchProgress.push_back(progress);
            }
            
            // Check time limit
            auto elapsed = std::chrono::steady_clock::now() - pImpl->startTime;
            if (elapsed > pImpl->timeLimit) {
                throw std::runtime_error("Time limit exceeded");
            }
        }
        
        // Save the brain state
        std::string outputFile = name + ".nnet";
        if (brain->save(outputFile)) {
            pImpl->completedSimulations++;
            return true;
        } else {
            throw std::runtime_error("Failed to save brain state");
        }
        
    } catch (const std::exception& e) {
        pImpl->failedSimulations++;
        
        std::ostringstream oss;
        oss << "Simulation failed: " << e.what();
        setResult(false, "", oss.str());
        return false;
    }
    
    return true;
}

bool BatchCommand::executeSingleFileSimulation(const std::string& filepath, size_t index) {
    std::string name = pImpl->simulationNames[index];
    
    // Load brain from file
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    
    if (!brain->load(filepath)) {
        pImpl->failedSimulations++;
        return false;
    }
    
    try {
        // Run some additional steps
        size_t steps = 50;
        
        for (size_t step = 0; step < steps && !pImpl->cancelled; ++step) {
            brain->step(step);
            
            // Update progress
            {
                std::lock_guard<std::mutex> lock(pImpl->progressMutex);
                BatchProgress progress;
                progress.simulationName = name;
                progress.progress = static_cast<double>(step) / static_cast<double>(steps);
                progress.statusText = "Running loaded simulation";
                progress.elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - pImpl->startTime);
                progress.simulationIndex = index;
                progress.totalSimulations = pImpl->simulationConfigs.size() + pImpl->simulationFiles.size();
                pImpl->batchProgress.push_back(progress);
            }
        }
        
        // Save the brain state with a new name
        std::string outputFile = name + "_processed.nnet";
        if (brain->save(outputFile)) {
            pImpl->completedSimulations++;
            return true;
        } else {
            throw std::runtime_error("Failed to save processed brain state");
        }
        
    } catch (const std::exception& e) {
        pImpl->failedSimulations++;
        return false;
    }
    
    return true;
}

void BatchCommand::executeFastMode(const std::vector<std::pair<std::string, std::function<bool()>>>& simulations) {
    // Simplified fast mode: process as many simulations as possible in parallel
    std::vector<std::future<bool>> futures;
    
    for (const auto& sim : simulations) {
        if (pImpl->cancelled) break;
        
        // Submit simulation to thread pool
        futures.emplace_back(std::async(std::launch::async, sim.second));
        
        // Limit the number of concurrent futures
        if (futures.size() >= pImpl->maxParallelSimulations) {
            // Wait for some futures to complete
            for (auto it = futures.begin(); it != futures.end(); ) {
                if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                    it->get(); // Get result and discard
                    it = futures.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    
    // Wait for remaining futures
    for (auto& future : futures) {
        future.wait();
        future.get(); // Get result and discard
    }
}

void BatchCommand::executeAccurateMode(const std::vector<std::pair<std::string, std::function<bool()>>>& simulations) {
    // Simplified accurate mode: process one simulation at a time with more detailed progress
    for (const auto& sim : simulations) {
        if (pImpl->cancelled) break;
        
        if (!sim.second()) {
            // Simulation failed, but we continue with others
            continue;
        }
        
        // Small delay for accuracy
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void BatchCommand::executeBalancedMode(const std::vector<std::pair<std::string, std::function<bool()>>>& simulations) {
    // Balanced mode: moderate parallelism
    std::vector<std::future<bool>> futures;
    
    for (const auto& sim : simulations) {
        if (pImpl->cancelled) break;
        
        // Submit simulation to thread pool
        futures.emplace_back(std::async(std::launch::async, sim.second));
        
        // Wait if we have too many concurrent futures
        if (futures.size() >= pImpl->maxParallelSimulations * 0.75) { // 75% of max
            for (auto it = futures.begin(); it != futures.end(); ) {
                if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                    it->get(); // Get result and discard
                    it = futures.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    
    // Wait for remaining futures
    for (auto& future : futures) {
        future.wait();
        future.get(); // Get result and discard
    }
}

} // namespace nlm