#include "NeuralParameters.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

struct ConfigurationDrivenNeuralParameters::Impl {
    // Default configuration values for each parameter type
    static const std::unordered_map<std::string, float> defaultFloatParams;
    static const std::unordered_map<std::string, std::string> defaultStringParams;
    static const std::unordered_map<std::string, int64_t> defaultIntParams;
    
    // Parameter validation bounds
    static const float MIN_WEIGHT;
    static const float MAX_WEIGHT;
    static const float MIN_POTENTIAL;
    static const float MAX_POTENTIAL;
    static const float MIN_TIME_CONSTANT;
    static const float MAX_TIME_CONSTANT;
    static const float MIN_REFACTORY_PERIOD;
    static const float MAX_REFACTORY_PERIOD;
    
    // Configuration file parsing
    bool parseConfigLine(const std::string& line, std::string& key, std::string& value) const;
    bool parseFloatConfig(const std::string& line, float& value) const;
    bool parseIntConfig(const std::string& line, int64_t& value) const;
    
    // Configuration validation
    bool validateFloatBounds(float value, float min, float max, const std::string& paramName) const;
    bool validateIntBounds(int64_t value, int64_t min, int64_t max, const std::string& paramName) const;
    
    // Parameter application
    void applyNeuronParameters(Neuron* neuron, const NeuralParameters::NeuronParams& params) const;
    void applySynapseParameters(Synapse* synapse, const NeuralParameters::SynapseParams& params) const;
    void applyPlasticityParameters(STDP* stdp, const NeuralParameters::PlasticityParams& params) const;
    void applyDevelopmentParameters(DevelopmentSystem* devSys, 
                                   const NeuralParameters::DevelopmentParams& params) const;
    
    // Parameter scaling for developmental stages
    float getPlasticityForStage(float start, float end, float currentAge, 
                               float criticalPeriod, float maturationRate) const;
};

ConfigurationDrivenNeuralParameters::ConfigurationDrivenNeuralParameters() : pImpl(std::make_unique<Impl>()) {}

ConfigurationDrivenNeuralParameters::~ConfigurationDrivenNeuralParameters() = default;

bool ConfigurationDrivenNeuralParameters::loadFromFile(const std::string& filepath, NeuralParameters& params) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::string key, value;
        if (pImpl->parseConfigLine(line, key, value)) {
            // Apply parameter based on key
            if (key.find("neuron_") == 0) {
                // Neuron parameter - extract specific parameter name
                std::string paramName = key.substr(6);
                bool success = false;
                float floatValue;
                
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    if (applyFloatNeuronParameter(params.neuron, paramName, floatValue)) {
                        success = true;
                    }
                }
                
                if (!success) {
                    NLM_LOG_WARNING("Unknown or invalid neuron parameter in config: " + key);
                }
            } else if (key.find("synapse_") == 0) {
                std::string paramName = key.substr(7);
                bool success = false;
                float floatValue;
                
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    if (applyFloatSynapseParameter(params.synapse, paramName, floatValue)) {
                        success = true;
                    }
                }
                
                if (!success) {
                    NLM_LOG_WARNING("Unknown or invalid synapse parameter in config: " + key);
                }
            } else if (key.find("plasticity_") == 0) {
                std::string paramName = key.substr(11);
                bool success = false;
                float floatValue;
                
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    if (applyFloatPlasticityParameter(params.plasticity, paramName, floatValue)) {
                        success = true;
                    }
                }
                
                if (!success) {
                    NLM_LOG_WARNING("Unknown or invalid plasticity parameter in config: " + key);
                }
            } else if (key.find("structural_") == 0) {
                std::string paramName = key.substr(11);
                bool success = false;
                float floatValue;
                
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    if (applyFloatStructuralParameter(params.structural, paramName, floatValue)) {
                        success = true;
                    }
                }
                
                if (!success) {
                    NLM_LOG_WARNING("Unknown or invalid structural parameter in config: " + key);
                }
            } else if (key.find("neuromodulation_") == 0) {
                std::string paramName = key.substr(16);
                bool success = false;
                float floatValue;
                
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    if (applyFloatNeuromodulationParameter(params.neuromodulation, paramName, floatValue)) {
                        success = true;
                    }
                }
                
                if (!success) {
                    NLM_LOG_WARNING("Unknown or invalid neuromodulation parameter in config: " + key);
                }
            } else if (key.find("development_") == 0) {
                std::string paramName = key.substr(11);
                bool success = false;
                float floatValue;
                
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    if (applyFloatDevelopmentParameter(params.development, paramName, floatValue)) {
                        success = true;
                    }
                }
                
                if (!success) {
                    NLM_LOG_WARNING("Unknown or invalid development parameter in config: " + key);
                }
            } else {
                // Generic parameter
                float floatValue;
                if (pImpl->parseFloatConfig(value, floatValue)) {
                    applyGenericParameter(params, key, floatValue);
                }
            }
        }
    }
    
    NLM_LOG_INFO("Configuration loaded from file: " + filepath);
    return true;
}

