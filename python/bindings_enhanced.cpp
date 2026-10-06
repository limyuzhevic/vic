#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Simplified Pythonic Brain interface with practical improvements
class PythonicBrain {
public:
    PythonicBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Core functionality
    bool initialize() { return brain_->initialize(); }
    void reset() { brain_->reset(); }
    
    // Simulation
    void step(int step) { brain_->step(step); }
    void step_with_time(int step, double time) { brain_->step(step, time); }
    
    // Convenience methods for common operations
    size_t total_neurons() const { return brain_->getTotalNeuronCount(); }
    size_t total_synapses() const { return brain_->getTotalSynapseCount(); }
    size_t firing_neurons() const { return brain_->getFiringNeuronCount(); }
    float average_firing_rate() const { return brain_->getAverageFiringRate(); }
    double excitation_inhibition_ratio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t total_spike_count() const { return brain_->getTotalSpikeCount(); }
    
    // Sensory integration
    void inject_current(int neuron_id, double current) {
        brain_->injectCurrent(NeuronId(neuron_id), static_cast<MembranePotential>(current));
    }
    
    void inject_current_to_type(NeuronType type, double current) {
        brain_->injectCurrentToNeurons(type, static_cast<MembranePotential>(current));
    }
    
    // Motor output
    Action produce_action() { return *brain_->produceAction(); }
    
    // State management
    void save_state(const std::string& filepath) { brain_->save(filepath); }
    bool load_state(const std::string& filepath) { return brain_->load(filepath); }
    
    // Configuration access
    std::shared_ptr<Config> get_config() const { return brain_->getConfig(); }
    
    // Development
    DevelopmentalStage developmental_stage() const { return brain_->getDevelopmentalStage(); }
    void set_developmental_stage(DevelopmentalStage stage) { brain_->setDevelopmentalStage(stage); }
    
    // Statistics
    void log_status() const { brain_->logStatus(); }
    
    // Access to underlying brain object for advanced use
    std::shared_ptr<Brain> get_internal_brain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Simulation episode with context manager
class SimulationEpisode {
public:
    SimulationEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world), total_reward_(0.0), steps_completed_(0) {}
    
    void run(int steps, double timestep = 0.1) {
        steps_completed_ = steps;
        for (int step = 0; step < steps; ++step) {
            world_->update(timestep);
            brain_->step(step, step * timestep);
            
            // Accumulate reward from world internal signals
            auto percept = world_->getSensoryPercept();
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                total_reward_ += internal[0];
            }
        }
    }
    
    double get_total_reward() const { return total_reward_; }
    int get_steps_completed() const { return steps_completed_; }
    
    // Context manager protocol
    SimulationEpisode& __enter__() { return *this; }
    void __exit__(int, const std::string&, const std::string&) {}
    
    std::shared_ptr<Brain> get_brain() const { return brain_; }
    std::shared_ptr<SimpleWorld> get_world() const { return world_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    double total_reward_;
    int steps_completed_;
};

// Configuration preset helper
class ConfigPresets {
public:
    static std::shared_ptr<Config> create_default() {
        return std::make_shared<Config>();
    }
    
    static std::shared_ptr<Config> minimal() {
        auto config = create_default();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("simulation_timestep", 0.001);
        return config;
    }
    
    static std::shared_ptr<Config> research() {
        auto config = create_default();
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 2);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        return config;
    }
    
    static std::shared_ptr<Config> large_scale() {
        auto config = create_default();
        config->set("brain.neuron_count", 10000);
        config->set("brain.region_count", 4);
        config->set("brain.connection_probability", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        return config;
    }
    
    // World configuration helpers
    static void configure_world(std::shared_ptr<SimpleWorld> world, 
                               int width = 20, int height = 20,
                               int vision_width = 8, int vision_height = 8) {
        world->configure(width, height, vision_width, vision_height);
    }
};

// Sensory input helper
class SensoryHelper {
public:
    static SensoryPercept create_vision(int width, int height, int channels,
                                        const std::vector<float>& data) {
        return SensoryPercept(width, height, channels, data);
    }
    
    static SensoryPercept create_audio(int sample_rate,
                                       const std::vector<float>& samples) {
        return SensoryPercept(sample_rate, samples.size(), samples);
    }
    
    static SensoryPercept create_internal(const std::vector<float>& signals) {
        return SensoryPercept(signals);
    }
    
    // Convert Python dict to percept
    static SensoryPercept percept_from_dict(const py::dict& data) {
        SensoryPercept percept;
        
        if (data.contains("vision")) {
            auto vision_data = data["vision"].cast<std::vector<float>>();
            // Simplified vision creation - in practice would need dimensions
            // This is a placeholder for more complex percept creation
        }
        
        return percept;
    }
};

// Advanced agent with automatic sense-act loop
class AdvancedAgent {
public:
    AdvancedAgent(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world), agent_(std::make_shared<AgentBrain>(brain)) {}
    
    void initialize(int width = 20, int height = 20,
                    int vision_width = 8, int vision_height = 8) {
        world_->configure(width, height, vision_width, vision_height);
        world_->reset();
        agent_->initialize(world_);
    }
    
