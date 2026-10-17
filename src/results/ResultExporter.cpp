// NLM Result Exporter
// Implementation of result export in various formats

#include "ResultExporter.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>

namespace nlm {

struct ResultExporter::Impl {
    std::string lastError;
    bool shuttingDown = false;
};

ResultExporter::ResultExporter() : pImpl(std::make_unique<Impl>()) {}
ResultExporter::~ResultExporter() = default;

bool ResultExporter::exportResults(const std::string& filename, const ExportConfig& config,
                                  const std::vector<std::map<std::string, std::string>>& results) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!validateExportConfig(config)) {
        return false;
    }
    
    if (!validateResults(results)) {
        return false;
    }
    
    lastExportStats.outputFile = filename;
    lastExportStats.format = config.format;
    lastExportStats.totalResultsExported = results.size();
    
    bool success = false;
    
    switch (config.format) {
        case ExportFormat::JSON:
            success = exportToJSON(filename, config, results);
            break;
        case ExportFormat::CSV:
            success = exportToCSV(filename, config, results);
            break;
        case ExportFormat::Text:
            success = exportToText(filename, config, results);
            break;
        case ExportFormat::YAML:
            success = exportToYAML(filename, config, results);
            break;
        default:
            NLM_LOG_ERROR("Unsupported export format: " + std::to_string(static_cast<int>(config.format)));
            return false;
    }
    
    if (success) {
        auto endTime = std::chrono::high_resolution_clock::now();
        lastExportStats.exportTime = std::chrono::duration<double>(endTime - startTime).count();
        lastExportStats.bytesWritten = std::filesystem::file_size(filename);
        
        NLM_LOG_INFO("Exported " + std::to_string(results.size()) + " results to " + filename +
                    " in " + std::to_string(lastExportStats.exportTime) + " seconds");
    }
    
    return success;
}

bool ResultExporter::exportToJSON(const std::string& filename, const ExportConfig& config,
                                 const std::vector<std::map<std::string, std::string>>& results) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for JSON export: " + filename);
        return false;
    }
    
    file << "{\n";
    file << "  \"export_info\": {\n";
    file << "    \"timestamp\": \"" << generateTimestamp() << "\",\n";
    file << "    \"format\": \"JSON\",\n";
    file << "    \"total_results\": " << results.size() << "\n";
    file << "  },\n";
    
    if (config.includeMetadata) {
        file << "  \"metadata\": {\n";
        file << "    \"version\": \"1.0.0\",\n";
        file << "    \"generator\": \"NLM Batch Processing System\"\n";
        file << "  },\n";
    }
    
    if (config.includeStatistics) {
        file << "  \"statistics\": {\n";
        file << "    \"average_results\": " << results.size() << ",\n";
        file << "    \"unique_fields\": " << countUniqueFields(results) << "\n";
        file << "  },\n";
    }
    
    if (config.includeRawResults) {
        file << "  \"results\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            file << "    " << formatJSONValue(results[i]);
            
            if (i < results.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        
        file << "  ]\n";
    }
    
    file << "}\n";
    file.close();
    
    return true;
}

bool ResultExporter::exportToCSV(const std::string& filename, const ExportConfig& config,
                                 const std::vector<std::map<std::string, std::string>>& results) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for CSV export: " + filename);
        return false;
    }
    
    // Write header
    std::string header = getCSVHeader(results, config);
    file << header;
    
    // Write rows
    for (const auto& row : results) {
        std::string csvRow = formatCSVRow(row, config);
        file << csvRow;
    }
    
    file.close();
    return true;
}

bool ResultExporter::exportToText(const std::string& filename, const ExportConfig& config,
                                 const std::vector<std::map<std::string, std::string>>& results) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for text export: " + filename);
        return false;
    }
    
    std::string textContent = formatTextTable(results, config);
    file << textContent;
    
    file.close();
    return true;
}

bool ResultExporter::exportToYAML(const std::string& filename, const ExportConfig& config,
                                 const std::vector<std::map<std::string, std::string>>& results) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for YAML export: " + filename);
        return false;
    }
    
    file << "# NLM Batch Processing Results\n";
    file << "# Exported: " << generateTimestamp() << "\n\n";
    
    file << "results:\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        file << "  - " << formatYAMLValue(results[i]);
        
        if (i < results.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

std::string ResultExporter::formatValue(const std::string& value, const std::string& format) const {
    // Format value based on type
    if (value.empty()) return "null";
    
    // Try to parse as number
    try {
        double num = std::stod(value);
        if (format == "json" || format == "yaml") {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << num;
            return oss.str();
        }
    } catch (...) {}
    
    // Try to parse as boolean
    if (value == "true" || value == "false") {
        return value;
    }
    
    // Return as string (with quotes for JSON/YAML)
    if (format == "json" || format == "yaml") {
        return "\"" + escapeString(value, true) + "\"";
    }
    
    return value;
}

std::string ResultExporter::escapeString(const std::string& value, bool escapeSpecial) const {
    if (!escapeSpecial) return value;
    
    std::string result;
    result.reserve(value.size());
    
    for (char c : value) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    result += oss.str();
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string ResultExporter::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time);
    timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
    return timestamp;
}

