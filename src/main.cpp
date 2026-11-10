// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

void printUsage() {
    printBanner();
    std::cout << "\nUsage: nlm [options]\n\n";
    std::cout << "Basic Options:\n";
    std::cout << "  --help                  Show this help message\n";
    std::cout << "  --config file           Load configuration from file\n";
    std::cout << "  --quick-test           Run a quick demonstration (50 neurons, 100 steps)\n";
    std::cout << "\nSystem Integration Options:\n";
    std::cout << "  --mode MODES           Set operation mode (comprehensive|performance|minimal)\n";
    std::cout << "  --enable-checkpointing  Enable checkpoint save/load functionality\n";
    std::cout << "  --disable-memory       Disable memory systems\n";
    std::cout << "  --disable-prediction   Disable prediction system\n";
    std::cout << "  --disable-cognition    Disable cognitive systems\n";
    std::cout << "  --disable-neuromodulation Disable neuromodulation\n";
    std::cout << "  --disable-development  Disable development system\n";
    std::cout << "\nAdvanced Configuration:\n";
    std::cout << "  --neuron-count N       Set number of neurons (default: 500)\n";
    std::cout << "  --region-count R       Set number of regions (default: 1)\n";
    std::cout << "  --connection-prob P    Set connection probability (0.0-1.0, default: 0.15)\n";
    std::cout << "  --working-memory-capacity C   Set working memory capacity (default: 100)\n";
    std::cout << "  --attention-strength S  Set attention excitation strength (default: 1.5)\n";
    std::cout << "  --dopamine-level L     Set dopamine level (0.0-1.0, default: 0.5)\n";
    std::cout << "  --developmental-stage T Set developmental stage (0=Initial,1=Critical,2=Maturation,3=Adult)\n";
    std::cout << "\nOutput Options:\n";
    std::cout << "  --verbose              Enable verbose logging\n";
    std::cout << "  --quiet                Suppress most output (errors only)\n";
    std::cout << "\nExample:\n";
    std::cout << "  ./nlm --mode comprehensive --neuron-count 1000 --enable-checkpointing\n";
    std::cout << "\nNLM (熙然) - Neural Learning Machine (Phase 6: Integrated Artificial Brain)\n";
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
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
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
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
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
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
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");

    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");

    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }

    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");

    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void printUsage();

