#include "ExperimentExporter.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace nlm {

struct ExperimentResultExporter::Impl {
    std::vector<std::unique_ptr<ExperimentExporter>> exporters;
    ExportProgressCallback progressCallback;
};

ExperimentResultExporter::ExperimentResultExporter() : pImpl(std::make_unique<Impl>()) {
    // Register default exporters
    pImpl->exporters.push_back(std::make_unique<CSVExporter>());
    pImpl->exporters.push_back(std::make_unique<JSONExporter>());
    pImpl->exporters.push_back(std::make_unique<YAMLEXporter>());
}

ExperimentResultExporter::~ExperimentResultExporter() = default;

void ExperimentResultExporter::registerExporter(std::unique_ptr<ExperimentExporter> exporter) {
    if (exporter) {
        pImpl->exporters.push_back(std::move(exporter));
    }
}

ExportStatus ExperimentResultExporter::export(const Experiment& experiment,
                                              const std::string& filepath,
                                              ExportFormat format,
                                              const ExportOptions& options) {
    // Find exporter for the specified format
    ExperimentExporter* selectedExporter = nullptr;
    for (auto& exporter : pImpl->exporters) {
        std::vector<std::string> extensions = exporter->getSupportedExtensions();
        for (const auto& ext : extensions) {
            if ((format == ExportFormat::CSV && ext == ".csv") ||
                (format == ExportFormat::JSON && ext == ".json") ||
                (format == ExportFormat::YAML && (ext == ".yaml" || ext == ".yml"))) {
                selectedExporter = exporter.get();
                break;
            }
        }
        if (selectedExporter) break;
    }
    
    if (!selectedExporter) {
        return ExportStatus::FAILED;
    }
    
    // Update progress callback
    if (pImpl->progressCallback) {
        pImpl->progressCallback(0.0, "Starting export...");
    }
    
    // Perform export
    ExportStatus status = selectedExporter->export(experiment, filepath, options);
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(1.0, status == ExportStatus::SUCCESS ? "Export completed successfully" : "Export failed");
    }
    
    return status;
}

std::vector<ExportFormat> ExperimentResultExporter::getAvailableFormats() const {
    std::vector<ExportFormat> formats;
    
    for (const auto& exporter : pImpl->exporters) {
        std::string formatName = exporter->getFormatName();
        if (formatName.find("CSV") != std::string::npos) {
            formats.push_back(ExportFormat::CSV);
        } else if (formatName.find("JSON") != std::string::npos) {
            formats.push_back(ExportFormat::JSON);
        } else if (formatName.find("YAML") != std::string::npos) {
            formats.push_back(ExportFormat::YAML);
        }
    }
    
    return formats;
}

std::string ExperimentResultExporter::getExporterName(ExportFormat format) const {
    for (const auto& exporter : pImpl->exporters) {
        std::vector<std::string> extensions = exporter->getSupportedExtensions();
        for (const auto& ext : extensions) {
            if ((format == ExportFormat::CSV && ext == ".csv") ||
                (format == ExportFormat::JSON && ext == ".json") ||
                (format == ExportFormat::YAML && (ext == ".yaml" || ext == ".yml"))) {
                return exporter->getFormatName();
            }
        }
    }
    return "Unknown format";
}

std::vector<std::string> ExperimentResultExporter::getExporterExtensions(ExportFormat format) const {
    std::vector<std::string> extensions;
    
    for (const auto& exporter : pImpl->exporters) {
        std::vector<std::string> exts = exporter->getSupportedExtensions();
        for (const auto& ext : exts) {
            if ((format == ExportFormat::CSV && ext == ".csv") ||
                (format == ExportFormat::JSON && ext == ".json") ||
                (format == ExportFormat::YAML && (ext == ".yaml" || ext == ".yml"))) {
                extensions.push_back(ext);
            }
        }
    }
    
    return extensions;
}

std::vector<ExportStatus> ExperimentResultExporter::exportMultiple(const std::vector<std::unique_ptr<Experiment>>& experiments,
                                                                   const std::string& outputDirectory,
                                                                   ExportFormat format,
                                                                   const ExportOptions& options) {
    std::vector<ExportStatus> results;
    results.reserve(experiments.size());
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        if (pImpl->progressCallback) {
            double progress = static_cast<double>(i) / experiments.size();
            pImpl->progressCallback(progress, "Exporting experiment " + std::to_string(i + 1) + "/" + std::to_string(experiments.size()));
        }
        
        std::string filename = options.filename;
        if (filename.empty()) {
            filename = "experiment_" + std::to_string(i + 1) + "." + 
                      (format == ExportFormat::CSV ? "csv" : format == ExportFormat::JSON ? "json" : "yaml");
        }
        
        std::string filepath = outputDirectory + "/" + filename;
        
        ExportStatus status = export(*experiments[i], filepath, format, options);
        results.push_back(status);
    }
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(1.0, "Batch export completed");
    }
    
    return results;
}

ExportStatus ExperimentResultExporter::convertFormat(const std::string& inputFile,
                                                     const std::string& outputFile,
                                                     ExportFormat targetFormat,
                                                     const ExportOptions& options) {
    // Simple format conversion - in a real implementation, you'd parse the input
    // and export in the target format
    
    // For now, just copy the file with different extension
    std::ifstream src(inputFile, std::ios::binary);
    if (!src) {
        return ExportStatus::FAILED;
    }
    
    std::string outputExt = targetFormat == ExportFormat::CSV ? ".csv" :
                           targetFormat == ExportFormat::JSON ? ".json" : ".yaml";
    
    std::string actualOutputFile = outputFile;
    if (actualOutputFile.substr(actualOutputFile.find_last_of(".")) != outputExt) {
        actualOutputFile = outputFile.substr(0, outputFile.find_last_of(".")) + outputExt;
    }
    
    std::ofstream dst(actualOutputFile, std::ios::binary);
    if (!dst) {
        return ExportStatus::FAILED;
    }
    
    dst << src.rdbuf();
    src.close();
    dst.close();
    
    return ExportStatus::SUCCESS;
}

bool ExperimentResultExporter::validateExportOptions(ExportFormat format, const ExportOptions& options) const {
    // Basic validation
    if (options.filename.empty()) {
        return false;
    }
    
    // Check if format is supported
    auto availableFormats = getAvailableFormats();
    if (std::find(availableFormats.begin(), availableFormats.end(), format) == availableFormats.end()) {
        return false;
    }
    
    // Check filename extension matches format
    std::string extension = ".csv";
    if (format == ExportFormat::JSON) extension = ".json";
    if (format == ExportFormat::YAML) extension = ".yaml";
    
    if (options.filename.find(extension) == std::string::npos) {
        // Not a strict requirement, but warning-worthy
        return false;
    }
    
    return true;
}

void ExperimentResultExporter::setProgressCallback(ExportProgressCallback callback) {
    pImpl->progressCallback = std::move(callback);
}

} // namespace nlm
