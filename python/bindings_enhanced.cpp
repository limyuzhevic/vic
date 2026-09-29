// NLM Enhanced Python Bindings
// Advanced Python bindings for NLM (Neural Learning Machine)
// Provides comprehensive, Pythonic interface to the C++ neural simulation framework
// Includes enhanced convenience methods, experiment support, and visualization tools

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

// Include enhanced NLM headers
#include "cli/NLMCLI.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python interfaces with improved documentation and type hints

// Configuration Class with Pythonic API
class ConfigWrapper {
public:
    ConfigWrapper() : config_(std::make_shared<Config>()) {}
    
    // File operations with error handling
    bool loadFromFile(const std::string& filepath) {
        try {
            return config_->loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw py::value_error("Failed to load config from " + filepath + ": " + e.what());
        }
    }
    
    bool saveToFile(const std::string& filepath) {
        try {
            return config_->saveToFile(filepath);
        } catch (const std::exception& e) {
            throw py::value_error("Failed to save config to " + filepath + ": " + e.what());
        }
    }
    
    // Pythonic getter/setter methods
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_->set(key, value);
    }
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Chainable configuration methods
    ConfigWrapper& setNeuronCount(size_t count) {
        config_->set("neuron_count", static_cast<int64_t>(count));
        return *this;
    }
    
    ConfigWrapper& setRegionCount(size_t count) {
        config_->set("region_count", static_cast<int64_t>(count));
        return *this;
    }
    
    ConfigWrapper& setConnectionProbability(float prob) {
        config_->set("connection_probability", prob);
        return *this;
    }
    
    ConfigWrapper& setTimestep(double timestep) {
        config_->set("simulation_timestep", timestep);
        return *this;
    }
    
    ConfigWrapper& setSTDPParams(float ltp, float ltd, float tau) {
        config_->set("stdp_ltp_weight", ltp);
        config_->set("stdp_ltd_weight", ltd);
        config_->set("stdp_tau", tau);
        return *this;
    }
    
    ConfigWrapper& setDevelopment(bool enable, double period = 60.0) {
        config_->set("enable_development", enable);
        config_->set("development_period", period);
        return *this;
    }
    
    // Convenience methods for common configurations
    ConfigWrapper& createResearchConfig() {
        return setNeuronCount(2000)
            .setRegionCount(2)
            .setConnectionProbability(0.05f)
            .setTimestep(0.001)
            .setSTDPParams(0.01f, 0.012f, 20.0f)
            .setDevelopment(true);
    }
    
    ConfigWrapper& createDemoConfig() {
        return setNeuronCount(500)
            .setRegionCount(1)
            .setConnectionProbability(0.15f)
            .setTimestep(0.001)
            .setSTDPParams(0.01f, 0.012f, 20.0f)
            .setDevelopment(true);
    }
    
    ConfigWrapper& createFastTestConfig() {
        return setNeuronCount(100)
            .setRegionCount(1)
            .setConnectionProbability(0.2f)
            .setTimestep(0.01)  // Larger timestep for faster testing
            .setSTDPParams(0.02f, 0.024f, 10.0f)
            .setDevelopment(false);  // Skip development for speed
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    void clear() {
        config_->clear();
    }
    
    std::shared_ptr<Config> getInternalConfig() const {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Vision Class with Python-friendly methods
class VisionWrapper {
public:
    VisionWrapper(size_t width = 0, size_t height = 0, size_t channels = 3)
        : vision_(width, height, channels) {}
    
    // Set vision data with different input formats
    void setData(const std::vector<float>& data) {
        vision_.setData(data);
    }
    
    void setData(const std::vector<std::vector<float>>& data) {
        // Flatten 2D data
        std::vector<float> flattened;
        for (const auto& row : data) {
            flattened.insert(flattened.end(), row.begin(), row.end());
        }
        vision_.setData(flattened);
    }
    
    void setData(const py::array& array) {
        // Convert numpy array to std::vector
        py::buffer_info buf = array.request();
        if (buf.ndim != 2) {
            throw py::value_error("Vision data must be 2-dimensional");
        }
        
        size_t width = buf.shape[1];
        size_t height = buf.shape[0];
        
        if (width * height != buf.size / buf.itemsize) {
            throw py::value_error("Vision data size mismatch");
        }
        
        std::vector<float> data(buf.size / buf.itemsize);
        std::memcpy(data.data(), buf.ptr, buf.size);
        
        vision_.setData(data);
    }
    
    // Get vision data in different formats
    py::array getData() const {
        const auto& data = vision_.getData();
        return py::array(py::buffer_info(
            const_cast<float*>(data.data()),  // Note: const cast due to API limitation
            sizeof(float),
            py::format_descriptor<float>::value,
            2,
            {vision_.getHeight(), vision_.getWidth()},
            {sizeof(float) * vision_.getWidth(), sizeof(float)}
        ));
    }
    
    std::vector<float> getFlattenedData() const {
        return vision_.getData();
    }
    
    size_t getWidth() const { return vision_.getWidth(); }
    size_t getHeight() const { return vision_.getHeight(); }
    size_t getChannels() const { return vision_.getChannels(); }
    
    // Normalize vision data
    void normalize(float min = 0.0f, float max = 1.0f) {
        auto& data = const_cast<std::vector<float>&>(vision_.getData());
        float currentMin = *std::min_element(data.begin(), data.end());
        float currentMax = *std::max_element(data.begin(), data.end());
        
        if (currentMax > currentMin) {
            for (auto& value : data) {
                value = (value - currentMin) / (currentMax - currentMin) * (max - min) + min;
            }
        }
    }
    
    // Add noise for data augmentation
    void addNoise(float amount = 0.1f) {
        auto& data = const_cast<std::vector<float>&>(vision_.getData());
        for (auto& value : data) {
            value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * amount;
        }
    }
    
    VisionWrapper& blur(float sigma = 1.0f) {
        // Simple blur implementation
        auto& data = const_cast<std::vector<float>&>(vision_.getData());
        size_t width = vision_.getWidth();
        size_t height = vision_.getHeight();
        
        std::vector<float> blurred(data.size(), 0.0f);
        
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                float sum = 0.0f;
                float weight = 0.0f;
                
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        size_t ny = y + dy;
                        size_t nx = x + dx;
                        
                        if (ny < height && nx < width) {
                            size_t idx = ny * width + nx;
                            float w = std::exp(-(dx*dx + dy*dy) / (2 * sigma * sigma));
                            sum += data[idx] * w;
                            weight += w;
                        }
                    }
                }
                
                if (weight > 0.0f) {
                    blurred[y * width + x] = sum / weight;
                }
            }
        }
        
        data = std::move(blurred);
        return *this;
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "Vision(width=" << vision_.getWidth()
           << ", height=" << vision_.getHeight()
           << ", channels=" << vision_.getChannels()
           << ", size=" << vision_.getData().size() << ")";
        return ss.str();
    }
    
    std::shared_ptr<Vision> getInternalVision() const {
        return std::make_shared<Vision>(vision_);
    }
    
