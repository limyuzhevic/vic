#include "Config.hpp"
#include "ConfigSystem.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::unique_ptr<ConfigSystem> configSystem;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->configSystem = std::make_unique<ConfigSystem>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use the new ConfigSystem with proper JSON/YAML parsing
    return pImpl->configSystem->loadFromFile(filepath, false);
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Use the new ConfigSystem's argument parsing
    return pImpl->configSystem->loadFromArgs(argc, argv);
}

bool Config::saveToFile(const std::string& filepath) const {
    // For backward compatibility, save in the old key=value format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->configSystem->getKeys()) {
        auto entryOpt = pImpl->configSystem->getEntry(entry);
        if (entryOpt) {
            file << "# " << entryOpt->description << "\n";
            file << entry << " = ";
            
            // Get value as string
            try {
                if (entryOpt->value.index() == 0) {  // std::string
                    file << "\"" << std::get<std::string>(entryOpt->value) << "\"";
                } else if (entryOpt->value.index() == 1) {  // int
                    file << std::get<int>(entryOpt->value);
                } else if (entryOpt->value.index() == 2) {  // int64_t
                    file << std::get<int64_t>(entryOpt->value);
                } else if (entryOpt->value.index() == 3) {  // double
                    file << std::get<double>(entryOpt->value);
                } else if (entryOpt->value.index() == 4) {  // bool
                    file << (std::get<bool>(entryOpt->value) ? "true" : "false");
                } else {
                    file << "UNKNOWN";
                }
            } catch (...) {
                file << "UNKNOWN";
            }
            
            file << "\n\n";
        }
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return pImpl->configSystem->get<T>(key);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    return pImpl->configSystem->getOr<T>(key, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    pImpl->configSystem->set(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->configSystem->set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    pImpl->configSystem->set(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    pImpl->configSystem->set(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    pImpl->configSystem->set(key, value, source);
}

bool Config::has(const std::string& key) const {
    return pImpl->configSystem->has(key);
}

void Config::remove(const std::string& key) {
    pImpl->configSystem->remove(key);
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->configSystem->getKeys();
}

void Config::clear() {
    pImpl->configSystem->clear();
}

std::string Config::summary() const {
    return pImpl->configSystem->summary();
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
