// python/bindings_brain.cpp

void bindBrain(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
    
    The core neural simulation brain implementing spiking neural networks
    with event-driven dynamics, plasticity, memory, and neuromodulation.
    
    This is the main computational engine of NLM, implementing:
    - Leaky Integrate-and-Fire (LIF) neuron dynamics
    - Event-driven spike propagation with synaptic delays
    - STDP and Hebbian plasticity rules
    - Structural plasticity (synaptogenesis/pruning)
    - Working memory, episodic memory, and associative memory
    - Neuromodulation (dopamine, curiosity, novelty, prediction error)
    - Prediction systems and neural planning
    - Concept formation and attentional selection
    - Development and aging processes
    
    Common Uses:
        - Neural network simulation
        - Learning and adaptation
        - Cognitive modeling
        - Brain-computer interfaces
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation
        >>> for step in range(1000):
        ...     brain.step(step)
        ...     if step % 100 == 0:
        ...         print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
        >>> 
        >>> # Get statistics
        >>> brain.getTotalNeuronCount()
        1000
        >>> brain.getAverageFiringRate()
        5.2
    
    Attributes:
        config: Brain configuration
        neuron_count: Total number of neurons
        synapse_count: Total number of synapses
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Create brain with configuration)
             
             Args:
                 config: Brain configuration (use createDefaultConfig() for simple setup)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
             ")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize brain with configuration)
             
             Returns:
                 bool: True if initialization succeeded, False otherwise
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> success = brain.initialize()
                 >>> success
                 True
             ")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step)
             
             Args:
                 currentStep: Current simulation step number
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.step(0)
                 >>> brain.step(100)
             ")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp)
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time (seconds)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.step(0, 0.0)
                 >>> brain.step(100, 0.1)
             ")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(Inject sensory input into the brain)
             
             Args:
                 input: Sensory input data (Vision, Audio, InternalSignals)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> vision = pynlm.Vision(8, 8, 3)
                 >>> brain.receiveSensoryInput(vision)
             ")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current directly into a specific neuron)
             
             Args:
                 neuron: Neuron ID to inject current into
                 current: Current amount (positive = excitation, negative = inhibition)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> neuron_id = pynlm.NeuronId(42)
                 >>> brain.injectCurrent(neuron_id, 10.0)
             ")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type)
             
             Args:
                 type: Neuron type (Sensory, Motor, Excitatory, etc.)
                 current: Current amount to inject
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
             ")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity)
             
             Returns:
                 Action: Motor action produced by the brain
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.step(0)
                 >>> action = brain.produceAction()
                 >>> action.getType()
                 'move_forward'
             ")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state)
             
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.step(10)
                 >>> brain.reset()
                 >>> brain.getTotalSpikeCount()
                 0
             ")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file)
             
             Args:
                 filepath: Path to save brain state
                 
             Returns:
                 bool: True if save succeeded, False otherwise
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.save("brain_state.bin")
             ")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file)
             
             Args:
                 filepath: Path to load brain state from
                 
             Returns:
                 bool: True if load succeeded, False otherwise
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.load("brain_state.bin")
             ")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region)
             
             Args:
                 name: Optional name for the region
                 
             Returns:
                 RegionId: ID of the newly created region
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> region_id = brain.addRegion("cortex")
                 >>> brain.getRegionCount()
                 2
             ")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID)
             
             Args:
                 id: Region ID
                 
             Returns:
                 NeuralRegion: Region object (or None if not found)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> region_id = brain.addRegion("cortex")
                 >>> region = brain.getRegion(region_id)
             ")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions)
             
             Returns:
                 int: Number of regions
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getRegionCount()
                 1
             ")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs)
             
             Returns:
                 list: List of all region IDs
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> region_ids = brain.getRegionIds()
                 >>> len(region_ids)
                 1
             ")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions)
             
             Returns:
                 list: List of all region objects
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> regions = brain.getRegions()
                 >>> len(regions)
                 1
             ")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions)
             
             Returns:
                 int: Total number of neurons
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getTotalNeuronCount()
                 1000
             ")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions)
             
             Returns:
                 int: Total number of synapses
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getTotalSynapseCount()
                 10000
             ")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons)
             
             Returns:
                 int: Number of active neurons
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getActiveNeuronCount()
                 50
             ")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons)
             
             Returns:
                 int: Number of firing neurons
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getFiringNeuronCount()
                 10
             ")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons)
             
             Returns:
                 float: Average firing rate (spikes per second)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> rate = brain.getAverageFiringRate()
                 >>> rate >= 0.0
                 True
             ")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio)
             
             Returns:
                 float: E/I ratio (higher = more excitation)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> ei_ratio = brain.getExcitationInhibitionRatio()
                 >>> ei_ratio > 0.0
                 True
             ")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count)
             
             Returns:
                 int: Total number of spikes generated
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getTotalSpikeCount()
                 0
             ")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(Get count of pending spike events)
             
             Returns:
                 int: Number of pending spike events
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.getPendingSpikeEventCount()
                 0
             ")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)
             
             Returns:
                 str: Developmental stage name
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> stage = brain.getDevelopmentalStage()
                 >>> stage
                 'adult'
             ")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage)
             
             Args:
                 stage: Developmental stage
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
             ")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration)
             
             Returns:
                 Config: Brain configuration
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> retrieved_config = brain.getConfig()
                 >>> retrieved_config.getOr("brain.neuron_count", 0)
                 1000
             ")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status and statistics)
             
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> brain.initialize()
                 >>> brain.logStatus()
                 Brain Status:
                   Neurons: 1000
                   Synapses: 10000
                   Active: 50
                   Firing: 10
                   ...
             ")
        .def("getMemoryStats", [](const Brain& brain) {
            // This would require accessing memory systems
            return "Memory stats not yet implemented in Python bindings";
        }, R"pbdoc(Get brain memory statistics)
        
        Returns:
            str: Memory statistics string
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> stats = brain.getMemoryStats()
        ")
        .def("getPredictionStats", [](const Brain& brain) {
            return "Prediction stats not yet implemented in Python bindings";
        }, R"pbdoc(Get brain prediction statistics)
        
        Returns:
            str: Prediction statistics string
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> stats = brain.getPredictionStats()
        ")
        .def("getNeuromodulationStats", [](const Brain& brain) {
            return "Neuromodulation stats not yet implemented in Python bindings";
        }, R"pbdoc(Get brain neuromodulation statistics)
        
        Returns:
            str: Neuromodulation statistics string
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> stats = brain.getNeuromodulationStats()
        ")
        .def("getNetworkStats", [](Brain& brain) {
            return "Network statistics not yet implemented in Python bindings";
        }, R"pbdoc(Get comprehensive network statistics)
        
        Returns:
            str: Network statistics string
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> stats = brain.getNetworkStats()
        ")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(b.getTotalSynapseCount()) + 
                   " synapses>";
        });
}