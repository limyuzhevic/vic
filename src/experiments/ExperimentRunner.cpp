#include "ExperimentRunner.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace nlm {

// PerformanceMonitor implementation
struct PerformanceMonitor::Impl {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    bool isMonitoring = false;
    double monitoringInterval = 0.01; // 10ms default
    
    // Performance metrics
    PerformanceMetrics metrics;
    
    // Metric collection flags
    std::unordered_map<std::string, bool> metricCollectionEnabled;
    
    // Event and timing data
    std::unordered_map<std::string, std::vector<double>> eventTimings;
    std::unordered_map<std::string, size_t> eventCounts;
    
    // Performance snapshots
    std::vector<std::pair<double, double>> snapshots;
    
    Impl() {
        // Initialize metric collection flags
        metricCollectionEnabled["timing"] = true;
        metricCollectionEnabled["memory"] = true;
        metricCollectionEnabled["cpu"] = true;
        metricCollectionEnabled["spikes"] = true;
        metricCollectionEnabled["neuron_activity"] = true;
        metricCollectionEnabled["synapse_activity"] = true;
        metricCollectionEnabled["events"] = true;
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(new Impl) {}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startMonitoring() {
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->isMonitoring = true;
    pImpl->metrics.reset();
    pImpl->eventTimings.clear();
    pImpl->eventCounts.clear();
    pImpl->snapshots.clear();
}

void PerformanceMonitor::stopMonitoring() {
    if (pImpl->isMonitoring) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->startTime);
        pImpl->metrics.totalSimulationTime = duration.count();
        pImpl->isMonitoring = false;
    }
}

void PerformanceMonitor::recordSnapshot(const std::string& label) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->startTime);
    
    double value = 0.0;
    if (label == "steps_per_second" && pImpl->metrics.totalSimulationTime > 0) {
        value = pImpl->metrics.stepsPerSecond;
    } else if (label == "memory_usage") {
        value = pImpl->metrics.memoryUsageMB;
    } else if (label == "spike_rate") {
        value = pImpl->metrics.averageSpikeRate;
    }
    
    pImpl->snapshots.push_back(std::make_pair(duration.count(), value));
}

void PerformanceMonitor::recordEvent(const std::string& eventName, double duration) {
    if (pImpl->metricCollectionEnabled["events"]) {
        pImpl->eventTimings[eventName].push_back(duration);
        pImpl->eventCounts[eventName]++;
    }
}

void PerformanceMonitor::recordSimulationMetrics(std::shared_ptr<Brain> brain, 
                                                  SimulationStep totalSteps,
                                                  double simulationTime) {
    if (!brain) return;
    
    pImpl->metrics.simulationSetupTime = simulationTime;
    pImpl->metrics.brainInitializationTime = simulationTime * 0.1; // Estimate
    pImpl->metrics.experimentRunTime = simulationTime * 0.8; // Estimate
    pImpl->metrics.cleanupTime = simulationTime * 0.1; // Estimate
    
    if (simulationTime > 0) {
        pImpl->metrics.stepsPerSecond = totalSteps / simulationTime;
        pImpl->metrics.neuronsPerSecond = brain->getTotalNeuronCount() / simulationTime;
        pImpl->metrics.synapsesPerSecond = brain->getTotalSynapseCount() / simulationTime;
        pImpl->metrics.averageSpikeRate = brain->getTotalSpikeCount() / 
                                         (brain->getTotalNeuronCount() * simulationTime);
    }
    
    recordSnapshot("steps_per_second");
    recordSnapshot("spike_rate");
}

PerformanceMetrics PerformanceMonitor::getMetrics() const {
    return pImpl->metrics;
}

