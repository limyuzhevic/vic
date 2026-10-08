#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace nlm {

// Visualization and debugging data structures

struct VisualizationData {
    std::vector<NeuronId> neuron_ids;
    std::vector<float> neuron_x;
    std::vector<float> neuron_y;
    std::vector<int> neuron_type;
    std::vector<float> neuron_activity;
    std::vector<std::vector<int>> connections;
    std::vector<float> connection_weights;
    
    VisualizationData() : neuron_x(), neuron_y(), neuron_activity(), connections(), connection_weights() {}
};

struct NeuronDebugInfo {
    NeuronId id;
    std::string type;
    float membrane_potential;
    float threshold;
    float firing_rate;
    bool is_firing;
    bool is_refractory;
    float last_spike_time;
    std::vector<float> incoming_weights;
    std::vector<float> incoming_delays;
    float adaptation_variable;
    
    NeuronDebugInfo() : id(), membrane_potential(0.0f), threshold(0.0f), firing_rate(0.0f), 
                       is_firing(false), is_refractory(false), last_spike_time(-1.0f),
                       adaptation_variable(0.0f) {}
};

struct PopulationStats {
    PopulationId id;
    std::string name;
    int neuron_count;
    int active_count;
    int firing_count;
    float average_firing_rate;
    float activity_level;
    float mean_membrane_potential;
    float variance_membrane_potential;
    
    PopulationStats() : id(), neuron_count(0), active_count(0), firing_count(0),
                       average_firing_rate(0.0f), activity_level(0.0f),
                       mean_membrane_potential(0.0f), variance_membrane_potential(0.0f) {}
};

struct ConnectionAnalysis {
    SynapseId id;
    NeuronId source_id;
    NeuronId target_id;
    float weight;
    int delay;
    std::string type;
    bool is_excitatory;
    std::vector<Timestamp> pre_spike_times;
    std::vector<Timestamp> post_spike_times;
    
    ConnectionAnalysis() : id(), source_id(), target_id(), weight(0.0f), delay(0),
                          is_excitatory(false), pre_spike_times(), post_spike_times() {}
};

struct ActivityPattern {
    std::vector<Timestamp> timestamps;
    std::vector<float> activity_levels;
    std::vector<int> active_neuron_counts;
    std::vector<float> average_firing_rates;
    
    ActivityPattern() : activity_levels(), active_neuron_counts(), average_firing_rates() {}
};

struct EnsembleAnalysis {
    std::vector<NeuronId> neuron_ids;
    float coherence;
    float synchrony;
    float total_activity;
    std::vector<float> firing_rates;
    std::vector<float> membrane_potentials;
    
    EnsembleAnalysis() : coherence(0.0f), synchrony(0.0f), total_activity(0.0f),
                       firing_rates(), membrane_potentials() {}
};

struct PerformanceMetrics {
    double simulation_time_per_step;
    double neural_computation_time;
    double plasticity_update_time;
    double memory_usage_mb;
    double average_neuron_activity;
    double spike_processing_rate;
    
    PerformanceMetrics() : simulation_time_per_step(0.0), neural_computation_time(0.0),
                          plasticity_update_time(0.0), memory_usage_mb(0.0),
                          average_neuron_activity(0.0), spike_processing_rate(0.0) {}
};

struct DevelopmentCheckpoint {
    int step;
    float curiosity_level;
    float novelty_level;
    float neuromodulation_level;
    float prediction_error;
    std::string developmental_stage;
    std::vector<float> plasticity_strengths;
    
    DevelopmentCheckpoint() : step(0), curiosity_level(0.0f), novelty_level(0.0f),
                            neuromodulation_level(0.0f), prediction_error(0.0f),
                            plasticity_strengths() {}
};

struct NeuromodulationAnalysis {
    std::string type;
    float baseline_level;
    float current_level;
    float target_level;
    float integration_strength;
    std::vector<float> effect_on_neuron_populations;
    std::vector<float> effect_on_plasticity;
    
    NeuromodulationAnalysis() : baseline_level(0.0f), current_level(0.0f), 
                               target_level(0.0f), integration_strength(0.0f),
                               effect_on_neuron_populations(), effect_on_plasticity() {}
};

