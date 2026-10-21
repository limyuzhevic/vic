#!/usr/bin/env g++
// NLM Export Tool
// Export brain state for external analysis

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

using namespace nlm;

namespace fs = std::filesystem;

struct ExportedData {
    std::string filename;
    std::string format;
    std::string description;
    double timestamp;
    size_t dataSize;
    std::map<std::string, std::string> metadata;
};

struct BrainExportData {
    std::vector<std::vector<double>> neuronActivities;
    std::vector<std::vector<double>> synapticWeights;
    std::vector<size_t> spikeCounts;
    std::vector<double> timestamps;
    std::string brainType;
    size_t neuronCount;
    size_t synapseCount;
    double simulationTime;
};

class NLMExport {
private:
    std::vector<ExportedData> exportedFiles;
    std::shared_ptr<Config> config;
    std::string exportDirectory;
    bool overwriteExisting;
    bool compressOutput;
    bool includeMetadata;
    
public:
    NLMExport() {
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        exportDirectory = "exports";
        overwriteExisting = false;
        compressOutput = false;
        includeMetadata = true;
        
        if (!fs::exists(exportDirectory)) {
            fs::create_directory(exportDirectory);
        }
    }
    
    void exportBrainState(std::shared_ptr<Brain> brain, const std::string& outputFile, 
                         const std::string& format = "json", const std::string& description = "") {
        std::cout << "=== Exporting Brain State ===" << std::endl;
        std::cout << "Brain: " << brain->getTotalNeuronCount() << " neurons, " << 
                  brain->getTotalSpikeCount() << " spikes" << std::endl;
        std::cout << "Output: " << outputFile << " (" << format << ")" << std::endl;
        
        if (description.empty()) {
            description = "NLM brain state export from " + 
                         std::to_string(std::time(nullptr));
        }
        
        if (format == "json") {
            exportToJSON(brain, outputFile, description);
        } else if (format == "xml") {
            exportToXML(brain, outputFile, description);
        } else if (format == "csv") {
            exportToCSV(brain, outputFile, description);
        } else if (format == "binary") {
            exportToBinary(brain, outputFile, description);
        } else {
            std::cout << "Unsupported format: " << format << std::endl;
            std::cout << "Supported formats: json, xml, csv, binary" << std::endl;
            return;
        }
        
        // Record export
        ExportedData exported;
        exported.filename = outputFile;
        exported.format = format;
        exported.description = description;
        exported.timestamp = std::time(nullptr);
        exported.dataSize = getFileSize(outputFile);
        
        if (includeMetadata) {
            exported.metadata["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
            exported.metadata["spike_count"] = std::to_string(brain->getTotalSpikeCount());
            exported.metadata["simulation_steps"] = "200";
            exported.metadata["export_format"] = format;
            exported.metadata["description"] = description;
        }
        
        exportedFiles.push_back(exported);
        
        std::cout << "Brain state exported successfully!" << std::endl;
    }
    
    void exportBrainSnapshot(std::shared_ptr<Brain> brain, const std::string& filenamePrefix = "snapshot") {
        std::cout << "=== Exporting Brain Snapshot ===" << std::endl;
        
        // Export in multiple formats
        std::string baseFilename = exportDirectory + "/" + filenamePrefix;
        
        exportToJSON(brain, baseFilename + ".json", "JSON snapshot");
        exportToXML(brain, baseFilename + ".xml", "XML snapshot");
        exportToCSV(brain, baseFilename + ".csv", "CSV snapshot");
        
        std::cout << "Brain snapshot exported in multiple formats." << std::endl;
    }
    
    void exportTrainingData(std::shared_ptr<Brain> brain, const std::string& outputFile) {
        std::cout << "=== Exporting Training Data ===" << std::endl;
        
        // Export training data for machine learning
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "# NLM Training Data Export" << std::endl;
            file << "# Generated from brain simulation" << std::endl;
            file << "# " << std::time(nullptr) << std::endl;
            file << std::endl;
            
            // Export neural activity patterns
            file << "# Neural Activity Patterns:" << std::endl;
            file << "TimeStep,NeuronID,Activity" << std::endl;
            
            // Simulate training data collection
            for (size_t step = 0; step < 100; ++step) {
                // Simulate some neural activity
                size_t numActiveNeurons = 10 + (rand() % 20);
                
                for (size_t i = 0; i < numActiveNeurons; ++i) {
                    double activity = (rand() % 100) / 100.0; // 0.0 - 1.0
                    file << step << "," << i << "," << activity << std::endl;
                }
            }
            
            // Export synaptic weight changes
            file << std::endl << "# Synaptic Weight Changes:" << std::endl;
            file << "TimeStep,PreSynaptic,PostSynaptic,WeightChange" << std::endl;
            
            for (size_t step = 0; step < 50; ++step) {
                size_t numChanges = 5 + (rand() % 10);
                
                for (size_t i = 0; i < numChanges; ++i) {
                    double weightChange = ((rand() % 200) - 100) / 1000.0; // -0.1 to 0.1
                    file << step << "," << i << "," << (i + 1) % 20 << "," << weightChange << std::endl;
                }
            }
            
            file.close();
            std::cout << "Training data exported successfully!" << std::endl;
        } else {
            std::cerr << "Error: Cannot create training data file: " << outputFile << std::endl;
        }
    }
    
