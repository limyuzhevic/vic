#pragma once

#include "Config.hpp"
#include <string>

namespace nlm {

class AdvancedConfig : public Config {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Advanced validation with detailed error reporting
    std::string detailedValidationReport() const;
    
    // Performance optimization
    double benchmark() const;
    
    // Configuration search and filtering
    std::vector<std::string> findByCategory(const std::string& category) const;
    std::vector<std::string> findByType(const std::string& type) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::string getSourceDistribution() const;
    
    // Version control
    std::string getVersion() const;
    void setVersion(const std::string& version);
    
    // Metadata
    std::string getAuthor() const;
    void setAuthor(const std::string& author);
    
    std::string getDescription() const;
    void setDescription(const std::string& description);
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> pImplAdvanced;
};

} // namespace nlm
