// Utility functions for enhanced NLM simulation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Performance monitoring utilities
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    double simulationTime;
    double wallTimePerStep;
    size_t stepsCompleted;
    std::map<std::string, double> customMetrics;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), simulationTime(0.0), 
                          wallTimePerStep(0.0), stepsCompleted(0) {}
};

class PerformanceMonitor {
private:
    PerformanceMetrics metrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<double> stepTimes_;
    
public:
    PerformanceMonitor() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void recordStep(double stepTime) {
        stepTimes_.push_back(stepTime);
        if (stepTimes_.size() > 1000) {
            stepTimes_.erase(stepTimes_.begin());
        }
    }
    
    PerformanceMetrics getMetrics() {
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics_.simulationTime = std::chrono::duration<double>(endTime - startTime_).count();
        metrics_.stepsCompleted = stepTimes_.size();
        
        if (!stepTimes_.empty()) {
            double sum = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0);
            metrics_.wallTimePerStep = sum / stepTimes_.size();
        }
        
        return metrics_;
    }
    
    void addCustomMetric(const std::string& name, double value) {
        metrics_.customMetrics[name] = value;
    }
};

// Experiment runner for managing multiple simulations
class ExperimentRunner {
private:
    struct ExperimentResult {
        std::string name;
        double totalReward;
        double avgFiringRate;
        double finalCuriosity;
        double finalNovelty;
        double totalTime;
        std::map<std::string, double> metrics;
        bool success;
        std::string errorMessage;
    };
    
    std::vector<ExperimentResult> results_;
    
public:
    void runExperiment(const std::string& name, 
                       std::function<void(std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>)> experimentFunc) {
        ExperimentResult result;
        result.name = name;
        result.success = true;
        
        try {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Create basic components
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10, 10, 8, 8);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Run experiment
            experimentFunc(brain, agent, world);
            
            // Collect results
            result.totalReward = 0.0; // Would need to be tracked in experiment
            result.avgFiringRate = brain->getAverageFiringRate();
            result.finalCuriosity = agent->getCuriosityLevel();
            result.finalNovelty = agent->getNoveltyLevel();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
        }
        
        results_.push_back(result);
    }
    
    std::vector<ExperimentResult> getResults() const {
        return results_;
    }
    
    void printSummary() const {
        std::cout << "Experiment Results Summary:" << std::endl;
        std::cout << "===========================" << std::endl;
        
        for (const auto& result : results_) {
            std::cout << "Experiment: " << result.name << std::endl;
            std::cout << "  Success: " << (result.success ? "Yes" : "No") << std::endl;
            if (result.success) {
                std::cout << "  Total Reward: " << result.totalReward << std::endl;
                std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
                std::cout << "  Final Curiosity: " << result.finalCuriosity << std::endl;
                std::cout << "  Final Novelty: " << result.finalNovelty << std::endl;
                std::cout << "  Total Time: " << result.totalTime << "s" << std::endl;
            } else {
                std::cout << "  Error: " << result.errorMessage << std::endl;
            }
            std::cout << std::endl;
        }
    }
};

// Data analysis utilities
class DataAnalyzer {
public:
    struct TimeSeriesData {
        std::vector<double> timestamps;
        std::vector<double> values;
        std::string name;
    };
    
    // Basic statistics
    struct Statistics {
        double mean;
        double median;
        double mode;
        double stdDev;
        double variance;
        double min;
        double max;
        double range;
        size_t count;
    };
    
