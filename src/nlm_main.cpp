// NLM Command Line Interface
// Provides easy access to different NLM demos and functionality

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace nlm;
using namespace std;

// Demo function types
declare function<void()> DemoFunction;

// Forward declarations for demo functions
void runPhase2Demo(Config& config);
void runPhase6Demo(Config& config);
void runBenchmarkDemo(Config& config);
void runLearningWorldDemo(Config& config);
void runPlasticityDemo(Config& config);
void runMemoryDemo(Config& config);
void runPredictionDemo(Config& config);
void runDevelopmentDemo(Config& config);
void runStdpDemo(Config& config);
void runHebbianDemo(Config& config);
void runComprehensiveDemo(Config& config);

// Available demos map
map<string, pair<string, function<void(Config&)>>> demos = {
    {"phase2", {"Phase 2: Real Neural Computation", runPhase2Demo}},
    {"phase6", {"Phase 6: Final Integration", runPhase6Demo}},
    {"benchmark", {"Performance Benchmark", runBenchmarkDemo}},
    {"learning", {"Learning in World", runLearningWorldDemo}},
    {"plasticity", {"Plasticity Learning", runPlasticityDemo}},
    {"memory", {"Memory Integration", runMemoryDemo}},
    {"prediction", {"Prediction & Planning", runPredictionDemo}},
    {"development", {"Development Process", runDevelopmentDemo}},
    {"stdp", {"STDP Verification", runStdpDemo}},
    {"hebbian", {"Hebbian Learning", runHebbianDemo}},
    {"comprehensive", {"Comprehensive Demo Suite", runComprehensiveDemo}}
};

// Global configuration
shared_ptr<Config> globalConfig;

// Print banner
void printBanner() {
    cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Command Line Interface                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << endl;
}

// Print help information
void printHelp() {
    printBanner();
    
    cout << "\nUsage: nlm [OPTIONS] [DEMO]" << endl;
    cout << "\nAvailable Demos:" << endl;
    for (const auto& demo : demos) {
        cout << "  " << left << setw(15) << demo.first << " - " << demo.second.first << endl;
    }
    
    cout << "\nOptions:" << endl;
    cout << "  --help, -h                    Show this help message" << endl;
    cout << "  --config=file                Load configuration from file" << endl;
    cout << "  --neuron-count=N             Set number of neurons" << endl;
    cout << "  --region-count=N             Set number of regions" << endl;
    cout << "  --random-seed=N              Set random seed for reproducibility" << endl;
    cout << "  --simulation-timestep=N      Set simulation timestep" << endl;
    cout << "  --log-level=L                Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << endl;
    cout << "  --log-to-file                 Enable logging to file" << endl;
    cout << "  --log-file=FILE              Set log filename" << endl;
    cout << "  --checkpoint-dir=DIR         Set checkpoint directory" << endl;
    cout << "  --disable-plasticities       Disable all plasticity systems" << endl;
    cout << "  --enable-plasticities        Enable all plasticity systems" << endl;
    cout << "  --enable-curiosity           Enable curiosity system" << endl;
    cout << "  --enable-novelty             Enable novelty detection" << endl;
    cout << "  --enable-reward-modulation   Enable reward modulation" << endl;
    cout << "  --enable-development         Enable development system" << endl;
    cout << "  --version                    Show version information" << endl;
    cout << "  --list-demos                 List all available demos" << endl;
    
    cout << "\nExample Usage:" << endl;
    cout << "  nlm --neuron-count=3000 --region-count=4 learning" << endl;
    cout << "  nlm --config=myconfig.cfg --log-to-file phase6" << endl;
    cout << "  nlm --enable-plasticities --enable-curiosity plasticity" << endl;
}

// Print version information
void printVersion() {
    cout << "NLM Neural Learning Machine - Version 0.1.0" << endl;
    cout << "Phase 6: Final Integration (Complete)" << endl;
    cout << "Built on " << __DATE__ << " at " << __TIME__ << endl;
}

