// NLM Experiment Framework Implementation
// Provides experiment base classes, factory system, and registry

#include "experiments/NLMExperimentRunner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <filesystem>

namespace nlm {
    
    // Global experiment registry
    std::map<std::string, std::function<bool(const std::shared_ptr<Config>&)>> 
        ExperimentCLI::experimentRegistry;
    
    // ConfigurableExperiment implementation
    ConfigurableExperiment::ConfigurableExperiment(const std::string& name, 
                                                  const std::string& description)
        : name_(name), description_(description), currentCombination_(0) {}
    
    void ConfigurableExperiment::addParameter(const std::string& paramName,
                                             const std::vector<float>& values) {
        parameterSweeps_[paramName] = values;
        requiredParams_.push_back(paramName);
    }
    
    void ConfigurableExperiment::addParameterRange(const std::string& paramName,
                                                  float min, float max, float step) {
        std::vector<float> values;
        for (float val = min; val <= max + step * 0.5f; val += step) {
            values.push_back(val);
        }
        parameterSweeps_[paramName] = values;
        requiredParams_.push_back(paramName);
    }
    
    void ConfigurableExperiment::addFixedParameter(const std::string& paramName, float value) {
        fixedParameters_[paramName] = value;
        if (std::find(requiredParams_.begin(), requiredParams_.end(), paramName) == requiredParams_.end()) {
            requiredParams_.push_back(paramName);
        }
    }
    
    bool ConfigurableExperiment::validate(const std::shared_ptr<Config>& config) const {
        // Check all required parameters are present
        for (const auto& param : requiredParams_) {
            if (!config->has(param)) {
                NLM_LOG_ERROR("Missing required parameter: " + param);
                return false;
            }
        }
        
        // Check parameter ranges
        for (const auto& sweep : parameterSweeps_) {
            float value = config->getOr<float>(sweep.first, 0.0f);
            auto it = std::find(sweep.second.begin(), sweep.second.end(), value);
            if (it == sweep.second.end()) {
                NLM_LOG_ERROR("Invalid value for parameter " + sweep.first + ": " + 
                            std::to_string(value) + ". Valid values: ");
                for (size_t i = 0; i < sweep.second.size() && i < 5; ++i) {
                    NLM_LOG_ERROR("  " + std::to_string(sweep.second[i]));
                }
                if (sweep.second.size() > 5) {
                    NLM_LOG_ERROR("  ... and " + std::to_string(sweep.second.size() - 5) + " more");
                }
                return false;
            }
        }
        
        return true;
    }
    
    bool ConfigurableExperiment::run(const std::shared_ptr<Config>& config) {
        // Generate all parameter combinations
        std::vector<std::map<std::string, float>> combinations;
        generateParameterCombinations(combinations);
        
        if (currentCombination_ >= combinations.size()) {
            NLM_LOG_ERROR("Invalid combination index: " + std::to_string(currentCombination_));
            return false;
        }
        
        currentParams_ = combinations[currentCombination_];
        
        // Apply parameters to config
        for (const auto& param : currentParams_) {
            config->set(param.first, param.second, ConfigSource::Runtime);
        }
        
        // Run the experiment (to be implemented by subclasses)
        return runWithConfig(config);
    }
    
    void ConfigurableExperiment::generateParameterCombinations(
        std::vector<std::map<std::string, float>>& combinations) {
        
        combinations.clear();
        
        // Base case: no more parameters to vary
        if (parameterSweeps_.empty()) {
            combinations.push_back(fixedParameters_);
            return;
        }
        
        // Take the first parameter to vary
        auto it = parameterSweeps_.begin();
        std::string paramName = it->first;
        std::vector<float> values = it->second;
        parameterSweeps_.erase(it);
        
        // Recursively generate combinations
        std::vector<std::map<std::string, float>> subCombinations;
        generateParameterCombinations(subCombinations);
        
        // Combine with current parameter values
        for (const auto& value : values) {
            for (const auto& subCombo : subCombinations) {
                std::map<std::string, float> combo = subCombo;
                combo[paramName] = value;
                
                // Merge with fixed parameters
                for (const auto& fixed : fixedParameters_) {
                    combo[fixed.first] = fixed.second;
                }
                
                combinations.push_back(combo);
            }
        }
        
        // Restore the parameter
        parameterSweeps_[paramName] = values;
    }
    
