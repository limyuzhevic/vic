#include "Brain.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Factory pattern for creating brain systems
class BrainFactory {
public:
    // Create a complete brain with all systems integrated
    static std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    // Create brain with specific configuration parameters
    static std::shared_ptr<Brain> createBrain(
        const std::shared_ptr<Config>& config,
        size_t neuronCount,
        size_t regionCount,
        float connectionProbability,
        float simulationTimestep
    ) {
        // Create configuration
        auto brainConfig = std::make_shared<Config>(*config);
        
        // Set parameters
        brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
        brainConfig->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::User);
        brainConfig->set("connection_probability", connectionProbability, ConfigSource::User);
        brainConfig->set("simulation_timestep", simulationTimestep, ConfigSource::User);
        
        // Add default system parameters
        brainConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        brainConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        brainConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
        brainConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        brainConfig->set("pruning_rate", 0.00001f, ConfigSource::Default);
        brainConfig->set("replay_interval", 100, ConfigSource::Default);
        brainConfig->set("consolidation_interval", 1000, ConfigSource::Default);
        brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        return createBrain(brainConfig);
    }
};

// Configuration factory for validation
class ConfigFactory {
public:
    // Create configuration with validation
    static std::shared_ptr<Config> createConfig() {
        auto config = std::make_shared<Config>();
        
        // Set default configuration for Phase 2: Real Neural Computation
        config->set("app_name", "NLM", ConfigSource::Default);
        config->set("version", "2.0", ConfigSource::Default);
        
        // System parameters
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Memory parameters
        config->set("replay_interval", 100, ConfigSource::Default);
        config->set("consolidation_interval", 1000, ConfigSource::Default);
        
        // Random seed
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        return config;
    }
    
    // Create configuration from file
    static std::shared_ptr<Config> createConfigFromFile(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }
    
    // Validate configuration
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            return false;
        }
        
        // Check required keys
        const std::vector<std::string> requiredKeys = {"app_name", "version"};
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                return false;
            }
        }
        
        // Check numeric parameter ranges
        auto neuronCount = config->get<size_t>("neuron_count");
        if (neuronCount && (*neuronCount == 0 || *neuronCount > 1000000)) {
            return false;
        }
        
        return true;
    }
};

// Service locator pattern for brain systems
class BrainServiceLocator {
private:
    static std::shared_ptr<Brain> currentBrain;
    static std::shared_ptr<Config> currentConfig;
    
public:
    // Set the current brain instance
    static void setBrain(const std::shared_ptr<Brain>& brain) {
        currentBrain = brain;
    }
    
    // Get the current brain instance
    static std::shared_ptr<Brain> getBrain() {
        return currentBrain;
    }
    
    // Set the current configuration
    static void setConfig(const std::shared_ptr<Config>& config) {
        currentConfig = config;
    }
    
    // Get the current configuration
    static std::shared_ptr<Config> getConfig() {
        return currentConfig;
    }
    
    // Clear all services
    static void clear() {
        currentBrain.reset();
        currentConfig.reset();
    }
};

// Helper functions for common operations
class BrainUtils {
public:
    // Run a basic connectivity test
    static void runBasicConnectivityTest(const std::shared_ptr<Brain>& brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        }
    }
    
    // Run plasticity experiment
    static void runPlasticityExperiment(const std::shared_ptr<Brain>& brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        experiment.recordInitialState();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
    }
    
    // Run STDP verification
    static void runStdpVerification(const std::shared_ptr<Brain>& brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " after: " + std::to_string(synapses[i]->getWeight()) +
                       " (Δ=" + std::to_string(delta) + ")");
        }
        
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
};

} // namespace nlm
