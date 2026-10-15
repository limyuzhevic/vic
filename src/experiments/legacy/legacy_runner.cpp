#include "legacy_experiment.hpp"
#include "legacy_runner.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

// Legacy experiment runner implementation
void LegacyExperimentRunner::setGlobalSeed(const std::string& seedStr) {
    try {
        uint64_t seed = std::stoul(seedStr);
        setGlobalSeed(seed);
    } catch (...) {
        setGlobalSeed(42); // Default seed
    }
}

std::string LegacyExperimentRunner::getStatus() const {
    std::stringstream ss;
    ss << "Legacy Experiment Runner Status:" << std::endl;
    ss << "  Number of experiments: " << getExperiments().size() << std::endl;
    ss << "  Global seed: " << runner_->getGlobalSeed() << std::endl; // Would need to add method
    return ss.str();
}

} // namespace nlm