    // Calculate basic statistics
    static Statistics calculateStats(const std::vector<double>& data) {
        Statistics stats;
        if (data.empty()) {
            return stats;
        }
        
        stats.count = data.size();
        stats.min = *std::min_element(data.begin(), data.end());
        stats.max = *std::max_element(data.begin(), data.end());
        stats.range = stats.max - stats.min;
        
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        stats.mean = sum / stats.count;
        
        // Median
        std::vector<double> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        if (stats.count % 2 == 0) {
            stats.median = (sorted[stats.count/2 - 1] + sorted[stats.count/2]) / 2.0;
        } else {
            stats.median = sorted[stats.count/2];
        }
        
        // Mode (simplified - first mode found)
        std::map<double, int> frequency;
        for (double val : data) {
            frequency[val]++;
        }
        int maxFreq = 0;
        for (const auto& pair : frequency) {
            if (pair.second > maxFreq) {
                maxFreq = pair.second;
                stats.mode = pair.first;
            }
        }
        
        // Standard deviation
        double sumSq = 0.0;
        for (double val : data) {
            sumSq += (val - stats.mean) * (val - stats.mean);
        }
        stats.variance = sumSq / stats.count;
        stats.stdDev = std::sqrt(stats.variance);
        
        return stats;
    }
    
    // Detect anomalies using simple statistical method
    static std::vector<size_t> detectAnomalies(const std::vector<double>& data, double zThreshold = 2.0) {
        std::vector<size_t> anomalies;
        Statistics stats = calculateStats(data);
        
        if (stats.stdDev == 0.0) {
            return anomalies; // No variation
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            double zScore = std::abs((data[i] - stats.mean) / stats.stdDev);
            if (zScore > zThreshold) {
                anomalies.push_back(i);
            }
        }
        
        return anomalies;
    }
    
    // Smooth time series using moving average
    static std::vector<double> smoothTimeSeries(const std::vector<double>& data, size_t windowSize) {
        std::vector<double> smoothed;
        if (data.empty() || windowSize == 0) {
            return smoothed;
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            size_t start = std::max(0ULL, i - windowSize/2);
            size_t end = std::min(data.size(), i + windowSize/2 + 1);
            
            double sum = 0.0;
            size_t count = 0;
            for (size_t j = start; j < end; ++j) {
                sum += data[j];
                count++;
            }
            
            smoothed.push_back(sum / count);
        }
        
        return smoothed;
    }
    
    // Find peaks in time series
    static std::vector<size_t> findPeaks(const std::vector<double>& data, double threshold = 0.0) {
        std::vector<size_t> peaks;
        if (data.size() < 3) {
            return peaks;
        }
        
        for (size_t i = 1; i < data.size() - 1; ++i) {
            if (data[i] > data[i-1] && data[i] > data[i+1] && data[i] > threshold) {
                peaks.push_back(i);
            }
        }
        
        return peaks;
    }
};

// Progress tracking utilities
class ProgressTracker {
private:
    std::string message_;
    size_t totalSteps_;
    size_t currentStep_ = 0;
    double startTime_;
    bool showProgress_ = true;
    
public:
    ProgressTracker(const std::string& message, size_t totalSteps) 
        : message_(message), totalSteps_(totalSteps) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void setMessage(const std::string& message) {
        message_ = message;
    }
    
    void increment() {
        currentStep_++;
        if (showProgress_ && currentStep_ % 100 == 0) {
            double elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime_).count();
            double estimatedTotal = elapsed * (totalSteps_ / (double)currentStep_);
            double remaining = estimatedTotal - elapsed;
            
            std::cout << message_ << " " << currentStep_ << "/" << totalSteps_ 
                      << " (" << (100.0 * currentStep_ / totalSteps_) << "%) "
                      << "Remaining: " << remaining << "s" << std::endl;
        }
    }
    
    void showFinalProgress() {
        if (!showProgress_) return;
        
        double elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime_).count();
        std::cout << message_ << " " << currentStep_ << "/" << totalSteps_ 
                  << " (100.0%) " << "Completed in " << elapsed << "s" << std::endl;
    }
    
    void enableProgress(bool enable) {
        showProgress_ = enable;
    }
};

// Simulation runner with enhanced features
class EnhancedSimulationRunner {
public:
    struct SimulationConfig {
        uint64_t steps;
        double timestep;
        std::string name;
        bool enableProgress;
        bool enableProfiling;
        bool recordData;
        std::function<bool(uint64_t)> stepCondition;
        std::function<void(uint64_t)> stepCallback;
    };
    