// Print available demos
void printAvailableDemos() {
    cout << "\nAvailable NLM Demos:" << endl;
    cout << "===================" << endl;
    for (const auto& demo : demos) {
        cout << "  " << left << setw(15) << demo.first << " - " << demo.second.first << endl;
    }
    cout << "\nTo run a demo: nlm [OPTIONS] <demo_name>" << endl;
    cout << "Example: nlm --neuron-count=2000 learning" << endl;
}

// Parse command line arguments and apply to config
void parseCommandLineArgs(int argc, char** argv) {
    if (globalConfig == nullptr) {
        globalConfig = make_shared<Config>();
    }
    
    for (int i = 1; i < argc; ++i) {
        string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        else if (arg == "--version") {
            printVersion();
            exit(0);
        }
        else if (arg == "--list-demos") {
            printAvailableDemos();
            exit(0);
        }
        else if (arg.substr(0, 7) == "--config") {
            string configFile = "configs/default.cfg"; // default
            if (arg.find('=') != string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            
            if (!globalConfig->loadFromFile(configFile)) {
                cout << "Warning: Could not load config file: " << configFile << endl;
                cout << "Using default configuration." << endl;
            }
        }
        else if (arg.substr(0, 17) == "--neuron-count=") {
            string value = arg.substr(17);
            globalConfig->set("neuron_count", stoi(value), ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 15) == "--region-count=") {
            string value = arg.substr(15);
            globalConfig->set("region_count", stoi(value), ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 15) == "--random-seed=") {
            string value = arg.substr(15);
            globalConfig->set("random_seed", stoll(value), ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 20) == "--simulation-timestep=") {
            string value = arg.substr(20);
            globalConfig->set("simulation_timestep", stod(value), ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 15) == "--log-level=") {
            string value = arg.substr(15);
            transform(value.begin(), value.end(), value.begin(), ::toupper);
            LogLevel level;
            if (value == "DEBUG") level = LogLevel::Debug;
            else if (value == "INFO") level = LogLevel::Info;
            else if (value == "WARNING") level = LogLevel::Warning;
            else if (value == "ERROR") level = LogLevel::Error;
            else if (value == "CRITICAL") level = LogLevel::Critical;
            else {
                cout << "Warning: Invalid log level: " << value << ". Using INFO." << endl;
                level = LogLevel::Info;
            }
            globalConfig->set("log_level", value, ConfigSource::CommandLine);
            Logger::getGlobal().setLevel(level);
        }
        else if (arg == "--log-to-file") {
            globalConfig->set("log_to_file", true, ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 12) == "--log-file=") {
            string value = arg.substr(12);
            globalConfig->set("log_filename", value, ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 18) == "--checkpoint-dir=") {
            string value = arg.substr(18);
            globalConfig->set("checkpoint_dir", value, ConfigSource::CommandLine);
        }
        else if (arg == "--disable-plasticities") {
            globalConfig->set("plasticity.stdp.enable", false, ConfigSource::CommandLine);
            globalConfig->set("plasticity.hebbian.enable", false, ConfigSource::CommandLine);
            globalConfig->set("plasticity.structural.enable", false, ConfigSource::CommandLine);
        }
        else if (arg == "--enable-plasticities") {
            globalConfig->set("plasticity.stdp.enable", true, ConfigSource::CommandLine);
            globalConfig->set("plasticity.hebbian.enable", true, ConfigSource::CommandLine);
            globalConfig->set("plasticity.structural.enable", true, ConfigSource::CommandLine);
        }
        else if (arg == "--enable-curiosity") {
            globalConfig->set("neuromod.curiosity.enable", true, ConfigSource::CommandLine);
        }
        else if (arg == "--enable-novelty") {
            globalConfig->set("neuromod.novelty.enable", true, ConfigSource::CommandLine);
        }
        else if (arg == "--enable-reward-modulation") {
            globalConfig->set("neuromod.dopamine.enable", true, ConfigSource::CommandLine);
        }
        else if (arg == "--enable-development") {
            globalConfig->set("development.enable", true, ConfigSource::CommandLine);
        }
        else {
            // Handle demo names or unknown arguments
            break;
        }
    }
}

// Run Phase 2 Demo
void runPhase2Demo(Config& config) {
    cout << "\nRunning Phase 2: Real Neural Computation Demo" << endl;
    cout << "=============================================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    // Run basic connectivity test
    cout << "Test 1: Basic Neural Connectivity" << endl;
    runBasicConnectivityTest(brain);
    
    // Reset and run plasticity experiment
    brain->reset();
    brain->initialize();
    
    cout << "\nTest 2: Plasticity Learning Experiment" << endl;
    runPlasticityExperiment(brain);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    
    cout << "\nTest 3: STDP Verification" << endl;
    runStdpVerification(brain);
    
    cout << "\nPhase 2 Demo Complete!" << endl;
    brain->logStatus();
}

// Run Phase 6 Demo
void runPhase6Demo(Config& config) {
    cout << "\nRunning Phase 6: Final Integration Demo" << endl;
    cout << "==========================================" << endl;
    
    // Create brain with comprehensive configuration
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    cout << "Brain initialized with integrated systems:" << endl;
    cout << "  - Memory systems: Working, Episodic, Associative" << endl;
    cout << "  - Prediction system" << endl;
    cout << "  - Cognition: Planning, Concept Formation, Attention" << endl;
    cout << "  - Development system" << endl;
    cout << "  - Neuromodulation: Dopamine, Curiosity, Novelty" << endl;
    
    // Run comprehensive integration test
    cout << "\nRunning integration test (5000 steps)..." << endl;
    for (SimulationStep step = 0; step < 5000; ++step) {
        // Inject sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + (rand() % 20));
        
        brain->step(step, step * config.getOr<double>("simulation_timestep", 0.001));
        
        if (step % 1000 == 0) {
            cout << "  Step " << step << ": " 
                 << brain->getFiringNeuronCount() << " firing, "
                 << brain->getTotalSpikeCount() << " spikes, "
                 << "Dev: " << static_cast<int>(brain->getDevelopmentalStage()) << endl;
        }
    }
    
    cout << "\nIntegration test complete!" << endl;
    brain->logStatus();
}

// Run Learning World Demo
void runLearningWorldDemo(Config& config) {
    cout << "\nRunning Learning in World Demo" << endl;
    cout << "=============================" << endl;
    
    // Create brain and agent
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    auto agent = make_shared<AgentBrain>(brain);
    
    // Create world
    auto world = make_shared<SimpleWorld>();
    world->configure(15, 15, 8, 8);
    world->reset();
    world->setAgentStart(7.5, 7.5);
    
    agent->initialize(world);
    
    cout << "Starting agent learning episode (1000 steps)..." << endl;
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step, world->getSimulationTime());
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward based on world state
        float reward = 0.0f;
        if (world->getAgentBody()->energy < 30.0f) {
            reward = 1.0f;
        }
        agent->applyRewardModulation(reward, 0.0f);
        agent->updateDevelopment(0.1);
        
        if (step % 200 == 0) {
            cout << "  Step " << step << ": " 
                 << "Energy: " << world->getAgentBody()->energy << ", "
                 << "Curiosity: " << agent->getCuriosityLevel() << ", "
                 << "Novelty: " << agent->getNoveltyLevel() << ", "
                 << "Reward: " << reward << endl;
        }
    }
    
    cout << "\nLearning episode complete!" << endl;
}

