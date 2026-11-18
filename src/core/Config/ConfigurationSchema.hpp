#pragma once

#include <string>
#include <vector>
#include <variant>

namespace nlm {

using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

struct SchemaEntry {
    std::string type;
    std::string defaultValue;
    std::string description;
    bool required;
};

class ConfigurationSchema {
public:
    ConfigurationSchema();
    
    void addEntry(const std::string& key, const SchemaEntry& entry);
    bool validate(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValidKeys() const;
    bool hasKey(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm