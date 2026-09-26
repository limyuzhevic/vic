#pragma once

#include "NVMVisualization.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Exporter {
public:
    Exporter();
    ~Exporter() = default;
    
    // Export visualization data
    bool exportVisualization(const nlm::NVMVisualization& visualization, const std::string& filename, const std::string& format = "json");
    
    // Export neural network visualization data
    bool exportNeuralNetworkVisualization(const std::string& filename, const nlm::NetworkTopology& topology, const std::string& format = "json");
    
    // Export memory visualization data
    bool exportMemoryVisualization(const std::string& filename, const std::vector<nlm::MemoryTrace>& memoryTraces, const std::string& format = "json");
    
    // Export neuromodulation visualization data
    bool exportNeuromodulationVisualization(const std::string& filename, const std::vector<nlm::NeuromodulationData>& neuromodulationData, const std::string& format = "json");
    
    // Export plasticity visualization data
    bool exportPlasticityVisualization(const std::string& filename, const std::vector<nlm::PlasticityData>& plasticityData, const std::string& format = "json");
    
    // Export development visualization data
    bool exportDevelopmentVisualization(const std::string& filename, const std::vector<nlm::DevelopmentData>& developmentData, const std::string& format = "json");
    
    // Export analysis data
    bool exportAnalysis(const std::string& filename, const nlm::PerformanceAnalyzer& performanceAnalyzer, const std::string& format = "json");
    bool exportAnalysis(const std::string& filename, const nlm::LearningAnalyzer& learningAnalyzer, const std::string& format = "json");
    bool exportAnalysis(const std::string& filename, const nlm::NetworkAnalyzer& networkAnalyzer, const std::string& format = "json");
    bool exportAnalysis(const std::string& filename, const nlm::SystemAnalyzer& systemAnalyzer, const std::string& format = "json");
    
    // Export multiple visualization data sets
    bool exportMultiple(const std::map<std::string, std::string>& filenames, const nlm::NVMVisualization& visualization);
    
    // Set export options
    void setExportOptions(const std::map<std::string, std::string>& options);
    
    // Get export options
    std::map<std::string, std::string> getExportOptions() const;
    
    // Create export directory
    bool createExportDirectory(const std::string& directory);
    
    // Validate export filename
    bool validateExportFilename(const std::string& filename) const;
    
    // Get supported export formats
    std::vector<std::string> getSupportedExportFormats() const;
    
    // Check if exporter is available
    bool isAvailable() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Export to JSON format
    bool exportToJSON(const std::string& filename, const json& data);
    
    // Export to CSV format
    bool exportToCSV(const std::string& filename, const json& data);
    
    // Export to XML format
    bool exportToXML(const std::string& filename, const json& data);
    
    // Export to binary format
    bool exportToBinary(const std::string& filename, const json& data);
    
    // Read JSON from file
    json readJSON(const std::string& filename) const;
    
    // Write JSON to file
    bool writeJSON(const std::string& filename, const json& data) const;
    
    // Convert visualization data to JSON
    json convertVisualizationToJSON(const nlm::NVMVisualization& visualization) const;
    
    // Convert network topology to JSON
    json convertNetworkTopologyToJSON(const nlm::NetworkTopology& topology) const;
    
    // Convert memory traces to JSON
    json convertMemoryTracesToJSON(const std::vector<nlm::MemoryTrace>& memoryTraces) const;
    
    // Convert neuromodulation data to JSON
    json convertNeuromodulationDataToJSON(const std::vector<nlm::NeuromodulationData>& neuromodulationData) const;
    
    // Convert plasticity data to JSON
    json convertPlasticityDataToJSON(const std::vector<nlm::PlasticityData>& plasticityData) const;
    
    // Convert development data to JSON
    json convertDevelopmentDataToJSON(const std::vector<nlm::DevelopmentData>& developmentData) const;
    
    // Convert analysis data to JSON
    json convertPerformanceAnalysisToJSON(const nlm::PerformanceAnalyzer& performanceAnalyzer) const;
    json convertLearningAnalysisToJSON(const nlm::LearningAnalyzer& learningAnalyzer) const;
    json convertNetworkAnalysisToJSON(const nlm::NetworkAnalyzer& networkAnalyzer) const;
    json convertSystemAnalysisToJSON(const nlm::SystemAnalyzer& systemAnalyzer) const;
};

} // namespace nlm