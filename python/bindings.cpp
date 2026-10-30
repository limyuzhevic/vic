#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;

// Forward declarations for context managers
class PyBrainContextManager;
class PySimpleWorldContextManager;

// Context manager implementations
class PyBrainContextManager {
public:
    explicit PyBrainContextManager(std::shared_ptr<nlm::Brain> brain) 
        : brain_(brain), initialized_(false) {
    }
    
    ~PyBrainContextManager() {
        cleanup();
    }
    
    PyBrainContextManager& operator=(const PyBrainContextManager&) = delete;
    PyBrainContextManager(const PyBrainContextManager&) = delete;
    
    bool enter() {
        if (brain_ && !initialized_) {
            initialized_ = brain_->initialize();
        }
        return initialized_;
    }
    
    void exit(py::object /* exc_type */, py::object /* exc_value */, py::object /* exc_traceback */) {
        cleanup();
    }
    
    std::shared_ptr<nlm::Brain> getBrain() const {
        return brain_;
    }
    
    bool isInitialized() const {
        return initialized_;
    }
    
private:
    void cleanup() {
        if (brain_) {
            brain_->reset();
        }
        initialized_ = false;
    }
    
    std::shared_ptr<nlm::Brain> brain_;
    bool initialized_;
};

class PySimpleWorldContextManager {
public:
    explicit PySimpleWorldContextManager(std::shared_ptr<nlm::SimpleWorld> world) 
        : world_(world), reset_(true) {
    }
    
    ~PySimpleWorldContextManager() {
        if (reset_ && world_) {
            world_->reset();
        }
    }
    
    PySimpleWorldContextManager& operator=(const PySimpleWorldContextManager&) = delete;
    PySimpleWorldContextManager(const PySimpleWorldContextManager&) = delete;
    
    bool enter() {
        return true;
    }
    
    void exit(py::object /* exc_type */, py::object /* exc_value */, py::object /* exc_traceback */) {
        if (world_) {
            world_->reset();
        }
    }
    
    std::shared_ptr<nlm::SimpleWorld> getWorld() const {
        return world_;
    }
    
    void preventReset() {
        reset_ = false;
    }
    
private:
    std::shared_ptr<nlm::SimpleWorld> world_;
    bool reset_;
};

// Helper functions
namespace helpers {
    
    // Brain factory function with sensible defaults
    std::shared_ptr<nlm::Brain> createBrainWithDefaults(const std::string& name = "default_brain") {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        return brain;
    }
    
    // SimpleWorld factory function with sensible defaults
    std::shared_ptr<nlm::SimpleWorld> createWorldWithDefaults(float width = 20.0f, float height = 20.0f) {
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(width, height, 16, 16);
        world->setRandomSeed(42);
        world->reset();
        return world;
    }
    
    // Utility function for running brain-world simulation
    float runSimulation(const std::shared_ptr<nlm::Brain>& brain, 
                       const std::shared_ptr<nlm::SimpleWorld>& world,
                       uint64_t steps,
                       float timestep = 0.001f) {
        if (!brain || !world) {
            throw std::runtime_error("Brain or world is null");
        }
        
        float totalReward = 0.0f;
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            world->update(timestep);
            totalReward += brain->getAverageFiringRate();
        }
        
        return totalReward / steps;
    }
    
    // Configuration utility functions
    std::shared_ptr<nlm::Config> createDefaultConfig() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
        return config;
    }
    
    // Get neural statistics as a dictionary
    py::dict getBrainStats(const nlm::Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain.getTotalSpikeCount();
        stats["pending_spike_events"] = brain.getPendingSpikeEventCount();
        return stats;
    }
    
    // Get world statistics as a dictionary
    py::dict getWorldStats(const nlm::SimpleWorld& world) {
        py::dict stats;
        stats["width"] = world.getWidth();
        stats["height"] = world.getHeight();
        stats["simulation_time"] = world.getSimulationTime();
        stats["max_energy"] = world.getMaxEnergy();
        stats["energy_decay_rate"] = world.getEnergyDecayRate();
        stats["random_seed"] = world.getRandomSeed();
        return stats;
    }
}

// Pythonic Brain wrapper class
class PyBrain {
public:
    PyBrain(const PyBrain&) = delete;
    PyBrain& operator=(const PyBrain&) = delete;
    
    explicit PyBrain(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {
    }
    
    ~PyBrain() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Context manager support
    PyBrainContextManager enter_context() {
        return PyBrainContextManager(brain_);
    }
    
    // Python properties
    bool isInitialized() const {
        return brain_ && brain_->initialize();
    }
    
    size_t getNeuronCount() const {
        return brain_ ? brain_->getTotalNeuronCount() : 0;
    }
    
    size_t getSynapseCount() const {
        return brain_ ? brain_->getTotalSynapseCount() : 0;
    }
    
    float getAverageFiringRate() const {
        return brain_ ? brain_->getAverageFiringRate() : 0.0f;
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_ ? brain_->getExcitationInhibitionRatio() : 0.0f;
    }
    
    // Step methods with different signatures
    void step(uint64_t step) {
        if (brain_) {
            brain_->step(step);
        }
    }
    
    void stepWithTime(uint64_t step, float time) {
        if (brain_) {
            brain_->step(step, time);
        }
    }
    
    // Save/load
    bool save(const std::string& filepath) const {
        return brain_ && brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_ && brain_->load(filepath);
    }
    
    // Access to sub-systems
    nlm::SpikeSystem* getSpikeSystem() {
        return brain_ ? brain_->getSpikeSystem() : nullptr;
    }
    
    nlm::STDP* getSTDP() {
        return brain_ ? brain_->getSTDP() : nullptr;
    }
    
    nlm::Hebbian* getHebbian() {
        return brain_ ? brain_->getHebbian() : nullptr;
    }
    
    nlm::StructuralPlasticity* getStructuralPlasticity() {
        return brain_ ? brain_->getStructuralPlasticity() : nullptr;
    }
    
    // Region management
    size_t getRegionCount() const {
        return brain_ ? brain_->getRegionCount() : 0;
    }
    
    std::vector<uint64_t> getRegionIds() const {
        return brain_ ? brain_->getRegionIds() : std::vector<uint64_t>();
    }
    
    nlm::Brain* getRawBrain() {
        return brain_.get();
    }
    
    const nlm::Brain* getRawBrain() const {
        return brain_.get();
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
};

// Pythonic SimpleWorld wrapper class
class PySimpleWorld {
public:
    PySimpleWorld(const PySimpleWorld&) = delete;
    PySimpleWorld& operator=(const PySimpleWorld&) = delete;
    
    explicit PySimpleWorld(std::shared_ptr<nlm::SimpleWorld> world) : world_(world) {
    }
    
    ~PySimpleWorld() {
        if (world_) {
            world_->reset();
        }
    }
    
    // Context manager support
    PySimpleWorldContextManager enter_context() {
        return PySimpleWorldContextManager(world_);
    }
    
    // Python properties
    float getWidth() const {
        return world_ ? world_->getWidth() : 0.0f;
    }
    
    float getHeight() const {
        return world_ ? world_->getHeight() : 0.0f;
    }
    
    float getSimulationTime() const {
        return world_ ? world_->getSimulationTime() : 0.0f;
    }
    
    float getMaxEnergy() const {
        return world_ ? world_->getMaxEnergy() : 0.0f;
    }
    
    void setMaxEnergy(float energy) {
        if (world_) {
            world_->setMaxEnergy(energy);
        }
    }
    
    float getEnergyDecayRate() const {
        return world_ ? world_->getEnergyDecayRate() : 0.0f;
    }
    
    void setEnergyDecayRate(float rate) {
        if (world_) {
            world_->setEnergyDecayRate(rate);
        }
    }
    
    uint64_t getRandomSeed() const {
        return world_ ? world_->getRandomSeed() : 0;
    }
    
    void setRandomSeed(uint64_t seed) {
        if (world_) {
            world_->setRandomSeed(seed);
        }
    }
    
    // World update
    void update(float timestep = 0.1f) {
        if (world_) {
            world_->update(timestep);
        }
    }
    
    // Reset
    void reset() {
        if (world_) {
            world_->reset();
        }
    }
    
    // Agent control
    void setAgentStart(float x, float y) {
        if (world_) {
            world_->setAgentStart(x, y);
        }
    }
    
    // Object management
    void addObject(const nlm::WorldObject& obj) {
        if (world_) {
            world_->addObject(obj);
        }
    }
    
    nlm::WorldObject* getObjectAt(float x, float y) {
        return world_ ? world_->getObjectAt(x, y) : nullptr;
    }
    
    // Access to raw world
    nlm::SimpleWorld* getRawWorld() {
        return world_.get();
    }
    
    const nlm::SimpleWorld* getRawWorld() const {
        return world_.get();
    }
    
private:
    std::shared_ptr<nlm::SimpleWorld> world_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM Python bindings - Improved version with context managers and Pythonic API";
    
    // Version information
    m.attr("__version__") = "2.0.0";
    
    // Exception classes
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // ==============================================================================
    // Core types
    // ==============================================================================
    
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .export_values();
    
    py::enum_<nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
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
        .export_values();
    
    // Basic value types
    py::class_<nlm::NeuronId>(m, "NeuronId")
        .def(py::init<uint64_t>(), py::arg("index"))
        .def("index", &nlm::NeuronId::index)
        .def("__str__", [](const nlm::NeuronId& id) { return "NeuronId(" + std::to_string(id.index()) + ")"; });
    
    py::class_<nlm::SynapseId>(m, "SynapseId")
        .def(py::init<uint64_t>(), py::arg("index"))
        .def("index", &nlm::SynapseId::index)
        .def("__str__", [](const nlm::SynapseId& id) { return "SynapseId(" + std::to_string(id.index()) + ")"; });
    
    py::class_<nlm::RegionId>(m, "RegionId")
        .def(py::init<uint64_t>(), py::arg("index"))
        .def("index", &nlm::RegionId::index)
        .def("__str__", [](const nlm::RegionId& id) { return "RegionId(" + std::to_string(id.index()) + ")"; });
    
    // ==============================================================================
    // Configuration
    // ==============================================================================
    
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::User)
        .def("get", &nlm::Config::get, py::arg("key"), py::arg("default_value"))
        .def("has_key", &nlm::Config::has_key, py::arg("key"))
        .def("remove", &nlm::Config::remove, py::arg("key"))
        .def("clear", &nlm::Config::clear)
        .def("copy", &nlm::Config::copy)
        .def("merge", &nlm::Config::merge, py::arg("other"));
    
    // ==============================================================================
    // WorldObject
    // ==============================================================================
    
    py::class_<nlm::WorldObject>(m, "WorldObject")
        .def(py::init<>())  // Default constructor
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active)
        .def("__str__", [](const nlm::WorldObject& obj) {
            return "WorldObject(x=" + std::to_string(obj.x) + ", y=" + std::to_string(obj.y) + 
                   ", type=" + std::to_string(static_cast<int>(obj.type)) + ")";
        });
    
