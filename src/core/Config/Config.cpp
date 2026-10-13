#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::unique_ptr<ConfigurationManager> manager;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->manager = std::make_unique<ConfigurationManager>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return pImpl->manager->loadFromFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    return pImpl->manager->loadFromArgs(argc, argv);
}

bool Config::saveToFile(const std::string& filepath) const {
    return pImpl->manager->saveToFile(filepath);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return pImpl->manager->legacyGet<T>(key);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    return pImpl->manager->legacyGetOr<T>(key, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Convert from new ConfigSource to old enum
    ConfigSource newSource = ConfigSource::Runtime;
    switch (source) {
        case ConfigSource::Default: newSource = ConfigSource::Default; break;
        case ConfigSource::File: newSource = ConfigSource::File; break;
        case ConfigSource::CommandLine: newSource = ConfigSource::CommandLine; break;
        case ConfigSource::Runtime: newSource = ConfigSource::Runtime; break;
    }
    pImpl->manager->set(key, value, static_cast<ConfigurationManager::ConfigSource>(newSource));
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    set(key, configValue, source);
}

bool Config::has(const std::string& key) const {
    return pImpl->manager->has(key);
}

void Config::remove(const std::string& key) {
    pImpl->manager->remove(key);
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->manager->getKeys();
}

void Config::clear() {
    pImpl->manager->clear();
}

std::string Config::summary() const {
    return pImpl->manager->summary();
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
