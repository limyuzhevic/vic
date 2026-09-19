#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <memory>

namespace nlm {

class BrainProfiler; // Forward declaration

/**
 * Performance Metrics Class
 * 
 * Comprehensive data structures for tracking simulation performance,
 * memory usage, network statistics, and system health indicators.
 */
class PerformanceMetrics {
public:
    /**
     * Time tracking metrics
     */
    struct TimeMetrics {
        double wallClockTime;      // Real elapsed time in seconds
        double simulationTime;     // Simulated time in seconds
        double stepDuration;       // Duration of last step in seconds
        std::vector<double> stepTimes;  // History of step times
        std::chrono::steady_clock::time_point startTime;
        
        TimeMetrics() : wallClockTime(0.0), simulationTime(0.0), stepDuration(0.0),
                        startTime(std::chrono::steady_clock::now()) {}
        
        void recordStepTime(double time) {
            stepTimes.push_back(time);
            if (stepTimes.size() > 1000) {
                stepTimes.erase(stepTimes.begin());
            }
        }
        
        double getAverageStepTime() const {
            if (stepTimes.empty()) return 0.0;
            double sum = 0.0;
            for (double t : stepTimes) sum += t;
            return sum / stepTimes.size();
        }
        
        double getMaxStepTime() const {
            if (stepTimes.empty()) return 0.0;
            return *std::max_element(stepTimes.begin(), stepTimes.end());
        }
    };

    /**
     * Memory usage statistics
     */
    struct MemoryMetrics {
        size_t totalAllocated;     // Total memory allocated (bytes)
        size_t usedMemory;         // Currently used memory (bytes)
        size_t freeMemory;         // Free memory (bytes)
        size_t allocationCount;    // Number of allocation calls
        std::vector<size_t> allocationHistory;  // History of allocation sizes
        double memoryUsagePercentage;  // Percentage of used memory
        
        MemoryMetrics() : totalAllocated(0), usedMemory(0), freeMemory(0),
                         allocationCount(0), memoryUsagePercentage(0.0) {}
        
        void recordAllocation(size_t bytes) {
            allocationHistory.push_back(bytes);
            allocationCount++;
            totalAllocated += bytes;
            
            if (allocationHistory.size() > 1000) {
                totalAllocated -= allocationHistory.front();
                allocationHistory.erase(allocationHistory.begin());
            }
        }
        
        void updateMemoryUsage(size_t used, size_t free) {
            usedMemory = used;
            freeMemory = free;
            size_t total = used + free;
            memoryUsagePercentage = (total > 0) ? (static_cast<double>(used) / total * 100.0) : 0.0;
        }
    };

    /**
     * Network statistics
     */
    struct NetworkMetrics {
        size_t totalNeurons;       // Total number of neurons
        size_t activeNeurons;      // Number of active neurons
        size_t firingNeurons;      // Number of firing neurons
        size_t totalSynapses;      // Total number of synapses
        double averageFiringRate;  // Average firing rate (Hz)
        double excitationInhibitionRatio;  // E/I balance
        std::vector<double> neuronFiringRates;  // Individual neuron firing rates
        std::vector<size_t> neuronActivityHistory;  // Neuron activity over time
        
        NetworkMetrics() : totalNeurons(0), activeNeurons(0), firingNeurons(0),
                           totalSynapses(0), averageFiringRate(0.0),
                           excitationInhibitionRatio(0.0) {}
        
        void updateStatistics(size_t totalN, size_t activeN, size_t firingN,
                             size_t totalS, double avgFiring, double eiratio) {
            totalNeurons = totalN;
            activeNeurons = activeN;
            firingNeurons = firingN;
            totalSynapses = totalS;
            averageFiringRate = avgFiring;
            excitationInhibitionRatio = eiratio;
            
            neuronActivityHistory.push_back(firingN);
            if (neuronActivityHistory.size() > 1000) {
                neuronActivityHistory.erase(neuronActivityHistory.begin());
            }
        }
        
        void addFiringRate(double rate) {
            neuronFiringRates.push_back(rate);
            if (neuronFiringRates.size() > 1000) {
                neuronFiringRates.erase(neuronFiringRates.begin());
            }
        }
        
