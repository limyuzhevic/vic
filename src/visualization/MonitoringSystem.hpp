#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include "../brain/Brain.hpp"
#include "../experiments/Experiment.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Export format for data export functionality
enum class ExportFormat {
    CSV,
    JSON,
    XML,
    BINARY
};

// MonitoringSystem: Comprehensive real-time monitoring and visualization of brain systems
// Provides real-time statistics collection, data visualization, and experiment tracking
// Integrates with brain step loop to monitor neural activity, memory, neuromodulation, and performance

class MonitoringSystem {
public:
    MonitoringSystem();
    ~MonitoringSystem();
    
    // Initialize monitoring system with brain reference
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update monitoring data (called each simulation step)
    void update(double dt);
    
    // Start/stop experiment monitoring
    void startExperiment(std::shared_ptr<Experiment> experiment);
    void stopExperiment();
    
    // Generate comprehensive status report
    std::string generateReport() const;
    
    // Export monitoring data
    void exportData(const std::string& filename, ExportFormat format = ExportFormat::CSV);
    
    // Configuration setters/getters
    void setUpdateInterval(double interval);
    double getUpdateInterval() const;
    
    void setEnableRealTimeUpdates(bool enable);
    bool getEnableRealTimeUpdates() const;
    
    void setEnablePerformanceProfiling(bool enable);
    bool getEnablePerformanceProfiling() const;
    
    void setEnableMemoryTracking(bool enable);
    bool getEnableMemoryTracking() const;
    
    std::string getExportDirectory() const;
    void setExportDirectory(const std::string& directory);
    
    // Close monitoring system
    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper methods
    void collectBrainData();
    void collectPerformanceData();
    void collectMemoryData();
    void collectNeuromodulationData();
    void updateBuffers();
    void recordExperimentData();
    void recordTimeSeries(const std::string& name, const std::vector<double>& values);
    
    void exportToCSV(const std::string& filepath);
    void exportToJSON(const std::string& filepath);
    void exportToXML(const std::string& filepath);
    void exportToBinary(const std::string& filepath);
};

} // namespace nlm
