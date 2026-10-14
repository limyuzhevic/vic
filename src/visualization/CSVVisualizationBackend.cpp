#include "VisualizationFramework.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

struct CSVVisualizationBackend::Impl {
    char separator;
    char quote;
    bool includeHeaders;
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> dataRows;
    
    Impl() : separator(','), quote('"'), includeHeaders(true) {}
};

CSVVisualizationBackend::CSVVisualizationBackend() : pImpl(std::make_unique<Impl>()) {}
CSVVisualizationBackend::~CSVVisualizationBackend() = default;

bool CSVVisualizationBackend::initialize(const std::unordered_map<std::string, std::any>& config) {
    try {
        if (config.find("separator") != config.end()) {
            pImpl->separator = std::any_cast<char>(config.at("separator"));
        }
        if (config.find("quote") != config.end()) {
            pImpl->quote = std::any_cast<char>(config.at("quote"));
        }
        if (config.find("headers") != config.end()) {
            pImpl->includeHeaders = std::any_cast<bool>(config.at("headers"));
        }
        
        pImpl->dataRows.clear();
        pImpl->columnNames.clear();
        return true;
    } catch (...) {
        return false;
    }
}

void CSVVisualizationBackend::shutdown() {
    pImpl->dataRows.clear();
    pImpl->columnNames.clear();
}

bool CSVVisualizationBackend::isActive() const {
    return !pImpl->dataRows.empty() || !pImpl->columnNames.empty();
}

void CSVVisualizationBackend::update(const VisualizationData& data) {
    if (data.format == "csv" || data.format.empty()) {
        std::stringstream ss(data.content);
        std::string line;
        size_t row = 0;
        
        while (std::getline(ss, line)) {
            std::vector<std::string> rowData;
            std::stringstream lineStream(line);
            std::string cell;
            
            while (std::getline(lineStream, cell, pImpl->separator)) {
                // Remove quotes if present
                if (cell.size() >= 2 && cell.front() == pImpl->quote && cell.back() == pImpl->quote) {
                    cell = cell.substr(1, cell.size() - 2);
                }
                rowData.push_back(cell);
            }
            
            if (row == 0 && pImpl->includeHeaders) {
                pImpl->columnNames = rowData;
            } else {
                pImpl->dataRows.push_back(rowData);
            }
            
            row++;
        }
    }
}

bool CSVVisualizationBackend::render(const std::string& output) {
    try {
        std::stringstream ss;
        
        if (pImpl->includeHeaders && !pImpl->columnNames.empty()) {
            // Write headers
            for (size_t i = 0; i < pImpl->columnNames.size(); ++i) {
                if (i > 0) ss << pImpl->separator;
                ss << pImpl->quote << pImpl->columnNames[i] << pImpl->quote;
            }
            ss << std::endl;
        }
        
        // Write data rows
        for (const auto& row : pImpl->dataRows) {
            for (size_t i = 0; i < row.size(); ++i) {
                if (i > 0) ss << pImpl->separator;
                ss << pImpl->quote << row[i] << pImpl->quote;
            }
            ss << std::endl;
        }
        
        std::string csvContent = ss.str();
        
        if (output.empty()) {
            std::cout << csvContent;
        } else {
            std::ofstream file(output);
            if (file.is_open()) {
                file << csvContent;
                file.close();
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

const char* CSVVisualizationBackend::getName() const {
    return "CSVVisualizationBackend";
}

std::vector<VisualizationFormat> CSVVisualizationBackend::getSupportedFormats() const {
    return {VisualizationFormat::CSV, VisualizationFormat::JSON, VisualizationFormat::HTML};
}

template<typename T>
void CSVVisualizationBackend::addColumn(const std::string& name, const std::vector<T>& data) {
    pImpl->columnNames.push_back(name);
    for (const auto& value : data) {
        pImpl->dataRows.push_back({std::to_string(value)});
    }
}

void CSVVisualizationBackend::setSeparator(char sep) {
    pImpl->separator = sep;
}

void CSVVisualizationBackend::setQuote(char quote) {
    pImpl->quote = quote;
}

void CSVVisualizationBackend::includeHeaders(bool include) {
    pImpl->includeHeaders = include;
}

} // namespace nlm