bool ConfigurationDrivenNeuralParameters::loadFromConfig(const std::shared_ptr<Config>& config, NeuralParameters& params) {
    if (!config) {
        NLM_LOG_ERROR("Invalid configuration object");
        return false;
    }
    
    // Get neuron parameters
    if (auto neuronCap = config->get<float>("neuron_capacitance")) {
        params.neuron.membraneCapacitance = *neuronCap;
    }
    if (auto timeConst = config->get<float>("neuron_time_constant")) {
        params.neuron.timeConstant = *timeConst;
    }
    if (auto restingPot = config->get<float>("neuron_resting_potential")) {
        params.neuron.restingPotential = *restingPot;
    }
    if (auto threshold = config->get<float>("neuron_threshold")) {
        params.neuron.threshold = *threshold;
    }
    if (auto resetPot = config->get<float>("neuron_reset_potential")) {
        params.neuron.resetPotential = *resetPot;
    }
    
    // Get synapse parameters
    if (auto excWeight = config->get<float>("synapse_exc_weight")) {
        params.synapse.initialExcitatoryWeight = *excWeight;
    }
    if (auto inhWeight = config->get<float>("synapse_inh_weight")) {
        params.synapse.initialInhibitoryWeight = *inhWeight;
    }
    if (auto maxWeight = config->get<float>("synapse_max_weight")) {
        params.synapse.maxWeight = *maxWeight;
    }
    
    // Get plasticity parameters
    if (auto stdpLTP = config->get<float>("plasticity_stdp_ltp")) {
        params.plasticity.stdpLTPWeight = *stdpLTP;
    }
    if (auto stdpLTD = config->get<float>("plasticity_stdp_ltd")) {
        params.plasticity.stdpLTDWeight = *stdpLTD;
    }
    if (auto stdpTau = config->get<float>("plasticity_stdp_tau")) {
        params.plasticity.stdpTau = *stdpTau;
    }
    
    // Get structural parameters
    if (auto synRate = config->get<float>("structural_synaptogenesis")) {
        params.structural.synaptogenesisRate = *synRate;
    }
    if (auto pruneRate = config->get<float>("structural_pruning")) {
        params.structural.pruningRate = *pruneRate;
    }
    
    // Get neuromodulation parameters
    if (auto dopamine = config->get<float>("neuromodulation_dopamine")) {
        params.neuromodulation.dopamineBaseline = *dopamine;
    }
    
    // Get development parameters
    if (auto plasticStart = config->get<float>("development_plasticity_start")) {
        params.development.plasticityRateStart = *plasticStart;
    }
    if (auto plasticEnd = config->get<float>("development_plasticity_end")) {
        params.development.plasticityRateEnd = *plasticEnd;
    }
    
    NLM_LOG_INFO("Configuration loaded from config object");
    return true;
}

