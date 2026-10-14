// BrainFactory Implementation
// Provides factory methods for creating NLM brain components and integrated brains

#include "BrainFactory.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace nlm {

// Initialize static members
BrainComponentFactory& BrainComponentFactory::getInstance() {
    static BrainComponentFactory instance;
    static bool initialized = false;
    
    if (!initialized) {
        instance.initializeDefaultFactories();
        initialized = true;
    }
    
    return instance;
}

BrainComponentFactory::BrainComponentFactory() {
    // Initialize type names
    typeNames_[ComponentType::Neuron] = "Neuron";
    typeNames_[ComponentType::Synapse] = "Synapse";
    typeNames_[ComponentType::NeuralRegion] = "NeuralRegion";
    typeNames_[ComponentType::NeuralPopulation] = "NeuralPopulation";
    typeNames_[ComponentType::PlasticityRule] = "PlasticityRule";
    typeNames_[ComponentType::STDP] = "STDP";
    typeNames_[ComponentType::Hebbian] = "Hebbian";
    typeNames_[ComponentType::StructuralPlasticity] = "StructuralPlasticity";
    typeNames_[ComponentType::Neuromodulator] = "Neuromodulator";
    typeNames_[ComponentType::Dopamine] = "Dopamine";
    typeNames_[ComponentType::Curiosity] = "Curiosity";
    typeNames_[ComponentType::Novelty] = "Novelty";
    typeNames_[ComponentType::PredictionError] = "PredictionError";
    typeNames_[ComponentType::WorkingMemory] = "WorkingMemory";
    typeNames_[ComponentType::EpisodicMemory] = "EpisodicMemory";
    typeNames_[ComponentType::AssociativeMemory] = "AssociativeMemory";
    typeNames_[ComponentType::PredictionSystem] = "PredictionSystem";
    typeNames_[ComponentType::NeuralPlanner] = "NeuralPlanner";
    typeNames_[ComponentType::ConceptFormation] = "ConceptFormation";
    typeNames_[ComponentType::AttentionalSelection] = "AttentionalSelection";
    typeNames_[ComponentType::DevelopmentSystem] = "DevelopmentSystem";
    typeNames_[ComponentType::Synaptogenesis] = "Synaptogenesis";
    typeNames_[ComponentType::Pruning] = "Pruning";
    typeNames_[ComponentType::Maturation] = "Maturation";
}

BrainComponentFactory::~BrainComponentFactory() {
    factories_.clear();
}

void BrainComponentFactory::registerFactory(std::unique_ptr<IComponentFactory> factory) {
    if (factory) {
        factories_.push_back(std::move(factory));
        NLM_LOG_INFO("Registered component factory: " + factory->getName());
    }
}

std::string BrainComponentFactory::getComponentName(ComponentType type) const {
    auto it = typeNames_.find(type);
    if (it != typeNames_.end()) {
        return it->second;
    }
    return "Unknown";
}

bool BrainComponentFactory::supportsComponent(ComponentType type) const {
    for (const auto& factory : factories_) {
        if (factory->canCreate(type)) {
            return true;
        }
    }
    return false;
}

