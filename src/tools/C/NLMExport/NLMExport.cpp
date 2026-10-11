#include "NLMExport.hpp"
#include <cstring>
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace tools {

int NLMExport::run(int argc, char* argv[]) {
    std::string configFile;
    std::string exportConfigFile;
    std::string exportConfigFormat;
    std::string exportBrainDataFile;
    std::string exportBrainDataFormat;
    std::string exportSimulationResultsFile;
    std::string exportAnalysisDataFile;
    std::string exportAllDirectory;
    bool help = false;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--export-config") {
            if (i + 2 < argc) {
                exportConfigFile = argv[++i];
                exportConfigFormat = argv[++i];
            }
        } else if (arg == "--export-brain-data") {
            if (i + 2 < argc) {
                exportBrainDataFile = argv[++i];
                exportBrainDataFormat = argv[++i];
            }
        } else if (arg == "--export-simulation-results") {
            if (i + 1 < argc) {
                exportSimulationResultsFile = argv[++i];
            }
        } else if (arg == "--export-analysis-data") {
            if (i + 1 < argc) {
                exportAnalysisDataFile = argv[++i];
            }
        } else if (arg == "--export-all") {
            if (i + 1 < argc) {
                exportAllDirectory = argv[++i];
            }
        } else if (!configFile.empty() && configFile == "" && arg.substr(0, 2) != "--") {
            configFile = arg;
        } else if (configFile.empty() && arg.substr(0, 2) != "--") {
            configFile = arg;
        }
    }
    
    if (help) {
        printHelp();
        return 0;
    }
    
    // Load configuration if provided
    std::map<std::string, std::string> config;
    if (!configFile.empty()) {
        if (!loadConfig(configFile, config)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 2;
        }
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    }
    
    // Run exports
    try {
        if (!exportConfigFile.empty()) {
            exportConfig(config, exportConfigFile, exportConfigFormat);
        }
        
        if (!exportBrainDataFile.empty()) {
            auto brain_data = _generateSampleBrainData(config);
            exportBrainData(brain_data, exportBrainDataFile, exportBrainDataFormat);
        }
        
        if (!exportSimulationResultsFile.empty()) {
            auto simulation_results = _generateSampleSimulationResults();
            exportSimulationResults(simulation_results, exportSimulationResultsFile);
        }
        
        if (!exportAnalysisDataFile.empty()) {
            auto analysis_data = _generateSampleAnalysisData();
            exportAnalysisData(analysis_data, exportAnalysisDataFile);
        }
        
        if (!exportAllDirectory.empty()) {
            exportAll(config, exportAllDirectory);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during export: " << e.what() << std::endl;
        return 4;
    }
    
    return 0;
}

bool NLMExport::loadConfig(const std::string& filepath, std::map<std::string, std::string>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Simple parsing for key=value format
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            config[key] = value;
        }
    }
    
    return true;
}

void NLMExport::exportConfig(const std::map<std::string, std::string>& config, const std::string& filepath, const std::string& format) {
    std::cout << "=== Exporting Configuration (" << format << ") ===" << std::endl;
    
    if (config.empty()) {
        std::cerr << "Error: No configuration to export" << std::endl;
        return;
    }
    
    try {
        if (format == "simple") {
            _exportConfigSimple(config, filepath);
        } else if (format == "json") {
            _exportConfigJSON(config, filepath);
        } else if (format == "yaml") {
            _exportConfigYAML(config, filepath);
        } else {
            std::cerr << "Error: Unsupported format: " << format << std::endl;
        }
        
        std::cout << "Configuration exported to: " << filepath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting configuration: " << e.what() << std::endl;
    }
}

void NLMExport::exportBrainData(const std::map<std::string, double>& brainData, const std::string& filepath, const std::string& format) {
    std::cout << "=== Exporting Brain Data (" << format << ") ===" << std::endl;
    
    try {
        if (format == "json") {
            _exportBrainDataJSON(brainData, filepath);
        } else if (format == "binary") {
            _exportBrainDataBinary(brainData, filepath);
        } else if (format == "npy") {
            _exportBrainDataNPY(brainData, filepath);
        } else {
            std::cerr << "Error: Unsupported format: " << format << std::endl;
        }
        
        std::cout << "Brain data exported to: " << filepath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting brain data: " << e.what() << std::endl;
    }
}

