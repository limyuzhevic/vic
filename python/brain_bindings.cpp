// Python bindings for NLM brain functionality
// This file contains bindings for the main Brain class and related functionality

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"

namespace py = pybind11;
namespace nlm {

void bindBrain(py::module& m);
void bindNeuron(py::module& m);
void bindSynapse(py::module& m);
void bindNeuralPopulation(py::module& m);
void bindNeuralRegion(py::module& m);

void include_brain_bindings(py::module& m) {
    bindBrain(m);
    bindNeuron(m);
    bindSynapse(m);
    bindNeuralPopulation(m);
    bindNeuralRegion(m);
}

void bindBrain(py::module& m) {
    // Brain class - Main neural simulation brain
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The Brain class implements real spiking neural computation with:
        - LIF neuron dynamics
        - Event-driven spike propagation
        - Memory and cognition integration
        - Neuromodulation
        - Developmental plasticity
        
        The Brain class is the core of the NLM system, coordinating all
        neural processing, learning, and behavior.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Run a few simulation steps
            >>> for i in range(100):
            ...     brain.step(i)
            ...
            >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
            
            >>> # Inject sensory input
            >>> sensory_input = pynlm.Vision(width=8, height=8)
            >>> brain.receiveSensoryInput(sensory_input)
            
        The brain follows this processing loop:
        1. Receive sensory input
        2. Apply neuromodulation (reward, curiosity, etc.)
        3. Perform neural computation (spiking dynamics)
        4. Apply plasticity (learning from experience)
        5. Produce motor output
        6. Update development
        
        Methods:
            __init__(config): Create brain with configuration
            initialize(): Initialize the brain with configuration
            step(currentStep): Perform a simulation step
            step(currentStep, currentTime): Perform a simulation step with timestamp
            receiveSensoryInput(input): Inject sensory input into the brain
            injectCurrent(neuron, current): Inject current into a specific neuron
            injectCurrentToNeurons(type, current): Inject current into all neurons of a specific type
            produceAction(): Produce motor action based on neural activity
            reset(): Reset brain state
            save(filepath): Save brain state to file
            load(filepath): Load brain state from file
            addRegion(name): Add a new neural region
            getRegion(id): Get a region by ID
            getRegionCount(): Get the number of regions
            getRegionIds(): Get all region IDs
            getRegions(): Get all regions
            getTotalNeuronCount(): Get total neuron count
            getTotalSynapseCount(): Get total synapse count
            getActiveNeuronCount(): Get count of active neurons
            getFiringNeuronCount(): Get count of currently firing neurons
            getAverageFiringRate(): Get average firing rate
            getExcitationInhibitionRatio(): Get excitation/inhibition balance ratio
            getTotalSpikeCount(): Get total spike count
            getPendingSpikeEventCount(): Get count of pending spike events
            getDevelopmentalStage(): Get current developmental stage
            getConfig(): Get the configuration
            logStatus(): Log brain status
        
        Attributes:
            total_neuron_count: Total number of neurons
            total_synapse_count: Total number of synapses
            firing_neuron_count: Number of currently firing neurons
            total_spike_count: Total spikes generated
            developmental_stage: Current developmental stage
            excitation_inhibition_ratio: E/I balance ratio
            
        Raises:
            RuntimeError: If brain operations fail
            ConfigurationError: If configuration is invalid
            ValueError: If invalid parameters are provided
            
        Context Managers:
            The Brain class can be used as a context manager:
            
            >>> with pynlm.createBrain(config) as brain:
            ...     brain.initialize()
            ...     brain.step(0)
            ...     # Brain automatically reset on exit
            
        See Also:
            pynlm.createBrain, pynlm.createDefaultConfig
            pynlm.createSimpleWorld, pynlm.createAgentBrain
            Neuron, NeuralRegion, NeuralPopulation
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Simulation step methods
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Sensory input methods
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        
        // Motor output
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Region management
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
        
        // Statistics
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        
        // Development
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        // Configuration
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        
        // Logging and status
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Spike system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // Context manager support
        .def("__enter__", [](Brain& self) {
            return &self;
        }, "Context manager entry - returns self")
        .def("__exit__", [](Brain& self, py::object) {
            self.reset();
        }, "Context manager exit - resets brain")
        
        // Convenience methods
        .def("runForSteps", [](Brain& self, size_t steps, double dt = 0.001) {
            for (SimulationStep step = 0; step < steps; ++step) {
                self.step(step, step * dt);
            }
        }, py::arg("steps"), py::arg("dt") = 0.001,
         "Run simulation for given number of steps")
        
        .def("getStatistics", &Brain::getStatistics,
             "Get comprehensive brain statistics as dictionary",
             py::return_value_policy::copy)
        
        .def("get_brain_stats", &Brain::getBrainStats,
             "Get brain statistics as dictionary (convenience method)",
             py::return_value_policy::copy)
        
        .def("__repr__", [](const Brain& b) {
            return "<Brain: neurons=" + std::to_string(b.getTotalNeuronCount()) +
                   ", synapses=" + std::to_string(b.getTotalSynapseCount()) +
                   ", stage=" + std::to_string(static_cast<int>(b.getDevelopmentalStage())) + ">";
        }, "String representation of the brain")
        
        .def("__len__", &Brain::getTotalNeuronCount,
             "Get total neuron count (supports len())");
}

void bindNeuron(py::module& m) {
    // Neuron class - Individual spiking neuron
    py::class_<Neuron>(m, "Neuron", R"pbdoc(
        Individual spiking neuron in the NLM system.
        
        The Neuron class implements Leaky Integrate-and-Fire (LIF) neuron dynamics,
        simulating the behavior of biological neurons including:
        - Membrane potential integration
        - Action potential generation and propagation
        - Refractory period after firing
        - Synaptic input integration
        
        Neurons are the fundamental building blocks of the NLM brain,
        capable of firing action potentials when their membrane potential
        exceeds a threshold.
        
        Example:
            >>> import pynlm
            >>> neuron = pynlm.Neuron(pynlm.NeuronType.Excitatory)
            >>> neuron.setThreshold(-50.0)
            >>> neuron.setRestingPotential(-70.0)
            >>> neuron.injectCurrent(60.0)
            >>> 
            >>> for i in range(10):
            ...     neuron.step()
            ...     if neuron.isFiring():
            ...         print(f"Spike at step {i}")
            
        Attributes:
            id: Unique neuron identifier
            type: Neuron type (excitatory, inhibitory, etc.)
            membrane_potential: Current membrane potential
            threshold: Firing threshold (mV)
            resting_potential: Resting potential (mV)
            firing_state: Current firing state
            
        Methods:
            __init__(type): Create neuron with specified type
            initialize(): Initialize neuron with default parameters
            step(): Perform one simulation step
            isFiring(): Check if neuron is currently firing
            isActive(): Check if neuron is active
            isRefractory(): Check if neuron is in refractory period
            isInhibited(): Check if neuron is inhibited
            getMembranePotential(): Get current membrane potential
            setMembranePotential(voltage): Set membrane potential
            getThreshold(): Get firing threshold
            setThreshold(threshold): Set firing threshold
            getRestingPotential(): Get resting potential
            setRestingPotential(voltage): Set resting potential
            getMembraneTimeConstant(): Get membrane time constant
            setMembraneTimeConstant(tau): Set membrane time constant
            getRefractoryPeriod(): Get refractory period duration
            setRefractoryPeriod(period): Set refractory period duration
            injectCurrent(current): Inject current into neuron
            reset(): Reset neuron to resting state
            getSpikingHistory(): Get spiking history
        
        Properties:
            membrane_potential: Get/set membrane potential
            threshold: Get/set firing threshold
            resting_potential: Get/set resting potential
            
        Raises:
            ValueError: If invalid parameters are provided
            
        See Also:
            Brain, Synapse, NeuralPopulation, NeuronType, FiringState
    )pbdoc")
        .def(py::init<NeuronType>(), py::arg("type"),
             "Create neuron with specified type")
        .def("initialize", &Neuron::initialize,
             "Initialize neuron with default parameters")
        .def("step", &Neuron::step,
             "Perform one simulation step")
        .def("isFiring", &Neuron::isFiring,
             "Check if neuron is currently firing")
        .def("isActive", &Neuron::isActive,
             "Check if neuron is active")
        .def("isRefractory", &Neuron::isRefractory,
             "Check if neuron is in refractory period")
        .def("isInhibited", &Neuron::isInhibited,
             "Check if neuron is inhibited")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get current membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential,
             py::arg("voltage"),
             "Set membrane potential")
        .def("getThreshold", &Neuron::getThreshold,
             "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold,
             py::arg("threshold"),
             "Set firing threshold")
        .def("getRestingPotential", &Neuron::getRestingPotential,
             "Get resting potential")
        .def("setRestingPotential", &Neuron::setRestingPotential,
             py::arg("voltage"),
             "Set resting potential")
        .def("getMembraneTimeConstant", &Neuron::getMembraneTimeConstant,
             "Get membrane time constant")
        .def("setMembraneTimeConstant", &Neuron::setMembraneTimeConstant,
             py::arg("tau"),
             "Set membrane time constant")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod,
             "Get refractory period duration")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod,
             py::arg("period"),
             "Set refractory period duration")
        .def("injectCurrent", &Neuron::injectCurrent,
             py::arg("current"),
             "Inject current into neuron")
        .def("reset", &Neuron::reset,
             "Reset neuron to resting state")
        .def("getSpikingHistory", &Neuron::getSpikingHistory,
             "Get spiking history")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: type=" + std::to_string(static_cast<int>(n.getType())) +
                   ", V=" + std::to_string(n.getMembranePotential()) +
                   ", thresh=" + std::to_string(n.getThreshold()) + ">";
        }, "String representation of the neuron");
}