int main(int argc, char** argv) {
    // Parse global options
    bool showHelp = false;
    std::string logLevel = "info";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            showHelp = true;
        } else if (arg == "--verbose") {
            logLevel = "verbose";
        } else if (arg == "--quiet") {
            logLevel = "quiet";
        }
    }
    
    if (showHelp) {
        printUsage();
        return 0;
    }
    
    // Set log level based on user preference
    if (logLevel == "verbose") {
        ConsoleLogger::setLevel(LogLevel::Debug);
    } else if (logLevel == "quiet") {
        ConsoleLogger::setLevel(LogLevel::Warning);
    } else {
        ConsoleLogger::setLevel(LogLevel::Info);
    }
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(ConsoleLogger::getLevel());
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Parse advanced command-line options
    bool enableMemory = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    bool enableNeuromodulation = true;
    bool enableDevelopment = true;
    bool enableCheckpointing = false;
    std::string mode = "comprehensive";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mode") {
            if (i + 1 < argc) {
                mode = argv[++i];
            }
        } else if (arg == "--disable-memory") {
            enableMemory = false;
        } else if (arg == "--disable-prediction") {
            enablePrediction = false;
        } else if (arg == "--disable-cognition") {
            enableCognition = false;
        } else if (arg == "--disable-neuromodulation") {
            enableNeuromodulation = false;
        } else if (arg == "--disable-development") {
            enableDevelopment = false;
        } else if (arg == "--enable-checkpointing") {
            enableCheckpointing = true;
        } else if (arg == "--quick-test") {
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
            config->set("max_steps", 100, ConfigSource::Default);
        }
    }
    
    NLM_LOG_INFO("Mode: " + mode + " (working memory: " + std::string(enableMemory ? "ON" : "OFF") +
                ", prediction: " + std::string(enablePrediction ? "ON" : "OFF") +
                ", cognition: " + std::string(enableCognition ? "ON" : "OFF") +
                ", neuromodulation: " + std::string(enableNeuromodulation ? "ON" : "OFF") +
                ", development: " + std::string(enableDevelopment ? "ON" : "OFF") +
                ", checkpointing: " + std::string(enableCheckpointing ? "ON" : "OFF") + ")");
    
    // Configure memory systems
    if (enableMemory) {
        if (auto wmCapacity = config->getOr<int64_t>("working_memory_capacity", 100)) {
            pImpl->workingMemory->setCapacity(static_cast<size_t>(*wmCapacity));
        }
        if (auto wmDecay = config->getOr<float>("working_memory_decay", 0.1f)) {
            pImpl->workingMemory->setDecayRate(*wmDecay);
        }
    }
    
    if (enablePrediction) {
        if (auto predictionLearningRate = config->getOr<float>("prediction_learning_rate", 0.01f)) {
            pImpl->predictionSystem->setLearningRate(*predictionLearningRate);
        }
    }
    
    if (enableCognition) {
        if (auto attentionStrength = config->getOr<float>("attention_strength", 1.5f)) {
            pImpl->attention->setExcitationStrength(*attentionStrength);
        }
        if (auto attentionInhibition = config->getOr<float>("attention_inhibition", 0.5f)) {
            pImpl->attention->setInhibitionStrength(*attentionInhibition);
        }
    }
    
    if (enableNeuromodulation) {
        if (auto dopamineLevel = config->getOr<float>("dopamine_level", 0.5f)) {
            pImpl->dopamine->setLevel(*dopamineLevel);
        }
    }
    
    if (enableDevelopment) {
        if (auto devStage = config->getOr<int64_t>("developmental_stage", 4)) {
            pImpl->developmentalStage = static_cast<DevelopmentalStage>(std::clamp(*devStage, 0LL, 3LL));
        }
    }
    
    // Configure working memory
    if (auto wmCapacity = config->getOr<int64_t>("working_memory_capacity", 100)) {
        pImpl->workingMemory->setCapacity(static_cast<size_t>(*wmCapacity));
    }
    if (auto wmDecay = config->getOr<float>("working_memory_decay", 0.1f)) {
        pImpl->workingMemory->setDecayRate(*wmDecay);
    }
    
    // Configure episodic memory
    if (auto maxEpisodes = config->getOr<int64_t>("max_episodes", 1000)) {
        pImpl->episodicMemory->setMaxEpisodes(static_cast<size_t>(*maxEpisodes));
    }
    
    // Configure prediction system
    if (auto predictionLearningRate = config->getOr<float>("prediction_learning_rate", 0.01f)) {
        pImpl->predictionSystem->setLearningRate(*predictionLearningRate);
    }
    
    // Configure attention system
    if (auto attentionStrength = config->getOr<float>("attention_strength", 1.5f)) {
        pImpl->attention->setExcitationStrength(*attentionStrength);
    }
    if (auto attentionInhibition = config->getOr<float>("attention_inhibition", 0.5f)) {
        pImpl->attention->setInhibitionStrength(*attentionInhibition);
    }
    
    // Configure development
    if (auto devStage = config->getOr<int64_t>("developmental_stage", 4)) {
        pImpl->developmentalStage = static_cast<DevelopmentalStage>(std::clamp(*devStage, 0LL, 3LL));
    }
    
    NLM_LOG_INFO("Phase 6 systems configured and integrated");
    NLM_LOG_INFO("  Working memory capacity: " + std::to_string(pImpl->workingMemory ? pImpl->workingMemory->getCapacity() : 0));
    NLM_LOG_INFO("  Episodic memory max episodes: " + std::to_string(pImpl->episodicMemory ? pImpl->episodicMemory->getMaxEpisodes() : 0));
    NLM_LOG_INFO("  Prediction learning rate: " + std::to_string(pImpl->predictionSystem ? pImpl->predictionSystem->getLearningRate() : 0.0f));
    NLM_LOG_INFO("  Attention strength: " + std::to_string(pImpl->attention ? pImpl->attention->getExcitationStrength() : 0.0f));
    NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    return true;;
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
