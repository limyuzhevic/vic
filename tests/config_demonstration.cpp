// Config Demonstration Tests
// This file demonstrates enhanced configuration system capabilities
// These are integration-style tests showing the new features

#include "core/Config/Config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>

namespace test_config_demo {

TEST_CASE("Configuration preset availability", "[config_demo][presets]") {
    std::cout << "=== Configuration Presets Demo ===" << std::endl;
    
    const std::map<std::string, std::string> presetDescriptions = {
        {"beginner", "Easy setup for new users with sensible defaults"},
        {"expert", "Full control for advanced users with performance optimizations"},
        {"balanced", "Good compromise between performance and resource usage"},
        {"learning", "Optimized for machine learning tasks"},
        {"simulation", "Optimized for large-scale simulations"},
        {"research", "Optimized for research and experimentation"}
    };
    
    auto availablePresets = nlm::Config::getPresets();
    REQUIRE(!availablePresets.empty());
    
    std::cout << "Available presets:" << std::endl;
    for (const auto& preset : availablePresets) {
        if (presetDescriptions.find(preset) != presetDescriptions.end()) {
            std::cout << "  - " << preset << ": " 
                      << presetDescriptions.at(preset) << std::endl;
        }
    }
}

TEST_CASE("Configuration preset loading", "[config_demo][preset_loading]") {
    std::cout << "\nLoading configuration presets:
    << std::endl;
    
    // Test beginner preset
    nlm::Config beginnerConfig;
    REQUIRE(nlm::loadFromPreset(beginnerConfig, "beginner"));
    
    auto learningRate = beginnerConfig.getOr<std::string>("neural.learning_rate", "");
    auto hiddenSize = beginnerConfig.getOr<std::string>("neural.hidden_size", "");
    auto adaptationRate = beginnerConfig.getOr<std::string>("plasticity.adaptation_rate", "");
    
    std::cout << "Beginner preset:" << std::endl;
    std::cout << "  learning_rate: " << learningRate << std::endl;
    std::cout << "  hidden_size: " << hiddenSize << std::endl;
    std::cout << "  adaptation_rate: " << adaptationRate << std::endl;
    
    // Test expert preset
    nlm::Config expertConfig;
    REQUIRE(nlm::loadFromPreset(expertConfig, "expert"));
    
    auto attentionHeads = expertConfig.getOr<std::string>("neural.attention_heads", "");
    auto batchSize = expertConfig.getOr<std::string>("system.batch_size", "");
    auto stdpEnabled = expertConfig.getOr<bool>("plasticity.stdp_enabled", false);
    
    std::cout << "Expert preset:" << std::endl;
    std::cout << "  attention_heads: " << attentionHeads << std::endl;
    std::cout << "  batch_size: " << batchSize << std::endl;
    std::cout << "  stdp_enabled: " << (stdpEnabled ? "true" : "false") << std::endl;
    
    // Test balanced preset
    nlm::Config balancedConfig;
    REQUIRE(nlm::loadFromPreset(balancedConfig, "balanced"));
    
    auto balancedHiddenSize = balancedConfig.getOr<std::string>("neural.hidden_size", "");
    
    std::cout << "Balanced preset:" << std::endl;
    std::cout << "  hidden_size: " << balancedHiddenSize << std::endl;
}

} // namespace test_config_demo