std::string PerformanceMonitor::getPerformanceSummary() const {
    std::ostringstream oss;
    oss << "=== Performance Monitor Summary ===" << std::endl;
    oss << "Total simulation time: " << pImpl->metrics.totalSimulationTime << " seconds" << std::endl;
    oss << "Steps per second: " << pImpl->metrics.stepsPerSecond << std::endl;
    oss << "Neurons processed per second: " << pImpl->metrics.neuronsPerSecond << std::endl;
    oss << "Synapses processed per second: " << pImpl->metrics.synapsesPerSecond << std::endl;
    oss << "Average spike rate: " << pImpl->metrics.averageSpikeRate << " spikes/neuron/sec" << std::endl;
    oss << "Memory usage: " << pImpl->metrics.memoryUsageMB << " MB" << std::endl;
    
    if (!pImpl->eventTimings.empty()) {
        oss << std::endl << "Event Timings:" << std::endl;
        for (const auto& pair : pImpl->eventTimings) {
            double avgTime = 0.0;
            if (!pair.second.empty()) {
                avgTime = std::accumulate(pair.second.begin(), pair.second.end(), 0.0) / pair.second.size();
            }
            oss << "  " << pair.first << ": " << pair.second.size() << " events, avg: " 
                << avgTime << "s" << std::endl;
        }
    }
    
    if (!pImpl->snapshots.empty()) {
        oss << std::endl << "Performance Snapshots:" << std::endl;
        for (size_t i = 0; i < std::min<size_t>(pImpl->snapshots.size(), 10); ++i) {
            auto& snapshot = pImpl->snapshots[i];
            oss << "  " << snapshot.first << "s: " << snapshot.second << std::endl;
        }
        if (pImpl->snapshots.size() > 10) {
            oss << "  ... and " << (pImpl->snapshots.size() - 10) << " more snapshots" << std::endl;
        }
    }
    
    return oss.str();
}

bool PerformanceMonitor::savePerformanceData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Performance Data" << std::endl;
    file << "=================" << std::endl;
    file << "Total time: " << pImpl->metrics.totalSimulationTime << std::endl;
    file << "Steps per second: " << pImpl->metrics.stepsPerSecond << std::endl;
    file << "Memory usage: " << pImpl->metrics.memoryUsageMB << std::endl;
    
    if (!pImpl->snapshots.empty()) {
        file << std::endl << "Snapshots:" << std::endl;
        for (const auto& snapshot : pImpl->snapshots) {
            file << snapshot.first << "," << snapshot.second << std::endl;
        }
    }
    
    return true;
}

bool PerformanceMonitor::loadPerformanceData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.find("=") != std::string::npos) {
            size_t pos = line.find("=");
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Parse based on key
            if (key.find("time") != std::string::npos) {
                pImpl->metrics.totalSimulationTime = std::stod(value);
            } else if (key.find("steps") != std::string::npos) {
                pImpl->metrics.stepsPerSecond = std::stod(value);
            } else if (key.find("memory") != std::string::npos) {
                pImpl->metrics.memoryUsageMB = std::stod(value);
            }
        }
    }
    
    return true;
}

void PerformanceMonitor::setMetricCollectionEnabled(const std::string& metric, bool enabled) {
    pImpl->metricCollectionEnabled[metric] = enabled;
}

bool PerformanceMonitor::isMetricCollectionEnabled(const std::string& metric) const {
    auto it = pImpl->metricCollectionEnabled.find(metric);
    return it != pImpl->metricCollectionEnabled.end() ? it->second : false;
}

double PerformanceMonitor::getAverageStepsPerSecond() const {
    if (pImpl->snapshots.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& snapshot : pImpl->snapshots) {
        sum += snapshot.second;
    }
    return sum / pImpl->snapshots.size();
}

double PerformanceMonitor::getAverageMemoryUsageMB() const {
    return pImpl->metrics.memoryUsageMB;
}

double PerformanceMonitor::getAverageSpikeRate() const {
    return pImpl->metrics.averageSpikeRate;
}

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    bool enablePerformanceMonitoring;
    std::shared_ptr<PerformanceMonitor> performanceMonitor;
    
    Impl() : globalSeed(42), enablePerformanceMonitoring(false) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