private:
    Vision vision_;
};

// Enhanced Action Class with Python-friendly methods
class ActionWrapper {
public:
    ActionWrapper(ActionType type = ActionType::Wait) : action_(type) {}
    
    ActionWrapper(ActionType type, const std::vector<float>& parameters) 
        : action_(type, parameters) {}
    
    ActionType getType() const { return action_.getType(); }
    
    void setType(ActionType type) { action_.setType(type); }
    
    std::vector<float> getParameters() const { return action_.getParameters(); }
    
    void setParameters(const std::vector<float>& params) { action_.setParameters(params); }
    
    std::string getName() const { return action_.getName(); }
    
    // Action execution methods
    bool execute() {
        // Simplified execution - in real implementation this would interface with world
        return true;
    }
    
    // Action utilities
    ActionWrapper& addParameter(float value) {
        auto params = action_.getParameters();
        params.push_back(value);
        action_.setParameters(params);
        return *this;
    }
    
    ActionWrapper& setParameter(size_t index, float value) {
        auto params = action_.getParameters();
        if (index < params.size()) {
            params[index] = value;
            action_.setParameters(params);
        }
        return *this;
    }
    
    ActionWrapper& scaleParameters(float factor) {
        auto params = action_.getParameters();
        for (auto& param : params) {
            param *= factor;
        }
        action_.setParameters(params);
        return *this;
    }
    
    // Action chaining for complex behaviors
    ActionWrapper& addSequentialAction(ActionType nextAction) {
        // In a real implementation, this would create a compound action
        auto params = action_.getParameters();
        params.push_back(static_cast<float>(nextAction));  // Encode next action
        action_.setParameters(params);
        return *this;
    }
    
    std::shared_ptr<Action> getInternalAction() const {
        return std::make_shared<Action>(action_);
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "Action(type=" << static_cast<int>(action_.getType())
           << ", params=" << action_.getParameters().size() << ")";
        return ss.str();
    }
    
private:
    Action action_;
};

