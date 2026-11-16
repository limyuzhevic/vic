#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <exception>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class SensoryPercept;
}

// Custom exception classes for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class NLMInitializationError : public NLMError {
public:
    explicit NLMInitializationError(const std::string& message) : NLMError("Initialization failed: " + message) {}
};

class NLMSimulationError : public NLMError {
public:
    explicit NLMSimulationError(const std::string& message) : NLMError("Simulation failed: " + message) {}
};

class NLMConfigError : public NLMError {
public:
    explicit NLMConfigError(const std::string& message) : NLMError("Configuration error: " + message) {}
};

// Context manager for Brain simulation
class BrainContext {
private:
    std::shared_ptr<nlm::Brain> brain_;
    bool auto_cleanup_;
public:
    BrainContext(std::shared_ptr<nlm::Brain> brain, bool auto_cleanup = true) 
        : brain_(brain), auto_cleanup_(auto_cleanup) {
        if (!brain_) {
            throw NLMInitializationError("Cannot create context with null brain");
        }
    }
    
    ~BrainContext() {
        if (auto_cleanup_ && brain_) {
            try {
                brain_->reset();
            } catch (...) {
                // Ignore errors during cleanup
            }
        }
    }
    
    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;
    
    BrainContext(BrainContext&& other) noexcept : brain_(std::move(other.brain_)), auto_cleanup_(other.auto_cleanup_) {
        other.brain_ = nullptr;
    }
    
    BrainContext& operator=(BrainContext&& other) noexcept {
        if (this != &other) {
            if (auto_cleanup_ && brain_) {
                brain_->reset();
            }
            brain_ = std::move(other.brain_);
            auto_cleanup_ = other.auto_cleanup_;
            other.brain_ = nullptr;
        }
        return *this;
    }
    
    std::shared_ptr<nlm::Brain> getBrain() const { return brain_; }
    
    void setAutoCleanup(bool auto_cleanup) { auto_cleanup_ = auto_cleanup; }
    
    bool getAutoCleanup() const { return auto_cleanup_; }
    
    // Context manager protocol
    void __enter__() {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    void __exit__(int, const char*, const char*, const char*) {
        if (auto_cleanup_ && brain_) {
            brain_->reset();
        }
    }
    
    // Simulation step with context
    void step_with_context(uint64_t current_step, double current_time = 0.0) {
        if (!brain_) {
            throw NLMSimulationError("Brain is not available in context");
        }
        brain_->step(current_step, current_time);
    }
};

// Enhanced configuration management
class ConfigManager {
private:
    std::shared_ptr<nlm::Config> config_;
public:
    ConfigManager() : config_(std::make_shared<nlm::Config>()) {}
    ConfigManager(const std::string& filepath) : config_(std::make_shared<nlm::Config>()) {
        if (!loadFromFile(filepath)) {
            throw NLMConfigError("Failed to load configuration from: " + filepath);
        }
    }
    
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    bool loadFromArgs(const std::vector<std::string>& args) {
        return config_->loadFromArgs(static_cast<int>(args.size()), 
                                    const_cast<char**>(&args[0]));
    }
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const {
        auto value = config_->get<T>(key);
        if (value.has_value()) {
            return value.value();
        }
        return defaultValue;
    }
    
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_->set(key, value, nlm::ConfigSource::Runtime);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    void clear() {
        config_->clear();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    std::shared_ptr<nlm::Config> getConfig() const { return config_; }
    
    // Enhanced configuration methods
    void setString(const std::string& key, const std::string& value) {
        config_->set(key, value, nlm::ConfigSource::Runtime);
    }
    
    void setInt(const std::string& key, int64_t value) {
        config_->set(key, value, nlm::ConfigSource::Runtime);
    }
    
    void setDouble(const std::string& key, double value) {
        config_->set(key, value, nlm::ConfigSource::Runtime);
    }
    
    void setBool(const std::string& key, bool value) {
        config_->set(key, value, nlm::ConfigSource::Runtime);
    }
    
    // Batch configuration
    void applyBatch(const std::map<std::string, std::string>& values) {
        for (const auto& [key, value] : values) {
            config_->set(key, value, nlm::ConfigSource::Runtime);
        }
    }
    
    // Validation
    bool validate() const {
        std::vector<std::string> errors = config_->getKeys(); // Simplified validation
        return errors.empty();
    }
};

// Enhanced brain wrapper with Pythonic interface
class EnhancedBrain {
private:
    std::shared_ptr<nlm::Brain> brain_;
    uint64_t current_step_;
    double current_time_;
public:
    EnhancedBrain(std::shared_ptr<nlm::Config> config) 
        : brain_(std::make_shared<nlm::Brain>(config)), 
          current_step_(0), current_time_(0.0) {
        if (!brain_->initialize()) {
            throw NLMInitializationError("Failed to initialize brain");
        }
    }
    
