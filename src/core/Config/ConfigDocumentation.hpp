#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "Config.hpp"

namespace nlm {

class ConfigDocumentation {
public:
    ConfigDocumentation(const std::string& appName, const std::string& version);
    
    std::string generateMarkdown(const std::vector<ConfigEntry>& entries) const;
    std::string generateHelpText(const std::vector<ConfigEntry>& entries) const;
    std::string generateExamples() const;
    bool saveToFile(const std::string& filepath) const;
    
private:
    std::string appName_;
    std::string version_;
    
    std::string formatValue(const ConfigValue& value) const;
    std::string getValueType(const ConfigValue& value) const;
};

} // namespace nlm