// Run Plasticity Demo
void runPlasticityDemo(Config& config) {
    cout << "\nRunning Plasticity Learning Demo" << endl;
    cout << "================================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    cout << "Demonstrating synaptic plasticity (3000 steps)..." << endl;
    
    // Enable all plasticity systems
    brain->getSTDP()->enable(true, true, false);
    brain->getHebbian()->enable(true);
    brain->getStructuralPlasticity()->enable(true);
    
    for (SimulationStep step = 0; step < 3000; ++step) {
        // Apply patterned input to drive learning
        for (size_t i = 0; i < 5; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        }
        
        brain->step(step, step * config.getOr<double>("simulation_timestep", 0.001));
        
        if (step % 600 == 0) {
            cout << "  Step " << step << ": " 
                 << "Learning: " << brain->getTotalSpikeCount() << " spikes, "
                 << "Plasticity active" << endl;
        }
    }
    
    cout << "\nPlasticity demo complete!" << endl;
    brain->logStatus();
}

// Run Memory Demo
void runMemoryDemo(Config& config) {
    cout << "\nRunning Memory Integration Demo" << endl;
    cout << "===============================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    auto associativeMemory = brain->getAssociativeMemory();
    
    cout << "Memory systems initialized and ready for encoding..." << endl;
    
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Create experiences for memory
        if (step % 100 == 0) {
            workingMemory->storeItem(step, step * 0.001);
            episodicMemory->encodeExperience(step % 10, step * 0.001, step % 5);
            associativeMemory->formAssociation(step % 7, step % 11);
        }
        
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain->step(step, step * 0.001);
        
        if (step % 400 == 0) {
            cout << "  Step " << step << ": " 
                 << "Working: " << workingMemory->getActiveTraces() << " traces, "
                 << "Episodic: " << episodicMemory->getEpisodeCount() << " episodes, "
                 << "Associative: " << associativeMemory->getPatternCount() << " patterns" << endl;
        }
    }
    
    cout << "\nMemory demo complete!" << endl;
    cout << "  - Working memory traces: " << workingMemory->getActiveTraces() << endl;
    cout << "  - Episodic memory episodes: " << episodicMemory->getEpisodeCount() << endl;
    cout << "  - Associative memory patterns: " << associativeMemory->getPatternCount() << endl;
}

