#ifndef NLM_CORE_CONFIG_CONFIG_HPP
#define NLM_CORE_CONFIG_CONFIG_HPP

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace nlm {

enum class ConfigSource {
    Default,
    File,
    CommandLine,
    User
};

class Config {
public:
    Config();
    ~Config();
    
    bool loadFromFile(const std::string& filepath);
    void loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::User);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::User);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::User);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::User);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::User);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    bool hasKey(const std::string& key) const;
    
    void remove(const std::string& key);
    void clear();
    
    size_t size() const { return configData.size(); }
    
    const std::map<std::string, std::string>& getAllData() const { return configData; }
    
    void validate() const;
    
    std::string toString() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_CORE_CONFIG_CONFIG_HPP