std::vector<ComponentType> BrainComponentFactory::getSupportedTypes() const {
    std::vector<ComponentType> types;
    for (const auto& factory : factories_) {
        if (factory->canCreate(ComponentType::Neuron)) types.push_back(ComponentType::Neuron);
        if (factory->canCreate(ComponentType::Synapse)) types.push_back(ComponentType::Synapse);
        if (factory->canCreate(ComponentType::NeuralRegion)) types.push_back(ComponentType::NeuralRegion);
        if (factory->canCreate(ComponentType::NeuralPopulation)) types.push_back(ComponentType::NeuralPopulation);
        if (factory->canCreate(ComponentType::PlasticityRule)) types.push_back(ComponentType::PlasticityRule);
        if (factory->canCreate(ComponentType::STDP)) types.push_back(ComponentType::STDP);
        if (factory->canCreate(ComponentType::Hebbian)) types.push_back(ComponentType::Hebbian);
        if (factory->canCreate(ComponentType::StructuralPlasticity)) types.push_back(ComponentType::StructuralPlasticity);
        if (factory->canCreate(ComponentType::Neuromodulator)) types.push_back(ComponentType::Neuromodulator);
        if (factory->canCreate(ComponentType::Dopamine)) types.push_back(ComponentType::Dopamine);
        if (factory->canCreate(ComponentType::Curiosity)) types.push_back(ComponentType::Curiosity);
        if (factory->canCreate(ComponentType::Novelty)) types.push_back(ComponentType::Novelty);
        if (factory->canCreate(ComponentType::PredictionError)) types.push_back(ComponentType::PredictionError);
        if (factory->canCreate(ComponentType::WorkingMemory)) types.push_back(ComponentType::WorkingMemory);
        if (factory->canCreate(ComponentType::EpisodicMemory)) types.push_back(ComponentType::EpisodicMemory);
        if (factory->canCreate(ComponentType::AssociativeMemory)) types.push_back(ComponentType::AssociativeMemory);
        if (factory->canCreate(ComponentType::PredictionSystem)) types.push_back(ComponentType::PredictionSystem);
        if (factory->canCreate(ComponentType::NeuralPlanner)) types.push_back(ComponentType::NeuralPlanner);
        if (factory->canCreate(ComponentType::ConceptFormation)) types.push_back(ComponentType::ConceptFormation);
        if (factory->canCreate(ComponentType::AttentionalSelection)) types.push_back(ComponentType::AttentionalSelection);
        if (factory->canCreate(ComponentType::DevelopmentSystem)) types.push_back(ComponentType::DevelopmentSystem);
        if (factory->canCreate(ComponentType::Synaptogenesis)) types.push_back(ComponentType::Synaptogenesis);
        if (factory->canCreate(ComponentType::Pruning)) types.push_back(ComponentType::Pruning);
        if (factory->canCreate(ComponentType::Maturation)) types.push_back(ComponentType::Maturation);
    }
    
    // Remove duplicates
    std::sort(types.begin(), types.end());
    types.erase(std::unique(types.begin(), types.end()), types.end());
    
    return types;
}

void BrainComponentFactory::initializeDefaultFactories() {
    // Register default factories
    registerFactory(std::make_unique<NeuronFactory>());
    registerFactory(std::make_unique<SynapseFactory>());
    registerFactory(std::make_unique<NeuralRegionFactory>());
    registerFactory(std::make_unique<NeuralPopulationFactory>());
    registerFactory(std::make_unique<PlasticityRuleFactory>());
    registerFactory(std::make_unique<STDPFactory>());
    registerFactory(std::make_unique<HebbianFactory>());
    registerFactory(std::make_unique<StructuralPlasticityFactory>());
    registerFactory(std::make_unique<NeuromodulatorFactory>());
    registerFactory(std::make_unique<DopamineFactory>());
    registerFactory(std::make_unique<CuriosityFactory>());
    registerFactory(std::make_unique<NoveltyFactory>());
    registerFactory(std::make_unique<PredictionErrorFactory>());
    registerFactory(std::make_unique<MemoryFactory>());
    registerFactory(std::make_unique<PredictionSystemFactory>());
    registerFactory(std::make_unique<CognitionFactory>());
    registerFactory(std::make_unique<DevelopmentFactory>());
    
    NLM_LOG_INFO("Initialized default component factories");
}

// ComponentCreator implementation
BrainComponentFactory::BrainComponentFactory& BrainComponentFactory::getBrainComponentFactory() {
    return getInstance();
}

BrainProfileManager& FactoryUtils::getBrainProfileManager() {
    static BrainProfileManager instance;
    return instance;
}

ComponentCreator& FactoryUtils::getComponentCreator() {
    static ComponentCreator instance;
    return instance;
}

// ComponentCreator implementation
ComponentCreator::ComponentCreator() {
    // Initialize default brain configurations
    Config basicConfig;
    basicConfig.set("neuron_count", 1000, ConfigSource::Default);
    basicConfig.set("region_count", 2, ConfigSource::Default);
    basicConfig.set("connection_probability", 0.15, ConfigSource::Default);
    basicConfig.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    basicConfig.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    registerBrainConfig("basic", basicConfig);
    
    Config advancedConfig;
    advancedConfig.set("neuron_count", 5000, ConfigSource::Default);
    advancedConfig.set("region_count", 4, ConfigSource::Default);
    advancedConfig.set("connection_probability", 0.1f, ConfigSource::Default);
    advancedConfig.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    advancedConfig.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    advancedConfig.set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    advancedConfig.set("pruning_rate", 0.00002f, ConfigSource::Default);
    
    registerBrainConfig("advanced", advancedConfig);
    
    Config demoConfig;
    demoConfig.set("neuron_count", 200, ConfigSource::Default);
    demoConfig.set("region_count", 1, ConfigSource::Default);
    demoConfig.set("connection_probability", 0.2f, ConfigSource::Default);
    demoConfig.set("max_steps", 500, ConfigSource::Default);
    
    registerBrainConfig("demo", demoConfig);
    
    NLM_LOG_INFO("Initialized default brain configurations");
}

