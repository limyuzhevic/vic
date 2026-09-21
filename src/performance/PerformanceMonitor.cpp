#include "performance/PerformanceMonitor.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <fstream>

namespace nlm {

void PerformanceMonitor::update(const PerformanceStats& stats, double timestamp, size_t step) {
    if (!monitoringEnabled_) return;
    
    currentStats_ = stats;
    
    // Check thresholds
    checkThresholds(stats);
    
    // Add to history
    history_.emplace_back(timestamp, step, stats);
    
    // Maintain history size
    if (history_.size() > maxHistorySize_) {
        history_.erase(history_.begin());
    }
    
    // Write to file if enabled
    if (writeToFile_) {
        // For now, we'll just log to console
        // In a real implementation, we'd write to a file
        if (outputFile_.find(".csv") != std::string::npos) {
            // CSV format for easy import into spreadsheets
            std::stringstream csv;
            csv << timestamp << "," << step << "," << stats.totalSpikes << ","
                << stats.totalNeurons << "," << stats.totalSynapses << ","
                << stats.activeNeurons << "," << stats.firingNeurons << ","
                << stats.cpuTime << "," << stats.wallClockTime << ","
                << stats.memoryUsage << "," << stats.avgSynapticWeight << ","
                << stats.weightChangeRate << "," << stats.dopamineLevel << ","
                << stats.curiosityLevel << "," << stats.noveltyLevel << "\n";
            
            // In a real implementation, we'd append to the file
            // std::ofstream outfile(outputFile_, std::ios::app);
            // outfile << csv.str();
            // outfile.close();
            
            NLM_LOG_INFO("CSV data: " + csv.str());
        } else if (outputFile_.find(".json") != std::string::npos) {
            // JSON format for programmatic access
            // In a real implementation, we'd serialize to JSON
            // For now, just log
            NLM_LOG_INFO("JSON output would be written to: " + outputFile_);
        }
    }
}

void PerformanceMonitor::checkThresholds(const PerformanceStats& stats) {
    thresholdViolations_.clear();
    thresholdViolationsReported_ = false;
    
    // Check CPU time threshold
    if (cpuMonitoringEnabled_ && stats.cpuTime > thresholds_.maxCpuTime) {
        std::stringstream ss;
        ss << "CPU time exceeded threshold: " << stats.cpuTime << "s > " << thresholds_.maxCpuTime << "s";
        thresholdViolations_.push_back(ss.str());
    }
    
    // Check memory usage threshold
    if (memoryMonitoringEnabled_ && stats.memoryUsage > thresholds_.maxMemoryUsage) {
        std::stringstream ss;
        ss << "Memory usage exceeded threshold: " << stats.memoryUsage << "MB > " << thresholds_.maxMemoryUsage << "MB";
        thresholdViolations_.push_back(ss.str());
    }
    
    // Check spiking rate threshold
    if (neuralMonitoringEnabled_) {
        double firingRate = (stats.simulationSteps > 0) ? 
            static_cast<double>(stats.firingNeurons) / stats.simulationSteps : 0.0;
        if (firingRate < static_cast<double>(thresholds_.minSpikingRate)) {
            std::stringstream ss;
            ss << "Firing rate below threshold: " << firingRate << " < " << thresholds_.minSpikingRate;
            thresholdViolations_.push_back(ss.str());
        }
    }
    
    // Check weight change rate threshold
    if (plasticityMonitoringEnabled_ && std::abs(stats.weightChangeRate) > thresholds_.maxWeightChangeRate) {
        std::stringstream ss;
        ss << "Weight change rate exceeded threshold: " << stats.weightChangeRate << " > " << thresholds_.maxWeightChangeRate;
        thresholdViolations_.push_back(ss.str());
    }
    
    thresholdViolationsReported_ = true;
}

std::string PerformanceMonitor::getOperationTimesSummary() const {
    if (lastOperationTimes_.empty()) {
        return "No operation times recorded.";
    }
    
    std::stringstream ss;
    ss << "Operation Times:" << std::endl;
    for (const auto& pair : lastOperationTimes_) {
        ss << "  " << pair.first << ": " << pair.second << " s" << std::endl;
    }
    return ss.str();
}

} // namespace nlm
