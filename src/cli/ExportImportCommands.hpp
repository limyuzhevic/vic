#pragma once

#include <string>
#include <vector>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"

namespace nlm {

class ExportImportCommands {
public:
    ExportImportCommands(std::shared_ptr<CommandOutputFormatter> formatter,
                         std::shared_ptr<Logger> logger);
    
    // Export commands
    bool exportNeuralGraph(const std::string& file);
    bool exportSynapticMatrix(const std::string& file);
    bool exportMemoryTraces(const std::string& file);
    bool exportPlasticityData(const std::string& file);
    bool exportBehaviorLog(const std::string& file);
    
    // Import commands
    bool importNeuralGraph(const std::string& file);
    bool importSynapticMatrix(const std::string& file);
    bool importBehaviorLog(const std::string& file);
    
    // File format helpers
    static bool exportToJSON(const std::string& filename, const std::string& data);
    static bool exportToBinary(const std::string& filename, const std::string& data);
    static bool importFromFile(const std::string& filename, std::string& data);
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    
    bool validateExportFile(const std::string& file) const;
    bool validateImportFile(const std::string& file) const;
    std::string formatExportData(const std::string& data, const std::string& format) const;
};