    struct SimulationResult {
        double totalReward;
        double avgFiringRate;
        double finalCuriosity;
        double finalNovelty;
        double simulationTime;
        size_t stepsCompleted;
        bool success;
        std::string errorMessage;
        std::vector<double> firingHistory;
        std::vector<double> rewardHistory;
        std::vector<double> curiosityHistory;
        std::vector<double> noveltyHistory;
    };
    
    static SimulationResult runSimulation(
        const SimulationConfig& config,
        std::function<void(uint64_t step, Timestamp time)> simulationStep,
        std::function<void()> initialization = nullptr,
        std::function<double(uint64_t step)> rewardFunction = nullptr) {
        
        SimulationResult result;
        result.success = true;
        result.stepsCompleted = 0;
        
        try {
            PerformanceMonitor monitor;
            ProgressTracker progress(config.name, config.steps);
            progress.enableProgress(config.enableProgress);
            
            if (initialization) initialization();
            
            double totalReward = 0.0;
            std::vector<double> firingHistory, rewardHistory, curiosityHistory, noveltyHistory;
            
            for (uint64_t step = 0; step < config.steps; ++step) {
                // Check step condition
                if (config.stepCondition && !config.stepCondition(step)) {
                    result.stepsCompleted = step;
                    break;
                }
                
                // Run simulation step
                auto stepStart = std::chrono::high_resolution_clock::now();
                simulationStep(step, step * config.timestep);
                auto stepEnd = std::chrono::high_resolution_clock::now();
                
                double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
                monitor.recordStep(stepTime);
                
                // Record data if enabled
                if (config.recordData) {
                    // This would need to capture data from the simulation
                    // For now, this is a placeholder
                }
                
                // Call step callback if provided
                if (config.stepCallback) {
                    config.stepCallback(step);
                }
                
                // Record reward if function provided
                if (rewardFunction) {
                    double reward = rewardFunction(step);
                    totalReward += reward;
                }
                
                progress.increment();
            }
            
            result.totalReward = totalReward;
            result.stepsCompleted = config.steps;
            result.firingHistory = firingHistory;
            result.rewardHistory = rewardHistory;
            result.curiosityHistory = curiosityHistory;
            result.noveltyHistory = noveltyHistory;
            
            auto metrics = monitor.getMetrics();
            result.simulationTime = metrics.simulationTime;
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
        }
        
        return result;
    }
};

// Convenience functions for common simulation patterns
namespace SimUtils {
    
    // Simple agent simulation
    EnhancedSimulationRunner::SimulationResult runAgentSimulation(
        const EnhancedSimulationRunner::SimulationConfig& config,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world) {
        
        auto simulationStep = [&](uint64_t step, Timestamp time) {
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step, time);
            
            // Get action and apply
            MotorCommand cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, time);
            
            // Apply reward modulation
            float reward = 0.0f;
            auto internal = percept.getInternal();
            if (internal && !internal->empty()) {
                reward = (*internal)[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(config.timestep);
        };
        
        auto initialization = [&]() {
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        };
        
        return EnhancedSimulationRunner::runSimulation(config, simulationStep, initialization);
    }
    
    // Simple world simulation
    EnhancedSimulationRunner::SimulationResult runWorldSimulation(
        const EnhancedSimulationRunner::SimulationConfig& config,
        std::shared_ptr<SimpleWorld> world) {
        
        auto simulationStep = [&](uint64_t step, Timestamp time) {
            world->update(time);
        };
        
        return EnhancedSimulationRunner::runSimulation(config, simulationStep);
    }
    