void NLMExport::exportSimulationResults(const std::map<std::string, double>& results, const std::string& filepath) {
    std::cout << "=== Exporting Simulation Results ===" << std::endl;
    
    try {
        _exportSimulationResultsJSON(results, filepath);
        std::cout << "Simulation results exported to: " << filepath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting simulation results: " << e.what() << std::endl;
    }
}

void NLMExport::exportAnalysisData(const std::map<std::string, std::map<std::string, double>>& analysisData, const std::string& filepath) {
    std::cout << "=== Exporting Analysis Data ===" << std::endl;
    
    try {
        _exportAnalysisDataJSON(analysisData, filepath);
        std::cout << "Analysis data exported to: " << filepath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting analysis data: " << e.what() << std::endl;
    }
}

void NLMExport::exportAll(const std::map<std::string, std::string>& config, const std::string& outputDir) {
    std::cout << "=== Exporting All Data ===" << std::endl;
    
    // Create output directory
    std::filesystem::create_directories(outputDir);
    
    // Export configuration
    exportConfig(config, outputDir + "/config.json", "json");
    
    // Export brain data
    auto brain_data = _generateSampleBrainData(config);
    exportBrainData(brain_data, outputDir + "/brain_data.json", "json");
    
    // Export simulation results
    auto simulation_results = _generateSampleSimulationResults();
    exportSimulationResults(simulation_results, outputDir + "/simulation_results.json");
    
    // Export analysis data
    auto analysis_data = _generateSampleAnalysisData();
    exportAnalysisData(analysis_data, outputDir + "/analysis_data.json");
    
    std::cout << "All data exported to: " << outputDir << std::endl;
}

void NLMExport::_exportConfigSimple(const std::map<std::string, std::string>& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    for (const auto& pair : config) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
}

void NLMExport::_exportConfigJSON(const std::map<std::string, std::string>& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    // Create JSON object
    json j;
    for (const auto& pair : config) {
        j[pair.first] = pair.second;
    }
    
    file << j.dump(2);
    file.close();
}

void NLMExport::_exportConfigYAML(const std::map<std::string, std::string>& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    // Create YAML representation
    json j;
    for (const auto& pair : config) {
        j[pair.first] = pair.second;
    }
    
    file << j.dump(2); // Note: This is actually JSON output, but YAML export requires external library
    file.close();
}

void NLMExport::_exportBrainDataJSON(const std::map<std::string, double>& brainData, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    json j;
    for (const auto& pair : brainData) {
        j[pair.first] = pair.second;
    }
    
    file << j.dump(2);
    file.close();
}

void NLMExport::_exportBrainDataBinary(const std::map<std::string, double>& brainData, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    // Write number of entries
    size_t count = brainData.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // Write entries
    for (const auto& pair : brainData) {
        // Write key length
        size_t key_len = pair.first.length();
        file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
        
        // Write key
        file.write(pair.first.c_str(), key_len);
        
        // Write value
        file.write(reinterpret_cast<const char*>(&pair.second), sizeof(double));
    }
    
    file.close();
}

void NLMExport::_exportBrainDataNPY(const std::map<std::string, double>& brainData, const std::string& filepath) {
    // For simplicity, we'll create a JSON representation
    _exportBrainDataJSON(brainData, filepath);
}

void NLMExport::_exportSimulationResultsJSON(const std::map<std::string, double>& results, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    json j;
    for (const auto& pair : results) {
        j[pair.first] = pair.second;
    }
    
    file << j.dump(2);
    file.close();
}

void NLMExport::_exportAnalysisDataJSON(const std::map<std::string, std::map<std::string, double>>& analysisData, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    json j;
    for (const auto& category_pair : analysisData) {
        json category_j;
        for (const auto& result_pair : category_pair.second) {
            category_j[result_pair.first] = result_pair.second;
        }
        j[category_pair.first] = category_j;
    }
    
    file << j.dump(2);
    file.close();
}