void bindSynapse(py::module& m) {
    // Synapse class - Connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(
        Connection between two neurons in the NLM system.
        
        The Synapse class represents synaptic connections between neurons,
        implementing synaptic transmission and plasticity:
        - Synaptic weight determines connection strength
        - Delay determines transmission time
        - Plasticity rules determine weight changes
        - Transmission conducts action potentials
        
        Synapses are the communication channels between neurons,
        enabling complex neural networks and information processing.
        
        Example:
            >>> import pynlm
            >>> pre_neuron = pynlm.Neuron(pynlm.NeuronType.Excitatory)
            >>> post_neuron = pynlm.Neuron(pynlm.NeuronType.Inhibitory)
            >>> synapse = pynlm.Synapse(pre_neuron, post_neuron)
            >>> synapse.setWeight(0.5)
            >>> synapse.setDelay(2)
            >>> synapse.enablePlasticity(True, True, False)
            
            >>> # Strengthen connection through correlated activity
            >>> pre_neuron.injectCurrent(60.0)
            >>> pre_neuron.step()
            >>> 
            >>> post_neuron.injectCurrent(60.0)
            >>> post_neuron.step()
            >>> 
            >>> print(f"Weight after correlated activity: {synapse.getWeight()}")
        
        Attributes:
            id: Unique synapse identifier
            source: Presynaptic neuron
            target: Postsynaptic neuron
            weight: Synaptic weight (connection strength)
            delay: Transmission delay (time steps)
            type: Synapse type (excitatory, inhibitory, etc.)
            
        Methods:
            __init__(pre, post): Create synapse between two neurons
            getSource(): Get presynaptic neuron
            getTarget(): Get postsynaptic neuron
            getWeight(): Get synaptic weight
            setWeight(weight): Set synaptic weight
            getDelay(): Get transmission delay
            setDelay(delay): Set transmission delay
            getType(): Get synapse type
            setType(type): Set synapse type
            isPlastic(): Check if synapse is plastic
            enablePlasticity(hebbian, stdp, structural): Enable plasticity rules
            disablePlasticity(): Disable all plasticity
            transmit(): Transmit action potential
            reset(): Reset synapse state
            getTransmissionHistory(): Get transmission history
        
        Properties:
            weight: Get/set synaptic weight
            delay: Get/set transmission delay
            type: Get/set synapse type
            
        Raises:
            ValueError: If invalid parameters are provided
            RuntimeError: If synapse operations fail
            
        See Also:
            Neuron, Brain, NeuralPopulation, SynapseType, STDP, Hebbian
    )pbdoc")
        .def(py::init<Neuron*, Neuron*>(), py::arg("source"), py::arg("target"),
             "Create synapse between two neurons")
        .def("getSource", &Synapse::getSource,
             "Get presynaptic neuron")
        .def("getTarget", &Synapse::getTarget,
             "Get postsynaptic neuron")
        .def("getWeight", &Synapse::getWeight,
             "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight,
             py::arg("weight"),
             "Set synaptic weight")
        .def("getDelay", &Synapse::getDelay,
             "Get transmission delay")
        .def("setDelay", &Synapse::setDelay,
             py::arg("delay"),
             "Set transmission delay")
        .def("getType", &Synapse::getType,
             "Get synapse type")
        .def("setType", &Synapse::setType,
             py::arg("type"),
             "Set synapse type")
        .def("isPlastic", &Synapse::isPlastic,
             "Check if synapse is plastic")
        .def("enablePlasticity", &Synapse::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("structural"),
             "Enable plasticity rules")
        .def("disablePlasticity", &Synapse::disablePlasticity,
             "Disable all plasticity")
        .def("transmit", &Synapse::transmit,
             "Transmit action potential")
        .def("reset", &Synapse::reset,
             "Reset synapse state")
        .def("getTransmissionHistory", &Synapse::getTransmissionHistory,
             "Get transmission history")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: source=" + std::to_string(s.getSource()->getId()) +
                   ", target=" + std::to_string(s.getTarget()->getId()) +
                   ", weight=" + std::to_string(s.getWeight()) + ">";
        }, "String representation of the synapse");
}

