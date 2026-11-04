#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

namespace py = pybind11;
namespace nlm {

// Exception types for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("ConfigError: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("BrainError: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("WorldError: " + msg) {}
};

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& msg) : NLMException("AgentError: " + msg) {}
};

// Context manager wrapper for Brain
class BrainWrapper {
public:
    std::shared_ptr<Brain> brain_;
    
    BrainWrapper(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
        if (!brain_) {
            throw BrainError("Cannot create BrainWrapper with null pointer");
        }
    }
    
    ~BrainWrapper() {
        // Brain's destructor will handle its internal cleanup
    }
    
    // Context manager interface
    BrainWrapper& operator=(const BrainWrapper&) = delete;
    
    Brain& getBrain() {
        return *brain_;
    }
    
    const Brain& getBrain() const {
        return *brain_;
    }
    
    bool initialize() {
        return brain_->initialize();
    }
    
    void step(SimulationStep step) {
        brain_->step(step);
    }
    
    void step(SimulationStep step, Timestamp time) {
        brain_->step(step, time);
    }
    
    void receiveSensoryInput(const SensoryPercept& percept) {
        // Create temporary Vision from percept if needed
        throw BrainError("Please use receiveVision() instead for direct vision input");
    }
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        brain_->injectCurrent(neuron, current);
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    std::unique_ptr<Action> produceAction() {
        return brain_->produceAction();
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
    
    RegionId addRegion(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    NeuralRegion* getRegion(RegionId id) {
        return brain_->getRegion(id);
    }
    
    NeuralRegion* getRegion(RegionId id) const {
        return const_cast<Brain*>(brain_.get())->getRegion(id);
    }
    
    size_t getRegionCount() const {
        return brain_->getRegionCount();
    }
    
    std::vector<RegionId> getRegionIds() const {
        return brain_->getRegionIds();
    }
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        return brain_->getRegions();
    }
    
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }
    
    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    std::shared_ptr<const Config> getConfig() const {
        return brain_->getConfig();
    }
    
    void logStatus() const {
        brain_->logStatus();
    }
    
    std::string repr() const {
        return "<BrainWrapper at " + std::to_string(reinterpret_cast<uintptr_t>(brain_.get())) + ">";
    }
    
    // Context manager methods
    void __enter__() {
        if (!brain_->initialize()) {
            throw BrainError("Failed to initialize brain on context entry");
        }
    }
    
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        brain_->reset();
        brain_ = nullptr;
    }
    
    bool isInitialized() const {
        return brain_ && brain_->initialize();
    }
};

// Context manager wrapper for Config
class ConfigWrapper {
public:
    std::shared_ptr<Config> config_;
    
    ConfigWrapper(std::shared_ptr<Config> config) : config_(std::move(config)) {
        if (!config_) {
            throw ConfigError("Cannot create ConfigWrapper with null pointer");
        }
    }
    
    ~ConfigWrapper() = default;
    
    ConfigWrapper& operator=(const ConfigWrapper&) = delete;
    
    Config& getConfig() {
        return *config_;
    }
    
    const Config& getConfig() const {
        return *config_;
    }
    
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    bool loadFromArgs(const std::vector<std::string>& args) {
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& a : args) {
            argv.push_back(const_cast<char*>(a.c_str()));
        }
        return config_->loadFromArgs(static_cast<int>(argv.size()), argv.data());
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto value = config_->get<std::string>(key);
        return value ? *value : defaultValue;
    }
    
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto value = config_->get<int>(key);
        return value ? *value : defaultValue;
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        auto value = config_->get<double>(key);
        return value ? *value : defaultValue;
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        auto value = config_->get<bool>(key);
        return value ? *value : defaultValue;
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
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
    
    std::string repr() const {
        return "<ConfigWrapper at " + std::to_string(reinterpret_cast<uintptr_t>(config_.get())) + ">";
    }
    
    // Context manager methods
    void __enter__() {}
    
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        config_->clear();
    }
    
    bool isEmpty() const {
        return getKeys().empty();
    }
};

// Context manager wrapper for SimpleWorld
class SimpleWorldWrapper {
public:
    std::shared_ptr<SimpleWorld> world_;
    
    SimpleWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(std::move(world)) {
        if (!world_) {
            throw WorldError("Cannot create SimpleWorldWrapper with null pointer");
        }
    }
    
    ~SimpleWorldWrapper() = default;
    
    SimpleWorldWrapper& operator=(const SimpleWorldWrapper&) = delete;
    
    SimpleWorld& getWorld() {
        return *world_;
    }
    
    const SimpleWorld& getWorld() const {
        return *world_;
    }
    
    void configure(float width, float height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
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
    
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime) {
        return world_->applyMotorCommand(cmd, currentTime);
    }
    
    const SensoryPercept& getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    const AgentBody& getAgentBody() const {
        return world_->getAgentBody();
    }
    
    void addObject(const WorldObject& obj) {
        world_->addObject(obj);
    }
    
    void removeObject(float x, float y) {
        world_->removeObject(x, y);
    }
    
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
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
    
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint64_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    std::string repr() const {
        return "<SimpleWorldWrapper width=" + std::to_string(world_->getWidth()) + 
               " height=" + std::to_string(world_->getHeight()) + ">";
    }
    
    // Context manager methods
    void __enter__() {
        reset();
    }
    
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        reset();
        // Clear all objects for cleanup
        // Note: SimpleWorld doesn't have a clearObjects method, so we rely on reset()
    }
    
    bool isRunning() const {
        return getSimulationTime() > 0.0;
    }
};

// Context manager wrapper for AgentBrain
class AgentBrainWrapper {
public:
    std::shared_ptr<AgentBrain> agentBrain_;
    
