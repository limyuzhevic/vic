#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() {
    delete pImpl;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



            }
            
            set(key, value, ConfigSource::File);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



        }

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    
    return true;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



            }

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



        }

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    return true;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    
    return true;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    return keys;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

void Config::clear() {
    pImpl->entries.clear();

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



    }
    return oss.str();

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;

// Implementation of convenience methods


define void Config::setRestingPotential(float value) { set(nlm::ConfigKeys::NEURON_RESTING_POTENTIAL, value); }

define void Config::setThreshold(float value) { set(nlm::ConfigKeys::NEURON_THRESHOLD, value); }

define void Config::setTimeStep(float value) { set(nlm::ConfigKeys::NEURON_TIME_STEP, value); }

define void Config::setTau(float value) { set(nlm::ConfigKeys::NEURON_TAU, value); }

define void Config::setAdaptationFactor(float value) { set(nlm::ConfigKeys::NEURON_ADAPTATION_FACTOR, value); }

define void Config::setLtpWeight(float value) { set(nlm::ConfigKeys::STDP_LTP_WEIGHT, value); }

define void Config::setLtdWeight(float value) { set(nlm::ConfigKeys::STDP_LTD_WEIGHT, value); }

define void Config::setWeightScalingPos(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_POS, value); }

define void Config::setWeightScalingNeg(float value) { set(nlm::ConfigKeys::WEIGHT_SCALING_NEG, value); }

define void Config::setSynaptogenesisRate(float value) { set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, value); }

define void Config::setPruningRate(float value) { set(nlm::ConfigKeys::PRUNING_RATE, value); }

define void Config::setPlanningDepth(int value) { set(nlm::ConfigKeys::PLANNING_DEPTH, value); }

define void Config::setIntervalFast(int value) { set(nlm::ConfigKeys::INTERVAL_FAST, value); }

define void Config::setIntervalSlow(int value) { set(nlm::ConfigKeys::INTERVAL_SLOW, value); }

define void Config::setAttentionStrengthPos(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_POS, value); }

define void Config::setAttentionStrengthNeg(float value) { set(nlm::ConfigKeys::ATTENTION_STRENGTH_NEG, value); }

define void Config::setMaxSteps(int value) { set(nlm::ConfigKeys::MAX_STEPS, value); }

define void Config::setPlasticityMod1(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_1, value); }

define void Config::setPlasticityMod2(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_2, value); }

define void Config::setPlasticityMod3(float value) { set(nlm::ConfigKeys::PLASTICITY_MOD_3, value); }

define void Config::setDopamineBaseline(float value) { set(nlm::ConfigKeys::DOPAMINE_BASELINE, value); }

define void Config::setCuriosityStrength(float value) { set(nlm::ConfigKeys::CURIOUSITY_STRENGTH, value); }

define void Config::setNoveltyThreshold(float value) { set(nlm::ConfigKeys::NOVELTY_THRESHOLD, value); }



}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