    ~EnhancedBrain() {
        try {
            brain_->reset();
        } catch (...) {
            // Ignore errors during cleanup
        }
    }
    
    EnhancedBrain(const EnhancedBrain&) = delete;
    EnhancedBrain& operator=(const EnhancedBrain&) = delete;
    
    EnhancedBrain(EnhancedBrain&& other) noexcept 
        : brain_(std::move(other.brain_)), 
          current_step_(other.current_step_), 
          current_time_(other.current_time_) {
        other.brain_ = nullptr;
    }
    
    EnhancedBrain& operator=(EnhancedBrain&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
            current_step_ = other.current_step_;
            current_time_ = other.current_time_;
            other.brain_ = nullptr;
        }
        return *this;
    }
    
    // Context manager support
    std::shared_ptr<BrainContext> createContext() {
        return std::make_shared<BrainContext>(brain_);
    }
    
    // Enhanced simulation methods
    void step(double timestep = 0.001, bool update_time = true) {
        if (!brain_) {
            throw NLMSimulationError("Brain is not initialized");
        }
        
        if (update_time) {
            current_time_ += timestep;
        }
        
        brain_->step(current_step_, current_time_);
        current_step_++;
    }
    
    void step(uint64_t step, double time) {
        if (!brain_) {
            throw NLMSimulationError("Brain is not initialized");
        }
        
        brain_->step(step, time);
        current_step_ = step;
        current_time_ = time;
    }
    
    void reset() {
        if (brain_) {
            brain_->reset();
        }
        current_step_ = 0;
        current_time_ = 0.0;
    }
    
    // Enhanced sensory and motor methods
    void injectSensory(const std::vector<float>& data) {
        if (!brain_) {
            throw NLMSimulationError("Brain is not initialized");
        }
        
        // Convert vector to appropriate sensory input type
        // This is a simplified version - actual implementation depends on sensor type
        brain_->injectCurrentToNeurons(nlm::NeuronType::Sensory, data.size() * 30.0f);
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    uint64_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    uint64_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    uint64_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    uint64_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }
    
    uint64_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    // Brain state management
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "EnhancedBrain Status:" << std::endl;
        oss << "  Step: " << current_step_ << std::endl;
        oss << "  Time: " << current_time_ << "s" << std::endl;
        oss << "  Neurons: " << getTotalNeuronCount() << std::endl;
        oss << "  Synapses: " << getTotalSynapseCount() << std::endl;
        oss << "  Spikes: " << getTotalSpikeCount() << std::endl;
        oss << "  Firing: " << getFiringNeuronCount() << std::endl;
        oss << "  Avg Rate: " << getAverageFiringRate() << "/s" << std::endl;
        return oss.str();
    }
    
    // Save/load
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    std::shared_ptr<nlm::Config> getConfig() const {
        return brain_->getConfig();
    }
    
    // Performance monitoring
    double getSimulationSpeed(double reference_time) const {
        return (current_step_ * 0.001) / std::max(reference_time, 0.001);
    }
    
    // Advanced neural operations
    double getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(nlm::DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
};

// Enhanced agent brain wrapper
class EnhancedAgentBrain {
private:
    std::shared_ptr<nlm::AgentBrain> agent_brain_;
    std::shared_ptr<EnhancedBrain> enhanced_brain_;
public:
    EnhancedAgentBrain(std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world) 
        : agent_brain_(std::make_shared<nlm::AgentBrain>(brain)), 
          enhanced_brain_(std::make_shared<EnhancedBrain>(std::static_pointer_cast<nlm::Config>(brain->getConfig()))) {
        agent_brain_->initialize(*world);
    }
    
    ~EnhancedAgentBrain() {
        try {
            agent_brain_->reset();
        } catch (...) {
            // Ignore errors during cleanup
        }
    }
    
    EnhancedAgentBrain(const EnhancedAgentBrain&) = delete;
    EnhancedAgentBrain& operator=(const EnhancedAgentBrain&) = delete;
    
    EnhancedAgentBrain(EnhancedAgentBrain&& other) noexcept 
        : agent_brain_(std::move(other.agent_brain_)), 
          enhanced_brain_(std::move(other.enhanced_brain_)) {
    }
    
    EnhancedAgentBrain& operator=(EnhancedAgentBrain&& other) noexcept {
        if (this != &other) {
            agent_brain_ = std::move(other.agent_brain_);
            enhanced_brain_ = std::move(other.enhanced_brain_);
        }
        return *this;
    }
    
    // Enhanced interface
    void processSensoryInput(const nlm::SensoryPercept& percept) {
        agent_brain_->processSensoryInput(percept);
        // Update enhanced brain simulation
        enhanced_brain_->step();
    }
    
