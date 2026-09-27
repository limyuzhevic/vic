#include "ExperimentExporter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iomanip>

namespace nlm {

// CSV Exporter Implementation
ExportStatus CSVExporter::export(const Experiment& experiment, 
                                const std::string& filepath, 
                                const ExportOptions& options) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return ExportStatus::FAILED;
        }
        
        std::string csvContent = transformExperimentToCSV(experiment, options);
        file << csvContent;
        file.close();
        
        return ExportStatus::SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "CSV export error: " << e.what() << std::endl;
        return ExportStatus::FAILED;
    }
}

// JSON Exporter Implementation
ExportStatus JSONExporter::export(const Experiment& experiment, 
                                  const std::string& filepath, 
                                  const ExportOptions& options) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return ExportStatus::FAILED;
        }
        
        std::string jsonContent = transformExperimentToJSON(experiment, options);
        file << jsonContent;
        file.close();
        
        return ExportStatus::SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "JSON export error: " << e.what() << std::endl;
        return ExportStatus::FAILED;
    }
}

// YAML Exporter Implementation
ExportStatus YAMLEXporter::export(const Experiment& experiment, 
                                  const std::string& filepath, 
                                  const ExportOptions& options) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return ExportStatus::FAILED;
        }
        
        std::string yamlContent = transformExperimentToYAML(experiment, options);
        file << yamlContent;
        file.close();
        
        return ExportStatus::SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "YAML export error: " << e.what() << std::endl;
        return ExportStatus::FAILED;
    }
}

// ExperimentExporter static utility methods
std::string ExperimentExporter::getTimestamp(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), format.c_str());
    return ss.str();
}

std::string ExperimentExporter::escapeCSV(const std::string& value) {
    std::string escaped = value;
    // Escape quotes by doubling them
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, """");
        pos += 2;
    }
    // Wrap in quotes if contains comma, quote, or newline
    if (escaped.find(',') != std::string::npos || escaped.find('"') != std::string::npos || 
        escaped.find('\n') != std::string::npos || escaped.find('\r') != std::string::npos) {
        escaped = "\"" + escaped + "\"";
    }
    return escaped;
}

std::string ExperimentExporter::escapeJSON(const std::string& value) {
    std::string escaped = value;
    // Replace " with \", \ with \\, and remove control characters
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\"");
        pos += 2;
    }
    pos = 0;
    while ((pos = escaped.find('\\', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = escaped.find('\n', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\n");
        pos += 2;
    }
    pos = 0;
    while ((pos = escaped.find('\r', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\r");
        pos += 2;
    }
    pos = 0;
    while ((pos = escaped.find('\t', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\t");
        pos += 2;
    }
    return escaped;
}

std::string ExperimentExporter::escapeYAML(const std::string& value) {
    std::string escaped = value;
    // Remove problematic YAML characters
    // For simplicity, we'll just return the value as-is
    // In a full implementation, you'd handle quotes, colons, etc.
    return escaped;
}

} // namespace nlm