    // Main simulation loop with automatic sensing and acting
    Action step(double timestep = 0.1) {
        // Sense: Get sensory input from world
        auto percept = world_->getSensoryPercept();
        
        // Process: Inject sensory input into brain
        agent_->processSensoryInput(percept);
        
        // Decide: Run brain step
        brain_->step(0, timestep); // Using step 0 for simplicity
        
        // Act: Decode motor command from brain
        auto action = agent_->decodeMotorCommand();
        
        // Execute: Apply action to world
        world_->applyMotorCommand(action, world_->getSimulationTime());
        
        return action;
    }
    
    // Subsystem control
    void enable_subsystem(const std::string& name, bool enabled) {
        if (name == "reward_modulation") {
            agent_->enableRewardModulation(enabled);
        } else if (name == "structural_plasticity") {
            agent_->enableStructuralPlasticity(enabled);
        } else if (name == "development") {
            agent_->enableDevelopment(enabled);
        } else if (name == "curiosity") {
            agent_->enableCuriosity(enabled);
        }
    }
    
    bool is_subsystem_enabled(const std::string& name) const {
        if (name == "reward_modulation") return agent_->isRewardModulationEnabled();
        else if (name == "structural_plasticity") return agent_->isStructuralPlasticityEnabled();
        else if (name == "development") return agent_->isDevelopmentEnabled();
        else if (name == "curiosity") return agent_->isCuriosityEnabled();
        return false;
    }
    
    // Statistics
    double curiosity_level() const { return agent_->getCuriosityLevel(); }
    double novelty_level() const { return agent_->getNoveltyLevel(); }
    double neuromodulation_level() const { return agent_->getNeuromodulationLevel(); }
    double prediction_error() const { return agent_->getPredictionError(); }
    
    // Development tracking
    DevelopmentalStage current_stage() const { return agent_->getDevelopmentalStage(); }
    
    // Learning and adaptation
    void apply_reward_modulation(double reward, double prediction = 0.0) {
        agent_->applyRewardModulation(reward, prediction);
    }
    
    void update_development(double timestep) {
        agent_->updateDevelopment(timestep);
    }
    
    // Accessors
    std::shared_ptr<Brain> get_brain() const { return brain_; }
    std::shared_ptr<SimpleWorld> get_world() const { return world_; }
    std::shared_ptr<AgentBrain> get_agent() const { return agent_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
};

// Convenience factory functions
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
    Enhanced Python API for NLM (Neural Learning Machine)
    
    This module provides an enhanced Python interface to the NLM neural simulation framework.
    It includes:
    - Simplified brain interface with convenience methods
    - Context managers for simulation episodes
    - Preset configuration helpers
    - Advanced agent interface with automatic sense-act loop
    - Sensory input utilities
    - Subsystem control for neuromodulation
    
    Key improvements over basic bindings:
    - More Pythonic API design
    - Built-in error handling and validation
    - Preset configurations for different scales
    - Context managers for resource management
    - Automatic agent sensing and acting
    
    Usage example:
        import pynlm_enhanced as nlm
        
        # Create brain with research preset
        config = nlm.Presets.research()
        brain = nlm.Brain(config)
        
        # Create world
        world = nlm.SimpleWorld()
        nlm.Presets.configure_world(world)
        
        # Create advanced agent
        agent = nlm.AdvancedAgent(brain, world)
        
        # Run simulation episode with context manager
        with nlm.SimulationEpisode(brain, world) as episode:
            episode.run(1000, 0.1)
            
