#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "../brain/Brain.hpp"

namespace nlm {

// Scientific data export system
class ScientificExporter {
public:
    ScientificExporter();
    ~ScientificExporter();
    
    // Export data structures
    struct ExportData {
        std::string name;
        std::vector<std::vector<float>> data;  // Multi-dimensional data
        std::vector<std::string> labels;
        std::map<std::string, std::string> metadata;
        std::chrono::system_clock::time_point timestamp;
    };
    
    // Export to different formats
    bool exportToNumPy(const ExportData& data, const std::string& filename);
    bool exportToCSV(const ExportData& data, const std::string& filename);
    bool exportToHDF5(const ExportData& data, const std::string& filename);
    bool exportToMAT(const ExportData& data, const std::string& filename);
    bool exportToJSON(const ExportData& data, const std::string& filename);
    
    // Specialized export functions
    bool exportBrainState(const std::shared_ptr<Brain>& brain, const std::string& filename);
    bool exportSimulationResults(const std::vector<BatchSimulation>& simulations, 
                                const std::string& outputDir);
    bool exportPerformanceMetrics(const PerformanceMonitor::PerformanceMetrics& metrics,
                                 const std::string& filename);
    
    // Get available export formats
    std::vector<std::string> getAvailableFormats() const;
    std::map<std::string, std::string> getFormatInfo(const std::string& format) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
