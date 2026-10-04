#include "experiments/ExperimentManager.hpp"
#include <iomanip>
#include <limits>
#include <sstream>

namespace nlm {

// DefaultExperimentExecutor implementation
void DefaultExperimentExecutor::saveParametersToJson(const ExperimentConfig& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"name\": \"" << config.name << "\",\n";
        file << "  \"description\": \"" << config.description << "\",\n";
        file << "  \"version\": \"" << config.version << "\",\n";
        file << "  \"parameters\": [\n";
        
        for (size_t i = 0; i < config.parameters.size(); ++i) {
            const auto& param = config.parameters[i];
            file << "    {\n";
            file << "      \"name\": \"" << param.name << "\",\n";
            file << "      \"type\": \"" << param.type << "\",\n";
            file << "      \"default\": \"" << param.getDefaultAsString() << "\",\n";
            file << "      \"description\": \"" << param.description << "\"\n";
            if (i < config.parameters.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
    }
}

} // namespace nlm