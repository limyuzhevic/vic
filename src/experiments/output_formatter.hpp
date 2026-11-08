#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <chrono>
#include <ctime>

namespace nlm {

/**
 * Common output formatting utilities for experiment results
 */
namespace OutputFormatter {
    /**
     * Format a header with equal signs
     */
    inline void logHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "  " << title << "\n";
        std::cout << std::string(60, '=') << "\n";
    }
    
    /**
     * Format a section header with dashes
     */
    inline void logSection(const std::string& title) {
        std::cout << "\n--- " << title << " ---\n";
    }
    
    /**
     * Format a key-value result pair
     */
    inline void logResult(const std::string& name, float value, float max = 1.0f) {
        std::cout << "  " << std::left << std::setw(30) << name << ": ";
        std::cout << std::fixed << std::setprecision(3) << value;
        if (max > 0) {
            std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
        }
        std::cout << "\n";
    }
    
    /**
     * Format a simulation step result
     */
    inline void logSimulationStep(int step, float reward, const std::string& action,
                                 float neuromod, float novelty, float curiosity,
                                 float energy) {
        std::cout << "  Step " << step << ": reward=" << reward
                  << ", action=" << action
                  << ", neuromod=" << neuromod
                  << ", novelty=" << novelty
                  << ", curiosity=" << curiosity
                  << ", energy=" << energy << "\n";
    }
    
    /**
     * Format experiment metrics summary
     */
    template<typename T>
    inline void logMetrics(const std::string& title, const std::vector<T>& metrics) {
        logSection(title);
        for (const auto& metric : metrics) {
            // Default implementation - can be overridden
            std::cout << "  Metric: " << metric << "\n";
        }
    }
    
    /**
     * Format comparison between two experiment results
     */
    template<typename T>
    inline void logComparison(const T& early, const T& late, const std::string& metricName,
                             float earlyValue, float lateValue) {
        logSection("Comparison: " + metricName);
        std::cout << "  Early: " << earlyValue << " -> Late: " << lateValue << "\n";
        
        float difference = lateValue - earlyValue;
        if (difference > 0) {
            std::cout << "  Change: +" << difference << " (positive learning)\n";
        } else if (difference < -5) {
            std::cout << "  Change: " << difference << " (possible exploration or extinction)\n";
        }
    }
    
    /**
     * Format experiment timing information
     */
    inline void logTiming(time_t startTime, time_t endTime) {
        double duration = std::difftime(endTime, startTime);
        std::cout << "\nExperiment completed in " << duration << " seconds\n";
    }
    
    /**
     * Format experiment result summary
     */
    inline void logExperimentSummary(const std::string& experimentName, int episodes,
                                    int steps, float totalReward, float avgFiringRate) {
        logHeader(experimentName + " Summary");
        std::cout << "  Episodes: " << episodes << " x " << steps << " steps\n";
        std::cout << "  Total Reward: " << totalReward << "\n";
        std::cout << "  Average Firing Rate: " << avgFiringRate << " Hz\n";
    }
    
    /**
     * Create a formatted CSV line from metrics
     */
    inline std::string formatCSV(const std::vector<std::pair<std::string, double>>& metrics) {
        std::stringstream ss;
        bool first = true;
        for (const auto& pair : metrics) {
            if (!first) ss << ",";
            ss << pair.first << "," << pair.second;
            first = false;
        }
        return ss.str();
    }
    
    /**
     * Create a formatted JSON line from metrics
     */
    inline std::string formatJSON(const std::vector<std::pair<std::string, double>>& metrics) {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& pair : metrics) {
            if (!first) ss << ", ";
            ss << "\"" << pair.first << "\": " << pair.second;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
}

} // namespace nlm
