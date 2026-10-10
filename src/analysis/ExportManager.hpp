// Export Manager Header
#pragma once

#include <string>
#include "NLMAnalysis.hpp"

namespace nlm {

/**
 * Export manager
 */
class ExportManager {
public:
    ExportManager();
    ~ExportManager();
    
    /**
     * Export data
     */
    bool exportData(const std::string& dataType, const std::string& filePath,
                   const std::vector<ExperimentResult>& results);
    
    /**
     * Set export options
     */
    void setExportOptions(const ExportOptions& options);
    
    /**
     * Get export options
     */
    ExportOptions getExportOptions() const;
    
private:
    NLMDataExporter _exporter;
};

} // namespace nlm