// Enhanced AgentBrain Class with Python-friendly methods
class AgentBrainWrapper {
public:
    AgentBrainWrapper(std::shared_ptr<Brain> brain) : agent_(std::make_shared<AgentBrain>(brain)) {}
    
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agent_->initialize(*world);
    }
    
    // Pythonic configuration methods
    AgentBrainWrapper& enableRewardModulation(bool enable = true) {
        agent_->enableRewardModulation(enable);
        return *this;
    }
    
    AgentBrainWrapper& enableStructuralPlasticity(bool enable = true) {
        agent_->enableStructuralPlasticity(enable);
        return *this;
    }
    
    AgentBrainWrapper& enableDevelopment(bool enable = true) {
        agent_->enableDevelopment(enable);
        return *this;
    }
    
    AgentBrainWrapper& enableCuriosity(bool enable = true) {
        agent_->enableCuriosity(enable);
        return *this;
    }
    
    // Neural network analysis
    std::map<std::string, float> analyzeNetwork() {
        std::map<std::string, float> stats;
        
        if (auto brain = agent_->getBrain()) {
            stats["neuron_count"] = static_cast<float>(brain->getTotalNeuronCount());
            stats["synapse_count"] = static_cast<float>(brain->getTotalSynapseCount());
            stats["firing_rate"] = brain->getAverageFiringRate();
            stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<float>(brain->getDevelopmentalStage());
        }
        
        return stats;
    }
    
    // Learning analysis
    std::map<std::string, float> analyzeLearning() {
        std::map<std::string, float> learningStats;
        
        learningStats["dopamine_level"] = agent_->getNeuromodulationLevel();
        learningStats["curiosity_level"] = agent_->getCuriosityLevel();
        learningStats["novelty_level"] = agent_->getNoveltyLevel();
        learningStats["prediction_error"] = agent_->getPredictionError();
        
        return learningStats;
    }
    
    // Sensory processing utilities
    std::map<std::string, py::array> processSensoryPercept(const SensoryPercept& percept) {
        std::map<std::string, py::array> processed;
        
        // Convert to numpy arrays for easier manipulation
        auto visionData = percept.getVision();
        auto touchData = percept.getTouch();
        auto internalData = percept.getInternal();
        auto proprioceptionData = percept.getProprioception();
        
        processed["vision"] = py::array(py::buffer_info(
            const_cast<float*>(visionData.data()),  // Note: const cast
            sizeof(float),
            py::format_descriptor<float>::value,
            1,
            {visionData.size()},
            {sizeof(float)}
        ));
        
        processed["touch"] = py::array(py::buffer_info(
            const_cast<float*>(touchData.data()),
            sizeof(float),
            py::format_descriptor<float>::value,
            1,
            {touchData.size()},
            {sizeof(float)}
        ));
        
        return processed;
    }
    
    // Advanced curiosity-based exploration
    ActionWrapper exploreWithCuriosity(ActionWrapper defaultAction) {
        // Implement curiosity-based action selection
        float curiosity = agent_->getCuriosityLevel();
        
        if (curiosity > 0.5f) {
            // High curiosity - select random action
            int choice = rand() % 6;
            switch (choice) {
                case 0: return ActionWrapper(ActionType::MoveForward);
                case 1: return ActionWrapper(ActionType::MoveBackward);
                case 2: return ActionWrapper(ActionType::TurnLeft);
                case 3: return ActionWrapper(ActionType::TurnRight);
                case 4: return ActionWrapper(ActionType::Interact);
                default: return ActionWrapper(ActionType::Wait);
            }
        }
        
        return defaultAction;
    }
    
    // Development progression analysis
    std::string getDevelopmentalStageString() const {
        auto stage = agent_->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    std::shared_ptr<AgentBrain> getInternalAgent() const {
        return agent_;
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "AgentBrain(" << analyzeNetwork().size() << " neural stats, "
           << analyzeLearning().size() << " learning stats)";
        return ss.str();
    }
    
private:
    std::shared_ptr<AgentBrain> agent_;
};

// Enhanced Brain Class with Python-friendly methods
class BrainWrapper {
public:
    BrainWrapper(std::shared_ptr<Config> config = nullptr) {
        if (config) {
            brain_ = std::make_shared<Brain>(config);
        } else {
            brain_ = std::make_shared<Brain>(std::make_shared<Config>());
        }
    }
    
    bool initialize() {
        try {
            return brain_->initialize();
        } catch (const std::exception& e) {
            throw py::runtime_error("Brain initialization failed: " + std::string(e.what()));
        }
    }
    
    // Simulation with progress and monitoring
    std::map<std::string, py::array> runSimulation(
        uint64_t steps,
        double timestep = 0.001,
        bool showProgress = false,
        std::function<void(uint64_t, float, float)> callback = nullptr
    ) {
        std::map<std::string, py::array> results;
        
        try {
            float totalReward = 0.0f;
            std::vector<float> rewards;
            std::vector<size_t> spikeCounts;
            std::vector<size_t> activeNeuronCounts;
            
            for (uint64_t step = 0; step < steps; ++step) {
                // Brain step
                brain_->step(step, step * timestep);
                
                // Collect statistics
                rewards.push_back(static_cast<float>(brain_->getTotalSpikeCount()));
                spikeCounts.push_back(static_cast<size_t>(brain_->getTotalSpikeCount()));
                activeNeuronCounts.push_back(brain_->getActiveNeuronCount());
                
                // Callback for progress
                if (callback) {
                    callback(step, totalReward / (step + 1), brain_->getAverageFiringRate());
                }
                
                if (showProgress && step % 100 == 0) {
                    std::cout << "\rStep: " << step << "/" << steps 
                             << " | Spikes: " << brain_->getTotalSpikeCount()
                             << " | Rate: " << brain_->getAverageFiringRate() << " Hz";
                    std::cout.flush();
                }
            }
            
            if (showProgress) {
                std::cout << std::endl;
            }
            
            // Prepare results as numpy arrays
            results["rewards"] = py::array(py::buffer_info(
                rewards.data(), sizeof(float), py::format_descriptor<float>::value,
                1, {rewards.size()}, {sizeof(float)}
            ));
            
            results["spike_counts"] = py::array(py::buffer_info(
                spikeCounts.data(), sizeof(size_t), py::format_descriptor<size_t>::value,
                1, {spikeCounts.size()}, {sizeof(size_t)}
            ));
            
            results["active_neurons"] = py::array(py::buffer_info(
                activeNeuronCounts.data(), sizeof(size_t), py::format_descriptor<size_t>::value,
                1, {activeNeuronCounts.size()}, {sizeof(size_t)}
            ));
            
            return results;
            
        } catch (const std::exception& e) {
            throw py::runtime_error("Simulation failed: " + std::string(e.what()));
        }
    }
    
    // Enhanced save/load with compression and validation
    bool save(const std::string& filepath, bool compress = false) {
        try {
            return brain_->save(filepath);
        } catch (const std::exception& e) {
            throw py::runtime_error("Save failed: " + std::string(e.what()));
        }
    }
    
