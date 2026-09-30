#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/Action.hpp"
#include "core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Context Manager for Brain Resources
class BrainContext {
public:
    BrainContext(std::shared_ptr<Config> config) : config_(config), brain_(nullptr) {
        if (config_) {
            brain_ = std::make_shared<Brain>(config_);
            if (!brain_->initialize()) {
                throw std::runtime_error("Failed to initialize brain in context");
            }
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    void step(SimulationStep step, Timestamp time) {
        if (brain_) {
            brain_->step(step, time);
        }
    }
    
    bool isInitialized() const {
        return brain_ != nullptr && brain_->getTotalNeuronCount() > 0;
    }
    
    py::dict getStatus() const {
        py::dict status;
        if (brain_) {
            status["neurons"] = brain_->getTotalNeuronCount();
            status["firing"] = brain_->getFiringNeuronCount();
            status["spikes"] = brain_->getTotalSpikeCount();
            status["dev_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        }
        return status;
    }
    
    // Context manager protocol
    BrainContext* operator->() {
        return this;
    }
    
    std::shared_ptr<Brain> operator*() {
        return getBrain();
    }
};

// Enhanced Context Manager for World Resources
class WorldContext {
public:
    WorldContext() : world_(nullptr) {
        world_ = std::make_shared<SimpleWorld>();
    }
    
    ~WorldContext() {
        if (world_) {
            world_->reset();
        }
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world_;
    }
    
    void configure(int width, int height, int visionWidth, int visionHeight) {
        if (world_) {
            world_->configure(width, height, visionWidth, visionHeight);
        }
    }
    
    void setAgentStart(float x, float y) {
        if (world_) {
            world_->setAgentStart(x, y);
        }
    }
    
    void update(float timestep) {
        if (world_) {
            world_->update(timestep);
        }
    }
    
    // Context manager protocol
    WorldContext* operator->() {
        return this;
    }
    
    std::shared_ptr<SimpleWorld> operator*() {
        return getWorld();
    }
};

// Enhanced Agent Interface with Fluent API
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_;
    
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agent_(std::make_shared<AgentBrain>(brain)) {}
    
    EnhancedAgentBrain& initialize(std::shared_ptr<SimpleWorld> world) {
        agent_->initialize(*world);
        return *this;
    }
    
    EnhancedAgentBrain& enableAll() {
        agent_->enableRewardModulation(true);
        agent_->enableStructuralPlasticity(true);
        agent_->enableDevelopment(true);
        agent_->enableCuriosity(true);
        return *this;
    }
    
    EnhancedAgentBrain& enableSystems(bool reward, bool plasticity, bool development, bool curiosity) {
        agent_->enableRewardModulation(reward);
        agent_->enableStructuralPlasticity(plasticity);
        agent_->enableDevelopment(development);
        agent_->enableCuriosity(curiosity);
        return *this;
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return agent_->getBrain();
    }
    
    std::shared_ptr<AgentBrain> getCoreAgent() const {
        return agent_;
    }
    
    // Convenience methods
    EnhancedAgentBrain& withRewardModulation() {
        agent_->enableRewardModulation(true);
        return *this;
    }
    
    EnhancedAgentBrain& withStructuralPlasticity() {
        agent_->enableStructuralPlasticity(true);
        return *this;
    }
    
    EnhancedAgentBrain& withDevelopment() {
        agent_->enableDevelopment(true);
        return *this;
    }
    
    EnhancedAgentBrain& withCuriosity() {
        agent_->enableCuriosity(true);
        return *this;
    }
    
    float getCuriosityLevel() const {
        return agent_->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agent_->getNoveltyLevel();
    }
    
    float getPredictionError() const {
        return agent_->getPredictionError();
    }
    
    py::dict getStats() const {
        py::dict stats;
        auto brain = agent_->getBrain();
        stats["neurons"] = brain->getTotalNeuronCount();
        stats["firing"] = brain->getFiringNeuronCount();
        stats["spikes"] = brain->getTotalSpikeCount();
        stats["curiosity"] = agent_->getCuriosityLevel();
        stats["novelty"] = agent_->getNoveltyLevel();
        stats["prediction_error"] = agent_->getPredictionError();
        stats["dev_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        return stats;
    }
    
    // Simulation runner
    EnhancedAgentBrain& runEpisode(std::shared_ptr<SimpleWorld> world, int steps, float timestep = 0.1f) {
        agent_->initialize(*world);
        agent_->enableAllSubsystems();
        
        for (int step = 0; step < steps; ++step) {
            world->update(timestep);
            agent_->processSensoryInput(world->getSensoryPercept());
            world->applyMotorCommand(agent_->decodeMotorCommand(), world->getSimulationTime());
            agent_->updateDevelopment(timestep);
        }
        
        return *this;
    }
    
    // Pythonic interface for chaining
    EnhancedAgentBrain& stepWithInput(std::shared_ptr<SimpleWorld> world, float timestep) {
        world->update(timestep);
        agent_->processSensoryInput(world->getSensoryPercept());
        return *this;
    }
    
    EnhancedAgentBrain& applyOutput(std::shared_ptr<SimpleWorld> world) {
        world->applyMotorCommand(agent_->decodeMotorCommand(), world->getSimulationTime());
        return *this;
    }
};

// Enhanced Configuration with Type Safety
class EnhancedConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    
    EnhancedConfig& set(const std::string& key, int64_t value) {
        config_->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    // Template-based getter with type safety
    template<typename T>
    T get(const std::string& key, T defaultValue = T()) const {
        try {
            return config_->get<T>(key, defaultValue);
        } catch (...) {
            return defaultValue;
        }
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    // Pythonic property access
    EnhancedConfig& withKey(const std::string& key, auto value) {
        config_->set(key, value);
        return *this;
    }
    
    // Context manager for temporary configs
    EnhancedConfig copy() const {
        EnhancedConfig copy;
        copy.config_ = std::make_shared<Config>(*config_);
        return copy;
    }
    
    std::shared_ptr<Config> getCoreConfig() const {
        return config_;
    }
};

// Enhanced Vision with NumPy-like interface
class EnhancedVision {
private:
    Vision vision_;
    
public:
    EnhancedVision() : vision_() {}
    
    EnhancedVision(size_t width, size_t height, size_t channels = 3) : vision_(width, height, channels) {}
    
    void setData(const std::vector<float>& data) {
        vision_.setData(data);
    }
    
    std::vector<float> getData() const {
        return vision_.getData();
    }
    
    size_t getWidth() const { return vision_.getWidth(); }
    size_t getHeight() const { return vision_.getHeight(); }
    size_t getChannels() const { return vision_.getChannels(); }
    
    std::vector<size_t> shape() const {
        return {getWidth(), getHeight(), getChannels()};
    }
    
    // Pythonic properties
    size_t width() const { return getWidth(); }
    size_t height() const { return getHeight(); }
    size_t channels() const { return getChannels(); }
    
    // Enhancement methods
    EnhancedVision& normalize() {
        auto data = getData();
        float min_val = *std::min_element(data.begin(), data.end());
        float max_val = *std::max_element(data.begin(), data.end());
        float range = max_val - min_val;
        if (range > 0) {
            for (float& val : data) {
                val = (val - min_val) / range;
            }
        }
        setData(data);
        return *this;
    }
    
    EnhancedVision& resize(size_t newWidth, size_t newHeight) {
        // Simple resize implementation - in practice would use interpolation
        std::vector<float> resizedData(newWidth * newHeight * getChannels(), 0.0f);
        float x_ratio = static_cast<float>(getWidth()) / newWidth;
        float y_ratio = static_cast<float>(getHeight()) / newHeight;
        
        for (size_t j = 0; j < newHeight; ++j) {
            for (size_t i = 0; i < newWidth; ++i) {
                size_t src_x = static_cast<size_t>(i * x_ratio);
                size_t src_y = static_cast<size_t>(j * y_ratio);
                src_x = std::min(src_x, getWidth() - 1);
                src_y = std::min(src_y, getHeight() - 1);
                
                for (size_t c = 0; c < getChannels(); ++c) {
                    size_t src_idx = ((src_y * getWidth()) + src_x) * getChannels() + c;
                    size_t dst_idx = ((j * newWidth) + i) * getChannels() + c;
                    resizedData[dst_idx] = getData()[src_idx];
                }
            }
        }
        setData(resizedData);
        return *this;
    }
    
    py::array_t<float> to_array() const {
        std::vector<float> data = getData();
        py::array_t<float> array({getHeight(), getWidth(), getChannels()});
        auto buf = array.mutable_unchecked();
        for (size_t h = 0; h < getHeight(); ++h) {
            for (size_t w = 0; w < getWidth(); ++w) {
                for (size_t c = 0; c < getChannels(); ++c) {
                    size_t idx = ((h * getWidth() + w) * getChannels()) + c;
                    buf(h, w, c) = data[idx];
                }
            }
        }
        return array;
    }
    
    // Conversion methods
    py::dict to_dict() const {
        return py::dict(
            "width", getWidth(),
            "height", getHeight(),
            "channels", getChannels(),
            "data", getData()
        );
    }
    
    static EnhancedVision from_dict(const py::dict& d) {
        EnhancedVision vision;
        vision.setData(d["data"].cast<std::vector<float>>());
        if (d.contains("width")) vision.width_ = d["width"].cast<size_t>();
        if (d.contains("height")) vision.height_ = d["height"].cast<size_t>();
        if (d.contains("channels")) vision.channels_ = d["channels"].cast<size_t>();
        return vision;
    }
    
    std::string to_json() const {
        py::dict d = to_dict();
        // Convert to simple JSON string
        std::string json = "{\"width\":" + std::to_string(getWidth()) +
                          "\"height\":" + std::to_string(getHeight()) +
                          "\"channels\":" + std::to_string(getChannels()) + "\"data\":";
        // For simplicity, just return basic representation
        return "EnhancedVision[width=" + std::to_string(getWidth()) +
               " height=" + std::to_string(getHeight()) + "]";
    }
}; 