bool ConfigurationDrivenNeuralParameters::loadFromArgs(int argc, char** argv, NeuralParameters& params) {
    // Simple command-line argument parsing
    // Format: --param value
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                
                // Try to parse as float
                char* end;
                float floatValue = std::strtof(value.c_str(), &end);
                if (*end == '\0') {
                    applyGenericParameter(params, key, floatValue);
                } else {
                    // Try to parse as int
                    char* end2;
                    long longValue = std::strtoll(value.c_str(), &end2, 10);
                    if (*end2 == '\0') {
                        applyGenericIntParameter(params, key, longValue);
                    } else {
                        NLM_LOG_WARNING("Unknown or invalid parameter: " + key + " = " + value);
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Configuration loaded from command line arguments");
    return true;
}

void ConfigurationDrivenNeuralParameters::applyToBrain(std::shared_ptr<Brain> brain, const NeuralParameters& params) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot apply parameters to null brain");
        return;
    }
    
    NLM_LOG_INFO("Applying configuration-driven parameters to brain");
    
    // Apply neuron parameters
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                pImpl->applyNeuronParameters(neuron, params.neuron);
            }
        }
    }
    
    // Apply synapse parameters
    for (auto& region : brain->getRegions()) {
        for (auto* synapse : region->getSynapses()) {
            pImpl->applySynapseParameters(synapse, params.synapse);
        }
    }
    
    // Apply plasticity system parameters
    if (auto* stdp = brain->getSTDP()) {
        pImpl->applyPlasticityParameters(stdp, params.plasticity);
    }
    
    // Apply development system parameters
    if (auto* devSys = brain->getDevelopmentSystem()) {
        pImpl->applyDevelopmentParameters(devSys, params.development);
    }
    
    // Note: Neuromodulation parameters are system-wide, not synapse-specific
    NLM_LOG_INFO("Configuration-driven parameters applied successfully");
}

bool ConfigurationDrivenNeuralParameters::validate(const NeuralParameters& params) const {
    bool valid = true;
    
    // Validate neuron parameters
    valid &= pImpl->validateFloatBounds(params.neuron.membraneCapacitance, 
                                       pImpl->MIN_TIME_CONSTANT, pImpl->MAX_TIME_CONSTANT, 
                                       "neuron.membraneCapacitance");
    valid &= pImpl->validateFloatBounds(params.neuron.timeConstant,
                                       pImpl->MIN_TIME_CONSTANT, pImpl->MAX_TIME_CONSTANT,
                                       "neuron.timeConstant");
    valid &= pImpl->validateFloatBounds(params.neuron.restingPotential,
                                       pImpl->MIN_POTENTIAL, pImpl->MAX_POTENTIAL,
                                       "neuron.restingPotential");
    valid &= pImpl->validateFloatBounds(params.neuron.threshold,
                                       pImpl->MIN_POTENTIAL, pImpl->MAX_POTENTIAL,
                                       "neuron.threshold");
    valid &= pImpl->validateFloatBounds(params.neuron.resetPotential,
                                       pImpl->MIN_POTENTIAL, pImpl->MAX_POTENTIAL,
                                       "neuron.resetPotential");
    valid &= pImpl->validateFloatBounds(params.neuron.refractoryPeriod,
                                       pImpl->MIN_REFACTORY_PERIOD, pImpl->MAX_REFACTORY_PERIOD,
                                       "neuron.refractoryPeriod");
    
    // Validate synapse parameters
    valid &= pImpl->validateFloatBounds(params.synapse.initialExcitatoryWeight,
                                       pImpl->MIN_WEIGHT, pImpl->MAX_WEIGHT,
                                       "synapse.initialExcitatoryWeight");
    valid &= pImpl->validateFloatBounds(params.synapse.initialInhibitoryWeight,
                                       pImpl->MIN_WEIGHT, pImpl->MAX_WEIGHT,
                                       "synapse.initialInhibitoryWeight");
    valid &= pImpl->validateFloatBounds(params.synapse.maxWeight,
                                       pImpl->MIN_WEIGHT, pImpl->MAX_WEIGHT,
                                       "synapse.maxWeight");
    
    // Validate plasticity parameters
    valid &= pImpl->validateFloatBounds(params.plasticity.stdpLTPWeight,
                                       0.0f, 10.0f, "plasticity.stdpLTPWeight");
    valid &= pImpl->validateFloatBounds(params.plasticity.stdpLTDWeight,
                                       0.0f, 10.0f, "plasticity.stdpLTDWeight");
    valid &= pImpl->validateFloatBounds(params.plasticity.stdpTau,
                                       pImpl->MIN_TIME_CONSTANT, pImpl->MAX_TIME_CONSTANT,
                                       "plasticity.stdpTau");
    
    // Validate structural parameters
    valid &= pImpl->validateFloatBounds(params.structural.synaptogenesisRate,
                                       0.0f, 1.0f, "structural.synaptogenesisRate");
    valid &= pImpl->validateFloatBounds(params.structural.pruningRate,
                                       0.0f, 1.0f, "structural.pruningRate");
    
    return valid;
}

