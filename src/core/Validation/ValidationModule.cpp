#include "ValidationModule.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"

namespace nlm {
    // Global validation framework instance
    ValidationFramework gValidationFramework;
    
    // Global validation results
    std::vector<ValidationResultDetail> gValidationResults;
    
    bool initializeValidationFramework(const std::string& configFile) {
        return gValidationFramework.initialize();
    }
    
    bool validateConfiguration(const Config& config) {
        ConfigSchema schema;  // In practice, you would have access to the actual schema
        gValidationResults = gValidationFramework.validate(config, schema);
        return isValid(gValidationResults);
    }
    
    const std::vector<ValidationResultDetail>& getValidationResults() {
        return gValidationResults;
    }
    
    bool saveValidationResults(const std::string& filepath, const std::string& format) {
        return ValidationUtilities::saveValidationResults(filepath, gValidationResults, format);
    }
    
    bool loadValidationConfiguration(const std::string& filepath) {
        return gValidationFramework.getConfig().validationFile == filepath; // Simplified
    }
    
    std::string getValidationStatistics() {
        return gValidationFramework.getStatistics();
    }
    
} // namespace nlm
