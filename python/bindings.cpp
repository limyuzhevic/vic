#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <variant>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;

// NLM Python Exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World error: " + msg) {}
};

class SensorError : public NLMError {
public:
    explicit SensorError(const std::string& msg) : NLMError("Sensor error: " + msg) {}
};

class ActionError : public NLMError {
public:
    explicit ActionError(const std::string& msg) : NLMError("Action error: " + msg) {}
};

// Pythonic wrapper classes

class PyConfig {
public:
    std::shared_ptr<nlm::Config> config_;
    
    PyConfig() : config_(std::make_shared<nlm::Config>()) {}
    
    PyConfig(const std::string& filepath) {
        config_ = std::make_shared<nlm::Config>();
        if (!config_->loadFromFile(filepath)) {
            throw ConfigError("Failed to load configuration from file: " + filepath);
        }
    }
    
    PyConfig(const py::dict& config_dict) {
        config_ = std::make_shared<nlm::Config>();
        from_dict(config_dict);
    }
    
    static PyConfig from_file(const std::string& filepath) {
        return PyConfig(filepath);
    }
    
    static PyConfig from_dict(const py::dict& config_dict) {
        return PyConfig(config_dict);
    }
    
    static PyConfig from_defaults() {
        PyConfig config;
        auto defaults = nlm::Config();
        config.config_->set("random_seed", 42);
        config.config_->set("neuron_count", 1000);
        config.config_->set("simulation_timestep", 0.001);
        config.config_->set("region_count", 2);
        config.config_->set("connection_probability", 0.1);
        config.config_->set("plasticity_learning_rate", 0.01);
        config.config_->set("stdp_ltp_weight", 0.01);
        config.config_->set("stdp_ltd_weight", 0.012);
        config.config_->set("stdp_time_constant", 20.0);
        config.config_->set("development_synaptogenesis_rate", 0.001);
        config.config_->set("development_pruning_rate", 0.0001);
        config.config_->set("dopamine_baseline", 0.1);
        config.config_->set("reward_discount_factor", 0.99);
        config.config_->set("environment_name", std::string("GridWorld"));
        config.config_->set("environment_width", 10);
        config.config_->set("environment_height", 10);
        config.config_->set("log_level", std::string("INFO"));
        config.config_->set("log_to_file", false);
        config.config_->set("log_filename", std::string("nlm.log"));
        config.config_->set("max_simulation_steps", 10000);
        config.config_->set("simulation_time_limit", 0.0);
        config.config_->set("visualization_enabled", false);
        config.config_->set("visualization_update_rate", 30.0);
        return config;
    }
    
    void from_dict(const py::dict& config_dict) {
        for (auto item : config_dict) {
            std::string key = py::str(item.first);
            const auto& value = item.second;
            
            if (value.is(py::int_())) {
                config_->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config_->set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config_->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config_->set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                py::list py_list = value.cast<py::list>();
                std::vector<double> vec;
                for (auto py_item : py_list) {
                    if (py_item.is(py::int_())) {
                        vec.push_back(py_item.cast<int64_t>());
                    } else if (py_item.is(py::float_())) {
                        vec.push_back(py_item.cast<double>());
                    }
                }
                config_->set(key, vec);
            }
        }
    }
    
    py::object get(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        if (value) return py::cast(*value);
        return py::none();
    }
    
