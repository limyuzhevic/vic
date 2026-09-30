#include "BrainBuilder.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace nlm {

namespace {

// BrainBuilder implementation
struct BrainBuilder::Impl {
    // Configuration storage
    struct Config {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        double timestep = 0.001;
        
        // Memory system
        size_t workingMemoryCapacity = 0; // Auto-calculated
        size_t episodicMemoryCapacity = 1000;
        bool associativeMemoryEnabled = true;
        
        // Neuromodulation
        float dopamineLevel = 0.5f;
        float curiosityLevel = 0.5f;
        float noveltyLevel = 0.5f;
        float predictionErrorLevel = 0.5f;
        
        // Development
        std::string developmentStages = "Initial:CriticalPeriod:Maturation:Adult:Aging";
        bool criticalPeriodEnabled = true;
        
        // Cognitive architecture
        bool planningEnabled = true;
        bool conceptFormationEnabled = true;
        bool attentionEnabled = true;
        bool predictionEnabled = true;
        
        // Population configuration
        size_t sensoryNeuronCount = 0; // Auto-calculated
        size_t internalNeuronCount = 0; // Auto-calculated
        size_t motorNeuronCount = 0; // Auto-calculated
        float excitatoryNeuronRatio = 0.8f;
        float inhibitoryNeuronRatio = 0.2f;
        
        // Plasticity
        float stdpLTPWeight = 0.01f;
        float stdpLTDWeight = 0.012f;
        float stdpTau = 20.0f;
        bool hebbianEnabled = true;
        bool structuralPlasticityEnabled = true;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        // Checkpoint
        std::string checkpointDir = "";
        size_t checkpointInterval = 10000;
        bool checkpointCompression = true;
        
        // Performance
        PerformanceMode performanceMode = PerformanceMode::Default;
        bool SIMDAcceleration = true;
        bool parallelProcessing = true;
        bool memoryPoolEnabled = true;
        
        // Preset type
        PresetType preset = PresetType::Default;
        
        // Custom validators
        std::vector<std::function<bool(const Config&)>> customValidators;
    };
    
    Config config;
    std::shared_ptr<Config> baseConfig;
    
