#include "BrainError.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "Neuron.hpp"
#include "NeuralRegion.hpp"
#include "NeuralPopulation.hpp"

namespace nlm {

// BrainValidator implementation
bool BrainValidator::validateConfig(std::shared_ptr<const Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Config is null");
        return false;
    }
    
    // Check for required configuration keys
    auto neuronCountOpt = config->get<size_t>("neuron_count");
    auto regionCountOpt = config->get<size_t>("region_count");
    
    if (!neuronCountOpt || neuronCountOpt.value() == 0) {
        NLM_LOG_ERROR("Invalid neuron_count in configuration");
        return false;
    }
    
    if (!regionCountOpt || regionCountOpt.value() == 0) {
        NLM_LOG_ERROR("Invalid region_count in configuration");
        return false;
    }
    
    auto connectionProbOpt = config->get<float>("connection_probability");
    if (connectionProbOpt && (connectionProbOpt.value() < 0.0f || connectionProbOpt.value() > 1.0f)) {
        NLM_LOG_ERROR("Invalid connection_probability in configuration");
        return false;
    }
    
    return true;
}

bool BrainValidator::validateRegions() const {
    // This would need access to regions, moved from Brain class
    return true; // Placeholder
}

bool BrainValidator::validateNeurons() const {
    // This would need access to neurons, moved from Brain class
    return true; // Placeholder
}

bool BrainValidator::validateConnections() const {
    // This would need access to connections, moved from Brain class
    return true; // Placeholder
}

bool BrainValidator::validateSystems() const {
    // This would need access to systems, moved from Brain class
    return true; // Placeholder
}

// BrainUtils implementation
void BrainUtils::logError(ILogger& logger, const std::string& operation, const std::string& error) {
    logger.error("Brain " + operation + " failed: " + error);
}

void BrainUtils::logWarning(ILogger& logger, const std::string& operation, const std::string& warning) {
    logger.warning("Brain " + operation + ": " + warning);
}

void BrainUtils::logInfo(ILogger& logger, const std::string& operation, const std::string& info) {
    logger.info("Brain " + operation + ": " + info);
}

template<typename RegionFunc>
void BrainUtils::forEachRegion(const std::vector<std::unique_ptr<NeuralRegion>>& regions, RegionFunc func) {
    for (auto& region : regions) {
        func(region.get());
    }
}

template<typename PopulationFunc>
void BrainUtils::forEachPopulation(const std::vector<NeuralPopulation*>& populations, PopulationFunc func) {
    for (auto& pop : populations) {
        func(pop);
    }
}

template<typename NeuronFunc>
void BrainUtils::forEachNeuron(const std::vector<Neuron*>& neurons, NeuronFunc func) {
    for (auto& neuron : neurons) {
        func(neuron);
    }
}

size_t BrainUtils::countTotalNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t total = 0;
    for (auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainUtils::countTotalSynapses(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t total = 0;
    for (auto& region : regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t BrainUtils::countActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t total = 0;
    for (auto& region : regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainUtils::countFiringNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // This would need access to firing neuron count from Brain
    return 0; // Placeholder
}

// Exception throwing helpers
void throwIfNot(bool condition, const std::string& operation, const std::string& message) {
    if (!condition) {
        NLM_LOG_ERROR("Validation failed during " + operation + ": " + message);
        throw BrainValidationError("Validation failed during " + operation + ": " + message);
    }
}

void throwIfNullptr(void* ptr, const std::string& operation, const std::string& message) {
    if (ptr == nullptr) {
        NLM_LOG_ERROR("Null pointer encountered during " + operation + ": " + message);
        throw BrainError("Null pointer encountered during " + operation + ": " + message);
    }
}

void throwIfError(bool error, const std::string& operation, const std::string& message) {
    if (error) {
        NLM_LOG_ERROR("Error occurred during " + operation + ": " + message);
        throw BrainError("Error occurred during " + operation + ": " + message);
    }
}

} // namespace nlm
