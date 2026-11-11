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
#include <stdexcept>
#include <sstream>
#include <cmath>

// Forward declarations for enhanced classes
namespace py = pybind11;
namespace nlm {

// Custom exception class for Python bindings
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
    NLMError(const std::string& message, const std::string& operation)
        : std::runtime_error("NLM Error: " + operation + ": " + message) {}
};

// Helper functions for Python integration
namespace py_helpers {
    template<typename T>
    T py_safe_get(const py::object& py_obj, const std::string& key, const T& default_val = T()) {
        if (py_obj.attr("has_key")(key).cast<bool>()) {
            return py_obj.attr(key).cast<T>();
        }
        return default_val;
    }
    
    std::string py_safe_str(const py::object& py_obj) {
        return py_obj.cast<std::string>();
    }
    
    float py_safe_float(const py::object& py_obj) {
        return py_obj.cast<float>();
    }
    
    int py_safe_int(const py::object& py_obj) {
        return py_obj.cast<int>();
    }
    
    bool py_safe_bool(const py::object& py_obj) {
        return py_obj.cast<bool>();
    }
    
    std::vector<float> py_safe_float_list(const py::object& py_obj) {
        return py_obj.cast<std::vector<float>>();
    }
    
    std::vector<int> py_safe_int_list(const py::object& py_obj) {
        return py_obj.cast<std::vector<int>>();
    }
    
    std::vector<std::string> py_safe_string_list(const py::object& py_obj) {
        return py_obj.cast<std::vector<std::string>>();
    }
}

// Enhanced PythonConfigWrapper class
class PythonConfigWrapper {
private:
    std::shared_ptr<Config> config_;
    
public:
    PythonConfigWrapper() : config_(std::make_shared<Config>()) {}
    
    explicit PythonConfigWrapper(const py::dict& config_dict) {
        config_ = std::make_shared<Config>();
        validateAndSetDict(config_dict);
    }
    
    explicit PythonConfigWrapper(const std::string& filepath) {
        config_ = std::make_shared<Config>();
        if (!config_->loadFromFile(filepath)) {
            throw NLMError("Failed to load configuration from file: " + filepath, "PythonConfigWrapper::constructor");
        }
    }
    
    void validateAndSetDict(const py::dict& config_dict) {
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value_obj = item.second;
            ConfigValue value;
            
            if (value_obj.is(py::int_())) {
                value = value_obj.cast<int>();
            } else if (value_obj.is(py::int64_t())) {
                value = value_obj.cast<int64_t>();
            } else if (value_obj.is(py::float_())) {
                value = value_obj.cast<double>();
            } else if (value_obj.is(py::bool_())) {
                value = value_obj.cast<bool>();
            } else if (value_obj.is(py::str())) {
                value = value_obj.cast<std::string>();
            } else if (value_obj.is(py::list())) {
                py::list list_obj = value_obj.cast<py::list>();
                if (list_obj.size() > 0) {
                    if (list_obj[0].is(py::int_())) {
                        std::vector<int> int_list;
                        for (auto item : list_obj) {
                            int_list.push_back(item.cast<int>());
                        }
                        value = int_list;
                    } else if (list_obj[0].is(py::float_())) {
                        std::vector<double> double_list;
                        for (auto item : list_obj) {
                            double_list.push_back(item.cast<double>());
                        }
                        value = double_list;
                    } else if (list_obj[0].is(py::str())) {
                        std::vector<std::string> str_list;
                        for (auto item : list_obj) {
                            str_list.push_back(item.cast<std::string>());
                        }
                        value = str_list;
                    } else {
                        throw NLMError("Unsupported list element type for key: " + key, "PythonConfigWrapper::validateAndSetDict");
                    }
                } else {
                    value = std::vector<int>();
                }
            } else {
                throw NLMError("Unsupported value type for key: " + key, "PythonConfigWrapper::validateAndSetDict");
            }
            
            config_->set(key, value);
        }
    }
    
    template<typename T>
    T get(const std::string& key, const T& default_value = T()) const {
        auto value_opt = config_->get<T>(key);
        if (value_opt.has_value()) {
            return value_opt.value();
        }
        return default_value;
    }
    
    int getInt(const std::string& key, int default_value = 0) const {
        auto value_opt = config_->get<int>(key);
        return value_opt.has_value() ? value_opt.value() : default_value;
    }
    
    double getDouble(const std::string& key, double default_value = 0.0) const {
        auto value_opt = config_->get<double>(key);
        return value_opt.has_value() ? value_opt.value() : default_value;
    }
    
    bool getBool(const std::string& key, bool default_value = false) const {
        auto value_opt = config_->get<bool>(key);
        return value_opt.has_value() ? value_opt.value() : default_value;
    }
    
    std::string getString(const std::string& key, const std::string& default_value = "") const {
        auto value_opt = config_->get<std::string>(key);
        return value_opt.has_value() ? value_opt.value() : default_value;
    }
    
    std::vector<float> getFloatList(const std::string& key) const {
        auto value_opt = config_->get<std::vector<double>>(key);
        std::vector<float> result;
        if (value_opt.has_value()) {
            for (auto val : value_opt.value()) {
                result.push_back(static_cast<float>(val));
            }
        }
        return result;
    }
    
    std::vector<int> getIntList(const std::string& key) const {
        auto value_opt = config_->get<std::vector<int>>(key);
        return value_opt.has_value() ? value_opt.value() : std::vector<int>();
    }
    
    std::vector<double> getDoubleList(const std::string& key) const {
        auto value_opt = config_->get<std::vector<double>>(key);
        return value_opt.has_value() ? value_opt.value() : std::vector<double>();
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, const std::vector<float>& value) {
        config_->set(key, std::vector<double>(value.begin(), value.end()));
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    void remove(const std::string& key) {
        config_->remove(key);
    }
    
    void clear() {
        config_->clear();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    void updateFromDict(const py::dict& config_dict) {
        validateAndSetDict(config_dict);
    }
    
    void batchUpdate(const std::map<std::string, ConfigValue>& updates) {
        for (const auto& update : updates) {
            config_->set(update.first, update.second);
        }
    }
    
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    bool loadFromFile(const std::string& filepath) {
        if (!config_->loadFromFile(filepath)) {
            throw NLMError("Failed to load configuration from file: " + filepath, "PythonConfigWrapper::loadFromFile");
        }
        return true;
    }
    
    std::string toDict() const {
        std::stringstream ss;
        ss << "{";
        auto keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << keys[i] << "\": ";
            auto value_opt = config_->get<ConfigValue>(keys[i]);
            if (value_opt.has_value()) {
                const ConfigValue& val = value_opt.value();
                ss << "\"" << std::visit([](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int>) return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, int64_t>) return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, double>) return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, bool>) return arg ? "true" : "false";
                    else if constexpr (std::is_same_v<T, std::string>) return arg;
                    else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        std::stringstream vss;
                        vss << "[";
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) vss << ", ";
                            vss << arg[j];
                        }
                        vss << "]";
                        return vss.str();
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        std::stringstream vss;
                        vss << "[";
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) vss << ", ";
                            vss << arg[j];
                        }
                        vss << "]";
                        return vss.str();
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        std::stringstream vss;
                        vss << "[\"";
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) vss << "\", \";
                            vss << arg[j];
                        }
                        vss << "\"]";
                        return vss.str();
                    } else {
                        return "\"unknown\"";
                    }
                }, val) << "\"";
            }
        }
        ss << "}";
        return ss.str();
    }
    
    bool validate() const {
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            if (key.empty()) {
                return false;
            }
        }
        return !keys.empty();
    }
};