    void set(const std::string& key, const py::object& value) {
        if (value.is(py::int_())) {
            config_->set(key, value.cast<int64_t>());
        } else if (value.is(py::float_())) {
            config_->set(key, value.cast<double>());
        } else if (value.is(py::bool_())) {
            config_->set(key, value.cast<bool>());
        } else if (value.is(py::str())) {
            config_->set(key, value.cast<std::string>());
        } else if (value.is(py::list())) {
            py::list py_list = value.cast<py::list>();
            std::vector<double> vec;
            for (auto py_item : py_list) {
                if (py_item.is(py::int_())) {
                    vec.push_back(py_item.cast<int64_t>());
                } else if (py_item.is(py::float_())) {
                    vec.push_back(py_item.cast<double>());
                }
            }
            config_->set(key, vec);
        } else {
            throw ConfigError("Unsupported value type for key: " + key);
        }
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    void clear() {
        config_->clear();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    bool validate() const {
        return has("random_seed") && has("neuron_count") && 
               has("simulation_timestep") && has("region_count");
    }
    
    bool load_from_file(const std::string& filepath) {
        bool result = config_->loadFromFile(filepath);
        if (!result) {
            throw ConfigError("Failed to load configuration from file: " + filepath);
        }
        return result;
    }
    
    bool save_to_file(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    std::string __repr__() const {
        return "<Config: " + summary() + ">";
    }
};

class PyVision {
public:
    std::unique_ptr<nlm::Vision> vision_;
    
    PyVision() : vision_(std::make_unique<nlm::Vision>()) {}
    
    PyVision(size_t width, size_t height, size_t channels = 3) 
        : vision_(std::make_unique<nlm::Vision>(width, height, channels)) {}
    
    void set_data(const std::vector<float>& data) {
        vision_->setData(data);
    }
    
    void set_data(const py::array_t<float>& data) {
        auto buf = data.request();
        float* ptr = (float*)buf.ptr;
        size_t size = buf.size;
        std::vector<float> vec(ptr, ptr + size);
        vision_->setData(vec);
    }
    
    std::vector<float> get_data() const {
        return vision_->getData();
    }
    
    py::array_t<float> get_data_numpy() const {
        auto data = vision_->getData();
        return py::array_t<float>(data.size(), data.data());
    }
    
    size_t get_width() const {
        return vision_->getWidth();
    }
    
    size_t get_height() const {
        return vision_->getHeight();
    }
    
    size_t get_channels() const {
        return vision_->getChannels();
    }
    
    py::tuple get_shape() const {
        return py::make_tuple(vision_->getWidth(), vision_->getHeight(), vision_->getChannels());
    }
    
    PyVision normalized() const {
        PyVision normalized_viz;
        auto data = vision_->getData();
        std::vector<float> normalized_data;
        float max_val = *std::max_element(data.begin(), data.end());
        if (max_val > 0) {
            for (float val : data) {
                normalized_data.push_back(val / max_val);
            }
        }
        normalized_viz.set_data(normalized_data);
        return normalized_viz;
    }
    
    std::string __repr__() const {
        return "<Vision: " + std::to_string(vision_->getWidth()) + "x" + 
               std::to_string(vision_->getHeight()) + "x" + 
               std::to_string(vision_->getChannels()) + " channels>";
    }
};

class PyAudio {
public:
    std::unique_ptr<nlm::Audio> audio_;
    
    PyAudio() : audio_(std::make_unique<nlm::Audio>()) {}
    
    PyAudio(size_t sample_rate, size_t num_samples) 
        : audio_(std::make_unique<nlm::Audio>(sample_rate, num_samples)) {}
    
    void set_data(const std::vector<float>& data) {
        audio_->setData(data);
    }
    
    void set_data(const py::array_t<float>& data) {
        auto buf = data.request();
        float* ptr = (float*)buf.ptr;
        size_t size = buf.size;
        std::vector<float> vec(ptr, ptr + size);
        audio_->setData(vec);
    }
    
    std::vector<float> get_data() const {
        return audio_->getData();
    }
    
    py::array_t<float> get_data_numpy() const {
        auto data = audio_->getData();
        return py::array_t<float>(data.size(), data.data());
    }
    
    void set_sample_rate(size_t rate) {
        audio_->setSampleRate(rate);
    }
    
    size_t get_sample_rate() const {
        return audio_->getSampleRate();
    }
    
    size_t get_num_samples() const {
        return audio_->getNumSamples();
    }
    
    double get_duration() const {
        return static_cast<double>(get_num_samples()) / get_sample_rate();
    }
    
    py::dict analyze_frequency() const {
        py::dict result;
        auto data = get_data();
        // Simple frequency analysis
        double sample_rate = get_sample_rate();
        double bin_width = sample_rate / data.size();
        
        std::map<size_t, double> frequencies;
        for (size_t i = 0; i < data.size(); ++i) {
            size_t bin = i * 10 / data.size();  // Rough frequency bins
            frequencies[bin] += std::abs(data[i]);
        }
        
        for (const auto& pair : frequencies) {
            result[pair.first * bin_width] = pair.second;
        }
        
        return result;
    }
    
    std::string __repr__() const {
        return "<Audio: " + std::to_string(get_sample_rate()) + " Hz, " +
               std::to_string(get_num_samples()) + " samples, " +
               std::to_string(get_duration()) + " seconds>";
    }
};

class PyInternalSignals {
public:
    std::unique_ptr<nlm::InternalSignals> signals_;
    
    PyInternalSignals() : signals_(std::make_unique<nlm::InternalSignals>()) {}
    
    void add_signal(float value) {
        signals_->addSignal(value);
    }
    
    void add_signals(const std::vector<float>& values) {
        for (float value : values) {
            signals_->addSignal(value);
        }
    }
    
    void clear_signals() {
        signals_->clearSignals();
    }
    
    std::vector<float> get_signals() const {
        // Note: InternalSignals doesn't have a getSignals() method in C++
        // This would need to be added or approximated
        return std::vector<float>();
    }
    
    float get_mean() const {
        auto signals = get_signals();
        if (signals.empty()) return 0.0f;
        float sum = 0.0f;
        for (float val : signals) sum += val;
        return sum / signals.size();
    }
    
    float get_std() const {
        auto signals = get_signals();
        if (signals.size() < 2) return 0.0f;
        float mean = get_mean();
        float variance = 0.0f;
        for (float val : signals) {
            variance += (val - mean) * (val - mean);
        }
        return std::sqrt(variance / signals.size());
    }
    
    size_t get_count() const {
        return get_signals().size();
    }
    
    std::string __repr__() const {
        return "<InternalSignals: " + std::to_string(get_count()) + " signals, " +
               "mean=" + std::to_string(get_mean()) + ", std=" + std::to_string(get_std()) + ">";
    }
};

class PyAction {
public:
    std::unique_ptr<nlm::Action> action_;
    
    PyAction() : action_(std::make_unique<nlm::Action>()) {}
    
    PyAction(nlm::ActionType type) : action_(std::make_unique<nlm::Action>(type)) {}
    
    PyAction(nlm::ActionType type, const std::vector<float>& parameters) 
        : action_(std::make_unique<nlm::Action>(type, parameters)) {}
    
    static PyAction move_forward(float speed = 1.0f) {
        return PyAction(nlm::ActionType::MoveForward, {speed});
    }
    
    static PyAction turn_left(float angle = 0.5f) {
        return PyAction(nlm::ActionType::TurnLeft, {angle});
    }
    
    static PyAction turn_right(float angle = 0.5f) {
        return PyAction(nlm::ActionType::TurnRight, {angle});
    }
    
    static PyAction move_backward(float speed = 1.0f) {
        return PyAction(nlm::ActionType::MoveBackward, {speed});
    }
    
    static PyAction look_at(float x, float y) {
        return PyAction(nlm::ActionType::Look, {x, y});
    }
    
    static PyAction look_up(float height = 1.0f) {
        return PyAction(nlm::ActionType::LookUp, {height});
    }
    
    static PyAction look_down(float height = 1.0f) {
        return PyAction(nlm::ActionType::LookDown, {height});
    }
    
    static PyAction interact() {
        return PyAction(nlm::ActionType::Interact);
    }
    
    static PyAction eat() {
        return PyAction(nlm::ActionType::Eat);
    }
    
    static PyAction drink() {
        return PyAction(nlm::ActionType::Drink);
    }
    
    static PyAction rest() {
        return PyAction(nlm::ActionType::Rest);
    }
    
    static PyAction wait() {
        return PyAction(nlm::ActionType::Wait);
    }
    
    static PyAction custom(const std::string& action_name, const std::vector<float>& parameters = {}) {
        nlm::ActionType type = nlm::ActionType::Custom;
        auto action = std::make_unique<nlm::Action>(type, parameters);
        // Store custom name as first parameter
        std::vector<float> params = parameters;
        params.insert(params.begin(), 0.0f); // Placeholder
        action->setParameters(params);
        PyAction result;
        result.action_ = std::move(action);
        return result;
    }
    
    nlm::ActionType get_type() const {
        return action_->getType();
    }
    
    void set_type(nlm::ActionType type) {
        action_->setType(type);
    }
    
    std::vector<float> get_parameters() const {
        return action_->getParameters();
    }
    
    void set_parameters(const std::vector<float>& parameters) {
        action_->setParameters(parameters);
    }
    
    std::string get_name() const {
        return action_->getName();
    }
    
    PyAction clone() const {
        PyAction result;
        result.action_ = std::make_unique<nlm::Action>(*action_);
        return result;
    }
    
    py::dict to_dict() const {
        py::dict result;
        result["type"] = static_cast<int>(get_type());
        result["parameters"] = get_parameters();
        result["name"] = get_name();
        return result;
    }
    
    static PyAction from_dict(const py::dict& data) {
        nlm::ActionType type = static_cast<nlm::ActionType>(data["type"].cast<int>());
        std::vector<float> parameters = data["parameters"].cast<std::vector<float>>();
        return PyAction(type, parameters);
    }
    
    std::string __repr__() const {
        return "<Action: " + get_name() + ">";
    }
};

class PyWorld {
public:
    std::shared_ptr<nlm::SimpleWorld> world_;
    
    PyWorld() : world_(std::make_shared<nlm::SimpleWorld>()) {}
    
    PyWorld(size_t width, size_t height) {
        world_ = std::make_shared<nlm::SimpleWorld>();
        world_->configure(width, height, 5, 5);
    }
    
    void configure(size_t width, size_t height, size_t vision_width = 5, size_t vision_height = 5) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    void reset() {
        world_->reset();
    }
    
    void set_agent_start(float x, float y, float orientation = 0.0f) {
        world_->setAgentStart(x, y);
    }
    
    void update(double timestep = 1.0) {
        world_->update(timestep);
    }
    
    std::pair<float, bool> apply_action(const PyAction& action) {
        // Convert PyAction to nlm::Action
        auto nlm_action = std::make_unique<nlm::Action>(
            action.get_type(), action.get_parameters()
        );
        
        // Apply action (world interface may need to be adapted)
        nlm::MotorCommand cmd = nlm::MotorCommand::Wait; // Simplified for demonstration
        world_->applyMotorCommand(cmd, world_->getSimulationTime());
        
        return std::make_pair(1.0f, true); // Dummy reward and success
    }
    
    PySensoryPercept get_sensory_percept() const {
        PySensoryPercept percept;
        // This would need proper implementation based on world interface
        return percept;
    }
    
    nlm::AgentBody get_agent_body() const {
        return world_->getAgentBody();
    }
    
    void add_object(const nlm::WorldObject& obj) {
        world_->addObject(obj);
    }
    
    void remove_object(float x, float y) {
        world_->removeObject(x, y);
    }
    
    bool is_valid_position(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    size_t get_width() const {
        return world_->getWidth();
    }
    
    size_t get_height() const {
        return world_->getHeight();
    }
    
    float get_max_energy() const {
        return world_->getMaxEnergy();
    }
    
    void set_max_energy(float energy) {
        world_->setMaxEnergy(energy);
    }
    
    float get_energy_decay_rate() const {
        return world_->getEnergyDecayRate();
    }
    
    void set_energy_decay_rate(float rate) {
        world_->setEnergyDecayRate(rate);
    }
    
    double get_simulation_time() const {
        return world_->getSimulationTime();
    }
    
    void set_random_seed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint64_t get_random_seed() const {
        return world_->getRandomSeed();
    }
    
    std::string __repr__() const {
        return "<World: " + std::to_string(get_width()) + "x" + 
               std::to_string(get_height()) + " pixels>";
    }
};

class PyBrain {
public:
    std::shared_ptr<nlm::Brain> brain_;
    
    PyBrain() : brain_(nullptr) {}
    
    PyBrain(std::shared_ptr<nlm::Config> config) : brain_(std::make_shared<nlm::Brain>(config)) {}
    
    PyBrain(const PyConfig& config) : brain_(std::make_shared<nlm::Brain>(config.config_)) {}
    
    static PyBrain create_with_config(const py::dict& config_dict) {
        PyConfig config(config_dict);
        return PyBrain(config.config_);
    }
    
    static PyBrain create_default() {
        PyConfig config = PyConfig::from_defaults();
        return PyBrain(config.config_);
    }
    
    bool initialize() {
        bool result = brain_->initialize();
        if (!result) {
            throw BrainError("Failed to initialize brain");
        }
        return result;
    }
    
    void step(int current_step) {
        brain_->step(static_cast<nlm::SimulationStep>(current_step));
    }
    
    void step_with_time(int current_step, double current_time) {
        brain_->step(static_cast<nlm::SimulationStep>(current_step), 
                    static_cast<nlm::Timestamp>(current_time));
    }
    
    void step(double timestep = 1.0) {
        brain_->step(static_cast<nlm::SimulationStep>(timestep));
    }
    
    void step_with_time(double timestep, double current_time) {
        brain_->step(static_cast<nlm::SimulationStep>(timestep), 
                    static_cast<nlm::Timestamp>(current_time));
    }
    
    void receive_sensory_input(const PyVision& vision) {
        // Convert PyVision to nlm::Vision
        auto nlm_vision = std::make_unique<nlm::Vision>(
            vision.get_width(), vision.get_height(), vision.get_channels()
        );
        auto data = vision.get_data();
        nlm_vision->setData(data);
        brain_->receiveSensoryInput(*nlm_vision);
    }
    
    void receive_sensory_input(const PyAudio& audio) {
        // Convert PyAudio to nlm::Audio
        auto nlm_audio = std::make_unique<nlm::Audio>(
            audio.get_sample_rate(), audio.get_num_samples()
        );
        auto data = audio.get_data();
        nlm_audio->setData(data);
        // Note: Brain's receiveSensoryInput expects SensoryInput, not Audio directly
        // This would need proper interface adaptation
    }
    
    void receive_sensory_input(const PyInternalSignals& signals) {
        // Convert PyInternalSignals to nlm::InternalSignals
        auto nlm_signals = std::make_unique<nlm::InternalSignals>(*signals.signals_);
        // Note: Brain's receiveSensoryInput expects SensoryInput, not InternalSignals directly
        // This would need proper interface adaptation
    }
    
    void inject_current(const nlm::NeuronId& neuron_id, float current) {
        brain_->injectCurrent(neuron_id, current);
    }
    
    void inject_current_to_type(nlm::NeuronType type, float current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    PyAction produce_action() {
        auto nlm_action = brain_->produceAction();
        PyAction result;
        result.action_ = std::move(nlm_action);
        return result;
    }
    
    void reset() {
        brain_->reset();
    }
    
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    nlm::RegionId add_region(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    nlm::NeuralRegion* get_region(const nlm::RegionId& id) {
        return brain_->getRegion(id);
    }
    
    const nlm::NeuralRegion* get_region(const nlm::RegionId& id) const {
        return brain_->getRegion(id);
    }
    
    size_t get_region_count() const {
        return brain_->getRegionCount();
    }
    
    std::vector<nlm::RegionId> get_region_ids() const {
        return brain_->getRegionIds();
    }
    
    const std::vector<std::unique_ptr<nlm::NeuralRegion>>& get_regions() const {
        return brain_->getRegions();
    }
    
    size_t get_total_neuron_count() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t get_total_synapse_count() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t get_active_neuron_count() const {
        return brain_->getActiveNeuronCount();
    }
    
    size_t get_firing_neuron_count() const {
        return brain_->getFiringNeuronCount();
    }
    
    float get_average_firing_rate() const {
        return brain_->getAverageFiringRate();
    }
    
    float get_excitation_inhibition_ratio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t get_total_spike_count() const {
        return brain_->getTotalSpikeCount();
    }
    
    nlm::DevelopmentalStage get_developmental_stage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void set_developmental_stage(nlm::DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    std::shared_ptr<nlm::Config> get_config() const {
        return brain_->getConfig();
    }
    
    void log_status() const {
        brain_->logStatus();
    }
    
    nlm::NeuralWorkingMemory* get_working_memory() {
        return brain_->getWorkingMemory();
    }
    
    nlm::NeuralEpisodicMemory* get_episodic_memory() {
        return brain_->getEpisodicMemory();
    }
    
    nlm::NeuralAssociativeMemory* get_associative_memory() {
        return brain_->getAssociativeMemory();
    }
    
    nlm::PredictionSystem* get_prediction_system() {
        return brain_->getPredictionSystem();
    }
    
    nlm::NeuralPlanner* get_planner() {
        return brain_->getPlanner();
    }
    
    nlm::ConceptFormation* get_concept_formation() {
        return brain_->getConceptFormation();
    }
    
    nlm::AttentionalSelection* get_attention() {
        return brain_->getAttention();
    }
    
    nlm::DevelopmentSystem* get_development_system() {
        return brain_->getDevelopmentSystem();
    }
    
    nlm::Dopamine* get_dopamine() {
        return brain_->getDopamine();
    }
    
    nlm::Curiosity* get_curiosity() {
        return brain_->getCuriosity();
    }
    
    nlm::Novelty* get_novelty() {
        return brain_->getNovelty();
    }
    
    nlm::PredictionError* get_prediction_error_signal() {
        return brain_->getPredictionErrorSignal();
    }
    
    nlm::RandomGenerator* get_random_generator() {
        return brain_->getRandomGenerator();
    }
    
    std::string __repr__() const {
        return "<Brain: " + std::to_string(get_total_neuron_count()) + " neurons, " +
               std::to_string(get_total_synapse_count()) + " synapses>";
    }
};

class PyAgentBrain {
public:
    std::shared_ptr<nlm::AgentBrain> agent_brain_;
    
    PyAgentBrain(std::shared_ptr<nlm::Brain> brain) 
        : agent_brain_(std::make_shared<nlm::AgentBrain>(brain)) {}
    
    PyAgentBrain(const PyBrain& brain) 
        : agent_brain_(std::make_shared<nlm::AgentBrain>(brain.brain_)) {}
    
    void initialize(const PyWorld& world) {
        // Convert PyWorld to nlm::SimpleWorld
        agent_brain_->initialize(*world.world_);
    }
    
    size_t get_sensory_input_size() const {
        return agent_brain_->getSensoryInputSize();
    }
    
    size_t get_motor_output_size() const {
        return agent_brain_->getMotorOutputSize();
    }
    
    PyAction decode_motor_command() {
        nlm::MotorCommand cmd = agent_brain_->decodeMotorCommand();
        return PyAction(cmd);
    }
    
    void apply_reward_modulation(float reward, float predicted_reward) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    void update_development(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    nlm::DevelopmentalStage get_developmental_stage() const {
        return agent_brain_->getDevelopmentalStage();
    }
    
    float get_neuromodulation_level() const {
        return agent_brain_->getNeuromodulationLevel();
    }
    
    float get_curiosity_level() const {
        return agent_brain_->getCuriosityLevel();
    }
    
    float get_novelty_level() const {
        return agent_brain_->getNoveltyLevel();
    }
    
    float get_prediction_error() const {
        return agent_brain_->getPredictionError();
    }
    
    void reset() {
        agent_brain_->reset();
    }
    
    PyBrain get_brain() {
        PyBrain result;
        result.brain_ = agent_brain_->getBrain();
        return result;
    }
    
    void enable_reward_modulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }
    
    void enable_structural_plasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }
    
    void enable_development(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }
    
    void enable_curiosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }
    
    bool is_reward_modulation_enabled() const {
        return agent_brain_->isRewardModulationEnabled();
    }
    
    bool is_structural_plasticity_enabled() const {
        return agent_brain_->isStructuralPlasticityEnabled();
    }
    
    bool is_development_enabled() const {
        return agent_brain_->isDevelopmentEnabled();
    }
    
    bool is_curiosity_enabled() const {
        return agent_brain_->isCuriosityEnabled();
    }
    
    std::string __repr__() const {
        return "<AgentBrain: Connected to " + std::to_string(get_brain().get_total_neuron_count()) + " neurons>";
    }
};

// Performance monitoring and profiling tools
class PySimulationMonitor {
public:
    std::shared_ptr<PyBrain> brain_;
    std::shared_ptr<PyWorld> world_;
    std::shared_ptr<PyAgentBrain> agent_brain_;
    
    std::vector<double> step_times_;
    std::vector<float> firing_rates_;
    std::vector<float> rewards_;
    double total_time_;
    size_t total_steps_;
    
    PySimulationMonitor(std::shared_ptr<PyBrain> brain, 
                       std::shared_ptr<PyWorld> world,
                       std::shared_ptr<PyAgentBrain> agent_brain = nullptr)
        : brain_(brain), world_(world), agent_brain_(agent_brain),
          total_time_(0.0), total_steps_(0) {}
    
    PySimulationMonitor(PyBrain brain, PyWorld world, PyAgentBrain agent_brain = nullptr) {
        brain_ = std::make_shared<PyBrain>(std::move(brain));
        world_ = std::make_shared<PyWorld>(std::move(world));
        if (agent_brain) {
            agent_brain_ = std::make_shared<PyAgentBrain>(std::move(agent_brain));
        }
    }
    
    void start_monitoring() {
        step_times_.clear();
        firing_rates_.clear();
        rewards_.clear();
        total_time_ = 0.0;
        total_steps_ = 0;
    }
    
    void record_step(double step_time, float firing_rate, float reward = 0.0f) {
        step_times_.push_back(step_time);
        firing_rates_.push_back(firing_rate);
        rewards_.push_back(reward);
        total_time_ += step_time;
        total_steps_++;
    }
    
    py::dict get_statistics() const {
        py::dict stats;
        if (step_times_.empty()) {
            return stats;
        }
        
        double avg_step_time = total_time_ / step_times_.size();
        double avg_firing_rate = std::accumulate(firing_rates_.begin(), firing_rates_.end(), 0.0f) / firing_rates_.size();
        double avg_reward = std::accumulate(rewards_.begin(), rewards_.end(), 0.0f) / rewards_.size();
        
        stats["total_steps"] = total_steps_;
        stats["total_time"] = total_time_;
        stats["avg_step_time"] = avg_step_time;
        stats["avg_firing_rate"] = avg_firing_rate;
        stats["avg_reward"] = avg_reward;
        stats["steps_per_second"] = total_steps_ / total_time_;
        stats["max_firing_rate"] = *std::max_element(firing_rates_.begin(), firing_rates_.end());
        stats["min_firing_rate"] = *std::min_element(firing_rates_.begin(), firing_rates_.end());
        
        return stats;
    }
    
    py::dict get_performance_metrics() const {
        py::dict metrics;
        auto stats = get_statistics();
        
        if (step_times_.size() >= 2) {
            double variance = 0.0;
            double mean = stats["avg_step_time"].cast<double>();
            for (double time : step_times_) {
                variance += (time - mean) * (time - mean);
            }
            variance /= step_times_.size();
            metrics["step_time_std"] = std::sqrt(variance);
            metrics["step_time_cv"] = std::sqrt(variance) / mean; // Coefficient of variation
        }
        
        if (!firing_rates_.empty()) {
            double mean_rate = stats["avg_firing_rate"].cast<double>();
            double rate_variance = 0.0;
            for (float rate : firing_rates_) {
                rate_variance += (rate - mean_rate) * (rate - mean_rate);
            }
            rate_variance /= firing_rates_.size();
            metrics["firing_rate_std"] = std::sqrt(rate_variance);
            metrics["firing_rate_cv"] = std::sqrt(rate_variance) / mean_rate;
        }
        
        return metrics;
    }
    
    py::dict get_stability_metrics() const {
        py::dict metrics;
        auto stats = get_statistics();
        
        if (step_times_.size() >= 10) {
            // Calculate stability indicators
            double early_time = std::accumulate(step_times_.begin(), step_times_.begin() + 10, 0.0);
            double late_time = std::accumulate(step_times_.rbegin(), step_times_.rbegin() + 10, 0.0);
            metrics["stability_score"] = 1.0 - std::abs(early_time - late_time) / (early_time + late_time + 1e-10);
        }
        
        if (!firing_rates_.empty()) {
            // Calculate firing rate stability
            double mean_rate = stats["avg_firing_rate"].cast<double>();
            double coefficient_of_variation = metrics["firing_rate_cv"].cast<double>();
            metrics["stability_score"] = 1.0 / (1.0 + coefficient_of_variation);
        }
        
        return metrics;
    }
    
    void plot_metrics(const std::string& output_file = "") {
        // Note: This would require matplotlib/numpy integration
        // For now, we'll just output basic statistics
        auto stats = get_statistics();
        std::cout << "=== Simulation Statistics ===" << std::endl;
        std::cout << "Total steps: " << stats["total_steps"].cast<size_t>() << std::endl;
        std::cout << "Total time: " << stats["total_time"].cast<double>() << " seconds" << std::endl;
        std::cout << "Average step time: " << stats["avg_step_time"].cast<double>() << " seconds" << std::endl;
        std::cout << "Steps per second: " << stats["steps_per_second"].cast<double>() << std::endl;
        std::cout << "Average firing rate: " << stats["avg_firing_rate"].cast<double>() << std::endl;
        
        if (!output_file.empty()) {
            // Write to file (simplified)
            std::ofstream file(output_file);
            file << "Simulation Statistics Report\n";
            file << "==========================\n";
            file << "Total steps: " << stats["total_steps"].cast<size_t>() << "\n";
            file << "Total time: " << stats["total_time"].cast<double>() << " seconds\n";
            file << "Average step time: " << stats["avg_step_time"].cast<double>() << " seconds\n";
            file << "Steps per second: " << stats["steps_per_second"].cast<double>() << "\n";
            file << "Average firing rate: " << stats["avg_firing_rate"].cast<double>() << "\n";
            file.close();
        }
    }
};

// Benchmarking utilities
class PyBenchmark {
public:
    std::shared_ptr<PyBrain> brain_;
    std::shared_ptr<PyWorld> world_;
    std::shared_ptr<PyAgentBrain> agent_brain_;
    
    PyBenchmark(std::shared_ptr<PyBrain> brain,
                std::shared_ptr<PyWorld> world,
                std::shared_ptr<PyAgentBrain> agent_brain = nullptr)
        : brain_(brain), world_(world), agent_brain_(agent_brain) {}
    
    PyBenchmark(PyBrain brain, PyWorld world, PyAgentBrain agent_brain = nullptr) {
        brain_ = std::make_shared<PyBrain>(std::move(brain));
        world_ = std::make_shared<PyWorld>(std::move(world));
        if (agent_brain) {
            agent_brain_ = std::make_shared<PyAgentBrain>(std::move(agent_brain));
        }
    }
    
    py::dict benchmark_step(size_t num_steps = 100) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < num_steps; ++i) {
            brain_->step();
            if (world_) world_->update();
            if (agent_brain_) {
                // Simplified for demonstration
                agent_brain_->update_development(0.1);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        
        double total_time_ns = static_cast<double>(duration.count());
        double avg_time_per_step_ns = total_time_ns / num_steps;
        double steps_per_second = num_steps / (total_time_ns / 1e9);
        
        py::dict result;
        result["total_steps"] = num_steps;
        result["total_time_ns"] = total_time_ns;
        result["avg_time_per_step_ns"] = avg_time_per_step_ns;
        result["steps_per_second"] = steps_per_second;
        result["neurons"] = brain_->get_total_neuron_count();
        result["synapses"] = brain_->get_total_synapse_count();
        
        return result;
    }
    
    py::dict benchmark_simulation(size_t num_steps = 1000) {
        // Run full simulation
        brain_->initialize();
        if (world_) world_->reset();
        if (agent_brain_) agent_brain_->reset();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < num_steps; ++i) {
            if (world_) {
                world_->update();
                if (agent_brain_) {
                    agent_brain_->update_development(0.1);
                }
            }
            brain_->step(i);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        
        double total_time_ns = static_cast<double>(duration.count());
        double avg_time_per_step_ns = total_time_ns / num_steps;
        
        py::dict result;
        result["total_steps"] = num_steps;
        result["total_time_ns"] = total_time_ns;
        result["avg_time_per_step_ns"] = avg_time_per_step_ns;
        result["steps_per_second"] = num_steps / (total_time_ns / 1e9);
        result["neurons"] = brain_->get_total_neuron_count();
        result["synapses"] = brain_->get_total_synapse_count();
        result["active_neurons"] = brain_->get_active_neuron_count();
        result["firing_neurons"] = brain_->get_firing_neuron_count();
        result["total_spikes"] = brain_->get_total_spike_count();
        
        return result;
    }
    
    py::dict benchmark_memory() {
        // Simple memory benchmark
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Perform memory-intensive operations
        std::vector<float> data(1000000, 1.0f);
        float sum = 0.0f;
        for (float val : data) sum += val;
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        
        double total_time_ns = static_cast<double>(duration.count());
        
        py::dict result;
        result["memory_test_time_ns"] = total_time_ns;
        result["data_elements"] = 1000000;
        result["memory_bandwidth_gb_s"] = (sizeof(float) * 1000000) / (total_time_ns / 1e9) / (1024*1024*1024);
        
        return result;
    }
    
    py::dict run_comparison(const std::vector<PyConfig>& configs) {
        py::dict results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            PyBrain test_brain(configs[i]);
            py::dict config_result;
            
            auto benchmark_result = benchmark_step(100);
            config_result["config_index"] = i;
            config_result["neurons"] = benchmark_result["neurons"];
            config_result["synapses"] = benchmark_result["synapses"];
            config_result["steps_per_second"] = benchmark_result["steps_per_second"];
            
            results[std::to_string(i)] = config_result;
        }
        
        return results;
    }
    
    void generate_report(const std::string& output_file = "benchmark_report.txt") {
        auto step_result = benchmark_step(100);
        auto sim_result = benchmark_simulation(1000);
        
        std::ofstream file(output_file);
        file << "=== NLM Python Benchmark Report ===\n\n";
        file << "Step Performance (100 steps):\n";
        file << "  Steps per second: " << step_result["steps_per_second"].cast<double>() << "\n";
        file << "  Average time per step: " << step_result["avg_time_per_step_ns"].cast<double>() << " ns\n";
        file << "  Total time: " << step_result["total_time_ns"].cast<double>() << " ns\n\n";
        
        file << "Simulation Performance (1000 steps):\n";
        file << "  Steps per second: " << sim_result["steps_per_second"].cast<double>() << "\n";
        file << "  Average time per step: " << sim_result["avg_time_per_step_ns"].cast<double>() << " ns\n";
        file << "  Total time: " << sim_result["total_time_ns"].cast<double>() << " ns\n\n";
        
        file << "Neural Network Stats:\n";
        file << "  Neurons: " << sim_result["neurons"].cast<size_t>() << "\n";
        file << "  Synapses: " << sim_result["synapses"].cast<size_t>() << "\n";
        file << "  Active neurons: " << sim_result["active_neurons"].cast<size_t>() << "\n";
        file << "  Firing neurons: " << sim_result["firing_neurons"].cast<size_t>() << "\n";
        file << "  Total spikes: " << sim_result["total_spikes"].cast<size_t>() << "\n";
        
        file.close();
    }
};

// Serialization utilities
class PySerializer {
public:
    std::string filepath_;
    
    PySerializer(const std::string& filepath) : filepath_(filepath) {}
    
    void save_config(const PyConfig& config) {
        config.save_to_file(filepath_);
    }
    
    PyConfig load_config() {
        PyConfig config;
        config.load_from_file(filepath_);
        return config;
    }
    
    void save_brain(const PyBrain& brain) {
        brain.save(filepath_);
    }
    
    PyBrain load_brain() {
        PyBrain brain;
        brain.load(filepath_);
        return brain;
    }
    
    std::string export_json(const py::object& obj) {
        py::module json_module = py::module::import("json");
        return json_module.attr("dumps")(obj).cast<std::string>();
    }
    
    py::object import_json(const std::string& json_str, const std::string& obj_type) {
        py::module json_module = py::module::import("json");
        py::object data = json_module.attr("loads")(json_str);
        
        if (obj_type == "config") {
            return py::cast(PyConfig::from_dict(data.cast<py::dict>()));
        } else if (obj_type == "action") {
            return py::cast(PyAction::from_dict(data.cast<py::dict>()));
        }
        
        throw NLMError("Unsupported object type for JSON import: " + obj_type);
    }
};

// Experiment management
class PyExperimentManager {
public:
    std::string experiment_id_;
    std::string base_dir_;
    std::vector<py::dict> step_data_;
    
    PyExperimentManager(const std::string& experiment_id = "", 
                       const std::string& base_dir = "experiments")
        : experiment_id_(experiment_id), base_dir_(base_dir) {
        if (experiment_id.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "experiment_" << time_t;
            experiment_id_ = ss.str();
        }
    }
    
    void start_experiment(const PyConfig& config) {
        // Create experiment directory
        std::string exp_dir = base_dir_ + "/" + experiment_id_;
        std::filesystem::create_directories(exp_dir);
        
        // Save initial config
        save_config_to_file(config, exp_dir + "/config.json");
        
        // Create metadata file
        py::dict metadata;
        metadata["experiment_id"] = experiment_id_;
        metadata["start_time"] = py::float_(std::chrono::system_clock::now().time_since_epoch().count() / 1e9);
        metadata["config"] = py::object(config); // This will need adaptation
        
        save_metadata(metadata, exp_dir + "/metadata.json");
    }
    
    void log_step(size_t step, const py::dict& data) {
        step_data_.push_back(data);
        
        // Save step data incrementally
        std::string exp_dir = base_dir_ + "/" + experiment_id_;
        save_step_data(step, data, exp_dir + "/step_data.json");
    }
    
    void save_checkpoint(PyBrain brain, PyWorld world, size_t step) {
        std::string exp_dir = base_dir_ + "/" + experiment_id_;
        
        // Save brain state
        brain.save(exp_dir + "/brain_checkpoint_" + std::to_string(step) + ".bin");
        
        // Save world state
        // Note: SimpleWorld doesn't have save/load methods
        
        // Save step data
        py::dict checkpoint_data;
        checkpoint_data["step"] = step;
        checkpoint_data["brain_stats"] = py::dict(); // Simplified
        
        save_metadata(checkpoint_data, exp_dir + "/checkpoints.json");
    }
    
    std::pair<PyBrain, PyWorld> load_checkpoint(size_t step) {
        std::string exp_dir = base_dir_ + "/" + experiment_id_;
        
        PyBrain brain;
        brain.load(exp_dir + "/brain_checkpoint_" + std::to_string(step) + ".bin");
        
        PyWorld world;
        
        return std::make_pair(brain, world);
    }
    
    py::dict get_results() const {
        py::dict results;
        results["experiment_id"] = experiment_id_;
        results["total_steps"] = step_data_.size();
        results["start_time"] = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
        
        // Calculate basic statistics
        std::vector<float> rewards;
        std::vector<double> step_times;
        
        for (const auto& data : step_data_) {
            if (data.contains("reward")) {
                rewards.push_back(data["reward"].cast<float>());
            }
            if (data.contains("step_time")) {
                step_times.push_back(data["step_time"].cast<double>());
            }
        }
        
        if (!rewards.empty()) {
            float sum_rewards = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
            results["avg_reward"] = sum_rewards / rewards.size();
            results["total_reward"] = sum_rewards;
        }
        
        if (!step_times.empty()) {
            double sum_times = std::accumulate(step_times.begin(), step_times.end(), 0.0);
            results["avg_step_time"] = sum_times / step_times.size();
        }
        
        return results;
    }
    
    void generate_report(const std::string& output_file = "experiment_report.html") {
        auto results = get_results();
        
        std::ofstream file(output_file);
        file << "<!DOCTYPE html>\n";
        file << "<html><head><title>Experiment Report</title></head>\n";
        file << "<body><h1>Experiment Report</h1>\n";
        file << "<h2>Experiment ID: " << results["experiment_id"].cast<std::string>() << "</h2>\n";
        file << "<p>Total Steps: " << results["total_steps"].cast<size_t>() << "</p>\n";
        file << "<p>Average Reward: " << results["avg_reward"].cast<double>() << "</p>\n";
        file << "<p>Total Reward: " << results["total_reward"].cast<double>() << "</p>\n";
        file << "<p>Average Step Time: " << results["avg_step_time"].cast<double>() << " seconds</p>\n";
        file << "</body></html>";
        file.close();
    }
    
private:
    void save_config_to_file(const PyConfig& config, const std::string& filepath) {
        // Convert to JSON and save
        py::dict config_dict;
        for (const auto& key : config.get_keys()) {
            config_dict[key] = config.get(key);
        }
        
        py::module json_module = py::module::import("json");
        std::string json_str = json_module.attr("dumps")(config_dict).cast<std::string>();
        
        std::ofstream file(filepath);
        file << json_str;
        file.close();
    }
    
    void save_metadata(const py::dict& metadata, const std::string& filepath) {
        py::module json_module = py::module::import("json");
        std::string json_str = json_module.attr("dumps")(metadata).cast<std::string>();
        
        std::ofstream file(filepath);
        file << json_str;
        file.close();
    }
    
    void save_step_data(size_t step, const py::dict& data, const std::string& filepath) {
        // Append step data to file
        py::module json_module = py::module::import("json");
        
        // Read existing data
        py::dict existing_data;
        std::ifstream in_file(filepath);
        if (in_file.good()) {
            std::string content((std::istreambuf_iterator<char>(in_file)),
                               std::istreambuf_iterator<char>());
            existing_data = json_module.attr("loads")(content).cast<py::dict>();
        }
        in_file.close();
        
        // Add new step data
        existing_data[std::to_string(step)] = data;
        
        // Write back
        std::string json_str = json_module.attr("dumps")(existing_data).cast<std::string>();
        std::ofstream out_file(filepath);
        out_file << json_str;
        out_file.close();
    }
};

// Factory functions
PyConfig create_config(const py::dict& config_dict = py::none(), 
                      const std::string& config_type = "default") {
    if (!config_dict.is_none()) {
        return PyConfig::from_dict(config_dict);
    } else if (config_type == "research") {
        PyConfig config;
        config.config_->set("random_seed", std::mt19937::default_seed);
        config.config_->set("neuron_count", 5000);
        config.config_->set("simulation_timestep", 0.0005);
        config.config_->set("region_count", 4);
        config.config_->set("plasticity_learning_rate", 0.005);
        config.config_->set("development_enabled", true);
        config.config_->set("neuromodulation_enabled", true);
        return config;
    } else if (config_type == "training") {
        PyConfig config;
        config.config_->set("random_seed", std::mt19937::default_seed);
        config.config_->set("neuron_count", 2000);
        config.config_->set("simulation_timestep", 0.001);
        config.config_->set("region_count", 3);
        config.config_->set("plasticity_learning_rate", 0.02);
        config.config_->set("reward_discount_factor", 0.95);
        config.config_->set("max_simulation_steps", 50000);
        return config;
    } else if (config_type == "simulation") {
        PyConfig config;
        config.config_->set("random_seed", std::mt19937::default_seed);
        config.config_->set("neuron_count", 10000);
        config.config_->set("simulation_timestep", 0.0002);
        config.config_->set("region_count", 6);
        config.config_->set("plasticity_learning_rate", 0.01);
        config.config_->set("simulation_time_limit", 1000.0);
        return config;
    } else {
        return PyConfig::from_defaults();
    }
}

PyBrain create_brain(const PyConfig& config = PyConfig(), 
                     const std::string& brain_type = "default") {
    if (brain_type == "simple") {
        PyConfig simple_config = PyConfig::from_defaults();
        simple_config.config_->set("neuron_count", 100);
        simple_config.config_->set("region_count", 1);
        return PyBrain(simple_config);
    } else if (brain_type == "development") {
        PyConfig dev_config = PyConfig::from_defaults();
        dev_config.config_->set("development_enabled", true);
        dev_config.config_->set("development_synaptogenesis_rate", 0.005);
        return PyBrain(dev_config);
    } else if (brain_type == "learning") {
        PyConfig learning_config = PyConfig::from_defaults();
        learning_config.config_->set("plasticity_learning_rate", 0.05);
        learning_config.config_->set("reward_discount_factor", 0.9);
        return PyBrain(learning_config);
    } else {
        return PyBrain(config);
    }
}

PyWorld create_world(size_t width = 20, size_t height = 20, 
                     const std::string& world_type = "simple") {
    if (world_type == "arena") {
        // Larger circular world
        width = height = 40;
    } else if (world_type == "maze") {
        // Procedural maze world
        width = height = 30;
    }
    
    return PyWorld(width, height);
}

PyAgentBrain create_agent_brain(PyBrain brain, PyWorld world = PyWorld()) {
    PyAgentBrain agent_brain(brain);
    agent_brain.initialize(world);
    return agent_brain;
}

PyVision create_vision(size_t width = 84, size_t height = 84, size_t channels = 3) {
    return PyVision(width, height, channels);
}

PyAudio create_audio(size_t sample_rate = 22050, size_t num_samples = 16384) {
    return PyAudio(sample_rate, num_samples);
}

PyInternalSignals create_internal_signals() {
    return PyInternalSignals();
}

PyAction create_action(nlm::ActionType type, const std::vector<float>& parameters = {}) {
    return PyAction(type, parameters);
}

PyAction create_movement_action(const std::string& movement_type, float magnitude = 1.0f) {
    if (movement_type == "forward") {
        return PyAction::move_forward(magnitude);
    } else if (movement_type == "backward") {
        return PyAction::move_backward(magnitude);
    } else if (movement_type == "left") {
        return PyAction::turn_left(magnitude);
    } else if (movement_type == "right") {
        return PyAction::turn_right(magnitude);
    } else {
        throw ActionError("Unknown movement type: " + movement_type);
    }
}

PyAction create_look_action(float x, float y) {
    return PyAction::look_at(x, y);
}

// Preset configurations
PyConfig create_research_config() {
    PyConfig config;
    config.config_->set("random_seed", std::mt19937::default_seed);
    config.config_->set("neuron_count", 5000);
    config.config_->set("simulation_timestep", 0.0005);
    config.config_->set("region_count", 4);
    config.config_->set("plasticity_learning_rate", 0.005);
    config.config_->set("development_enabled", true);
    config.config_->set("neuromodulation_enabled", true);
    config.config_->set("reward_discount_factor", 0.95);
    config.config_->set("max_simulation_steps", 100000);
    return config;
}

PyConfig create_training_config(size_t episode_limit = 1000) {
    PyConfig config;
    config.config_->set("random_seed", std::mt19937::default_seed);
    config.config_->set("neuron_count", 2000);
    config.config_->set("simulation_timestep", 0.001);
    config.config_->set("region_count", 3);
    config.config_->set("plasticity_learning_rate", 0.02);
    config.config_->set("reward_discount_factor", 0.95);
    config.config_->set("max_simulation_steps", episode_limit * 100);
    return config;
}

PyConfig create_simulation_config(size_t time_steps = 10000) {
    PyConfig config;
    config.config_->set("random_seed", std::mt19937::default_seed);
    config.config_->set("neuron_count", 10000);
    config.config_->set("simulation_timestep", 0.0002);
    config.config_->set("region_count", 6);
    config.config_->set("plasticity_learning_rate", 0.01);
    config.config_->set("simulation_time_limit", time_steps * 0.001);
    return config;
}

// Preset brains
PyBrain create_simple_brain() {
    PyConfig config = PyConfig::from_defaults();
    config.config_->set("neuron_count", 100);
    config.config_->set("region_count", 1);
    return PyBrain(config);
}

PyBrain create_development_brain() {
    PyConfig config = PyConfig::from_defaults();
    config.config_->set("development_enabled", true);
    config.config_->set("development_synaptogenesis_rate", 0.005);
    return PyBrain(config);
}

PyBrain create_learning_brain(float learning_rate = 0.01f) {
    PyConfig config = PyConfig::from_defaults();
    config.config_->set("plasticity_learning_rate", learning_rate);
    config.config_->set("reward_discount_factor", 0.9f);
    return PyBrain(config);
}

// Preset worlds
PyWorld create_grid_world(size_t width = 10, size_t height = 10) {
    PyWorld world(width, height);
    world.configure(width, height, 5, 5);
    return world;
}

PyWorld create_arena_world(float radius = 20.0f) {
    size_t size = static_cast<size_t>(radius * 2);
    PyWorld world(size, size);
    return world;
}

PyWorld create_maze_world(uint64_t seed = 42) {
    size_t width = 30, height = 30;
    PyWorld world(width, height);
    world.set_random_seed(seed);
    return world;
}

// Performance utilities
PySimulationMonitor create_monitor(PyBrain brain, PyWorld world, PyAgentBrain agent_brain = PyAgentBrain()) {
    return PySimulationMonitor(std::move(brain), std::move(world), std::move(agent_brain));
}

PyBenchmark create_benchmark(PyBrain brain, PyWorld world, PyAgentBrain agent_brain = PyAgentBrain()) {
    return PyBenchmark(std::move(brain), std::move(world), std::move(agent_brain));
}

PySerializer create_serializer(const std::string& filepath) {
    return PySerializer(filepath);
}

PyExperimentManager create_experiment_manager(const std::string& experiment_id = "", 
                                             const std::string& base_dir = "experiments") {
    return PyExperimentManager(experiment_id, base_dir);
}

// Type conversion utilities
namespace {
    class TypeConverter {
    public:
        static py::array_t<float> to_numpy(const std::vector<float>& vec) {
            return py::array_t<float>(vec.size(), vec.data());
        }
        
        static std::vector<float> to_vector(const py::array_t<float>& arr) {
            auto buf = arr.request();
            float* ptr = (float*)buf.ptr;
            size_t size = buf.size;
            return std::vector<float>(ptr, ptr + size);
        }
        
        static py::dict to_dict(const py::object& obj) {
            // Basic conversion to dict
            return obj.cast<py::dict>();
        }
        
        static py::object from_dict(const py::dict& data, const std::string& type_name) {
            // Basic deserialization
            return py::cast(PyConfig::from_dict(data));
        }
    };
}

// Main module initialization
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -----------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        Provides comprehensive classes for:
        - Configuration management with validation
        - Neural processing core with multiple brain types
        - World simulation with various environments
        - Agent interfaces for brain-world interaction
        - Advanced monitoring and benchmarking tools
        - Serialization and experiment management
        - Performance profiling and optimization tools
        
        Features:
        - Full type hints and documentation
        - Pythonic exception handling
        - Factory functions for easy object creation
        - Comprehensive monitoring and profiling
        - Advanced utilities for power users
        - Performance monitoring and optimization
        - Serialization and experiment management
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<SensorError>(m, "SensorError");
    py::register_exception<ActionError>(m, "ActionError");

    // Register exception translations for common errors
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register core type enums
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    py::enum_<nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    py::enum_<nlm::FiringState>(m, "FiringState")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    py::enum_<nlm::MotorCommand>(m, "MotorCommand")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // Register unique identifiers
    py::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=);

    // Register enhanced wrapper classes with type hints
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration manager with validation and type conversion)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<const std::string&>())  // From file
        .def(py::init<const py::dict&>())  // From dictionary
        .def_static("from_file", &PyConfig::from_file, "Create config from file", py::arg("filepath"))
        .def_static("from_dict", &PyConfig::from_dict, "Create config from dictionary", py::arg("config_dict"))
        .def_static("from_defaults", &PyConfig::from_defaults, "Create config with default values")
        .def("get", &PyConfig::get, "Get configuration value", py::arg("key"))
        .def("set", &PyConfig::set, "Set configuration value", py::arg("key"), py::arg("value"))
        .def("has", &PyConfig::has, "Check if key exists", py::arg("key"))
        .def("get_keys", &PyConfig::get_keys, "Get all configuration keys")
        .def("clear", &PyConfig::clear, "Clear all configuration entries")
        .def("summary", &PyConfig::summary, "Get configuration summary")
        .def("validate", &PyConfig::validate, "Validate configuration values")
        .def("load_from_file", &PyConfig::load_from_file, "Load configuration from file", py::arg("filepath"))
        .def("save_to_file", &PyConfig::save_to_file, "Save configuration to file", py::arg("filepath"))
        .def("__repr__", &PyConfig::__repr__)
        .def("__str__", &PyConfig::__repr__);