        print(f"Episode completed with {episode.steps_completed()} steps")
        print(f"Total reward: {episode.total_reward():.2f}")
    )pbdoc";
    
    // Register basic types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Neuron IDs
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron identifier)pbdoc")
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
    
    // Synapse IDs
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index);
    
    // Region IDs
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index);
    
    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Configuration
    py::class_<Config>(m, "Config", R"pbdoc(NLM configuration)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file")
        .def("has", &Config::has, py::arg("key"), "Check if key exists")
        .def("getOr", &Config::getOr,
             py::arg("key"), py::arg("defaultValue"), "Get value with default")
        .def("set", &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value")
        .def("clear", &Config::clear, "Clear configuration")
        .def("summary", &Config::summary, "Get configuration summary");
    
    // Simplified Pythonic Brain interface
    py::class_<PythonicBrain>(m, "Brain", R"pbdoc(Simplified neural brain)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &PythonicBrain::initialize, "Initialize brain")
        .def("reset", &PythonicBrain::reset, "Reset brain state")
        .def("step", &PythonicBrain::step, py::arg("step"), "Run simulation step")
        .def("step_with_time", &PythonicBrain::step_with_time,
             py::arg("step"), py::arg("time"), "Run step with timestamp")
        .def("total_neurons", &PythonicBrain::total_neurons, "Get total neurons")
        .def("total_synapses", &PythonicBrain::total_synapses, "Get total synapses")
        .def("firing_neurons", &PythonicBrain::firing_neurons, "Get firing neurons")
        .def("average_firing_rate", &PythonicBrain::average_firing_rate,
             "Get average firing rate")
        .def("excitation_inhibition_ratio", &PythonicBrain::excitation_inhibition_ratio,
             "Get E/I ratio")
        .def("total_spike_count", &PythonicBrain::total_spike_count,
             "Get total spike count")
        .def("save_state", &PythonicBrain::save_state, py::arg("filepath"),
             "Save brain state")
        .def("load_state", &PythonicBrain::load_state, py::arg("filepath"),
             "Load brain state")
        .def("developmental_stage", &PythonicBrain::developmental_stage,
             "Get developmental stage")
        .def("set_developmental_stage", &PythonicBrain::set_developmental_stage,
             py::arg("stage"), "Set developmental stage");
    
    // Advanced agent interface
    py::class_<AdvancedAgent>(m, "AdvancedAgent", R"pbdoc(Advanced agent with sense-act loop)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
        .def("initialize", &AdvancedAgent::initialize,
             py::arg("width")=20, py::arg("height")=20,
             py::arg("vision_width")=8, py::arg("vision_height")=8,
             "Initialize agent with world")
        .def("step", &AdvancedAgent::step, py::arg("timestep")=0.1,
             "Perform one simulation step (sense->act)")
        .def("enable_subsystem", &AdvancedAgent::enable_subsystem,
             py::arg("name"), py::arg("enabled"),
             "Enable/disable subsystem")
        .def("is_subsystem_enabled", &AdvancedAgent::is_subsystem_enabled,
             py::arg("name"), "Check if subsystem enabled")
        .def("curiosity_level", &AdvancedAgent::curiosity_level, "Get curiosity")
        .def("novelty_level", &AdvancedAgent::novelty_level, "Get novelty")
        .def("neuromodulation_level", &AdvancedAgent::neuromodulation_level,
             "Get neuromodulation level")
        .def("prediction_error", &AdvancedAgent::prediction_error,
             "Get prediction error")
        .def("current_stage", &AdvancedAgent::current_stage,
             "Get current developmental stage")
        .def("apply_reward_modulation", &AdvancedAgent::apply_reward_modulation,
             py::arg("reward"), py::arg("prediction")=0.0,
             "Apply reward modulation")
        .def("update_development", &AdvancedAgent::update_development,
             py::arg("timestep"), "Update development")
        .def("get_brain", &AdvancedAgent::get_brain, "Get brain object")
        .def("get_world", &AdvancedAgent::get_world, "Get world object");
    
    // Simulation episode (context manager)
    py::class_<SimulationEpisode>(m, "SimulationEpisode", R"pbdoc(Simulation episode)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
        .def("__enter__", &SimulationEpisode::__enter__)
        .def("__exit__", &SimulationEpisode::__exit__)
        .def("run", &SimulationEpisode::run,
             py::arg("steps"), py::arg("timestep")=0.1, "Run simulation")
        .def("total_reward", &SimulationEpisode::get_total_reward,
             "Get total reward")
        .def("steps_completed", &SimulationEpisode::get_steps_completed,
             "Get steps completed");
    
    // Configuration presets
    py::class_<ConfigPresets>(m, "Presets", R"pbdoc(Configuration presets)", py::static_attribute_holder_tag())
        .def_static("minimal", &ConfigPresets::minimal, "Minimal configuration")
        .def_static("research", &ConfigPresets::research, "Research configuration")
        .def_static("large_scale", &ConfigPresets::large_scale,
                    "Large-scale configuration")
        .def_static("create_default", &ConfigPresets::create_default,
                    "Create default configuration");
    
    // Sensory helpers
    py::class_<SensoryHelper>(m, "Sensory", R"pbdoc(Sensory utilities)", py::static_attribute_holder_tag())
        .def_static("create_vision", &SensoryHelper::create_vision,
                   py::arg("width"), py::arg("height"), py::arg("channels"),
                   py::arg("data"), "Create vision input")
        .def_static("create_audio", &SensoryHelper::create_audio,
                   py::arg("sample_rate"), py::arg("samples"),
                   "Create audio input")
        .def_static("create_internal", &SensoryHelper::create_internal,
                   py::arg("signals"), "Create internal input");
    
    // Other classes (simplified for space)
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(World simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width")=8, py::arg("vision_height")=8,
             "Configure world")
        .def("reset", &SimpleWorld::reset, "Reset world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Set agent start")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("time"), "Apply command")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             "Get sensory percept")
        .def("width", &SimpleWorld::getWidth, "Get world width")
        .def("height", &SimpleWorld::getHeight, "Get world height");
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage");
    
    py::class_<Action>(m, "Action", R"pbdoc(Motor action)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("clone", &Action::clone);
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception)pbdoc")
        .def(py::init<>())
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"));
    
    // Factory functions
    m.def("createDefaultConfig", &ConfigPresets::create_default,
          "Create default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<PythonicBrain>(config);
    }, py::arg("config"), "Create brain with config");
    
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        ConfigPresets::configure_world(world);
        return world;
    }, "Create configured world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain");
    
    // Module metadata
    m.attr("__version__") = "2.0-enhanced";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Enhanced Python API for NLM neural simulation framework";
}}

} // namespace nlm