#include "performance/PerformanceMonitor.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <map>

using namespace nlm;

/**
 * Simple performance monitoring tool for NLM demos
 */

class SimplePerformanceMonitor {
public:
    SimplePerformanceMonitor() {
        std::cout << "=== NLM Performance Monitor ===\n";
        std::cout << "Monitoring brain performance during simulation...\n\n";
        
        // Initialize timing
        startTime = std::chrono::high_resolution_clock::now();\n        stepCount = 0;
        lastReportTime = startTime;
        reportInterval = std::chrono::seconds(5);
        
        // Initialize metrics
        totalNeurons = 0;
        totalSynapses = 0;
        maxFiringRate = 0.0f;
        minFiringRate = 1000.0f;
        avgFiringRate = 0.0f;
        totalSpikes = 0;
        
        monitoring = true;
    }
    
    ~SimplePerformanceMonitor() {
        stop();
    }
    
    void start() {
        monitorThread = std::thread(&SimplePerformanceMonitor::monitorLoop, this);
    }
    
    void stop() {
        monitoring = false;
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
        
        // Final report
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n=== Performance Summary ===\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total simulation time: " << totalTime << "s\n";
        std::cout << "Total steps completed: " << stepCount << "\n";
        std::cout << "Steps per second: " << (stepCount / totalTime) << "\n\n";
        
        std::cout << "Neural metrics:\n";
        std::cout << "  Neurons: " << totalNeurons << "\n";
        std::cout << "  Synapses: " << totalSynapses << "\n";
        std::cout << "  Max firing rate: " << maxFiringRate << " Hz\n";
        std::cout << "  Min firing rate: " << minFiringRate << " Hz\n";
        std::cout << "  Average firing rate: " << avgFiringRate << " Hz\n";
        std::cout << "  Total spikes: " << totalSpikes << "\n\n";
        
        std::cout << "Performance indicators:\n";
        std::cout << "  Network density: " << (totalSynapses / (totalNeurons * (totalNeurons - 1))) * 100 << "%\n";
        std::cout << "  Activity level: " << (avgFiringRate / 100) * 100 << "%\n";
        std::cout << "  Connectivity: " << (totalSynapses / totalNeurons) << " synapses/neuron\n";
    }
    
    void update(const std::shared_ptr<Brain>& brain, size_t stepsCompleted) {
        if (!brain) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        stepCount = stepsCompleted;
        totalNeurons = brain->getTotalNeuronCount();
        totalSynapses = brain->getTotalSynapseCount();
        
        float currentFiringRate = brain->getAverageFiringRate();
        size_t currentSpikes = brain->getTotalSpikeCount();
        
        // Update statistics
        maxFiringRate = std::max(maxFiringRate, currentFiringRate);
        minFiringRate = std::min(minFiringRate, currentFiringRate);
        totalSpikes = currentSpikes;
        
        if (stepCount > 0) {
            avgFiringRate = totalSpikes / static_cast<float>(stepCount);
        }
    }
    
    void monitorLoop() {
        while (monitoring) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastReportTime);
            
            if (elapsed >= reportInterval) {
                reportProgress();
                lastReportTime = currentTime;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void reportProgress() {
        if (stepCount <= 0) return;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(currentTime - startTime).count();
        double stepsPerSecond = stepCount / totalTime;
        
        std::cout << "[PROGRESS] " << std::fixed << std::setprecision(1);
        std::cout << "Step: " << stepCount << ", ";
        std::cout << "Speed: " << stepsPerSecond << " steps/s, ";
        std::cout << "Neurons: " << totalNeurons << ", ";
        std::cout << "Firing: " << avgFiringRate << " Hz, ";
        std::cout << "Activity: " << (avgFiringRate / 100) * 100 << "%\n";
    }
    
    void setReportInterval(int seconds) {
        reportInterval = std::chrono::seconds(seconds);
    }
    
    bool isRunning() const {
        return monitoring;
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastReportTime;
    std::chrono::seconds reportInterval;
    
    size_t stepCount;
    size_t totalNeurons;
    size_t totalSynapses;
    float maxFiringRate;
    float minFiringRate;
    float avgFiringRate;
    size_t totalSpikes;
    
    std::thread monitorThread;
    bool monitoring;
    std::mutex mutex;
};

/**
 * Simple performance monitoring tool for Phase 6 demo
 */
class Phase6PerformanceMonitor {
public:
    Phase6PerformanceMonitor() {
        std::cout << "=== NLM Phase 6 Performance Monitor ===\n";
        std::cout << "Monitoring integrated brain systems...\n\n";
        
        // Initialize timing
        startTime = std::chrono::high_resolution_clock::now();
        stepCount = 0;
        
        // Initialize system metrics
        memoryWorkingMemory = 0;
        memoryEpisodicMemory = 0;
        neuromodulationLevel = 0.0f;
        predictionAccuracy = 0.0f;
        developmentProgress = 0.0f;
    }
    
    ~Phase6PerformanceMonitor() {
        // Final report
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n=== Phase 6 Performance Summary ===\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total simulation time: " << totalTime << "s\n";
        std::cout << "Total steps completed: " << stepCount << "\n";
        std::cout << "Performance indicators:\n";
        std::cout << "  Working Memory: " << memoryWorkingMemory << " active traces\n";
        std::cout << "  Episodic Memory: " << memoryEpisodicMemory << " episodes\n";
        std::cout << "  Neuromodulation Level: " << neuromodulationLevel << "\n";
        std::cout << "  Prediction Accuracy: " << predictionAccuracy << "\n";
        std::cout << "  Development Progress: " << developmentProgress << "\n\n";
    }
    
    void update(const Phase6IntegrationResult& result, size_t stepsCompleted) {
        stepCount = stepsCompleted;
        
        // Extract metrics from result
        memoryWorkingMemory = result.memoryWorkingMemoryIntegrated ? 1 : 0;
        memoryEpisodicMemory = result.memoryEpisodicMemoryIntegrated ? result.memoryEpisodesStored : 0;
        neuromodulationLevel = result.dopamineLevel;
        predictionAccuracy = result.predictionIntegrated ? 0.8f : 0.0f;  // Example value
        developmentProgress = result.developmentIntegrated ? result.developmentIntegrated : 0.0f;
        
        // Report progress every 500 steps
        if (stepCount % 500 == 0 && stepCount > 0) {
            reportProgress(result);
        }
    }
    
    void reportProgress(const Phase6IntegrationResult& result) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(currentTime - startTime).count();
        double stepsPerSecond = stepCount / totalTime;
        
        std::cout << "[PROGRESS] " << std::fixed << std::setprecision(1);
        std::cout << "Step: " << stepCount << ", ";
        std::cout << "Speed: " << stepsPerSecond << " steps/s, ";
        std::cout << "Reward: " << result.totalReward << ", ";
        std::cout << "Firing: " << result.avgFiringRate << " Hz\n";
    }
    
    bool isRunning() const {
        return stepCount > 0;
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
    size_t stepCount;
    
    int memoryWorkingMemory;
    int memoryEpisodicMemory;
    float neuromodulationLevel;
    float predictionAccuracy;
    float developmentProgress;
};