void ExperimentRunner::enablePerformanceMonitoring(bool enabled) {
    pImpl->enablePerformanceMonitoring = enabled;
    if (enabled) {
        pImpl->performanceMonitor = std::make_shared<PerformanceMonitor>();
    }
}

void ExperimentRunner::setPerformanceMonitor(std::shared_ptr<PerformanceMonitor> monitor) {
    pImpl->performanceMonitor = monitor;
    pImpl->enablePerformanceMonitoring = (monitor != nullptr);
}

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps,
                                     bool enableMonitoring) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    bool shouldMonitor = pImpl->enablePerformanceMonitoring && enableMonitoring && pImpl->performanceMonitor;
    
    if (shouldMonitor) {
        auto startTime = std::chrono::high_resolution_clock::now();
        pImpl->performanceMonitor->startMonitoring();
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    double experimentStartTime = 0.0;
    if (shouldMonitor) {
        auto now = std::chrono::high_resolution_clock::now();
        experimentStartTime = std::chrono::duration<double>(now - pImpl->performanceMonitor->pImpl->startTime).count();
    }
    
    // Run simulation loop
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        auto stepStartTime = std::chrono::high_resolution_clock::now();
        
        // Brain step
        brain->step(step);
        
        if (shouldMonitor) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(now - stepStartTime);
            pImpl->performanceMonitor->recordEvent("brain_step", duration.count());
        }
        
        // Record data
        experiment->recordStep(
            step,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        
        if (shouldMonitor) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(now - stepStartTime);
            pImpl->performanceMonitor->recordEvent("environment_step", duration.count());
        }
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        if (shouldMonitor) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(now - stepStartTime);
            pImpl->performanceMonitor->recordEvent("total_step", duration.count());
        }
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    
    if (shouldMonitor) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double>(endTime - pImpl->performanceMonitor->pImpl->startTime);
        
        pImpl->performanceMonitor->recordSimulationMetrics(brain, maxSteps, totalDuration.count());
        pImpl->performanceMonitor->stopMonitoring();
        
        // Add experiment to results
        experiment->setPerformanceMetrics(pImpl->performanceMonitor->getMetrics());
    }
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps,
    bool enableMonitoring) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // For now, use the same environment template
        // In a real implementation, you might want to clone the environment
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps, enableMonitoring)) {
            results.push_back(exp);
        }
    }
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath, bool includePerformance) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Experiment Results" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        file << "Experiment " << i+1 << ": " << pImpl->experiments[i]->getName() << std::endl;
        file << "  Seed: " << pImpl->experiments[i]->getSeed() << std::endl;
        file << "  Start Step: " << pImpl->experiments[i]->getStartStep() << std::endl;
        file << "  End Step: " << pImpl->experiments[i]->getEndStep() << std::endl;
        file << "  Total Reward: " << pImpl->experiments[i]->getMetric("total_reward") << std::endl;
        
        if (includePerformance) {
            auto perfMetrics = pImpl->experiments[i]->getPerformanceMetrics();
            if (perfMetrics.totalSimulationTime > 0) {
                file << "  Performance Metrics:" << std::endl;
                file << "    Total Time: " << perfMetrics.totalSimulationTime << std::endl;
                file << "    Steps per Second: " << perfMetrics.stepsPerSecond << std::endl;
                file << "    Memory Usage: " << perfMetrics.memoryUsageMB << " MB" << std::endl;
            }
        }
        file << std::endl;
    }
    
    return true;
}