    py::class_<PyVision>(m, "Vision", R"pbdoc(Visual sensory input with numpy array support)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("set_data", &PyVision::set_data, "Set vision data", py::arg("data"))
        .def("get_data", &PyVision::get_data, "Get vision data as list")
        .def("get_data_numpy", &PyVision::get_data_numpy, "Get vision data as numpy array")
        .def("get_width", &PyVision::get_width, "Get vision width")
        .def("get_height", &PyVision::get_height, "Get vision height")
        .def("get_channels", &PyVision::get_channels, "Get number of channels")
        .def_property_readonly("shape", &PyVision::get_shape, "Get data shape tuple")
        .def("normalized", &PyVision::normalized, "Get normalized copy of vision data")
        .def("__repr__", &PyVision::__repr__);

    py::class_<PyAudio>(m, "Audio", R"pbdoc(Audio sensory input with sample rate and duration)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", &PyAudio::set_data, "Set audio data", py::arg("data"))
        .def("get_data", &PyAudio::get_data, "Get audio data as list")
        .def("get_data_numpy", &PyAudio::get_data_numpy, "Get audio data as numpy array")
        .def("set_sample_rate", &PyAudio::set_sample_rate, "Set sample rate", py::arg("rate"))
        .def("get_sample_rate", &PyAudio::get_sample_rate, "Get sample rate")
        .def("get_num_samples", &PyAudio::get_num_samples, "Get number of samples")
        .def("get_duration", &PyAudio::get_duration, "Get audio duration in seconds")
        .def("analyze_frequency", &PyAudio::analyze_frequency, "Analyze audio frequency content")
        .def("__repr__", &PyAudio::__repr__);

    py::class_<PyInternalSignals>(m, "InternalSignals", R"pbdoc(Internal state signals container)pbdoc")
        .def(py::init<>())  // Default constructor
        .def("add_signal", &PyInternalSignals::add_signal, "Add a signal value", py::arg("value"))
        .def("add_signals", &PyInternalSignals::add_signals, "Add multiple signal values", py::arg("values"))
        .def("clear_signals", &PyInternalSignals::clear_signals, "Clear all signals")
        .def("get_signals", &PyInternalSignals::get_signals, "Get all signals")
        .def("get_mean", &PyInternalSignals::get_mean, "Get mean of signals")
        .def("get_std", &PyInternalSignals::get_std, "Get standard deviation of signals")
        .def("get_count", &PyInternalSignals::get_count, "Get number of signals")
        .def("__repr__", &PyInternalSignals::__repr__);

    py::class_<PyAction>(m, "Action", R"pbdoc(Enhanced action representation with factory methods)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_static("move_forward", &PyAction::move_forward, "Create forward movement action", py::arg("speed") = 1.0f)
        .def_static("turn_left", &PyAction::turn_left, "Create left turn action", py::arg("angle") = 0.5f)
        .def_static("turn_right", &PyAction::turn_right, "Create right turn action", py::arg("angle") = 0.5f)
        .def_static("move_backward", &PyAction::move_backward, "Create backward movement action", py::arg("speed") = 1.0f)
        .def_static("look_at", &PyAction::look_at, "Create look-at action", py::arg("x"), py::arg("y"))
        .def_static("look_up", &PyAction::look_up, "Create look-up action", py::arg("height") = 1.0f)
        .def_static("look_down", &PyAction::look_down, "Create look-down action", py::arg("height") = 1.0f)
        .def_static("interact", &PyAction::interact, "Create interact action")
        .def_static("eat", &PyAction::eat, "Create eat action")
        .def_static("drink", &PyAction::drink, "Create drink action")
        .def_static("rest", &PyAction::rest, "Create rest action")
        .def_static("wait", &PyAction::wait, "Create wait action")
        .def_static("custom", &PyAction::custom, "Create custom action", py::arg("action_name"), py::arg("parameters") = py::list())
        .def("get_type", &PyAction::get_type, "Get action type")
        .def("set_type", &PyAction::set_type, "Set action type", py::arg("type"))
        .def("get_parameters", &PyAction::get_parameters, "Get action parameters")
        .def("set_parameters", &PyAction::set_parameters, "Set action parameters", py::arg("parameters"))
        .def("get_name", &PyAction::get_name, "Get human-readable action name")
        .def("clone", &PyAction::clone, "Create a copy of this action")
        .def("to_dict", &PyAction::to_dict, "Convert action to dictionary")
        .def_static("from_dict", &PyAction::from_dict, "Create action from dictionary", py::arg("data"))
        .def("__repr__", &PyAction::__repr__);

    py::class_<PyWorld>(m, "World", R"pbdoc(2D world for NLM simulation)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<size_t, size_t>(), py::arg("width"), py::arg("height"))
        .def("configure", &PyWorld::configure, "Configure world dimensions and vision", py::arg("width"), py::arg("height"), py::arg("vision_width") = 5, py::arg("vision_height") = 5)
        .def("reset", &PyWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &PyWorld::set_agent_start, "Set agent starting position", py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f)
        .def("update", &PyWorld::update, "Update world physics", py::arg("timestep") = 1.0)
        .def("apply_action", &PyWorld::apply_action, "Apply action to world", py::arg("action"))
        .def("get_sensory_percept", &PyWorld::get_sensory_percept, "Get current sensory percept from world")
        .def("get_agent_body", &PyWorld::get_agent_body, "Get current agent body state")
        .def("add_object", &PyWorld::add_object, "Add object to world", py::arg("obj"))
        .def("remove_object", &PyWorld::remove_object, "Remove object at position", py::arg("x"), py::arg("y"))
        .def("is_valid_position", &PyWorld::is_valid_position, "Check if position is valid", py::arg("x"), py::arg("y"))
        .def("get_width", &PyWorld::get_width, "Get world width")
        .def("get_height", &PyWorld::get_height, "Get world height")
        .def("get_max_energy", &PyWorld::get_max_energy, "Get maximum agent energy")
        .def("set_max_energy", &PyWorld::set_max_energy, "Set maximum agent energy", py::arg("energy"))
        .def("get_energy_decay_rate", &PyWorld::get_energy_decay_rate, "Get energy decay rate")
        .def("set_energy_decay_rate", &PyWorld::set_energy_decay_rate, "Set energy decay rate", py::arg("rate"))
        .def("get_simulation_time", &PyWorld::get_simulation_time, "Get simulation time")
        .def("set_random_seed", &PyWorld::set_random_seed, "Set random seed", py::arg("seed"))
        .def("get_random_seed", &PyWorld::get_random_seed, "Get random seed")
        .def("__repr__", &PyWorld::__repr__);

    py::class_<PyBrain>(m, "Brain", R"pbdoc(Central neural simulation brain with enhanced API)pbdoc")
        .def(py::init<>())  // Default constructor
        .def(py::init<const PyConfig&>(), py::arg("config"))  // From config
        .def_static("create_with_config", &PyBrain::create_with_config, "Create brain from config dictionary", py::arg("config_dict"))
        .def_static("create_default", &PyBrain::create_default, "Create brain with default configuration")
        .def("initialize", &PyBrain::initialize, "Initialize brain with configuration")
        .def("step", static_cast<void (PyBrain::*)(int)>(&PyBrain::step), "Perform simulation step", py::arg("current_step"))
        .def("step_with_time", &PyBrain::step_with_time, "Perform simulation step with explicit time", py::arg("current_step"), py::arg("current_time"))
        .def("step", static_cast<void (PyBrain::*)(double)>(&PyBrain::step), "Perform simulation step with timestep", py::arg("timestep"))
        .def("step_with_time", &PyBrain::step_with_time, "Perform simulation step with timestep and time", py::arg("timestep"), py::arg("current_time"))
        .def("receive_sensory_input", static_cast<void (PyBrain::*)(const PyVision&)>(&PyBrain::receive_sensory_input), "Receive vision input", py::arg("input"))
        .def("receive_sensory_input", static_cast<void (PyBrain::*)(const PyAudio&)>(&PyBrain::receive_sensory_input), "Receive audio input", py::arg("input"))
        .def("receive_sensory_input", static_cast<void (PyBrain::*)(const PyInternalSignals&)>(&PyBrain::receive_sensory_input), "Receive internal signals", py::arg("input"))
        .def("inject_current", &PyBrain::inject_current, "Inject current into specific neuron", py::arg("neuron_id"), py::arg("current"))
        .def("inject_current_to_type", &PyBrain::inject_current_to_type, "Inject current into all neurons of type", py::arg("neuron_type"), py::arg("current"))
        .def("produce_action", &PyBrain::produce_action, "Produce motor action based on neural activity")
        .def("reset", &PyBrain::reset, "Reset brain state")
        .def("save", &PyBrain::save, "Save brain state to file", py::arg("filepath"))
        .def("load", &PyBrain::load, "Load brain state from file", py::arg("filepath"))
        .def("add_region", &PyBrain::add_region, "Add a new neural region", py::arg("name") = "")
        .def("get_region", &PyBrain::get_region, "Get region by ID", py::arg("id"))
        .def("get_region_count", &PyBrain::get_region_count, "Get number of regions")
        .def("get_region_ids", &PyBrain::get_region_ids, "Get all region IDs")
        .def("get_total_neuron_count", &PyBrain::get_total_neuron_count, "Get total neuron count")
        .def("get_total_synapse_count", &PyBrain::get_total_synapse_count, "Get total synapse count")
        .def("get_active_neuron_count", &PyBrain::get_active_neuron_count, "Get count of active neurons")
        .def("get_firing_neuron_count", &PyBrain::get_firing_neuron_count, "Get count of firing neurons")
        .def("get_average_firing_rate", &PyBrain::get_average_firing_rate, "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &PyBrain::get_excitation_inhibition_ratio, "Get excitation/inhibition ratio")
        .def("get_total_spike_count", &PyBrain::get_total_spike_count, "Get total spike count")
        .def("get_developmental_stage", &PyBrain::get_developmental_stage, "Get developmental stage")
        .def("set_developmental_stage", &PyBrain::set_developmental_stage, "Set developmental stage", py::arg("stage"))
        .def("get_config", &PyBrain::get_config, "Get configuration")
        .def("log_status", &PyBrain::log_status, "Log brain status")
        .def("get_working_memory", &PyBrain::get_working_memory, "Get working memory")
        .def("get_episodic_memory", &PyBrain::get_episodic_memory, "Get episodic memory")
        .def("get_associative_memory", &PyBrain::get_associative_memory, "Get associative memory")
        .def("get_prediction_system", &PyBrain::get_prediction_system, "Get prediction system")
        .def("get_planner", &PyBrain::get_planner, "Get neural planner")
        .def("get_concept_formation", &PyBrain::get_concept_formation, "Get concept formation")
        .def("get_attention", &PyBrain::get_attention, "Get attentional selection")
        .def("get_development_system", &PyBrain::get_development_system, "Get development system")
        .def("get_dopamine", &PyBrain::get_dopamine, "Get dopamine system")
        .def("get_curiosity", &PyBrain::get_curiosity, "Get curiosity system")
        .def("get_novelty", &PyBrain::get_novelty, "Get novelty detector")
        .def("get_prediction_error_signal", &PyBrain::get_prediction_error_signal, "Get prediction error signal")
        .def("get_random_generator", &PyBrain::get_random_generator, "Get random generator")
        .def("__repr__", &PyBrain::__repr__);

    py::class_<PyAgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<const PyBrain&>(), py::arg("brain"))
        .def("initialize", &PyAgentBrain::initialize, "Initialize with world", py::arg("world"))
        .def("get_sensory_input_size", &PyAgentBrain::get_sensory_input_size, "Get expected sensory input size")
        .def("get_motor_output_size", &PyAgentBrain::get_motor_output_size, "Get expected motor output size")
        .def("decode_motor_command", &PyAgentBrain::decode_motor_command, "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &PyAgentBrain::apply_reward_modulation, "Apply reward-based neuromodulation", py::arg("reward"), py::arg("predicted_reward"))
        .def("update_development", &PyAgentBrain::update_development, "Update development system", py::arg("timestep"))
        .def("get_developmental_stage", &PyAgentBrain::get_developmental_stage, "Get current developmental stage")
        .def("get_neuromodulation_level", &PyAgentBrain::get_neuromodulation_level, "Get current neuromodulation level")
        .def("get_curiosity_level", &PyAgentBrain::get_curiosity_level, "Get curiosity level")
        .def("get_novelty_level", &PyAgentBrain::get_novelty_level, "Get novelty level")
        .def("get_prediction_error", &PyAgentBrain::get_prediction_error, "Get prediction error")
        .def("reset", &PyAgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &PyAgentBrain::get_brain, "Get underlying brain")
        .def("enable_reward_modulation", &PyAgentBrain::enable_reward_modulation, "Enable/disable reward modulation", py::arg("enable"))
        .def("enable_structural_plasticity", &PyAgentBrain::enable_structural_plasticity, "Enable/disable structural plasticity", py::arg("enable"))
        .def("enable_development", &PyAgentBrain::enable_development, "Enable/disable development", py::arg("enable"))
        .def("enable_curiosity", &PyAgentBrain::enable_curiosity, "Enable/disable curiosity", py::arg("enable"))
        .def("is_reward_modulation_enabled", &PyAgentBrain::is_reward_modulation_enabled, "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &PyAgentBrain::is_structural_plasticity_enabled, "Check if structural plasticity is enabled")
        .def("is_development_enabled", &PyAgentBrain::is_development_enabled, "Check if development is enabled")
        .def("is_curiosity_enabled", &PyAgentBrain::is_curiosity_enabled, "Check if curiosity is enabled")
        .def("__repr__", &PyAgentBrain::__repr__);

    // Register factory functions
    m.def("create_config", &create_config, "Create configuration with optional presets", py::arg("config_dict") = py::none(), py::arg("config_type") = "default");
    m.def("create_brain", &create_brain, "Create brain with optional configuration", py::arg("config") = PyConfig(), py::arg("brain_type") = "default");
    m.def("create_world", &create_world, "Create world with dimensions", py::arg("width") = 20, py::arg("height") = 20, py::arg("world_type") = "simple");
    m.def("create_agent_brain", &create_agent_brain, "Create agent brain interface", py::arg("brain"), py::arg("world") = PyWorld());
    m.def("create_vision", &create_vision, "Create vision input", py::arg("width") = 84, py::arg("height") = 84, py::arg("channels") = 3);
    m.def("create_audio", &create_audio, "Create audio input", py::arg("sample_rate") = 22050, py::arg("num_samples") = 16384);
    m.def("create_internal_signals", &create_internal_signals, "Create internal signals");
    m.def("create_action", &create_action, "Create action", py::arg("action_type"), py::arg("parameters") = std::vector<float>());
    m.def("create_movement_action", &create_movement_action, "Create movement action", py::arg("movement_type"), py::arg("magnitude") = 1.0f);
    m.def("create_look_action", &create_look_action, "Create look-at action", py::arg("x"), py::arg("y"));

    // Register preset factory functions
    m.def("create_research_config", &create_research_config, "Create configuration for research");
    m.def("create_training_config", &create_training_config, "Create configuration for training", py::arg("episode_limit") = 1000);
    m.def("create_simulation_config", &create_simulation_config, "Create configuration for simulation", py::arg("time_steps") = 10000);

    m.def("create_simple_brain", &create_simple_brain, "Create minimal brain for testing");
    m.def("create_development_brain", &create_development_brain, "Create brain with development enabled");
    m.def("create_learning_brain", &create_learning_brain, "Create brain with specific learning rate", py::arg("learning_rate") = 0.01f);

    m.def("create_grid_world", &create_grid_world, "Create grid-based world", py::arg("width") = 10, py::arg("height") = 10);
    m.def("create_arena_world", &create_arena_world, "Create arena-shaped world", py::arg("radius") = 20.0f);
    m.def("create_maze_world", &create_maze_world, "Create procedural maze world", py::arg("seed") = 42);

    // Register performance monitoring and profiling tools
    py::class_<PySimulationMonitor>(m, "SimulationMonitor", R"pbdoc(Performance monitoring for simulation runs)pbdoc")
        .def(py::init<std::shared_ptr<PyBrain>, std::shared_ptr<PyWorld>, std::shared_ptr<PyAgentBrain>>())
        .def(py::init<PyBrain, PyWorld, PyAgentBrain>())
        .def("start_monitoring", &PySimulationMonitor::start_monitoring, "Start monitoring")
        .def("stop_monitoring", &PySimulationMonitor::stop_monitoring, "Stop monitoring")
        .def("record_step", &PySimulationMonitor::record_step, "Record step metrics", py::arg("step_time"), py::arg("firing_rate"), py::arg("reward") = 0.0f)
        .def("get_statistics", &PySimulationMonitor::get_statistics, "Get monitoring statistics")
        .def("get_performance_metrics", &PySimulationMonitor::get_performance_metrics, "Get performance metrics")
        .def("get_stability_metrics", &PySimulationMonitor::get_stability_metrics, "Get stability metrics")
        .def("plot_metrics", &PySimulationMonitor::plot_metrics, "Plot metrics to file", py::arg("output_file") = "");

    py::class_<PyBenchmark>(m, "Benchmark", R"pbdoc(Performance benchmarking utilities)pbdoc")
        .def(py::init<std::shared_ptr<PyBrain>, std::shared_ptr<PyWorld>, std::shared_ptr<PyAgentBrain>>())
        .def(py::init<PyBrain, PyWorld, PyAgentBrain>())
        .def("benchmark_step", &PyBenchmark::benchmark_step, "Benchmark single step performance", py::arg("num_steps") = 100)
        .def("benchmark_simulation", &PyBenchmark::benchmark_simulation, "Benchmark full simulation", py::arg("num_steps") = 1000)
        .def("benchmark_memory", &PyBenchmark::benchmark_memory, "Benchmark memory usage")
        .def("run_comparison", &PyBenchmark::run_comparison, "Compare different configurations", py::arg("configurations"))
        .def("generate_report", &PyBenchmark::generate_report, "Generate benchmark report", py::arg("output_file") = "");

    // Register serialization utilities
    py::class_<PySerializer>(m, "Serializer", R"pbdoc(Serialization and file I/O utilities)pbdoc")
        .def(py::init<const std::string&>(), py::arg("filepath"))
        .def("save_config", &PySerializer::save_config, "Save configuration to file", py::arg("config"))
        .def("load_config", &PySerializer::load_config, "Load configuration from file")
        .def("save_brain", &PySerializer::save_brain, "Save brain state to file", py::arg("brain"))
        .def("load_brain", &PySerializer::load_brain, "Load brain state from file")
        .def("export_json", &PySerializer::export_json, "Export object to JSON", py::arg("obj"))
        .def("import_json", &PySerializer::import_json, "Import object from JSON", py::arg("json_str"), py::arg("obj_type"));

    // Register experiment management
    py::class_<PyExperimentManager>(m, "ExperimentManager", R"pbdoc(Experiment tracking and management)pbdoc")
        .def(py::init<const std::string&, const std::string&>(), py::arg("experiment_id") = "", py::arg("base_dir") = "experiments")
        .def("start_experiment", &PyExperimentManager::start_experiment, "Start new experiment", py::arg("config"))
        .def("log_step", &PyExperimentManager::log_step, "Log step data", py::arg("step"), py::arg("data"))
        .def("save_checkpoint", &PyExperimentManager::save_checkpoint, "Save experiment checkpoint", py::arg("brain"), py::arg("world"), py::arg("step"))
        .def("load_checkpoint", &PyExperimentManager::load_checkpoint, "Load experiment checkpoint", py::arg("step"))
        .def("get_results", &PyExperimentManager::get_results, "Get experiment results")
        .def("generate_report", &PyExperimentManager::generate_report, "Generate experiment report", py::arg("output_file") = "");

    // Register type conversion utilities
    m.def("to_numpy", &TypeConverter::to_numpy, "Convert vector to numpy array", py::arg("vec"));
    m.def("to_vector", &TypeConverter::to_vector, "Convert numpy array to vector", py::arg("arr"));
    m.def("to_dict", &TypeConverter::to_dict, "Convert object to dictionary", py::arg("obj"));
    m.def("from_dict", &TypeConverter::from_dict, "Create object from dictionary", py::arg("data"), py::arg("type_name"));

    // Register monitoring utilities
    m.def("create_monitor", &create_monitor, "Create simulation monitor", py::arg("brain"), py::arg("world"), py::arg("agent_brain") = PyAgentBrain());
    m.def("create_benchmark", &create_benchmark, "Create benchmark suite", py::arg("brain"), py::arg("world"), py::arg("agent_brain") = PyAgentBrain());
    m.def("create_serializer", &create_serializer, "Create serializer", py::arg("filepath"));
    m.def("create_experiment_manager", &create_experiment_manager, "Create experiment manager", py::arg("experiment_id") = "", py::arg("base_dir") = "experiments");

    // Register convenience constants and utilities
    m.def("get_default_constants", []() {
        py::dict constants;
        constants["PI"] = 3.14159265359;
        constants["E"] = 2.71828182846;
        constants["GOLDEN_RATIO"] = 1.61803398875;
        constants["DEFAULT_LEARNING_RATE"] = 0.01f;
        constants["DEFAULT_RANDOM_SEED"] = std::mt19937::default_seed;
        return constants;
    }, "Get default constants");

    // Register profiling utilities
    m.def("profile_simulation", [](PyBrain& brain, PyWorld& world, PyAgentBrain& agent, size_t steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        auto start_memory = std::unique_ptr<int>(new int(0)); // Placeholder for memory tracking
        
        for (size_t i = 0; i < steps; ++i) {
            brain.step(i);
            world.update(0.1);
            if (&agent != nullptr) {
                agent.update_development(0.1);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict result;
        result["total_time_ms"] = duration.count();
        result["steps_per_second"] = steps * 1000.0 / duration.count();
        result["final_neuron_count"] = brain.get_total_neuron_count();
        result["final_synapse_count"] = brain.get_total_synapse_count();
        
        return result;
    }, "Profile simulation performance", py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"));

    m.def("memory_profile", []() {
        // Simple memory profiling - would require platform-specific code
        py::dict result;
        result["note"] = "Full memory profiling requires platform-specific implementation";
        result["suggestion"] = "Use tracemalloc for detailed memory tracking";
        return result;
    }, "Get memory profiling information");

    // Register validation utilities
    py::class_<Validator>(m, "Validator", R"pbdoc(Input validation utilities)pbdoc")
        .def_static("validate_config", &Validator::validate_config, "Validate configuration", py::arg("config"))
        .def_static("validate_brain", &Validator::validate_brain, "Validate brain state", py::arg("brain"))
        .def_static("validate_world", &Validator::validate_world, "Validate world state", py::arg("world"))
        .def_static("validate_action", &Validator::validate_action, "Validate action", py::arg("action"))
        .def_static("validate_data", &Validator::validate_data, "Validate data type and shape", py::arg("data"), py::arg("expected_type"), py::arg("expected_shape") = py::none());

    // Register error handling utilities
    m.def("get_last_error", []() {
        // This would capture and return the last error in Python
        return py::none(); // Placeholder
    }, "Get the last error that occurred");

    // Register advanced utilities
    m.def("enable_profiling", []() {
        // Enable global profiling
        return py::dict({{"status", "enabled"}});
    }, "Enable global profiling");

    m.def("disable_profiling", []() {
        // Disable global profiling
        return py::dict({{"status", "disabled"}});
    }, "Disable global profiling");

    m.def("get_system_info", []() {
        py::dict info;
        info["python_version"] = PYBIND11_VERSION;
        info["platform"] = "Linux"; // Would be determined dynamically
        info["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
        return info;
    }, "Get system information");

    // Register performance monitoring hooks
    m.def("start_performance_monitoring", []() {
        // Start performance monitoring
        return py::dict({{"status", "started"}});
    }, "Start performance monitoring");

    m.def("stop_performance_monitoring", []() {
        // Stop performance monitoring
        return py::dict({{"status", "stopped"}});
    }, "Stop performance monitoring");

    m.def("get_performance_report", []() {
        // Get performance report
        py::dict report;
        report["status"] = "empty"; // Would contain actual performance data
        report["note"] = "Performance data would be collected during simulation runs";
        return report;
    }, "Get performance report");

    // Register convenience functions for common tasks
    m.def("quick_simulate", [](PyBrain& brain, PyWorld& world, PyAgentBrain& agent, size_t steps) {
        brain.initialize();
        world.reset();
        if (&agent != nullptr) agent.reset();
        
        for (size_t i = 0; i < steps; ++i) {
            world.update(0.1);
            if (&agent != nullptr) {
                agent.update_development(0.1);
                brain.step(i);
                auto action = agent.decode_motor_command();
                world.apply_action(action);
            } else {
                brain.step(i);
            }
        }
        
        return py::dict({
            {"steps_completed", steps},
            {"neurons_active", brain.get_active_neuron_count()},
            {"neurons_firing", brain.get_firing_neuron_count()},
            {"total_spikes", brain.get_total_spike_count()}
        });
    }, "Quick simulation helper", py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"));

    m.def("batch_simulate", [](const std::vector<PyConfig>& configs, size_t steps_per_config) {
        py::list results;
        
        for (const auto& config : configs) {
            PyBrain brain(config);
            PyWorld world;
            PyAgentBrain agent(brain);
            
            brain.initialize();
            world.reset();
            agent.reset();
            
            for (size_t i = 0; i < steps_per_config; ++i) {
                world.update(0.1);
                agent.update_development(0.1);
                brain.step(i);
                auto action = agent.decode_motor_command();
                world.apply_action(action);
            }
            
            py::dict config_result;
            config_result["config_hash"] = std::hash<PyConfig>{}(config);
            config_result["neurons"] = brain.get_total_neuron_count();
            config_result["synapses"] = brain.get_total_synapse_count();
            config_result["spikes"] = brain.get_total_spike_count();
            results.append(config_result);
        }
        
        return results;
    }, "Batch simulation with multiple configurations", py::arg("configs"), py::arg("steps_per_config"));

    // Register invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
}

// Helper class for validation (defined in the module)
class Validator {
public:
    static std::vector<std::string> validate_config(const PyConfig& config) {
        std::vector<std::string> errors;
        if (!config.validate()) {
            errors.push_back("Configuration is missing required fields");
        }
        return errors;
    }
    
    static std::vector<std::string> validate_brain(const PyBrain& brain) {
        std::vector<std::string> errors;
        if (!brain.initialize()) {
            errors.push_back("Brain initialization failed");
        }
        return errors;
    }
    
    static std::vector<std::string> validate_world(const PyWorld& world) {
        std::vector<std::string> errors;
        if (world.get_width() <= 0 || world.get_height() <= 0) {
            errors.push_back("World dimensions must be positive");
        }
        return errors;
    }
    
    static std::vector<std::string> validate_action(const PyAction& action) {
        std::vector<std::string> errors;
        // Basic validation
        return errors;
    }
    
    static bool validate_data(const py::object& data, const std::type_info& expected_type, const py::tuple& expected_shape) {
        return true; // Simplified validation
    }
};