// Run Prediction Demo
void runPredictionDemo(Config& config) {
    cout << "\nRunning Prediction & Planning Demo" << endl;
    cout << "=====================================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    auto predictionSystem = brain->getPredictionSystem();
    auto planner = brain->getPlanner();
    auto attention = brain->getAttention();
    
    cout << "Prediction and planning systems active..." << endl;
    
    for (SimulationStep step = 0; step < 1500; ++step) {
        float sensoryInput = 15.0f + (rand() % 30);
        brain->injectCurrentToNeurons(NeuronType::Sensory, sensoryInput);
        brain->step(step, step * 0.001);
        
        if (predictionSystem) {
            float prediction = predictionSystem->predict(sensoryInput);
            float error = std::abs(prediction - sensoryInput);
            predictionSystem->update(prediction, error, step * 0.001);
        }
        
        if (attention) {
            attention->update(sensoryInput, step * 0.001);
        }
        
        if (step % 300 == 0 && predictionSystem) {
            cout << "  Step " << step << ": " 
                 << "Prediction: " << predictionSystem->predict(sensoryInput) << ", "
                 << "Error: " << predictionSystem->getPredictionError() << ", "
                 << "Confidence: " << predictionSystem->getConfidence() << ", "
                 << "Attention: " << attention->getAttentionFocus() << endl;
        }
    }
    
    cout << "\nPrediction demo complete!" << endl;
    if (predictionSystem) {
        cout << "  - Final prediction accuracy: " << (100.0f - predictionSystem->getPredictionError()) << "%" << endl;
    }
}