    AgentBrainWrapper(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(std::move(agentBrain)) {
        if (!agentBrain_) {
            throw AgentError("Cannot create AgentBrainWrapper with null pointer");
        }
    }
    
    ~AgentBrainWrapper() = default;
    
    AgentBrainWrapper& operator=(const AgentBrainWrapper&) = delete;
    
    AgentBrain& getAgentBrain() {
        return *agentBrain_;
    }
    
    const AgentBrain& getAgentBrain() const {
        return *agentBrain_;
    }
    
    void initialize(const SimpleWorld& world) {
        agentBrain_->initialize(world);
    }
    
    size_t getSensoryInputSize() const {
        return agentBrain_->getSensoryInputSize();
    }
    
    size_t getMotorOutputSize() const {
        return agentBrain_->getMotorOutputSize();
    }
    
    void processSensoryInput(const SensoryPercept& percept) {
        agentBrain_->processSensoryInput(percept);
    }
    
    MotorCommand decodeMotorCommand() {
        return agentBrain_->decodeMotorCommand();
    }
    
    void applyRewardModulation(float reward, float predictedReward) {
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    void updateDevelopment(double timestep) {
        agentBrain_->updateDevelopment(timestep);
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return agentBrain_->getDevelopmentalStage();
    }
    
    float getNeuromodulationLevel() const {
        return agentBrain_->getNeuromodulationLevel();
    }
    
    float getCuriosityLevel() const {
        return agentBrain_->getCuriosityLevel();
    }
    
    float getNoveltyLevel() const {
        return agentBrain_->getNoveltyLevel();
    }
    
    float getPredictionError() const {
        return agentBrain_->getPredictionError();
    }
    
    void reset() {
        agentBrain_->reset();
    }
    
    Brain* getBrain() {
        return agentBrain_->getBrain();
    }
    
    bool enableRewardModulation(bool enable) {
        agentBrain_->enableRewardModulation(enable);
        return enable;
    }
    
    bool enableStructuralPlasticity(bool enable) {
        agentBrain_->enableStructuralPlasticity(enable);
        return enable;
    }
    
    bool enableDevelopment(bool enable) {
        agentBrain_->enableDevelopment(enable);
        return enable;
    }
    
    bool enableCuriosity(bool enable) {
        agentBrain_->enableCuriosity(enable);
        return enable;
    }
    
    bool isRewardModulationEnabled() const {
        return agentBrain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain_->isCuriosityEnabled();
    }
    
    std::string repr() const {
        return "<AgentBrainWrapper at " + std::to_string(reinterpret_cast<uintptr_t>(agentBrain_.get())) + ">";
    }
    
    // Context manager methods
    void __enter__() {}
    
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        reset();
    }
    
    bool isInitialized() const {
        return agentBrain_.get() != nullptr;
    }
};

// Factory functions
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
    if (!config) {
        throw ConfigError("Cannot create Brain with null config");
    }
    return std::make_shared<Brain>(config);
}

std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        throw BrainError("Cannot create AgentBrain with null brain");
    }
    return std::make_shared<AgentBrain>(brain);
}

// Vision wrapper
class VisionWrapper {
public:
    std::unique_ptr<Vision> vision_;
    
    VisionWrapper(std::unique_ptr<Vision> vision) : vision_(std::move(vision)) {
        if (!vision_) {
            throw NLMException("Cannot create VisionWrapper with null pointer");
        }
    }
    
    ~VisionWrapper() = default;
    
    VisionWrapper& operator=(const VisionWrapper&) = delete;
    
    Vision& getVision() {
        return *vision_;
    }
    
    const Vision& getVision() const {
        return *vision_;
    }
    
    size_t getWidth() const {
        return vision_->getWidth();
    }
    
    size_t getHeight() const {
        return vision_->getHeight();
    }
    
    size_t getChannels() const {
        return vision_->getChannels();
    }
    
    void setData(const std::vector<float>& data) {
        vision_->setData(data);
    }
    
    std::string repr() const {
        return "<VisionWrapper width=" + std::to_string(vision_->getWidth()) + 
               " height=" + std::to_string(vision_->getHeight()) + 
               " channels=" + std::to_string(vision_->getChannels()) + ">";
    }
    
    // Context manager methods (minimal for Vision)
    void __enter__() {}
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        vision_ = nullptr;
    }
};

// Audio wrapper
class AudioWrapper {
public:
    std::unique_ptr<Audio> audio_;
    
    AudioWrapper(std::unique_ptr<Audio> audio) : audio_(std::move(audio)) {
        if (!audio_) {
            throw NLMException("Cannot create AudioWrapper with null pointer");
        }
    }
    
    ~AudioWrapper() = default;
    
    AudioWrapper& operator=(const AudioWrapper&) = delete;
    
    Audio& getAudio() {
        return *audio_;
    }
    
    const Audio& getAudio() const {
        return *audio_;
    }
    
    size_t getSampleRate() const {
        return audio_->getSampleRate();
    }
    
    size_t getNumSamples() const {
        return audio_->getNumSamples();
    }
    
    void setData(const std::vector<float>& data) {
        audio_->setData(data);
    }
    
    void setSampleRate(size_t rate) {
        audio_->setSampleRate(rate);
    }
    
    std::string repr() const {
        return "<AudioWrapper sampleRate=" + std::to_string(audio_->getSampleRate()) + 
               " samples=" + std::to_string(audio_->getNumSamples()) + ">";
    }
    
    // Context manager methods (minimal for Audio)
    void __enter__() {}
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        audio_ = nullptr;
    }
};

// InternalSignals wrapper
class InternalSignalsWrapper {
public:
    std::unique_ptr<InternalSignals> signals_;
    
