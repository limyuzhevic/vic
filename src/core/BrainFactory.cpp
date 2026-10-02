#include "../src/core/BrainFactory.hpp"

namespace nlm {

BrainFactory::BrainFactory() = default;
BrainFactory::~BrainFactory() = default;

std::shared_ptr<Brain> BrainFactory::createDefaultBrain(const std::string& configName) {
    if (configName == "default") {
        auto config = BrainConfigBuilder::defaultConfig();
        return std::make_shared<Brain>(config);
    } else if (configName == "minimal") {
        auto config = BrainConfigBuilder::minimalConfig();
        return std::make_shared<Brain>(config);
    } else if (configName == "experimental") {
        auto config = BrainConfigBuilder::experimentalConfig();
        return std::make_shared<Brain>(config);
    } else if (configName == "research") {
        auto config = BrainConfigBuilder::researchConfig();
        return std::make_shared<Brain>(config);
    } else {
        throw std::invalid_argument("Unknown configuration name: " + configName);
    }
}

std::shared_ptr<Brain> BrainFactory::createMinimalBrain() {
    auto config = BrainConfigBuilder::minimalConfig();
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Brain> BrainFactory::createExperimentalBrain() {
    auto config = BrainConfigBuilder::experimentalConfig();
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Brain> BrainFactory::createResearchBrain() {
    auto config = BrainConfigBuilder::researchConfig();
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Brain> BrainFactory::createFromConfig(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        throw std::runtime_error("Failed to load configuration from file: " + configFile);
    }
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Brain> BrainFactory::createWithCallback(const std::function<void(std::shared_ptr<Brain>)>& initCallback) {
    auto config = BrainConfigBuilder::defaultConfig();
    auto brain = std::make_shared<Brain>(config);
    initCallback(brain);
    return brain;
}

// Python bindings registration for BrainFactory
void BrainFactory::registerPythonBindings(py::module_& m) {
    py::class_<BrainFactory>(m, "BrainFactory")
        .def_static("create_default_brain", &BrainFactory::createDefaultBrain,
                    "Create a brain with preset configuration", py::arg("configName") = "default")
        .def_static("create_minimal_brain", &BrainFactory::createMinimalBrain,
                    "Create a minimal brain with reduced features")
        .def_static("create_experimental_brain", &BrainFactory::createExperimentalBrain,
                    "Create an experimental brain with enhanced features")
        .def_static("create_research_brain", &BrainFactory::createResearchBrain,
                    "Create a research-grade brain with maximum features")
        .def_static("create_from_config", &BrainFactory::createFromConfig,
                    "Create a brain from configuration file", py::arg("configFile"))
        .def_static("create_with_callback", &BrainFactory::createWithCallback,
                    "Create a brain with custom initialization callback", py::arg("initCallback"));
}

} // namespace nlm
