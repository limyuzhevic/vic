#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Phase6Config::Impl {
    std::shared_ptr<Config> coreConfig;
};

Phase6Config::Phase6Config()
    : maxSteps(10000)
    , neuronCount(1000)
    , regionCount(1)
    , connectionProbability(0.1f)
    , enableCheckpointing(true)
    , enableReplay(true)
    , enableDevelopment(true)
    , checkpointPath("./checkpoint_test.bin")
    , pImpl(std::make_unique<Impl>()) {
    updateCoreConfig();
}

Phase6Config::~Phase6Config() = default;

Phase6Config::Phase6Config(Phase6Config&&) noexcept = default;

Phase6Config& Phase6Config::operator=(Phase6Config&&) noexcept = default;

Phase6Config Phase6Config::fromCoreConfig(const Config& config) {
    Phase6Config phase6Config;
    
    // Extract Phase6-specific values from core config
    if (auto val = config.get<int64_t>("phase6_max_steps")) {
        phase6Config.maxSteps = static_cast<uint64_t>(*val);
    }
    if (auto val = config.get<int64_t>("phase6_neuron_count")) {
        phase6Config.neuronCount = static_cast<size_t>(*val);
    }
    if (auto val = config.get<int64_t>("phase6_region_count")) {
        phase6Config.regionCount = static_cast<size_t>(*val);
    }
    if (auto val = config.get<float>("phase6_connection_probability")) {
        phase6Config.connectionProbability = *val;
    }
    if (auto val = config.get<bool>("phase6_enable_checkpointing")) {
        phase6Config.enableCheckpointing = *val;
    }
    if (auto val = config.get<bool>("phase6_enable_replay")) {
        phase6Config.enableReplay = *val;
    }
    if (auto val = config.get<bool>("phase6_enable_development")) {
        phase6Config.enableDevelopment = *val;
    }
    if (auto val = config.get<std::string>("phase6_checkpoint_path")) {
        phase6Config.checkpointPath = *val;
    }
    
    phase6Config.updateCoreConfig();
    return phase6Config;
}

std::shared_ptr<Config> Phase6Config::toCoreConfig() const {
    auto cfg = std::make_shared<Config>();
    cfg->set("phase6_max_steps", static_cast<int64_t>(maxSteps));
    cfg->set("phase6_neuron_count", static_cast<int64_t>(neuronCount));
    cfg->set("phase6_region_count", static_cast<int64_t>(regionCount));
    cfg->set("phase6_connection_probability", connectionProbability);
    cfg->set("phase6_enable_checkpointing", enableCheckpointing);
    cfg->set("phase6_enable_replay", enableReplay);
    cfg->set("phase6_enable_development", enableDevelopment);
    cfg->set("phase6_checkpoint_path", checkpointPath);
    
    // Set default brain config
    cfg->set("random_seed", static_cast<int64_t>(42));
    cfg->set("simulation_timestep", 0.001);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    return cfg;
}

bool Phase6Config::loadFromFile(const std::string& filepath) {
    if (pImpl->coreConfig->loadFromFile(filepath)) {
        *this = Phase6Config::fromCoreConfig(*pImpl->coreConfig);
        return true;
    }
    return false;
}

bool Phase6Config::saveToFile(const std::string& filepath) const {
    return pImpl->coreConfig->saveToFile(filepath);
}

bool Phase6Config::loadFromArgs(int argc, char** argv) {
    if (pImpl->coreConfig->loadFromArgs(argc, argv)) {
        *this = Phase6Config::fromCoreConfig(*pImpl->coreConfig);
        return true;
    }
    return false;
}

void Phase6Config::updateCoreConfig() {
    pImpl->coreConfig = std::make_shared<Config>();
    pImpl->coreConfig->set("phase6_max_steps", static_cast<int64_t>(maxSteps));
    pImpl->coreConfig->set("phase6_neuron_count", static_cast<int64_t>(neuronCount));
    pImpl->coreConfig->set("phase6_region_count", static_cast<int64_t>(regionCount));
    pImpl->coreConfig->set("phase6_connection_probability", connectionProbability);
    pImpl->coreConfig->set("phase6_enable_checkpointing", enableCheckpointing);
    pImpl->coreConfig->set("phase6_enable_replay", enableReplay);
    pImpl->coreConfig->set("phase6_enable_development", enableDevelopment);
    pImpl->coreConfig->set("phase6_checkpoint_path", checkpointPath);
}

// Core config key constants
const std::string Phase6Config::MAX_STEPS_KEY = "phase6_max_steps";
const std::string Phase6Config::NEURON_COUNT_KEY = "phase6_neuron_count";
const std::string Phase6Config::REGION_COUNT_KEY = "phase6_region_count";
const std::string Phase6Config::CONNECTION_PROBABILITY_KEY = "phase6_connection_probability";
const std::string Phase6Config::ENABLE_CHECKPOINTING_KEY = "phase6_enable_checkpointing";
const std::string Phase6Config::ENABLE_REPLAY_KEY = "phase6_enable_replay";
const std::string Phase6Config::ENABLE_DEVELOPMENT_KEY = "phase6_enable_development";
const std::string Phase6Config::CHECKPOINT_PATH_KEY = "phase6_checkpoint_path";

} // namespace nlm