    bool load(const std::string& filepath, bool validate = false) {
        try {
            bool success = brain_->load(filepath);
            
            if (validate && success) {
                // Validate loaded state
                validateState();
            }
            
            return success;
        } catch (const std::exception& e) {
            throw py::runtime_error("Load failed: " + std::string(e.what()));
        }
    }
    
    void validateState() {
        // Validate brain state consistency
        if (!brain_) {
            throw py::runtime_error("Brain is not initialized");
        }
        
        // Check for null pointers
        if (!brain_->getSpikeSystem()) {
            throw py::runtime_error("Spike system is not initialized");
        }
        
        if (!brain_->getSTDP()) {
            throw py::runtime_error("STDP system is not initialized");
        }
        
        // Check consistency
        auto totalNeurons = brain_->getTotalNeuronCount();
        auto totalSpikes = brain_->getTotalSpikeCount();
        
        if (totalNeurons == 0 && totalSpikes > 0) {
            throw py::runtime_error("Invalid state: zero neurons but non-zero spikes");
        }
    }
    
    // Neural network analysis
    std::map<std::string, py::array> analyzeNetwork() {
        std::map<std::string, py::array> analysis;
        
        // Connectivity matrix (simplified)
        std::vector<std::vector<float>> connectivity(brain_->getRegionCount(), 
                                                      std::vector<float>(brain_->getRegionCount(), 0.0f));
        
        // Fill in connectivity (simplified - would need actual connection data)
        for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
            for (size_t j = 0; j < brain_->getRegionCount(); ++j) {
                if (i != j) {
                    connectivity[i][j] = 0.1f;  // Dummy value
                }
            }
        }
        
        analysis["connectivity"] = py::array(py::buffer_info(
            connectivity.data()->data(), sizeof(float), py::format_descriptor<float>::value,
            2, {connectivity.size(), connectivity[0].size()}, 
            {sizeof(std::vector<float>), sizeof(float)}
        ));
        
