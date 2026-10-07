#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "../src/brain/Brain.hpp"

namespace py = pybind11;
namespace nlm {

// brain_bindings.cpp - Brain class
// Purpose: Defines Python bindings for the central neural simulation brain class

// Register Brain class bindings to the given module
void register_brain(pybind11::module_& m) {

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class for NLM.
        
        Brain is the core computational component of the NLM system, representing
        a complete artificial brain with neurons, synapses, plasticity, learning,
        memory, and developmental capabilities. It implements a closed-loop
        neural system that can receive sensory input, process it internally,
        and generate motor actions to interact with the environment.
        
        The brain operates using leaky integrate-and-fire (LIF) neurons with
        various plasticity rules including STDP, Hebbian learning, and
        neuromodulation. It supports complex behaviors through its integrated
        system of memory, prediction, and developmental processes.
        
        Examples:
            >>> import nlm
            >>> config = nlm.createDefaultConfig()
            >>> config.set("neuron_count", 500)
            >>> config.set("region_count", 4)
            >>> brain = nlm.createBrain(config)
            >>> brain.initialize()
            >>> for step in range(100):
            ...     brain.step(step)
            >>> f"Brain active: {brain.getFiringNeuronCount()} neurons firing"
            'Brain active: 42 neurons firing'
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Initialize Brain with configuration.
             
             Args:
                 config: Configuration object containing brain parameters.
                        Must include settings like neuron_count, region_count,
                        connection_probability, etc.
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> config.set("neuron_count", 100)
                 >>> brain = nlm.createBrain(config)
                 >>> brain.initialize()
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration.
             
             This method sets up the neural architecture, creates neurons and
             synapses according to the configuration, and prepares the brain
             for simulation. Must be called before any simulation steps.
             
             Raises:
                 RuntimeError: If initialization fails due to invalid config
                             or resource constraints
             
             Examples:
                 >>> brain.initialize()
                 >>> brain.getTotalNeuronCount()
                 100
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a single simulation step.
             
             Args:
                 currentStep: Current simulation step number (starts from 0)
             
             This performs one complete iteration of neural processing:
             1. Update neuron states based on incoming currents
             2. Detect and process spikes from neurons
             3. Propagate spikes through synapses
             4. Apply plasticity rules for learning
             5. Update brain-wide statistics
             
             Examples:
                 >>> for step in range(10):
                 ...     brain.step(step)
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             
             Extended version of step() that includes timestamp information
             for time-dependent processes and synchronization.
             
             Examples:
                 >>> for step, time in enumerate([0.0, 0.1, 0.2, 0.3]):
                 ...     brain.step(step, time)
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Receive and process sensory input from the world.
             
             Args:
                 input: Sensory input object containing sensory data.
                       Can be Vision, Audio, or InternalSignals.
             
             The brain processes this sensory information by injecting appropriate
             currents into sensory neurons, which then propagates through the
             neural network to generate responses.
             
             Raises:
                 ValueError: If input is None or invalid type
                 RuntimeError: If brain is not initialized
             
             Examples:
                 >>> vision = nlm.Vision(640, 480, 3)
                 >>> vision.setData([0.5] * 640 * 480 * 3)
                 >>> agent.processSensoryInput(vision)
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current into a specific neuron.
             
             Args:
                 neuron: Neuron identifier to inject current into
                 current: Current value in nanoamps (nA)
             
             Direct current injection allows for precise control over neural
             activity, useful for testing and specific stimulation protocols.
             
             Examples:
                 >>> brain.injectCurrent(nlm.NEURON_ID, 5.0)
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: NeuronType (e.g., NeuronType.Sensory, NeuronType.Excitatory)
                 current: Current value in nanoamps (nA)
             
             This is useful for simulating sensory input or global modulation
             of specific neural populations.
             
             Examples:
                 >>> brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 2.5)
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Generate a motor action based on current neural activity.
             
             Returns:
                 Action: The motor action determined by the brain's current
                        neural state. This represents the agent's behavioral
                        response to the current situation.
             
             The brain analyzes its current neural activity, particularly from
             motor regions, to produce an appropriate action for interacting
             with the environment.
             
             Examples:
                 >>> action = brain.produceAction()
                 >>> action.getType()
                 <ActionType.MoveForward: 0>
                 >>> action.getName()
                 'MoveForward'
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset the brain to its initial state.
             
             This clears all neural activity, resets neuron states to resting,
             and reinitializes the brain's internal state. Useful for starting
             a new simulation episode or testing.
             
             Examples:
                 >>> brain.reset()
                 >>> brain.getTotalSpikeCount()
                 0
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save the current brain state to a file.
             
             Args:
                 filepath: Path where the brain state will be saved
             
             The saved state includes all neural activity, memory contents,
             developmental stage, and configuration. Can be loaded later to
             resume a simulation or study the brain's learned behavior.
             
             Examples:
                 >>> brain.save("my_brain_state.nml")
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from a file.
             
             Args:
                 filepath: Path to the saved brain state file
             
             Restores the brain's neural activity, memory contents, and all
             internal state from a previous save. The brain must be properly
             initialized before loading.
             
             Raises:
                 RuntimeError: If file cannot be read or state is invalid
             
             Examples:
                 >>> brain.load("saved_brain_state.nml")
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region to the brain.
             
             Args:
                 name: Optional name for the new region. If empty, will be
                       assigned a unique identifier.
             
             Returns:
                 RegionId: The unique identifier of the newly created region.
             
             Examples:
                 >>> region_id = brain.addRegion("VisualCortex")
                 >>> region_id.index()
                 1
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a neural region by its identifier.
             
             Args:
                 id: RegionId of the region to retrieve
             
             Returns:
                 NeuralRegion*: Pointer to the requested region. Returns nullptr
                              if the region ID is invalid.
             
             Examples:
                 >>> region = brain.getRegion(region_id)
                 >>> region is not None
                 True
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of neural regions in the brain.
             
             Returns:
                 int: Number of regions
             
             Examples:
                 >>> regions = brain.getRegionCount()
                 >>> regions > 0
                 True
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region identifiers.
             
             Returns:
                 list[RegionId]: List of all region IDs in the brain
             
             Examples:
                 >>> region_ids = brain.getRegionIds()
                 >>> len(region_ids) == brain.getRegionCount()
                 True
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all neural regions.
             
             Returns:
                 list[NeuralRegion*]: List of all region pointers
             
             Examples:
                 >>> regions = brain.getRegions()
                 >>> len(regions) > 0
                 True
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get the total number of neurons in the brain.
             
             Returns:
                 int: Total neuron count across all regions
             
             Examples:
                 >>> neurons = brain.getTotalNeuronCount()
                 >>> neurons == 100
                 True
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get the total number of synapses in the brain.
             
             Returns:
                 int: Total synapse count across all regions
             
             Examples:
                 >>> synapses = brain.getTotalSynapseCount()
                 >>> synapses >= 0
                 True
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get the number of currently active neurons.
             
             Returns:
                 int: Number of active neurons (those with voltage above threshold)
             
             Examples:
                 >>> active = brain.getActiveNeuronCount()
                 >>> active >= 0
                 True
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get the number of neurons currently firing spikes.
             
             Returns:
                 int: Number of neurons that fired in the current timestep
             
             Examples:
                 >>> firing = brain.getFiringNeuronCount()
                 >>> firing >= 0
                 True
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get the average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate in Hz (spikes per second)
             
             Examples:
                 >>> rate = brain.getAverageFiringRate()
                 >>> rate >= 0.0
                 True
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get the excitation/inhibition balance ratio.
             
             Returns:
                 float: Ratio of excitatory to inhibitory influence (higher = more excitation)
             
             Examples:
                 >>> ratio = brain.getExcitationInhibitionRatio()
                 >>> ratio >= 0.0
                 True
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get the total number of spikes generated by the brain.
             
             Returns:
                 int: Cumulative spike count since brain initialization
             
             Examples:
                 >>> spikes = brain.getTotalSpikeCount()
                 >>> spikes >= 0
                 True
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get the current developmental stage of the brain.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             
             Examples:
                 >>> stage = brain.getDevelopmentalStage()
                 >>> stage == nlm.DevelopmentalStage.Adult
                 True
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set the developmental stage of the brain.
             
             Args:
                 stage: New developmental stage
            
            This affects plasticity rates, learning capabilities, and
            neural properties. Useful for controlled experiments and
            studying developmental effects.
            
            Examples:
                >>> brain.setDevelopmentalStage(nlm.DevelopmentalStage.CriticalPeriod)
                >>> brain.getDevelopmentalStage() == nlm.DevelopmentalStage.CriticalPeriod
                True
            )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration object associated with this brain.
             
             Returns:
                 Config: Reference to the brain's configuration
             
             Examples:
                 >>> config = brain.getConfig()
                 >>> config.get("neuron_count")
                 100
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log the current status and statistics of the brain.
             
             This outputs diagnostic information about the brain's current
             state, including neuron counts, firing rates, and key statistics
             useful for debugging and monitoring.
             
             Examples:
                 >>> brain.logStatus()
                 # Outputs status information to console
             )pbdoc");

} // namespace nlm