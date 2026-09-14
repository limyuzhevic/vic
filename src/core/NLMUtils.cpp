#include "NLMUtils.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

NLMUtils::NLMUtils() {}

NLMUtils::~NLMUtils() {}

void NLMUtils::initializePhase2Config(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void NLMUtils::setupGlobalLogger() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void NLMUtils::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void NLMUtils::displayConfigSummary(const Config& config) {
    std::cout << "" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  random_seed: " << std::to_string(config.getOr<int64_t>("random_seed", 42)) << std::endl;
    std::cout << "  simulation_timestep: " << std::to_string(config.getOr<double>("simulation_timestep", 0.001)) << "s" << std::endl;
    std::cout << "  neuron_count: " << std::to_string(config.getOr<int64_t>("neuron_count", 500)) << std::endl;
    std::cout << "  region_count: " << std::to_string(config.getOr<int64_t>("region_count", 1)) << std::endl;
    std::cout << "  connection_probability: " << std::to_string(config.getOr<float>("connection_probability", 0.15f)) << std::endl;
    std::cout << "" << std::endl;
}

} // namespace nlm