NeuralParameters ConfigurationDrivenNeuralParameters::getDefaultParameters(const std::string& preset) {
    if (preset == "development") {
        return ParameterPresets::createDevelopmentPreset();
    } else if (preset == "learning") {
        return ParameterPresets::createLearningPreset();
    } else if (preset == "mature") {
        return ParameterPresets::createMaturePreset();
    } else if (preset == "minimal") {
        return ParameterPresets::createMinimalPreset();
    } else if (preset == "high_plasticity") {
        return ParameterPresets::createHighPlasticityPreset();
    } else if (preset == "low_noise") {
        return ParameterPresets::createLowNoisePreset();
    } else {
        return ParameterPresets::createStandardPreset();
    }
}

NeuralParameters ConfigurationDrivenNeuralParameters::getParametersForDevelopmentalStage(
    const DevelopmentParams& devParams, 
    DevelopmentalStage stage) {
    NeuralParameters params = getDefaultParameters();
    
    float plasticityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;  // Stable
            break;
    }
    
    params.development.plasticityRateStart *= plasticityMod;
    params.development.plasticityRateEnd *= plasticityMod;
    params.structural.synaptogenesisRate *= plasticityMod;
    params.structural.pruningRate *= (2.0f - plasticityMod);
    
    return params;
}

bool ConfigurationDrivenNeuralParameters::exportToFile(const std::string& filepath, const NeuralParameters& params) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create configuration file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (Exported)" << std::endl;
    file << "# Generated by ConfigurationDrivenNeuralParameters" << std::endl;
    file << "# Version: " << params.configVersion << std::endl;
    file << std::endl;
    
    // Write neuron parameters
    file << "# Neuron Parameters" << std::endl;
    file << "neuron_capacitance = " << params.neuron.membraneCapacitance << std::endl;
    file << "neuron_time_constant = " << params.neuron.timeConstant << std::endl;
    file << "neuron_resting_potential = " << params.neuron.restingPotential << std::endl;
    file << "neuron_threshold = " << params.neuron.threshold << std::endl;
    file << "neuron_reset_potential = " << params.neuron.resetPotential << std::endl;
    file << "neuron_refractory_period = " << params.neuron.refractoryPeriod << std::endl;
    file << std::endl;
    
    // Write synapse parameters
    file << "# Synapse Parameters" << std::endl;
    file << "synapse_exc_weight = " << params.synapse.initialExcitatoryWeight << std::endl;
    file << "synapse_inh_weight = " << params.synapse.initialInhibitoryWeight << std::endl;
    file << "synapse_max_weight = " << params.synapse.maxWeight << std::endl;
    file << std::endl;
    
    // Write plasticity parameters
    file << "# Plasticity Parameters" << std::endl;
    file << "plasticity_stdp_ltp = " << params.plasticity.stdpLTPWeight << std::endl;
    file << "plasticity_stdp_ltd = " << params.plasticity.stdpLTDWeight << std::endl;
    file << "plasticity_stdp_tau = " << params.plasticity.stdpTau << std::endl;
    file << "plasticity_hebbian_rate = " << params.plasticity.hebbianLearningRate << std::endl;
    file << std::endl;
    
    // Write structural parameters
    file << "# Structural Plasticity Parameters" << std::endl;
    file << "structural_synaptogenesis = " << params.structural.synaptogenesisRate << std::endl;
    file << "structural_pruning = " << params.structural.pruningRate << std::endl;
    file << std::endl;
    
    // Write neuromodulation parameters
    file << "# Neuromodulation Parameters" << std::endl;
    file << "neuromodulation_dopamine = " << params.neuromodulation.dopamineBaseline << std::endl;
    file << "neuromodulation_curiosity_gain = " << params.neuromodulation.curiosityDecay << std::endl;
    file << std::endl;
    
    // Write development parameters
    file << "# Development Parameters" << std::endl;
    file << "development_plasticity_start = " << params.development.plasticityRateStart << std::endl;
    file << "development_plasticity_end = " << params.development.plasticityRateEnd << std::endl;
    file << "development_critical_period = " << params.development.criticalPeriodStart << std::endl;
    file << std::endl;
    
    file.close();
    NLM_LOG_INFO("Configuration exported to file: " + filepath);
    return true;
}