    // Memory profiling simulation
    EnhancedSimulationRunner::SimulationResult runMemoryProfilingSimulation(
        const EnhancedSimulationRunner::SimulationConfig& config,
        std::shared_ptr<Brain> brain) {
        
        auto simulationStep = [&](uint64_t step, Timestamp time) {
            brain->step(step, time);
            
            // Record memory usage
            if (step % 100 == 0) {
                // This would use platform-specific memory tracking
            }
        };
        
        return EnhancedSimulationRunner::runSimulation(config, simulationStep);
    }
}

// Output and visualization utilities
namespace OutputUtils {
    
    // Print simulation results
    void printSimulationResults(const EnhancedSimulationRunner::SimulationResult& result) {
        std::cout << "Simulation Results:" << std::endl;
        std::cout << "===================" << std::endl;
        std::cout << "Success: " << (result.success ? "Yes" : "No") << std::endl;
        if (!result.success) {
            std::cout << "Error: " << result.errorMessage << std::endl;
            return;
        }
        
        std::cout << "Total Reward: " << result.totalReward << std::endl;
        std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "Final Curiosity: " << result.finalCuriosity << std::endl;
        std::cout << "Final Novelty: " << result.finalNovelty << std::endl;
        std::cout << "Simulation Time: " << result.simulationTime << "s" << std::endl;
        std::cout << "Steps Completed: " << result.stepsCompleted << std::endl;
        
        if (!result.firingHistory.empty()) {
            double sum = std::accumulate(result.firingHistory.begin(), result.firingHistory.end(), 0.0);
            double avg = sum / result.firingHistory.size();
            std::cout << "Average Firing (recorded): " << avg << std::endl;
        }
    }
    
    // Save results to file
    bool saveResults(const EnhancedSimulationRunner::SimulationResult& result, const std::string& filename) {
        // In practice, would save using CSV, JSON, or binary format
        std::cout << "Results would be saved to: " << filename << std::endl;
        return true;
    }
    
    // Plot results (would use matplotlib, etc.)
    void plotResults(const EnhancedSimulationRunner::SimulationResult& result) {
        // In practice, would create plots using external libraries
        std::cout << "Plotting results would be implemented with plotting libraries" << std::endl;
    }
}

