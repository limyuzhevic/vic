#include "ConfigTemplate.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace nlm {

// Template specializations for addField()

template<typename T>
void ConfigTemplate::addField(const std::string& name, const T& defaultValue,
                              const std::string& description) {
    fields[name] = defaultValue;
    info.defaultValues[name] = defaultValue;
}

// Template specializations for getFieldValue()

template<typename T>
std::optional<T> ConfigTemplate::getFieldValue(const std::string& name) const {
    auto it = fields.find(name);
    if (it != fields.end()) {
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

// Helper function for JSON serialization
static std::string toJson(const std::unordered_map<std::string, std::any>& data) {
    nlohmann::json json;
    
    for (const auto& pair : data) {
        // Convert any to JSON based on type
        if (pair.second.type() == typeid(int)) {
            json[pair.first] = std::any_cast<int>(pair.second);
        } else if (pair.second.type() == typeid(int64_t)) {
            json[pair.first] = std::any_cast<int64_t>(pair.second);
        } else if (pair.second.type() == typeid(double)) {
            json[pair.first] = std::any_cast<double>(pair.second);
        } else if (pair.second.type() == typeid(bool)) {
            json[pair.first] = std::any_cast<bool>(pair.second);
        } else if (pair.second.type() == typeid(std::string)) {
            json[pair.first] = std::any_cast<std::string>(pair.second);
        } else if (pair.second.type() == typeid(std::vector<int>)) {
            json[pair.first] = std::any_cast<std::vector<int>>(pair.second);
        } else if (pair.second.type() == typeid(std::vector<double>)) {
            json[pair.first] = std::any_cast<std::vector<double>>(pair.second);
        } else if (pair.second.type() == typeid(std::vector<std::string>)) {
            json[pair.first] = std::any_cast<std::vector<std::string>>(pair.second);
        } else {
            json[pair.first] = pair.second.type().name(); // Fallback
        }
    }
    
    return json.dump();
}

void ConfigTemplate::applyTo(Config& config) const {
    for (const auto& pair : fields) {
        const std::string& name = pair.first;
        const std::any& value = pair.second;
        
        // Apply value based on type
        if (value.type() == typeid(int)) {
            config.set(name, std::any_cast<int>(value), ConfigSource::Default);
        } else if (value.type() == typeid(int64_t)) {
            config.set(name, std::any_cast<int64_t>(value), ConfigSource::Default);
        } else if (value.type() == typeid(double)) {
            config.set(name, std::any_cast<double>(value), ConfigSource::Default);
        } else if (value.type() == typeid(bool)) {
            config.set(name, std::any_cast<bool>(value), ConfigSource::Default);
        } else if (value.type() == typeid(std::string)) {
            config.set(name, std::any_cast<std::string>(value), ConfigSource::Default);
        }
    }
}

std::vector<std::string> ConfigTemplate::getFieldNames() const {
    std::vector<std::string> names;
    names.reserve(fields.size());
    for (const auto& pair : fields) {
        names.push_back(pair.first);
    }
    return names;
}

bool ConfigTemplate::hasField(const std::string& name) const {
    return fields.find(name) != fields.end();
}

const std::any* ConfigTemplate::getDefaultValue(const std::string& name) const {
    auto it = info.defaultValues.find(name);
    if (it != info.defaultValues.end()) {
        return &it->second;
    }
    return nullptr;
}

bool ConfigTemplate::exportToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON representation
    nlohmann::json json;
    json["name"] = info.name;
    json["description"] = info.description;
    json["author"] = info.author;
    json["version"] = info.version;
    json["tags"] = info.tags;
    json["isPublic"] = info.isPublic;
    
    // Add fields
    nlohmann::json fieldsJson;
    for (const auto& pair : fields) {
        fieldsJson[pair.first] = pair.second; // nlohmann::json can serialize std::any
    }
    json["fields"] = fieldsJson;
    
    file << json.dump(2);
    return true;
}

std::unique_ptr<ConfigTemplate> ConfigTemplate::importFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    auto templatePtr = std::make_unique<ConfigTemplate>();
    try {
        nlohmann::json json = nlohmann::json::parse(file);
        
        templatePtr->info.name = json.value("name", "");
        templatePtr->info.description = json.value("description", "");
        templatePtr->info.author = json.value("author", "");
        templatePtr->info.version = json.value("version", "1.0");
        templatePtr->info.isPublic = json.value("isPublic", true);
        
        if (json.contains("tags")) {
            templatePtr->info.tags = json["tags"].get<std::vector<std::string>>();
        }
        
        if (json.contains("fields")) {
            for (const auto& pair : json["fields"].items()) {
                // Convert JSON value to std::any
                if (pair.value().is_number_integer()) {
                    templatePtr->fields[pair.key()] = pair.value().get<int64_t>();
                } else if (pair.value().is_number_float()) {
                    templatePtr->fields[pair.key()] = pair.value().get<double>();
                } else if (pair.value().is_boolean()) {
                    templatePtr->fields[pair.key()] = pair.value().get<bool>();
                } else if (pair.value().is_string()) {
                    templatePtr->fields[pair.key()] = pair.value().get<std::string>();
                }
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        return nullptr;
    }
    
    return templatePtr;
}

std::unique_ptr<ConfigTemplate> ConfigTemplate::clone() const {
    auto clone = std::make_unique<ConfigTemplate>();
    clone->info = info;
    clone->fields = fields;
    return clone;
}

} // namespace nlm