// Run Development Demo
void runDevelopmentDemo(Config& config) {
    cout << "\nRunning Development Process Demo" << endl;
    cout << "=================================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    cout << "Brain development cycle active..." << endl;
    
    for (SimulationStep step = 0; step < 3000; ++step) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f + (rand() % 25));
        brain->step(step, step * 0.001);
        
        // Development effects
        brain->develop();
        
        if (step % 500 == 0) {
            cout << "  Step " << step << ": " 
                 << "Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << ", "
                 << "Firing: " << brain->getFiringNeuronCount() << " neurons, "
                 << "Development active" << endl;
        }
    }
    
    cout << "\nDevelopment demo complete!" << endl;
    brain->logStatus();
}

// Run STDP Demo
void runStdpDemo(Config& config) {
    cout << "\nRunning STDP Verification Demo" << endl;
    cout << "===============================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    cout << "Demonstrating Spike-Timing-Dependent Plasticity..." << endl;
    
    // Simple STDP demonstration
    auto* region = brain->getRegion(RegionId(1));
    if (region && !region->getSynapses().empty()) {
        auto synapse = region->getSynapses()[0];
        synapse->enablePlasticity(false, true, false); // STDP only
        
        float beforeWeight = synapse->getWeight();
        cout << "  Initial synapse weight: " << beforeWeight << endl;
        
        // Correlated pre-post activity
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(NeuronId(trial % 10), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(NeuronId((trial + 1) % 10), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float afterWeight = synapse->getWeight();
        float delta = afterWeight - beforeWeight;
        
        cout << "  Final synapse weight: " << afterWeight << endl;
        cout << "  Change: " << delta << " (" << (delta > 0 ? "potentiation" : "depression") << ")" << endl;
        
        if (delta > 0.001f) {
            cout << "  ✓ STDP working: Pre-before-post produced potentiation" << endl;
        } else if (delta < -0.001f) {
            cout << "  ! STDP reversed: Check parameters" << endl;
        } else {
            cout << "  ! No change: STDP may not be triggering" << endl;
        }
    }
    
    cout << "\nSTDP demo complete!" << endl;
}

// Run Hebbian Demo
void runHebbianDemo(Config& config) {
    cout << "\nRunning Hebbian Learning Demo" << endl;
    cout << "=============================" << endl;
    
    auto brain = make_shared<Brain>(make_shared<Config>(config));
    brain->initialize();
    
    cout << "Demonstrating Hebbian co-activation learning..." << endl;
    
    // Enable Hebbian learning
    brain->getHebbian()->enable(true);
    
    // Create correlated activity patterns
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Correlated pre-post activation
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 400 == 0) {
            cout << "  Step " << step << ": " 
                 << "Hebbian learning active, "
                 << brain->getTotalSpikeCount() << " spikes so far" << endl;
        }
    }
    
    cout << "\nHebbian demo complete!" << endl;
    brain->logStatus();
}

// Run Benchmark Demo
void runBenchmarkDemo(Config& config) {
    cout << "\nRunning Performance Benchmark" << endl;
    cout << "=============================" << endl;
    
    // This would use the existing benchmark.cpp functionality
    cout << "Benchmark functionality available in separate benchmark binary." << endl;
    cout << "Run: ./nlm_benchmark" << endl;
}

// Run Comprehensive Demo
void runComprehensiveDemo(Config& config) {
    cout << "\nRunning Comprehensive Demo Suite" << endl;
    cout << "===============================" << endl;
    
    // This would use the comprehensive_demo.cpp we created
    cout << "Comprehensive demo available in separate comprehensive_demo binary." << endl;
    cout << "Run: ./nlm --demo=comprehensive" << endl;
}

// Placeholder functions for basic connectivity test
void runBasicConnectivityTest(shared_ptr<Brain> brain) {
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
    for (size_t i = 0; i < min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
    }
}

void runPlasticityExperiment(shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    vector<NeuronId> patternNeurons;
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
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + to_string(step) + 
                        " | Spikes: " + to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);
    float beforeWeight = syn->getWeight();
    
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + to_string(beforeWeight) +
                " (pre=" + to_string(syn->getSourceNeuron().index()) +
                ", post=" + to_string(syn->getDestinationNeuron().index()) + ")");
    
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
    NLM_LOG_INFO("    Synapse after: " + to_string(syn->getWeight()) +
                " (Δ=" + to_string(delta) + ")");
    
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