    void decodeMotorCommand(std::vector<float>& output) {
        // Simplified - actual motor decoding depends on specific implementation
        float firing_rate = enhanced_brain_->getAverageFiringRate();
        output.clear();
        output.push_back(firing_rate);
    }
    
    void applyRewardModulation(float reward, float predicted_reward) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    void updateDevelopment(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Configuration and control
    void enableRewardModulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }
    
    bool isRewardModulationEnabled() const {
        return agent_brain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agent_brain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agent_brain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agent_brain_->isCuriosityEnabled();
    }
    
    // Get underlying brain
    std::shared_ptr<EnhancedBrain> getEnhancedBrain() const { return enhanced_brain_; }
    std::shared_ptr<nlm::Brain> getBrain() const { return enhanced_brain_->getBrain(); }
    std::shared_ptr<nlm::SimpleWorld> getWorld() const {
        // This would require world to be passed to constructor
        return nullptr; // Simplified
    }
    
    // Enhanced metrics
    float getNeuromodulationLevel() const {
        return agent_brain_->getNeuromodulationLevel();
    }
    
    float getCuriosityLevel() const {
        return agent_brain_->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agent_brain_->getNoveltyLevel();
    }
    
    float getPredictionError() const {
        return agent_brain_->getPredictionError();
    }
    
    nlm::DevelopmentalStage getDevelopmentalStage() const {
        return agent_brain_->getDevelopmentalStage();
    }
    
    // Learning and adaptation
    void reset() {
        agent_brain_->reset();
        enhanced_brain_->reset();
    }
    
    // State serialization
    bool saveState(const std::string& filepath) {
        return enhanced_brain_->save(filepath);
    }
    
    bool loadState(const std::string& filepath) {
        return enhanced_brain_->load(filepath);
    }
    
    std::string getState() const {
        std::ostringstream oss;
        oss << "EnhancedAgentBrain State:" << std::endl;
        oss << "  Neuromodulation: " << getNeuromodulationLevel() << std::endl;
        oss << "  Curiosity: " << getCuriosityLevel() << std::endl;
        oss << "  Novelty: " << getNoveltyLevel() << std::endl;
        oss << "  Prediction Error: " << getPredictionError() << std::endl;
        oss << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
        oss << "  Brain: " << enhanced_brain_->getStatus();
        return oss.str();
    }
};

// Simplified world wrapper
class EnhancedWorld {
private:
    std::shared_ptr<nlm::SimpleWorld> world_;
    uint64_t simulation_step_;
public:
    EnhancedWorld() : world_(std::make_shared<nlm::SimpleWorld>()), simulation_step_(0) {}
    
    EnhancedWorld(float width, float height, int vision_width, int vision_height) 
        : world_(std::make_shared<nlm::SimpleWorld>()), simulation_step_(0) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    EnhancedWorld(const EnhancedWorld&) = delete;
    EnhancedWorld& operator=(const EnhancedWorld&) = delete;
    
    EnhancedWorld(EnhancedWorld&& other) noexcept 
        : world_(std::move(other.world_)), 
          simulation_step_(other.simulation_step_) {
    }
    
    EnhancedWorld& operator=(EnhancedWorld&& other) noexcept {
        if (this != &other) {
            world_ = std::move(other.world_);
            simulation_step_ = other.simulation_step_;
        }
        return *this;
    }
    
    // Configuration
    void configure(float width, float height, int vision_width, int vision_height) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    void reset() {
        world_->reset();
        simulation_step_ = 0;
    }
    
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Simulation control
    void update(double timestep) {
        world_->update(timestep);
        simulation_step_++;
    }
    
    // Sensory and action
    nlm::SensoryPercept getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    bool applyMotorCommand(nlm::MotorCommand cmd, double current_time) {
        return world_->applyMotorCommand(cmd, current_time);
    }
    
    // World state
    nlm::AgentBody getAgentBody() const {
        return world_->getAgentBody();
    }
    
    float getWidth() const { return world_->getWidth(); }
    float getHeight() const { return world_->getHeight(); }
    
    uint64_t getSimulationStep() const { return simulation_step_; }
    
    double getSimulationTime() const { return world_->getSimulationTime(); }
    
    // Object management
    void addObject(const nlm::WorldObject& obj) {
        world_->addObject(obj);
    }
    
    void removeObject(float x, float y) {
        world_->removeObject(x, y);
    }
    
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Configuration
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint64_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    // World metrics
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "EnhancedWorld Status:" << std::endl;
        oss << "  Width: " << getWidth() << std::endl;
        oss << "  Height: " << getHeight() << std::endl;
        oss << "  Simulation Step: " << getSimulationStep() << std::endl;
        oss << "  Simulation Time: " << getSimulationTime() << "s" << std::endl;
        oss << "  Agent Position: (" << getAgentBody().x << ", " << getAgentBody().y << ")" << std::endl;
        return oss.str();
    }
};