void bindNeuralPopulation(py::module& m) {
    // NeuralPopulation class - Group of neurons
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(
        Group of neurons with shared properties and functions.
        
        The NeuralPopulation class represents a group of neurons that
        share common properties and perform related functions. Populations
        are used to organize neurons by type, function, or location:
        - Motor populations: Control actions
        - Sensory populations: Process sensory input
        - Association populations: Integrate information
        - Interneuron populations: Connect different regions
        
        Populations enable efficient management of neuron groups
        and facilitate coordinated activity patterns.
        
        Example:
            >>> import pynlm
            >>> motor_pop = pynlm.NeuralPopulation(
            ...     pynlm.PopulationId(1),
            ...     "motor",
            ...     pynlm.NeuronType.Motor,
            ...     10
            ... )
            >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
            >>> brain.initialize()
            >>> # Add neurons to population
            >>> for i in range(10):
            ...     brain.getRegion(brain.getRegionIds()[0]).addNeuron(motor_pop)
            
            >>> # Coordinate population activity
            >>> for i in range(100):
            ...     motor_pop.injectCurrent(30.0)
            ...     brain.step(i)
            
            >>> firing_count = motor_pop.getFiringCount()
            >>> print(f"Population firing rate: {firing_count}")
        
        Attributes:
            id: Unique population identifier
            name: Population name
            type: Neuron type for this population
            size: Number of neurons in population
            
        Methods:
            __init__(id, name, type, size): Create neural population
            getId(): Get population identifier
            getName(): Get population name
            setName(name): Set population name
            getType(): Get neuron type
            getSize(): Get population size
            setSize(size): Set population size
            getNeurons(): Get all neurons in population
            addNeuron(neuron): Add neuron to population
            removeNeuron(neuron): Remove neuron from population
            injectCurrent(current): Inject current into all neurons
            getFiringCount(): Get number of firing neurons
            getAverageFiringRate(): Get average firing rate
            reset(): Reset all neurons in population
            getStatistics(): Get population statistics
        
        Properties:
            neurons: Get/set neurons in population
            firing_count: Get number of firing neurons
            average_firing_rate: Get average firing rate
            
        Raises:
            ValueError: If invalid parameters are provided
            RuntimeError: If population operations fail
            
        See Also:
            Brain, NeuralRegion, Neuron, NeuralPopulation, PopulationId
    )pbdoc")
        .def(py::init<PopulationId, const std::string&, NeuronType, size_t>(),
             py::arg("id"), py::arg("name"), py::arg("type"), py::arg("size"),
             "Create neural population")
        .def("getId", &NeuralPopulation::getId,
             "Get population identifier")
        .def("getName", &NeuralPopulation::getName,
             "Get population name")
        .def("setName", &NeuralPopulation::setName, py::arg("name"),
             "Set population name")
        .def("getType", &NeuralPopulation::getType,
             "Get neuron type")
        .def("getSize", &NeuralPopulation::getSize,
             "Get population size")
        .def("setSize", &NeuralPopulation::setSize, py::arg("size"),
             "Set population size")
        .def("getNeurons", &NeuralPopulation::getNeurons,
             "Get all neurons in population")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"),
             "Add neuron to population")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuron"),
             "Remove neuron from population")
        .def("injectCurrent", &NeuralPopulation::injectCurrent,
             py::arg("current"),
             "Inject current into all neurons")
        .def("getFiringCount", &NeuralPopulation::getFiringCount,
             "Get number of firing neurons")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate,
             "Get average firing rate")
        .def("reset", &NeuralPopulation::reset,
             "Reset all neurons in population")
        .def("getStatistics", &NeuralPopulation::getStatistics,
             "Get population statistics")
        .def("__repr__", [](const NeuralPopulation& pop) {
            return "<NeuralPopulation: " + pop.getName() + 
                   " (type=" + std::to_string(static_cast<int>(pop.getType())) +
                   ", size=" + std::to_string(pop.getSize()) + ")>";
        }, "String representation of the neural population");
}

