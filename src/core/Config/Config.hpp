#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include <string>
#include <memory>

namespace nlohmann {
    template<typename T>
    class basic_json;
    using json = basic_json<>;
}

namespace nlm {

class Config {
public:
    Config();
    ~Config();

    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& jsonContent);
    bool saveToFile(const std::string& filepath) const;
    bool saveToJson(std::string& jsonContent) const;

    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    void set(const std::string& key, const nlohmann::json& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);

    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;

    // Merge another config
    void merge(const Config& other);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& j);
};

} // namespace nlm

#endif // NLM_CONFIG_HPP
