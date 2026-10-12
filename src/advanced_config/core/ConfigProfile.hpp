#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <vector>
#include <memory>
#include <set>
#include "Config.hpp"

namespace nlm {

// Configuration profile for quick access to common configuration sets
class ConfigProfile {
public:
    struct ProfileInfo {
        std::string name;
        std::string description;
        std::string author;
        std::string version;
        std::vector<std::string> tags;
        bool isBuiltIn;
        bool isSystemProfile;
        
        ProfileInfo() : name(), description(), author(), version("1.0"), 
                       isBuiltIn(false), isSystemProfile(false) {}
        ProfileInfo(const std::string& name, const std::string& description = "")
            : name(name), description(description), author(), version("1.0"), 
              isBuiltIn(false), isSystemProfile(false) {}
    };

    ConfigProfile() = default;
    ~ConfigProfile() = default;

    // Add a configuration value to the profile
    template<typename T>
    void addValue(const std::string& key, const T& value,
                  const std::string& description = "");

    // Remove a value from the profile
    void removeValue(const std::string& key);

    // Apply profile to a configuration
    void applyTo(Config& config) const;

    // Get profile info
    const ProfileInfo& getInfo() const { return info; }
    ProfileInfo& getInfo() { return info; }

    // Get all value names
    std::vector<std::string> getValueNames() const;

    // Get value by key
    template<typename T>
    std::optional<T> getValue(const std::string& key) const;

    // Get value as any
    const std::any* getValueAny(const std::string& key) const;

    // Check if value exists
    bool hasValue(const std::string& key) const;

    // Merge another profile into this one
    void merge(const ConfigProfile& other, bool overwrite = true);

    // Export profile to file
    bool exportToFile(const std::string& filepath) const;

    // Import profile from file
    static std::unique_ptr<ConfigProfile> importFromFile(const std::string& filepath);

    // Clone profile
    std::unique_ptr<ConfigProfile> clone() const;

    // Get system profiles (built-in profiles)
    static std::vector<std::unique_ptr<ConfigProfile>> getSystemProfiles();

    // Get profile by name from system profiles
    static std::unique_ptr<ConfigProfile> getSystemProfile(const std::string& name);

private:
    ProfileInfo info;
    std::unordered_map<std::string, std::any> values;
    std::unordered_map<std::string, std::string> valueDescriptions;
};

// Template specializations for different types
// These provide type-safe implementations for adding and retrieving profile values

} // namespace nlm