void bindNeuralRegion(py::module& m) {
    // NeuralRegion class - Container for neurons and synapses
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(
        Container for neurons and synapses in the NLM system.
        
        The NeuralRegion class represents a logical container for neurons
        and synapses, organizing neural tissue into functional units:
        - Cortical areas: Different processing regions
        - Subcortical nuclei: subcortical structures
        - Neural circuits: Specific functional pathways
        - Working memory modules: Temporary storage units
        
        Regions enable spatial and functional organization of neural activity,
        facilitating complex information processing and integration.
        
        Example:
            >>> import pynlm
            >>> cortex = pynlm.NeuralRegion(
            ...     pynlm.RegionId(1),
            ...     "cortex",
            ...     pynlm.NeuronType.Excitatory,
            ...     pynlm.NeuronType.Inhibitory
            ... )
            >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
            >>> brain.initialize()
            >>> 
            >>> # Add region to brain
            >>> region_id = brain.addRegion("cortex")
            >>> region = brain.getRegion(region_id)
            
            >>> # Populate region with neurons
            >>> neurons = []
            >>> for i in range(50):
            ...     neuron = pynlm.Neuron(pynlm.NeuronType.Excitatory)
            ...     region.addNeuron(neuron)
            ...     neurons.append(neuron)
            
            >>> # Process information through region
            >>> for i in range(100):
            ...     for neuron in neurons:
            ...         if i % 10 == 0:
            ...             neuron.injectCurrent(40.0)
            ...     brain.step(i)
            
            >>> total_spikes = brain.getTotalSpikeCount()
            >>> print(f"Region activity: {total_spikes} spikes")
        
        Attributes:
            id: Unique region identifier
            name: Region name
            neuron_type: Primary neuron type
            inhibitory_type: Inhibitory neuron type
            size: Number of neurons
            
        Methods:
            __init__(id, name, neuron_type, inhibitory_type): Create neural region
            getId(): Get region identifier
            getName(): Get region name
            setName(name): Set region name
            getNeuronType(): Get primary neuron type
            getInhibitoryType(): Get inhibitory neuron type
            getSize(): Get region size
            setSize(size): Set region size
            getAllNeurons(): Get all neurons in region
            addNeuron(neuron): Add neuron to region
            removeNeuron(neuron): Remove neuron from region
            getNeuronsByType(type): Get neurons by type
            getSynapses(): Get all synapses in region
            addSynapse(synapse): Add synapse to region
            removeSynapse(synapse): Remove synapse from region
            getSynapsesByType(type): Get synapses by type
            getConnectivity(): Get connectivity statistics
            injectCurrentToNeurons(type, current): Inject current to neurons by type
            reset(): Reset all neurons and synapses
            getStatistics(): Get region statistics
        
        Properties:
            neurons: Get all neurons in region
            synapses: Get all synapses in region
            size: Get/set region size
            
        Raises:
            ValueError: If invalid parameters are provided
            RuntimeError: If region operations fail
            
        See Also:
            Brain, NeuralPopulation, Neuron, Synapse, RegionId
    )pbdoc")
        .def(py::init<RegionId, const std::string&, NeuronType, NeuronType>(),
             py::arg("id"), py::arg("name"), py::arg("neuron_type"), py::arg("inhibitory_type"),
             "Create neural region")
        .def("getId", &NeuralRegion::getId,
             "Get region identifier")
        .def("getName", &NeuralRegion::getName,
             "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"),
             "Set region name")
        .def("getNeuronType", &NeuralRegion::getNeuronType,
             "Get primary neuron type")
        .def("getInhibitoryType", &NeuralRegion::getInhibitoryType,
             "Get inhibitory neuron type")
        .def("getSize", &NeuralRegion::getSize,
             "Get region size")
        .def("setSize", &NeuralRegion::setSize, py::arg("size"),
             "Set region size")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons,
             "Get all neurons in region")
        .def("addNeuron", &NeuralRegion::addNeuron, py::arg("neuron"),
             "Add neuron to region")
        .def("removeNeuron", &NeuralRegion::removeNeuron, py::arg("neuron"),
             "Remove neuron from region")
        .def("getNeuronsByType", &NeuralRegion::getNeuronsByType,
             py::arg("type"),
             "Get neurons by type")
        .def("getSynapses", &NeuralRegion::getSynapses,
             "Get all synapses in region")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("synapse"),
             "Add synapse to region")
        .def("removeSynapse", &NeuralRegion::removeSynapse, py::arg("synapse"),
             "Remove synapse from region")
        .def("getSynapsesByType", &NeuralRegion::getSynapsesByType,
             py::arg("type"),
             "Get synapses by type")
        .def("getConnectivity", &NeuralRegion::getConnectivity,
             "Get connectivity statistics")
        .def("injectCurrentToNeurons", &NeuralRegion::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to neurons by type")
        .def("reset", &NeuralRegion::reset,
             "Reset all neurons and synapses")
        .def("getStatistics", &NeuralRegion::getStatistics,
             "Get region statistics")
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: " + region.getName() + 
                   " (size=" + std::to_string(region.getSize()) + ")>";
        }, "String representation of the neural region");
}
