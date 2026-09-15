// Forward declarations for JSON parsing
bool json::parseNumber(const std::string& str, ConfigValue& out);
bool json::parseArray(const std::string& str, size_t& pos, std::vector<std::string>& out);
bool json::parseObject(const std::string& str, size_t& pos, std::unordered_map<std::string, std::string>& out);
bool json::parseJsonObject(const std::string& json, Config& config, size_t& pos);
std::string json::serializeToJson(const ConfigValue& value);
std::string json::escapeString(const std::string& str);

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Parse a JSON object into Config values
bool Config::parseJsonObject(const std::string& json, Config& config, size_t& pos) {
    config.clear();
    
    if (json.empty() || json[pos] != '{') {
        return false;
    }
    pos++;
    
    pos = json::skipWhitespace(json, pos);
    
    bool first = true;
    
    while (pos < json.size() && json[pos] != '}') {
        if (!first && json[pos] == ',') {
            pos++;
            pos = json::skipWhitespace(json, pos);
        }
        first = false;
        
        if (pos >= json.size() || json[pos] == '}') break;
        
        // Parse key
        size_t keyStart = pos;
        if (json[pos] != '\"') return false;
        
        size_t keyEnd = json.find('\"', pos + 1);
        if (keyEnd == std::string::npos) return false;
        
        std::string key = json.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        pos = json::skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != ':') return false;
        pos++;
        pos = json::skipWhitespace(json, pos);
        
        if (pos >= json.size()) break;
        
        // Parse value
        char c = json[pos];
        
        if (c == '\"') {
            // String value
            size_t valueEnd = json.find('\"', pos + 1);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = json.substr(pos + 1, valueEnd - pos - 1);
            config.set(key, valueStr, ConfigSource::File);
            pos = valueEnd + 1;
        }
        else if (c == 't' && json.substr(pos, 4) == "true") {
            // Boolean true
            config.set(key, true, ConfigSource::File);
            pos += 4;
        }
        else if (c == 'f' && json.substr(pos, 5) == "false") {
            // Boolean false
            config.set(key, false, ConfigSource::File);
            pos += 5;
        }
        else if (c == 'n' && json.substr(pos, 4) == "null") {
            // Skip null values
            pos += 4;
        }
        else if (c == '[') {
            // Array value
            std::vector<std::string> arr;
            if (json::parseArray(json, pos, arr)) {
                // Try to infer array type
                bool allInts = true;
                bool allDoubles = true;
                bool allStrings = true;
                
                for (const auto& str : arr) {
                    if (str == "null") {
                        allInts = allDoubles = false;
                        continue;
                    }
                    try {
                        size_t pos2 = 0;
                        std::stoll(str, &pos2);
                        if (pos2 != str.size()) allInts = false;
                    } catch (...) { allInts = false; }
                    
                    try {
                        size_t pos2 = 0;
                        std::stod(str, &pos2);
                        if (pos2 != str.size()) allDoubles = false;
                    } catch (...) { allDoubles = false; }
                    
                    allStrings = false; // If we have a string representation, it's a string array
                }
                
                if (allInts) {
                    std::vector<int> intArr;
                    for (const auto& str : arr) {
                        if (str != "null") {
                            intArr.push_back(std::stoi(str));
                        }
                    }
                    config.set(key, intArr, ConfigSource::File);
                }
                else if (allDoubles) {
                    std::vector<double> doubleArr;
                    for (const auto& str : arr) {
                        if (str != "null") {
                            doubleArr.push_back(std::stod(str));
                        }
                    }
                    config.set(key, doubleArr, ConfigSource::File);
                }
                else {
                    // Remove null strings for string array
                    std::vector<std::string> stringArr;
                    for (const auto& str : arr) {
                        if (str != "null") {
                            stringArr.push_back(str);
                        }
                    }
                    config.set(key, stringArr, ConfigSource::File);
                }
            }
        }
        else {
            // Try to parse as number
            std::string valueStr;
            size_t valueEnd = pos;
            while (valueEnd < json.size() && json[valueEnd] != ',' && json[valueEnd] != '}' && !json::isWhitespace(json[valueEnd])) {
                valueEnd++;
            }
            valueStr = json.substr(pos, valueEnd - pos);
            
            // Try to convert to int first, then double
            ConfigValue parsedValue;
            if (json::parseNumber(valueStr, valueEnd, parsedValue)) {
                config.set(key, parsedValue, ConfigSource::File);
                pos = valueEnd;
            }
            else {
                // Try as string
                config.set(key, valueStr, ConfigSource::File);
                pos = valueEnd;
            }
        }
        
        pos = json::skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            pos++;
        }
    }
    
    if (pos < json.size() && json[pos] == '}') {
        pos++;
        return true;
    }
    
    return false;
}

// Implementation of Config::loadFromFile (JSON format)
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Read entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    // Skip whitespace at the beginning
    size_t pos = json::skipWhitespace(jsonContent, 0);
    
    // Check if it's a JSON object
    if (jsonContent.empty() || jsonContent[pos] != '{') {
        return loadFromSimple(filepath); // Fall back to simple format
    }
    
    // Parse JSON
    if (parseJsonObject(jsonContent, *this, pos)) {
        return true;
    }
    
    // If JSON parsing failed, try simple format
    return loadFromSimple(filepath);
}

// Load from a simple key=value format (backward compatibility)
bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }

        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            set(key, value, ConfigSource::File);
        }
    }

    return true;
}

// Implementation of Config::loadFromArgs
bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

// Implementation of Config::saveToFile (JSON format)
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Write JSON header and opening brace
    file << "{\n";

    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        // Write key
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            }
            else if constexpr (std::is_same_v<T, double>) {
                // Format with fixed precision to avoid scientific notation
                file << std::fixed << std::setprecision(6) << arg;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes and backslashes in strings
                for (char c : arg) {
                    if (c == '\\' || c == '\"') file << '\\';
                    file << c;
                }
            }
            else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            }
            else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[j];
                }
                file << "]";
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    // Escape quotes and backslashes in strings
                    for (char c : arg[j]) {
                        if (c == '\\' || c == '\"') file << '\\';
                        file << c;
                    }
                }
                file << "]";
            }
        }, entry.value);
        
        // Add comma except for the last entry
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }

    // Write closing brace
    file << "}\n";

    return true;
}

// Template function definitions
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
    return json::trim(str);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
namespace {
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
}

} // namespace nlm