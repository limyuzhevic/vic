#include "NLMFacade.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace nlm {

struct NLMFacade::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    Phase6IntegratedExperiment integrationExperiment;
    
    // Advanced system components
    std::unique_ptr<Neuromodulator> neuromodulator;
    std::unique_ptr<MemorySystem> memorySystem;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // State tracking
    bool initialized;
    bool running;
    bool monitoring;
    size_t monitoringIntervalMs;
    std::vector<std::string> monitoringData;
    
    // Event listeners
    std::vector<std::function<void(const SpikeEvent&)>> spikeListeners;
    std::vector<std::function<void(float)>> rewardListeners;
    std::vector<std::function<void(const MemoryEvent&)>> memoryListeners;
    
    // Real-time mode
    bool realTimeMode;
    float targetFPS;
    float simulationSpeedMultiplier;
    
    // Debugging
    bool debugMode;
    bool debugOutput;
    std::vector<std::string> systemWarnings;
    
    // Custom commands
    std::map<std::string, std::function<void()>> customCommands;
    
    // Environmental model
    std::vector<float> environmentalModel;
    bool environmentalModelInitialized;
    
    // Social connections
    std::map<std::string, std::shared_ptr<Brain>> peerConnections;
    
    Impl() : config(nullptr), brain(nullptr), initialized(false), running(false),
             monitoring(false), monitoringIntervalMs(1000), realTimeMode(false),
             targetFPS(60.0f), simulationSpeedMultiplier(1.0f), debugMode(false),
             debugOutput(false), environmentalModelInitialized(false) {
    }
};

NLMFacade::NLMFacade(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    if (!pImpl->config) {
        pImpl->config = std::make_shared<Config>();
    }
    
    setupDefaultConfiguration();
    initializeAdvancedSystems();
    setupEventHandlers();
}

NLMFacade::~NLMFacade() = default;

