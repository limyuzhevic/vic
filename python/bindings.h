// Core types and enumerations for NLM Python bindings

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

// Forward declarations for all C++ classes that need Python bindings
class Config;
class Neuron;
class Synapse;
class NeuralPopulation;
class NeuralRegion;
class SpikeSystem;
class Brain;
class AgentBrain;
class NeuronId;
class SynapseId;
class RegionId;
class PopulationId;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class MotorSystem;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;
class PerceptOutput;
class SimpleWorld;
class Environment;
class Observation;
class RandomGenerator;
class SimulationClock;
class Logger;
class Neuromodulator;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Reward;
class PredictionError;
class Novelty;
class Curiosity;
class PlasticityRule;
class STDP;
class Hebbian;
class StructuralPlasticity;
class DevelopmentSystem;
class Synaptogenesis;
class Pruning;
class Maturation;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class AssociativeMemory;
class NeuralPlanner;
class SelfModel;
class SocialLearning;
class ConceptFormation;
class AttentionalSelection;
class PredictionSystem;
class NeuralPrediction;
class VisualizationInterface;
class Experiment;
class ExperimentRunner;
class Metrics;
class AblationSystem;
class Phase3Experiment;
class Phase4Experiment;
class Phase5Experiment;
class Phase6IntegratedExperiment;
class ScalingBenchmark;

// Factory functions
std::shared_ptr<Config> createDefaultConfig();
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
std::shared_ptr<SimpleWorld> createSimpleWorld();
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);

// Utility functions and constants
constexpr NeuronId INVALID_NEURON_ID = NeuronId{};
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId{};
constexpr RegionId INVALID_REGION_ID = RegionId{};
constexpr PopulationId INVALID_POPULATION_ID = PopulationId{};

// Function declarations for binding modules
void bind_core_types(py::module_& m);
void bind_enums(py::module_& m);
void bind_identifier_classes(py::module_& m);
void bind_basic_constants(py::module_& m);

void bind_config_module(py::module_& m);
void bind_neuron_module(py::module_& m);
void bind_synapse_module(py::module_& m);
void bind_plasticity_module(py::module_& m);
void bind_neuromodulation_module(py::module_& m);
void bind_memory_module(py::module_& m);
void bind_cognition_module(py::module_& m);
void bind_development_module(py::module_& m);
void bind_prediction_module(py::module_& m);
void bind_sensory_module(py::module_& m);
void bind_motor_module(py::module_& m);
void bind_environment_module(py::module_& m);
void bind_world_module(py::module_& m);
void bind_agent_module(py::module_& m);
void bind_brain_module(py::module_& m);
void bind_simulation_module(py::module_& m);
void bind_experiments_module(py::module_& m);
void bind_visualization_module(py::module_& m);

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to the complete neural brain architecture,
        including memory systems, neuromodulation, plasticity rules, cognition,
        prediction, and development.
        
        Key features:
        - Full integration of all NLM brain systems
        - Event-driven spiking neural computation
        - Memory systems (working, episodic, semantic, procedural, associative)
        - Neuromodulation (dopamine, acetylcholine, norepinephrine, serotonin)
        - Plasticity rules (STDP, Hebbian, structural plasticity)
        - Cognitive functions (planning, concept formation, attention)
        - Prediction systems
        - Developmental stages and changes
        - Complete brain management and control
        
        See individual class documentation for details.
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Bind all modules
    bind_core_types(m);
    bind_enums(m);
    bind_identifier_classes(m);
    bind_basic_constants(m);
    bind_config_module(m);
    bind_neuron_module(m);
    bind_synapse_module(m);
    bind_plasticity_module(m);
    bind_neuromodulation_module(m);
    bind_memory_module(m);
    bind_cognition_module(m);
    bind_development_module(m);
    bind_prediction_module(m);
    bind_sensory_module(m);
    bind_motor_module(m);
    bind_environment_module(m);
    bind_world_module(m);
    bind_agent_module(m);
    bind_brain_module(m);
    bind_simulation_module(m);
    bind_experiments_module(m);
    bind_visualization_module(m);

    // Factory functions
    m.def("createDefaultConfig", &createDefaultConfig, R"pbdoc(
        Create a default configuration object.
        
        Returns:
            Config: A new configuration object with default settings.
        )pbdoc");

    m.def("createBrain", &createBrain, py::arg("config"), R"pbdoc(
        Create a new brain with the given configuration.
        
        Args:
            config: Configuration object for the brain.
        
        Returns:
            Brain: A new brain instance.
        )pbdoc");

    m.def("createSimpleWorld", &createSimpleWorld, R"pbdoc(
        Create a new simple world environment.
        
        Returns:
            SimpleWorld: A new world instance.
        )pbdoc");

    m.def("createAgentBrain", &createAgentBrain, py::arg("brain"), R"pbdoc(
        Create an agent brain interface for the given brain.
        
        Args:
            brain: The brain to interface with.
        
        Returns:
            AgentBrain: An agent brain interface.
        )pbdoc");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm