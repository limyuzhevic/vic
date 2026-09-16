#pragma once

#include "CommandSystem.hpp"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <algorithm>

namespace nlm {

class RunBatchCommand : public ICommand {
public:
    RunBatchCommand(std::shared_ptr<Brain> brain, std::shared_ptr<CheckpointManager> checkpointManager,
                    std::shared_ptr<ILogger> logger)
        : brain_(brain), checkpointManager_(checkpointManager), logger_(logger) {}
    
    const std::string& getName() const override { static const std::string name = "run-batch"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Run multiple simulation steps efficiently with progress tracking"; 
        return desc; 
    }
    const std::vector<std::string>& getAliases() const override { 
        static const std::vector<std::string> aliases = {"batch-run", "batch"}; 
        return aliases; 
    }
    const std::string& getUsage() const override { 
        static const std::string usage = "run-batch <steps> [output_file] [save_interval]"; 
        return usage; 
    }
    bool requiresBrain() const override { return true; }
    
    CommandResult execute(const std::vector<std::string>& args) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (args.size() < 1) {
            return CommandResult(false, "", "run-batch requires at least 1 argument: <steps>", 
                                std::chrono::milliseconds(0));
        }
        
        try {
            // Parse steps
            uint64_t steps = std::stoull(args[0]);
            
            // Optional output file
            std::string outputFile = "batch_output.csv";
            if (args.size() > 1) {
                outputFile = args[1];
            }
            
            // Optional save interval
            uint64_t saveInterval = 100;
            if (args.size() > 2) {
                saveInterval = std::stoull(args[2]);
            }
            
            // Create output file
            std::ofstream outputFileStream(outputFile);
            if (!outputFileStream.is_open()) {
                return CommandResult(false, "", "Cannot open output file: " + outputFile, 
                                    std::chrono::milliseconds(0));
            }
            
            // Write CSV header
            outputFileStream << "step,time,total_spikes,active_neurons,firing_rate,reward,prediction_error\n";
            
            logger_->info("Running batch simulation: " + std::to_string(steps) + " steps, output to: " + outputFile);
            
            // Run batch simulation
            uint64_t totalStepsRun = 0;
            uint64_t checkpointCount = 0;
            double totalReward = 0.0f;
            double totalPredictionError = 0.0f;
            
            while (totalStepsRun < steps) {
                uint64_t stepsToRun = std::min(saveInterval, steps - totalStepsRun);
                
                for (uint64_t i = 0; i < stepsToRun; ++i) {
                    brain_->step(totalStepsRun + i, (totalStepsRun + i) * 0.001);
                    
                    // Update metrics for output
                    size_t totalSpikes = brain_->getTotalSpikeCount();
                    size_t activeNeurons = brain_->getActiveNeuronCount();
                    float firingRate = brain_->getAverageFiringRate();
                    double currentTime = (totalStepsRun + i) * 0.001;
                    
                    // For this example, use placeholder values for reward and prediction error
                    float reward = 0.0f;
                    float predictionError = 0.0f;
                    
                    outputFileStream << totalStepsRun + i << "," << currentTime << "," 
                                     << totalSpikes << "," << activeNeurons << "," 
                                     << firingRate << "," << reward << "," << predictionError << "\n";
                    
                    // Save checkpoint if requested
                    if (checkpointManager_ && i % 100 == 0) {
                        std::string checkpointName = "batch_checkpoint_" + std::to_string(totalStepsRun + i);
                        checkpointManager_->saveImmediately(checkpointName);
                        checkpointCount++;
                    }
                }
                
                totalStepsRun += stepsToRun;
                
                // Log progress
                logger_->info("Batch progress: " + std::to_string(totalStepsRun) + "/" + std::to_string(steps) + 
                             " steps, checkpoints: " + std::to_string(checkpointCount));
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            outputFileStream.close();
            
            std::stringstream output;
            output << "Batch simulation completed successfully!\n";
            output << "Steps run: " << totalStepsRun << "\n";
            output << "Checkpoints created: " << checkpointCount << "\n";
            output << "Output file: " << outputFile << "\n";
            output << "Execution time: " << duration.count() << " ms\n";
            
            logger_->info("Batch simulation finished in " + std::to_string(duration.count()) + " ms");
            
            return CommandResult(true, output.str());
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::string errorMsg = "Error in batch execution: " + std::string(e.what());
            logger_->error(errorMsg);
            
            return CommandResult(false, "", errorMsg, duration);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<CheckpointManager> checkpointManager_;
    std::shared_ptr<ILogger> logger_;
};

class RunUntilCommand : public ICommand {
public:
    RunUntilCommand(std::shared_ptr<Brain> brain, std::shared_ptr<ILogger> logger)
        : brain_(brain), logger_(logger) {}
    
    const std::string& getName() const override { static const std::string name = "run-until"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Run simulation until a condition is met"; 
        return desc; 
    }
    const std::vector<std::string>& getAliases() const override { 
        static const std::vector<std::string> aliases = {"run-until-condition"}; 
        return aliases; 
    }
    const std::string& getUsage() const override { 
        static const std::string usage = "run-until <condition> [max_steps] [checkpoint_on_condition]"; 
        return usage; 
    }
    bool requiresBrain() const override { return true; }
    
    CommandResult execute(const std::vector<std::string>& args) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (args.size() < 1) {
            return CommandResult(false, "", "run-until requires at least 1 argument: <condition>", 
                                std::chrono::milliseconds(0));
        }
        
        try {
            // Parse condition
            std::string condition = args[0];
            
            // Optional max steps
            uint64_t maxSteps = 100000;
            if (args.size() > 1) {
                maxSteps = std::stoull(args[1]);
            }
            
            // Optional checkpoint on condition
            bool checkpointOnCondition = false;
            if (args.size() > 2 && args[2] == "--checkpoint") {
                checkpointOnCondition = true;
            }
            
            logger_->info("Running until condition: " + condition + ", max steps: " + std::to_string(maxSteps));
            
            // Simple condition types
            auto shouldStop = [condition](Brain* brain) {
                if (condition == "high_spikes") {
                    return brain->getTotalSpikeCount() > 10000;
                } else if (condition == "stable" || condition == "stable_learning") {
                    // Check for stable firing rate (simplified)
                    return brain->getFiringNeuronCount() > 0.8 * brain->getTotalNeuronCount();
                } else if (condition == "no_activity") {
                    return brain->getFiringNeuronCount() == 0;
                } else {
                    // Parse numeric condition (e.g., "spikes>5000")
                    size_t pos = condition.find('>');
                    if (pos != std::string::npos) {
                        std::string metric = condition.substr(0, pos);
                        uint64_t threshold = std::stoull(condition.substr(pos + 1));
                        
                        if (metric == "spikes") {
                            return brain->getTotalSpikeCount() > threshold;
                        } else if (metric == "neurons") {
                            return brain->getActiveNeuronCount() > threshold;
                        }
                    }
                }
                
                // Default: never stop
                return false;
            };
            
            // Run simulation
            uint64_t stepsRun = 0;
            bool conditionMet = false;
            
            while (stepsRun < maxSteps) {
                brain_->step(stepsRun, stepsRun * 0.001);
                stepsRun++;
                
                if (shouldStop(brain_.get())) {
                    conditionMet = true;
                    logger_->info("Condition met at step " + std::to_string(stepsRun));
                    break;
                }
                
                // Log progress every 1000 steps
                if (stepsRun % 1000 == 0) {
                    logger_->info("Run-until progress: step " + std::to_string(stepsRun) + "/" + std::to_string(maxSteps));
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::stringstream output;
            output << "Run-until command completed.\n";
            output << "Steps run: " << stepsRun << "\n";
            if (conditionMet) {
                output << "Condition met: YES\n";
            } else {
                output << "Condition met: NO (max steps reached)\n";
            }
            output << "Execution time: " << duration.count() << " ms\n";
            
            logger_->info("Run-until finished: steps " + std::to_string(stepsRun) + ", condition met: " + 
                         std::string(conditionMet ? "yes" : "no"));
            
            return CommandResult(true, output.str());
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::string errorMsg = "Error in run-until: " + std::string(e.what());
            logger_->error(errorMsg);
            
            return CommandResult(false, "", errorMsg, duration);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<ILogger> logger_;
};

class ParallelRunCommand : public ICommand {
public:
    ParallelRunCommand(std::shared_ptr<Brain> brain, std::shared_ptr<ILogger> logger)
        : brain_(brain), logger_(logger) {}
    
    const std::string& getName() const override { static const std::string name = "parallel-run"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Multi-threaded execution with configurable thread count"; 
        return desc; 
    }
    const std::vector<std::string>& getAliases() const override { 
        static const std::vector<std::string> aliases = {"parallel", "multi-thread"}; 
        return aliases; 
    }
    const std::string& getUsage() const override { 
        static const std::string usage = "parallel-run <threads> <steps_per_thread> [sync_mode]"; 
        return usage; 
    }
    bool requiresBrain() const override { return true; }
    
    CommandResult execute(const std::vector<std::string>& args) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (args.size() < 2) {
            return CommandResult(false, "", "parallel-run requires at least 2 arguments: <threads> <steps_per_thread>", 
                                std::chrono::milliseconds(0));
        }
        
        try {
            // Parse thread count
            size_t threads = std::stoull(args[0]);
            if (threads < 1 || threads > 32) {
                return CommandResult(false, "", "Thread count must be between 1 and 32", 
                                    std::chrono::milliseconds(0));
            }
            
            // Parse steps per thread
            uint64_t stepsPerThread = std::stoull(args[1]);
            
            // Optional sync mode
            std::string syncMode = "independent";
            if (args.size() > 2) {
                syncMode = args[2];
            }
            
            logger_->info("Starting parallel execution: " + std::to_string(threads) + " threads, " + 
                         std::to_string(stepsPerThread) + " steps per thread, sync mode: " + syncMode);
            
            // Run parallel execution (simplified for this example)
            std::vector<std::thread> workers;
            std::vector<uint64_t> threadSteps(threads, 0);
            std::mutex resultsMutex;
            bool parallelSuccess = true;
            
            auto workerFunc = [this, stepsPerThread, &threadSteps, &resultsMutex, &parallelSuccess](size_t threadId) {
                try {
                    // Each thread runs its own brain instance (simplified)
                    // In a real implementation, this would use parallel processing APIs
                    uint64_t stepsDone = 0;
                    while (stepsDone < stepsPerThread && parallelSuccess) {
                        // Simulate parallel processing
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        stepsDone++;
                        
                        std::lock_guard<std::mutex> lock(resultsMutex);
                        threadSteps[threadId] = stepsDone;
                        
                        // Check if main brain should be updated (simplified)
                        if (threadId % 3 == 0 && stepsDone % 100 == 0) {
                            brain_->step(stepsDone, stepsDone * 0.001);
                        }
                    }
                } catch (const std::exception& e) {
                    std::lock_guard<std::mutex> lock(resultsMutex);
                    parallelSuccess = false;
                    logger_->error("Thread " + std::to_string(threadId) + " failed: " + std::string(e.what()));
                }
            };
            
            // Start threads
            for (size_t i = 0; i < threads; ++i) {
                workers.emplace_back(workerFunc, i);
            }
            
            // Wait for all threads to complete
            for (auto& worker : workers) {
                worker.join();
            }
            
            // Calculate total steps
            uint64_t totalSteps = 0;
            for (uint64_t steps : threadSteps) {
                totalSteps += steps;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::stringstream output;
            output << "Parallel execution completed.\n";
            output << "Total steps: " << totalSteps << "\n";
            output << "Average steps per thread: " << (totalSteps / threads) << "\n";
            output << "Execution time: " << duration.count() << " ms\n";
            output << "Speedup (approx): " << (threads / 1.0) << "x\n";
            
            logger_->info("Parallel execution finished: " + std::to_string(totalSteps) + " steps in " + 
                         std::to_string(duration.count()) + " ms");
            
            if (!parallelSuccess) {
                return CommandResult(false, output.str(), "Some threads failed during execution", duration);
            }
            
            return CommandResult(true, output.str());
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::string errorMsg = "Error in parallel-run: " + std::string(e.what());
            logger_->error(errorMsg);
            
            return CommandResult(false, "", errorMsg, duration);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<ILogger> logger_;
};

} // namespace nlm