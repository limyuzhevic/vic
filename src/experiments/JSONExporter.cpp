#include "ExperimentExporter.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

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

} // namespace nlm