ComponentCreator::~ComponentCreator() {
    brainConfigs_.clear();
}

std::shared_ptr<Brain> ComponentCreator::createBrain(const Config& config) {
    return createIntegratedBrain(config);
}

std::shared_ptr<Brain> ComponentCreator::createIntegratedBrain(const Config& config) {
    if (!isConfigValid(config)) {
        throw std::runtime_error("Invalid brain configuration");
    }
    
    // Create brain with configuration
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    NLM_LOG_INFO("Created integrated brain successfully");
    return brain;
}

std::shared_ptr<Brain> ComponentCreator::createBrain(const std::string& configName) {
    auto it = brainConfigs_.find(configName);
    if (it == brainConfigs_.end()) {
        throw std::runtime_error("Brain configuration not found: " + configName);
    }
    
    return createBrain(it->second);
}

std::shared_ptr<Brain> ComponentCreator::createCustomBrain(const Config& config, 
                                                          const std::vector<ComponentType>& componentTypes) {
    // Create a brain with specific components
    auto brain = createBrain(config);
    
    // TODO: Add custom component configuration
    
    return brain;
}

std::vector<ComponentType> ComponentCreator::getBrainComponents(const Config& config) {
    std::vector<ComponentType> components;
    
    // Determine which components are needed based on configuration
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    auto regionCount = config.getOr<size_t>("region_count", 0);
    
    if (neuronCount > 0) components.push_back(ComponentType::Neuron);
    if (regionCount > 0) components.push_back(ComponentType::NeuralRegion);
    if (regionCount > 1) components.push_back(ComponentType::NeuralPopulation);
    
    if (config.has("stdp_ltp_weight")) components.push_back(ComponentType::STDP);
    if (config.has("stdp_ltd_weight")) components.push_back(ComponentType::Hebbian);
    if (config.has("synaptogenesis_rate")) components.push_back(ComponentType::StructuralPlasticity);
    if (config.has("dopamine_baseline")) components.push_back(ComponentType::Dopamine);
    if (config.has("curiosity_sensitivity")) components.push_back(ComponentType::Curiosity);
    if (config.has("working_memory_size")) components.push_back(ComponentType::WorkingMemory);
    if (config.has("episodic_memory_capacity")) components.push_back(ComponentType::EpisodicMemory);
    if (config.has("prediction_system")) components.push_back(ComponentType::PredictionSystem);
    if (config.has("development_system")) components.push_back(ComponentType::DevelopmentSystem);
    
    return components;
}

bool ComponentCreator::isConfigValid(const Config& config) {
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    auto regionCount = config.getOr<size_t>("region_count", 0);
    auto connectionProb = config.getOr<float>("connection_probability", 0.0f);
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    
    return neuronCount > 0 && regionCount > 0 && 
           connectionProb >= 0.0f && connectionProb <= 1.0f &&
           timestep > 0.0;
}

Config ComponentCreator::getDefaultConfig(ComponentType componentType) {
    Config config;
    
    switch (componentType) {
        case ComponentType::Neuron:
            config.set("neuron_count", 1000, ConfigSource::Default);
            config.set("simulation_timestep", 0.001, ConfigSource::Default);
            break;
        case ComponentType::NeuralRegion:
            config.set("region_count", 2, ConfigSource::Default);
            break;
        case ComponentType::NeuralPopulation:
            config.set("connection_probability", 0.15, ConfigSource::Default);
            break;
        case ComponentType::STDP:
            config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
            config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
            break;
        case ComponentType::Hebbian:
            config.set("hebbian_learning_rate", 0.001f, ConfigSource::Default);
            break;
        case ComponentType::StructuralPlasticity:
            config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            config.set("pruning_rate", 0.00001f, ConfigSource::Default);
            break;
        case ComponentType::Dopamine:
            config.set("dopamine_baseline", 0.5f, ConfigSource::Default);
            break;
        case ComponentType::Curiosity:
            config.set("curiosity_sensitivity", 0.3f, ConfigSource::Default);
            break;
        case ComponentType::WorkingMemory:
            config.set("working_memory_size", 100, ConfigSource::Default);
            break;
        case ComponentType::EpisodicMemory:
            config.set("episodic_memory_capacity", 50, ConfigSource::Default);
            break;
        default:
            break;
    }
    
    return config;
}