int main(int argc, char** argv) {
    // Initialize global configuration
    globalConfig = make_shared<Config>();
    
    // Parse command line arguments
    parseCommandLineArgs(argc, argv);
    
    // Determine demo to run
    string demoName;
    bool demoSpecified = false;
    
    for (int i = 1; i < argc; ++i) {
        string arg(argv[i]);
        
        // Skip option parsing
        if (arg.substr(0, 2) == "--" && arg.find('=') == string::npos && 
            arg != "--help" && arg != "-h" && arg != "--version" && 
            arg != "--list-demos") {
            // Check if it's a numeric value (e.g., --neuron-count=2000)
            bool isNumber = false;
            if (arg.find('=') != string::npos) {
                string value = arg.substr(arg.find('=') + 1);
                // Check if value looks like a number
                isNumber = !value.empty() && (isdigit(value[0]) || (value[0] == '-' && value.size() > 1 && isdigit(value[1])));
            }
            
            if (!isNumber) {
                // This is a demo name
                demoName = arg.substr(2); // Remove -- prefix
                demoSpecified = true;
                break;
            }
        }
        else if (arg[0] == '-' && arg.size() > 1 && i + 1 < argc && 
                 !(isdigit(argv[i + 1][0]) || argv[i + 1][0] == '-')) {
            // Check if it's a demo name (-demo)
            demoName = arg.substr(1);
            demoSpecified = true;
            break;
        }
        else if (arg[0] == '-' && arg.size() > 1 && i + 1 < argc && 
                 isdigit(argv[i + 1][0])) {
            // Skip numeric arguments after options
            i++;
        }
    }
    
    if (!demoSpecified) {
        printHelp();
        return 1;
    }
    
    // Initialize logger
    auto logger = make_shared<Logger>();
    auto consoleLogger = make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    printBanner();
    
    // Log configuration summary
    NLM_LOG_INFO("NLM Neural Learning Machine - Command Line Interface");
    NLM_LOG_INFO("Demo: " + demoName);
    
    // Check if demo exists
    if (demos.find(demoName) == demos.end()) {
        NLM_LOG_ERROR("Unknown demo: " + demoName);
        printAvailableDemos();
        return 1;
    }
    
    try {
        // Run the selected demo
        NLM_LOG_INFO("Starting demo: " + demos[demoName].second(demos[demoName].first + " Demo"));
        
        if (demoName == "phase2") {
            runPhase2Demo(*globalConfig);
        }
        else if (demoName == "phase6") {
            runPhase6Demo(*globalConfig);
        }
        else if (demoName == "learning") {
            runLearningWorldDemo(*globalConfig);
        }
        else if (demoName == "plasticity") {
            runPlasticityDemo(*globalConfig);
        }
        else if (demoName == "memory") {
            runMemoryDemo(*globalConfig);
        }
        else if (demoName == "prediction") {
            runPredictionDemo(*globalConfig);
        }
        else if (demoName == "development") {
            runDevelopmentDemo(*globalConfig);
        }
        else if (demoName == "stdp") {
            runStdpDemo(*globalConfig);
        }
        else if (demoName == "hebbian") {
            runHebbianDemo(*globalConfig);
        }
        else if (demoName == "benchmark") {
            runBenchmarkDemo(*globalConfig);
        }
        else if (demoName == "comprehensive") {
            runComprehensiveDemo(*globalConfig);
        }
        
        NLM_LOG_INFO("Demo completed successfully!");
        
    } catch (const exception& e) {
        NLM_LOG_ERROR("Demo failed with exception: " + string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Demo failed with unknown exception");
        return 1;
    }
    
    return 0;
}