        // Firing rate statistics
        std::vector<float> firingRates;
        for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
            firingRates.push_back(brain_->getAverageFiringRate());
        }
        
        analysis["firing_rates"] = py::array(py::buffer_info(
            firingRates.data(), sizeof(float), py::format_descriptor<float>::value,
            1, {firingRates.size()}, {sizeof(float)}
        ));
        
        return analysis;
    }
    
    // Deep learning style analysis
    std::map<std::string, py::array> extractFeatures() {
        std::map<std::string, py::array> features;
        
        // Extract neural activity features
        std::vector<float> activityFeatures;
        
        // Add various neural activity features
        activityFeatures.push_back(brain_->getAverageFiringRate());
        activityFeatures.push_back(brain_->getExcitationInhibitionRatio());
        activityFeatures.push_back(static_cast<float>(brain_->getTotalSpikeCount()) / brain_->getTotalNeuronCount());
        activityFeatures.push_back(static_cast<float>(brain_->getActiveNeuronCount()) / brain_->getTotalNeuronCount());
        
        features["activity"] = py::array(py::buffer_info(
            activityFeatures.data(), sizeof(float), py::format_descriptor<float>::value,
            1, {activityFeatures.size()}, {sizeof(float)}
        ));
        
        return features;
    }
    
    // Advanced experiment execution
    std::map<std::string, py::array> runPhase6Experiment(
        uint64_t steps,
        double timestep = 0.001,
        bool enableCheckpointing = true,
        bool enableReplay = true,
        bool enableDevelopment = true
    ) {
        // Create Phase6 config
        Phase6Config config;
        config.maxSteps = steps;
        config.neuronCount = brain_->getTotalNeuronCount();
        config.regionCount = brain_->getRegionCount();
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        config.checkpointPath = "./phase6_checkpoint.bin";
        
        // Run Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Convert results to Python format
        std::map<std::string, py::array> results;
        
        // Create arrays for experiment results
        std::vector<float> metricValues;
        metricValues.push_back(result.totalReward);
        metricValues.push_back(result.avgFiringRate);
        metricValues.push_back(result.memoryEpisodesStored);
        metricValues.push_back(result.dopamineLevel);
        
        results["metrics"] = py::array(py::buffer_info(
            metricValues.data(), sizeof(float), py::format_descriptor<float>::value,
            1, {metricValues.size()}, {sizeof(float)}
        ));
        
        results["wall_time"] = py::array(py::buffer_info(
            &result.totalWallClockTime, sizeof(double), py::format_descriptor<double>::value,
            1, {1}, {sizeof(double)}
        ));
        
        return results;
    }
    
    std::shared_ptr<Brain> getInternalBrain() const {
        return brain_;
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "Brain(neurons=" << brain_->getTotalNeuronCount()
           << ", regions=" << brain_->getRegionCount()
           << ", synapses=" << brain_->getTotalSynapseCount()
           << ", state=" << (brain_->initialize() ? "initialized" : "not_initialized") << ")";
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced SimpleWorld Class with Python-friendly methods
class SimpleWorldWrapper {
public:
    SimpleWorldWrapper() : world_(std::make_shared<SimpleWorld>()) {}
    
    void configure(float width, float height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world_->reset();
    }
    
    // Pythonic action execution
    std::map<std::string, float> applyAction(
        ActionType action,
        double timestep = 0.1,
        std::function<bool()> customCondition = nullptr
    ) {
        std::map<std::string, float> result;
        
        // Create motor command from action type
        MotorCommand cmd;
        switch (action) {
            case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
            case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
            case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
            case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
            case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
            case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
            case ActionType::Interact: cmd = MotorCommand::Interact; break;
            default: cmd = MotorCommand::Wait; break;
        }
        
        // Check custom condition
        if (customCondition && !customCondition()) {
            result["success"] = 0.0f;
            result["reward"] = 0.0f;
            result["message"] = "condition_not_met";
            return result;
        }
        
        // Apply action
        ActionResult actionResult = world_->applyMotorCommand(cmd, timestep);
        
        result["success"] = actionResult.success ? 1.0f : 0.0f;
        result["reward"] = actionResult.reward;
        result["message"] = actionResult.message;
        
        return result;
    }
    
    // Enhanced sensory processing
    std::map<std::string, py::array> getSensoryData() {
        std::map<std::string, py::array> sensoryData;
        
        auto& percept = world_->getSensoryPercept();
        
        // Convert all sensory modalities to numpy arrays
        sensoryData["vision"] = py::array(py::buffer_info(
            const_cast<float*>(percept.getVision().data()),
            sizeof(float), py::format_descriptor<float>::value,
            1, {percept.getVision().size()}, {sizeof(float)}
        ));
        
        sensoryData["touch"] = py::array(py::buffer_info(
            const_cast<float*>(percept.getTouch().data()),
            sizeof(float), py::format_descriptor<float>::value,
            1, {percept.getTouch().size()}, {sizeof(float)}
        ));
        
        sensoryData["internal"] = py::array(py::buffer_info(
            const_cast<float*>(percept.getInternal().data()),
            sizeof(float), py::format_descriptor<float>::value,
            1, {percept.getInternal().size()}, {sizeof(float)}
        ));
        
        sensoryData["proprioception"] = py::array(py::buffer_info(
            const_cast<float*>(percept.getProprioception().data()),
            sizeof(float), py::format_descriptor<float>::value,
            1, {percept.getProprioception().size()}, {sizeof(float)}
        ));
        
        return sensoryData;
    }
    
    // World simulation with time control
    std::map<std::string, py::array> simulate(
        uint64_t steps,
        double timestep = 0.1,
        bool saveHistory = true
    ) {
        std::map<std::string, py::array> simulationResults;
        
        std::vector<double> times;
        std::vector<float> energies;
        std::vector<float> scores;
        
        for (uint64_t step = 0; step < steps; ++step) {
            world_->update(timestep);
            
            auto& agent = world_->getAgentBody();
            times.push_back(world_->getSimulationTime());
            energies.push_back(agent.energy);
            scores.push_back(agent.energy / world_->getMaxEnergy());
            
            if (saveHistory) {
                // In a real implementation, save sensory history
            }
        }
        
        // Convert to numpy arrays
        simulationResults["times"] = py::array(py::buffer_info(
            times.data(), sizeof(double), py::format_descriptor<double>::value,
            1, {times.size()}, {sizeof(double)}
        ));
        
        simulationResults["energies"] = py::array(py::buffer_info(
            energies.data(), sizeof(float), py::format_descriptor<float>::value,
            1, {energies.size()}, {sizeof(float)}
        ));
        
        simulationResults["scores"] = py::array(py::buffer_info(
            scores.data(), sizeof(float), py::format_descriptor<float>::value,
            1, {scores.size()}, {sizeof(float)}
        ));
        
        return simulationResults;
    }
    
    // World analysis and statistics
    std::map<std::string, float> analyzeWorld() {
        std::map<std::string, float> analysis;
        
        auto& agent = world_->getAgentBody();
        analysis["agent_x"] = agent.x;
        analysis["agent_y"] = agent.y;
        analysis["agent_orientation"] = agent.orientation;
        analysis["agent_energy"] = agent.energy;
        analysis["agent_health"] = agent.health;
        analysis["agent_age"] = agent.age;
        analysis["agent_is_moving"] = agent.isMoving ? 1.0f : 0.0f;
        analysis["agent_is_turning"] = agent.isTurning ? 1.0f : 0.0f;
        
        analysis["world_width"] = world_->getWidth();
        analysis["world_height"] = world_->getHeight();
        analysis["world_time"] = world_->getSimulationTime();
        analysis["max_energy"] = world_->getMaxEnergy();
        
        return analysis;
    }
    
    std::shared_ptr<SimpleWorld> getInternalWorld() const {
        return world_;
    }
    
    std::string summary() const {
        std::stringstream ss;
        auto analysis = analyzeWorld();
        ss << "SimpleWorld(width=" << analysis["world_width"]
           << ", height=" << analysis["world_height"]
           << ", time=" << analysis["world_time"]
           << ", agent=(" << analysis["agent_x"] << "," << analysis["agent_y"] << ")"
           << ")";
        return ss.str();
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// High-level NLM interface for Python users
class NLMSystem {
public:
    NLMSystem() : cli_(std::make_shared<NLMCLI>()) {}
    
    // Pythonic factory methods
    static std::shared_ptr<BrainWrapper> createBrain(
        size_t neuronCount = 500,
        size_t regionCount = 1,
        float connectionProbability = 0.15f
    ) {
        auto config = std::make_shared<ConfigWrapper>();
        config->setNeuronCount(neuronCount)
             .setRegionCount(regionCount)
             .setConnectionProbability(connectionProbability);
        
        return std::make_shared<BrainWrapper>(config->getInternalConfig());
    }
    
    static std::shared_ptr<SimpleWorldWrapper> createWorld(
        float width = 20.0f,
        float height = 20.0f,
        size_t visionWidth = 16,
        size_t visionHeight = 16
    ) {
        auto world = std::make_shared<SimpleWorldWrapper>();
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        return world;
    }
    
    static std::shared_ptr<AgentBrainWrapper> createAgent(
        std::shared_ptr<BrainWrapper> brain,
        std::shared_ptr<SimpleWorldWrapper> world
    ) {
        auto agent = std::make_shared<AgentBrainWrapper>(brain->getInternalBrain());
        agent->initialize(world->getInternalWorld());
        return agent;
    }
    
    // Complete system setup and initialization
    bool setupCompleteSystem(
        size_t neuronCount = 500,
        size_t regionCount = 1,
        uint64_t steps = 1000,
        double timestep = 0.001,
        bool useDemoConfig = true
    ) {
        try {
            // Create components
            auto config = std::make_shared<ConfigWrapper>();
            if (useDemoConfig) {
                config->createDemoConfig();
            }
            
            auto brain = std::make_shared<BrainWrapper>(config->getInternalConfig());
            auto world = std::make_shared<SimpleWorldWrapper>();
            auto agent = std::make_shared<AgentBrainWrapper>(brain->getInternalBrain());
            
            // Initialize system
            if (!brain->initialize()) {
                return false;
            }
            
            world->reset();
            agent->initialize(world->getInternalWorld());
            
            // Store references
            systemBrain_ = brain;
            systemWorld_ = world;
            systemAgent_ = agent;
            
            // Run simulation
            systemResults_ = brain->runSimulation(steps, timestep, true);
            
            return true;
        } catch (const std::exception& e) {
            throw py::runtime_error("System setup failed: " + std::string(e.what()));
        }
    }
    
    // System analysis and reporting
    std::map<std::string, py::array> getSystemAnalysis() {
        std::map<std::string, py::array> analysis;
        
        if (!systemBrain_ || !systemWorld_ || !systemAgent_) {
            throw py::runtime_error("System not initialized. Call setupCompleteSystem() first.");
        }
        
        // Neural network analysis
        auto networkAnalysis = systemBrain_->analyzeNetwork();
        for (const auto& pair : networkAnalysis) {
            analysis["network_" + pair.first] = pair.second;
        }
        
        // Agent analysis
        auto agentAnalysis = systemAgent_->analyzeNetwork();
        for (const auto& pair : agentAnalysis) {
            analysis["agent_" + pair.first] = py::array(py::buffer_info(
                &pair.second, sizeof(float), py::format_descriptor<float>::value,
                1, {1}, {sizeof(float)}
            ));
        }
        
        // World analysis
        auto worldAnalysis = systemWorld_->analyzeWorld();
        for (const auto& pair : worldAnalysis) {
            analysis["world_" + pair.first] = py::array(py::buffer_info(
                &pair.second, sizeof(float), py::format_descriptor<float>::value,
                1, {1}, {sizeof(float)}
            ));
        }
        
        return analysis;
    }
    
    // Save/load system state
    bool saveSystem(const std::string& filepath) {
        if (!systemBrain_) {
            return false;
        }
        
        return systemBrain_->save(filepath);
    }
    
    bool loadSystem(const std::string& filepath) {
        if (!systemBrain_) {
            return false;
        }
        
        return systemBrain_->load(filepath);
    }
    
    // Get system components
    std::shared_ptr<BrainWrapper> getBrain() const { return systemBrain_; }
    std::shared_ptr<SimpleWorldWrapper> getWorld() const { return systemWorld_; }
    std::shared_ptr<AgentBrainWrapper> getAgent() const { return systemAgent_; }
    
    std::map<std::string, py::array> getSimulationResults() const { return systemResults_; }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "NLMSystem(";
        if (systemBrain_) ss << "brain=" << systemBrain_->summary() << ", ";
        if (systemWorld_) ss << "world=" << systemWorld_->summary() << ", ";
        if (systemAgent_) ss << "agent=" << systemAgent_->summary();
        ss << ")";
        return ss.str();
    }
    
private:
    std::shared_ptr<NLMCLI> cli_;
    std::shared_ptr<BrainWrapper> systemBrain_;
    std::shared_ptr<SimpleWorldWrapper> systemWorld_;
    std::shared_ptr<AgentBrainWrapper> systemAgent_;
    std::map<std::string, py::array> systemResults_;
};

// Global function to start NLM with Python interface
void startNLM(const std::vector<std::string>& args) {
    // Redirect to CLI for command-line processing
    NLMCLI::getInstance().run();
}

// Export Python bindings
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------
        Advanced Python interface for NLM (Neural Learning Machine)
        with enhanced convenience methods and improved user experience.
        
        Key Features:
        - Pythonic configuration and setup
        - Enhanced sensory processing
        - Advanced visualization and analysis
        - System-level convenience methods
        - Error handling and type safety
        - Integration with PyTorch-style data processing
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<py::value_error>(m, "ValueError");
    
    // Register enhanced classes
    py::class_<ConfigWrapper>(m, "Config", R"pbdoc(Advanced configuration wrapper)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &ConfigWrapper::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &ConfigWrapper::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("setNeuronCount", &ConfigWrapper::setNeuronCount,
             "Set neuron count and return self for chaining")
        .def("setRegionCount", &ConfigWrapper::setRegionCount,
             "Set region count and return self for chaining")
        .def("setConnectionProbability", &ConfigWrapper::setConnectionProbability,
             "Set connection probability and return self for chaining")
        .def("setTimestep", &ConfigWrapper::setTimestep,
             "Set simulation timestep and return self for chaining")
        .def("setSTDPParams", &ConfigWrapper::setSTDPParams,
             py::arg("ltp"), py::arg("ltd"), py::arg("tau"),
             "Set STDP parameters and return self for chaining")
        .def("setDevelopment", &ConfigWrapper::setDevelopment,
             py::arg("enable"), py::arg("period") = 60.0,
             "Set development settings and return self for chaining")
        .def("createResearchConfig", &ConfigWrapper::createResearchConfig,
             "Create research configuration")
        .def("createDemoConfig", &ConfigWrapper::createDemoConfig,
             "Create demo configuration")
        .def("createFastTestConfig", &ConfigWrapper::createFastTestConfig,
             "Create fast test configuration")
        .def("summary", &ConfigWrapper::summary,
             "Get configuration summary");
    
    py::class_<VisionWrapper>(m, "Vision", R"pbdoc(Enhanced vision interface)pbdoc")
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 0, 
             py::arg("height") = 0, py::arg("channels") = 3)
        .def("setData", &VisionWrapper::setData, py::arg("data"),
             "Set vision data from vector")
        .def("setData", &VisionWrapper::setData, py::arg("array"),
             "Set vision data from numpy array")
        .def("getData", &VisionWrapper::getData,
             "Get vision data as numpy array")
        .def("getFlattenedData", &VisionWrapper::getFlattenedData,
             "Get flattened vision data")
        .def("normalize", &VisionWrapper::normalize,
             py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize vision data")
        .def("addNoise", &VisionWrapper::addNoise,
             py::arg("amount") = 0.1f,
             "Add noise to vision data")
        .def("blur", &VisionWrapper::blur, py::arg("sigma") = 1.0f,
             "Apply blur filter to vision data")
        .def("getWidth", &VisionWrapper::getWidth)
        .def("getHeight", &VisionWrapper::getHeight)
        .def("getChannels", &VisionWrapper::getChannels)
        .def("summary", &VisionWrapper::summary,
             "Get vision summary");
    
    py::class_<ActionWrapper>(m, "Action", R"pbdoc(Enhanced action interface)pbdoc")
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &ActionWrapper::getType)
        .def("setType", &ActionWrapper::setType, py::arg("type"))
        .def("getParameters", &ActionWrapper::getParameters)
        .def("setParameters", &ActionWrapper::setParameters, py::arg("params"))
        .def("getName", &ActionWrapper::getName)
        .def("execute", &ActionWrapper::execute,
             "Execute the action")
        .def("addParameter", &ActionWrapper::addParameter, py::arg("value"),
             "Add parameter to action")
        .def("setParameter", &ActionWrapper::setParameter, py::arg("index"), py::arg("value"),
             "Set parameter at index")
        .def("scaleParameters", &ActionWrapper::scaleParameters, py::arg("factor"),
             "Scale all parameters by factor")
        .def("summary", &ActionWrapper::summary,
             "Get action summary");
    
    py::class_<AgentBrainWrapper>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrainWrapper::initialize, py::arg("world"))
        .def("enableRewardModulation", &AgentBrainWrapper::enableRewardModulation,
             py::arg("enable") = true)
        .def("enableStructuralPlasticity", &AgentBrainWrapper::enableStructuralPlasticity,
             py::arg("enable") = true)
        .def("enableDevelopment", &AgentBrainWrapper::enableDevelopment,
             py::arg("enable") = true)
        .def("enableCuriosity", &AgentBrainWrapper::enableCuriosity,
             py::arg("enable") = true)
        .def("analyzeNetwork", &AgentBrainWrapper::analyzeNetwork,
             "Analyze neural network")
        .def("analyzeLearning", &AgentBrainWrapper::analyzeLearning,
             "Analyze learning statistics")
        .def("processSensoryPercept", &AgentBrainWrapper::processSensoryPercept,
             py::arg("percept"),
             "Process sensory percept")
        .def("exploreWithCuriosity", &AgentBrainWrapper::exploreWithCuriosity,
             py::arg("defaultAction"),
             "Explore using curiosity-driven strategy")
        .def("getDevelopmentalStageString", &AgentBrainWrapper::getDevelopmentalStageString,
             "Get developmental stage as string")
        .def("summary", &AgentBrainWrapper::summary,
             "Get agent brain summary");
    
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(Enhanced brain wrapper)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config") = nullptr)
        .def("initialize", &BrainWrapper::initialize,
             "Initialize the brain")
        .def("runSimulation", &BrainWrapper::runSimulation,
             py::arg("steps"), py::arg("timestep") = 0.001,
             py::arg("showProgress") = false, py::arg("callback") = nullptr,
             "Run simulation with optional callback")
        .def("save", &BrainWrapper::save, py::arg("filepath"), py::arg("compress") = false,
             "Save brain state to file")
        .def("load", &BrainWrapper::load, py::arg("filepath"), py::arg("validate") = false,
             "Load brain state from file")
        .def("analyzeNetwork", &BrainWrapper::analyzeNetwork,
             "Analyze neural network")
        .def("extractFeatures", &BrainWrapper::extractFeatures,
             "Extract features from brain state")
        .def("runPhase6Experiment", &BrainWrapper::runPhase6Experiment,
             py::arg("steps"), py::arg("timestep") = 0.001,
             py::arg("enableCheckpointing") = true,
             py::arg("enableReplay") = true,
             py::arg("enableDevelopment") = true,
             "Run Phase 6 integration experiment")
        .def("summary", &BrainWrapper::summary,
             "Get brain summary");
    
    py::class_<SimpleWorldWrapper>(m, "SimpleWorld", R"pbdoc(Enhanced world wrapper)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorldWrapper::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorldWrapper::reset)
        .def("applyAction", &SimpleWorldWrapper::applyAction,
             py::arg("action"), py::arg("timestep") = 0.1,
             py::arg("customCondition") = nullptr,
             "Apply action to world")
        .def("getSensoryData", &SimpleWorldWrapper::getSensoryData,
             "Get sensory data as dictionary")
        .def("simulate", &SimpleWorldWrapper::simulate,
             py::arg("steps"), py::arg("timestep") = 0.1,
             py::arg("saveHistory") = true,
             "Run world simulation")
        .def("analyzeWorld", &SimpleWorldWrapper::analyzeWorld,
             "Analyze world state")
        .def("summary", &SimpleWorldWrapper::summary,
             "Get world summary");
    
    py::class_<NLMSystem>(m, "NLM", R"pbdoc(Complete NLM system interface)pbdoc")
        .def(py::init<>())
        .def("setupCompleteSystem", &NLMSystem::setupCompleteSystem,
             py::arg("neuronCount") = 500, py::arg("regionCount") = 1,
             py::arg("steps") = 1000, py::arg("timestep") = 0.001,
             py::arg("useDemoConfig") = true,
             "Setup complete NLM system")
        .def("getSystemAnalysis", &NLMSystem::getSystemAnalysis,
             "Get system analysis results")
        .def("saveSystem", &NLMSystem::saveSystem, py::arg("filepath"),
             "Save system state")
        .def("loadSystem", &NLMSystem::loadSystem, py::arg("filepath"),
             "Load system state")
        .def("getBrain", &NLMSystem::getBrain,
             "Get brain component")
        .def("getWorld", &NLMSystem::getWorld,
             "Get world component")
        .def("getAgent", &NLMSystem::getAgent,
             "Get agent component")
        .def("summary", &NLMSystem::summary,
             "Get system summary");
    
    // Factory methods
    m.def("createBrain", &NLMSystem::createBrain,
          py::arg("neuronCount") = 500, py::arg("regionCount") = 1,
          py::arg("connectionProbability") = 0.15f,
          "Create brain with configuration");
    
    m.def("createWorld", &NLMSystem::createWorld,
          py::arg("width") = 20.0f, py::arg("height") = 20.0f,
          py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
          "Create world with dimensions");
    
    m.def("createAgent", &NLMSystem::createAgent,
          py::arg("brain"), py::arg("world"),
          "Create agent brain interface");
    
    // Global interface functions
    m.def("startNLM", &startNLM, py::arg("args"),
          "Start NLM CLI with arguments");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Error handling
    m.def("setErrorHandler", [](std::function<void(const std::string&)> handler) {
        py::register_exception_translator([handler](std::exception_ptr p) {
            if (p) {
                try {
                    std::rethrow_exception(p);
                } catch (const py::error_already_set& e) {
                    handler(e.what());
                } catch (const std::exception& e) {
                    handler(e.what());
                }
            }
        });
    });
    
    // Performance monitoring
    m.def("enablePerformanceMonitoring", []() {
        // Implement performance monitoring setup
    });
    
    // Documentation
    m.def("getDocumentation", []() {
        return R"doc(
            NLM Enhanced Python Bindings
            ==============================
            
            This module provides enhanced Python bindings for the NLM
            (Neural Learning Machine) C++ framework, offering:
            
            **Key Features:**
            1. Pythonic configuration and setup
            2. Enhanced sensory processing with automatic normalization
            3. Advanced action execution with chaining
            4. System-level convenience methods
            5. Comprehensive error handling
            6. Type safety and documentation
            
            **Main Classes:**
            - Config: Configuration management
            - Vision: Vision sensory interface
            - Action: Action execution
            - AgentBrain: Agent brain interface
            - Brain: Neural brain simulation
            - SimpleWorld: World simulation
            - NLM: Complete system interface
            
            **Usage Examples:**
            ```python
            import pynlm_enhanced as nlm
            
            # Create brain with configuration
            brain = nlm.createBrain(neuron_count=1000, region_count=2)
            
            # Create world and agent
            world = nlm.createWorld(width=30, height=30)
            agent = nlm.createAgent(brain, world)
            
            # Initialize and run
            if brain.initialize():
                results = brain.runSimulation(steps=1000)
                analysis = nlm.getSystemAnalysis()
            ```
            
            **Advanced Usage:**
            ```python
            # Setup complete system
            system = nlm.NLM()
            if system.setupCompleteSystem(steps=5000):
                network_stats = system.getBrain().analyzeNetwork()
                learning_stats = system.getAgent().analyzeLearning()
                system.saveSystem("my_brain")
            ```
        )doc";
    });
}

} // namespace nlm