bool NLMFacade::initialize(size_t neuronCount) {
    if (pImpl->initialized) {
        return true;
    }
    
    try {
        // Create brain with configuration
        pImpl->brain = std::make_shared<Brain>(pImpl->config);
        
        // Initialize with specified neuron count
        auto neuronConfig = pImpl->config->getOr<int64_t>("neuron_count", static_cast<int64_t>(neuronCount));
        pImpl->config->set("neuron_count", static_cast<int64_t>(neuronConfig), ConfigSource::Runtime);
        
        if (!pImpl->brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain" << std::endl;
            return false;
        }
        
        pImpl->initialized = true;
        pImpl->running = true;
        
        // Initialize advanced systems
        initializeAdvancedSystems();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

void NLMFacade::setupDefaultConfiguration() {
    if (!pImpl->config) {
        pImpl->config = std::make_shared<Config>();
    }
    
    // Set sensible defaults if not already set
    if (!pImpl->config->has("random_seed")) {
        pImpl->config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    if (!pImpl->config->has("simulation_timestep")) {
        pImpl->config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    if (!pImpl->config->has("neuron_count")) {
        pImpl->config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    }
    if (!pImpl->config->has("region_count")) {
        pImpl->config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    if (!pImpl->config->has("connection_probability")) {
        pImpl->config->set("connection_probability", 0.1f, ConfigSource::Default);
    }
    
    // Enable advanced features
    pImpl->config->set("eligibility_trace_decay", 0.01f, ConfigSource::Default);
    pImpl->config->set("reward_scale", 1.0f, ConfigSource::Default);
    pImpl->config->set("max_eligibility_trace", 1.0f, ConfigSource::Default);
}

void NLMFacade::initializeAdvancedSystems() {
    if (!pImpl->brain) return;
    
    // Initialize neuromodulation
    pImpl->neuromodulator = std::make_unique<Neuromodulator>(pImpl->brain.get());
    
    // Initialize memory systems
    pImpl->memorySystem = std::make_unique<MemorySystem>(pImpl->brain.get());
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>(pImpl->brain.get());
    
    // Initialize planner
    pImpl->planner = std::make_unique<NeuralPlanner>(pImpl->brain.get());
    
    // Initialize development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>(pImpl->brain.get());
}

void NLMFacade::setupEventHandlers() {
    if (!pImpl->brain) return;
    
    // Set up spike listeners
    pImpl->brain->getSpikeSystem()->registerHandler(
        [this](const DetailedSpikeEvent& event) {
            SpikeEvent spikeEvent;
            spikeEvent.neuron_id = event.neuron_id;
            spikeEvent.time = event.time;
            spikeEvent.is_excitatory = event.is_excitatory;
            spikeEvent.weight = event.weight;
            
            for (auto& listener : pImpl->spikeListeners) {
                listener(spikeEvent);
            }
        }
    );
    
    // Set up reward listeners
    pImpl->brain->applyNeuromodulation(
        Neuromodulator::createRewardSignal(0.0f, 1.0f)
    );
}

void NLMFacade::reset() {
    if (pImpl->brain) {
        pImpl->brain->reset();
        pImpl->brain->initialize();
    }
    
    // Reset advanced systems
    if (pImpl->memorySystem) {
        pImpl->memorySystem->reset();
    }
}

void NLMFacade::step() {
    if (!pImpl->running || !pImpl->brain) return;
    
    static SimulationStep step = 0;
    pImpl->brain->step(step, static_cast<Timestamp>(step) * 0.001);
    step++;
    
    // Update monitoring data
    if (pImpl->monitoring) {
        std::stringstream ss;
        ss << "Step " << step << ": " 
           << "Neurons: " << pImpl->brain->getTotalNeuronCount() << ", "
           << "Spikes: " << pImpl->brain->getTotalSpikeCount() << ", "
           << "Firing: " << pImpl->brain->getFiringNeuronCount();
        
        pImpl->monitoringData.push_back(ss.str());
        
        // Keep only recent data
        if (pImpl->monitoringData.size() > 100) {
            pImpl->monitoringData.erase(pImpl->monitoringData.begin());
        }
    }
}

void NLMFacade::step(Timestamp time) {
    if (!pImpl->running || !pImpl->brain) return;
    
    static SimulationStep step = 0;
    pImpl->brain->step(step, time);
    step++;
}

void NLMFacade::receiveSensoryInput(const std::vector<float>& input) {
    if (!pImpl->brain) return;
    
    // Convert float input to SensoryInput
    // This is a simplified implementation - real SensoryInput would have more structure
    for (size_t i = 0; i < std::min(input.size(), size_t(10)); ++i) {
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, input[i] * 10.0f);
    }
}

std::vector<float> NLMFacade::getMotorOutput() const {
    if (!pImpl->brain) return std::vector<float>();
    
    // Get motor neuron activity
    auto motorNeurons = pImpl->brain->getMotorNeurons();
    std::vector<float> output;
    
    for (auto* neuron : motorNeurons) {
        if (neuron) {
            output.push_back(neuron->getFiringRate());
        }
    }
    
    return output;
}

void NLMFacade::storeWorkingMemory(const std::string& key, const std::vector<float>& value) {
    if (pImpl->memorySystem) {
        pImpl->memorySystem->storeWorkingMemory(key, value);
    }
}

std::vector<float> NLMFacade::retrieveWorkingMemory(const std::string& key) const {
    if (pImpl->memorySystem) {
        return pImpl->memorySystem->retrieveWorkingMemory(key);
    }
    return std::vector<float>();
}

void NLMFacade::storeEpisodic(const std::string& episodeKey, const std::vector<std::string>& events) {
    if (pImpl->memorySystem) {
        pImpl->memorySystem->storeEpisodic(episodeKey, events);
    }
}

std::vector<std::string> NLMFacade::retrieveEpisodic(const std::string& episodeKey) const {
    if (pImpl->memorySystem) {
        return pImpl->memorySystem->retrieveEpisodic(episodeKey);
    }
    return std::vector<std::string>();
}

void NLMFacade::setReward(float reward) {
    if (pImpl->neuromodulator) {
        pImpl->neuromodulator->setCurrentReward(reward);
    }
    
    // Notify reward listeners
    for (auto& listener : pImpl->rewardListeners) {
        listener(reward);
    }
}

void NLMFacade::setCuriosity(float curiosity) {
    if (pImpl->neuromodulator) {
        pImpl->neuromodulator->setCurrentCuriosity(curiosity);
    }
}

void NLMFacade::setNovelty(float novelty) {
    if (pImpl->neuromodulator) {
        pImpl->neuromodulator->setCurrentNovelty(novelty);
    }
}

float NLMFacade::getCurrentDopamineLevel() const {
    if (pImpl->neuromodulator) {
        return pImpl->neuromodulator->getCurrentDopamineLevel();
    }
    return 0.0f;
}

void NLMFacade::predict(const std::vector<float>& sensoryInput) {
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->updatePrediction(sensoryInput);
    }
}

std::vector<float> NLMFacade::getPrediction() const {
    if (pImpl->predictionSystem) {
        return pImpl->predictionSystem->getCurrentPrediction();
    }
    return std::vector<float>();
}

float NLMFacade::getPredictionError() const {
    if (pImpl->predictionSystem) {
        return pImpl->predictionSystem->getPredictionError();
    }
    return 0.0f;
}

void NLMFacade::planAction(const std::vector<float>& context) {
    if (pImpl->planner) {
        pImpl->planner->updatePlanning(context);
    }
}

std::vector<int> NLMFacade::getPlannedActions() const {
    if (pImpl->planner) {
        return pImpl->planner->getPlannedActions();
    }
    return std::vector<int>();
}

void NLMFacade::setPlanningDepth(size_t depth) {
    if (pImpl->planner) {
        pImpl->planner->setPlanningDepth(depth);
    }
}

void NLMFacade::advanceDevelopmentalStage() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->advanceStage();
    }
}

DevelopmentalStage NLMFacade::getDevelopmentalStage() const {
    if (pImpl->developmentSystem) {
        return pImpl->developmentSystem->getCurrentStage();
    }
    return DevelopmentalStage::Initial;
}

float NLMFacade::getPlasticityRate() const {
    if (pImpl->developmentSystem) {
        return pImpl->developmentSystem->getCurrentPlasticityRate();
    }
    return 1.0f;
}

void NLMFacade::enableEligibilityTraces(bool enable) {
    if (pImpl->brain) {
        auto* stdp = pImpl->brain->getSTDP();
        if (stdp) {
            // Enable or configure eligibility traces based on the advanced plasticity system
            pImpl->brain->getSTDP()->configure(
                pImpl->config->getOr<float>("stdp_ltp_weight", 0.02f),
                pImpl->config->getOr<float>("stdp_ltd_weight", 0.015f),
                pImpl->config->getOr<float>("stdp_tau", 20.0f)
            );
        }
    }
}

void NLMFacade::setEligibilityTraceDecay(float decayRate) {
    pImpl->config->set("eligibility_trace_decay", decayRate, ConfigSource::Runtime);
}

void NLMFacade::applyExternalReward(float reward, float timeHorizon) {
    setReward(reward);
    
    // Apply reward to all synapses using eligibility trace plasticity
    if (pImpl->brain) {
        auto* stdp = pImpl->brain->getSTDP();
        if (stdp) {
            // This would apply reward-modulated learning
            // In practice, this would integrate with the eligibility trace system
        }
    }
}

bool NLMFacade::saveBrainState(const std::string& filepath) const {
    if (!pImpl->brain) return false;
    return pImpl->brain->save(filepath);
}

bool NLMFacade::loadBrainState(const std::string& filepath) {
    if (!pImpl->brain) return false;
    return pImpl->brain->load(filepath);
}

void NLMFacade::enableReplay(bool enable) {
    pImpl->config->set("enable_replay", enable, ConfigSource::Runtime);
}

void NLMFacade::enableCheckpointing(bool enable, const std::string& path) {
    pImpl->config->set("enable_checkpointing", enable, ConfigSource::Runtime);
    if (!path.empty()) {
        pImpl->config->set("checkpoint_path", path, ConfigSource::Runtime);
    }
}

std::string NLMFacade::getBrainStatus() const {
    if (!pImpl->brain) return "Brain not initialized";
    
    std::stringstream ss;
    ss << "NLM Brain Status:" << std::endl;
    ss << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    ss << "  Running: " << (pImpl->running ? "Yes" : "No") << std::endl;
    ss << "  Neuron Count: " << pImpl->brain->getTotalNeuronCount() << std::endl;
    ss << "  Synapse Count: " << pImpl->brain->getTotalSynapseCount() << std::endl;
    ss << "  Total Spikes: " << pImpl->brain->getTotalSpikeCount() << std::endl;
    ss << "  Firing Neurons: " << pImpl->brain->getFiringNeuronCount() << std::endl;
    ss << "  Average Firing Rate: " << pImpl->brain->getAverageFiringRate() << " Hz" << std::endl;
    ss << "  Excitatory/Inhibition Ratio: " << pImpl->brain->getExcitationInhibitionRatio() << std::endl;
    
    return ss.str();
}

std::string NLMFacade::getSystemMetrics() const {
    std::stringstream ss;
    ss << "System Metrics:" << std::endl;
    ss << "  Configuration:" << std::endl;
    ss << "    Random Seed: " << pImpl->config->getOr<int64_t>("random_seed", 42) << std::endl;
    ss << "    Simulation Timestep: " << pImpl->config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    ss << "    Neuron Count: " << pImpl->config->getOr<int64_t>("neuron_count", 1000) << std::endl;
    ss << "    Connection Probability: " << pImpl->config->getOr<float>("connection_probability", 0.1f) << std::endl;
    
    if (pImpl->brain) {
        ss << "  Brain Metrics:" << std::endl;
        ss << "    Total Synapses: " << pImpl->brain->getTotalSynapseCount() << std::endl;
        ss << "    Average Synaptic Weight: " << pImpl->brain->getAverageSynapticWeight() << std::endl;
        ss << "    Pending Spike Events: " << pImpl->brain->getPendingSpikeEventCount() << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, float> NLMFacade::getIntegrationScore() const {
    std::map<std::string, float> scores;
    
    // Calculate integration scores for different systems
    scores["memory_integration"] = 0.85f;  // Placeholder - would calculate actual integration
    scores["neuromodulation_integration"] = 0.90f;
    scores["prediction_integration"] = 0.75f;
    scores["development_integration"] = 0.80f;
    scores["checkpointing"] = 0.95f;
    scores["replay"] = 0.70f;
    
    return scores;
}

void NLMFacade::runExperiment(const std::string& experimentType, const std::map<std::string, std::string>& params) {
    if (experimentType == "phase6_integration") {
        Phase6Config config;
        
        // Parse parameters
        auto it = params.find("max_steps");
        if (it != params.end()) {
            config.maxSteps = std::stoul(it->second);
        }
        
        it = params.find("neuron_count");
        if (it != params.end()) {
            config.neuronCount = std::stoul(it->second);
        }
        
        it = params.find("enable_checkpointing");
        if (it != params.end()) {
            config.enableCheckpointing = (it->second == "true");
        }
        
        // Run the experiment
        auto result = pImpl->integrationExperiment.run(config);
        
        // Log results
        std::cout << "Phase 6 Integration Experiment Results:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    }
}

void NLMFacade::runIntegrationTest() {
    std::cout << "Running NLM Integration Test..." << std::endl;
    
    bool integrationOK = pImpl->integrationExperiment.verifyIntegration();
    
    if (integrationOK) {
        std::cout << "✓ Integration verification PASSED" << std::endl;
    } else {
        std::cout << "✗ Integration verification FAILED" << std::endl;
    }
}

void NLMFacade::runPlasticityExperiment(size_t trials) {
    std::cout << "Running Plasticity Learning Experiment..." << std::endl;
    std::cout << "  Trials: " << trials << std::endl;
    std::cout << "  This would apply repeated input patterns to stimulate synaptic changes" << std::endl;
    
    // This would implement the plasticity experiment using the facade
    // For now, it's a placeholder that demonstrates the advanced commands
}

void NLMFacade::runLearningExperiment(float targetAccuracy) {
    std::cout << "Running Learning Experiment..." << std::endl;
    std::cout << "  Target Accuracy: " << targetAccuracy << std::endl;
    std::cout << "  This would test the brain's ability to learn through experience" << std::endl;
}

void NLMFacade::setConfig(const std::string& key, const std::string& value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

void NLMFacade::setConfig(const std::string& key, float value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

void NLMFacade::setConfig(const std::string& key, int value) {
    pImpl->config->set(key, static_cast<int64_t>(value), ConfigSource::Runtime);
}

std::string NLMFacade::getConfig(const std::string& key) const {
    auto value = pImpl->config->get<std::string>(key);
    return value ? *value : "";
}

void NLMFacade::processBatch(const std::vector<std::vector<float>>& inputs, 
                           std::vector<std::vector<float>>& outputs) {
    std::cout << "Processing batch of " << inputs.size() << " inputs" << std::endl;
    
    // Batch processing implementation
    outputs.resize(inputs.size());
    
    for (size_t i = 0; i < inputs.size(); ++i) {
        outputs[i] = inputs[i];
        // Apply some transformation or processing
        for (float& val : outputs[i]) {
            val *= 0.5f;  // Simple transformation
        }
    }
}

void NLMFacade::injectCurrentBatch(const std::vector<NeuronId>& neuronIds, 
                                   const std::vector<float>& currents) {
    std::cout << "Injecting currents into " << neuronIds.size() << " neurons" << std::endl;
    
    // Batch current injection
    for (size_t i = 0; i < std::min(neuronIds.size(), currents.size()); ++i) {
        if (pImpl->brain) {
            pImpl->brain->injectCurrent(neuronIds[i], currents[i]);
        }
    }
}

void NLMFacade::startMonitoring(size_t intervalMs) {
    pImpl->monitoring = true;
    pImpl->monitoringIntervalMs = intervalMs;
    std::cout << "Monitoring started with interval " << intervalMs << "ms" << std::endl;
}

void NLMFacade::stopMonitoring() {
    pImpl->monitoring = false;
    std::cout << "Monitoring stopped" << std::endl;
}

std::vector<std::string> NLMFacade::getMonitoringData() const {
    return pImpl->monitoringData;
}

size_t NLMFacade::getNeuronCount() const {
    if (!pImpl->brain) return 0;
    return pImpl->brain->getTotalNeuronCount();
}

size_t NLMFacade::getTotalSynapseCount() const {
    if (!pImpl->brain) return 0;
    return pImpl->brain->getTotalSynapseCount();
}

float NLMFacade::getAverageFiringRate() const {
    if (!pImpl->brain) return 0.0f;
    return pImpl->brain->getAverageFiringRate();
}

float NLMFacade::getExcitatoryInhibitionRatio() const {
    if (!pImpl->brain) return 0.0f;
    return pImpl->brain->getExcitationInhibitionRatio();
}

void NLMFacade::enableRealTimeMode(bool enable) {
    pImpl->realTimeMode = enable;
    std::cout << "Real-time mode: " << (enable ? "ENABLED" : "DISABLED") << std::endl;
}

void NLMFacade::setTargetFPS(float fps) {
    pImpl->targetFPS = fps;
    std::cout << "Target FPS set to: " << fps << std::endl;
}

void NLMFacade::enableDebugOutput(bool enable) {
    pImpl->debugOutput = enable;
    if (enable) {
        std::cout << "Debug output enabled" << std::endl;
    }
}

void NLMFacade::addCustomCommand(const std::string& name, std::function<void()> command) {
    pImpl->customCommands[name] = command;
    std::cout << "Custom command added: " << name << std::endl;
}

void NLMFacade::triggerExploration() {
    std::cout << "Triggering exploration behavior" << std::endl;
    if (pImpl->neuromodulator) {
        pImpl->neuromodulator->increaseCuriosity();
    }
}

void NLMFacade::triggerExploitation() {
    std::cout << "Triggering exploitation behavior" << std::endl;
    if (pImpl->neuromodulator) {
        pImpl->neuromodulator->maximizeReward();
    }
}

void NLMFacade::updateEnvironmentalModel(const std::vector<float>& model) {
    pImpl->environmentalModel = model;
    pImpl->environmentalModelInitialized = true;
    std::cout << "Environmental model updated with " << model.size() << " parameters" << std::endl;
}

std::vector<float> NLMFacade::getEnvironmentalModel() const {
    return pImpl->environmentalModel;
}

void NLMFacade::connectToPeer(const std::string& peerId) {
    pImpl->peerConnections[peerId] = pImpl->brain;
    std::cout << "Connected to peer: " << peerId << std::endl;
}

void NLMFacade::disconnectFromPeer(const std::string& peerId) {
    auto it = pImpl->peerConnections.find(peerId);
    if (it != pImpl->peerConnections.end()) {
        pImpl->peerConnections.erase(it);
        std::cout << "Disconnected from peer: " << peerId << std::endl;
    }
}

void NLMFacade::shareBrainState(const std::string& peerId) {
    auto it = pImpl->peerConnections.find(peerId);
    if (it != pImpl->peerConnections.end()) {
        std::cout << "Sharing brain state with peer: " << peerId << std::endl;
        // Implementation would share serialized brain state
    }
}

void NLMFacade::mergeBrainState(const std::string& peerId, float weight) {
    auto it = pImpl->peerConnections.find(peerId);
    if (it != pImpl->peerConnections.end()) {
        std::cout << "Merging brain state with peer: " << peerId << " (weight: " << weight << ")" << std::endl;
        // Implementation would merge brain states
    }
}

bool NLMFacade::isInitialized() const {
    return pImpl->initialized;
}

bool NLMFacade::isRunning() const {
    return pImpl->running;
}

std::string NLMFacade::getVersionInfo() const {
    return "NLM Integration Facade - Version 2.0 (Advanced)";
}

void NLMFacade::addSpikeListener(std::function<void(const SpikeEvent&)> listener) {
    pImpl->spikeListeners.push_back(listener);
}

void NLMFacade::addRewardListener(std::function<void(float)> listener) {
    pImpl->rewardListeners.push_back(listener);
}

void NLMFacade::addMemoryListener(std::function<void(const MemoryEvent&)> listener) {
    pImpl->memoryListeners.push_back(listener);
}

void NLMFacade::loadConfiguration(const std::string& filepath) {
    if (pImpl->config->loadFromFile(filepath)) {
        std::cout << "Configuration loaded from: " << filepath << std::endl;
        // Reinitialize if brain is already initialized
        if (pImpl->initialized && pImpl->brain) {
            pImpl->brain->reset();
            pImpl->brain->initialize();
        }
    } else {
        std::cerr << "Failed to load configuration from: " << filepath << std::endl;
    }
}

bool NLMFacade::saveConfiguration(const std::string& filepath) const {
    return pImpl->config->saveToFile(filepath);
}

void NLMFacade::resetConfiguration() {
    setupDefaultConfiguration();
    std::cout << "Configuration reset to defaults" << std::endl;
}

void NLMFacade::optimizeForSpeed() {
    std::cout << "Optimizing for speed..." << std::endl;
    // Implementation would optimize parameters for faster simulation
    pImpl->config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
}

void NLMFacade::optimizeForAccuracy() {
    std::cout << "Optimizing for accuracy..." << std::endl;
    // Implementation would optimize parameters for higher accuracy
    pImpl->config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
}

void NLMFacade::setSimulationSpeed(float multiplier) {
    pImpl->simulationSpeedMultiplier = multiplier;
    std::cout << "Simulation speed set to: " << multiplier << "x" << std::endl;
}

float NLMFacade::getSimulationSpeed() const {
    return pImpl->simulationSpeedMultiplier;
}

void NLMFacade::enableDebugMode(bool enable) {
    pImpl->debugMode = enable;
    if (enable) {
        std::cout << "Debug mode enabled" << std::endl;
        std::cout << getDebugInfo() << std::endl;
    }
}

std::string NLMFacade::getDebugInfo() const {
    std::stringstream ss;
    ss << "Debug Information:" << std::endl;
    ss << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    ss << "  Running: " << (pImpl->running ? "Yes" : "No") << std::endl;
    ss << "  Real-time Mode: " << (pImpl->realTimeMode ? "Yes" : "No") << std::endl;
    ss << "  Monitoring: " << (pImpl->monitoring ? "Yes" : "No") << std::endl;
    ss << "  Debug Mode: " << (pImpl->debugMode ? "Yes" : "No") << std::endl;
    ss << "  Debug Output: " << (pImpl->debugOutput ? "Yes" : "No") << std::endl;
    ss << "  Custom Commands: " << pImpl->customCommands.size() << std::endl;
    ss << "  Peer Connections: " << pImpl->peerConnections.size() << std::endl;
    
    return ss.str();
}

std::vector<std::string> NLMFacade::getSystemWarnings() const {
    return pImpl->systemWarnings;
}

bool NLMFacade::hasSystemErrors() const {
    return !pImpl->systemWarnings.empty();
}

} // namespace nlm
