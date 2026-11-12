#include "examples/MainRunner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>

namespace nlm {
namespace examples {

struct MainRunner::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::string configFile;
    bool verbose;
    std::shared_ptr<Logger> logger;
};

MainRunner::MainRunner(const std::string& configFile) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = std::make_shared<Config>();
    pImpl->configFile = configFile;
    pImpl->verbose = true;
    pImpl->logger = std::make_shared<Logger>();
    Logger::setGlobal(pImpl->logger);
    Logger::getGlobal().setLevel(LogLevel::Info);
}

MainRunner::~MainRunner() = default;

MainRunner::MainRunner(MainRunner&& other) noexcept : pImpl(std::move(other.pImpl)) {}

MainRunner& MainRunner::operator=(MainRunner&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void MainRunner::setConfigValue(const std::string& key, const std::string& value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

void MainRunner::setConfigValue(const std::string& key, int64_t value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

void MainRunner::setConfigValue(const std::string& key, double value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

void MainRunner::setConfigValue(const std::string& key, bool value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

bool MainRunner::loadConfigFromFile(const std::string& filepath) {
    if (pImpl->verbose) {
        std::cout << "Loading configuration from: " << filepath << std::endl;
    }
    
    bool success = pImpl->config->loadFromFile(filepath);
    if (success) {
        pImpl->configFile = filepath;
    }
    return success;
}

bool MainRunner::loadConfigFromArgs(int argc, char** argv) {
    if (pImpl->verbose) {
        std::cout << "Loading configuration from command line arguments..." << std::endl;
    }
    
    return pImpl->config->loadFromArgs(argc, argv);
}

bool MainRunner::initializeBrain() {
    if (pImpl->verbose) {
        std::cout << "Initializing NLM Brain..." << std::endl;
    }
    
    pImpl->brain = std::make_shared<Brain>(pImpl->config);
    
    if (!pImpl->brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    pImpl->brain->logStatus();
    return true;
}

void MainRunner::printBanner() const {
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

void MainRunner::printConfigSummary() const {
    if (!pImpl->config) return;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  random_seed: " << pImpl->config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << pImpl->config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << pImpl->config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  region_count: " << pImpl->config->getOr<int64_t>("region_count", 1) << std::endl;
    std::cout << "  connection_probability: " << pImpl->config->getOr<float>("connection_probability", 0.15f) << std::endl;
}

std::string MainRunner::getBrainStatus() const {
    std::ostringstream oss;
    oss << "Brain Status:" << std::endl;
    oss << "  Total neurons: " << (pImpl->brain ? pImpl->brain->getTotalNeuronCount() : 0) << std::endl;
    oss << "  Total synapses: " << (pImpl->brain ? pImpl->brain->getTotalSynapseCount() : 0) << std::endl;
    oss << "  Total spikes: " << (pImpl->brain ? pImpl->brain->getTotalSpikeCount() : 0) << std::endl;
    oss << "  Active neurons: " << (pImpl->brain ? pImpl->brain->getActiveNeuronCount() : 0) << std::endl;
    oss << "  Firing neurons: " << (pImpl->brain ? pImpl->brain->getFiringNeuronCount() : 0) << std::endl;
    return oss.str();
}

std::shared_ptr<const Config> MainRunner::getConfig() const {
    return pImpl->config;
}

std::shared_ptr<Brain> MainRunner::getBrain() const {
    return pImpl->brain;
}

}} // namespace nlm::examples