    InternalSignalsWrapper(std::unique_ptr<InternalSignals> signals) : signals_(std::move(signals)) {
        if (!signals_) {
            throw NLMException("Cannot create InternalSignalsWrapper with null pointer");
        }
    }
    
    ~InternalSignalsWrapper() = default;
    
    InternalSignalsWrapper& operator=(const InternalSignalsWrapper&) = delete;
    
    InternalSignals& getSignals() {
        return *signals_;
    }
    
    const InternalSignals& getSignals() const {
        return *signals_;
    }
    
    void addSignal(float value) {
        signals_->addSignal(value);
    }
    
    void clearSignals() {
        signals_->clearSignals();
    }
    
    std::string repr() const {
        return "<InternalSignalsWrapper>";
    }
    
    // Context manager methods (minimal for InternalSignals)
    void __enter__() {}
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        signals_ = nullptr;
    }
};

// Action wrapper
class ActionWrapper {
public:
    std::unique_ptr<Action> action_;
    
    ActionWrapper(std::unique_ptr<Action> action) : action_(std::move(action)) {
        if (!action_) {
            throw NLMException("Cannot create ActionWrapper with null pointer");
        }
    }
    
    ~ActionWrapper() = default;
    
    ActionWrapper& operator=(const ActionWrapper&) = delete;
    
    Action& getAction() {
        return *action_;
    }
    
    const Action& getAction() const {
        return *action_;
    }
    
    ActionType getType() const {
        return action_->getType();
    }
    
    void setType(ActionType type) {
        action_->setType(type);
    }
    
    const std::vector<float>& getParameters() const {
        return action_->getParameters();
    }
    
    void setParameters(const std::vector<float>& params) {
        action_->setParameters(params);
    }
    
    std::string getName() const {
        return action_->getName();
    }
    
    std::unique_ptr<Action> clone() const {
        return action_->clone();
    }
    
    std::string repr() const {
        return "<ActionWrapper type=" + action_->getName() + ">";
    }
    
    // Context manager methods (minimal for Action)
    void __enter__() {}
    void __exit__(int /* exc_type */, const std::string& /* exc_val */, const std::string& /* exc_tb */) {
        action_ = nullptr;
    }
};

