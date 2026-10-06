// Performance benchmarking tools for NLM
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "World.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <map>

namespace benchmark {

struct PerformanceMetrics {
    double totalTime;           // Total simulation time in seconds
    double cpuTime;             // CPU time in seconds
    double memoryUsage;         // Memory usage in MB
    double brainTime;           // Time spent in brain updates
    double agentTime;           // Time spent in agent updates
    double worldTime;           // Time spent in world updates
    uint64_t stepsPerSecond;    // Steps executed per second
    uint64_t totalSteps;        // Total simulation steps
    float avgFiringRate;        // Average firing rate during benchmark
    float maxFiringRate;        // Maximum firing rate during benchmark
    float avgCuriosity;         // Average curiosity level
    float avgNovelty;           // Average novelty level
    float developmentProgress;  // Development stage progress
    
    PerformanceMetrics() : totalTime(0.0), cpuTime(0.0), memoryUsage(0.0),
                          brainTime(0.0), agentTime(0.0), worldTime(0.0),
                          stepsPerSecond(0), totalSteps(0), avgFiringRate(0.0),
                          maxFiringRate(0.0), avgCuriosity(0.0), avgNovelty(0.0),
                          developmentProgress(0.0) {}
};

struct BenchmarkConfig {
    std::string name;           // Benchmark name
    uint64_t duration;          // Duration in seconds (0 for unlimited)
    uint64_t targetSteps;       // Target number of steps (0 for unlimited)
    bool recordDetailed;        // Record detailed metrics
    bool exportMetrics;         // Export metrics to file
    std::string outputPath;     // Output file path
    
    BenchmarkConfig() : duration(60), targetSteps(0), recordDetailed(true),
                       exportMetrics(false), outputPath("") {}
    
    BenchmarkConfig(std::string n, uint64_t d = 60) : name(n), duration(d),
                       targetSteps(0), recordDetailed(true), exportMetrics(false),
                       outputPath("") {}
};

class PerformanceBenchmarker {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    BenchmarkConfig config;
    PerformanceMetrics metrics;
    std::vector<double> timeHistory;     // Time history for averaging
    std::vector<float> firingHistory;    // Firing rate history
    std::vector<float> curiosityHistory; // Curiosity level history
    
    PerformanceBenchmarker() {}
    
    void initialize(std::shared_ptr<Config> brainConfig, std::shared_ptr<SimpleWorld> w) {
        brain = std::make_shared<Brain>(brainConfig);
        world = w;
        agent = std::make_shared<AgentBrain>(brain);
        
        brain->initialize();
        agent->initialize(*world);
        
        // Enable monitoring
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
    }
    
    void runBenchmark() {
        auto startTime = std::chrono::steady_clock::now();
        auto cpuStart = std::chrono::high_resolution_clock::now();
        
        uint64_t stepsCompleted = 0;
        bool timeLimitReached = false;
        bool targetStepsReached = false;
        
        while (!timeLimitReached && !targetStepsReached) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Update world
            double time = world->getSimulationTime();
            world->update(0.1);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(stepsCompleted);
            
            // Get action
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyAction(agent->getBrain()->getRegions()[0].get(), cmd);
            
            // Apply reward modulation
            float reward = agent->getCuriosityLevel() * 0.1f;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            stepsCompleted++;
            
            // Record metrics
            recordStepMetrics(stepsCompleted);
            
            // Check time limits
            auto cpuNow = std::chrono::high_resolution_clock::now();
            double elapsedSeconds = std::chrono::duration<double>(cpuNow - cpuStart).count();
            
            if (config.duration > 0 && elapsedSeconds >= config.duration) {
                timeLimitReached = true;
            }
            
            if (config.targetSteps > 0 && stepsCompleted >= config.targetSteps) {
                targetStepsReached = true;
            }
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration<double, std::milli>(stepEnd - stepStart).count();
            
            // Accumulate timing metrics
            metrics.totalTime += stepDuration / 1000.0;
            metrics.brainTime += stepDuration * 0.4; // Approximate distribution
            metrics.agentTime += stepDuration * 0.3;
            metrics.worldTime += stepDuration * 0.3;
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto cpuEnd = std::chrono::high_resolution_clock::now();
        
        metrics.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        metrics.cpuTime = std::chrono::duration<double>(cpuEnd - cpuStart).count();
        metrics.totalSteps = stepsCompleted;
        metrics.stepsPerSecond = static_cast<uint64_t>(stepsCompleted / std::max(metrics.totalTime, 1.0));
        
        // Calculate averages
        calculateAverages();
        
        // Export metrics if requested
        if (config.exportMetrics) {
            exportMetrics();
        }
    }
    
    void recordStepMetrics(uint64_t step) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double timeSinceStart = std::chrono::duration<double>(currentTime - std::chrono::high_resolution_clock::now()).count();
        
        timeHistory.push_back(timeSinceStart);
        firingHistory.push_back(brain->getAverageFiringRate());
        curiosityHistory.push_back(agent->getCuriosityLevel());
        
        // Keep history limited to prevent memory issues
        if (timeHistory.size() > 1000) {
            timeHistory.erase(timeHistory.begin());
            firingHistory.erase(firingHistory.begin());
            curiosityHistory.erase(curiosityHistory.begin());
        }
    }
    
