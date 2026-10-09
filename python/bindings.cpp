#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

// Include core headers
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include new advanced headers
#include "../src/advanced/DNNLayer.hpp"
#include "../src/memory/MemoryPool.hpp"
#include "../src/error/NLMErrors.hpp"
#include "../src/error/ErrorHandler.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function for error handling
void setup_error_handling() {
    ErrorHandler::setErrorHandler([](const std::string& error) {
        py::print("NLM Error: " + error);
    });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python API
        -----------------------
        Advanced neural network bindings for NLM (Neural Learning Machine).
        Provides deep learning capabilities, memory management, and error handling.
    )pbdoc";

    // Initialize error handling
    setup_error_handling();

    // Register exception types
    py::register_exception<nlm::NLMException>(m, "NLMException");
    py::register_exception<nlm::SimulationError>(m, "SimulationError");
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::MemoryError>(m, "MemoryError");
    py::register_exception<nlm::InvalidOperationError>(m, "InvalidOperationError");

    // Original type bindings
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

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MOVE_FORWARD", ActionType::MOVE_FORWARD)
        .value("MOVE_BACKWARD", ActionType::MOVE_BACKWARD)
        .value("TURN_LEFT", ActionType::TURN_LEFT)
        .value("TURN_RIGHT", ActionType::TURN_RIGHT)
        .value("STAY", ActionType::STAY)
        .value("GRAB", ActionType::GRAB)
        .value("RELEASE", ActionType::RELEASE)
        .value("NONE", ActionType::NONE)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("AGENT", WorldObjectType::AGENT)
        .value("FOOD", WorldObjectType::FOOD)
        .value("WALL", WorldObjectType::WALL)
        .value("WATER", WorldObjectType::WATER)
        .value("OBSTACLE", WorldObjectType::OBSTACLE)
        .export_values();

    // === ENHANCED DNN LAYER BINDINGS ===
    py::class_<DNNLayer>(m, "DNNLayer", R"pbdoc(Deep Neural Network layer with advanced features)pbdoc")
        .def(py::init<size_t, size_t, NeuronType, float>(), 
             py::arg("input_size"), py::arg("output_size"), 
             py::arg("activation") = NeuronType::Excitatory,
             py::arg("dropout_rate") = 0.0f,
             "Create DNN layer with specified parameters")
        .def("forward", &DNNLayer::forward, py::arg("input"), 
             "Forward pass through DNN layer")
        .def("set_weights", &DNNLayer::set_weights, py::arg("weights"),
             "Set layer weights (flattened)")
        .def("get_weights", &DNNLayer::get_weights,
             "Get layer weights (flattened)")
        .def("set_biases", &DNNLayer::set_biases, py::arg("biases"),
             "Set layer biases")
        .def("get_biases", &DNNLayer::get_biases,
             "Get layer biases")
        .def("set_training_mode", &DNNLayer::set_training_mode, py::arg("training"),
             "Set training mode (affects dropout)")
        .def("is_training", &DNNLayer::is_training,
             "Check if layer is in training mode")
        .def_property_readonly("input_size", &DNNLayer::get_input_size,
                              "Get input size")
        .def_property_readonly("output_size", &DNNLayer::get_output_size,
                              "Get output size")
        .def_property_readonly("dropout_rate", &DNNLayer::get_dropout_rate,
                              "Get dropout rate")
        .def("get_architecture_summary", &DNNLayer::get_architecture_summary,
             "Get layer architecture summary");

    // === MEMORY MANAGEMENT BINDINGS ===
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Advanced memory pool for neural data)pbdoc")
        .def(py::init<size_t>(), py::arg("capacity_bytes"),
             "Create memory pool with specified capacity")
        .def("allocate", &MemoryPool::allocate, py::arg("size"),
             "Allocate memory from pool")
        .def("deallocate", &MemoryPool::deallocate, py::arg("ptr"), py::arg("size"),
             "Deallocate memory back to pool")
        .def("get_used_memory", &MemoryPool::get_used_memory,
             "Get used memory in bytes")
        .def("get_free_memory", &MemoryPool::get_free_memory,
             "Get free memory in bytes")
        .def("is_full", &MemoryPool::is_full,
             "Check if memory pool is full")
        .def("clear", &MemoryPool::clear,
             "Clear all allocated memory from pool")
        .def("allocate_vector", [](MemoryPool& pool, size_t size) {
            return std::vector<float*>(1, static_cast<float*>(pool.allocate(size * sizeof(float))));
        }, py::arg("size"), "Allocate float vector memory")
        .def("deallocate_vector", [](MemoryPool& pool, std::vector<float*> ptrs, size_t size_per_ptr) {
            for (auto ptr : ptrs) {
                pool.deallocate(ptr, size_per_ptr * sizeof(float));
            }
        }, py::arg("ptrs"), py::arg("size_per_ptr"), "Deallocate float vector memory");

    // === ERROR HANDLING BINDINGS ===
    m.def("set_error_handler", &ErrorHandler::setErrorHandler,
          py::arg("handler"), "Set global error handler callback");
    m.def("get_last_error", &ErrorHandler::getLastError,
          "Get last error message");
    m.def("clear_error", &ErrorHandler::clearError,
          "Clear last error");

    // Error-specific helper functions
    m.def("create_memory_error", [](const std::string& msg) {
        throw MemoryError(msg);
    }, py::arg("message"), "Create memory error");

    m.def("create_simulation_error", [](const std::string& msg) {
        throw SimulationError(msg);
    }, py::arg("message"), "Create simulation error");

    m.def("create_configuration_error", [](const std::string& msg) {
        throw ConfigurationError(msg);
    }, py::arg("message"), "Create configuration error");

    // === ADVANCED CONFIGURATION BINDINGS ===
    m.def("configure_dnn_network", [](std::shared_ptr<Brain> brain,
                                     size_t input_dim, size_t hidden_dim,
                                     size_t num_layers, const std::string& activation) {
        // This would call an internal method to configure DNN parameters
        return brain->configure_dnn(input_dim, hidden_dim, num_layers, activation);
    }, py::arg("brain"), py::arg("input_dim"), py::arg("hidden_dim"),
       py::arg("num_layers"), py::arg("activation"),
       "Configure brain for DNN processing");

    m.def("configure_rnn_network", [](std::shared_ptr<Brain> brain,
                                     size_t input_dim, size_t hidden_dim,
                                     size_t num_layers, const std::string& rnn_type) {
        return brain->configure_rnn(input_dim, hidden_dim, num_layers, rnn_type);
    }, py::arg("brain"), py::arg("input_dim"), py::arg("hidden_dim"),
       py::arg("num_layers"), py::arg("rnn_type"),
       "Configure brain for RNN processing");

    m.def("configure_cnn_network", [](std::shared_ptr<Brain> brain,
                                     size_t in_channels, size_t out_channels,
                                     const std::vector<size_t>& kernel_sizes,
                                     const std::vector<size_t>& strides) {
        return brain->configure_cnn(in_channels, out_channels, kernel_sizes, strides);
    }, py::arg("brain"), py::arg("in_channels"), py::arg("out_channels"),
       py::arg("kernel_sizes"), py::arg("strides"),
       "Configure brain for CNN processing");

    // === PERFORMANCE MONITORING ===
    m.def("get_performance_stats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->get_total_neuron_count();
        stats["total_synapses"] = brain->get_total_synapse_count();
        stats["active_neurons"] = brain->get_active_neuron_count();
        stats["firing_neurons"] = brain->get_firing_neuron_count();
        stats["total_spikes"] = brain->get_total_spike_count();
        stats["average_firing_rate"] = brain->get_average_firing_rate();
        stats["excitement_inhibition_ratio"] = brain->get_excitation_inhibition_ratio();
        return stats;
    }, py::arg("brain"), "Get comprehensive performance statistics");

    m.def("get_memory_usage", [](const std::shared_ptr<MemoryPool>& pool) {
        py::dict usage;
        usage["total_capacity"] = pool->get_capacity();
        usage["used_memory"] = pool->get_used_memory();
        usage["free_memory"] = pool->get_free_memory();
        usage["usage_percentage"] = (double)pool->get_used_memory() / pool->get_capacity() * 100.0;
        return usage;
    }, py::arg("pool"), "Get memory pool usage statistics");

    // === ADVANCED UTILITY FUNCTIONS ===
    m.def("create_brain_with_dnn", [](std::shared_ptr<Config> config,
                                     size_t dnn_input_size, size_t dnn_hidden_size,
                                     size_t dnn_num_layers) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            brain->configure_dnn(dnn_input_size, dnn_hidden_size, dnn_num_layers);
            return brain;
        }
        throw SimulationError("Failed to initialize brain with DNN configuration");
    }, py::arg("config"), py::arg("dnn_input_size"), py::arg("dnn_hidden_size"),
       py::arg("dnn_num_layers"), "Create brain with DNN configuration");

    m.def("save_checkpoint", [](std::shared_ptr<Brain> brain,
                               const std::string& filename,
                               const std::shared_ptr<MemoryPool>& memory_pool) {
        return brain->save_to_file(filename);
    }, py::arg("brain"), py::arg("filename"), py::arg("memory_pool"),
       "Save brain state with memory pool management");

    m.def("load_checkpoint", [](std::shared_ptr<Brain>& brain,
                               const std::string& filename,
                               const std::shared_ptr<MemoryPool>& memory_pool) {
        return brain->load_from_file(filename);
    }, py::arg("brain"), py::arg("filename"), py::arg("memory_pool"),
       "Load brain state with memory pool management");

    // === ORIGINAL FUNCTIONALITY (UNCHANGED) ===

    // Keep all original bindings from the base implementation
    // (Brain, AgentBrain, SimpleWorld, etc.)

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
             "Log brain status");

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

// Helper function for accessing Brain member functions that need to be exposed
void setup_brain_bindings(py::module_& m) {
    // This would include any additional Brain bindings needed
}