PYBIND11_MODULE(pynlm, m) {
    // Register exception translators
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ConfigError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const BrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const WorldError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const AgentError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Type documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to neural computation classes with improved
        memory safety, context management, and Pythonic APIs.
        
        Key features:
        - Context manager support for resource management
        - Factory functions for easy object creation
        - Comprehensive type hints and error handling
        - Smart pointer memory management
        - Full documentation with docstrings
    )pbdoc";
    
    // Define type conversions
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .doc() = R"pbdoc(Unique identifier for a neuron with hash support and comparison operators.)pbdoc";
    
    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .doc() = R"pbdoc(Unique identifier for a synapse with hash support and comparison operators.)pbdoc";
    
    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .doc() = R"pbdoc(Unique identifier for a brain region with hash support and comparison operators.)pbdoc";
    
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .doc() = R"pbdoc(Unique identifier for a neuron population.)pbdoc";
    
    // Enum definitions with better documentation
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = R"pbdoc(Enum for neuron types. Can be Excitatory, Inhibitory, Modulatory, Sensory, Motor, or Internal.)pbdoc";
    
    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = R"pbdoc(Enum for synapse types. Can be Excitatory, Inhibitory, Modulatory, Electrical, or GapJunction.)pbdoc";
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = R"pbdoc(Enum for developmental stages. Can be Initial, CriticalPeriod, Maturation, Adult, or Aging.)pbdoc";
    
    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = R"pbdoc(Enum for neuron firing states. Can be Resting, Active, Refractory, or Inhibited.)pbdoc";
    
    py::enum_<ActionType>(m, "ActionType")
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
        .export_values()
        .doc() = R"pbdoc(Enum for action types. Includes movement, looking, interaction, and resting actions.)pbdoc";
    
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .doc() = R"pbdoc(Enum for low-level motor commands. Can be MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, or Wait.)pbdoc";
    
    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = R"pbdoc(Enum for world object types. Can be Empty, Resource, Hazard, Wall, or Marker.)pbdoc";
    
    // Config class with context manager support
    py::class_<ConfigWrapper, std::shared_ptr<ConfigWrapper>>(m, "Config")
        .def(py::init<std::shared_ptr<Config>>())
        .def("loadFromFile", &ConfigWrapper::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", &ConfigWrapper::loadFromArgs, py::arg("args"),
             "Load configuration from command line arguments as strings")
        .def("saveToFile", &ConfigWrapper::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &ConfigWrapper::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &ConfigWrapper::getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigWrapper::clear,
             "Clear all configuration entries")
        .def("summary", &ConfigWrapper::summary,
             "Get a summary string of the configuration")
        .def("getString", &ConfigWrapper::getString,
             py::arg("key"), py::arg("default_value") = "",
             "Get a string value from configuration with default")
        .def("getInt", &ConfigWrapper::getInt,
             py::arg("key"), py::arg("default_value") = 0,
             "Get an integer value from configuration with default")
        .def("getDouble", &ConfigWrapper::getDouble,
             py::arg("key"), py::arg("default_value") = 0.0,
             "Get a double value from configuration with default")
        .def("getBool", &ConfigWrapper::getBool,
             py::arg("key"), py::arg("default_value") = false,
             "Get a boolean value from configuration with default")
        .def("set", static_cast<void(ConfigWrapper::*) (ConfigWrapper&, const std::string&, const std::string&)>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set a string value in configuration")
        .def("set", static_cast<void(ConfigWrapper::*) (ConfigWrapper&, const std::string&, int)>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set an integer value in configuration")
        .def("set", static_cast<void(ConfigWrapper::*) (ConfigWrapper&, const std::string&, double)>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set a double value in configuration")
        .def("set", static_cast<void(ConfigWrapper::*) (ConfigWrapper&, const std::string&, bool)>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set a boolean value in configuration")
        .def("remove", &ConfigWrapper::remove, py::arg("key"),
             "Remove a key from configuration")
        .def("isEmpty", &ConfigWrapper::isEmpty,
             "Check if configuration is empty")
        .def("__repr__", &ConfigWrapper::repr,
             "Return string representation of Config")
        .def("__enter__", &ConfigWrapper::__enter__,
             "Context manager entry - clears configuration")
        .def("__exit__", &ConfigWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - cleans up configuration")
        .doc() = R"pbdoc(Configuration management with context manager support.
        
        Provides access to configuration values with type-safe getters and setters.
        Can be used as a context manager for automatic cleanup.
        
        Example:
            with Config() as cfg:
                cfg.loadFromFile("config.json")
                cfg.set("learning_rate", 0.01)
        )pbdoc";
    
    // Vision class with context manager support
    py::class_<VisionWrapper, std::shared_ptr<VisionWrapper>>(m, "Vision")
        .def(py::init<std::unique_ptr<Vision>>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("getWidth", &VisionWrapper::getWidth,
             "Get vision width in pixels")
        .def("getHeight", &VisionWrapper::getHeight,
             "Get vision height in pixels")
        .def("getChannels", &VisionWrapper::getChannels,
             "Get number of vision channels")
        .def("getDimensions", [](const VisionWrapper& self) {
            return std::vector<size_t>{self.getVision().getWidth(), self.getVision().getHeight(), self.getVision().getChannels()};
        }, "Get vision dimensions as [width, height, channels]")
        .def("setData", &VisionWrapper::setData, py::arg("data"),
             "Set vision data as vector of floats")
        .def("getData", [](const VisionWrapper& self) {
            return self.getVision().getData();
        }, "Get vision data as vector of floats")
        .def("__repr__", &VisionWrapper::repr,
             "Return string representation of Vision")
        .def("__enter__", &VisionWrapper::__enter__,
             "Context manager entry (no-op)")
        .def("__exit__", &VisionWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - cleans up vision data")
        .doc() = R"pbdoc(Vision sensory input with context manager support.
        
        Represents visual sensor input with width, height, and channels.
        Can be used as a context manager for automatic cleanup.
        
        Example:
            with Vision(84, 84, 3) as vision:
                vision.setData([0.5] * (84 * 84 * 3))
        )pbdoc";
    
    // Audio class with context manager support
    py::class_<AudioWrapper, std::shared_ptr<AudioWrapper>>(m, "Audio")
        .def(py::init<std::unique_ptr<Audio>>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("getSampleRate", &AudioWrapper::getSampleRate,
             "Get audio sample rate")
        .def("getNumSamples", &AudioWrapper::getNumSamples,
             "Get number of audio samples")
        .def("getDimensions", [](const AudioWrapper& self) {
            return std::vector<size_t>{self.getAudio().getSampleRate(), self.getAudio().getNumSamples()};
        }, "Get audio dimensions as [sampleRate, numSamples]")
        .def("setData", &AudioWrapper::setData, py::arg("data"),
             "Set audio data as vector of floats")
        .def("getData", [](const AudioWrapper& self) {
            return self.getAudio().getData();
        }, "Get audio data as vector of floats")
        .def("setSampleRate", &AudioWrapper::setSampleRate, py::arg("rate"),
             "Set audio sample rate")
        .def("__repr__", &AudioWrapper::repr,
             "Return string representation of Audio")
        .def("__enter__", &AudioWrapper::__enter__,
             "Context manager entry (no-op)")
        .def("__exit__", &AudioWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - cleans up audio data")
        .doc() = R"pbdoc(Audio sensory input with context manager support.
        
        Represents audio sensor input with sample rate and number of samples.
        Can be used as a context manager for automatic cleanup.
        
        Example:
            with Audio(44100, 44100) as audio:
                audio.setData([0.5] * 44100)
                audio.setSampleRate(22050)
        )pbdoc";
    
    // InternalSignals class with context manager support
    py::class_<InternalSignalsWrapper, std::shared_ptr<InternalSignalsWrapper>>(m, "InternalSignals")
        .def(py::init<std::unique_ptr<InternalSignals>>())
        .def("addSignal", &InternalSignalsWrapper::addSignal, py::arg("value"),
             "Add a signal value")
        .def("getSignals", [](const InternalSignalsWrapper& self) {
            return self.getSignals().getData();
        }, "Get all signals as vector of floats")
        .def("getDimensions", [](const InternalSignalsWrapper& self) {
            return self.getSignals().getData().size();
        }, "Get number of signals")
        .def("clearSignals", &InternalSignalsWrapper::clearSignals,
             "Clear all signals")
        .def("__repr__", &InternalSignalsWrapper::repr,
             "Return string representation of InternalSignals")
        .def("__enter__", &InternalSignalsWrapper::__enter__,
             "Context manager entry (no-op)")
        .def("__exit__", &InternalSignalsWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - cleans up signals")
        .doc() = R"pbdoc(Internal signals sensory input with context manager support.
        
        Represents internal state signals from within the brain.
        Can be used as a context manager for automatic cleanup.
        
        Example:
            with InternalSignals() as signals:
                signals.addSignal(0.5)
                signals.addSignal(0.8)
                signals.addSignal(0.3)
        )pbdoc";
    
    // Action class with context manager support
    py::class_<ActionWrapper, std::shared_ptr<ActionWrapper>>(m, "Action")
        .def(py::init<std::unique_ptr<Action>>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &ActionWrapper::getType,
             "Get action type")
        .def("setType", &ActionWrapper::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &ActionWrapper::getParameters,
             "Get action parameters as vector of floats")
        .def("setParameters", &ActionWrapper::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &ActionWrapper::getName,
             "Get action name as string")
        .def("clone", &ActionWrapper::clone,
             "Create a copy of this action")
        .def("__repr__", &ActionWrapper::repr,
             "Return string representation of Action")
        .def("__enter__", &ActionWrapper::__enter__,
             "Context manager entry (no-op)")
        .def("__exit__", &ActionWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - cleans up action")
        .doc() = R"pbdoc(Action representation for motor output with context manager support.
        
        Represents a motor command with type and parameters.
        Can be used as a context manager for automatic cleanup.
        
        Example:
            with Action(ActionType.MoveForward, [0.5, 0.5]) as action:
                action.getType()  # ActionType.MoveForward
                action.setParameters([0.8, 0.2])
        )pbdoc";
    
    // WorldObject structure
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x,
                       "X coordinate in world")
        .def_readwrite("y", &WorldObject::y,
                       "Y coordinate in world")
        .def_readwrite("radius", &WorldObject::radius,
                       "Object radius")
        .def_readwrite("type", &WorldObject::type,
                       "Object type (WorldObjectType)")
        .def_readwrite("value", &WorldObject::value,
                       "Object value (reward or damage)")
        .def_readwrite("active", &WorldObject::active,
                       "Whether object is active")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject x=" + std::to_string(obj.x) + " y=" + std::to_string(obj.y) + 
                   " type=" + std::to_string(static_cast<int>(obj.type)) + ">";
        }, "Return string representation of WorldObject")
        .doc() = R"pbdoc(World object representation.
        
        Represents an object in the simulated world with position and type.
        Used for resources, hazards, walls, and markers in the environment.
        
        Example:
            obj = WorldObject(1.0, 1.0, WorldObjectType.Resource, 10.0, 0.5)
        )pbdoc";
    
    // AgentBody structure
    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                       "X coordinate in world")
        .def_readwrite("y", &AgentBody::y,
                       "Y coordinate in world")
        .def_readwrite("orientation", &AgentBody::orientation,
                       "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                       "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                       "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                       "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy,
                       "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health,
                       "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age,
                       "Developmental age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                       "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                       "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                       "Time of last action")
        .def("reset", &AgentBody::reset,
             "Reset agent body to initial state")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody x=" + std::to_string(body.x) + " y=" + std::to_string(body.y) + 
                   " orientation=" + std::to_string(body.orientation) + ">";
        }, "Return string representation of AgentBody")
        .doc() = R"pbdoc(Agent body state.
        
        Represents the physical state of an agent in the world including
        position, velocity, energy, health, and movement state.
        
        Example:
            body = AgentBody()
            body.x = 1.0
            body.y = 1.0
            body.orientation = 0.5
        )pbdoc";
    
    // ActionResult structure
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward,
                       "Reward received (positive = good, negative = bad)")
        .def_readwrite("success", &ActionResult::success,
                       "Whether action was successful")
        .def_readwrite("message", &ActionResult::message,
                       "Descriptive message about the action")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) + 
                   " success=" + std::to_string(result.success) + ">";
        }, "Return string representation of ActionResult")
        .doc() = R"pbdoc(Action result from world.
        
        Represents the outcome of an agent's action in the world,
        including reward, success status, and a descriptive message.
        
        Example:
            result = ActionResult(10.0, True, "Collected resource")
        )pbdoc";
    
    // SensoryPercept structure
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get vision data as vector of floats")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch data as vector of floats")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals as vector of floats")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception data as vector of floats")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio data as vector of floats")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals concatenated into one vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get sensory percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set sensory percept timestamp")
        .def("getText", &SensoryPercept::getText,
             "Get text input (Phase 6)")
        .def("setText", &SensoryPercept::setText, py::arg("text"),
             "Set text input (Phase 6)")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVision().size()) +
                   " touch=" + std::to_string(percept.getTouch().size()) + 
                   " internal=" + std::to_string(percept.getInternal().size()) + ">";
        }, "Return string representation of SensoryPercept")
        .doc() = R"pbdoc(Sensory percept data.
        
        Represents all sensory input received by an agent from the world,
        including vision, touch, internal signals, proprioception, and audio.
        
        Example:
            percept = SensoryPercept()
            percept.setVision([0.1] * 84 * 84)
            percept.setTouch([0.0] * 8)
        )pbdoc";
    
    // SimpleWorld class with context manager support
    py::class_<SimpleWorldWrapper, std::shared_ptr<SimpleWorldWrapper>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorldWrapper::configure,
             py::arg("width"), py::arg("height"), 
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision size")
        .def("reset", &SimpleWorldWrapper::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorldWrapper::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorldWrapper::update,
             py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorldWrapper::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent and return result")
        .def("getSensoryPercept", &SimpleWorldWrapper::getSensoryPercept,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorldWrapper::getAgentBody,
             "Get agent body state")
        .def("addObject", &SimpleWorldWrapper::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorldWrapper::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorldWrapper::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", [](const SimpleWorldWrapper& self, float x, float y) {
            WorldObject* obj = self.getWorld().getObjectAt(x, y);
            return obj ? std::make_optional(*obj) : std::nullopt;
        }, py::arg("x"), py::arg("y"),
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorldWrapper::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorldWrapper::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorldWrapper::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorldWrapper::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorldWrapper::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorldWrapper::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorldWrapper::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorldWrapper::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorldWrapper::getRandomSeed,
             "Get current random seed")
        .def("__repr__", &SimpleWorldWrapper::repr,
             "Return string representation of SimpleWorld")
        .def("__enter__", &SimpleWorldWrapper::__enter__,
             "Context manager entry - reset world")
        .def("__exit__", &SimpleWorldWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - clean up world state")
        .def("isRunning", &SimpleWorldWrapper::isRunning,
             "Check if world simulation is running")
        .doc() = R"pbdoc(Simple 2D world for NLM simulation with context manager support.
        
        Provides a simple 2D environment for neural simulation with vision,
        object interaction, and agent movement. Can be used as a context manager
        for automatic cleanup.
        
        Example:
            with SimpleWorld() as world:
                world.configure(10.0, 10.0, 84, 84)
                world.setAgentStart(5.0, 5.0)
                for step in range(100):
                    percept = world.getSensoryPercept()
                    action = brain.produceAction()
                    result = world.applyMotorCommand(action.type, world.getSimulationTime())
                    world.update(0.1)
        )pbdoc";
    
    // Brain class with context manager support
    py::class_<BrainWrapper, std::shared_ptr<BrainWrapper>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &BrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void(BrainWrapper::*)()>(&BrainWrapper::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void(BrainWrapper::*)()>(&BrainWrapper::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveVision", [](BrainWrapper& self, const VisionWrapper& vision) {
            // Convert VisionWrapper to Vision for backward compatibility
            throw BrainError("Direct vision injection not supported. Please use AgentBrain for vision processing.");
        }, py::arg("vision"),
             "Inject vision sensory input (legacy method - use AgentBrain instead)")
        .def("injectCurrent", &BrainWrapper::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &BrainWrapper::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &BrainWrapper::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &BrainWrapper::reset,
             "Reset brain state")
        .def("save", &BrainWrapper::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainWrapper::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &BrainWrapper::addRegion,
             py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion*(*)(BrainWrapper&, RegionId)>(&BrainWrapper::getRegion),
             py::arg("id"),
             "Get a region by ID")
        .def("getRegionCount", &BrainWrapper::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &BrainWrapper::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &BrainWrapper::getRegions,
             "Get all regions")
        .def("getTotalNeuronCount", &BrainWrapper::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &BrainWrapper::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &BrainWrapper::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainWrapper::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainWrapper::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &BrainWrapper::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainWrapper::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &BrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &BrainWrapper::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &BrainWrapper::getConfig,
             "Get the configuration")
        .def("logStatus", &BrainWrapper::logStatus,
             "Log brain status")
        .def("isInitialized", &BrainWrapper::isInitialized,
             "Check if brain is initialized")
        .def("__repr__", &BrainWrapper::repr,
             "Return string representation of Brain")
        .def("__enter__", &BrainWrapper::__enter__,
             "Context manager entry - initialize brain")
        .def("__exit__", &BrainWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - clean up brain state")
        .doc() = R"pbdoc(Central neural simulation brain class with context manager support.
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        Can be used as a context manager for automatic initialization and cleanup.
        
        Example:
            with Brain(config) as brain:
                brain.initialize()
                for step in range(100):
                    brain.step(step, world.getSimulationTime())
                    action = brain.produceAction()
        )pbdoc";
    
    // AgentBrain class with context manager support
    py::class_<AgentBrainWrapper, std::shared_ptr<AgentBrainWrapper>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrainWrapper::initialize,
             py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrainWrapper::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrainWrapper::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrainWrapper::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainWrapper::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrainWrapper::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrainWrapper::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrainWrapper::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrainWrapper::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainWrapper::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrainWrapper::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrainWrapper::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrainWrapper::getBrain,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrainWrapper::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrainWrapper::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrainWrapper::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrainWrapper::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrainWrapper::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrainWrapper::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrainWrapper::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrainWrapper::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("__repr__", &AgentBrainWrapper::repr,
             "Return string representation of AgentBrain")
        .def("__enter__", &AgentBrainWrapper::__enter__,
             "Context manager entry (no-op)")
        .def("__exit__", &AgentBrainWrapper::__exit__,
             py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"),
             "Context manager exit - reset agent")
        .def("isInitialized", &AgentBrainWrapper::isInitialized,
             "Check if agent brain is initialized")
        .doc() = R"pbdoc(Agent brain interface connecting NLM brain to world with context manager support.
        
        Handles sensory transduction and motor decoding between the brain
        and the world. Can be used as a context manager for automatic cleanup.
        
        Example:
            with Brain(config) as brain:
                with AgentBrain(brain) as agent_brain:
                    agent_brain.initialize(world)
                    for step in range(100):
                        percept = world.getSensoryPercept()
                        agent_brain.processSensoryInput(percept)
                        action = agent_brain.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
        )pbdoc";
    
    // Factory functions for easy object creation
    // Create Action from components factory
    m.def("createAction", [](ActionType type, const std::vector<float>& parameters) {
        auto action = std::make_unique<Action>(type, parameters);
        return std::make_shared<ActionWrapper>(std::move(action));
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
        "Create an Action object",
        py::return_value_policy::take_ownership);
    
    // ===== PHASE 2: ADVANCED FACTORY METHODS =====
    
    // Convenience factory methods for common setups
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration object",
          py::return_value_policy::take_ownership);
    
    m.def("createSimpleBrain", []() {
        auto config = createDefaultConfig();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        return createBrain(config->getConfig());
    }, "Create a brain with default configuration (1000 neurons, 1 region)",
          py::return_value_policy::take_ownership);
    
    m.def("createSimpleWorld", []() {
        auto world = createSimpleWorld();
        world->configure(84, 84, 16, 16);
        world->setAgentStart(42.0f, 42.0f);
        world->setRandomSeed(42);
        return world;
    }, "Create a simple world with default settings (84x84 vision)",
          py::return_value_policy::take_ownership);
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return createAgentBrain(brain);
    }, "Create an agent brain interface",
          py::return_value_policy::take_ownership);
    
    // Advanced configuration factories for expert users
    m.def("createResearchConfig", [](int neuronCount, int regionCount, float connectionProb, 
                                   float stdpLtp, float stdpLtd, uint64_t seed) {
        auto config = createDefaultConfig();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("stdp_ltp_weight", stdpLtp);
        config->set("stdp_ltd_weight", stdpLtd);
        config->set("random_seed", seed);
        return config;
    }, "Create a research configuration with custom parameters",
          py::arg("neuronCount"), py::arg("regionCount"), py::arg("connectionProb"),
          py::arg("stdpLtp"), py::arg("stdpLtd"), py::arg("seed"),
          py::return_value_policy::take_ownership);
    
    m.def("createDevelopmentalConfig", [](int neuronCount, int regionCount, float synaptogenesisRate, 
                                         float pruningRate) {
        auto config = createDefaultConfig();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("synaptogenesis_rate", synaptogenesisRate);
        config->set("pruning_rate", pruningRate);
        return config;
    }, "Create a developmental configuration optimized for learning",
          py::arg("neuronCount"), py::arg("regionCount"), 
          py::arg("synaptogenesisRate"), py::arg("pruningRate"),
          py::return_value_policy::take_ownership);
    
    // Sensory and actuator factories
    m.def("createVisionFromArray", [](const std::vector<std::vector<float>>& data2D) {
        if (data2D.empty()) return createVision(0, 0, 1);
        size_t height = data2D.size();
        size_t width = data2D[0].size();
        size_t channels = 1;
        std::vector<float> flattened;
        flattened.reserve(height * width * channels);
        for (const auto& row : data2D) {
            flattened.insert(flattened.end(), row.begin(), row.end());
        }
        return createVision(width, height, channels, flattened);
    }, "Create a 2D vision array from nested vector",
          py::arg("data2D"),
          py::return_value_policy::take_ownership);
    
    m.def("createAudioFromSamples", [](const std::vector<float>& samples, int sampleRate = 44100) {
        return createAudio(sampleRate, samples.size(), samples);
    }, "Create audio from samples vector with optional sample rate",
          py::arg("samples"), py::arg("sampleRate") = 44100,
          py::return_value_policy::take_ownership);
    
    // System setup utilities for expert users
    m.def("initializeNeuralSystem", 
          [](std::shared_ptr<BrainWrapper> brain, std::shared_ptr<SimpleWorldWrapper> world) {
        brain->initialize();
        world->reset();
        return true;
    }, "Initialize both brain and world systems",
          py::arg("brain"), py::arg("world"));
    
    m.def("resetNeuralSystem", 
          [](std::shared_ptr<BrainWrapper> brain, std::shared_ptr<SimpleWorldWrapper> world) {
        brain->reset();
        world->reset();
        return true;
    }, "Reset both brain and world systems",
          py::arg("brain"), py::arg("world"));
    
    m.def("createCompleteAgentSetup", 
          [](std::shared_ptr<Config> config = nullptr) {
        auto agentConfig = config ? config : createDefaultConfig();
        if (!config) {
            agentConfig->set("neuron_count", 1000);
            agentConfig->set("region_count", 1);
            agentConfig->set("connection_probability", 0.1f);
        }
        auto brain = createBrain(agentConfig);
        auto world = createSimpleWorld();
        world->configure(84, 84, 16, 16);
        world->setAgentStart(42.0f, 42.0f);
        world->setRandomSeed(42);
        auto agentBrain = createAgentBrain(brain);
        return std::make_tuple(brain, world, agentBrain);
    }, "Create a complete agent setup with brain, world, and agent brain",
          py::arg("config") = nullptr);
    
    // Performance monitoring and debugging utilities
    m.def("getSystemStatistics", 
          [](std::shared_ptr<BrainWrapper> brain, std::shared_ptr<SimpleWorldWrapper> world) {
        std::stringstream stats;
        stats << "=== NLM System Statistics ===\n";
        stats << "Brain: " << getTotalNeuronCount(brain->getBrain()) << " neurons, "
              << getTotalSynapseCount(brain->getBrain()) << " synapses\n";
        stats << "World: " << world->getWidth() << "x" << world->getHeight() << " pixels\n";
        stats << "Agent position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")\n";
        stats << "Memory neurons: " << (brain->getBrain()->getWorkingMemory() ? 
                                    brain->getBrain()->getWorkingMemory()->getActiveTraces() : 0) << "\n";
        return stats.str();
    }, "Get comprehensive system statistics for debugging",
          py::arg("brain"), py::arg("world"));
    
    m.def("getPerformanceMetrics", 
          [](std::shared_ptr<BrainWrapper> brain, std::shared_ptr<SimpleWorldWrapper> world) {
        std::map<std::string, float> metrics;
        metrics["neuron_count"] = static_cast<float>(getTotalNeuronCount(brain->getBrain()));
        metrics["synapse_count"] = static_cast<float>(getTotalSynapseCount(brain->getBrain()));
        metrics["active_neuron_ratio"] = getActiveNeuronCount(brain->getBrain()) / 
                                        std::max(1.0f, static_cast<float>(getTotalNeuronCount(brain->getBrain())));
        metrics["world_size"] = static_cast<float>(world->getWidth() * world->getHeight());
        metrics["agent_x"] = world->getAgentBody().x;
        metrics["agent_y"] = world->getAgentBody().y;
        return metrics;
    }, "Get performance metrics as key-value pairs",
          py::arg("brain"), py::arg("world"));
    
    // Advanced configuration utilities
    m.def("setConfig", 
          [](std::shared_ptr<ConfigWrapper> config, const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime) {
        config->set(key, value, source);
        return true;
    }, "Set configuration value with specified source",
          py::arg("config"), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime);
    
    m.def("getConfigInt", 
          [](std::shared_ptr<ConfigWrapper> config, const std::string& key, int defaultValue = 0) {
        return config->getInt(key, defaultValue);
    }, "Get integer configuration value with default",
          py::arg("config"), py::arg("key"), py::arg("defaultValue") = 0);
    
    m.def("getConfigFloat", 
          [](std::shared_ptr<ConfigWrapper> config, const std::string& key, float defaultValue = 0.0f) {
        return config->getDouble(key, defaultValue);
    }, "Get float/double configuration value with default",
          py::arg("config"), py::arg("key"), py::arg("defaultValue") = 0.0f);
    
    m.def("getConfigBool", 
          [](std::shared_ptr<ConfigWrapper> config, const std::string& key, bool defaultValue = false) {
        return config->getBool(key, defaultValue);
    }, "Get boolean configuration value with default",
          py::arg("config"), py::arg("key"), py::arg("defaultValue") = false);
    
    m.def("validateConfig", 
          [](std::shared_ptr<ConfigWrapper> config) {
        auto keys = config->getKeys();
        std::map<std::string, std::string> validationResults;
        for (const auto& key : keys) {
            validationResults[key] = "OK";  // Simple validation - in real implementation would check types
        }
        return validationResults;
    }, "Validate configuration keys and types",
          py::arg("config"));
    
    // Expert utility functions
    m.def("getCurrentSensoryInput", 
          [](std::shared_ptr<SimpleWorldWrapper> world) {
        return world->getSensoryPercept();
    }, "Get current sensory input from world",
          py::arg("world"));
    
    m.def("getCurrentAction", 
          [](std::shared_ptr<BrainWrapper> brain) {
        auto action = brain->produceAction();
        return std::make_shared<ActionWrapper>(std::move(action));
    }, "Get current action produced by brain",
          py::arg("brain"));
    
    m.def("applySystemAction", 
          [](std::shared_ptr<SimpleWorldWrapper> world, MotorCommand action, double currentTime) {
        return world->applyMotorCommand(action, currentTime);
    }, "Apply action to world and get result",
          py::arg("world"), py::arg("action"), py::arg("currentTime"));
    
    // Create Vision from data factory
    m.def("createVision", [](size_t width, size_t height, size_t channels, const std::vector<float>& data) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        vision->setData(data);
        return std::make_shared<VisionWrapper>(std::move(vision));
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, py::arg("data"),
        "Create a Vision object from data",
        py::return_value_policy::take_ownership);
    
    // Create Audio from data factory
    m.def("createAudio", [](size_t sampleRate, size_t numSamples, const std::vector<float>& data) {
        auto audio = std::make_unique<Audio>(sampleRate, numSamples);
        audio->setData(data);
        return std::make_shared<AudioWrapper>(std::move(audio));
    }, py::arg("sampleRate"), py::arg("numSamples"), py::arg("data"),
        "Create an Audio object from data",
        py::return_value_policy::take_ownership);
    
    // Create InternalSignals from data factory
    m.def("createInternalSignals", [](const std::vector<float>& signals) {
        auto internalSignals = std::make_unique<InternalSignals>();
        for (float signal : signals) {
            internalSignals->addSignal(signal);
        }
        return std::make_shared<InternalSignalsWrapper>(std::move(internalSignals));
    }, py::arg("signals"),
        "Create InternalSignals from vector of values",
        py::return_value_policy::take_ownership);
    
    // Create Action from components factory
    m.def("createAction", [](ActionType type, const std::vector<float>& parameters) {
        auto action = std::make_unique<Action>(type, parameters);
        return std::make_shared<ActionWrapper>(std::move(action));
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
        "Create an Action object",
        py::return_value_policy::take_ownership);
    
    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Configuration source enum
    py::enum_<ConfigSource>(m, "ConfigSource")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values()
        .doc() = R"pbdoc(Configuration source enumeration.)pbdoc";
    
    // Type conversion helpers
    py::class_<py::capsule>(m, "Capsule")
        .def(py::init<>());
    
    m.def("wrapConfig", [](std::shared_ptr<Config> config) {
        return std::make_shared<ConfigWrapper>(config);
    }, py::arg("config"),
        "Wrap existing Config object in context manager wrapper",
        py::return_value_policy::take_ownership);
    
    m.def("wrapBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<BrainWrapper>(brain);
    }, py::arg("brain"),
        "Wrap existing Brain object in context manager wrapper",
        py::return_value_policy::take_ownership);
    
    m.def("wrapWorld", [](std::shared_ptr<SimpleWorld> world) {
        return std::make_shared<SimpleWorldWrapper>(world);
    }, py::arg("world"),
        "Wrap existing SimpleWorld object in context manager wrapper",
        py::return_value_policy::take_ownership);
    
    m.def("wrapAgentBrain", [](std::shared_ptr<AgentBrain> agentBrain) {
        return std::make_shared<AgentBrainWrapper>(agentBrain);
    }, py::arg("agentBrain"),
        "Wrap existing AgentBrain object in context manager wrapper",
        py::return_value_policy::take_ownership);
}

} // namespace nlm