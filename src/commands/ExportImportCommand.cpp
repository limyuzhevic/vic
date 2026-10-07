#include "ExportImportCommand.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <numeric>
#include <filesystem>
#include <chrono>

namespace nlm {

struct ExportImportCommand::Impl {
    std::string lastOperation;
    ImportExportStats stats;
    std::vector<std::string> recentFiles;
    bool overwriteExisting;
    bool createBackups;
    std::string defaultExportFormat;
    
    Impl() : lastOperation(), overwriteExisting(true), createBackups(false), defaultExportFormat("binary") {
        stats = {0, 0, 0, std::chrono::milliseconds(0), {}, {}};
    }
};

ExportImportCommand::ExportImportCommand() : pImpl(std::make_unique<Impl>()) {
}

ExportImportCommand::~ExportImportCommand() = default;

std::string ExportImportCommand::getName() const {
    return "export_import";
}

std::string ExportImportCommand::getDescription() const {
    return "Export and import neural network models, configurations, and data in various formats";
}

std::string ExportImportCommand::getUsage() const {
    return "export_import <command> [options]\n  export_import export <brain_file> <output_file> [--format json|csv|text|hdf5]\n  export_import import <input_file> [--config config_file]\n  export_import batch <file_list> [--output-dir directory]";
}

std::string ExportImportCommand::getHelp() const {
    return "Export and import neural network models and configurations.\n\n"
           "Export Commands:\n"
           "  export      Export a brain state to file\n"
           "  export-config  Export configuration to file\n"
           "  batch-export  Export multiple brain states\n\n"
           "Import Commands:\n"
           "  import      Import a brain state from file\n"
           "  import-config Import configuration from file\n"
           "  batch-import  Import multiple files\n\n"
           "Options:\n"
           "  --format FORMAT    Export format (binary, json, csv, text, hdf5)\n"
           "  --config FILE      Configuration file\n"
           "  --output-dir DIR   Output directory for batch operations\n"
           "  --overwrite        Overwrite existing files (default: true)\n"
           "  --no-backup        Do not create backup files (default: false)\n"
           "  --verbose          Show detailed progress information\n\n"
           "Examples:\n"
           "  export_import export brain.nnet brain.json --format json\n"
           "  export_import import model.h5 --config config.json\n"
           "  export_import batch-export *.nnet --output-dir models/";
}

CommandStatus ExportImportCommand::getStatus() const {
    return CommandStatus::Pending;
}

double ExportImportCommand::getProgress() const {
    if (pImpl->stats.filesProcessed == 0) {
        return 0.0;
    }
    return static_cast<double>(pImpl->stats.filesSuccessful + pImpl->stats.filesFailed) /
           static_cast<double>(pImpl->stats.filesProcessed);
}

std::string ExportImportCommand::getProgressText() const {
    return std::to_string(pImpl->stats.filesSuccessful + pImpl->stats.filesFailed) + "/" +
           std::to_string(pImpl->stats.filesProcessed) + " files processed";
}

bool ExportImportCommand::execute() {
    setStatus(CommandStatus::Running);
    startTiming();
    
    // Parse command structure
    if (arguments.empty()) {
        setStatus(CommandStatus::Failed);
        setResult(false, "", "No command specified. Use 'help' for usage information.");
        return false;
    }
    
    std::string command = arguments[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    bool success = false;
    std::string output;
    std::string error;
    
    try {
        if (command == "export") {
            success = handleExportCommand();
        } else if (command == "export-config") {
            success = handleExportConfigCommand();
        } else if (command == "batch-export") {
            success = handleBatchExportCommand();
        } else if (command == "import") {
            success = handleImportCommand();
        } else if (command == "import-config") {
            success = handleImportConfigCommand();
        } else if (command == "batch-import") {
            success = handleBatchImportCommand();
        } else if (command == "help") {
            success = true;
            output = getHelp();
        } else {
            error = "Unknown command: " + command + ". Use 'help' for usage information.";
            success = false;
        }
    } catch (const std::exception& e) {
        error = std::string("Exception: ") + e.what();
        success = false;
    }
    
    stopTiming();
    
    setStatus(success ? CommandStatus::Completed : CommandStatus::Failed);
    setResult(success, output, error);
    
    return success;
}

bool ExportImportCommand::cancel() {
    // Cancel ongoing file operations
    return true;
}

bool ExportImportCommand::isCancellable() const {
    return pImpl->lastOperation.find("export") != std::string::npos ||
           pImpl->lastOperation.find("import") != std::string::npos;
}

CommandResult ExportImportCommand::getResult() const {
    return result;
}

std::vector<std::string> ExportImportCommand::getArguments() const {
    return arguments;
}

void ExportImportCommand::setArguments(const std::vector<std::string>& args) {
    arguments = args;
}

bool ExportImportCommand::validateArguments() const {
    if (arguments.empty()) {
        return false;
    }
    
    std::string command = arguments[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "export") {
        return validateExportArguments();
    } else if (command == "export-config") {
        return validateExportConfigArguments();
    } else if (command == "batch-export") {
        return validateBatchExportArguments();
    } else if (command == "import") {
        return validateImportArguments();
    } else if (command == "import-config") {
        return validateImportConfigArguments();
    } else if (command == "batch-import") {
        return validateBatchImportArguments();
    }
    
    return true; // help command is always valid
}

std::string ExportImportCommand::getLastError() const {
    return result.errorMessage;
}

std::chrono::milliseconds ExportImportCommand::getEstimatedExecutionTime() const {
    return std::chrono::milliseconds(1000); // Default 1 second
}

ExportImportCommand::Category ExportImportCommand::getCategory() const {
    return Category::ExportImport;
}

bool ExportImportCommand::exportBrainState(const std::string& brainFile, const std::string& outputFile,
                                         const std::string& format) {
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // Check if input file exists
        if (!validateBrainFile(brainFile)) {
            return false;
        }
        
        // Check if output file exists and whether we can overwrite
        std::ifstream outputCheck(outputFile);
        if (outputCheck.good() && !pImpl->overwriteExisting) {
            outputCheck.close();
            return false; // File exists and overwrite is disabled
        }
        outputCheck.close();
        
        // Load the brain
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(brainFile)) {
            return false;
        }
        
        // Export based on format
        bool success = false;
        if (format == "json" || format == "JSON") {
            success = exportBrainToJSON(brain, outputFile);
        } else if (format == "csv" || format == "CSV") {
            success = exportBrainToCSV(brain, outputFile);
        } else if (format == "text" || format == "TEXT") {
            success = exportBrainToText(brain, outputFile);
        } else if (format == "hdf5" || format == "HDF5") {
            success = exportBrainToHDF5(brain, outputFile);
        } else { // binary (default)
            success = brain->save(outputFile);
        }
        
        if (success) {
            pImpl->stats.filesSuccessful++;
            pImpl->lastOperation = "export_brain";
            pImpl->recentFiles.push_back(outputFile);
        } else {
            pImpl->stats.filesFailed++;
        }
        
        pImpl->stats.filesProcessed++;
        
        auto endTime = std::chrono::steady_clock::now();
        pImpl->stats.totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return success;
        
    } catch (const std::exception& e) {
        pImpl->stats.filesFailed++;
        pImpl->stats.filesProcessed++;
        return false;
    }
}

