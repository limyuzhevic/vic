#include <filesystem>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with better error handling and multiple format support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Failed to open configuration file: " + filepath);
        return false;
    }
    
    // Try to detect file format
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Reset file pointer
        file.clear();
        file.seekg(0);
        
        // Check if it's a JSON file (starts with { or [
        if (firstLine.empty() || firstLine.find('{') == 0 || firstLine.find('[') == 0) {
            return loadFromJSONFile(filepath);
        }
        // Check if it's a YAML file (starts with key: value format)
        else if (firstLine.find(':') != std::string::npos) {
            return loadFromSimpleFile(filepath);
        }
        // Try simple format as fallback
        else {
            return loadFromSimpleFile(filepath);
        }
    }
    
    return false;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Enhanced JSON file loading with better error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    if (content.empty()) {
        NLM_LOG_ERROR("Empty JSON configuration file: " + filepath);
        return false;
    }
    
    try {
        // TODO: Implement proper JSON parsing using a JSON library
        // For now, fall back to simple parser but with JSON format detection
        if (content.find('{') != std::string::npos) {
            NLM_LOG_INFO("JSON object detected - implementing simple JSON parser");
            return parseSimpleJSON(content);
        } else {
            NLM_LOG_ERROR("Invalid JSON format in: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::parseSimpleJSON(const std::string& jsonContent) {
    // Simple JSON parser implementation (TODO: replace with proper library)
    size_t pos = 0;
    bool success = true;
    
    try {
        // Find opening brace
        if (jsonContent.find('{') == std::string::npos) {
            return false;
        }
        
        // Simple parsing for key=value pairs in JSON-like format
        // This is a minimal implementation - TODO: replace with proper JSON library
        size_t start = jsonContent.find('{') + 1;
        size_t end = jsonContent.rfind('}');
        
        if (start == std::string::npos || end == std::string::npos) {
            return false;
        }
        
        std::string inner = jsonContent.substr(start, end - start);
        
        // Parse comma-separated key:value pairs
        size_t keyPos = 0;
        while (keyPos < inner.length()) {
            // Find next key
            size_t keyEnd = inner.find(':', keyPos);
            if (keyEnd == std::string::npos) break;
            
            std::string key = trim(inner.substr(keyPos, keyEnd - keyPos));
            key = trim(key.erase(0, key.find_first_not_of('\"')));
            key = trim(key.erase(key.find_last_not_of('\"') + 1));
            
            // Find next comma or end
            size_t valueEnd = inner.find(',', keyEnd);
            if (valueEnd == std::string::npos) {
                valueEnd = inner.length();
            }
            
            std::string value = trim(inner.substr(keyEnd + 1, valueEnd - keyEnd - 1));
            value = trim(value.erase(0, value.find_first_not_of('\"')));
            value = trim(value.erase(value.find_last_not_of('\"') + 1));
            
            // Parse value based on content
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
            } else {
                try {
                    // Try to parse as number
                    if (value.find('.') != std::string::npos) {
                        double doubleValue = std::stod(value);
                        set(key, doubleValue, ConfigSource::File);
                    } else {
                        int intValue = std::stoi(value);
                        set(key, intValue, ConfigSource::File);
                    }
                } catch (...) {
                    // Fall back to string
                    set(key, value, ConfigSource::File);
                }
            }
            
            keyPos = valueEnd + 1;
            if (inner[keyPos] != ',') break;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON content: " + std::string(e.what()));
        success = false;
    }
    
    return success;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