bool ExperimentRunner::runBenchmarks(std::shared_ptr<Brain> brain,
                                    std::shared_ptr<Environment> environment,
                                    const std::vector<SimulationStep>& stepCounts) {
    if (!brain || !environment) {
        return false;
    }
    
    NLM_LOG_INFO("=== Performance Benchmarks ===");
    
    for (size_t i = 0; i < stepCounts.size(); ++i) {
        SimulationStep steps = stepCounts[i];
        NLM_LOG_INFO("Running benchmark with " << steps << " steps...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run experiment
        auto experiment = createExperiment("benchmark_" + std::to_string(i));
        runExperiment(experiment, brain, environment, steps);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        double stepsPerSecond = steps / duration.count();
        
        NLM_LOG_INFO("Benchmark " << i+1 << " completed in " << duration.count() << "s");
        NLM_LOG_INFO("  Performance: " << stepsPerSecond << " steps/second");
        NLM_LOG_INFO("  Total spikes: " << brain->getTotalSpikeCount());
        NLM_LOG_INFO("  Average firing rate: " << brain->getAverageFiringRate());
        
        // Reset brain for next benchmark
        brain->reset();
        brain->initialize();
        environment->reset();
        
        NLM_LOG_INFO("");
    }
    
    return true;
}

std::string ExperimentRunner::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "=== NLM Performance Report ===" << std::endl;
    oss << "Generated: " << getCurrentTimestamp() << std::endl;
    oss << std::endl;
    
    oss << "Summary Statistics:" << std::endl;
    oss << "  Total Experiments: " << pImpl->experiments.size() << std::endl;
    oss << "  Average Steps per Experiment: " << calculateAverageSteps() << std::endl;
    oss << "  Total Reward Generated: " << calculateTotalReward() << std::endl;
    oss << std::endl;
    
    if (pImpl->performanceMonitor) {
        oss << "Performance Monitoring:" << std::endl;
        oss << "  Enabled: Yes" << std::endl;
        oss << "  Total Simulation Time: " << pImpl->performanceMonitor->getMetrics().totalSimulationTime << "s" << std::endl;
        oss << "  Average Performance: " << pImpl->performanceMonitor->getAverageStepsPerSecond() << " steps/sec" << std::endl;
        oss << "  Average Memory Usage: " << pImpl->performanceMonitor->getAverageMemoryUsageMB() << " MB" << std::endl;
        oss << "  Average Spike Rate: " << pImpl->performanceMonitor->getAverageSpikeRate() << " spikes/neuron/sec" << std::endl;
    } else {
        oss << "Performance Monitoring: Disabled" << std::endl;
    }
    
    oss << std::endl;
    oss << "Detailed Results:" << std::endl;
    
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        oss << "  Experiment " << i+1 << ": " << pImpl->experiments[i]->getName() << std::endl;
        oss << "    Performance: " << pImpl->experiments[i]->getPerformanceMetrics().stepsPerSecond << " steps/sec" << std::endl;
        oss << "    Efficiency: " << calculateEfficiency(pImpl->experiments[i]) << "%" << std::endl;
    }
    
    return oss.str();
}

// Helper functions
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

SimulationStep calculateAverageSteps() {
    if (pImpl->experiments.empty()) return 0;
    
    double totalSteps = 0.0;
    for (const auto& exp : pImpl->experiments) {
        totalSteps += exp->getEndStep() - exp->getStartStep();
    }
    
    return static_cast<SimulationStep>(totalSteps / pImpl->experiments.size());
}

float calculateTotalReward() {
    float total = 0.0;
    for (const auto& exp : pImpl->experiments) {
        total += exp->getMetric("total_reward");
    }
    return total;
}

double calculateEfficiency(std::shared_ptr<Experiment> experiment) {
    if (!experiment) return 0.0;
    
    auto perfMetrics = experiment->getPerformanceMetrics();
    if (perfMetrics.totalSimulationTime <= 0) return 0.0;
    
    // Simple efficiency metric: steps per second normalized by time
    double efficiency = perfMetrics.stepsPerSecond / 1000.0; // Normalize to 1000 steps/sec
    return std::min(100.0, efficiency * 100.0); // Cap at 100%
}

} // namespace nlm