    // Statistics
    size_t buildCount = 0;
    size_t validationCount = 0;
    size_t errorCount = 0;
};

// BrainBuilder constructor/destructor
BrainBuilder::BrainBuilder() : pImpl(std::make_unique<Impl>()) {
    pImpl->setupDefaultConfiguration();
}

BrainBuilder::~BrainBuilder() = default;

// Fluent configuration methods
BrainBuilder& BrainBuilder::withNeuronCount(size_t count) {
    pImpl->config.neuronCount = count;
    return *this;
}

BrainBuilder& BrainBuilder::withRegionCount(size_t count) {
    pImpl->config.regionCount = count;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withConnectionProbability(float probability) {
    pImpl->config.connectionProbability = probability;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withTimestep(double timestep) {
    pImpl->config.timestep = timestep;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withWorkingMemoryCapacity(size_t capacity) {
    pImpl->config.workingMemoryCapacity = capacity;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withEpisodicMemoryCapacity(size_t capacity) {
    pImpl->config.episodicMemoryCapacity = capacity;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withAssociativeMemoryEnabled(bool enabled) {
    pImpl->config.associativeMemoryEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withDopamineLevel(float level) {
    pImpl->config.dopamineLevel = level;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withCuriosityLevel(float level) {
    pImpl->config.curiosityLevel = level;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withNoveltyLevel(float level) {
    pImpl->config.noveltyLevel = level;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withPredictionErrorLevel(float level) {
    pImpl->config.predictionErrorLevel = level;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withDevelopmentStages(const std::string& stages) {
    pImpl->config.developmentStages = stages;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withCriticalPeriodEnabled(bool enabled) {
    pImpl->config.criticalPeriodEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withPlanningEnabled(bool enabled) {
    pImpl->config.planningEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withConceptFormationEnabled(bool enabled) {
    pImpl->config.conceptFormationEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withAttentionEnabled(bool enabled) {
    pImpl->config.attentionEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withPredictionEnabled(bool enabled) {
    pImpl->config.predictionEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withSensoryNeuronCount(size_t count) {
    pImpl->config.sensoryNeuronCount = count;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withInternalNeuronCount(size_t count) {
    pImpl->config.internalNeuronCount = count;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withMotorNeuronCount(size_t count) {
    pImpl->config.motorNeuronCount = count;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withExcitatoryNeuronRatio(float ratio) {
    pImpl->config.excitatoryNeuronRatio = ratio;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withInhibitoryNeuronRatio(float ratio) {
    pImpl->config.inhibitoryNeuronRatio = ratio;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withSTDPWeight(float ltp, float ltd, float tau) {
    pImpl->config.stdpLTPWeight = ltp;
    pImpl->config.stdpLTDWeight = ltd;
    pImpl->config.stdpTau = tau;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withHebbianLearningEnabled(bool enabled) {
    pImpl->config.hebbianEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withStructuralPlasticityEnabled(bool enabled) {
    pImpl->config.structuralPlasticityEnabled = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withSynaptogenesisRate(float rate) {
    pImpl->config.synaptogenesisRate = rate;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withPruningRate(float rate) {
    pImpl->config.pruningRate = rate;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withCheckpointDir(const std::string& dir) {
    pImpl->config.checkpointDir = dir;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withCheckpointInterval(size_t steps) {
    pImpl->config.checkpointInterval = steps;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withCheckpointCompression(bool enabled) {
    pImpl->config.checkpointCompression = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withPerformanceMode(PerformanceMode mode) {
    pImpl->config.performanceMode = mode;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withSIMDAcceleration(bool enabled) {
    pImpl->config.SIMDAcceleration = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withParallelProcessing(bool enabled) {
    pImpl->config.parallelProcessing = enabled;
    return *this;
}

BrainBuilder& BrainBuilder& BrainBuilder::withMemoryPoolEnabled(bool enabled) {
    pImpl->config.memoryPoolEnabled = enabled;
    return *this;
}

// Preset configurations
BrainBuilder& BrainBuilder::createControlSystem() {
    pImpl->config.preset = PresetType::SimpleControl;
    pImpl->config.neuronCount = 500;
    pImpl->config.regionCount = 1;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = false;
    pImpl->config.excitatoryNeuronRatio = 0.7f;
    pImpl->config.inhibitoryNeuronRatio = 0.3f;
    return *this;
}

BrainBuilder& BrainBuilder::createSensorySystem() {
    pImpl->config.preset = PresetType::SensoryProcessing;
    pImpl->config.neuronCount = 2000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.6f;
    pImpl->config.inhibitoryNeuronRatio = 0.4f;
    pImpl->config.sensoryNeuronCount = 1000;
    return *this;
}

BrainBuilder& BrainBuilder::createCognitiveArchitecture() {
    pImpl->config.preset = PresetType::CognitiveArchitecture;
    pImpl->config.neuronCount = 5000;
    pImpl->config.regionCount = 3;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.8f;
    pImpl->config.inhibitoryNeuronRatio = 0.2f;
    return *this;
}

BrainBuilder& BrainBuilder::createMemoryDominant() {
    pImpl->config.preset = PresetType::MemoryDominant;
    pImpl->config.neuronCount = 3000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.associativeMemoryEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.5f;
    pImpl->config.inhibitoryNeuronRatio = 0.5f;
    return *this;
}

BrainBuilder& BrainBuilder::createMotorControl() {
    pImpl->config.preset = PresetType::MotorControl;
    pImpl->config.neuronCount = 1500;
    pImpl->config.regionCount = 1;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.motorNeuronCount = 500;
    pImpl->config.excitatoryNeuronRatio = 0.8f;
    pImpl->config.inhibitoryNeuronRatio = 0.2f;
    return *this;
}

BrainBuilder& BrainBuilder::createDevelopmentalBrain() {
    pImpl->config.preset = PresetType::Developmental;
    pImpl->config.neuronCount = 4000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.criticalPeriodEnabled = true;
    pImpl->config.developmentStages = "Initial:CriticalPeriod:Maturation:Adult:Aging";
    pImpl->config.excitatoryNeuronRatio = 0.7f;
    pImpl->config.inhibitoryNeuronRatio = 0.3f;
    return *this;
}

BrainBuilder& BrainBuilder::createSocialLearning() {
    pImpl->config.preset = PresetType::SocialLearning;
    pImpl->config.neuronCount = 4000;
    pImpl->config.regionCount = 3;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.associativeMemoryEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.6f;
    pImpl->config.inhibitoryNeuronRatio = 0.4f;
    return *this;
}

BrainBuilder& BrainBuilder::createAttentionFocused() {
    pImpl->config.preset = PresetType::AttentionFocused;
    pImpl->config.neuronCount = 3000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.7f;
    pImpl->config.inhibitoryNeuronRatio = 0.3f;
    return *this;
}

BrainBuilder& BrainBuilder::createPredictionHeavy() {
    pImpl->config.preset = PresetType::PredictionHeavy;
    pImpl->config.neuronCount = 5000;
    pImpl->config.regionCount = 3;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = false;
    pImpl->config.predictionEnabled = true;
    pImpl->config.associativeMemoryEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.8f;
    pImpl->config.inhibitoryNeuronRatio = 0.2f;
    return *this;
}

BrainBuilder& BrainBuilder::createRewardBased() {
    pImpl->config.preset = PresetType::RewardBased;
    pImpl->config.neuronCount = 3000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.dopamineLevel = 0.8f;
    pImpl->config.excitatoryNeuronRatio = 0.7f;
    pImpl->config.inhibitoryNeuronRatio = 0.3f;
    return *this;
}

BrainBuilder& BrainBuilder::createExplorationFocused() {
    pImpl->config.preset = PresetType::ExplorationFocused;
    pImpl->config.neuronCount = 4000;
    pImpl->config.regionCount = 3;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.curiosityLevel = 0.8f;
    pImpl->config.excitatoryNeuronRatio = 0.6f;
    pImpl->config.inhibitoryNeuronRatio = 0.4f;
    return *this;
}

BrainBuilder& BrainBuilder::createCuriosityDriven() {
    pImpl->config.preset = PresetType::CuriosityDriven;
    pImpl->config.neuronCount = 4000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.curiosityLevel = 0.9f;
    pImpl->config.noveltyLevel = 0.7f;
    pImpl->config.excitatoryNeuronRatio = 0.5f;
    pImpl->config.inhibitoryNeuronRatio = 0.5f;
    return *this;
}

BrainBuilder& BrainBuilder::forNavigationTask() {
    pImpl->config.preset = PresetType::Navigation;
    pImpl->config.neuronCount = 2000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.motorNeuronCount = 300;
    pImpl->config.excitatoryNeuronRatio = 0.7f;
    pImpl->config.inhibitoryNeuronRatio = 0.3f;
    return *this;
}

BrainBuilder& BrainBuilder::forObjectRecognition() {
    pImpl->config.preset = PresetType::ObjectRecognition;
    pImpl->config.neuronCount = 3000;
    pImpl->config.regionCount = 3;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.sensoryNeuronCount = 1500;
    pImpl->config.excitatoryNeuronRatio = 0.6f;
    pImpl->config.inhibitoryNeuronRatio = 0.4f;
    return *this;
}

BrainBuilder& BrainBuilder::forSequentialLearning() {
    pImpl->config.preset = PresetType::SequentialLearning;
    pImpl->config.neuronCount = 3000;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = false;
    pImpl->config.conceptFormationEnabled = false;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.associativeMemoryEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.5f;
    pImpl->config.inhibitoryNeuronRatio = 0.5f;
    return *this;
}

BrainBuilder& BrainBuilder::forDecisionMaking() {
    pImpl->config.preset = PresetType::DecisionMaking;
    pImpl->config.neuronCount = 2500;
    pImpl->config.regionCount = 2;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.dopamineLevel = 0.7f;
    pImpl->config.excitatoryNeuronRatio = 0.7f;
    pImpl->config.inhibitoryNeuronRatio = 0.3f;
    return *this;
}

BrainBuilder& BrainBuilder::forSocialInteraction() {
    pImpl->config.preset = PresetType::SocialInteraction;
    pImpl->config.neuronCount = 4000;
    pImpl->config.regionCount = 3;
    pImpl->config.planningEnabled = true;
    pImpl->config.conceptFormationEnabled = true;
    pImpl->config.attentionEnabled = true;
    pImpl->config.predictionEnabled = true;
    pImpl->config.associativeMemoryEnabled = true;
    pImpl->config.socialLearningEnabled = true;
    pImpl->config.excitatoryNeuronRatio = 0.6f;
    pImpl->config.inhibitoryNeuronRatio = 0.4f;
    return *this;
}

// Build methods
std::shared_ptr<Brain> BrainBuilder::build() {
    pImpl->buildCount++;
    
    // Validate configuration
    if (!validateConfigurationInternal()) {
        throw NLMException(NLMErrorCode::IntegrationFailed, "Brain configuration validation failed", "BrainBuilder::build");
    }
    
    // Create configuration
    auto config = createConfigFromBuilder();
    
    // Build brain
    auto brain = std::make_shared<Brain>(config);
    
    // Apply preset-specific configurations
    applyPreset(pImpl->config.preset);
    
    return brain;
}

std::shared_ptr<Brain> BrainBuilder::buildAndInitialize() {
    auto brain = build();
    brain->initialize();
    return brain;
}

std::string BrainBuilder::getConfigurationSummary() const {
    std::stringstream ss;
    ss << "Brain Configuration Summary:";
    ss << "\n  Neurons: " << pImpl->config.neuronCount;
    ss << "\n  Regions: " << pImpl->config.regionCount;
    ss << "\n  Connection probability: " << pImpl->config.connectionProbability;
    ss << "\n  Timestep: " << pImpl->config.timestep;
    ss << "\n  Working memory capacity: " << (pImpl->config.workingMemoryCapacity > 0 ? std::to_string(pImpl->config.workingMemoryCapacity) : "Auto");
    ss << "\n  Episodic memory capacity: " << pImpl->config.episodicMemoryCapacity;
    ss << "\n  Associative memory enabled: " << (pImpl->config.associativeMemoryEnabled ? "Yes" : "No");
    ss << "\n  Dopamine level: " << pImpl->config.dopamineLevel;
    ss << "\n  Curiosity level: " << pImpl->config.curiosityLevel;
    ss << "\n  Planning enabled: " << (pImpl->config.planningEnabled ? "Yes" : "No");
    ss << "\n  Concept formation enabled: " << (pImpl->config.conceptFormationEnabled ? "Yes" : "No");
    ss << "\n  Attention enabled: " << (pImpl->config.attentionEnabled ? "Yes" : "No");
    ss << "\n  Prediction enabled: " << (pImpl->config.predictionEnabled ? "Yes" : "No");
    ss << "\n  STDP enabled: " << (pImpl->config.hebbianEnabled ? "Yes" : "No");
    ss << "\n  Structural plasticity enabled: " << (pImpl->config.structuralPlasticityEnabled ? "Yes" : "No");
    ss << "\n  Performance mode: " << static_cast<int>(pImpl->config.performanceMode);
    ss << "\n  SIMD acceleration: " << (pImpl->config.SIMDAcceleration ? "Yes" : "No");
    ss << "\n  Parallel processing: " << (pImpl->config.parallelProcessing ? "Yes" : "No");
    ss << "\n  Memory pool enabled: " << (pImpl->config.memoryPoolEnabled ? "Yes" : "No");
    
    return ss.str();
}

bool BrainBuilder::validateConfiguration() const {
    pImpl->validationCount++;
    return validateConfigurationInternal();
}

void BrainBuilder::reset() {
    pImpl->config = Impl::Config{};
    pImpl->setupDefaultConfiguration();
}

bool BrainBuilder::exportToJSON(const std::string& filepath) const {
    rapidjson::Document doc;
    doc.SetObject();
    
    // Convert config to JSON
    doc.AddMember("neuronCount", pImpl->config.neuronCount, doc.GetAllocator());
    doc.AddMember("regionCount", pImpl->config.regionCount, doc.GetAllocator());
    doc.AddMember("connectionProbability", pImpl->config.connectionProbability, doc.GetAllocator());
    doc.AddMember("timestep", pImpl->config.timestep, doc.GetAllocator());
    
    // ... add more fields
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::OStringStream stream;
    rapidjson::PrettyWriter<rapidjson::OStringStream> writer(stream);
    doc.Accept(writer);
    
    file << stream.GetString();
    file.close();
    
    return true;
}

bool BrainBuilder::importFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    rapidjson::Document doc;
    doc.Parse(buffer.str().c_str());
    
    if (doc.HasParseError()) {
        return false;
    }
    
    // Load config from JSON
    if (doc.HasMember("neuronCount")) {
        pImpl->config.neuronCount = doc["neuronCount"].GetUint();
    }
    if (doc.HasMember("regionCount")) {
        pImpl->config.regionCount = doc["regionCount"].GetUint();
    }
    if (doc.HasMember("connectionProbability")) {
        pImpl->config.connectionProbability = doc["connectionProbability"].GetFloat();
    }
    if (doc.HasMember("timestep")) {
        pImpl->config.timestep = doc["timestep"].GetDouble();
    }
    
    return true;
}

std::unique_ptr<BrainBuilder> BrainBuilder::clone() const {
    auto cloned = std::make_unique<BrainBuilder>();
    cloned->pImpl = std::make_unique<Impl>(*pImpl);
    return cloned;
}

std::unordered_map<std::string, std::string> BrainBuilder::getDefaultConfiguration() {
    std::unordered_map<std::string, std::string> defaults;
    defaults["neuronCount"] = "1000";
    defaults["regionCount"] = "1";
    defaults["connectionProbability"] = "0.1";
    defaults["timestep"] = "0.001";
    // ... more defaults
    return defaults;
}

BrainBuilder BrainBuilder::createDefault() {
    return BrainBuilder();
}

BrainBuilder BrainBuilder::createMinimal() {
    BrainBuilder builder;
    builder.pImpl->config.neuronCount = 100;
    builder.pImpl->config.regionCount = 1;
    builder.pImpl->config.connectionProbability = 0.01f;
    builder.pImpl->config.timestep = 0.001;
    builder.pImpl->config.planningEnabled = false;
    builder.pImpl->config.conceptFormationEnabled = false;
    builder.pImpl->config.attentionEnabled = false;
    builder.pImpl->config.predictionEnabled = false;
    builder.pImpl->config.hebbianEnabled = false;
    builder.pImpl->config.structuralPlasticityEnabled = false;
    return builder;
}

// Internal helper methods
void BrainBuilder::applyPreset(PresetType preset) {
    switch (preset) {
        case PresetType::Default:
            setupDefaultConfiguration();
            break;
        case PresetType::SimpleControl:
            createControlSystem();
            break;
        case PresetType::SensoryProcessing:
            createSensorySystem();
            break;
        // ... other cases
        default:
            setupDefaultConfiguration();
            break;
    }
}

void BrainBuilder::setupDefaultConfiguration() {
    pImpl->config = Impl::Config{};
}

bool BrainBuilder::validateConfigurationInternal() const {
    // Validate neuron count
    if (!isNeuronCountValid(pImpl->config.neuronCount)) {
        return false;
    }
    
    // Validate region count
    if (!isRegionCountValid(pImpl->config.regionCount)) {
        return false;
    }
    
    // Validate connection probability
    if (!isConnectionProbabilityValid(pImpl->config.connectionProbability)) {
        return false;
    }
    
    // Validate timestep
    if (!isTimestepValid(pImpl->config.timestep)) {
        return false;
    }
    
    // Validate population counts
    if (!arePopulationCountsValid()) {
        return false;
    }
    
    // Validate ratios
    if (!isRatioValid(pImpl->config.excitatoryNeuronRatio)) {
        return false;
    }
    if (!isRatioValid(pImpl->config.inhibitoryNeuronRatio)) {
        return false;
    }
    
    // Check that ratios sum to reasonable values
    float totalRatio = pImpl->config.excitatoryNeuronRatio + pImpl->config.inhibitoryNeuronRatio;
    if (totalRatio < 0.8f || totalRatio > 2.0f) {
        return false;
    }
    
    return true;
}

std::shared_ptr<Config> BrainBuilder::createConfigFromBuilder() const {
    auto config = std::make_shared<Config>();
    
    // Set configuration values from builder
    // This would require adding all the builder config fields to the Config class
    // For brevity, we'll just create a basic config
    
    return config;
}

// Validation helper methods
bool BrainBuilder::isNeuronCountValid(size_t count) const {
    return count > 0 && count < 1000000;
}

bool BrainBuilder::isRegionCountValid(size_t count) const {
    return count > 0 && count < 1000;
}

bool BrainBuilder::isConnectionProbabilityValid(float prob) const {
    return prob >= 0.0f && prob <= 1.0f;
}

bool BrainBuilder::isTimestepValid(double timestep) const {
    return timestep > 0.0 && timestep < 0.1;
}

bool BrainBuilder::arePopulationCountsValid() const {
    // Check that at least one population type is configured
    return pImpl->config.sensoryNeuronCount > 0 || 
           pImpl->config.internalNeuronCount > 0 || 
           pImpl->config.motorNeuronCount > 0;
}

bool BrainBuilder::isRatioValid(float ratio) const {
    return ratio >= 0.0f && ratio <= 1.0f;
}

// Factory functions
namespace BrainFactory {
    
    std::shared_ptr<Brain> createSimpleBrain(size_t neuronCount) {
        return BrainBuilder()
            .withNeuronCount(neuronCount)
            .withRegionCount(1)
            .createControlSystem()
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createCognitiveBrain(size_t neuronCount) {
        return BrainBuilder()
            .withNeuronCount(neuronCount)
            .withRegionCount(3)
            .createCognitiveArchitecture()
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createMemoryBrain(size_t neuronCount) {
        return BrainBuilder()
            .withNeuronCount(neuronCount)
            .withRegionCount(2)
            .createMemoryDominant()
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createDevelopmentalBrain(size_t neuronCount) {
        return BrainBuilder()
            .withNeuronCount(neuronCount)
            .withRegionCount(2)
            .createDevelopmentalBrain()
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createMotorControlBrain(size_t neuronCount) {
        return BrainBuilder()
            .withNeuronCount(neuronCount)
            .withRegionCount(1)
            .createMotorControl()
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createSocialLearningBrain(size_t neuronCount) {
        return BrainBuilder()
            .withNeuronCount(neuronCount)
            .withRegionCount(3)
            .createSocialLearning()
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createBrainForTask(TaskType task, size_t neuronCount) {
        switch (task) {
            case TaskType::Navigation:
                return createNavigationBrain(neuronCount);
            case TaskType::ObjectRecognition:
                return createObjectRecognitionBrain(neuronCount);
            case TaskType::SequentialLearning:
                return createSequentialLearningBrain(neuronCount);
            case TaskType::DecisionMaking:
                return createDecisionMakingBrain(neuronCount);
            case TaskType::SocialInteraction:
                return createSocialInteractionBrain(neuronCount);
            default:
                return createSimpleBrain(neuronCount);
        }
    }
    
    std::shared_ptr<Brain> createCustomBrain(const std::string& configFile) {
        BrainBuilder builder;
        if (builder.importFromJSON(configFile)) {
            return builder.buildAndInitialize();
        }
        return nullptr;
    }
    
    std::shared_ptr<Brain> createBrainFromJSON(const std::string& jsonConfig) {
        // TODO: Parse JSON and create brain
        return nullptr;
    }
    
    // Task-specific factory methods
    std::shared_ptr<Brain> createNavigationBrain(size_t neuronCount) {
        return BrainBuilder()
            .forNavigationTask()
            .withNeuronCount(neuronCount)
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createObjectRecognitionBrain(size_t neuronCount) {
        return BrainBuilder()
            .forObjectRecognition()
            .withNeuronCount(neuronCount)
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createSequentialLearningBrain(size_t neuronCount) {
        return BrainBuilder()
            .forSequentialLearning()
            .withNeuronCount(neuronCount)
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createDecisionMakingBrain(size_t neuronCount) {
        return BrainBuilder()
            .forDecisionMaking()
            .withNeuronCount(neuronCount)
            .buildAndInitialize();
    }
    
    std::shared_ptr<Brain> createSocialInteractionBrain(size_t neuronCount) {
        return BrainBuilder()
            .forSocialInteraction()
            .withNeuronCount(neuronCount)
            .buildAndInitialize();
    }

} // namespace BrainFactory

// BuilderConfig implementation
BuilderConfig BuilderConfig::instance;

const BuilderConfig& BuilderConfig::getInstance() {
    return instance;
}

void BuilderConfig::setNeuronCount(size_t count) {
    instance.defaultNeuronCount = count;
}

void BuilderConfig::setRegionCount(size_t count) {
    instance.defaultRegionCount = count;
}

void BuilderConfig::setDefaultConnectionProbability(float prob) {
    instance.defaultConnectionProbability = prob;
}

void BuilderConfig::setDefaultTimestep(double timestep) {
    instance.defaultTimestep = timestep;
}

size_t BuilderConfig::getNeuronCount() const {
    return instance.defaultNeuronCount;
}

size_t BuilderConfig::getRegionCount() const {
    return instance.defaultRegionCount;
}

float BuilderConfig::getDefaultConnectionProbability() const {
    return instance.defaultConnectionProbability;
}

double BuilderConfig::getDefaultTimestep() const {
    return instance.defaultTimestep;
}

bool BuilderConfig::isValid() const {
    return defaultNeuronCount > 0 && 
           defaultRegionCount > 0 &&
           defaultConnectionProbability >= 0.0f && defaultConnectionProbability <= 1.0f &&
           defaultTimestep > 0.0;
}

void BuilderConfig::resetToDefaults() {
    defaultNeuronCount = 1000;
    defaultRegionCount = 1;
    defaultConnectionProbability = 0.1f;
    defaultTimestep = 0.001;
}

bool BuilderConfig::exportConfig(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "neuronCount=" << defaultNeuronCount << std::endl;
    file << "regionCount=" << defaultRegionCount << std::endl;
    file << "connectionProbability=" << defaultConnectionProbability << std::endl;
    file << "timestep=" << defaultTimestep << std::endl;
    
    file.close();
    return true;
}

bool BuilderConfig::importConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        std::string value;
        std::getline(iss, key, '=');
        std::getline(iss, value);
        
        if (key == "neuronCount") {
            defaultNeuronCount = std::stoul(value);
        } else if (key == "regionCount") {
            defaultRegionCount = std::stoul(value);
        } else if (key == "connectionProbability") {
            defaultConnectionProbability = std::stof(value);
        } else if (key == "timestep") {
            defaultTimestep = std::stod(value);
        }
    }
    
    file.close();
    return true;
}

// NeuronPopulationConfigurator implementation
NeuronPopulationConfigurator::NeuronPopulationConfigurator(BrainBuilder& builder) : builder(builder) {}

NeuronPopulationConfigurator::~NeuronPopulationConfigurator() = default;

BrainBuilder& NeuronPopulationConfigurator::sensory(size_t count) {
    builder.withSensoryNeuronCount(count);
    return builder;
}

BrainBuilder& NeuronPopulationConfigurator::internal(size_t count) {
    builder.withInternalNeuronCount(count);
    return builder;
}

BrainBuilder& NeuronPopulationConfigurator::motor(size_t count) {
    builder.withMotorNeuronCount(count);
    return builder;
}

BrainBuilder& NeuronPopulationConfigurator::mixed(size_t sensory, size_t internal, size_t motor) {
    return builder.withSensoryNeuronCount(sensory)
                  .withInternalNeuronCount(internal)
                  .withMotorNeuronCount(motor);
}

BrainBuilder& NeuronPopulationConfigurator::byTypeRatios(float sensory, float internal, float motor) {
    return builder.withExcitatoryNeuronRatio(sensory)
                  .withInhibitoryNeuronRatio(internal);
}

BrainBuilder& NeuronPopulationConfigurator::forTask(TaskType task) {
    switch (task) {
        case TaskType::Navigation:
            return builder.forNavigationTask();
        case TaskType::ObjectRecognition:
            return builder.forObjectRecognition();
        case TaskType::SequentialLearning:
            return builder.forSequentialLearning();
        case TaskType::DecisionMaking:
            return builder.forDecisionMaking();
        case TaskType::SocialInteraction:
            return builder.forSocialInteraction();
        default:
            return builder;
    }
}

std::string NeuronPopulationConfigurator::getConfiguration() const {
    std::stringstream ss;
    ss << "Sensory neurons: " << builder.pImpl->config.sensoryNeuronCount << std::endl;
    ss << "Internal neurons: " << builder.pImpl->config.internalNeuronCount << std::endl;
    ss << "Motor neurons: " << builder.pImpl->config.motorNeuronCount << std::endl;
    ss << "Excitatory ratio: " << builder.pImpl->config.excitatoryNeuronRatio << std::endl;
    ss << "Inhibitory ratio: " << builder.pImpl->config.inhibitoryNeuronRatio << std::endl;
    return ss.str();
}

// MemorySystemConfigurator implementation
MemorySystemConfigurator::MemorySystemConfigurator(BrainBuilder& builder) : builder(builder) {}

MemorySystemConfigurator::~MemorySystemConfigurator() = default;

BrainBuilder& MemorySystemConfigurator::workingMemory(size_t capacity, float recallSpeed) {
    builder.withWorkingMemoryCapacity(capacity);
    return builder;
}

BrainBuilder& MemorySystemConfigurator::episodicMemory(size_t maxEpisodes, float consolidationRate) {
    builder.withEpisodicMemoryCapacity(maxEpisodes);
    return builder;
}

BrainBuilder& MemorySystemConfigurator::associativeMemory(float associationStrength, size_t maxAssociations) {
    builder.withAssociativeMemoryEnabled(true);
    return builder;
}

BrainBuilder& MemorySystemConfigurator::memoryConsolidation(float consolidationRate, size_t consolidationInterval) {
    // TODO: Store these values in builder config
    return builder;
}

BrainBuilder& MemorySystemConfigurator::memoryReplay(size_t replayInterval, float replayStrength) {
    // TODO: Store these values in builder config
    return builder;
}

BrainBuilder& MemorySystemConfigurator::enableWorkingMemory(bool enabled) {
    // TODO: Store this in builder config
    return builder;
}

BrainBuilder& MemorySystemConfigurator::enableEpisodicMemory(bool enabled) {
    // TODO: Store this in builder config
    return builder;
}

BrainBuilder& MemorySystemConfigurator::enableAssociativeMemory(bool enabled) {
    builder.withAssociativeMemoryEnabled(enabled);
    return builder;
}

std::string MemorySystemConfigurator::getMemoryConfiguration() const {
    std::stringstream ss;
    ss << "Working memory: " << (builder.pImpl->config.workingMemoryCapacity > 0 ? std::to_string(builder.pImpl->config.workingMemoryCapacity) : "Auto") << std::endl;
    ss << "Episodic memory: " << builder.pImpl->config.episodicMemoryCapacity << std::endl;
    ss << "Associative memory enabled: " << (builder.pImpl->config.associativeMemoryEnabled ? "Yes" : "No") << std::endl;
    return ss.str();
}

// PlasticitySystemConfigurator implementation
PlasticitySystemConfigurator::PlasticitySystemConfigurator(BrainBuilder& builder) : builder(builder) {}

PlasticitySystemConfigurator::~PlasticitySystemConfigurator() = default;

BrainBuilder& PlasticitySystemConfigurator::stdp(float ltpWeight, float ltdWeight, float tau, float learningRate) {
    builder.withSTDPWeight(ltpWeight, ltdWeight, tau);
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::hebbian(float learningRate, float weightMax) {
    builder.withHebbianLearningEnabled(true);
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::structuralPlasticity(float synaptogenesisRate, float pruningRate, float maturationRate) {
    builder.withStructuralPlasticityEnabled(true);
    builder.withSynaptogenesisRate(synaptogenesisRate);
    builder.withPruningRate(pruningRate);
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::neuromodulatedPlasticity(float rewardModulation, float predictionErrorModulation) {
    builder.withDopamineLevel(rewardModulation)
          .withPredictionErrorLevel(predictionErrorModulation);
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::enableSTDP(bool enabled) {
    if (enabled) {
        builder.withSTDPWeight(builder.pImpl->config.stdpLTPWeight, builder.pImpl->config.stdpLTDWeight, builder.pImpl->config.stdpTau);
    }
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::enableHebbian(bool enabled) {
    builder.withHebbianLearningEnabled(enabled);
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::enableStructural(bool enabled) {
    builder.withStructuralPlasticityEnabled(enabled);
    return builder;
}

BrainBuilder& PlasticitySystemConfigurator::enableNeuromodulated(bool enabled) {
    if (enabled) {
        builder.withDopamineLevel(builder.pImpl->config.dopamineLevel)
              .withCuriosityLevel(builder.pImpl->config.curiosityLevel)
              .withPredictionErrorLevel(builder.pImpl->config.predictionErrorLevel);
    }
    return builder;
}

std::string PlasticitySystemConfigurator::getPlasticityConfiguration() const {
    std::stringstream ss;
    ss << "STDP: LTP=" << builder.pImpl->config.stdpLTPWeight << ", LTD=" << builder.pImpl->config.stdpLTDWeight << ", tau=" << builder.pImpl->config.stdpTau << std::endl;
    ss << "Hebbian: " << (builder.pImpl->config.hebbianEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Structural: " << (builder.pImpl->config.structuralPlasticityEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Neuromodulated: Dopamine=" << builder.pImpl->config.dopamineLevel << ", Curiosity=" << builder.pImpl->config.curiosityLevel << std::endl;
    return ss.str();
}

} // namespace nlm

// BrainBuilder.cpp implementation ends here