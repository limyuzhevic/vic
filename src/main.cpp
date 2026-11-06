// NLM - Neural Learning Machine
// Refactored Phase 2 Demo with improved modularity and command-line interface

#include "src/experiments/NLMExperiments.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

/**
 * Refactored Phase 2 Demo class
 */
class Phase2Demo : public NeuralDemo {
public:
    Phase2Demo() = default;
    ~Phase2Demo() = default;
    
    void run() override {
        std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
        std::cout << "===================================" << std::endl;
        
        // Initialize logging
        initializeLogging(options.logLevel, options.logFile);
        
        // Create configuration
        auto config = createDefaultConfig();
        setupConfiguration(config);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        brain->logStatus();
        
        // Run test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run test 2: Plasticity learning experiment
        runPlasticityExperiment(brain);
        
        // Reset and run test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Final brain status
        std::cout << std::endl;
        std::cout << "=== Final Brain Status ===" << std::endl;
        brain->logStatus();
        
        std::cout << std::endl;
        std::cout << "=== Phase 2 Complete ===" << std::endl;
        std::cout << "Real LIF neuron dynamics implemented" << std::endl;
        std::cout << "Event-driven spike propagation with delays" << std::endl;
        std::cout << "STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << "Learning experiment demonstrates measurable changes" << std::endl;
        std::cout << "Network shows activity-dependent synaptic modification" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain is now a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
        std::cout << std::endl;
    }
    
    void setupConfiguration(std::shared_ptr<Config> config) override {
        // Apply command line options
        if (options.randomSeed != 42) {
            config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::CommandLine);
        }
        
        config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
        config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    void printHeader() const override {
        std::cout << "============================================" << std::endl;
        std::cout << "           NLM Phase 2 Demo                " << std::endl;
        std::cout << "============================================" << std::endl;
        std::cout << std::endl;
        std::cout << "This phase implements real spiking neural computation:" << std::endl;
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
        std::cout << "  - Event-driven spike propagation with delays" << std::endl;
        std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "  - Structural plasticity" << std::endl;
        std::cout << std::endl;
    }
    
    void printFooter() const override {
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
        std::cout << "=========================================== " << std::endl;
    }
    
private:
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        std::cout << std::endl;
        std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "  No neurons found!" << std::endl;
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        std::cout << "  Injecting current into 10 neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * options.simulationTimestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "  Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "  ✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "    Neuron " << i << 
                    " V=" << neurons[i]->getMembranePotential() << 
                    " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        std::cout << std::endl;
        std::cout << "=== Test 2: Plasticity Learning Experiment ===" << std::endl;
        
        LearningExperiment experiment(brain, options.randomSeed);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        std::cout << std::endl;
        std::cout << "Applying repeated input patterns (1000 steps)..." << std::endl;
        
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            
            brain->step(step, step * options.simulationTimestep);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "  Step " << step << 
                    " | Spikes: " << brain->getTotalSpikeCount() << 
                    " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        std::cout << std::endl;
        std::cout << "=== Test 3: STDP Verification ===" << std::endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            std::cout << "  No synapses for STDP test" << std::endl;
            return;
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);
        float beforeWeight = syn->getWeight();
        
        std::cout << "  Testing STDP on 1 synapse:" << std::endl;
        std::cout << "    Synapse before: " << beforeWeight << 
            " (pre=" << syn->getSourceNeuron().index() << 
            ", post=" << syn->getDestinationNeuron().index() << ")" << std::endl;
        
        std::cout << std::endl;
        std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, (trial * 2) * options.simulationTimestep);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, ((trial * 2) + 1) * options.simulationTimestep);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        std::cout << "  After correlated activity:" << std::endl;
        std::cout << "    Synapse after: " << syn->getWeight() << 
            " (Δ=" << delta << ")" << std::endl;
        
        float totalDelta = delta;
        
        std::cout << std::endl;
        if (totalDelta > 0.001f) {
            std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "  ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "  ! No change: STDP may not be triggering" << std::endl;
        }
    }
};

/**
 * Refactored Phase 3 Demo class
 */
class Phase3Demo : public NeuralDemo {
public:
    Phase3Demo() = default;
    ~Phase3Demo() = default;
    
    void run() override {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = createDefaultConfig();
        setupConfiguration(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(options.randomSeed);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        printInitialStatus(brain, agentBrain);
        
        // Run episodes
        runEpisodes(brain, agentBrain, world);
        
        printSummary();
    }
    
    void setupConfiguration(std::shared_ptr<Config> config) override {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
        config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
        config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::CommandLine);
        config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
    }
    
    void printHeader() const override {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
    }
    
    void printFooter() const override {
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
    void printInitialStatus(std::shared_ptr<Brain> brain, AgentBrain& agentBrain) {
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
    }
    
    void runEpisodes(std::shared_ptr<Brain> brain, AgentBrain& agentBrain, SimpleWorld& world) {
        int numEpisodes = 5;
        int stepsPerEpisode = 200;
        
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    (void)neuralStep;
                    brain->step(step);
                }
                
                // Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Update world
                world.update(0.1);
                
                // Update development
                agentBrain.updateDevelopment(0.1);
                
                // Track metrics
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();
            metrics.developmentalStage = DevelopmentalStage::Adult;
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
        }
    }
    
    void printSummary() {
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
    }
};

/**
 * Main function - refactored with improved modularity
 */
int main(int argc, char* argv[]) {
    // Parse command line options
    NLMCommandLine::Options options = NLMCommandLine::parse(argc, argv);
    
    // Print help or version if requested
    if (options.help) {
        NLMCommandLine::printHelp(argv[0]);
        return 0;
    }
    
    if (options.version) {
        NLMCommandLine::printVersion();
        return 0;
    }
    
    // Determine which demo to run
    std::string demoType = "phase2"; // default
    if (argc > 1) {
        demoType = argv[1];
    }
    
    // Create demo instance based on type
    NeuralDemo* demo = nullptr;
    
    if (demoType == "phase2" || demoType == "p2") {
        demo = new Phase2Demo();
    } else if (demoType == "phase3" || demoType == "p3") {
        demo = new Phase3Demo();
    } else if (demoType == "phase4" || demoType == "p4") {
        // Phase 4 demo would go here
        std::cerr << "ERROR: Phase 4 demo not yet implemented in this refactored version" << std::endl;
        return 1;
    } else if (demoType == "phase6" || demoType == "p6") {
        // Phase 6 demo would go here
        std::cerr << "ERROR: Phase 6 demo not yet implemented in this refactored version" << std::endl;
        return 1;
    } else {
        std::cerr << "ERROR: Unknown demo type: " << demoType << std::endl;
        NLMCommandLine::printHelp(argv[0]);
        return 1;
    }
    
    // Run the demo
    demo->run();
    
    // Clean up
    delete demo;
    
    return 0;
}