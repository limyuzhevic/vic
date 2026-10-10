#include "Configuration.hpp"

namespace nlm {

std::unique_ptr<Config> createDefaultConfig() {
    return std::make_unique<Config>();
}

std::unique_ptr<Config> createConfigFromFile(const std::string& filepath) {
    auto config = std::make_unique<Config>();
    if (!config->loadFromFile(filepath)) {
        return nullptr;
    }
    return config;
}

std::unique_ptr<Config> createConfigFromArgs(int argc, char** argv) {
    auto config = std::make_unique<Config>();
    if (!config->loadFromArgs(argc, argv)) {
        return nullptr;
    }
    return config;
}

} // namespace nlm