        double getFiringRateStdDev() const {
            if (neuronFiringRates.size() < 2) return 0.0;
            
            double sum = std::accumulate(neuronFiringRates.begin(), neuronFiringRates.end(), 0.0);
            double mean = sum / neuronFiringRates.size();
            
            double sqSum = 0.0;
            for (double rate : neuronFiringRates) {
                sqSum += (rate - mean) * (rate - mean);
            }
            
            return std::sqrt(sqSum / neuronFiringRates.size());
        }
    };

    /**
     * Integration metrics for system-wide measurements
     */
    struct IntegrationMetrics {
        size_t stepsCompleted;     // Number of completed simulation steps
        double totalReward;        // Total reward accumulated
        size_t episodesStored;    // Number of episodes stored
        double dopamineLevel;     // Current dopamine level
        double curiosityLevel;    // Current curiosity level
        std::vector<double> rewardHistory;      // Reward over time
        std::vector<double> dopamineHistory;    // Dopamine level over time
        std::vector<double> curiosityHistory;   // Curiosity level over time
        
        IntegrationMetrics() : stepsCompleted(0), totalReward(0.0), episodesStored(0),
                               dopamineLevel(0.0), curiosityLevel(0.0) {}
        
        void recordStep(double reward, double dopamine, double curiosity) {
            stepsCompleted++;
            totalReward += reward;
            rewardHistory.push_back(reward);
            dopamineHistory.push_back(dopamine);
            curiosityHistory.push_back(curiosity);
            
            if (rewardHistory.size() > 1000) {
                rewardHistory.erase(rewardHistory.begin());
                dopamineHistory.erase(dopamineHistory.begin());
                curiosityHistory.erase(curiosityHistory.begin());
            }
        }
        
        double getAverageReward() const {
            if (rewardHistory.empty()) return 0.0;
            double sum = std::accumulate(rewardHistory.begin(), rewardHistory.end(), 0.0);
            return sum / rewardHistory.size();
        }
    };

    /**
     * System health indicators
     */
    struct SystemHealthMetrics {
        double cpuUsage;           // CPU usage percentage (0-100)
        double memoryUsage;        // Memory usage percentage (0-100)
        double stabilityScore;     // Stability indicator (0-1)
        bool hasMemoryLeaks;       // Memory leak detection
        bool isStable;            // Overall system stability
        std::string healthStatus;  // "healthy", "warning", "critical"
        
        SystemHealthMetrics() : cpuUsage(0.0), memoryUsage(0.0), stabilityScore(1.0),
                               hasMemoryLeaks(false), isStable(true),
                               healthStatus("healthy") {}
        
        void updateHealth(double cpu, double memory, double stability,
                         bool leaks, bool stable) {
            cpuUsage = cpu;
            memoryUsage = memory;
            stabilityScore = stability;
            hasMemoryLeaks = leaks;
            isStable = stable;
            
            // Determine health status
            if (cpuUsage > 90.0 || memoryUsage > 90.0 || hasMemoryLeaks) {
                healthStatus = "critical";
            } else if (cpuUsage > 70.0 || memoryUsage > 70.0 || stabilityScore < 0.5) {
                healthStatus = "warning";
            } else {
                healthStatus = "healthy";
            }
        }
    };

    /**
     * Comprehensive performance report
     */
    struct PerformanceReport {
        TimeMetrics timeMetrics;
        MemoryMetrics memoryMetrics;
        NetworkMetrics networkMetrics;
        IntegrationMetrics integrationMetrics;
        SystemHealthMetrics healthMetrics;
        
