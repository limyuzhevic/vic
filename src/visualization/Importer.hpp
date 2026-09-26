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

class Importer {
public:
    Importer();
    ~Importer() = default;
    
    // Import visualization data
    bool importVisualization(nlm::NVMVisualization& visualization, const std::string& filename, const std::string& format = "json");
    
    // Import neural network visualization data
    bool importNeuralNetworkVisualization(nlm::NetworkTopology& topology, const std::string& filename, const std::string& format = "json");
    
    // Import memory visualization data
    bool importMemoryVisualization(std::vector<nlm::MemoryTrace>& memoryTraces, const std::string& filename, const std::string& format = "json");
    
    // Import neuromodulation visualization data
    bool importNeuromodulationVisualization(std::vector<nlm::NeuromodulationData>& neuromodulationData, const std::string& filename, const std::string& format = "json");
    
    // Import plasticity visualization data
    bool importPlasticityVisualization(std::vector<nlm::PlasticityData>& plasticityData, const std::string& filename, const std::string& format = "json");
    
    // Import development visualization data
    bool importDevelopmentVisualization(std::vector<nlm::DevelopmentData>& developmentData, const std::string& filename, const std::string& format = "json");
    
    // Import analysis data
    bool importAnalysis(nlm::PerformanceAnalyzer& performanceAnalyzer, const std::string& filename, const std::string& format = "json");
    bool importAnalysis(nlm::LearningAnalyzer& learningAnalyzer, const std::string& filename, const std::string& format = "json");
    bool importAnalysis(nlm::NetworkAnalyzer& networkAnalyzer, const std::string& filename, const std::string& format = "json");
    bool importAnalysis(nlm::SystemAnalyzer& systemAnalyzer, const std::string& filename, const std::string& format = "json");
    
    // Import multiple visualization data sets
    bool importMultiple(std::map<std::string, std::string>& filenames, nlm::NVMVisualization& visualization);
    
    // Set import options
    void setImportOptions(const std::map<std::string, std::string>& options);
    
    // Get import options
    std::map<std::string, std::string> getImportOptions() const;
    
    // Validate import filename
    bool validateImportFilename(const std::string& filename) const;
    
    // Get supported import formats
    std::vector<std::string> getSupportedImportFormats() const;
    
    // Check if importer is available
    bool isAvailable() const;
    
    // Check if file exists
    bool fileExists(const std::string& filename) const;
    
    // Get file size
    size_t getFileSize(const std::string& filename) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Import from JSON format
    bool importFromJSON(const std::string& filename, json& data);
    
    // Import from CSV format
    bool importFromCSV(const std::string& filename, json& data);
    
    // Import from XML format
    bool importFromXML(const std::string& filename, json& data);
    
    // Import from binary format
    bool importFromBinary(const std::string& filename, json& data);
    
    // Read JSON from file
    json readJSON(const std::string& filename) const;
    
    // Write JSON to file
    bool writeJSON(const std::string& filename, const json& data) const;
    
    // Convert JSON to visualization data
    bool convertJSONToVisualization(const json& data, nlm::NVMVisualization& visualization) const;
    
    // Convert JSON to network topology
    bool convertJSONToNetworkTopology(const json& data, nlm::NetworkTopology& topology) const;
    
    // Convert JSON to memory traces
    bool convertJSONToMemoryTraces(const json& data, std::vector<nlm::MemoryTrace>& memoryTraces) const;
    
    // Convert JSON to neuromodulation data
    bool convertJSONToNeuromodulationData(const json& data, std::vector<nlm::NeuromodulationData>& neuromodulationData) const;
    
    // Convert JSON to plasticity data
    bool convertJSONToPlasticityData(const json& data, std::vector<nlm::PlasticityData>& plasticityData) const;
    
    // Convert JSON to development data
    bool convertJSONToDevelopmentData(const json& data, std::vector<nlm::DevelopmentData>& developmentData) const;
    
    // Convert JSON to analysis data
    bool convertJSONToPerformanceAnalysis(const json& data, nlm::PerformanceAnalyzer& performanceAnalyzer) const;
    bool convertJSONToLearningAnalysis(const json& data, nlm::LearningAnalyzer& learningAnalyzer) const;
    bool convertJSONToNetworkAnalysis(const json& data, nlm::NetworkAnalyzer& networkAnalyzer) const;
    bool convertJSONToSystemAnalysis(const json& data, nlm::SystemAnalyzer& systemAnalyzer) const;
    
    // Validate JSON data
    bool validateJSON(const json& data, const std::string& type) const;
    
    // Get default import options
    std::map<std::string, std::string> getDefaultImportOptions() const;
};

} // namespace nlm