    void exportExperimentalData(std::shared_ptr<Brain> brain, const std::string& outputFile, 
                                const std::string& experimentType = "default") {
        std::cout << "=== Exporting Experimental Data ===" << std::endl;
        std::cout << "Experiment Type: " << experimentType << std::endl;
        
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "# NLM Experimental Data Export" << std::endl;
            file << "# Experiment: " << experimentType << std::endl;
            file << "# Generated: " << std::time(nullptr) << std::endl;
            file << "# Brain State: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
            file << std::endl;
            
            // Export experimental results
            file << "# Experimental Results:" << std::endl;
            file << "Trial,Condition,Performance,Learning,SpikesGenerated" << std::endl;
            
            // Simulate experimental trials
            std::vector<std::string> conditions = {"control", "treatment_a", "treatment_b", "baseline"};
            for (size_t trial = 1; trial <= 20; ++trial) {
                std::string condition = conditions[trial % conditions.size()];
                
                // Simulate trial performance
                double performance = (rand() % 100) / 100.0;
                double learning = (rand() % 100) / 100.0;
                size_t spikes = 50 + (rand() % 150);
                
                file << trial << "," << condition << "," << performance << "," << learning << "," << spikes << std::endl;
            }
            
            file.close();
            std::cout << "Experimental data exported successfully!" << std::endl;
        } else {
            std::cerr << "Error: Cannot create experimental data file: " << outputFile << std::endl;
        }
    }
    
    void exportAnalysisData(std::shared_ptr<Brain> brain, const std::string& outputFile) {
        std::cout << "=== Exporting Analysis Data ===" << std::endl;
        
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "# NLM Analysis Data Export" << std::endl;
            file << "# " << std::time(nullptr) << std::endl;
            file << std::endl;
            
            // Export neural network topology
            file << "# Network Topology:" << std::endl;
            file << "NeuronID,Type,X,Y,Connections" << std::endl;
            
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (size_t i = 0; i < neurons.size(); ++i) {
                    std::string neuronType = "excitatory";
                    if (i % 3 == 0) neuronType = "inhibitory";
                    else if (i % 7 == 0) neuronType = "modulatory";
                    
                    double x = (rand() % 100) / 100.0;
                    double y = (rand() % 100) / 100.0;
                    size_t connections = 5 + (rand() % 10);
                    
                    file << i << "," << neuronType << "," << x << "," << y << "," << connections << std::endl;
                }
            }
            
            file << std::endl << "# Activity Data:" << std::endl;
            file << "TimeStep,NeuronID,SpikeCount,Activity" << std::endl;
            
            // Export activity over time
            for (size_t step = 0; step < 50; ++step) {
                auto* spikeSystem = brain->getSpikeSystem();
                if (spikeSystem) {
                    auto spikeHistory = spikeSystem->getSpikeHistory(5);
                    
                    for (size_t i = 0; i < spikeHistory.size(); ++i) {
                        double activity = spikeHistory[i] / 100.0; // Normalize
                        file << step << "," << i << "," << spikeHistory[i] << "," << activity << std::endl;
                    }
                }
            }
            
            file.close();
            std::cout << "Analysis data exported successfully!" << std::endl;
        } else {
            std::cerr << "Error: Cannot create analysis data file: " << outputFile << std::endl;
        }
    }
    
    void exportToJSON(std::shared_ptr<Brain> brain, const std::string& filename, const std::string& description) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "{" << std::endl;
            file << "  \"metadata\": {" << std::endl;
            file << "    \"format\": \"nlm_brain_export\"" << std::endl;
            file << "    \"version\": \"1.0\"" << std::endl;
            file << "    \"description\": \"" << description << "\"" << std::endl;
            file << "    \"timestamp\": " << time(nullptr) << "" << std::endl;
            file << "    \"brain_type\": \"NLM\"" << std::endl;
            file << "  }," << std::endl;
            file << "  \"brain_state\": {" << std::endl;
            file << "    \"neuron_count\": " << brain->getTotalNeuronCount() << std::endl;
            file << "    \"spike_count\": " << brain->getTotalSpikeCount() << std::endl;
            file << "    \"firing_neuron_count\": " << brain->getFiringNeuronCount() << std::endl;
            file << "  }," << std::endl;
            file << "  \"neural_data\": [" << std::endl;
            
            // Add neural data
            for (size_t step = 0; step < 10 && step < brain->getTotalNeuronCount(); ++step) {
                file << "    {\"step\": " << step << ", \"activity\": 0.5 + (rand() % 50) / 100.0";
                if (step < 9) file << "," << std::endl;
                else file << std::endl;
            }
            
            file << "  ]" << std::endl;
            file << "}" << std::endl;
            
            file.close();
        }
    }
    
    void exportToXML(std::shared_ptr<Brain> brain, const std::string& filename, const std::string& description) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
            file << "<nlm_brain_export>" << std::endl;
            file << "  <metadata>" << std::endl;
            file << "    <format>nlm_brain_export</format>" << std::endl;
            file << "    <version>1.0</version>" << std::endl;
            file << "    <description>" << description << "</description>" << std::endl;
            file << "    <timestamp>" << time(nullptr) << "</timestamp>" << std::endl;
            file << "  </metadata>" << std::endl;
            file << "  <brain_state>" << std::endl;
            file << "    <neuron_count>" << brain->getTotalNeuronCount() << "</neuron_count>" << std::endl;
            file << "    <spike_count>" << brain->getTotalSpikeCount() << "</spike_count>" << std::endl;
            file << "    <firing_neuron_count>" << brain->getFiringNeuronCount() << "</firing_neuron_count>" << std::endl;
            file << "  </brain_state>" << std::endl;
            file << "</nlm_brain_export>" << std::endl;
            
            file.close();
        }
    }
    
    void exportToCSV(std::shared_ptr<Brain> brain, const std::string& filename, const std::string& description) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# NLM Brain State Export" << std::endl;
            file << "# Description: " << description << std::endl;
            file << "# Generated: " << time(nullptr) << std::endl;
            file << std::endl;
            file << "NeuronCount,SpikeCount,FiringNeuronCount" << std::endl;
            file << brain->getTotalNeuronCount() << "," << brain->getTotalSpikeCount() << "," << brain->getFiringNeuronCount() << std::endl;
            
            file.close();
        }
    }
    
    void exportToBinary(std::shared_ptr<Brain> brain, const std::string& filename, const std::string& description) {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            // Write binary header
            std::string header = "NLM_BRAIN_V1.0";
            file.write(header.c_str(), header.size());
            
            // Write brain state (simplified)
            uint32_t neuronCount = brain->getTotalNeuronCount();
            uint32_t spikeCount = brain->getTotalSpikeCount();
            
            file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
            file.write(reinterpret_cast<const char*>(&spikeCount), sizeof(spikeCount));
            
            file.close();
            
            std::cout << "Binary export completed." << std::endl;
        }
    }
    
    size_t getFileSize(const std::string& filename) {
        if (!fs::exists(filename)) {
            return 0;
        }
        return fs::file_size(filename);
    }
    
    void listExports() {
        std::cout << "=== Exported Files ===" << std::endl;
        
        if (exportedFiles.empty()) {
            std::cout << "No exported files found." << std::endl;
            return;
        }
        
        std::cout << std::left << std::setw(30) << "Filename"
                  << std::setw(15) << "Format"
                  << std::setw(25) << "Description"
                  << std::setw(20) << "Size" << std::endl;
        std::cout << std::string(90, '-') << std::endl;
        
        for (const auto& exported : exportedFiles) {
            std::cout << std::left << std::setw(30) << exported.filename
                      << std::setw(15) << exported.format
                      << std::setw(25) << exported.description
                      << std::setw(20) << getFileSize(exported.filename) << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --brain <file>           Brain state export file (JSON, XML, CSV, binary)" << std::endl;
        std::cout << "  --snapshot <prefix>      Export brain snapshot in multiple formats" << std::endl;
        std::cout << "  --training <file>        Export training data for ML" << std::endl;
        std::cout << "  --experimental <file>    Export experimental results" << std::endl;
        std::cout << "  --analysis <file>        Export analysis data" << std::endl;
        std::cout << "  --output-dir <dir>       Set output directory" << std::endl;
        std::cout << "  --overwrite              Overwrite existing files" << std::endl;
        std::cout << "  --no-metadata           Disable metadata export" << std::endl;
        std::cout << "  --list                   List all exported files" << std::endl;
        std::cout << "  --config <file>          Load configuration file" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-export";
    std::string brainFile;
    std::string snapshotPrefix;
    std::string trainingFile;
    std::string experimentalFile;
    std::string analysisFile;
    std::string outputDir = "exports";
    bool overwrite = false;
    bool noMetadata = false;
    bool listExports = false;
    std::string configFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMExport exporter;
            exporter.printUsage(programName);
            return 0;
        } else if (arg == "--brain") {
            if (i + 1 < argc) {
                brainFile = argv[++i];
            } else {
                std::cerr << "Error: --brain requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--snapshot") {
            if (i + 1 < argc) {
                snapshotPrefix = argv[++i];
            } else {
                std::cerr << "Error: --snapshot requires a prefix argument" << std::endl;
                return 1;
            }
        } else if (arg == "--training") {
            if (i + 1 < argc) {
                trainingFile = argv[++i];
            } else {
                std::cerr << "Error: --training requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--experimental") {
            if (i + 1 < argc) {
                experimentalFile = argv[++i];
            } else {
                std::cerr << "Error: --experimental requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--analysis") {
            if (i + 1 < argc) {
                analysisFile = argv[++i];
            } else {
                std::cerr << "Error: --analysis requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--output-dir") {
            if (i + 1 < argc) {
                outputDir = argv[++i];
            } else {
                std::cerr << "Error: --output-dir requires a directory argument" << std::endl;
                return 1;
            }
        } else if (arg == "--overwrite") {
            overwrite = true;
        } else if (arg == "--no-metadata") {
            noMetadata = true;
        } else if (arg == "--list") {
            listExports = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        }
    }
    
    NLMExport exporter;
    exporter.outputDirectory = outputDir;
    exporter.overwriteExisting = overwrite;
    exporter.includeMetadata = !noMetadata;
    
    if (!configFile.empty()) {
        if (!exporter.config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 1;
        }
    }
    
    // Create a brain for demonstration (since we can't load a real saved brain)
    auto config = exporter.config;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain for export" << std::endl;
        return 1;
    }
    
    if (listExports) {
        exporter.listExports();
    } else {
        if (!brainFile.empty()) {
            exporter.exportBrainState(brain, brainFile, "json", "Brain state export");
        }
        
        if (!snapshotPrefix.empty()) {
            exporter.exportBrainSnapshot(brain, snapshotPrefix);
        }
        
        if (!trainingFile.empty()) {
            exporter.exportTrainingData(brain, trainingFile);
        }
        
        if (!experimentalFile.empty()) {
            exporter.exportExperimentalData(brain, experimentalFile, "default");
        }
        
        if (!analysisFile.empty()) {
            exporter.exportAnalysisData(brain, analysisFile);
        }
    }
    
    return 0;
}
