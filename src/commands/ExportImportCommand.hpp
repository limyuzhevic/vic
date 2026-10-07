#pragma once

#include "CommandBase.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

class ExportImportCommand : public CommandBase {
public:
    ExportImportCommand();
    ~ExportImportCommand();
    
    std::string getName() const override;
    std::string getDescription() const override;
    std::string getUsage() const override;
    std::string getHelp() const override;
    
    CommandStatus getStatus() const override;
    double getProgress() const override;
    std::string getProgressText() const override;
    
    bool execute() override;
    bool cancel() override;
    bool isCancellable() const override;
    
    CommandResult getResult() const override;
    std::vector<std::string> getArguments() const override;
    void setArguments(const std::vector<std::string>& args) override;
    
    bool validateArguments() const override;
    std::string getLastError() const override;
    
    std::chrono::milliseconds getEstimatedExecutionTime() const override;
    
    Category getCategory() const override;
    
    // Export-specific functionality
    bool exportBrainState(const std::string& brainFile, const std::string& outputFile, 
                         const std::string& format = "binary");
    bool exportConfiguration(const std::shared_ptr<Config>& config, const std::string& outputFile,
                             bool includeRuntime = false);
    bool exportMultipleBrains(const std::vector<std::string>& brainFiles, const std::string& outputDirectory);
    
    // Import-specific functionality
    bool importBrainState(const std::string& inputFile, const std::string& configFile = "");
    bool importConfiguration(const std::string& configFile, const std::string& outputConfigFile = "");
    bool importBatch(const std::string& fileList, const std::string& outputDirectory);
    
    // Format support
    enum class ExportFormat {
        Binary,
        JSON,
        CSV,
        Text,
        HDF5
    };
    
    // Get supported formats
    std::vector<std::string> getSupportedExportFormats() const;
    std::vector<std::string> getSupportedImportFormats() const;
    
    // Validation
    bool validateFilePath(const std::string& filepath) const;
    bool validateBrainFile(const std::string& filepath) const;
    bool validateConfigFile(const std::string& filepath) const;
    
    // Statistics
    struct ImportExportStats {
        size_t filesProcessed;
        size_t filesSuccessful;
        size_t filesFailed;
        std::chrono::milliseconds totalTime;
        std::vector<std::string> successfulFiles;
        std::vector<std::string> failedFiles;
    };
    
    ImportExportStats getLastStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm