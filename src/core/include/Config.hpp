// Core configuration
#ifndef NLM_CORE_CONFIG_HPP
#define NLM_CORE_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {
namespace core {

class Config {
public:
    Config();
    ~Config();
    
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    bool has(const std::string& key) const;
    
    template<typename T>
    void set(const std::string& key, T value);
    
    template<typename T>
    T get(const std::string& key) const;
    
private:
    std::unordered_map<std::string, std::string> configData;
};

} // namespace core
} // namespace nlm

#endif // NLM_CORE_CONFIG_HPP
