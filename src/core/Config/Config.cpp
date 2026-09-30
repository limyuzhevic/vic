#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Simple JSON parser (supports basic objects and values)
bool parseJSON(const std::string& content, std::unordered_map<std::string, ConfigEntry>& entries) {
    size_t pos = 0;
    size_t len = content.size();
    
    // Skip whitespace and BOM
    while (pos < len && std::isspace(content[pos])) {
        pos++;
    }
    
    if (pos >= len) return false;
    
    // Must start with {
    if (content[pos] != '{') return false;
    
    pos++; // Skip {
    
    while (pos < len) {
        // Skip whitespace
        while (pos < len && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= len) break;
        
        // Check for end of object
        if (content[pos] == '}') {
            break;
        }
        
        // Parse key (string)
        std::string key;
        if (content[pos] == '"') {
            pos++; // Skip quote
            while (pos < len && content[pos] != '"') {
                if (content[pos] == '\\' && pos + 1 < len) {
                    pos++; // Skip escaped character
                }
                key += content[pos];
                pos++;
            }
            if (pos < len && content[pos] == '"') {
                pos++; // Skip closing quote
            }
        }
        
        if (key.empty()) {
            break; // Invalid JSON
        }
        
        // Skip whitespace
        while (pos < len && std::isspace(content[pos])) {
            pos++;
        }
        
        // Must have :
        if (pos >= len || content[pos] != ':') {
            break;
        }
        
        pos++; // Skip :
        
        // Skip whitespace
        while (pos < len && std::isspace(content[pos])) {
            pos++;
        }
        
        // Parse value
        ConfigValue value;
        if (!parseJSONValue(content, pos, value)) {
            break;
        }
        
        // Add to entries
        entries[key] = ConfigEntry(key, value, ConfigSource::File);
        
        // Skip whitespace
        while (pos < len && std::isspace(content[pos])) {
            pos++;
        }
        
        // Check for comma or end
        if (pos < len && content[pos] == ',') {
            pos++; // Skip comma
            continue;
        } else if (pos < len && content[pos] == '}') {
            break;
        }
    }
    
    return pos < len && content[pos] == '}';
}

// Parse JSON value (string, number, boolean, null)
bool parseJSONValue(const std::string& content, size_t& pos, ConfigValue& value) {
    size_t len = content.size();
    
    // Skip whitespace
    while (pos < len && std::isspace(content[pos])) {
        pos++;
    }
    
    if (pos >= len) {
        return false;
    }
    
    char ch = content[pos];
    
    if (ch == '"') {
        // String
        std::string str;
        pos++; // Skip quote
        while (pos < len && content[pos] != '"') {
            if (content[pos] == '\\' && pos + 1 < len) {
                pos++; // Skip escaped character
                if (pos < len) {
                    str += content[pos];
                }
            } else {
                str += content[pos];
            }
            pos++;
        }
        if (pos < len && content[pos] == '"') {
            pos++; // Skip closing quote
            value = str;
            return true;
        }
        return false;
    } else if (ch == 't' || ch == 'f') {
        // Boolean
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            value = true;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            pos += 5;
            value = false;
            return true;
        }
        return false;
    } else if (ch == 'n') {
        // null
        if (content.substr(pos, 4) == "null") {
            pos += 4;
            value = std::string("");
            return true;
        }
        return false;
    } else if (ch == '-' || std::isdigit(ch)) {
        // Number
        size_t numPos = pos;
        bool isFloat = false;
        while (numPos < len && (std::isdigit(content[numPos]) || 
                               content[numPos] == '-' || 
                               content[numPos] == '.' || 
                               content[numPos] == 'e' || 
                               content[numPos] == 'E')) {
            if (content[numPos] == '.' || 
                (content[numPos] == 'e' || content[numPos] == 'E')) {
                isFloat = true;
            }
            numPos++;
        }
        
        if (numPos == pos) {
            return false;
        }
        
        std::string numStr = content.substr(pos, numPos - pos);
        pos = numPos;
        
        if (isFloat) {
            try {
                double val = std::stod(numStr);
                value = val;
                return true;
            } catch (...) {
                return false;
            }
        } else {
            try {
                int64_t val = std::stoll(numStr);
                value = val;
                return true;
            } catch (...) {
                return false;
            }
        }
    }
    
    return false;
}

// Trim string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Updated loadFromFile with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format by extension
    bool isJSON = false;
    bool isYAML = false;
    
    if (filepath.size() >= 5) {
        std::string ext = filepath.substr(filepath.size() - 5);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        isJSON = (ext == ".json");
        isYAML = (ext == ".yaml") || (ext == ".yml");
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Parse based on file format
    if (isJSON || isYAML) {
        // Try JSON/YAML parser
        if (parseJSON(content, pImpl->entries)) {
            return true;
        }
        // Fall back to simple format if parsing fails
        file.clear();
        file.seekg(0);
    }
    
    // Simple key=value format (original implementation)
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : pImpl->entries) {
        const auto& entry = pair.second;
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second.value);
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
    // Use exception safety with find-or-create
    auto& entry = pImpl->entries[key];
    entry.key = key;
    entry.value = value;
    entry.source = source;
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
    return pImpl->entries.find(key) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& pair : pImpl->entries) {
        const auto& entry = pair.second;
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