    // Built-in experiment implementations
    bool BasicConnectivityExperiment::validate(const std::shared_ptr<Config>& config) const {
        if (!config->has("neuron_count")) {
            NLM_LOG_ERROR("Missing neuron_count parameter");
            return false;
        }
        if (!config->has("injection_current")) {
            NLM_LOG_ERROR("Missing injection_current parameter");
            return false;
        }
        if (!config->has("steps")) {
            NLM_LOG_ERROR("Missing steps parameter");
            return false;
        }
        
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 100);
        float current = config->getOr<float>("injection_current", 10.0f);
        int64_t steps = config->getOr<int64_t>("steps", 1000);
        
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("neuron_count must be positive");
            return false;
        }
        if (current < 0.0f) {
            NLM_LOG_ERROR("injection_current must be non-negative");
            return false;
        }
        if (steps <= 0) {
            NLM_LOG_ERROR("steps must be positive");
            return false;
        }
        
        return true;
    }
    
    bool BasicConnectivityExperiment::run(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Running Basic Connectivity Experiment");
        NLM_LOG_INFO("Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 100)));
        NLM_LOG_INFO("Injection current: " + std::to_string(config->getOr<float>("injection_current", 10.0f)));
        NLM_LOG_INFO("Steps: " + std::to_string(config->getOr<int64_t>("steps", 1000)));
        
        // Create brain with configuration
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Record initial state
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found");
            return false;
        }
        
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into sensory neurons
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 100);
        float injectionCurrent = config->getOr<float>("injection_current", 10.0f);
        
        for (size_t i = 0; i < std::min<size_t>(10, brain->getTotalNeuronCount()); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, injectionCurrent);
        }
        
        // Run simulation
        int64_t steps = config->getOr<int64_t>("steps", 1000);
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        size_t finalSpikes = brain->getTotalSpikeCount() - initialSpikes;
        size_t finalSynapseCount = brain->getTotalSynapseCount();
        
        // Calculate statistics
        results.metrics["initial_spikes"] = static_cast<double>(initialSpikes);
        results.metrics["final_spikes"] = static_cast<double>(finalSpikes);
        results.metrics["total_spikes_generated"] = static_cast<double>(finalSpikes);
        results.metrics["synapse_count_initial"] = static_cast<double>(initialSynapseCount);
        results.metrics["synapse_count_final"] = static_cast<double>(finalSynapseCount);
        results.metrics["synapse_change"] = static_cast<double>(finalSynapseCount - initialSynapseCount);
        results.metrics["success_rate"] = finalSpikes > 0 ? 1.0 : 0.0;
        
        NLM_LOG_INFO("Basic connectivity experiment completed");
        NLM_LOG_INFO("Spikes generated: " + std::to_string(finalSpikes));
        NLM_LOG_INFO("Synapse count change: " + std::to_string(finalSynapseCount - initialSynapseCount));
        
        return true;
    }
    
    bool PlasticityExperiment::validate(const std::shared_ptr<Config>& config) const {
        if (!config->has("neuron_count")) {
            NLM_LOG_ERROR("Missing neuron_count parameter");
            return false;
        }
        if (!config->has("hebbian_rate")) {
            NLM_LOG_ERROR("Missing hebbian_rate parameter");
            return false;
        }
        if (!config->has("stdp_rate")) {
            NLM_LOG_ERROR("Missing stdp_rate parameter");
            return false;
        }
        if (!config->has("steps")) {
            NLM_LOG_ERROR("Missing steps parameter");
            return false;
        }
        
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 100);
        float hebbianRate = config->getOr<float>("hebbian_rate", 0.01f);
        float stdpRate = config->getOr<float>("stdp_rate", 0.015f);
        int64_t steps = config->getOr<int64_t>("steps", 1000);
        
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("neuron_count must be positive");
            return false;
        }
        if (hebbianRate < 0.0f || hebbianRate > 1.0f) {
            NLM_LOG_ERROR("hebbian_rate must be between 0.0 and 1.0");
            return false;
        }
        if (stdpRate < 0.0f || stdpRate > 1.0f) {
            NLM_LOG_ERROR("stdp_rate must be between 0.0 and 1.0");
            return false;
        }
        if (steps <= 0) {
            NLM_LOG_ERROR("steps must be positive");
            return false;
        }
        
        return true;
    }
    
    bool PlasticityExperiment::run(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Running Plasticity Learning Experiment");
        NLM_LOG_INFO("Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 100)));
        NLM_LOG_INFO("Hebbian rate: " + std::to_string(config->getOr<float>("hebbian_rate", 0.01f)));
        NLM_LOG_INFO("STDP rate: " + std::to_string(config->getOr<float>("stdp_rate", 0.015f)));
        NLM_LOG_INFO("Steps: " + std::to_string(config->getOr<int64_t>("steps", 1000)));
        
        // Create brain with configuration
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Enable plasticity
        auto region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        float initialMeanWeight = 0.0f;
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            initialMeanWeight = sum / initialWeights.size();
        }
        
        // Run learning simulation
        int64_t steps = config->getOr<int64_t>("steps", 1000);
        int64_t hebbianRate = static_cast<int64_t>(config->getOr<float>("hebbian_rate", 0.01f) * 1000);
        int64_t stdpRate = static_cast<int64_t>(config->getOr<float>("stdp_rate", 0.015f) * 1000);
        
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
        
        for (SimulationStep step = 0; step < steps; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        float finalMeanWeight = 0.0f;
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            finalMeanWeight = sum / finalWeights.size();
        }
        
        // Calculate plasticity statistics
        results.metrics["initial_mean_weight"] = static_cast<double>(initialMeanWeight);
        results.metrics["final_mean_weight"] = static_cast<double>(finalMeanWeight);
        results.metrics["weight_change"] = static_cast<double>(finalMeanWeight - initialMeanWeight);
        results.metrics["weight_change_percent"] = static_cast<double>((finalMeanWeight - initialMeanWeight) / initialMeanWeight * 100.0f);
        
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        results.metrics["strengthened_synapses"] = static_cast<double>(strengthened);
        results.metrics["weakened_synapses"] = static_cast<double>(weakened);
        results.metrics["unchanged_synapses"] = static_cast<double>(unchanged);
        
        results.metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        results.metrics["firing_neuron_count"] = static_cast<double>(brain->getFiringNeuronCount());
        
        // Determine if learning occurred
        bool learningOccurred = std::abs(finalMeanWeight - initialMeanWeight) > 0.001f ||
                               (strengthened > 0 || weakened > 0);
        
        results.metrics["learning_detected"] = learningOccurred ? 1.0 : 0.0;
        
        NLM_LOG_INFO("Plasticity experiment completed");
        NLM_LOG_INFO("Initial mean weight: " + std::to_string(initialMeanWeight));
        NLM_LOG_INFO("Final mean weight: " + std::to_string(finalMeanWeight));
        NLM_LOG_INFO("Change: " + std::to_string(finalMeanWeight - initialMeanWeight));
        NLM_LOG_INFO("Learning detected: " + std::string(learningOccurred ? "YES" : "NO"));
        
        return true;
    }
    
    bool STDPVerificationExperiment::validate(const std::shared_ptr<Config>& config) const {
        if (!config->has("neuron_count")) {
            NLM_LOG_ERROR("Missing neuron_count parameter");
            return false;
        }
        if (!config->has("pre_post_trials")) {
            NLM_LOG_ERROR("Missing pre_post_trials parameter");
            return false;
        }
        if (!config->has("weight_threshold")) {
            NLM_LOG_ERROR("Missing weight_threshold parameter");
            return false;
        }
        
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 100);
        int64_t trials = config->getOr<int64_t>("pre_post_trials", 50);
        float threshold = config->getOr<float>("weight_threshold", 0.001f);
        
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("neuron_count must be positive");
            return false;
        }
        if (trials <= 0) {
            NLM_LOG_ERROR("pre_post_trials must be positive");
            return false;
        }
        if (threshold < 0.0f) {
            NLM_LOG_ERROR("weight_threshold must be non-negative");
            return false;
        }
        
        return true;
    }
    
    bool STDPVerificationExperiment::run(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Running STDP Verification Experiment");
        NLM_LOG_INFO("Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 100)));
        NLM_LOG_INFO("Pre-post trials: " + std::to_string(config->getOr<int64_t>("pre_post_trials", 50)));
        NLM_LOG_INFO("Weight threshold: " + std::to_string(config->getOr<float>("weight_threshold", 0.001f)));
        
        // Create brain with configuration
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                       " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
        int64_t trials = config->getOr<int64_t>("pre_post_trials", 50);
        float weightThreshold = config->getOr<float>("weight_threshold", 0.001f);
        
        for (int trial = 0; trial < trials; ++trial) {
            // Get neurons for pre and post synaptic actions
            auto neurons = region->getAllNeurons();
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                       " after: " + std::to_string(synapses[i]->getWeight()) +
                       " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check results
        results.metrics["stdp_potentiation"] = totalDelta > 0.0f ? 1.0 : 0.0;
        results.metrics["total_delta"] = static_cast<double>(totalDelta);
        results.metrics["max_delta"] = std::abs(totalDelta);
        
        NLM_LOG_INFO("");
        if (totalDelta > weightThreshold) {
            NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
            results.metrics["stdp_success"] = 1.0;
        } else if (totalDelta < -weightThreshold) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            results.metrics["stdp_success"] = 0.0;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            results.metrics["stdp_success"] = 0.0;
        }
        
        return true;
    }
    
    bool ScalingExperiment::run(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Running Scaling Experiment");
        NLM_LOG_INFO("Testing performance scaling with different network sizes");
        
        // Test with different neuron counts
        std::vector<int64_t> neuronCounts = {100, 500, 1000, 2000};
        std::vector<double> executionTimes;
        std::vector<size_t> memoryUsages;
        
        for (int64_t neuronCount : neuronCounts) {
            NLM_LOG_INFO("Testing with " + std::to_string(neuronCount) + " neurons...");
            
            // Update config
            config->set("neuron_count", neuronCount, ConfigSource::Runtime);
            
            // Create and run brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain with " + std::to_string(neuronCount) + " neurons");
                return false;
            }
            
            // Record start time
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run basic connectivity test
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            for (size_t i = 0; i < std::min<size_t>(10, brain->getTotalNeuronCount()); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            }
            
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            
            // Record end time
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            executionTimes.push_back(duration.count());
            memoryUsages.push_back(brain->getTotalNeuronCount() * sizeof(Neuron));
            
            NLM_LOG_INFO("  Execution time: " + std::to_string(duration.count()) + "ms");
            NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        }
        
        // Calculate scaling statistics
        if (executionTimes.size() >= 2) {
            double totalTime = 0.0;
            size_t totalMemory = 0;
            
            for (size_t i = 0; i < executionTimes.size(); ++i) {
                totalTime += executionTimes[i];
                totalMemory += memoryUsages[i];
            }
            
            results.metrics["scaling_efficiency"] = executionTimes.size() > 1 ? 
                1.0 / (executionTimes.back() / executionTimes.front()) : 1.0;
            results.metrics["average_execution_time_ms"] = totalTime / executionTimes.size();
            results.metrics["average_memory_usage_kb"] = totalMemory / 1024 / executionTimes.size();
            results.metrics["scaling_factor_neuron_count"] = 
                static_cast<double>(neuronCounts.back()) / neuronCounts.front();
        }
        
        NLM_LOG_INFO("Scaling experiment completed");
        return true;
    }

} // namespace nlm
