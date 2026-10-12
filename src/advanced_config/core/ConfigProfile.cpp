#include "ConfigProfile.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace nlm {

// Template specializations for addValue()

template<typename T>
void ConfigProfile::addValue(const std::string& key, const T& value,
                             const std::string& description) {
    values[key] = value;
    valueDescriptions[key] = description;
}

// Template specializations for getValue()

template<typename T>
std::optional<T> ConfigProfile::getValue(const std::string& key) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

void ConfigProfile::applyTo(Config& config) const {
    for (const auto& pair : values) {
        const std::string& key = pair.first;
        const std::any& value = pair.second;
        
        // Apply value based on type
        if (value.type() == typeid(int)) {
            config.set(key, std::any_cast<int>(value), ConfigSource::File);
        } else if (value.type() == typeid(int64_t)) {
            config.set(key, std::any_cast<int64_t>(value), ConfigSource::File);
        } else if (value.type() == typeid(double)) {
            config.set(key, std::any_cast<double>(value), ConfigSource::File);
        } else if (value.type() == typeid(bool)) {
            config.set(key, std::any_cast<bool>(value), ConfigSource::File);
        } else if (value.type() == typeid(std::string)) {
            config.set(key, std::any_cast<std::string>(value), ConfigSource::File);
        }
    }
}

std::vector<std::string> ConfigProfile::getValueNames() const {
    std::vector<std::string> names;
    names.reserve(values.size());
    for (const auto& pair : values) {
        names.push_back(pair.first);
    }
    return names;
}

bool ConfigProfile::hasValue(const std::string& key) const {
    return values.find(key) != values.end();
}

const std::any* ConfigProfile::getValueAny(const std::string& key) const {
    auto it = values.find(key);
    if (it != values.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigProfile::merge(const ConfigProfile& other, bool overwrite) {
    for (const auto& pair : other.values) {
        const std::string& key = pair.first;
        const std::any& value = pair.second;
        
        if (overwrite || values.find(key) == values.end()) {
            values[key] = value;
        }
        
        // Merge descriptions
        if (other.valueDescriptions.find(key) != other.valueDescriptions.end()) {
            valueDescriptions[key] = other.valueDescriptions.at(key);
        }
    }
}

bool ConfigProfile::exportToFile(const std::string& filepath) const {
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
    json["isBuiltIn"] = info.isBuiltIn;
    json["isSystemProfile"] = info.isSystemProfile;
    
    // Add values
    nlohmann::json valuesJson;
    for (const auto& pair : values) {
        valuesJson[pair.first] = pair.second;
    }
    json["values"] = valuesJson;
    
    // Add descriptions
    nlohmann::json descriptionsJson;
    for (const auto& pair : valueDescriptions) {
        descriptionsJson[pair.first] = pair.second;
    }
    json["descriptions"] = descriptionsJson;
    
    file << json.dump(2);
    return true;
}

std::unique_ptr<ConfigProfile> ConfigProfile::importFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    auto profilePtr = std::make_unique<ConfigProfile>();
    try {
        nlohmann::json json = nlohmann::json::parse(file);
        
        profilePtr->info.name = json.value("name", "");
        profilePtr->info.description = json.value("description", "");
        profilePtr->info.author = json.value("author", "");
        profilePtr->info.version = json.value("version", "1.0");
        profilePtr->info.isBuiltIn = json.value("isBuiltIn", false);
        profilePtr->info.isSystemProfile = json.value("isSystemProfile", false);
        
        if (json.contains("tags")) {
            profilePtr->info.tags = json["tags"].get<std::vector<std::string>>();
        }
        
        if (json.contains("values")) {
            for (const auto& pair : json["values"].items()) {
                // Convert JSON value to std::any
                if (pair.value().is_number_integer()) {
                    profilePtr->values[pair.key()] = pair.value().get<int64_t>();
                } else if (pair.value().is_number_float()) {
                    profilePtr->values[pair.key()] = pair.value().get<double>();
                } else if (pair.value().is_boolean()) {
                    profilePtr->values[pair.key()] = pair.value().get<bool>();
                } else if (pair.value().is_string()) {
                    profilePtr->values[pair.key()] = pair.value().get<std::string>();
                }
            }
        }
        
        if (json.contains("descriptions")) {
            for (const auto& pair : json["descriptions"].items()) {
                profilePtr->valueDescriptions[pair.key()] = pair.value().get<std::string>();
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        return nullptr;
    }
    
    return profilePtr;
}

std::unique_ptr<ConfigProfile> ConfigProfile::clone() const {
    auto clone = std::make_unique<ConfigProfile>();
    clone->info = info;
    clone->values = values;
    clone->valueDescriptions = valueDescriptions;
    return clone;
}

// Static method to get system profiles
std::vector<std::unique_ptr<ConfigProfile>> ConfigProfile::getSystemProfiles() {
    std::vector<std::unique_ptr<ConfigProfile>> profiles;
    
    // Create basic learning profile
    auto learningProfile = std::make_unique<ConfigProfile>();
    learningProfile->info.name = "Basic Learning";
    learningProfile->info.description = "Basic learning profile for neural networks";
    learningProfile->info.author = "NLM Team";
    learningProfile->info.version = "1.0";
    learningProfile->info.isBuiltIn = true;
    learningProfile->info.isSystemProfile = true;
    learningProfile->addValue<int64_t>("neuron_count", 1000, "Number of neurons in the network");
    learningProfile->addValue<double>("learning_rate", 0.01, "Learning rate for synaptic plasticity");
    learningProfile->addValue<double>("plasticity_strength", 0.5, "Strength of plasticity rules");
    
    // Create reactive behavior profile
    auto reactiveProfile = std::make_unique<ConfigProfile>();
    reactiveProfile->info.name = "Reactive Behavior";
    reactiveProfile->info.description = "Profile for reactive behavior";
    reactiveProfile->info.author = "NLM Team";
    reactiveProfile->info.version = "1.0";
    reactiveProfile->info.isBuiltIn = true;
    reactiveProfile->info.isSystemProfile = true;
    reactiveProfile->addValue<std::string>("behavior_mode", "reactive", "Behavioral mode (reactive/proactive)");
    reactiveProfile->addValue<int>("attention_strength", 5, "Attention strength (1-10)");
    reactiveProfile->addValue<int>("planning_depth", 1, "Planning depth (1-5)");
    
    // Create stable environment profile
    auto stableEnvProfile = std::make_unique<ConfigProfile>();
    stableEnvProfile->info.name = "Stable Environment";
    stableProfile->info.description = "Profile for stable environment simulations";
    stableProfile->info.author = "NLM Team";
    stableProfile->info.version = "1.0";
    stableProfile->info.isBuiltIn = true;
    stableProfile->info.isSystemProfile = true;
    stableProfile->addValue<double>("environment_stability", 1.0, "Environment stability factor (0-1)");
    stableProfile->addValue<int64_t>("memory_capacity", 1000000, "Memory capacity in bytes");
    stableProfile->addValue<double>("novelty_threshold", 0.5, "Novelty threshold for curiosity");
    
    profiles.push_back(std::move(learningProfile));
    profiles.push_back(std::move(reactiveProfile));
    profiles.push_back(std::move(stableEnvProfile));
    
    return profiles;
}

// Static method to get system profile by name
std::unique_ptr<ConfigProfile> ConfigProfile::getSystemProfile(const std::string& name) {
    auto profiles = getSystemProfiles();
    for (auto& profile : profiles) {
        if (profile->info.name == name) {
            return profile;
        }
    }
    return nullptr;
}

} // namespace nlm