NeuralParameters ConfigurationDrivenNeuralParameters::mergeParameters(const NeuralParameters& fileParams, 
                                                                      const NeuralParameters& defaultParams) const {
    NeuralParameters merged = defaultParams;
    
    // Use file parameters where they exist, fallback to defaults otherwise
    if (fileParams.neuron.membraneCapacitance != 1.0f) {
        merged.neuron.membraneCapacitance = fileParams.neuron.membraneCapacitance;
    }
    if (fileParams.neuron.timeConstant != 20.0f) {
        merged.neuron.timeConstant = fileParams.neuron.timeConstant;
    }
    // ... continue for all parameters
    
    return merged;
}

// Implementation of helper functions
bool ConfigurationDrivenNeuralParameters::Impl::parseConfigLine(const std::string& line, 
                                                             std::string& key, 
                                                             std::string& value) const {
    size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos || equalsPos == 0) {
        return false;
    }
    
    key = line.substr(0, equalsPos);
    value = line.substr(equalsPos + 1);
    
    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    return true;
}

bool ConfigurationDrivenNeuralParameters::Impl::parseFloatConfig(const std::string& line, float& value) const {
    std::istringstream iss(line);
    return (iss >> value) && iss.eof();
}

bool ConfigurationDrivenNeuralParameters::Impl::parseIntConfig(const std::string& line, int64_t& value) const {
    std::istringstream iss(line);
    return (iss >> value) && iss.eof();
}

bool ConfigurationDrivenNeuralParameters::Impl::validateFloatBounds(float value, float min, float max, 
                                                                   const std::string& paramName) const {
    if (value < min || value > max) {
        NLM_LOG_WARNING("Parameter " + paramName + " out of bounds: " + 
                       std::to_string(value) + " (expected " + std::to_string(min) + 
                       " to " + std::to_string(max) + ")");
        return false;
    }
    return true;
}

bool ConfigurationDrivenNeuralParameters::Impl::validateIntBounds(int64_t value, int64_t min, int64_t max, 
                                                                 const std::string& paramName) const {
    if (value < min || value > max) {
        NLM_LOG_WARNING("Parameter " + paramName + " out of bounds: " + 
                       std::to_string(value) + " (expected " + std::to_string(min) + 
                       " to " + std::to_string(max) + ")");
        return false;
    }
    return true;
}

void ConfigurationDrivenNeuralParameters::Impl::applyNeuronParameters(Neuron* neuron, 
                                                                    const NeuralParameters::NeuronParams& params) const {
    if (!neuron) return;
    
    // Apply parameters with some randomization for biological realism
    // Get current random generator
    auto* rng = neuron->getRandomGenerator();
    if (rng) {
        // Apply parameters with small random variations
        neuron->setMembranePotential(params.restingPotential + 
                                   rng->uniformReal(-1.0f, 1.0f));
        neuron->setThreshold(params.threshold + rng->uniformReal(-0.5f, 0.5f));
        neuron->setResetPotential(params.resetPotential + rng->uniformReal(-0.5f, 0.5f));
        neuron->setRefractoryPeriod(static_cast<uint32_t>(params.refractoryPeriod));
        neuron->setLeakConductance(params.leakConductance);
    } else {
        // No random generator, apply directly
        neuron->setMembranePotential(params.restingPotential);
        neuron->setThreshold(params.threshold);
        neuron->setResetPotential(params.resetPotential);
        neuron->setRefractoryPeriod(static_cast<uint32_t>(params.refractoryPeriod));
        neuron->setLeakConductance(params.leakConductance);
    }
}

