#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration entry with type safety
struct ConfigEntry {
    std::string key;
    std::variant<int64_t, double, bool, std::string> value;
    enum class Source { File, CommandLine, Default } source;
    std::string description;
    
    ConfigEntry(const std::string& k, const ConfigValue& v, 
                Source s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Forward declaration for Config
class Config {
public:
    Config();
    ~Config();
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    
    // Configuration saving
    bool saveToFile(const std::string& filepath) const;
    std::string toYAML() const;
    std::string toJSON() const;
    
    // Configuration management
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, 
             ConfigEntry::Source source = ConfigEntry::Source::Default,
             const std::string& description = "");
    
    void set(const std::string& key, int64_t value, 
             ConfigEntry::Source source = ConfigEntry::Source::Default,
             const std::string& description = "");
    
    void set(const std::string& key, double value, 
             ConfigEntry::Source source = ConfigEntry::Source::Default,
             const std::string& description = "");
    
    void set(const std::string& key, bool value, 
             ConfigEntry::Source source = ConfigEntry::Source::Default,
             const std::string& description = "");
    
    void set(const std::string& key, const std::string& value, 
             ConfigEntry::Source source = ConfigEntry::Source::Default,
             const std::string& description = "");
    
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    
    // Configuration introspection
    std::vector<std::string> getKeys() const;
    std::string summary() const;
    void clear();
    
    // Validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Static utility functions
    static bool isJSON(const std::string& filepath);
    static bool isYAML(const std::string& filepath);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // CONFIG_HPP