void ComponentCreator::registerBrainConfig(const std::string& name, const Config& config) {
    brainConfigs_[name] = config;
    NLM_LOG_INFO("Registered brain configuration: " + name);
}

std::vector<std::string> ComponentCreator::getRegisteredBrainConfigs() {
    std::vector<std::string> names;
    for (const auto& pair : brainConfigs_) {
        names.push_back(pair.first);
    }
    return names;
}

std::shared_ptr<Brain> ComponentCreator::createBrainFromName(const std::string& configName) {
    return createBrain(configName);
}

// BrainProfileManager implementation
BrainProfileManager::BrainProfileManager() {
    // Create default profiles
    createDefaultProfiles();
}

BrainProfileManager::~BrainProfileManager() {
    profiles_.clear();
}

void BrainProfileManager::registerProfile(const BrainProfile& profile) {
    profiles_[profile.name] = profile;
    NLM_LOG_INFO("Registered brain profile: " + profile.name);
}

BrainProfile BrainProfileManager::getProfile(const std::string& name) const {
    auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        throw std::runtime_error("Brain profile not found: " + name);
    }
    return it->second;
}

std::vector<std::string> BrainProfileManager::getProfileNames() const {
    std::vector<std::string> names;
    for (const auto& pair : profiles_) {
        names.push_back(pair.first);
    }
    return names;
}

BrainProfile BrainProfileManager::findBestProfile(const Config& config) const {
    BrainProfile bestProfile;
    float bestScore = 0.0f;
    
    for (const auto& pair : profiles_) {
        const auto& profile = pair.second;
        float score = calculateProfileScore(profile, config);
        
        if (score > bestScore) {
            bestScore = score;
            bestProfile = profile;
        }
    }
    
    return bestProfile;
}

std::vector<BrainProfile> BrainProfileManager::getProfilesByUseCase(const std::string& useCase) const {
    std::vector<BrainProfile> matchingProfiles;
    
    for (const auto& pair : profiles_) {
        if (pair.second.useCase == useCase) {
            matchingProfiles.push_back(pair.second);
        }
    }
    
    return matchingProfiles;
}

std::vector<BrainProfile> BrainProfileManager::getProfilesByPerformance(float minScore, float maxScore) const {
    std::vector<BrainProfile> matchingProfiles;
    
    for (const auto& pair : profiles_) {
        if (pair.second.expectedPerformanceScore >= minScore && 
            pair.second.expectedPerformanceScore <= maxScore) {
            matchingProfiles.push_back(pair.second);
        }
    }
    
    return matchingProfiles;
}

bool BrainProfileManager::deleteProfile(const std::string& name) {
    auto it = profiles_.find(name);
    if (it != profiles_.end()) {
        profiles_.erase(it);
        NLM_LOG_INFO("Deleted brain profile: " + name);
        return true;
    }
    return false;
}

bool BrainProfileManager::updateProfile(const std::string& name, const BrainProfile& newProfile) {
    auto it = profiles_.find(name);
    if (it != profiles_.end()) {
        profiles_[name] = newProfile;
        NLM_LOG_INFO("Updated brain profile: " + name);
        return true;
    }
    return false;
}