void ConfigurationDrivenNeuralParameters::Impl::applySynapseParameters(Synapse* synapse, 
                                                                    const NeuralParameters::SynapseParams& params) const {
    if (!synapse) return;
    
    // Initialize synapse weight based on type with configured bounds
    auto* rng = synapse->getRandomGenerator();
    if (rng) {
        if (synapse->getType() == SynapseType::Excitatory) {
            float weight = rng->uniformReal(params.initialExcitatoryWeight * 0.8f, 
                                          params.initialExcitatoryWeight * 1.2f);
            synapse->setWeight(std::clamp(weight, params.minWeight, params.maxWeight));
        } else if (synapse->getType() == SynapseType::Inhibitory) {
            float weight = -rng->uniformReal(params.initialInhibitoryWeight * 0.8f, 
                                            params.initialInhibitoryWeight * 1.2f);
            synapse->setWeight(std::clamp(weight, params.minWeight, params.maxWeight));
        } else {
            float weight = rng->uniformReal(params.initialOtherWeight * 0.8f, 
                                          params.initialOtherWeight * 1.2f);
            synapse->setWeight(std::clamp(weight, params.minWeight, params.maxWeight));
        }
    } else {
        // No random generator, use default
        if (synapse->getType() == SynapseType::Excitatory) {
            synapse->setWeight(params.initialExcitatoryWeight);
        } else if (synapse->getType() == SynapseType::Inhibitory) {
            synapse->setWeight(params.initialInhibitoryWeight);
        } else {
            synapse->setWeight(params.initialOtherWeight);
        }
    }
    
    // Set efficacy
    if (rng) {
        float efficacy = rng->uniformReal(params.efficacyRangeMin, params.efficacyRangeMax);
        synapse->setEfficacy(efficacy);
    } else {
        synapse->setEfficacy(params.efficacyRangeMin);
    }
}

void ConfigurationDrivenNeuralParameters::Impl::applyPlasticityParameters(STDP* stdp, 
                                                                         const NeuralParameters::PlasticityParams& params) const {
    if (!stdp) return;
    
    stdp->configure(params.stdpLTPWeight, params.stdpLTDWeight, params.stdpTau);
    // Note: STDP class may not have learning rate configuration, this is a placeholder
}

void ConfigurationDrivenNeuralParameters::Impl::applyDevelopmentParameters(DevelopmentSystem* devSys, 
                                                                          const NeuralParameters::DevelopmentParams& params) const {
    if (!devSys) return;
    
    // Development system typically has its own configuration
    // This is a placeholder for future integration
    NLM_LOG_INFO("Development parameters applied (implementation pending)");
}

float ConfigurationDrivenNeuralParameters::Impl::getPlasticityForStage(float start, float end, float currentAge, 
                                                                      float criticalPeriod, float maturationRate) const {
    if (currentAge < criticalPeriod) {
        // During critical period, higher plasticity
        return start + (end - start) * (1.0f - currentAge / criticalPeriod) * 0.5f;
    } else {
        // After critical period, decaying plasticity
        float ageExcess = currentAge - criticalPeriod;
        return end + (start - end) * std::exp(-ageExcess * maturationRate);
    }
}

// Implementation of parameter application helper functions
void applyFloatNeuronParameter(NeuralParameters::NeuronParams& params, 
                               const std::string& paramName, float value) {
    if (paramName == "membraneCapacitance") params.membraneCapacitance = value;
    else if (paramName == "timeConstant") params.timeConstant = value;
    else if (paramName == "restingPotential") params.restingPotential = value;
    else if (paramName == "threshold") params.threshold = value;
    else if (paramName == "resetPotential") params.resetPotential = value;
    else if (paramName == "refractoryPeriod") params.refractoryPeriod = value;
    else if (paramName == "leakConductance") params.leakConductance = value;
}

void applyFloatSynapseParameter(NeuralParameters::SynapseParams& params, 
                                const std::string& paramName, float value) {
    if (paramName == "initialExcitatoryWeight") params.initialExcitatoryWeight = value;
    else if (paramName == "initialInhibitoryWeight") params.initialInhibitoryWeight = value;
    else if (paramName == "maxWeight") params.maxWeight = value;
    else if (paramName == "efficacyRangeMin") params.efficacyRangeMin = value;
    else if (paramName == "efficacyRangeMax") params.efficacyRangeMax = value;
    else if (paramName == "depressionTimeConstant") params.depressionTimeConstant = value;
    else if (paramName == "facilitationTimeConstant") params.facilitationTimeConstant = value;
}