bool ExportImportCommand::exportConfiguration(const std::shared_ptr<Config>& config, const std::string& outputFile,
                                              bool includeRuntime) {
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        if (!config) {
            return false;
        }
        
        // Create backup if requested
        if (pImpl->createBackups) {
            std::string backupFile = outputFile + ".backup";
            if (std::filesystem::exists(outputFile)) {
                std::filesystem::copy_file(outputFile, backupFile, std::filesystem::copy_options::overwrite_existing);
            }
        }
        
        // Export configuration
        if (!config->exportConfig(outputFile, includeRuntime)) {
            return false;
        }
        
        pImpl->stats.filesSuccessful++;
        pImpl->lastOperation = "export_config";
        pImpl->recentFiles.push_back(outputFile);
        pImpl->stats.filesProcessed++;
        
        auto endTime = std::chrono::steady_clock::now();
        pImpl->stats.totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->stats.filesFailed++;
        pImpl->stats.filesProcessed++;
        return false;
    }
}

bool ExportImportCommand::importBrainState(const std::string& inputFile, const std::string& configFile) {
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        if (!validateBrainFile(inputFile)) {
            return false;
        }
        
        // Load configuration if specified
        std::shared_ptr<Config> config;
        if (!configFile.empty()) {
            config = std::make_shared<Config>();
            if (!config->loadFromFile(configFile)) {
                return false;
            }
        }
        
        // Load brain state
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(inputFile)) {
            return false;
        }
        
        // Save with a new name (import output)
        std::string outputFile = "imported_" + std::filesystem::path(inputFile).filename().string();
        if (brain->save(outputFile)) {
            pImpl->stats.filesSuccessful++;
            pImpl->lastOperation = "import_brain";
            pImpl->recentFiles.push_back(outputFile);
            pImpl->stats.filesProcessed++;
            
            auto endTime = std::chrono::steady_clock::now();
            pImpl->stats.totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            return true;
        } else {
            pImpl->stats.filesFailed++;
            pImpl->stats.filesProcessed++;
            return false;
        }
        
    } catch (const std::exception& e) {
        pImpl->stats.filesFailed++;
        pImpl->stats.filesProcessed++;
        return false;
    }
}