void BrainProfileManager::createDefaultProfiles() {
    // Research profile
    {
        Config config;
        config.set("neuron_count", 5000, ConfigSource::Default);
        config.set("region_count", 4, ConfigSource::Default);
        config.set("connection_probability", 0.1f, ConfigSource::Default);
        config.set("max_steps", 20000, ConfigSource::Default);
        
        BrainProfile profile;
        profile.name = "research";
        profile.description = "Research-grade neural simulation with high complexity";
        profile.config = config;
        profile.expectedPerformanceScore = 0.95f;
        profile.useCase = "research";
        
        registerProfile(profile);
    }
    
    // Demo profile
    {
        Config config;
        config.set("neuron_count", 200, ConfigSource::Default);
        config.set("region_count", 1, ConfigSource::Default);
        config.set("connection_probability", 0.2f, ConfigSource::Default);
        config.set("max_steps", 500, ConfigSource::Default);
        
        BrainProfile profile;
        profile.name = "demo";
        profile.description = "Demo neural simulation with moderate complexity";
        profile.config = config;
        profile.expectedPerformanceScore = 0.70f;
        profile.useCase = "demo";
        
        registerProfile(profile);
    }
    
    // Learning profile
    {
        Config config;
        config.set("neuron_count", 1000, ConfigSource::Default);
        config.set("region_count", 2, ConfigSource::Default);
        config.set("connection_probability", 0.15f, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config.set("pruning_rate", 0.00002f, ConfigSource::Default);
        
        BrainProfile profile;
        profile.name = "learning";
        profile.description = "Neural simulation optimized for learning and plasticity";
        profile.config = config;
        profile.expectedPerformanceScore = 0.85f;
        profile.useCase = "learning";
        
        registerProfile(profile);
    }
    
    // Production profile
    {
        Config config;
        config.set("neuron_count", 2000, ConfigSource::Default);
        config.set("region_count", 3, ConfigSource::Default);
        config.set("connection_probability", 0.12f, ConfigSource::Default);
        config.set("max_steps", 10000, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.013f, ConfigSource::Default);
        
        BrainProfile profile;
        profile.name = "production";
        profile.description = "Production-grade neural simulation with balanced performance";
        profile.config = config;
        profile.expectedPerformanceScore = 0.90f;
        profile.useCase = "production";
        
        registerProfile(profile);
    }
}

float BrainProfileManager::calculateProfileScore(const BrainProfile& profile, const Config& config) const {
    float score = 0.0f;
    
    // Compare neuron count
    auto profileNeurons = profile.config.getOr<size_t>("neuron_count", 0);
    auto configNeurons = config.getOr<size_t>("neuron_count", 0);
    if (profileNeurons > 0 && configNeurons > 0) {
        float ratio = static_cast<float>(configNeurons) / profileNeurons;
        score += std::min(0.4f, 0.2f * std::min(1.0f, ratio));
    }
    
    // Compare connection probability
    auto profileConn = profile.config.getOr<float>("connection_probability", 0.0f);
    auto configConn = config.getOr<float>("connection_probability", 0.0f);
    if (profileConn > 0 && configConn > 0) {
        float diff = std::abs(configConn - profileConn);
        score += std::max(0.0f, 0.3f - diff);
    }
    
    // Compare timestep
    auto profileStep = profile.config.getOr<double>("simulation_timestep", 0.0);
    auto configStep = config.getOr<double>("simulation_timestep", 0.0);
    if (profileStep > 0 && configStep > 0) {
        float ratio = std::min(configStep / profileStep, 10.0f);
        score += std::min(0.3f, 0.1f * (ratio - 1.0f));
    }
    
    return std::min(1.0f, score);
}

// FactoryUtils implementation
void FactoryUtils::initializeDefaultFactories() {
    getBrainComponentFactory().initializeDefaultFactories();
    getBrainProfileManager(); // Initialize profile manager
    getComponentCreator();    // Initialize component creator
}

void FactoryUtils::cleanup() {
    // Clean up factory resources
    // Implementation details would go here
}

bool FactoryUtils::validateConfigForComponent(ComponentType type, const Config& config) {
    // Validate configuration based on component type
    // Implementation details would go here
    return true;
}

Config FactoryUtils::getComponentDefaults(ComponentType type) {
    return ComponentCreator::getDefaultConfig(type);
}

Config FactoryUtils::mergeConfigs(const Config& baseConfig, const Config& overrideConfig) {
    // Merge two configurations
    Config merged = baseConfig;
    // Implementation details would go here
    return merged;
}

Config FactoryUtils::configFromString(const std::string& configString) {
    // Parse configuration from string
    Config config;
    // Implementation details would go here
    return config;
}

Config FactoryUtils::configFromPairs(const std::vector<std::pair<std::string, std::string>>& pairs) {
    // Parse configuration from name-value pairs
    Config config;
    // Implementation details would go here
    return config;
}

std::vector<ComponentType> FactoryUtils::getAvailableComponentTypes() {
    return getBrainComponentFactory().getSupportedTypes();
}

std::unique_ptr<IComponentFactory> FactoryUtils::getFactory(ComponentType type) {
    for (const auto& factory : getBrainComponentFactory().factories_) {
        if (factory->canCreate(type)) {
            return std::make_unique<IComponentFactory>(*factory);
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<IComponentFactory>> FactoryUtils::getAllFactories() {
    return getBrainComponentFactory().factories_;
}

// Static member definitions
std::map<std::string, Config> ComponentCreator::brainConfigs_;

} // namespace nlm