    void calculateAverages() {
        if (firingHistory.empty()) return;
        
        metrics.avgFiringRate = std::accumulate(firingHistory.begin(), firingHistory.end(), 0.0f) / firingHistory.size();
        metrics.maxFiringRate = *std::max_element(firingHistory.begin(), firingHistory.end());
        metrics.avgCuriosity = std::accumulate(curiosityHistory.begin(), curiosityHistory.end(), 0.0f) / curiosityHistory.size();
        metrics.avgNovelty = agent->getNoveltyLevel(); // Note: would need historical data
        metrics.developmentProgress = static_cast<float>(brain->getDevelopmentalStage()) / 4.0f; // Normalize to [0,1]
    }
    
    void exportMetrics() {
        if (config.outputPath.empty()) return;
        
        std::ofstream file(config.outputPath);
        if (!file.is_open()) {
            return;
        }
        
        file << "NLM Performance Benchmark Report" << std::endl;
        file << "=================================" << std::endl;
        file << std::endl;
        
        file << "Benchmark Name: " << config.name << std::endl;
        file << "Duration: " << metrics.totalTime << " seconds" << std::endl;
        file << "Steps Completed: " << metrics.totalSteps << std::endl;
        file << "Steps per Second: " << metrics.stepsPerSecond << std::endl;
        file << "CPU Time: " << metrics.cpuTime << " seconds" << std::endl;
        file << std::endl;
        
        file << "Performance Metrics:" << std::endl;
        file << "  Average Firing Rate: " << metrics.avgFiringRate << " Hz" << std::endl;
        file << "  Maximum Firing Rate: " << metrics.maxFiringRate << " Hz" << std::endl;
        file << "  Average Curiosity Level: " << metrics.avgCuriosity << std::endl;
        file << "  Average Novelty Level: " << metrics.avgNovelty << std::endl;
        file << "  Development Progress: " << (metrics.developmentProgress * 100) << "%" << std::endl;
        file << std::endl;
        
        file << "Time Breakdown:" << std::endl;
        file << "  Brain Processing: " << (metrics.brainTime / metrics.totalTime * 100) << "%" << std::endl;
        file << "  Agent Processing: " << (metrics.agentTime / metrics.totalTime * 100) << "%" << std::endl;
        file << "  World Processing: " << (metrics.worldTime / metrics.totalTime * 100) << "%" << std::endl;
        file << std::endl;
        
        file << "Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
        file << std::endl;
        
        if (config.recordDetailed) {
            file << "Detailed Metrics:" << std::endl;
            file << "Step,Time,FiringRate,Curiosity" << std::endl;
            
            for (size_t i = 0; i < timeHistory.size(); ++i) {
                file << i << ","
                     << timeHistory[i] << ","
                     << firingHistory[i] << ","
                     << curiosityHistory[i] << std::endl;
            }
        }
        
        file.close();
    }
    
    PerformanceMetrics getMetrics() const {
        return metrics;
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Benchmark: " << config.name << std::endl;
        ss << "Steps: " << metrics.totalSteps << " (" << metrics.stepsPerSecond << " Hz)" << std::endl;
        ss << "Duration: " << metrics.totalTime << " s" << std::endl;
        ss << "Avg Firing Rate: " << metrics.avgFiringRate << " Hz" << std::endl;
        ss << "Efficiency: " << (metrics.avgFiringRate / 1000.0 * 100) << "%"; // Normalize efficiency
        
        return ss.str();
    }
};

// Comparison benchmarking
class BenchmarkComparator {
public:
    std::vector<PerformanceBenchmarker> benchmarks;
    std::string comparisonName;
    
    BenchmarkComparator(std::string name = "comparison") : comparisonName(name) {}
    
    void addBenchmark(std::string name, BenchmarkConfig config) {
        PerformanceBenchmarker bench;
        benchmarks.push_back(bench);
        // In a real implementation, you would create different configurations
        // for each benchmark
    }
    
    void runAllBenchmarks() {
        for (auto& bench : benchmarks) {
            bench.runBenchmark();
        }
    }
    
    void compareResults() {
        std::cout << "Benchmark Comparison: " << comparisonName << std::endl;
        std::cout << "=======================" << std::endl;
        
        for (size_t i = 0; i < benchmarks.size(); ++i) {
            std::cout << "Benchmark " << i << ": " << benchmarks[i].getSummary() << std::endl;
        }
        
        std::cout << std::endl << "Winner: " << getWinner() << std::endl;
    }
    
    std::string getWinner() {
        if (benchmarks.empty()) return "No benchmarks run";
        
        size_t bestIndex = 0;
        double bestScore = 0.0;
        
        for (size_t i = 0; i < benchmarks.size(); ++i) {
            double score = benchmarks[i].getPerformanceScore();
            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
            }
        }
        
        return "Benchmark " + std::to_string(bestIndex);
    }
    
    double getPerformanceScore(const PerformanceBenchmarker& bench) {
        // Simple scoring algorithm based on efficiency
        double efficiency = bench.metrics.avgFiringRate / std::max(bench.metrics.totalTime, 1.0);
        double stability = 1.0 / (1.0 + bench.metrics.maxFiringRate - bench.metrics.avgFiringRate);
        double learning = bench.metrics.avgCuriosity * bench.metrics.avgNovelty;
        
        return efficiency * 0.5 + stability * 0.3 + learning * 0.2;
    }
};

} // namespace benchmark