// Enhanced PythonBrainWrapper class
class PythonBrainWrapper {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PythonBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void initializeWithConfig(const py::dict& config_dict) {
        PythonConfigWrapper config_wrapper(config_dict);
        brain_->initialize();
    }
    
    void simulateStep(SimulationStep step, Timestamp time = 0.0) {
        brain_->step(step, time);
    }
    
    void injectSensory(const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        vision->setData(data);
        brain_->receiveSensoryInput(*vision);
    }
    
    std::vector<float> getActionVector() {
        auto action = brain_->produceAction();
        return action->getParameters();
    }
    
    py::dict getStatistics() const {
        py::dict stats;
        
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain_->getTotalSpikeCount();
        stats["pending_spike_events"] = brain_->getPendingSpikeEventCount();
        
        return stats;
    }
    
    std::vector<py::dict> getNeuronPopulationStats() const {
        std::vector<py::dict> population_stats;
        
        auto regions = brain_->getRegions();
        for (const auto& region : regions) {
            py::dict region_dict;
            
            auto populations = region->getPopulations();
            for (const auto& population : populations) {
                py::dict pop_dict;
                
                auto neurons = population->getNeurons();
                size_t excitatory = 0, inhibitory = 0, sensory = 0, motor = 0, internal = 0;
                
                for (const auto& neuron : neurons) {
                    switch (neuron->getType()) {
                        case NeuronType::Excitatory: excitatory++; break;
                        case NeuronType::Inhibitory: inhibitory++; break;
                        case NeuronType::Sensory: sensory++; break;
                        case NeuronType::Motor: motor++; break;
                        case NeuronType::Internal: internal++; break;
                        default: break;
                    }
                }
                
                pop_dict["region_name"] = region->getName();
                pop_dict["neuron_type"] = static_cast<int>(population->getType());
                pop_dict["total_neurons"] = neurons.size();
                pop_dict["excitatory"] = excitatory;
                pop_dict["inhibitory"] = inhibitory;
                pop_dict["sensory"] = sensory;
                pop_dict["motor"] = motor;
                pop_dict["internal"] = internal;
                pop_dict["avg_firing_rate"] = population->getAverageFiringRate();
                
                population_stats.push_back(pop_dict);
            }
        }
        
        return population_stats;
    }
    