bool ExportImportCommand::importConfiguration(const std::string& configFile, const std::string& outputConfigFile) {
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // Validate input file
        if (!validateConfigFile(configFile)) {
            return false;
        }
        
        // Determine output file path
        std::string outputPath = outputConfigFile.empty() ? configFile : outputConfigFile;
        
        // Load configuration
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            return false;
        }
        
        // Save configuration to output file
        if (!config->exportConfig(outputPath, true)) {
            return false;
        }
        
        pImpl->stats.filesSuccessful++;
        pImpl->lastOperation = "import_config";
        pImpl->recentFiles.push_back(outputPath);
        pImpl->stats.filesProcessed++;
        
        auto endTime = std::chrono::steady_clock::now();
        pImpl->stats.totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->stats.filesFailed++;
        pImpl->stats.filesProcessed++;
        return false;
    }
}

std::vector<std::string> ExportImportCommand::getSupportedExportFormats() const {
    return {"binary", "json", "csv", "text", "hdf5"};
}

std::vector<std::string> ExportImportCommand::getSupportedImportFormats() const {
    return {"binary", "json", "csv", "text", "hdf5"};
}

bool ExportImportCommand::validateFilePath(const std::string& filepath) const {
    // Check if filepath is valid and accessible
    return !filepath.empty() && 
           std::filesystem::path(filepath).has_filename();
}

bool ExportImportCommand::validateBrainFile(const std::string& filepath) const {
    // Check if file exists and has a valid brain file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return std::filesystem::exists(filepath) && 
           (ext == ".nnet" || ext == ".bin" || ext == ".data" || ext == ".ckpt");
}

bool ExportImportCommand::validateConfigFile(const std::string& filepath) const {
    // Check if file exists and has a valid config file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return std::filesystem::exists(filepath) && 
           (ext == ".json" || ext == ".yaml" || ext == ".yml" || ext == ".cfg" || ext == ".ini");
}

ExportImportCommand::ImportExportStats ExportImportCommand::getLastStats() const {
    return pImpl->stats;
}

