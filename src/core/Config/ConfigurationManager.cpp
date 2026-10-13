#include "ConfigurationManager.hpp"

namespace nlm {

ConfigurationManager::ConfigurationManager(ConfigurationManager&& other) noexcept
    : config_(std::move(other.config_)) {
}

ConfigurationManager& ConfigurationManager::operator=(ConfigurationManager&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
    }
    return *this;
}

} // namespace nlm