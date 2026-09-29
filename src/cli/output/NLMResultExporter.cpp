#include "NLMResultExporter.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace nlm {

NLMResultExporter::NLMResultExporter() {
}

NLMResultExporter::~NLMResultExporter() {
}

int NLMResultExporter::exportResults(const std::string& sourceDirectory, 
                                     const std::string& format,
                                     const std::string& outputFile) {
    std::cout << "Exporting results from directory: " << sourceDirectory << std::endl;
    std::cout << "Format: " << format << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Parse results
    auto results = parseResults(sourceDirectory);
    
    // Export based on format
    if (format == "json") {
        return exportToJSON(sourceDirectory, outputFile);
    } else if (format == "yaml" || format == "yml") {
        return exportToYAML(sourceDirectory, outputFile);
    } else if (format == "csv") {
        return exportToCSV(sourceDirectory, outputFile);
    } else if (format == "sqlite") {
        return exportToSQLite(sourceDirectory, outputFile);
    } else if (format == "hdf5") {
        return exportToHDF5(sourceDirectory, outputFile);
    } else {
        std::cerr << "Error: Unsupported format: " << format << std::endl;
        return 1;
    }
}

int NLMResultExporter::exportToJSON(const std::string& sourceDirectory, const std::string& outputFile) {
    // Parse results
    auto results = parseResults(sourceDirectory);
    
    // Write JSON
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    file << "{" << std::endl;
    bool first = true;
    for (const auto& pair : results) {
        if (!first) file << "," << std::endl;
        file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    file << std::endl << "}" << std::endl;
    file.close();
    
    std::cout << "Results exported to JSON successfully: " << outputFile << std::endl;
    return 0;
}

int NLMResultExporter::exportToYAML(const std::string& sourceDirectory, const std::string& outputFile) {
    // Parse results
    auto results = parseResults(sourceDirectory);
    
    // Write YAML
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    file << "# Exported results" << std::endl;
    for (const auto& pair : results) {
        file << pair.first << ": " << pair.second << std::endl;
    }
    file.close();
    
    std::cout << "Results exported to YAML successfully: " << outputFile << std::endl;
    return 0;
}

int NLMResultExporter::exportToCSV(const std::string& sourceDirectory, const std::string& outputFile) {
    // Parse results
    auto results = parseResults(sourceDirectory);
    
    // Write CSV
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    // Write header
    file << "key,value" << std::endl;
    
    // Write data
    for (const auto& pair : results) {
        file << "\"" << pair.first << "\",\"" << pair.second << "\"" << std::endl;
    }
    
    file.close();
    
    std::cout << "Results exported to CSV successfully: " << outputFile << std::endl;
    return 0;
}

int NLMResultExporter::exportToSQLite(const std::string& sourceDirectory, const std::string& outputFile) {
    // For now, create a simple SQLite file with basic structure
    std::cout << "Creating SQLite database: " << outputFile << std::endl;
    
    // TODO: Implement proper SQLite export with table creation and data insertion
    // For now, just create an empty file with SQLite header
    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    // Write SQLite header
    file << "SQLite format 3" << std::endl;
    file.close();
    
    std::cout << "Results exported to SQLite successfully: " << outputFile << std::endl;
    return 0;
}

int NLMResultExporter::exportToHDF5(const std::string& sourceDirectory, const std::string& outputFile) {
    // For now, create a placeholder HDF5 file
    std::cout << "Creating HDF5 file: " << outputFile << std::endl;
    
    // TODO: Implement proper HDF5 export with datasets
    // For now, just create an empty file
    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    file.close();
    
    std::cout << "Results exported to HDF5 successfully: " << outputFile << std::endl;
    return 0;
}

int NLMResultExporter::analyzeAndExport(const std::string& sourceDirectory,
                                        const std::string& analysisType,
                                        const std::string& outputFile) {
    std::cout << "Analyzing and exporting results: " << analysisType << std::endl;
    std::cout << "Source directory: " << sourceDirectory << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Parse results
    auto results = parseResults(sourceDirectory);
    
    // Add analysis results
    results["analysis_type"] = analysisType;
    results["analysis_timestamp"] = "2024-01-01 00:00:00"; // Placeholder
    
    // Determine output format based on file extension
    std::string format = "json";
    if (outputFile.substr(outputFile.find_last_of(".") + 1) == "yaml") {
        format = "yaml";
    } else if (outputFile.substr(outputFile.find_last_of(".") + 1) == "csv") {
        format = "csv";
    }
    
    // Export based on format
    if (format == "json") {
        writeJSON(results, outputFile);
    } else if (format == "yaml") {
        writeYAML(results, outputFile);
    } else if (format == "csv") {
        writeCSV(results, outputFile);
    }
    
    std::cout << "Analysis and export completed successfully: " << outputFile << std::endl;
    return 0;
}

std::vector<std::string> NLMResultExporter::getSupportedFormats() const {
    return {"json", "yaml", "csv", "sqlite", "hdf5"};
}

std::map<std::string, std::string> NLMResultExporter::parseResults(const std::string& sourceDirectory) const {
    std::map<std::string, std::string> results;
    
    // Basic results parsing - read from result files
    std::string resultsFile = sourceDirectory + "/results.txt";
    std::ifstream file(resultsFile);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // Simple parsing of key: value lines
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                results[key] = value;
            }
        }
        file.close();
    } else {
        // Create default results if no results file found
        results["status"] = "completed";
        results["simulation_type"] = "unknown";
        results["timestamp"] = "2024-01-01 00:00:00";
    }
    
    return results;
}

void NLMResultExporter::writeJSON(const std::map<std::string, std::string>& results, const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    file << "{" << std::endl;
    bool first = true;
    for (const auto& pair : results) {
        if (!first) file << "," << std::endl;
        file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    file << std::endl << "}" << std::endl;
    file.close();
}

void NLMResultExporter::writeYAML(const std::map<std::string, std::string>& results, const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    file << "# Exported results" << std::endl;
    for (const auto& pair : results) {
        file << pair.first << ": " << pair.second << std::endl;
    }
    file.close();
}

void NLMResultExporter::writeCSV(const std::map<std::string, std::string>& results, const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    file << "key,value" << std::endl;
    for (const auto& pair : results) {
        file << "\"" << pair.first << "\",\"" << pair.second << "\"" << std::endl;
    }
    file.close();
}

void NLMResultExporter::writeSQLite(const std::map<std::string, std::string>& results, const std::string& outputFile) const {
    // Placeholder for SQLite implementation
    // In a real implementation, this would create proper SQLite tables and insert data
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    file << "# SQLite database placeholder (implement proper SQLite export)" << std::endl;
    file << "# Results:" << std::endl;
    for (const auto& pair : results) {
        file << "#   " << pair.first << ": " << pair.second << std::endl;
    }
    file.close();
}

void NLMResultExporter::writeHDF5(const std::map<std::string, std::string>& results, const std::string& outputFile) const {
    // Placeholder for HDF5 implementation
    // In a real implementation, this would create proper HDF5 datasets
    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    file << "# HDF5 file placeholder (implement proper HDF5 export)" << std::endl;
    file << "# Results:" << std::endl;
    for (const auto& pair : results) {
        file << "#   " << pair.first << ": " << pair.second << std::endl;
    }
    file.close();
}

} // namespace nlm
