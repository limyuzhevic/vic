#include "DataExporter.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nlm {

// DataExporterManager implementation
DataExporterManager::DataExporterManager() {
    // Register exporters
    exporters_[ExportFormat::JSON] = std::make_unique<JSONExporter>();
    exporters_[ExportFormat::CSV] = std::make_unique<CSVExporter>();
    exporters_[ExportFormat::TSV] = std::make_unique<TSVExporter>();
    
    // Register visualizers
    visualizers_["text"] = std::make_unique<TextVisualizer>();
}

// Export data using specified format
bool DataExporterManager::exportData(std::shared_ptr<Brain> brain,
                    ExportFormat format,
                    const std::string& filepath,
                    const std::map<std::string, std::string>& options) {
    auto it = exporters_.find(format);
    if (it == exporters_.end()) {
        return false;
    }
    
    ExportOptions exportOptions;
    exportOptions.format = format;
    exportOptions.filepath = filepath;
    exportOptions.parameters = options;
    
    return it->second->exportBrainState(brain, exportOptions);
}

// Export simulation results
bool DataExporterManager::exportSimulationResults(const std::string& name,
                                 const std::vector<std::map<std::string, double>>& results,
                                 ExportFormat format,
                                 const std::string& filepath,
                                 const std::map<std::string, std::string>& options) {
    auto it = exporters_.find(format);
    if (it == exporters_.end()) {
        return false;
    }
    
    ExportOptions exportOptions;
    exportOptions.format = format;
    exportOptions.filepath = filepath;
    exportOptions.parameters = options;
    
    return it->second->exportSimulationResults(name, results, exportOptions);
}

// Create visualization
bool DataExporterManager::createVisualization(std::shared_ptr<Brain> brain,
                              const std::string& type,
                              const std::string& outputPath,
                              const std::map<std::string, std::string>& options) {
    auto it = visualizers_.find(type);
    if (it == visualizers_.end()) {
        return false;
    }
    
    return it->second->createNeuronVisualization(brain, outputPath, options);
}

// Get supported formats
std::vector<ExportFormat> DataExporterManager::getSupportedFormats(ExportFormat format) const {
    std::vector<ExportFormat> allFormats;
    for (const auto& pair : exporters_) {
        allFormats.push_back(pair.first);
    }
    return allFormats;
}

// Get available visualizers
std::vector<std::string> DataExporterManager::getAvailableVisualizers() const {
    std::vector<std::string> visualizers;
    for (const auto& pair : visualizers_) {
        visualizers.push_back(pair.first);
    }
    return visualizers;
}

} // namespace nlm