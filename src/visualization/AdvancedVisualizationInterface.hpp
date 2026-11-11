#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <nlohmann/json.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using json = nlohmann::json;

namespace nlm {

// Model export formats
enum class ExportFormat {
    Binary,
    JSON,
    CSV,
    HDF5,
    NumPy
};

// Model export options
struct ExportOptions {
    ExportFormat format;
    bool includeMetadata;
    bool includePlasticity;
    bool includeMemory;
    bool includeDevelopment;
    bool includeNeuromodulation;
    double scaleFactor;
    std::string version;
    std::map<std::string, std::string> customMetadata;
    
    ExportOptions() : format(ExportFormat::JSON),
                     includeMetadata(true),
                     includePlasticity(true),
                     includeMemory(true),
                     includeDevelopment(true),
                     includeNeuromodulation(true),
                     scaleFactor(1.0),
                     version("2.0.0") {}
};

// Model statistics
struct ModelStats {
    double totalSize;
    size_t neuronCount;
    size_t synapseCount;
    size_t regionCount;
    double firingRate;
    double plasticityCount;
    double memoryUsage;
    double trainingSteps;
    double learningRate;
    std::map<std::string, double> componentStats;
    
    ModelStats() : totalSize(0.0), neuronCount(0), synapseCount(0), regionCount(0),
                   firingRate(0.0), plasticityCount(0.0), memoryUsage(0.0),
                   trainingSteps(0.0), learningRate(0.0) {}
};

// Advanced visualization interface with real implementation
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with specified backend
    bool initialize(const std::string& backend = "console");
    
    // Update visualization data
    void update();
    
    // Render current state with enhanced visualization modes
    void render(const Brain& brain, const std::string& mode = "default");
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Enhanced visualization methods
    void visualizeNetwork(const Brain& brain, bool showWeights = true);
    void visualizeActivity(const Brain& brain, bool showRaster = true);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain, bool showSynapticStrength = true);
    
    // Specialized visualization modes
    void visualizeDevelopment(const Brain& brain);
    void visualizeMemory(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);
    void visualizeLearning(const Brain& brain);
    
    // Set visualization options
    void setVisualizationOptions(const json& options);
    json getVisualizationOptions() const;
    
    // Export visualization frames
    bool exportFrame(const std::string& filename);
    bool exportSequence(const std::string& directory, int maxFrames = 100);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Model export utilities
class ModelExporter {
public:
    ModelExporter();
    ~ModelExporter();
    
    // Export brain model to file
    bool exportModel(const Brain& brain, const std::string& filepath,
                    const ExportOptions& options = ExportOptions());
    
    // Export brain model to JSON
    json exportToJson(const Brain& brain, const ExportOptions& options = ExportOptions());
    
    // Export brain model to CSV
    bool exportToCsv(const Brain& brain, const std::string& directory,
                    const ExportOptions& options = ExportOptions());
    
    // Export neural network topology
    bool exportTopology(const Brain& brain, const std::string& filepath);
    
    // Export synaptic weights
    bool exportWeights(const Brain& brain, const std::string& filepath);
    
    // Export neuron states
    bool exportNeurons(const Brain& brain, const std::string& filepath);
    
    // Export spike statistics
    bool exportSpikeStats(const Brain& brain, const std::string& filepath);
    
    // Export learning statistics
    bool exportLearningStats(const Brain& brain, const std::string& filepath);
    
    // Get model statistics
    ModelStats calculateStats(const Brain& brain) const;
    
    // Validate model before export
    bool validateModel(const Brain& brain, std::vector<std::string>& errors) const;
    
    // Set export backend
    void setExportBackend(const std::string& backend);
    std::string getExportBackend() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Python interface for model export and visualization
namespace python {

// Enhanced visualization factory
std::shared_ptr<VisualizationInterface> createVisualization(const std::string& backend = "console");

// Enhanced model exporter factory  
std::shared_ptr<ModelExporter> createModelExporter(const std::string& backend = "json");

// Convenience functions for common visualization tasks
void visualizeBrain(const Brain& brain, const std::string& outputFile = "");
void exportBrainModel(const Brain& brain, const std::string& outputFile, 
                     const std::string& format = "json");

// Advanced learning visualization
void visualizeLearningProgress(const Brain& brain, int maxSteps = 1000);
void visualizePerformanceMetrics(const Brain& brain, const std::vector<double>& metrics);

// Model comparison tools
void compareModels(const std::vector<std::shared_ptr<Brain>>& brains,
                  const std::vector<std::string>& labels);

// Export utilities for Python integration
json exportBrainForPython(const Brain& brain, const std::map<std::string, bool>& includeComponents);

} // namespace python

} // namespace nlm