PYBIND11_MODULE(pynlm_enhanced_utils, m) {
    m.doc() = R"pbdoc(
        NLM Enhanced Utilities
        ----------------------
        Additional utilities for enhanced NLM simulation workflows.
        
        Includes:
        - Performance monitoring
        - Experiment running and management
        - Data analysis tools
        - Progress tracking
        - Simulation result processing
    )pbdoc";

    // Performance Monitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("recordStep", &PerformanceMonitor::recordStep, py::arg("stepTime"),
             "Record the duration of a simulation step")
        .def("getMetrics", &PerformanceMonitor::getMetrics,
             "Get performance metrics")
        .def("addCustomMetric", &PerformanceMonitor::addCustomMetric, 
             py::arg("name"), py::arg("value"),
             "Add a custom performance metric");

    // Experiment Runner
    py::class_<ExperimentRunner>(m, "ExperimentRunner")
        .def(py::init<>())
        .def("runExperiment", &ExperimentRunner::runExperiment,
             py::arg("name"), py::arg("experimentFunc"),
             "Run an experiment")
        .def("getResults", &ExperimentRunner::getResults,
             "Get experiment results")
        .def("printSummary", &ExperimentRunner::printSummary,
             "Print summary of all experiments");

    // Data Analyzer
    py::class_<DataAnalyzer>(m, "DataAnalyzer")
        .def_static("calculateStats", &DataAnalyzer::calculateStats, py::arg("data"),
                   "Calculate basic statistics from data")
        .def_static("detectAnomalies", &DataAnalyzer::detectAnomalies, py::arg("data"), py::arg("zThreshold") = 2.0,
                   "Detect anomalies in time series data")
        .def_static("smoothTimeSeries", &DataAnalyzer::smoothTimeSeries, py::arg("data"), py::arg("windowSize"),
                   "Smooth time series data using moving average")
        .def_static("findPeaks", &DataAnalyzer::findPeaks, py::arg("data"), py::arg("threshold") = 0.0,
                   "Find peaks in time series data");

    // Progress Tracker
    py::class_<ProgressTracker>(m, "ProgressTracker")
        .def(py::init<std::string, size_t>(), py::arg("message"), py::arg("totalSteps"),
             "Create a progress tracker")
        .def("setMessage", &ProgressTracker::setMessage, py::arg("message"),
             "Set the progress message")
        .def("increment", &ProgressTracker::increment,
             "Increment progress")
        .def("showFinalProgress", &ProgressTracker::showFinalProgress,
             "Show final progress")
        .def("enableProgress", &ProgressTracker::enableProgress, py::arg("enable"),
             "Enable or disable progress display");

    // Enhanced Simulation Runner
    py::class_<EnhancedSimulationRunner>(m, "EnhancedSimulationRunner")
        .def_static("runSimulation", &EnhancedSimulationRunner::runSimulation,
                   py::arg("config"), py::arg("simulationStep"), py::arg("initialization") = nullptr,
                   py::arg("rewardFunction") = nullptr,
                   "Run a simulation with enhanced features");

    // Simulation Config
    py::class_<EnhancedSimulationRunner::SimulationConfig>(m, "SimulationConfig")
        .def(py::init<>())
        .def_readwrite("steps", &EnhancedSimulationRunner::SimulationConfig::steps)
        .def_readwrite("timestep", &EnhancedSimulationRunner::SimulationConfig::timestep)
        .def_readwrite("name", &EnhancedSimulationRunner::SimulationConfig::name)
        .def_readwrite("enableProgress", &EnhancedSimulationRunner::SimulationConfig::enableProgress)
        .def_readwrite("enableProfiling", &EnhancedSimulationRunner::SimulationConfig::enableProfiling)
        .def_readwrite("recordData", &EnhancedSimulationRunner::SimulationConfig::recordData);

    // Simulation Result
    py::class_<EnhancedSimulationRunner::SimulationResult>(m, "SimulationResult")
        .def_readonly("totalReward", &EnhancedSimulationRunner::SimulationResult::totalReward)
        .def_readonly("avgFiringRate", &EnhancedSimulationRunner::SimulationResult::avgFiringRate)
        .def_readonly("finalCuriosity", &EnhancedSimulationRunner::SimulationResult::finalCuriosity)
        .def_readonly("finalNovelty", &EnhancedSimulationRunner::SimulationResult::finalNovelty)
        .def_readonly("simulationTime", &EnhancedSimulationRunner::SimulationResult::simulationTime)
        .def_readonly("stepsCompleted", &EnhancedSimulationRunner::SimulationResult::stepsCompleted)
        .def_readonly("success", &EnhancedSimulationRunner::SimulationResult::success)
        .def_readonly("errorMessage", &EnhancedSimulationRunner::SimulationResult::errorMessage);

    // SimUtils namespace
    m.def("runAgentSimulation", &SimUtils::runAgentSimulation, 
          py::arg("config"), py::arg("brain"), py::arg("agent"), py::arg("world"),
          "Run an agent simulation")
      .def("runWorldSimulation", &SimUtils::runWorldSimulation,
           py::arg("config"), py::arg("world"),
           "Run a world simulation")
      .def("runMemoryProfilingSimulation", &SimUtils::runMemoryProfilingSimulation,
           py::arg("config"), py::arg("brain"),
           "Run a memory profiling simulation");

    // OutputUtils namespace
    m.def("printSimulationResults", &OutputUtils::printSimulationResults,
          py::arg("result"),
          "Print simulation results")
      .def("saveResults", &OutputUtils::saveResults,
           py::arg("result"), py::arg("filename"),
           "Save simulation results to file")
      .def("plotResults", &OutputUtils::plotResults,
           py::arg("result"),
           "Plot simulation results");
}

} // namespace nlm