        /**
         * Generate human-readable report
         */
        std::string generateHumanReadableReport() const {
            std::ostringstream oss;
            oss << "=== NLM Brain Performance Report ===\n\n";
            
            // Time metrics section
            oss << "--- Time Metrics ---\n";
            oss << "Wall Clock Time: " << timeMetrics.wallClockTime << " seconds\n";
            oss << "Simulation Time: " << timeMetrics.simulationTime << " seconds\n";
            oss << "Average Step Duration: " << timeMetrics.getAverageStepTime() << " seconds\n";
            oss << "Max Step Duration: " << timeMetrics.getMaxStepTime() << " seconds\n";
            oss << "Steps Recorded: " << timeMetrics.stepTimes.size() << "\n\n";
            
            // Memory metrics section
            oss << "--- Memory Metrics ---\n";
            oss << "Total Allocations: " << memoryMetrics.allocationCount << "\n";
            oss << "Total Allocated: " << memoryMetrics.totalAllocated << " bytes\n";
            oss << "Currently Used: " << memoryMetrics.usedMemory << " bytes\n";
            oss << "Free Memory: " << memoryMetrics.freeMemory << " bytes\n";
            oss << "Memory Usage: " << memoryMetrics.memoryUsagePercentage << "%\n\n";
            
            // Network metrics section
            oss << "--- Network Metrics ---\n";
            oss << "Total Neurons: " << networkMetrics.totalNeurons << "\n";
            oss << "Active Neurons: " << networkMetrics.activeNeurons << "\n";
            oss << "Firing Neurons: " << networkMetrics.firingNeurons << "\n";
            oss << "Total Synapses: " << networkMetrics.totalSynapses << "\n";
            oss << "Average Firing Rate: " << networkMetrics.averageFiringRate << " Hz\n";
            oss << "E/I Ratio: " << networkMetrics.excitationInhibitionRatio << "\n";
            oss << "Firing Rate Standard Deviation: " << networkMetrics.getFiringRateStdDev() << " Hz\n\n";
            
            // Integration metrics section
            oss << "--- Integration Metrics ---\n";
            oss << "Steps Completed: " << integrationMetrics.stepsCompleted << "\n";
            oss << "Total Reward: " << integrationMetrics.totalReward << "\n";
            oss << "Average Reward: " << integrationMetrics.getAverageReward() << "\n";
            oss << "Episodes Stored: " << integrationMetrics.episodesStored << "\n";
            oss << "Current Dopamine Level: " << integrationMetrics.dopamineLevel << "\n";
            oss << "Current Curiosity Level: " << integrationMetrics.curiosityLevel << "\n\n";
            
            // System health section
            oss << "--- System Health ---\n";
            oss << "CPU Usage: " << healthMetrics.cpuUsage << "%\n";
            oss << "Memory Usage: " << healthMetrics.memoryUsage << "%\n";
            oss << "Stability Score: " << healthMetrics.stabilityScore << "\n";
            oss << "Memory Leaks: " << (healthMetrics.hasMemoryLeaks ? "YES" : "NO") << "\n";
            oss << "System Status: " << healthMetrics.healthStatus << "\n";
            
            return oss.str();
        }
    };

    /**
     * PerformanceMetrics constructor
     */
    PerformanceMetrics();

    /**
     * PerformanceMetrics destructor
     */
    ~PerformanceMetrics();

    /**
     * Update time metrics
     */
    void updateTimeMetrics(double wallTime, double simTime, double stepDuration);

    /**
     * Update memory metrics
     */
    void updateMemoryMetrics(size_t used, size_t free, size_t allocatedBytes = 0);

    /**
     * Update network metrics
     */
    void updateNetworkMetrics(size_t totalNeurons, size_t activeNeurons, size_t firingNeurons,
                             size_t totalSynapses, double avgFiring, double eiratio);

    /**
     * Update integration metrics
     */
    void updateIntegrationMetrics(size_t steps, double reward, size_t episodes,
                                  double dopamine, double curiosity);

    /**
     * Update system health metrics
     */
    void updateHealthMetrics(double cpuUsage, double memoryUsage, double stability,
                            bool hasLeaks, bool isStable);

    /**
     * Get complete performance report
     */
    PerformanceReport getPerformanceReport() const;

    /**
     * Export metrics to JSON
     */
    std::string exportToJSON() const;

    /**
     * Export metrics to CSV
     */
    std::string exportToCSV() const;

    /**
     * Reset all metrics
     */
    void reset();

    /**
     * Merge metrics from another PerformanceMetrics instance
     */
    void merge(const PerformanceMetrics& other);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
