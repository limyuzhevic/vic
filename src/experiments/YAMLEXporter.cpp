#include "ExperimentExporter.hpp"
#include <yaml-cpp/yaml.h>

namespace nlm {

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

} // namespace nlm