std::map<std::string, double> NLMExport::_generateSampleBrainData(const std::map<std::string, std::string>& config) {
    std::map<std::string, double> brain_data;
    
    brain_data["neuron_count"] = std::stod(config.at("neuron_count", "1000"));
    brain_data["region_count"] = std::stod(config.at("region_count", "2"));
    brain_data["connection_probability"] = std::stod(config.at("connection_probability", "0.1"));
    brain_data["plasticity_learning_rate"] = std::stod(config.at("plasticity_learning_rate", "0.01"));
    brain_data["firing_rate_mean"] = 5.0;
    brain_data["firing_rate_std"] = 2.0;
    brain_data["memory_capacity"] = 100.0;
    brain_data["learning_rate"] = 0.01;
    
    return brain_data;
}

std::map<std::string, double> NLMExport::_generateSampleSimulationResults() {
    std::map<std::string, double> results;
    
    results["total_reward"] = 100.0;
    results["avg_firing_rate"] = 5.5;
    results["memory_episodes_stored"] = 25.0;
    results["novelty_level"] = 0.3;
    results["curiosity_level"] = 0.4;
    results["dopamine_level"] = 0.2;
    results["total_wall_clock_time"] = 50.0;
    
    return results;
}

std::map<std::string, std::map<std::string, double>> NLMExport::_generateSampleAnalysisData() {
    std::map<std::string, std::map<std::string, double>> analysis_data;
    
    // Connectivity analysis
    std::map<std::string, double> connectivity;
    connectivity["connectivity_density"] = 0.05;
    connectivity["expected_connections"] = 50.0;
    connectivity["actual_connections"] = 60.0;
    connectivity["regional_connectivity_mean"] = 0.8;
    
    analysis_data["connectivity"] = connectivity;
    
    // Clustering analysis
    std::map<std::string, double> clustering;
    clustering["modularity"] = 0.7;
    clustering["clustering_coefficient"] = 0.3;
    clustering["diameter"] = 4.5;
    clustering["radius"] = 2.25;
    
    analysis_data["clustering"] = clustering;
    
    // Topology analysis
    std::map<std::string, double> topology;
    topology["degree_mean"] = 10.5;
    topology["degree_std"] = 8.2;
    topology["betweenness_mean"] = 0.5;
    topology["closeness_mean"] = 0.6;
    topology["eigenvector_mean"] = 0.4;
    topology["network_efficiency"] = 0.8;
    
    analysis_data["topology"] = topology;
    
    // Performance analysis
    std::map<std::string, double> performance;
    performance["learning_rate"] = 0.01;
    performance["convergence_speed"] = 100.0;
    performance["stability_index"] = 0.8;
    performance["generalization"] = 0.7;
    
    analysis_data["performance"] = performance;
    
    return analysis_data;
}

void NLMExport::printHelp() {
    std::cout << "NLMExport - Data Export Tool" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "USAGE: nllexport [OPTIONS] [CONFIG_FILE]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "  Advanced data export capabilities for NLM (Neural Learning Machine)." << std::endl;
    std::cout << "  Supports exporting brain data, configuration, simulation results, and analysis data." << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --export-config FILE FORMAT   Export configuration (simple, json, yaml)" << std::endl;
    std::cout << "  --export-brain-data FILE FORMAT Export brain data (json, binary, npy)" << std::endl;
    std::cout << "  --export-simulation-results FILE Export simulation results" << std::endl;
    std::cout << "  --export-analysis-data FILE Export analysis data" << std::endl;
    std::cout << "  --export-all DIRECTORY         Export all data to directory" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nllexport --export-config config.json json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-brain-data brain_data.json json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-simulation-results results.json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-analysis-data analysis.json configs/default.cfg" << std::endl;
    std::cout << "  nllexport --export-all /path/to/output/configs/default.cfg" << std::endl;
    std::cout << std::endl;
}

std::string NLMExport::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string NLMExport::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

} // namespace tools
} // namespace nlm