void applyFloatPlasticityParameter(NeuralParameters::PlasticityParams& params, 
                                   const std::string& paramName, float value) {
    if (paramName == "stdpLTPWeight") params.stdpLTPWeight = value;
    else if (paramName == "stdpLTDWeight") params.stdpLTDWeight = value;
    else if (paramName == "stdpTau") params.stdpTau = value;
    else if (paramName == "hebbianLearningRate") params.hebbianLearningRate = value;
    else if (paramName == "hebbianTimeConstant") params.hebbianTimeConstant = value;
    else if (paramName == "dopaminePlasticityMod") params.dopaminePlasticityMod = value;
    else if (paramName == "rewardPredictionError") params.rewardPredictionError = value;
}

void applyFloatStructuralParameter(NeuralParameters::StructuralParams& params, 
                                   const std::string& paramName, float value) {
    if (paramName == "synaptogenesisRate") params.synaptogenesisRate = value;
    else if (paramName == "pruningRate") params.pruningRate = value;
    else if (paramName == "competitionThreshold") params.competitionThreshold = value;
    else if (paramName == "ageDependentDecay") params.ageDependentDecay = value;
    else if (paramName == "criticalPeriodEnd") params.criticalPeriodEnd = value;
}

void applyFloatNeuromodulationParameter(NeuralParameters::NeuromodulationParams& params, 
                                        const std::string& paramName, float value) {
    if (paramName == "dopamineBaseline") params.dopamineBaseline = value;
    else if (paramName == "dopamineGain") params.dopamineGain = value;
    else if (paramName == "curiosityDecay") params.curiosityDecay = value;
    else if (paramName == "noveltyThreshold") params.noveltyThreshold = value;
    else if (paramName == "predictionErrorGain") params.predictionErrorGain = value;
}

void applyFloatDevelopmentParameter(NeuralParameters::DevelopmentParams& params, 
                                    const std::string& paramName, float value) {
    if (paramName == "plasticityRateStart") params.plasticityRateStart = value;
    else if (paramName == "plasticityRateEnd") params.plasticityRateEnd = value;
    else if (paramName == "criticalPeriodStart") params.criticalPeriodStart = value;
    else if (paramName == "maturationRate") params.maturationRate = value;
    else if (paramName == "structuralPlasticityStart") params.structuralPlasticityStart = value;
    else if (paramName == "structuralPlasticityEnd") params.structuralPlasticityEnd = value;
}

void applyGenericParameter(NeuralParameters& params, const std::string& paramName, float value) {
    // Generic parameter application - check various parameter categories
    if (paramName.find("neuron_") == 0) {
        std::string subName = paramName.substr(6);
        applyFloatNeuronParameter(params.neuron, subName, value);
    } else if (paramName.find("synapse_") == 0) {
        std::string subName = paramName.substr(7);
        applyFloatSynapseParameter(params.synapse, subName, value);
    } else if (paramName.find("plasticity_") == 0) {
        std::string subName = paramName.substr(11);
        applyFloatPlasticityParameter(params.plasticity, subName, value);
    } else if (paramName.find("structural_") == 0) {
        std::string subName = paramName.substr(11);
        applyFloatStructuralParameter(params.structural, subName, value);
    } else if (paramName.find("neuromodulation_") == 0) {
        std::string subName = paramName.substr(16);
        applyFloatNeuromodulationParameter(params.neuromodulation, subName, value);
    } else if (paramName.find("development_") == 0) {
        std::string subName = paramName.substr(11);
        applyFloatDevelopmentParameter(params.development, subName, value);
    }
}

void applyGenericIntParameter(NeuralParameters& params, const std::string& paramName, int64_t value) {
    // Integer parameter application
    if (paramName == "configVersion") params.configVersion = static_cast<int>(value);
}

} // namespace nlm