    // ==============================================================================
    // Action
    // ==============================================================================
    
    py::class_<nlm::Action>(m, "Action")
        .def(py::init<>())
        .def_readwrite("value", &nlm::Action::value)
        .def_readwrite("success", &nlm::Action::success)
        .def_readwrite("message", &nlm::Action::message);
    
    // ==============================================================================
    // ActionResult
    // ==============================================================================
    
    py::class_<nlm::ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, const std::string&>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message);
    
    // ==============================================================================
    // SimpleWorld
    // ==============================================================================
    
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"), 
             py::arg("vision_width") = 16, py::arg("vision_height") = 16)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("set_agent_start", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep") = 0.1f)
        .def("get_sensory_percept", &nlm::SimpleWorld::getSensoryPercept)
        .def("get_agent_body", &nlm::SimpleWorld::getAgentBody)
        .def("add_object", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_object_at", &nlm::SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
        .def("get_width", &nlm::SimpleWorld::getWidth)
        .def("get_height", &nlm::SimpleWorld::getHeight)
        .def("get_max_energy", &nlm::SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &nlm::SimpleWorld::setMaxEnergy, py::arg("energy"))
        .def("get_energy_decay_rate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("rate"))
        .def("get_simulation_time", &nlm::SimpleWorld::getSimulationTime)
        .def("set_random_seed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &nlm::SimpleWorld::getRandomSeed)
        .def("next_random", &nlm::SimpleWorld::nextRandom);
    
    // ==============================================================================
    // PySimpleWorld wrapper class
    // ==============================================================================
    
    py::class_<PySimpleWorld>(m, "PySimpleWorld")
        .def(py::init<std::shared_ptr<nlm::SimpleWorld>>(), py::arg("world"))
        .def("enter_context", &PySimpleWorld::enter_context)
        .def("get_width", &PySimpleWorld::getWidth)
        .def("get_height", &PySimpleWorld::getHeight)
        .def("get_simulation_time", &PySimpleWorld::getSimulationTime)
        .def("get_max_energy", &PySimpleWorld::getMaxEnergy)
        .def("set_max_energy", &PySimpleWorld::setMaxEnergy, py::arg("energy"))
        .def("get_energy_decay_rate", &PySimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &PySimpleWorld::setEnergyDecayRate, py::arg("rate"))
        .def("get_random_seed", &PySimpleWorld::getRandomSeed)
        .def("set_random_seed", &PySimpleWorld::setRandomSeed, py::arg("seed"))
        .def("update", &PySimpleWorld::update, py::arg("timestep") = 0.1f)
        .def("reset", &PySimpleWorld::reset)
        .def("set_agent_start", &PySimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("add_object", &PySimpleWorld::add_object, py::arg("obj"))
        .def("get_object_at", &PySimpleWorld::get_object_at, py::arg("x"), py::arg("y"))
        .def("get_raw_world", &PySimpleWorld::getRawWorld)
        .def("__enter__", &PySimpleWorld::enter_context)
        .def("__exit__", [](PySimpleWorld& self, py::object, py::object, py::object) {
            auto ctx = self.enter_context();
            ctx.exit(py::object(), py::object(), py::object());
        });
    
    // ==============================================================================
    // Brain
    // ==============================================================================
    
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step, py::arg("step"))
        .def("step_with_time", &nlm::Brain::step, py::arg("step"), py::arg("time"))
        .def("receive_sensory_input", &nlm::Brain::receiveSensoryInput, py::arg("input"))
        .def("inject_current", &nlm::Brain::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("inject_current_to_neurons", &nlm::Brain::injectCurrentToNeurons, 
             py::arg("type"), py::arg("current"))
        .def("get_spike_system", &nlm::Brain::getSpikeSystem)
        .def("get_stdp", &nlm::Brain::getSTDP)
        .def("get_hebbian", &nlm::Brain::getHebbian)
        .def("get_structural_plasticity", &nlm::Brain::getStructuralPlasticity)
        .def("get_excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("get_total_spike_count", &nlm::Brain::getTotalSpikeCount)
        .def("get_pending_spike_event_count", &nlm::Brain::getPendingSpikeEventCount)
        .def("produce_action", &nlm::Brain::produceAction)
        .def("apply_neuromodulation", &nlm::Brain::applyNeuromodulation, py::arg("signal"))
        .def("update_plasticity", &nlm::Brain::updatePlasticity)
        .def("develop", &nlm::Brain::develop)
        .def("reset", &nlm::Brain::reset)
        .def("save", &nlm::Brain::save, py::arg("filepath"))
        .def("load", &nlm::Brain::load, py::arg("filepath"))
        .def("add_region", &nlm::Brain::addRegion, py::arg("name") = "")
        .def("get_region", &nlm::Brain::getRegion, py::arg("id"))
        .def("get_region_count", &nlm::Brain::getRegionCount)
        .def("get_region_ids", &nlm::Brain::getRegionIds)
        .def("get_regions", &nlm::Brain::getRegions)
        .def("add_inter_region_connection", &nlm::Brain::addInterRegionConnection, 
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("remove_inter_region_connection", &nlm::Brain::removeInterRegionConnection, 
             py::arg("source"), py::arg("target"))
        .def("get_total_neuron_count", &nlm::Brain::getTotalNeuronCount)
        .def("get_total_synapse_count", &nlm::Brain::getTotalSynapseCount)
        .def("get_active_neuron_count", &nlm::Brain::getActiveNeuronCount)
        .def("get_firing_neuron_count", &nlm::Brain::getFiringNeuronCount)
        .def("get_average_firing_rate", &nlm::Brain::getAverageFiringRate)
        .def("get_working_memory", &nlm::Brain::getWorkingMemory)
        .def("get_episodic_memory", &nlm::Brain::getEpisodicMemory)
        .def("get_associative_memory", &nlm::Brain::getAssociativeMemory)
        .def("get_prediction_system", &nlm::Brain::getPredictionSystem)
        .def("get_planner", &nlm::Brain::getPlanner)
        .def("get_concept_formation", &nlm::Brain::getConceptFormation)
        .def("get_attention", &nlm::Brain::getAttention)
        .def("get_development_system", &nlm::Brain::getDevelopmentSystem)
        .def("get_developmental_stage", &nlm::Brain::getDevelopmentalStage)
        .def("set_developmental_stage", &nlm::Brain::setDevelopmentalStage, py::arg("stage"))
        .def("get_dopamine", &nlm::Brain::getDopamine)
        .def("get_curiosity", &nlm::Brain::getCuriosity)
        .def("get_novelty", &nlm::Brain::getNovelty)
        .def("get_prediction_error_signal", &nlm::Brain::getPredictionErrorSignal)
        .def("get_config", &nlm::Brain::getConfig)
        .def("get_random_generator", &nlm::Brain::getRandomGenerator)
        .def("log_status", &nlm::Brain::logStatus)
        .def("move_construct", &nlm::Brain::Brain)
        .def("move_assign", &nlm::Brain::operator=);
    
    // ==============================================================================
    // PyBrain wrapper class
    // ==============================================================================
    
    py::class_<PyBrain>(m, "PyBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("enter_context", &PyBrain::enter_context)
        .def("is_initialized", &PyBrain::isInitialized)
        .def("get_neuron_count", &PyBrain::getNeuronCount)
        .def("get_synapse_count", &PyBrain::getSynapseCount)
        .def("get_average_firing_rate", &PyBrain::getAverageFiringRate)
        .def("get_excitation_inhibition_ratio", &PyBrain::getExcitationInhibitionRatio)
        .def("step", &PyBrain::step, py::arg("step"))
        .def("step_with_time", &PyBrain::stepWithTime, py::arg("step"), py::arg("time"))
        .def("save", &PyBrain::save, py::arg("filepath"))
        .def("load", &PyBrain::load, py::arg("filepath"))
        .def("get_spike_system", &PyBrain::getSpikeSystem)
        .def("get_stdp", &PyBrain::getSTDP)
        .def("get_hebbian", &PyBrain::getHebbian)
        .def("get_structural_plasticity", &PyBrain::getStructuralPlasticity)
        .def("get_region_count", &PyBrain::getRegionCount)
        .def("get_region_ids", &PyBrain::getRegionIds)
        .def("get_raw_brain", &PyBrain::getRawBrain)
        .def("__enter__", &PyBrain::enter_context)
        .def("__exit__", [](PyBrain& self, py::object, py::object, py::object) {
            auto ctx = self.enter_context();
            ctx.exit(py::object(), py::object(), py::object());
        });
    
    // ==============================================================================
    // AgentBrain
    // ==============================================================================
    
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"))
        .def("get_sensory_input_size", &nlm::AgentBrain::getSensoryInputSize)
        .def("get_motor_output_size", &nlm::AgentBrain::getMotorOutputSize)
        .def("process_sensory_input", &nlm::AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decode_motor_command", &nlm::AgentBrain::decodeMotorCommand)
        .def("apply_reward_modulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0f)
        .def("update_development", &nlm::AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("get_developmental_stage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("get_neuromodulation_level", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("get_curiosity_level", &nlm::AgentBrain::getCuriosityLevel)
        .def("get_novelty_level", &nlm::AgentBrain::getNoveltyLevel)
        .def("get_prediction_error", &nlm::AgentBrain::getPredictionError)
        .def("reset", &nlm::AgentBrain::reset)
        .def("get_brain", &nlm::AgentBrain::getBrain)
        .def("enable_reward_modulation", &nlm::AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enable_structural_plasticity", &nlm::AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enable_development", &nlm::AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enable_curiosity", &nlm::AgentBrain::enableCuriosity, py::arg("enable"))
        .def("is_reward_modulation_enabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &nlm::AgentBrain::isCuriosityEnabled);
    
    // ==============================================================================
    // Observation
    // ==============================================================================
    
    py::class_<nlm::Observation>(m, "Observation")
        .def(py::init<>())
        .def("get_data", &nlm::Observation::getData)
        .def("get_dimensions", &nlm::Observation::getDimensions)
        .def("set_from_percept", &nlm::Observation::setFromPercept, py::arg("percept"));
    
    // ==============================================================================
    // Utility functions and factory functions
    // ==============================================================================
    
    m.def("create_brain_with_defaults", &helpers::createBrainWithDefaults, py::arg("name") = "default_brain",
          "Factory function to create a brain with sensible default configuration");
    
    m.def("create_world_with_defaults", &helpers::createWorldWithDefaults, py::arg("width") = 20.0f, py::arg("height") = 20.0f,
          "Factory function to create a world with sensible default configuration");
    
    m.def("run_simulation", &helpers::runSimulation, py::arg("brain"), py::arg("world"), py::arg("steps"),
          py::arg("timestep") = 0.001f, "Run a brain-world simulation for the specified number of steps");
    
    m.def("create_default_config", &helpers::createDefaultConfig,
          "Create a configuration with sensible default values for a new brain");
    
    m.def("get_brain_stats", &helpers::getBrainStats, py::arg("brain"),
          "Get brain statistics as a dictionary (neurons, synapses, firing rates, etc.)");
    
    m.def("get_world_stats", &helpers::getWorldStats, py::arg("world"),
          "Get world statistics as a dictionary (dimensions, settings, etc.)");
    
    m.def("version", []() { return "2.0.0"; }, "Get the version of the pynlm bindings");
    
    // ==============================================================================
    // Context manager functions
    // ==============================================================================
    
    m.def("brain_context", [](std::shared_ptr<nlm::Brain> brain) {
        return py::make_scope_guard([brain]() {
            if (brain) {
                brain->reset();
            }
        });
    }, py::arg("brain"), "Context manager for automatic brain cleanup");
    
    m.def("world_context", [](std::shared_ptr<nlm::SimpleWorld> world) {
        return py::make_scope_guard([world]() {
            if (world) {
                world->reset();
            }
        });
    }, py::arg("world"), "Context manager for automatic world cleanup");
    
    // ==============================================================================
    // Example usage helper functions
    // ==============================================================================
    
    m.def("example_brain_creation", []() {
        py::dict example;
        example["description"] = "Example of creating and using a brain";
        example["code"] = R"(
from pynlm import create_brain_with_defaults, get_brain_stats

brain = create_brain_with_defaults("my_brain")
print(f"Brain created with {brain.get_neuron_count()} neurons")
print("Stats:", get_brain_stats(brain))
        )";
        return example;
    });
    
    m.def("example_world_creation", []() {
        py::dict example;
        example["description"] = "Example of creating and using a world";
        example["code"] = R"(
from pynlm import create_world_with_defaults, get_world_stats

world = create_world_with_defaults(30.0, 30.0)
print(f"World created with dimensions: {world.get_width()}x{world.get_height()}")
print("Stats:", get_world_stats(world))
        )";
        return example;
    });
    
    m.def("example_simulation", []() {
        py::dict example;
        example["description"] = "Example of running a simulation";
        example["code"] = R"(
from pynlm import create_brain_with_defaults, create_world_with_defaults, run_simulation

brain = create_brain_with_defaults()
world = create_world_with_defaults()
avg_firing_rate = run_simulation(brain, world, 1000)
print(f"Average firing rate over 1000 steps: {avg_firing_rate:.2f}")
        )";
        return example;
    });
    
    m.def("example_context_manager", []() {
        py::dict example;
        example["description"] = "Example of using context managers for resource cleanup";
        example["code"] = R"(
from pynlm import create_brain_with_defaults, create_world_with_defaults

brain = create_brain_with_defaults()
world = create_world_with_defaults()

with brain.enter_context():
    print("Brain initialized and managed by context manager")
    brain.step_with_time(0, 0.0)

with world.enter_context():
    print("World reset and managed by context manager")
    world.update(0.1)
        )";
        return example;
    });
    
    // ==============================================================================
    // Backward compatibility functions
    // ==============================================================================
    
    m.def("backward_compatibility_warning", []() {
        py::dict warning;
        warning["message"] = "This is version 2.0.0 with improved API. The old API is still supported for compatibility.";
        warning["changes"] = py::list({
            "Added context managers for automatic resource cleanup",
            "Added PyBrain and PySimpleWorld wrapper classes with Pythonic properties",
            "Added factory functions with sensible defaults",
            "Added utility functions for common workflows",
            "Improved error handling and validation",
            "Reduced code duplication"
        });
        return warning;
    });
}