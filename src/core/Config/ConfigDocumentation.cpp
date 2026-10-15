#include "ConfigDocumentation.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

ConfigDocumentation::ConfigDocumentation(const std::string& appName, const std::string& version)
    : appName_(appName), version_(version) {}

std::string ConfigDocumentation::generateMarkdown(const std::vector<ConfigEntry>& entries) const {
    std::ostringstream oss;
    oss << "# " << appName_ << " Configuration\n\n";
    oss << "Version: " << version_ << "\n\n";
    
    oss << "## Configuration Options\n\n";
    oss << "| Key | Type | Default | Description |\n";
    oss << "|-----|------|---------|-------------|\n";
    
    for (const auto& entry : entries) {
        std::string type = getValueType(entry.value);
        oss << "| `" << entry.key << "` | `" << type << "` | `" << formatValue(entry.value) << "` | ";
        if (!entry.description.empty()) {
            oss << entry.description;
        }
        oss << " |\n";
    }
    
    oss << "\n## Examples\n\n";
    oss << generateExamples();
    
    return oss.str();
}

std::string ConfigDocumentation::generateHelpText(const std::vector<ConfigEntry>& entries) const {
    std::ostringstream oss;
    oss << appName_ << " " << version_ << "\n\n";
    oss << "Available configuration options:\n\n";
    
    for (const auto& entry : entries) {
        oss << "  --" << entry.key << "=" << formatValue(entry.value) << "  ";
        if (!entry.description.empty()) {
            oss << "  " << entry.description << "\n";
        }
    }
    
    return oss.str();
}

std::string ConfigDocumentation::generateExamples() const {
    std::ostringstream oss;
    oss << "Here are some example configurations:\n\n";
    
    oss << "**Basic configuration:**\n\n";
    oss << "```\n";
    oss << "key1 = value1\n";
    oss << "key2 = 123\n";
    oss << "key3 = true\n";
    oss << "```\n\n";
    
    oss << "**JSON configuration:**\n\n";
    oss << "```json\n";
    oss << "{\n";
    oss << "  \"key1\": \"value1\",\n";
    oss << "  \"key2\": 123,\n";
    oss << "  \"key3\": true\n";
    oss << "}\n";
    oss << "```\n\n";
    
    return oss.str();
}

bool ConfigDocumentation::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# " << appName_ << " Configuration Documentation\n\n";
    
    return true;
}

std::string ConfigDocumentation::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string ConfigDocumentation::getValueType(const ConfigValue& value) const {
    std::string type;
    std::visit([&type](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) type = "int";
        else if constexpr (std::is_same_v<T, int64_t>) type = "int64_t";
        else if constexpr (std::is_same_v<T, double>) type = "double";
        else if constexpr (std::is_same_v<T, bool>) type = "bool";
        else if constexpr (std::is_same_v<T, std::string>) type = "string";
        else type = "unknown";
    }, value);
    return type;
}

} // namespace nlm