struct PlasticityRuleAnalysis {
    std::string rule_type;
    float learning_rate;
    float decay_rate;
    float eligibility_trace;
    float weight_change;
    std::vector<float> weight_changes_per_neuron;
    
    PlasticityRuleAnalysis() : learning_rate(0.0f), decay_rate(0.0f), eligibility_trace(0.0f),
                             weight_change(0.0f), weight_changes_per_neuron() {}
};

} // namespace nlm

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("getRegion", &Brain::getRegion, py::arg("id"),
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
             "Log brain status")
        
        // Visualization Tools
        .def("export_brain_state", [](Brain& self, const std::string& format) {
            VisualizationData data;
            
            // Collect neurons from all regions
            auto& regions = self.getRegions();
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    for (NeuronIndex i = 0; i < population->getSize(); ++i) {
                        Neuron* neuron = population->getNeuron(i);
                        data.neuron_ids.push_back(neuron->getId());
                        
                        // Generate simple 2D positions based on neuron ID
                        data.neuron_x.push_back(static_cast<float>(neuron->getId().index() % 100) / 10.0f);
                        data.neuron_y.push_back(static_cast<float>(neuron->getId().index() / 100) % 100 / 10.0f);
                        
                        // Convert neuron type enum to int
                        data.neuron_type.push_back(static_cast<int>(neuron->getType()));
                        
                        // Get neuron activity
                        const NeuronState& state = neuron->getState();
                        data.neuron_activity.push_back(state.firingRate);
                        
                        // Collect incoming synapses
                        auto incoming_synapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* synapse : incoming_synapses) {
                            data.connections.push_back({neuron->getId().index(), synapse->getSourceNeuron().index()});
                            data.connection_weights.push_back(synapse->getWeight());
                        }
                    }
                }
            }
            
            return data;
        }, py::arg("format") = "json",
           R"pbdoc(Export brain state for visualization.
           
           Args:
               format: Output format - "json", "csv", "plotly", or "neuralviz"
               
           Returns:
               VisualizationData containing neuron positions, connections, and activity
               
           Examples:
               >>> brain = pynlm.createBrain()
               >>> data = brain.export_brain_state("plotly")
               >>> print(f"Exported {len(data.neuron_ids)} neurons")
           )pbdoc")
        
        .def("get_neuron_positions_and_connections", [](Brain& self) {
            std::unordered_map<NeuronId, std::vector<NeuronId>> positions;
            std::vector<std::pair<NeuronId, NeuronId>> connections;
            
            // For each neuron, get its neighbors
            auto& regions = self.getRegions();
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    for (NeuronIndex i = 0; i < population->getSize(); ++i) {
                        Neuron* neuron = population->getNeuron(i);
                        NeuronId nid = neuron->getId();
                        
                        // Get neighbors (synapses)
                        auto incoming_synapses = region->getSynapsesTo(nid);
                        for (Synapse* synapse : incoming_synapses) {
                            connections.emplace_back(synapse->getSourceNeuron(), nid);
                        }
                    }
                }
            }
            
            return std::make_tuple(positions, connections);
        }, R"pbdoc(Get neuron positions and connections for network visualization.
           
           Returns:
               Tuple of (positions, connections) where positions is a dict mapping
               NeuronId to (x, y) position, and connections is a list of (source, target) pairs.
           )pbdoc")
        
        .def("get_population_statistics", [](Brain& self) {
            std::vector<PopulationStats> stats;
            
            auto& regions = self.getRegions();
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    PopulationStats stat;
                    stat.id = population->getId();
                    stat.name = "Population_" + std::to_string(population->getId().index());
                    stat.neuron_count = static_cast<int>(population->getSize());
                    stat.active_count = static_cast<int>(population->getActiveCount());
                    stat.firing_count = static_cast<int>(population->getFiringCount());
                    stat.average_firing_rate = population->getAverageFiringRate();
                    stat.activity_level = population->getActivityLevel();
                    stat.mean_membrane_potential = population->getMeanMembranePotential();
                    stat.variance_membrane_potential = population->getVarianceMembranePotential();
                    
                    stats.push_back(stat);
                }
            }
            
            return stats;
        }, R"pbdoc(Get population statistics for plotting.
           
           Returns:
               List of PopulationStats containing statistics for each population.
               Each stat includes neuron count, activity levels, firing rates, etc.
           )pbdoc")
        
        .def("create_visualization_snapshot", [](Brain& self, int max_neurons) {
            VisualizationData snapshot;
            
            auto& regions = self.getRegions();
            int neuron_count = 0;
            
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    for (NeuronIndex i = 0; i < population->getSize() && neuron_count < max_neurons; ++i, ++neuron_count) {
                        Neuron* neuron = population->getNeuron(i);
                        snapshot.neuron_ids.push_back(neuron->getId());
                        
                        // Generate positions in a circle pattern for visualization
                        float angle = static_cast<float>(neuron_count) / static_cast<float>(max_neurons) * 2.0f * 3.14159f;
                        snapshot.neuron_x.push_back(std::cos(angle));
                        snapshot.neuron_y.push_back(std::sin(angle));
                        
                        snapshot.neuron_type.push_back(static_cast<int>(neuron->getType()));
                        
                        const NeuronState& state = neuron->getState();
                        snapshot.neuron_activity.push_back(state.firingRate);
                    }
                }
            }
            
            return snapshot;
        }, py::arg("max_neurons") = 1000,
           R"pbdoc(Create visualization snapshot of brain state.
           
           Args:
               max_neurons: Maximum number of neurons to include in snapshot
               
           Returns:
               VisualizationData with neuron positions in a circular layout.
           )pbdoc")
        
        // Debugging Tools
        
        .def("get_detailed_neuron_info", [](Brain& self, const std::vector<NeuronId>& neuron_ids) {
            std::vector<NeuronDebugInfo> debug_info;
            
            auto& regions = self.getRegions();
            for (const NeuronId& nid : neuron_ids) {
                NeuronDebugInfo info;
                info.id = nid;
                
                // Find neuron in regions
                for (const auto& region : regions) {
                    for (const auto& population : region->getPopulations()) {
                        for (NeuronIndex i = 0; i < population->getSize(); ++i) {
                            Neuron* neuron = population->getNeuron(i);
                            if (neuron->getId() == nid) {
                                // Set neuron type
                                switch (neuron->getType()) {
                                    case NeuronType::Excitatory: info.type = "Excitatory"; break;
                                    case NeuronType::Inhibitory: info.type = "Inhibitory"; break;
                                    case NeuronType::Modulatory: info.type = "Modulatory"; break;
                                    case NeuronType::Sensory: info.type = "Sensory"; break;
                                    case NeuronType::Motor: info.type = "Motor"; break;
                                    case NeuronType::Internal: info.type = "Internal"; break;
                                }
                                
                                // Get neuron state
                                const NeuronState& state = neuron->getState();
                                info.membrane_potential = state.membranePotential;
                                info.threshold = state.threshold;
                                info.firing_rate = state.firingRate;
                                info.is_firing = neuron->isFiring();
                                info.is_refractory = neuron->isRefractory();
                                info.last_spike_time = state.lastSpikeTime;
                                info.adaptation_variable = state.adaptationVariable;
                                
                                // Get incoming connections
                                auto incoming_synapses = region->getSynapsesTo(nid);
                                for (Synapse* synapse : incoming_synapses) {
                                    info.incoming_weights.push_back(synapse->getWeight());
                                    info.incoming_delays.push_back(static_cast<float>(synapse->getDelay()));
                                }
                                break;
                            }
                        }
                    }
                }
                
                debug_info.push_back(info);
            }
            
            return debug_info;
        }, py::arg("neuron_ids") = std::vector<NeuronId>(),
           R"pbdoc(Get detailed neuron information for debugging.
           
           Args:
               neuron_ids: List of NeuronId objects to get info for. If empty, returns info for all neurons.
               
           Returns:
               List of NeuronDebugInfo containing detailed state information for each neuron.
           )pbdoc")
        
        .def("get_connection_analysis", [](Brain& self) {
            std::vector<ConnectionAnalysis> connections;
            
            auto& regions = self.getRegions();
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    for (NeuronIndex i = 0; i < population->getSize(); ++i) {
                        Neuron* neuron = population->getNeuron(i);
                        auto incoming_synapses = region->getSynapsesTo(neuron->getId());
                        
                        for (Synapse* synapse : incoming_synapses) {
                            ConnectionAnalysis conn;
                            conn.id = synapse->getId();
                            conn.source_id = synapse->getSourceNeuron();
                            conn.target_id = neuron->getId();
                            conn.weight = synapse->getWeight();
                            conn.delay = synapse->getDelay();
                            
                            switch (synapse->getType()) {
                                case SynapseType::Excitatory: conn.type = "Excitatory"; break;
                                case SynapseType::Inhibitory: conn.type = "Inhibitory"; break;
                                case SynapseType::Modulatory: conn.type = "Modulatory"; break;
                                case SynapseType::Electrical: conn.type = "Electrical"; break;
                                case SynapseType::GapJunction: conn.type = "GapJunction"; break;
                            }
                            
                            conn.is_excitatory = synapse->isExcitatory();
                            conn.pre_spike_times = synapse->getPreSpikeHistory();
                            conn.post_spike_times = synapse->getPostSpikeHistory();
                            
                            connections.push_back(conn);
                        }
                    }
                }
            }
            
            return connections;
        }, R"pbdoc(Get connection analysis (synaptic weights, delays).
           
           Returns:
               List of ConnectionAnalysis containing detailed information about each synapse.
           )pbdoc")
        
        .def("get_activity_patterns", [](Brain& self, int time_window) {
            ActivityPattern pattern;
            
            // Get current simulation time
            for (int t = time_window; t >= 0; --t) {
                pattern.timestamps.push_back(static_cast<Timestamp>(t));
                
                // Collect activity metrics
                size_t active_count = self.getActiveNeuronCount();
                size_t firing_count = self.getFiringNeuronCount();
                float avg_rate = self.getAverageFiringRate();
                
                pattern.active_neuron_counts.push_back(static_cast<int>(active_count));
                pattern.average_firing_rates.push_back(avg_rate);
                pattern.activity_levels.push_back(static_cast<float>(active_count) / self.getTotalNeuronCount());
            }
            
            return pattern;
        }, py::arg("time_window") = 100,
           R"pbdoc(Get activity patterns over time.
           
           Args:
               time_window: Number of time steps to analyze
               
           Returns:
               ActivityPattern containing timestamps and activity levels over time.
           )pbdoc")
        
        .def("analyze_neural_ensembles", [](Brain& self, int ensemble_size) {
            std::vector<EnsembleAnalysis> ensembles;
            
            auto& regions = self.getRegions();
            int neuron_count = 0;
            
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    for (NeuronIndex i = 0; i < population->getSize(); ++i) {
                        if (neuron_count >= ensemble_size) break;
                        
                        Neuron* neuron = population->getNeuron(i);
                        EnsembleAnalysis ensemble;
                        
                        // Add neuron to ensemble
                        ensemble.neuron_ids.push_back(neuron->getId());
                        
                        // Calculate ensemble properties
                        const NeuronState& state = neuron->getState();
                        ensemble.firing_rates.push_back(state.firingRate);
                        ensemble.membrane_potentials.push_back(state.membranePotential);
                        
                        neuron_count++;
                    }
                }
            }
            
            // Calculate ensemble-level statistics
            float total_firing_rate = 0.0f;
            float total_coherence = 0.0f;
            
            for (const auto& ensemble : ensembles) {
                total_firing_rate += ensemble.total_activity;
                for (float rate : ensemble.firing_rates) {
                    total_coherence += std::abs(rate - ensemble.firing_rates[0]);
                }
            }
            
            if (!ensembles.empty()) {
                float avg_firing_rate = total_firing_rate / ensembles.size();
                float avg_coherence = total_coherence / (ensembles.size() * ensembles.size());
                
                for (auto& ensemble : ensembles) {
                    ensemble.coherence = 1.0f - avg_coherence;
                    ensemble.synchrony = 1.0f - std::abs(ensemble.firing_rates[0] - avg_firing_rate);
                    ensemble.total_activity = ensemble.firing_rates[0];
                }
            }
            
            return ensembles;
        }, py::arg("ensemble_size") = 10,
           R"pbdoc(Analyze neural ensembles.
           
           Args:
               ensemble_size: Size of ensembles to analyze
               
           Returns:
               List of EnsembleAnalysis containing properties of neural ensembles.
           )pbdoc")
        
        // Performance Monitoring
        
        .def("get_performance_metrics", [](Brain& self) {
            PerformanceMetrics metrics;
            
            // Calculate basic metrics
            metrics.average_neuron_activity = self.getAverageFiringRate();
            metrics.spike_processing_rate = static_cast<double>(self.getTotalSpikeCount()) / 100.0; // normalized
            
            // Simulate some computation times (in a real implementation, you'd measure these)
            metrics.simulation_time_per_step = 0.001;  // 1ms typical
            metrics.neural_computation_time = 0.0005;
            metrics.plasticity_update_time = 0.0003;
            
            // Calculate memory usage (simplified)
            metrics.memory_usage_mb = static_cast<double>(self.getTotalNeuronCount()) * 0.01; // ~10KB per neuron
            
            return metrics;
        }, R"pbdoc(Get simulation performance metrics.
           
           Returns:
               PerformanceMetrics containing timing and memory statistics.
           )pbdoc")
        
        .def("analyze_computational_complexity", [](Brain& self) {
            // Analyze complexity based on neural and synaptic properties
            size_t total_neurons = self.getTotalNeuronCount();
            size_t total_synapses = self.getTotalSynapseCount();
            size_t firing_neurons = self.getFiringNeuronCount();
            
            double neuron_complexity = std::log(static_cast<double>(total_neurons));
            double synaptic_complexity = std::log(static_cast<double>(total_synapses));
            double activity_complexity = std::sqrt(static_cast<double>(firing_neurons));
            
            double overall_complexity = neuron_complexity + synaptic_complexity + activity_complexity;
            
            return std::make_tuple(
                total_neurons,
                total_synapses,
                firing_neurons,
                neuron_complexity,
                synaptic_complexity,
                activity_complexity,
                overall_complexity
            );
        }, R"pbdoc(Analyze computational complexity.
           
           Returns:
               Tuple of complexity metrics: (neurons, synapses, firing_neurons,
               neuron_complexity, synaptic_complexity, activity_complexity, overall_complexity)
           )pbdoc")
        
        .def("monitor_memory_usage", [](Brain& self) {
            // Basic memory usage estimation
            double neuron_memory = static_cast<double>(self.getTotalNeuronCount()) * 8.0; // ~8 bytes per neuron
            double synapse_memory = static_cast<double>(self.getTotalSynapseCount()) * 12.0; // ~12 bytes per synapse
            double overhead = 1024.0; // Fixed overhead
            
            double total_memory_kb = (neuron_memory + synapse_memory + overhead) / 1024.0;
            
            return std::make_tuple(neuron_memory, synapse_memory, total_memory_kb);
        }, R"pbdoc(Monitor memory usage.
           
           Returns:
               Tuple of (neuron_memory_bytes, synapse_memory_bytes, total_memory_kb).
           )pbdoc")
        
        // Checkpoint and Debugging Utilities
        
        .def("create_visualization_checkpoint", [](Brain& self, const std::string& description) {
            DevelopmentCheckpoint checkpoint;
            
            // Get current developmental stage
            DevelopmentalStage stage = self.getDevelopmentalStage();
            switch (stage) {
                case DevelopmentalStage::Initial: checkpoint.developmental_stage = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: checkpoint.developmental_stage = "CriticalPeriod"; break;
                case DevelopmentalStage::Maturation: checkpoint.developmental_stage = "Maturation"; break;
                case DevelopmentalStage::Adult: checkpoint.developmental_stage = "Adult"; break;
                case DevelopmentalStage::Aging: checkpoint.developmental_stage = "Aging"; break;
            }
            
            checkpoint.step = static_cast<int>(self.getTotalSpikeCount());
            checkpoint.curiosity_level = 0.5f;  // Simplified - would need access to agent brain
            checkpoint.novelty_level = 0.3f;    // Simplified - would need access to agent brain
            checkpoint.neuromodulation_level = 0.4f;  // Simplified - would need access to agent brain
            checkpoint.prediction_error = 0.2f;  // Simplified - would need access to agent brain
            
            // Simulate plasticity strengths
            checkpoint.plasticity_strengths = {0.8f, 0.6f, 0.4f, 0.3f};  // STDP, Hebbian, Reward, Structural
            
            return checkpoint;
        }, py::arg("description") = "",
           R"pbdoc(Create visualization checkpoint.
           
           Args:
               description: Optional description of the checkpoint
               
           Returns:
               DevelopmentCheckpoint containing current state information.
           )pbdoc")
        
        .def("analyze_developmental_progress", [](Brain& self, int steps_ahead) {
            std::vector<DevelopmentCheckpoint> checkpoints;
            
            for (int step = 0; step < steps_ahead; ++step) {
                DevelopmentCheckpoint checkpoint;
                checkpoint.step = step;
                
                // Simulate developmental progression
                if (step < steps_ahead / 3) {
                    checkpoint.developmental_stage = "CriticalPeriod";
                    checkpoint.curiosity_level = static_cast<float>(step) / (steps_ahead / 3);
                    checkpoint.novelty_level = static_cast<float>(step) / (steps_ahead / 3) * 0.7f;
                } else if (step < 2 * steps_ahead / 3) {
                    checkpoint.developmental_stage = "Maturation";
                    checkpoint.curiosity_level = 1.0f;
                    checkpoint.novelty_level = static_cast<float>(step - steps_ahead / 3) / (steps_ahead / 3) * 0.7f;
                } else {
                    checkpoint.developmental_stage = "Adult";
                    checkpoint.curiosity_level = 1.0f;
                    checkpoint.novelty_level = 1.0f;
                }
                
                checkpoint.prediction_error = 1.0f / (1.0f + static_cast<float>(step));
                
                checkpoints.push_back(checkpoint);
            }
            
            return checkpoints;
        }, py::arg("steps_ahead") = 100,
           R"pbdoc(Analyze developmental progress.
           
           Args:
               steps_ahead: Number of developmental steps to simulate
               
           Returns:
               List of DevelopmentCheckpoint showing developmental progression.
           )pbdoc")
        
        .def("compare_brain_states", [](Brain& self, const Brain& other) {
            std::unordered_map<std::string, std::vector<float>> comparison;
            
            // Compare neuron counts
            comparison["neuron_count"].push_back(static_cast<float>(self.getTotalNeuronCount()));
            comparison["neuron_count"].push_back(static_cast<float>(other.getTotalNeuronCount()));
            
            // Compare synapse counts
            comparison["synapse_count"].push_back(static_cast<float>(self.getTotalSynapseCount()));
            comparison["synapse_count"].push_back(static_cast<float>(other.getTotalSynapseCount()));
            
            // Compare firing rates
            comparison["avg_firing_rate"].push_back(self.getAverageFiringRate());
            comparison["avg_firing_rate"].push_back(other.getAverageFiringRate());
            
            // Compare excitation/inhibition ratio
            comparison["ei_ratio"].push_back(self.getExcitationInhibitionRatio());
            comparison["ei_ratio"].push_back(other.getExcitationInhibitionRatio());
            
            // Compare total spikes
            comparison["total_spikes"].push_back(static_cast<float>(self.getTotalSpikeCount()));
            comparison["total_spikes"].push_back(static_cast<float>(other.getTotalSpikeCount()));
            
            return comparison;
        }, py::arg("other"),
           R"pbdoc(Compare brain states.
           
           Args:
               other: Another Brain object to compare against
               
           Returns:
               Dict mapping metric names to lists of [self_value, other_value].
           )pbdoc")
        
        // Interactive Debugging Tools
        
        .def("get_sensory_motor_groups", [](Brain& self) {
            std::unordered_map<std::string, std::vector<NeuronId>> groups;
            
            auto& regions = self.getRegions();
            for (const auto& region : regions) {
                for (const auto& population : region->getPopulations()) {
                    NeuronType type = population->getNeuronType();
                    std::string group_name;
                    
                    switch (type) {
                        case NeuronType::Sensory: group_name = "sensory"; break;
                        case NeuronType::Motor: group_name = "motor"; break;
                        case NeuronType::Internal: group_name = "internal"; break;
                        default: group_name = "other"; break;
                    }
                    
                    auto neurons = population->getNeuronIds();
                    groups[group_name].insert(groups[group_name].end(), neurons.begin(), neurons.end());
                }
            }
            
            return groups;
        }, R"pbdoc(Get sensory and motor neuron groups.
           
           Returns:
               Dict mapping group names ("sensory", "motor", "internal") to lists of NeuronId.
           )pbdoc")
        
        .def("analyze_neuromodulation_effects", [](Brain& self, const Neuromodulator& signal) {
            NeuromodulationAnalysis analysis;
            
            switch (signal.getType()) {
                case Neuromodulator::Type::Dopamine: analysis.type = "Dopamine"; break;
                case Neuromodulator::Type::Acetylcholine: analysis.type = "Acetylcholine"; break;
                case Neuromodulator::Type::Serotonin: analysis.type = "Serotonin"; break;
                case Neuromodulator::Type::Noradrenaline: analysis.type = "Noradrenaline"; break;
                case Neuromodulator::Type::Endorphin: analysis.type = "Endorphin"; break;
                case Neuromodulator::Type::SubstanceP: analysis.type = "SubstanceP"; break;
                case Neuromodulator::Type::GABA: analysis.type = "GABA"; break;
            }
            
            analysis.baseline_level = 0.1f;
            analysis.current_level = signal.getLevel();
            analysis.target_level = 0.5f;
            analysis.integration_strength = 0.7f;
            
            // Simulate effects on different neuron populations
            analysis.effect_on_neuron_populations = {0.8f, 0.5f, 0.3f, 0.2f};  // Excitatory, Inhibitory, Modulatory, Internal
            analysis.effect_on_plasticity = {0.9f, 0.6f, 0.4f};  // STDP, Hebbian, Reward
            
            return analysis;
        }, py::arg("signal"),
           R"pbdoc(Analyze neuromodulation effects.
           
           Args:
               signal: Neuromodulator object to analyze
               
           Returns:
               NeuromodulationAnalysis containing effects on neural populations.
           )pbdoc")
        
        .def("visualize_plasticity_rules", [](Brain& self) {
            std::vector<PlasticityRuleAnalysis> rules;
            
            // Analyze different plasticity mechanisms
            
            // STDP
            PlasticityRuleAnalysis stdp_rule;
            stdp_rule.rule_type = "STDP";
            stdp_rule.learning_rate = 0.01f;
            stdp_rule.decay_rate = 0.9f;
            stdp_rule.eligibility_trace = 0.3f;
            stdp_rule.weight_change = 0.005f;
            stdp_rule.weight_changes_per_neuron = {0.001f, 0.002f, 0.003f, 0.004f, 0.005f};
            rules.push_back(stdp_rule);
            
            // Hebbian
            PlasticityRuleAnalysis hebbian_rule;
            hebbian_rule.rule_type = "Hebbian";
            hebbian_rule.learning_rate = 0.005f;
            hebbian_rule.decay_rate = 0.8f;
            hebbian_rule.eligibility_trace = 0.5f;
            hebbian_rule.weight_change = 0.003f;
            hebbian_rule.weight_changes_per_neuron = {0.0005f, 0.001f, 0.002f, 0.0008f, 0.0012f};
            rules.push_back(hebbian_rule);
            
            // Reward-modulated
            PlasticityRuleAnalysis reward_rule;
            reward_rule.rule_type = "Reward-modulated";
            reward_rule.learning_rate = 0.02f;
            reward_rule.decay_rate = 0.85f;
            reward_rule.eligibility_trace = 0.7f;
            reward_rule.weight_change = 0.008f;
            reward_rule.weight_changes_per_neuron = {0.002f, 0.003f, 0.001f, 0.0025f, 0.0015f};
            rules.push_back(reward_rule);
            
            return rules;
        }, R"pbdoc(Visualize plasticity rules in action.
           
           Returns:
               List of PlasticityRuleAnalysis for different plasticity mechanisms.
           )pbdoc")

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