bool ResultExporter::validateExportConfig(const ExportConfig& config) const {
    if (config.outputFile.empty()) {
        pImpl->lastError = "Output file cannot be empty";
        return false;
    }
    
    if (config.format == ExportFormat::Unknown) {
        pImpl->lastError = "Unknown export format";
        return false;
    }
    
    return true;
}

bool ResultExporter::validateResults(const std::vector<std::map<std::string, std::string>>& results) const {
    if (results.empty()) {
        pImpl->lastError = "No results to export";
        return false;
    }
    
    // Validate each result row
    for (const auto& row : results) {
        for (const auto& pair : row) {
            if (pair.first.empty()) {
                pImpl->lastError = "Result row contains empty key";
                return false;
            }
        }
    }
    
    return true;
}

std::string ResultExporter::formatJSONValue(const std::map<std::string, std::string>& value) const {
    std::string result = "{\n";
    
    size_t i = 0;
    for (const auto& pair : value) {
        result += "      \"" + escapeString(pair.first, true) + "\": ";
        
        // Try to determine value type
        std::string formattedValue = formatValue(pair.second, "json");
        
        result += formattedValue;
        
        if (i < value.size() - 1) {
            result += ",\n";
        } else {
            result += "\n";
        }
        
        i++;
    }
    
    result += "    }";
    return result;
}

std::string ResultExporter::formatCSVRow(const std::map<std::string, std::string>& row,
                                         const ExportConfig& config) const {
    std::stringstream result;
    
    size_t i = 0;
    for (const auto& pair : row) {
        // Escape quotes in value
        std::string value = pair.second;
        size_t pos = 0;
        while ((pos = value.find('"', pos)) != std::string::npos) {
            value.replace(pos, 1, "\"\"");
            pos += 3;
        }
        
        // Add quotes if string contains special characters
        if (config.quoteAllStrings || value.find(config.delimiter) != std::string::npos ||
            value.find('\n') != std::string::npos || value.find('\r') != std::string::npos) {
            result << "\"" << value << "\"";
        } else {
            result << value;
        }
        
        if (i < row.size() - 1) {
            result << config.delimiter;
        }
        
        i++;
    }
    
    result << "\n";
    return result.str();
}

std::string ResultExporter::getCSVHeader(const std::vector<std::map<std::string, std::string>>& results,
                                         const ExportConfig& config) const {
    std::set<std::string> allKeys;
    
    for (const auto& row : results) {
        for (const auto& pair : row) {
            allKeys.insert(pair.first);
        }
    }
    
    std::string header;
    size_t i = 0;
    
    for (const auto& key : allKeys) {
        header << key;
        if (i < allKeys.size() - 1) {
            header << config.delimiter;
        }
        i++;
    }
    
    header << "\n";
    return header;
}

std::string ResultExporter::formatTextTable(const std::vector<std::map<std::string, std::string>>& results,
                                            const ExportConfig& config) const {
    std::stringstream result;
    
    if (results.empty()) return result.str();
    
    // Determine column widths
    std::map<std::string, size_t> columnWidths;
    
    for (const auto& row : results) {
        for (const auto& pair : row) {
            columnWidths[pair.first] = std::max(columnWidths[pair.first], (size_t)pair.first.length());
            columnWidths[pair.first] = std::max(columnWidths[pair.first], (size_t)pair.second.length());
        }
    }
    
    // Write header
    result << "Results Table\n";
    result << "==============\n\n";
    
    for (const auto& pair : columnWidths) {
        result << std::left << std::setw(pair.second + 2) << pair.first;
    }
    result << "\n";
    
    for (const auto& pair : columnWidths) {
        result << std::left << std::setw(pair.second + 2) << std::setfill('-') << "";
    }
    result << "\n";
    
    // Write rows
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& row = results[i];
        for (const auto& pair : columnWidths) {
            auto it = row.find(pair.first);
            if (it != row.end()) {
                result << std::left << std::setw(pair.second + 2) << it->second;
            } else {
                result << std::left << std::setw(pair.second + 2) << "";
            }
        }
        result << "\n";
    }
    
    return result.str();
}

std::string ResultExporter::formatYAMLValue(const std::map<std::string, std::string>& value) const {
    std::string result = "\n";
    
    for (const auto& pair : value) {
        result += "        " + pair.first + ": " + formatValue(pair.second, "yaml") + "\n";
    }
    
    return result;
}

size_t ResultExporter::countUniqueFields(const std::vector<std::map<std::string, std::string>>& results) const {
    std::set<std::string> uniqueFields;
    
    for (const auto& row : results) {
        for (const auto& pair : row) {
            uniqueFields.insert(pair.first);
        }
    }
    
    return uniqueFields.size();
}

bool ResultExporter::writeToFile(const std::string& filename, const std::string& content,
                                const std::string& compression) {
    // For now, just write directly
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

std::string ResultExporter::compressContent(const std::string& content, const std::string& compression) {
    // For now, return content as-is
    // In real implementation, would compress based on compression type
    return content;
}

} // namespace nlm