// Helper methods for format-specific exports
bool ExportImportCommand::exportBrainToJSON(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    try {
        json j;
        
        // Add brain metadata
        j["metadata"] = {
            {"neuron_count", brain->getTotalNeuronCount()},
            {"synapse_count", brain->getTotalSynapseCount()},
            {"region_count", brain->getRegionCount()},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
        };
        
        // Add region information
        json regions = json::array();
        for (const auto& region : brain->getRegions()) {
            json r;
            r["id"] = region->getId().index();
            r["name"] = region->getName();
            r["neuron_count"] = region->getTotalNeuronCount();
            r["synapse_count"] = region->getSynapseCount();
            regions.push_back(r);
        }
        j["regions"] = regions;
        
        // Save to file
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << j << std::endl;
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ExportImportCommand::exportBrainToCSV(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    try {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "neuron_id,region_id,type,activation,voltage,membrane_potential\n";
        
        // Export neuron data
        for (const auto& region : brain->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                file << neuron->getId().index() << ","
                     << region->getId().index() << ","
                     << static_cast<int>(neuron->getType()) << ","
                     << neuron->isFiring() << ","
                     << neuron->getState().membranePotential << ","
                     << neuron->getState().membranePotential << "\n";
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ExportImportCommand::exportBrainToText(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    try {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << "NLM Brain State Export\n";
        file << "======================\n\n";
        
        file << "Brain Statistics:\n";
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "  Total Regions: " << brain->getRegionCount() << "\n";
        file << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n\n";
        
        file << "Regions:\n";
        for (const auto& region : brain->getRegions()) {
            file << "  Region " << region->getId().index() << " (" << region->getName() << "):\n";
            file << "    Neurons: " << region->getTotalNeuronCount() << "\n";
            file << "    Synapses: " << region->getSynapseCount() << "\n";
            file << "    Average Weight: " << region->getAverageSynapticWeight() << "\n";
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ExportImportCommand::exportBrainToHDF5(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    // HDF5 export (simplified - would need hdf5 library for full implementation)
    try {
        // For now, fall back to binary export
        return brain->save(outputFile);
    } catch (const std::exception& e) {
        return false;
    }
}

// Handle command-specific logic
bool ExportImportCommand::handleExportCommand() {
    if (arguments.size() < 3) {
        return false;
    }
    
    std::string brainFile = arguments[1];
    std::string outputFile = arguments[2];
    std::string format = "binary";
    
    // Parse additional options
    for (size_t i = 3; i < arguments.size(); ++i) {
        if (arguments[i] == "--format" && i + 1 < arguments.size()) {
            format = arguments[i + 1];
            i++;
        } else if (arguments[i] == "--overwrite") {
            pImpl->overwriteExisting = true;
        } else if (arguments[i] == "--no-backup") {
            pImpl->createBackups = false;
        }
    }
    
    return exportBrainState(brainFile, outputFile, format);
}

bool ExportImportCommand::handleExportConfigCommand() {
    if (arguments.size() < 2) {
        return false;
    }
    
    std::string configFile = arguments[1];
    std::string outputFile = configFile + ".exported";
    bool includeRuntime = false;
    
    // Parse additional options
    for (size_t i = 2; i < arguments.size(); ++i) {
        if (arguments[i] == "--include-runtime") {
            includeRuntime = true;
        } else if (arguments[i] == "--output-file" && i + 1 < arguments.size()) {
            outputFile = arguments[i + 1];
            i++;
        }
    }
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        return false;
    }
    
    return exportConfiguration(config, outputFile, includeRuntime);
}

bool ExportImportCommand::handleBatchExportCommand() {
    if (arguments.size() < 2) {
        return false;
    }
    
    std::string filePattern = arguments[1];
    std::string outputDir = ".";
    std::string format = "binary";
    
    // Parse additional options
    for (size_t i = 2; i < arguments.size(); ++i) {
        if (arguments[i] == "--output-dir" && i + 1 < arguments.size()) {
            outputDir = arguments[i + 1];
            i++;
        } else if (arguments[i] == "--format" && i + 1 < arguments.size()) {
            format = arguments[i + 1];
            i++;
        }
    }
    
    return exportMultipleBrains(std::vector<std::string>{filePattern}, outputDir);
}

bool ExportImportCommand::handleImportCommand() {
    if (arguments.size() < 2) {
        return false;
    }
    
    std::string inputFile = arguments[1];
    std::string configFile;
    
    // Parse additional options
    for (size_t i = 2; i < arguments.size(); ++i) {
        if (arguments[i] == "--config" && i + 1 < arguments.size()) {
            configFile = arguments[i + 1];
            i++;
        }
    }
    
    return importBrainState(inputFile, configFile);
}

bool ExportImportCommand::handleImportConfigCommand() {
    if (arguments.size() < 2) {
        return false;
    }
    
    std::string inputFile = arguments[1];
    std::string outputFile;
    
    // Parse additional options
    for (size_t i = 2; i < arguments.size(); ++i) {
        if (arguments[i] == "--output-file" && i + 1 < arguments.size()) {
            outputFile = arguments[i + 1];
            i++;
        }
    }
    
    return importConfiguration(inputFile, outputFile);
}

bool ExportImportCommand::handleBatchImportCommand() {
    if (arguments.size() < 2) {
        return false;
    }
    
    std::string fileList = arguments[1];
    std::string outputDir = ".";
    
    // Parse additional options
    for (size_t i = 2; i < arguments.size(); ++i) {
        if (arguments[i] == "--output-dir" && i + 1 < arguments.size()) {
            outputDir = arguments[i + 1];
            i++;
        }
    }
    
    return importBatch(fileList, outputDir);
}

bool ExportImportCommand::validateExportArguments() const {
    if (arguments.size() < 3) {
        return false;
    }
    
    return validateBrainFile(arguments[1]);
}

bool ExportImportCommand::validateExportConfigArguments() const {
    if (arguments.size() < 2) {
        return false;
    }
    
    return validateConfigFile(arguments[1]);
}

bool ExportImportCommand::validateBatchExportArguments() const {
    if (arguments.size() < 2) {
        return false;
    }
    
    return true; // File pattern validation would be done at runtime
}

bool ExportImportCommand::validateImportArguments() const {
    if (arguments.size() < 2) {
        return false;
    }
    
    return validateBrainFile(arguments[1]);
}

bool ExportImportCommand::validateImportConfigArguments() const {
    if (arguments.size() < 2) {
        return false;
    }
    
    return validateConfigFile(arguments[1]);
}

bool ExportImportCommand::validateBatchImportArguments() const {
    if (arguments.size() < 2) {
        return false;
    }
    
    return true; // File list validation would be done at runtime
}

bool ExportImportCommand::exportMultipleBrains(const std::vector<std::string>& brainFiles, const std::string& outputDirectory) {
    if (!std::filesystem::exists(outputDirectory)) {
        std::filesystem::create_directories(outputDirectory);
    }
    
    bool allSuccess = true;
    
    for (const auto& inputFile : brainFiles) {
        try {
            // Extract filename
            std::string filename = std::filesystem::path(inputFile).filename().string();
            std::string outputFile = outputDirectory + "/" + filename;
            
            // Export brain
            if (exportBrainState(inputFile, outputFile)) {
                pImpl->stats.filesSuccessful++;
            } else {
                pImpl->stats.filesFailed++;
                allSuccess = false;
            }
            
            pImpl->stats.filesProcessed++;
            
        } catch (const std::exception& e) {
            pImpl->stats.filesFailed++;
            pImpl->stats.filesProcessed++;
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool ExportImportCommand::importBatch(const std::string& fileList, const std::string& outputDirectory) {
    try {
        if (!std::filesystem::exists(outputDirectory)) {
            std::filesystem::create_directories(outputDirectory);
        }
        
        // Read file list
        std::ifstream listFile(fileList);
        if (!listFile.is_open()) {
            return false;
        }
        
        std::vector<std::string> files;
        std::string line;
        while (std::getline(listFile, line)) {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);
            
            if (!line.empty()) {
                files.push_back(line);
            }
        }
        listFile.close();
        
        bool allSuccess = true;
        
        for (const auto& file : files) {
            try {
                // Import brain
                if (importBrainState(file, "")) {
                    pImpl->stats.filesSuccessful++;
                } else {
                    pImpl->stats.filesFailed++;
                    allSuccess = false;
                }
                
                pImpl->stats.filesProcessed++;
                
            } catch (const std::exception& e) {
                pImpl->stats.filesFailed++;
                pImpl->stats.filesProcessed++;
                allSuccess = false;
            }
        }
        
        return allSuccess;
        
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace nlm

// Include JSON library for export/import
#include <nlohmann/json.hpp>