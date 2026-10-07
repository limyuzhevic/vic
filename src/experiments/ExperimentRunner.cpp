#include "ExperimentRunner.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>

namespace nlm {

struct ExperimentRunner::Impl {
    std::unordered_map<std::string, CustomExperimentFunction> customExperiments;
    std::function<void(int, int, const std::string&)> progressCallback;
    double defaultProgressValue;
    
    Impl() : defaultProgressValue(0.0) {
        // Register built-in experiments
        registerBuiltInExperiments();
    }
    
    void registerBuiltInExperiments() {
        // Basic Connectivity Test
        customExperiments["basic_connectivity"] = 
            [this](std::shared_ptr<Brain> brain, std::shared_ptr<Environment> env, const Config& config) -> bool {
                return runBasicConnectivityTestInternal(brain, config);
            };
        
        // Plasticity Experiment
        customExperiments["plasticity"] = 
            [this](std::shared_ptr<Brain> brain, std::shared_ptr<Environment> env, const Config& config) -> bool {
                return runPlasticityExperimentInternal(brain, config);
            };
        
        // STDP Verification
        customExperiments["stdp_verification"] = 
            [this](std::shared_ptr<Brain> brain, std::shared_ptr<Environment> env, const Config& config) -> bool {
                return runStdpVerificationInternal(brain, config);
            };
    }
    
    bool runBasicConnectivityTestInternal(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            return true;
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            return false;
        }
    }
    
    bool runPlasticityExperimentInternal(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
        return true;
    }
    
    bool runStdpVerificationInternal(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
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
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            return false;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            return false;
        }
    }
};

ExperimentRunner::ExperimentRunner() : pImpl(std::make_unique<Impl>()) {}

ExperimentRunner::~ExperimentRunner() = default;

ExperimentRunner::ExperimentRunner(ExperimentRunner&&) noexcept = default;

ExperimentRunner& ExperimentRunner::operator=(ExperimentRunner&&) noexcept = default;

bool ExperimentRunner::runBasicConnectivityTest(std::shared_ptr<Brain> brain, const Config& config) {
    auto startTime = std::chrono::steady_clock::now();
    
    bool success = pImpl->runBasicConnectivityTestInternal(brain, config);
    
    double elapsed = getElapsedTime(startTime);
    NLM_LOG_INFO("Basic connectivity test completed in " + std::to_string(elapsed) + "s");
    
    return success;
}

bool ExperimentRunner::runPlasticityExperiment(std::shared_ptr<Brain> brain, const Config& config) {
    auto startTime = std::chrono::steady_clock::now();
    
    bool success = pImpl->runPlasticityExperimentInternal(brain, config);
    
    double elapsed = getElapsedTime(startTime);
    NLM_LOG_INFO("Plasticity experiment completed in " + std::to_string(elapsed) + "s");
    
    return success;
}

bool ExperimentRunner::runStdpVerification(std::shared_ptr<Brain> brain, const Config& config) {
    auto startTime = std::chrono::steady_clock::now();
    
    bool success = pImpl->runStdpVerificationInternal(brain, config);
    
    double elapsed = getElapsedTime(startTime);
    NLM_LOG_INFO("STDP verification completed in " + std::to_string(elapsed) + "s");
    
    return success;
}

bool ExperimentRunner::runCustomExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                                         const Config& config, CustomExperimentFunction experimentFunc) {
    if (!experimentFunc) {
        NLM_LOG_ERROR("No experiment function provided");
        return false;
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Running custom experiment...");
    bool success = experimentFunc(brain, environment, config);
    
    double elapsed = getElapsedTime(startTime);
    NLM_LOG_INFO("Custom experiment completed in " + std::to_string(elapsed) + "s");
    
    return success;
}

void ExperimentRunner::registerExperiment(const std::string& name, CustomExperimentFunction func) {
    if (func) {
        pImpl->customExperiments[name] = std::move(func);
        NLM_LOG_INFO("Registered custom experiment: " + name);
    }
}

bool ExperimentRunner::runExperiment(const std::string& name, std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<Environment> environment, const Config& config) {
    auto it = pImpl->customExperiments.find(name);
    if (it == pImpl->customExperiments.end()) {
        NLM_LOG_ERROR("Experiment not found: " + name);
        return false;
    }
    
    return runCustomExperiment(brain, environment, config, it->second);
}

void ExperimentRunner::resetBrain(std::shared_ptr<Brain> brain) const {
    if (brain) {
        brain->reset();
        brain->initialize();
    }
}

void ExperimentRunner::logExperimentResult(const std::string& name, const ExperimentResult& result) const {
    NLM_LOG_INFO("Experiment: " + name);
    NLM_LOG_INFO("  Status: " + (result.success ? "SUCCESS" : "FAILED"));
    NLM_LOG_INFO("  Time: " + std::to_string(result.executionTime.count()) + "s");
    if (!result.output.empty()) {
        NLM_LOG_INFO("  Output: " + result.output);
    }
    
    for (const auto& [key, value] : result.metrics) {
        NLM_LOG_INFO("  Metric " + key + ": " + std::to_string(value));
    }
}

bool ExperimentRunner::validateExperimentConfig(const Config& config) {
    // Basic validation
    if (config.has("neuron_count")) {
        auto count = config.getOr<int64_t>("neuron_count", 500);
        if (count < 10) {
            NLM_LOG_WARNING("Small neuron count may affect experiment validity");
        }
    }
    
    if (config.has("simulation_timestep")) {
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 0.1) {
            NLM_LOG_WARNING("Unusual simulation timestep may affect stability");
        }
    }
    
    return true;
}

std::string ExperimentRunner::getExperimentInfo(const std::string& name) const {
    auto it = pImpl->customExperiments.find(name);
    if (it == pImpl->customExperiments.end()) {
        return "Experiment not found: " + name;
    }
    
    std::ostringstream oss;
    oss << "Experiment: " << name << std::endl;
    oss << "  Type: Custom experiment" << std::endl;
    oss << "  Description: User-defined experiment" << std::endl;
    
    return oss.str();
}

void ExperimentRunner::setProgressCallback(std::function<void(int, int, const std::string&)> callback) {
    pImpl->progressCallback = std::move(callback);
}

double ExperimentRunner::getElapsedTime(std::chrono::steady_clock::time_point start) const {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

void ExperimentRunner::logProgress(int current, int total, const std::string& message) const {
    if (pImpl->progressCallback) {
        pImpl->progressCallback(current, total, message);
    }
}

} // namespace nlm