    void resetSimulation() {
        brain_->reset();
    }
    
    bool saveToFile(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool loadFromFile(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    void setDevelopmentalStage(const std::string& stage_str) {
        DevelopmentalStage stage;
        if (stage_str == "Initial") stage = DevelopmentalStage::Initial;
        else if (stage_str == "CriticalPeriod") stage = DevelopmentalStage::CriticalPeriod;
        else if (stage_str == "Maturation") stage = DevelopmentalStage::Maturation;
        else if (stage_str == "Adult") stage = DevelopmentalStage::Adult;
        else if (stage_str == "Aging") stage = DevelopmentalStage::Aging;
        else {
            throw NLMError("Invalid developmental stage: " + stage_str, "PythonBrainWrapper::setDevelopmentalStage");
        }
        brain_->setDevelopmentalStage(stage);
    }
    
    std::string getDevelopmentalStage() const {
        switch (brain_->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    RegionId addNeuralRegion(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    float getLearnedPreference() const {
        return brain_->getLearnedPreference();
    }
    
    void setLearnedPreference(float preference) {
        brain_->setLearnedPreference(preference);
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
};

// Enhanced PythonWorldWrapper class
class PythonWorldWrapper {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    PythonWorldWrapper() : world_(std::make_shared<SimpleWorld>()) {}
    
    explicit PythonWorldWrapper(const py::dict& config_dict) {
        world_ = std::make_shared<SimpleWorld>();
        configureFromDict(config_dict);
    }
    
    void configureFromDict(const py::dict& config_dict) {
        size_t vision_width = py_helpers::py_safe_get<size_t>(config_dict, "vision_width", 16);
        size_t vision_height = py_helpers::py_safe_get<size_t>(config_dict, "vision_height", 16);
        float width = py_helpers::py_safe_get<float>(config_dict, "width", 100.0f);
        float height = py_helpers::py_safe_get<float>(config_dict, "height", 100.0f);
        
        world_->configure(width, height, vision_width, vision_height);
        
        world_->setMaxEnergy(py_helpers::py_safe_get<float>(config_dict, "max_energy", 100.0f));
        world_->setEnergyDecayRate(py_helpers::py_safe_get<float>(config_dict, "energy_decay_rate", 0.01f));
        world_->setRandomSeed(py_helpers::py_safe_get<uint64_t>(config_dict, "random_seed", 12345ULL));
    }
    
    void reset() {
        world_->reset();
    }
    
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    ActionResult applyAction(const py::dict& action_dict) {
        MotorCommand cmd;
        std::string type_str = py_helpers::py_safe_str(action_dict["type"]);
        
        if (type_str == "MoveForward") cmd = MotorCommand::MoveForward;
        else if (type_str == "MoveBackward") cmd = MotorCommand::MoveBackward;
        else if (type_str == "MoveLeft") cmd = MotorCommand::MoveLeft;
        else if (type_str == "MoveRight") cmd = MotorCommand::MoveRight;
        else if (type_str == "TurnLeft") cmd = MotorCommand::TurnLeft;
        else if (type_str == "TurnRight") cmd = MotorCommand::TurnRight;
        else if (type_str == "Look") cmd = MotorCommand::Look;
        else if (type_str == "Interact") cmd = MotorCommand::Interact;
        else if (type_str == "Wait") cmd = MotorCommand::Wait;
        else {
            throw NLMError("Unknown action type: " + type_str, "PythonWorldWrapper::applyAction");
        }
        
        double current_time = world_->getSimulationTime();
        return world_->applyMotorCommand(cmd, current_time);
    }
    
    void addWorldObject(const py::dict& obj_dict) {
        float x = py_helpers::py_safe_get<float>(obj_dict, "x", 0.0f);
        float y = py_helpers::py_safe_get<float>(obj_dict, "y", 0.0f);
        std::string type_str = py_helpers::py_safe_str(obj_dict["type"]);
        WorldObjectType type;
        
        if (type_str == "Empty") type = WorldObjectType::Empty;
        else if (type_str == "Resource") type = WorldObjectType::Resource;
        else if (type_str == "Hazard") type = WorldObjectType::Hazard;
        else if (type_str == "Wall") type = WorldObjectType::Wall;
        else if (type_str == "Marker") type = WorldObjectType::Marker;
        else {
            throw NLMError("Unknown world object type: " + type_str, "PythonWorldWrapper::createWorldObject");
        }
        
        float value = py_helpers::py_safe_get<float>(obj_dict, "value", 0.0f);
        float radius = py_helpers::py_safe_get<float>(obj_dict, "radius", 0.5f);
        
        WorldObject obj(x, y, type, value, radius);
        world_->addObject(obj);
    }
    
    py::dict getAgentState() const {
        py::dict state;
        const AgentBody& agent = world_->getAgentBody();
        
        state["x"] = agent.x;
        state["y"] = agent.y;
        state["orientation"] = agent.orientation;
        state["velocity_x"] = agent.velocityX;
        state["velocity_y"] = agent.velocityY;
        state["angular_velocity"] = agent.angularVelocity;
        state["energy"] = agent.energy;
        state["health"] = agent.health;
        state["age"] = agent.age;
        state["is_moving"] = agent.isMoving;
        state["is_turning"] = agent.isTurning;
        state["last_action_time"] = agent.lastActionTime;
        
        return state;
    }
    
    py::dict getSensoryPercept() const {
        const SensoryPercept& percept = world_->getSensoryPercept();
        py::dict percept_dict;
        
        auto vision = percept.getVision();
        if (vision) {
            std::vector<float> vision_data = vision->getData();
            percept_dict["vision"] = vision_data;
            percept_dict["vision_width"] = vision->getWidth();
            percept_dict["vision_height"] = vision->getHeight();
            percept_dict["vision_channels"] = vision->getChannels();
        }
        
        auto touch = percept.getTouch();
        if (touch) {
            std::vector<float> touch_data = touch->getData();
            percept_dict["touch"] = touch_data;
        }
        
        auto internal = percept.getInternal();
        if (internal) {
            std::vector<float> internal_data = internal->getData();
            percept_dict["internal"] = internal_data;
        }
        
        auto proprioception = percept.getProprioception();
        if ( proprioception) {
            std::vector<float> proprioception_data = proprioception->getData();
            percept_dict["proprioception"] = proprioception_data;
        }
        
        auto audio = percept.getAudio();
        if (audio) {
            std::vector<float> audio_data = audio->getData();
            percept_dict["audio"] = audio_data;
            percept_dict["sample_rate"] = audio->getSampleRate();
        }
        
        percept_dict["timestamp"] = percept.getTimestamp();
        
        return percept_dict;
    }
    
    float getWidth() const {
        return world_->getWidth();
    }
    
    float getHeight() const {
        return world_->getHeight();
    }
    
    size_t getVisionWidth() const {
        return world_->getVisionWidth();
    }
    
    size_t getVisionHeight() const {
        return world_->getVisionHeight();
    }
    
    float getMaxEnergy() const {
        return world_->getMaxEnergy();
    }
    
    void setMaxEnergy(float e) {
        world_->setMaxEnergy(e);
    }
    
    float getEnergyDecayRate() const {
        return world_->getEnergyDecayRate();
    }
    
    void setEnergyDecayRate(float r) {
        world_->setEnergyDecayRate(r);
    }
    
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    uint64_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world_;
    }
};

// PythonAgentBrainWrapper class
class PythonAgentBrainWrapper {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    PythonAgentBrainWrapper(std::shared_ptr<AgentBrain> agent_brain) 
        : agent_brain_(agent_brain) {}
    
    void initializeStepByStep(const PythonWorldWrapper& world_wrapper) {
        agent_brain_->initialize(*world_wrapper.getWorld());
    }
    
    void processSensoryInputStepByStep(const py::dict& percept_dict) {
        SensoryPercept percept;
        
        if (percept_dict.contains("vision")) {
            auto vision_data = percept_dict["vision"].cast<std::vector<float>>();
            auto vision = std::make_unique<Vision>(16, 16, 3);
            vision->setData(vision_data);
            percept.setVision(std::move(vision));
        }
        
        if (percept_dict.contains("touch")) {
            auto touch_data = percept_dict["touch"].cast<std::vector<float>>();
            auto touch = std::make_unique<Touch>(touch_data);
            percept.setTouch(std::move(touch));
        }
        
        if (percept_dict.contains("internal")) {
            auto internal_data = percept_dict["internal"].cast<std::vector<float>>();
            auto internal = std::make_unique<InternalSignals>(internal_data);
            percept.setInternal(std::move(internal));
        }
        
        if (percept_dict.contains("proprioception")) {
            auto proprioception_data = percept_dict["proprioception"].cast<std::vector<float>>();
            auto proprioception = std::make_unique<Proprioception>(proprioception_data);
            percept.setProprioception(std::move(proprioception));
        }
        
        if (percept_dict.contains("audio")) {
            auto audio_data = percept_dict["audio"].cast<std::vector<float>>();
            auto audio = std::make_unique<Audio>(44100, audio_data.size());
            audio->setData(audio_data);
            percept.setAudio(std::move(audio));
        }
        
        if (percept_dict.contains("timestamp")) {
            double timestamp = percept_dict["timestamp"].cast<double>();
            percept.setTimestamp(timestamp);
        }
        
        agent_brain_->processSensoryInput(percept);
    }
    
    std::string decodeMotorCommandStepByStep() {
        MotorCommand cmd = agent_brain_->decodeMotorCommand();
        
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    void applyRewardModulationStepByStep(float reward, float predicted_reward) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    void updateDevelopmentStepByStep(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    py::dict getStateInfo() const {
        py::dict state;
        
        state["developmental_stage"] = static_cast<int>(agent_brain_->getDevelopmentalStage());
        state["neuromodulation_level"] = agent_brain_->getNeuromodulationLevel();
        state["curiosity_level"] = agent_brain_->getCuriosityLevel();
        state["novelty_level"] = agent_brain_->getNoveltyLevel();
        state["prediction_error"] = agent_brain_->getPredictionError();
        
        return state;
    }
    
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
    
    std::shared_ptr<Brain> getBrain() const {
        return agent_brain_->getBrain();
    }
};

// Factory functions
std::shared_ptr<Config> createConfiguredConfig(const py::dict& config_dict) {
    return std::make_shared<PythonConfigWrapper>(config_dict).getConfig();
}

std::shared_ptr<Brain> createConfiguredBrain(const py::dict& config_dict) {
    auto config_wrapper = std::make_shared<PythonConfigWrapper>(config_dict);
    auto brain = std::make_shared<Brain>(config_wrapper->getConfig());
    brain->initialize();
    return brain;
}

std::shared_ptr<SimpleWorld> createConfiguredWorld(const py::dict& config_dict) {
    auto world_wrapper = std::make_shared<PythonWorldWrapper>(config_dict);
    return world_wrapper->getWorld();
}

std::shared_ptr<AgentBrain> createSimpleAgentSetup(
    const py::dict& brain_config_dict,
    const py::dict& world_config_dict,
    const py::dict& agent_config_dict
) {
    auto config_wrapper = std::make_shared<PythonConfigWrapper>(brain_config_dict);
    
    auto brain = std::make_shared<Brain>(config_wrapper->getConfig());
    brain->initialize();
    
    auto world_wrapper = std::make_shared<PythonWorldWrapper>(world_config_dict);
    
    auto agent_brain = std::make_shared<AgentBrain>(brain);
    agent_brain->initialize(*world_wrapper->getWorld());
    
    return agent_brain;
}

// Batch processing utilities
namespace batch {
    
    class BatchSimulator {
    private:
        std::vector<std::shared_ptr<Brain>> brains_;
        std::vector<std::shared_ptr<SimpleWorld>> worlds_;
        std::vector<std::shared_ptr<AgentBrain>> agent_brains_;
        size_t max_batch_size_;
        
    public:
        BatchSimulator(size_t max_batch_size = 10) 
            : max_batch_size_(max_batch_size) {}
        
        void addConfig(const py::dict& config_dict) {
            brains_.push_back(createConfiguredBrain(config_dict));
        }
        
        void addWorld(const py::dict& config_dict) {
            worlds_.push_back(createConfiguredWorld(config_dict));
        }
        
        std::vector<py::dict> runBatchSimulations(const std::vector<std::vector<float>>& sensory_inputs, 
                                                 double timestep,
                                                 size_t steps) {
            std::vector<py::dict> results;
            
            for (size_t batch_start = 0; batch_start < brains_.size(); 
                 batch_start += max_batch_size_) {
                size_t batch_end = std::min(batch_start + max_batch_size_, brains_.size());
                
                for (size_t i = batch_start; i < batch_end; ++i) {
                    py::dict result;
                    result["brain_id"] = i;
                    result["brain_stats"] = brains_[i]->getStatistics();
                    result["brain_region_stats"] = brains_[i]->getNeuronPopulationStats();
                    
                    if (sensory_inputs.size() > i && i < sensory_inputs.size()) {
                        auto vision = std::make_unique<Vision>(16, 16, 3);
                        vision->setData(sensory_inputs[i]);
                        brains_[i]->receiveSensoryInput(*vision);
                    }
                    
                    for (size_t step = 0; step < steps; ++step) {
                        brains_[i]->step(step, timestep * step);
                    }
                    
                    result["final_stats"] = brains_[i]->getStatistics();
                    results.push_back(result);
                }
            }
            
            return results;
        }
        
        py::dict compareBrains() {
            py::dict comparison;
            
            if (brains_.empty()) {
                return comparison;
            }
            
            for (size_t i = 0; i < brains_.size(); ++i) {
                std::string key = "brain_" + std::to_string(i);
                comparison[key] = brains_[i]->getStatistics();
            }
            
            py::dict averages;
            size_t num_brains = brains_.size();
            
            double total_neurons = 0, total_synapses = 0, total_active = 0, total_firing = 0;
            double total_firing_rate = 0, total_spikes = 0;
            
            for (size_t i = 0; i < num_brains; ++i) {
                auto stats = brains_[i]->getStatistics();
                total_neurons += stats["total_neurons"].cast<double>();
                total_synapses += stats["total_synapses"].cast<double>();
                total_active += stats["active_neurons"].cast<double>();
                total_firing += stats["firing_neurons"].cast<double>();
                total_firing_rate += stats["average_firing_rate"].cast<double>();
                total_spikes += stats["total_spike_count"].cast<double>();
            }
            
            averages["avg_total_neurons"] = total_neurons / num_brains;
            averages["avg_total_synapses"] = total_synapses / num_brains;
            averages["avg_active_neurons"] = total_active / num_brains;
            averages["avg_firing_neurons"] = total_firing / num_brains;
            averages["avg_firing_rate"] = total_firing_rate / num_brains;
            averages["avg_total_spikes"] = total_spikes / num_brains;
            
            comparison["averages"] = averages;
            return comparison;
        }
        
        void clear() {
            brains_.clear();
            worlds_.clear();
            agent_brains_.clear();
        }
        
        size_t getBatchSize() const {
            return max_batch_size_;
        }
        
        void setBatchSize(size_t size) {
            max_batch_size_ = size;
        }
    };
    
    std::vector<py::dict> run_batch_simulations(
        const std::vector<py::dict>& config_dicts,
        const std::vector<std::vector<float>>& sensory_inputs,
        double timestep,
        size_t steps
    ) {
        BatchSimulator simulator;
        
        for (const auto& config_dict : config_dicts) {
            simulator.addConfig(config_dict);
        }
        
        return simulator.runBatchSimulations(sensory_inputs, timestep, steps);
    }
} // namespace batch

// Statistics utilities
namespace statistics {
    
    py::dict calculateStatistics(const py::dict& data) {
        py::dict stats;
        
        if (data.contains("values")) {
            auto values = data["values"].cast<std::vector<double>>();
            if (!values.empty()) {
                double sum = 0.0;
                for (double val : values) sum += val;
                
                stats["mean"] = sum / values.size();
                
                double sq_sum = 0.0;
                for (double val : values) {
                    double diff = val - stats["mean"].cast<double>();
                    sq_sum += diff * diff;
                }
                stats["std_dev"] = std::sqrt(sq_sum / values.size());
                
                double min_val = values[0];
                double max_val = values[0];
                for (double val : values) {
                    if (val < min_val) min_val = val;
                    if (val > max_val) max_val = val;
                }
                stats["min"] = min_val;
                stats["max"] = max_val;
            }
        }
        
        return stats;
    }
    
    py::dict calculateNeuronPopulationStats(const py::list& population_stats_list) {
        py::dict overall_stats;
        
        if (population_stats_list.empty()) {
            return overall_stats;
        }
        
        size_t count = 0;
        double total_neurons = 0, total_excitatory = 0, total_inhibitory = 0, 
               total_sensory = 0, total_motor = 0, total_internal = 0;
        double total_firing_rate = 0;
        
        for (auto item : population_stats_list) {
            py::dict pop_stats = item.cast<py::dict>();
            
            total_neurons += pop_stats["total_neurons"].cast<double>();
            total_excitatory += pop_stats["excitatory"].cast<double>();
            total_inhibitory += pop_stats["inhibitory"].cast<double>();
            total_sensory += pop_stats["sensory"].cast<double>();
            total_motor += pop_stats["motor"].cast<double>();
            total_internal += pop_stats["internal"].cast<double>();
            total_firing_rate += pop_stats["avg_firing_rate"].cast<double>();
            
            count++;
        }
        
        overall_stats["total_populations"] = count;
        overall_stats["avg_total_neurons"] = total_neurons / count;
        overall_stats["avg_excitatory"] = total_excitatory / count;
        overall_stats["avg_inhibitory"] = total_inhibitory / count;
        overall_stats["avg_sensory"] = total_sensory / count;
        overall_stats["avg_motor"] = total_motor / count;
        overall_stats["avg_internal"] = total_internal / count;
        overall_stats["avg_firing_rate"] = total_firing_rate / count;
        
        return overall_stats;
    }
    
    py::dict compareDatasets(const std::vector<py::dict>& datasets) {
        py::dict comparison;
        
        if (datasets.empty()) {
            return comparison;
        }
        
        size_t num_datasets = datasets.size();
        
        for (size_t i = 0; i < num_datasets; ++i) {
            std::string key = "dataset_" + std::to_string(i);
            comparison[key] = datasets[i];
        }
        
        py::dict averages;
        
        for (size_t metric_idx = 0; metric_idx < 10; ++metric_idx) {
            std::string metric_name = "metric_" + std::to_string(metric_idx);
            double sum = 0.0;
            size_t count = 0;
            
            for (size_t i = 0; i < num_datasets; ++i) {
                if (datasets[i].contains(metric_name)) {
                    sum += datasets[i][metric_name].cast<double>();
                    count++;
                }
            }
            
            if (count > 0) {
                averages[metric_name + "_avg"] = sum / count;
            }
        }
        
        comparison["averages"] = averages;
        return comparison;
    }
} // namespace statistics

// Python module initialization
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ----------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides Pythonic wrapper classes, factory functions, and utilities for
        advanced brain simulation and analysis.
    )pbdoc";

    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);

    // Register enhanced wrapper classes
    
    py::class_<PythonConfigWrapper>(m, "PythonConfigWrapper", 
        R"pbdoc(Enhanced configuration wrapper with validation and Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<const py::dict&>())
        .def(py::init<const std::string&>())
        .def("validate", &PythonConfigWrapper::validate)
        .def("update_from_dict", &PythonConfigWrapper::updateFromDict)
        .def("batch_update", &PythonConfigWrapper::batchUpdate)
        .def("to_dict", &PythonConfigWrapper::toDict)
        .def("has", &PythonConfigWrapper::has)
        .def("get_keys", &PythonConfigWrapper::getKeys)
        .def("remove", &PythonConfigWrapper::remove)
        .def("clear", &PythonConfigWrapper::clear)
        .def("summary", &PythonConfigWrapper::summary)
        .def("save_to_file", &PythonConfigWrapper::saveToFile)
        .def("load_from_file", &PythonConfigWrapper::loadFromFile)
        .def("get_int", &PythonConfigWrapper::getInt, py::arg("key"), py::arg("default_value") = 0)
        .def("get_double", &PythonConfigWrapper::getDouble, py::arg("key"), py::arg("default_value") = 0.0)
        .def("get_bool", &PythonConfigWrapper::getBool, py::arg("key"), py::arg("default_value") = false)
        .def("get_string", &PythonConfigWrapper::getString, py::arg("key"), py::arg("default_value") = "")
        .def("get_float_list", &PythonConfigWrapper::getFloatList)
        .def("get_int_list", &PythonConfigWrapper::getIntList)
        .def("get_double_list", &PythonConfigWrapper::getDoubleList)
        .def("set_int", &PythonConfigWrapper::set<int>, py::arg("key"), py::arg("value"))
        .def("set_double", &PythonConfigWrapper::set<double>, py::arg("key"), py::arg("value"))
        .def("set_bool", &PythonConfigWrapper::set<bool>, py::arg("key"), py::arg("value"))
        .def("set_string", &PythonConfigWrapper::set<std::string>, py::arg("key"), py::arg("value"))
        .def("set_float_list", &PythonConfigWrapper::set, py::arg("key"), py::arg("value"));

    py::class_<PythonBrainWrapper>(m, "PythonBrainWrapper",
        R"pbdoc(Enhanced brain wrapper with convenience methods and statistics)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize_with_config", &PythonBrainWrapper::initializeWithConfig)
        .def("simulate_step", &PythonBrainWrapper::simulateStep, py::arg("step"), py::arg("time") = 0.0)
        .def("inject_sensory", &PythonBrainWrapper::injectSensory, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("get_action_vector", &PythonBrainWrapper::getActionVector)
        .def("get_statistics", &PythonBrainWrapper::getStatistics)
        .def("get_neuron_population_stats", &PythonBrainWrapper::getNeuronPopulationStats)
        .def("reset_simulation", &PythonBrainWrapper::resetSimulation)
        .def("save_to_file", &PythonBrainWrapper::saveToFile, py::arg("filepath"))
        .def("load_from_file", &PythonBrainWrapper::loadFromFile, py::arg("filepath"))
        .def("set_developmental_stage", &PythonBrainWrapper::setDevelopmentalStage, py::arg("stage"))
        .def("get_developmental_stage", &PythonBrainWrapper::getDevelopmentalStage)
        .def("add_neural_region", &PythonBrainWrapper::addNeuralRegion, py::arg("name") = "")
        .def("get_learned_preference", &PythonBrainWrapper::getLearnedPreference)
        .def("set_learned_preference", &PythonBrainWrapper::setLearnedPreference, py::arg("preference"));

    py::class_<PythonWorldWrapper>(m, "PythonWorldWrapper",
        R"pbdoc(Enhanced world wrapper with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<const py::dict&>())
        .def("configure_from_dict", &PythonWorldWrapper::configureFromDict)
        .def("reset", &PythonWorldWrapper::reset)
        .def("set_agent_start", &PythonWorldWrapper::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &PythonWorldWrapper::update, py::arg("timestep"))
        .def("apply_action", &PythonWorldWrapper::applyAction, py::arg("action_dict"))
        .def("add_world_object", &PythonWorldWrapper::addWorldObject, py::arg("obj_dict"))
        .def("get_agent_state", &PythonWorldWrapper::getAgentState)
        .def("get_sensory_percept", &PythonWorldWrapper::getSensoryPercept)
        .def("get_width", &PythonWorldWrapper::getWidth)
        .def("get_height", &PythonWorldWrapper::getHeight)
        .def("get_vision_width", &PythonWorldWrapper::getVisionWidth)
        .def("get_vision_height", &PythonWorldWrapper::getVisionHeight)
        .def("get_max_energy", &PythonWorldWrapper::getMaxEnergy)
        .def("set_max_energy", &PythonWorldWrapper::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &PythonWorldWrapper::getEnergyDecayRate)
        .def("set_energy_decay_rate", &PythonWorldWrapper::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &PythonWorldWrapper::getSimulationTime)
        .def("get_random_seed", &PythonWorldWrapper::getRandomSeed)
        .def("set_random_seed", &PythonWorldWrapper::setRandomSeed, py::arg("seed"));

    py::class_<PythonAgentBrainWrapper>(m, "PythonAgentBrainWrapper",
        R"pbdoc(Enhanced agent brain wrapper with step-by-step control)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>())
        .def("initialize_step_by_step", &PythonAgentBrainWrapper::initializeStepByStep)
        .def("process_sensory_input_step_by_step", &PythonAgentBrainWrapper::processSensoryInputStepByStep, py::arg("percept_dict"))
        .def("decode_motor_command_step_by_step", &PythonAgentBrainWrapper::decodeMotorCommandStepByStep)
        .def("apply_reward_modulation_step_by_step", &PythonAgentBrainWrapper::applyRewardModulationStepByStep, py::arg("reward"), py::arg("predicted_reward"))
        .def("update_development_step_by_step", &PythonAgentBrainWrapper::updateDevelopmentStepByStep, py::arg("timestep"))
        .def("get_state_info", &PythonAgentBrainWrapper::getStateInfo)
        .def("enable_reward_modulation", &PythonAgentBrainWrapper::enableRewardModulation, py::arg("enable"))
        .def("enable_structural_plasticity", &PythonAgentBrainWrapper::enableStructuralPlasticity, py::arg("enable"))
        .def("enable_development", &PythonAgentBrainWrapper::enableDevelopment, py::arg("enable"))
        .def("enable_curiosity", &PythonAgentBrainWrapper::enableCuriosity, py::arg("enable"))
        .def("is_reward_modulation_enabled", &PythonAgentBrainWrapper::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &PythonAgentBrainWrapper::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &PythonAgentBrainWrapper::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &PythonAgentBrainWrapper::isCuriosityEnabled)
        .def("get_brain", &PythonAgentBrainWrapper::getBrain);

    m.def("createConfiguredBrain", &createConfiguredBrain,
        py::arg("config_dict"),
        "Create a brain with Python dictionary configuration");
    
    m.def("createConfiguredWorld", &createConfiguredWorld,
        py::arg("config_dict"),
        "Create a world with Python dictionary configuration");
    
    m.def("createSimpleAgentSetup", &createSimpleAgentSetup,
        py::arg("brain_config_dict"),
        py::arg("world_config_dict"),
        py::arg("agent_config_dict"),
        "Create a complete agent setup with Python dictionary configurations");
    
    m.def("createConfiguredConfig", &createConfiguredConfig,
        py::arg("config_dict"),
        "Create a configuration from Python dictionary");

    py::class_<batch::BatchSimulator>(m, "BatchSimulator",
        R"pbdoc(Advanced batch processing for multiple simulations)pbdoc")
        .def(py::init<size_t>())
        .def("add_config", &batch::BatchSimulator::addConfig, py::arg("config_dict"))
        .def("add_world", &batch::BatchSimulator::addWorld, py::arg("config_dict"))
        .def("run_batch_simulations", &batch::BatchSimulator::runBatchSimulations,
             py::arg("sensory_inputs"), py::arg("timestep"), py::arg("steps"))
        .def("compare_brains", &batch::BatchSimulator::compareBrains)
        .def("clear", &batch::BatchSimulator::clear)
        .def("get_batch_size", &batch::BatchSimulator::getBatchSize)
        .def("set_batch_size", &batch::BatchSimulator::setBatchSize, py::arg("size"));
    
    m.def("run_batch_simulations", &batch::run_batch_simulations,
        py::arg("config_dicts"),
        py::arg("sensory_inputs"),
        py::arg("timestep"),
        py::arg("steps"),
        "Run multiple simulations efficiently");
    
    m.def("compare_brains", &batch::BatchSimulator::compareBrains,
        "Compare brains across configurations");

    py::class_<statistics>(m, "statistics", "Statistical analysis utilities");
    m.def("calculate_statistics", &statistics::calculateStatistics, py::arg("data"),
        "Calculate statistics from simulation data");
    m.def("calculate_neuron_population_stats", &statistics::calculateNeuronPopulationStats, py::arg("population_stats_list"),
        "Calculate neuron population statistics");
    m.def("compare_datasets", &statistics::compareDatasets, py::arg("datasets"),
        "Compare multiple datasets");

    // Register all existing bindings for backward compatibility
    
    // Register NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // Register SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Register RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Register PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Register NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Register SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Register DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Register FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Register ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    // Register MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // Register WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Register Config (existing class)
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Register SensoryInput (existing class)
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Register Vision (existing class)
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Register Audio (existing class)
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Register InternalSignals (existing class)
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Register Action (existing class)
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Register WorldObject (existing class)
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // Register AgentBody (existing class)
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // Register ActionResult (existing class)
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Register SensoryPercept (existing class)
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Register SimpleWorld (existing class)
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Register Brain (existing class)
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Register AgentBrain (existing class)
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Register existing factory functions for backward compatibility
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Register global constants for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
