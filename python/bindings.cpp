#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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
namespace nlm {

// Custom exception classes for Python bindings
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ResourceError : public std::runtime_error {
public:
    explicit ResourceError(const std::string& msg) : std::runtime_error(msg) {}
};

// Example usage:
    // try:
    //     config = pynlm.createBrainFromFile("config.json")
    // except pynlm.ConfigError as e:
    //     print(f"Configuration error: {e}")
    // except pynlm.ValidationError as e:
    //     print(f"Validation error: {e}")
    // except pynlm.ResourceError as e:
    //     print(f"Resource error: {e}")

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::register_exception<ConfigError>(m, "ConfigError",
        "Exception raised for configuration errors");
    py::register_exception<ValidationError>(m, "ValidationError",
        "Exception raised for validation errors");
    py::register_exception<SimulationError>(m, "SimulationError",
        "Exception raised for simulation errors");
    py::register_exception<ResourceError>(m, "ResourceError",
        "Exception raised for resource-related errors");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with comprehensive documentation.
        
        NeuronId represents a unique identifier for neurons in the NLM system.
        Each neuron in the brain has a unique NeuronId that can be used for
        targeted operations like injecting current or accessing specific neurons.
        
        ## Key Features:
        - **Unique identification**: Each neuron has a distinct ID
        - **Index operations**: Convert ID to zero-based index
        - **Comparison operators**: Support for equality and inequality
        - **Hash support**: Can be used as dictionary keys
        - **String representation**: Human-readable format
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create neuron IDs
        neuron1_id = pynlm.NeuronId()
        neuron2_id = pynlm.NeuronId(42)
        
        # Get index for array operations
        index = neuron1_id.index()
        print(f"Neuron index: {index}")
        
        # Create a dictionary mapping IDs to values
        neuron_values = {}
        for i in range(10):
            neuron_id = pynlm.NeuronId(i)
            neuron_values[neuron_id] = {"type": "excitatory", "strength": 0.5}
        
        # Check equality
        if neuron1_id == neuron2_id:
            print("Neurons are the same")
        
        # Use in collection operations
        all_ids = [pynlm.NeuronId(i) for i in range(100)]
        if any(nid.value == 42 for nid in all_ids):
            print("Found neuron with value 42")
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a NeuronId with the specified value. Each neuron ID is unique.")
        .def_readwrite("value", &NeuronId::value,
             "The internal integer value of the NeuronId")
        .def("index", &NeuronId::index,
             "Get the zero-based index of this NeuronId (value)")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronIds are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Return string representation of the NeuronId");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synaptic connection with comprehensive documentation.
        
        SynapseId represents a unique identifier for synapses in the NLM system.
        Each synaptic connection between neurons has a unique SynapseId that can be used
        for targeted operations like modifying synaptic weights or analyzing connectivity.
        
        ## Key Features:
        - **Unique identification**: Each synapse has a distinct ID
        - **Index operations**: Convert ID to zero-based index for array access
        - **Comparison operators**: Support for equality and inequality
        - **Hash support**: Can be used as dictionary keys for synapse data storage
        - **String representation**: Human-readable format for debugging
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create synapse IDs
        synapse1_id = pynlm.SynapseId()
        synapse2_id = pynlm.SynapseId(123)
        
        # Get index for accessing synapse properties
        index = synapse1_id.index()
        print(f"Synapse array index: {index}")
        
        # Build connectivity map
        connections = {}
        for i in range(50):
            pre_neuron_id = pynlm.NeuronId(i % 100)  # Source neuron
            post_neuron_id = pynlm.NeuronId(i % 100 + 100)  # Target neuron
            synapse_id = pynlm.SynapseId(i)
            connections[synapse_id] = {
                "pre_neuron": pre_neuron_id,
                "post_neuron": post_neuron_id,
                "weight": 0.5,
                "delay": 0.1
            }
        
        # Find specific connections
        target_synapse = pynlm.SynapseId(42)
        if target_synapse in connections:
            conn = connections[target_synapse]
            print(f"Found connection from neuron {conn['pre_neuron'].value} to {conn['post_neuron'].value}")
        
        # Use in neural network analysis
        all_synapses = [pynlm.SynapseId(i) for i in range(1000)]
        high_degree_synapses = [sid for sid in all_synapses if sid.value > 500]
        print(f"Found {len(high_degree_synapses)} high-degree synapses")
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a SynapseId with the specified value. Each synapse ID is unique.")
        .def_readwrite("value", &SynapseId::value,
             "The internal integer value of the SynapseId")
        .def("index", &SynapseId::index,
             "Get the zero-based index of this SynapseId (value)")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseIds are not equal")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Return string representation of the SynapseId");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region with comprehensive documentation.
        
        RegionId represents a unique identifier for brain regions in the NLM system.
        Neural regions group neurons with similar properties and functions (e.g., cortex,
        hippocampus, amygdala). Each region has a unique RegionId used for
        region-specific operations and analysis.
        
        ## Key Features:
        - **Unique identification**: Each region has a distinct ID
        - **Index operations**: Convert ID to zero-based index for array access
        - **Comparison operators**: Support for equality and inequality
        - **Hash support**: Can be used as dictionary keys for region data
        - **String representation**: Human-readable format for debugging
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create region IDs
        cortex_id = pynlm.RegionId()
        hippocampus_id = pynlm.RegionId(2)
        
        # Get region index for array operations
        index = cortex_id.index()
        print(f"Region index: {index}")
        
        # Build region-specific data structures
        brain_regions = {}
        for i in range(10):
            region_id = pynlm.RegionId(i)
            brain_regions[region_id] = {
                "name": f"Region_{i}",
                "neuron_count": 1000 + i * 100,
                "connection_density": 0.1 + i * 0.01,
                "function": "unknown"
            }
        
        # Analyze region properties
        for region_id, properties in brain_regions.items():
            if properties["neuron_count"] > 1500:
                print(f"Large region {region_id}: {properties['neuron_count']} neurons")
        
        # Find specific regions
        target_region = pynlm.RegionId(3)
        if target_region in brain_regions:
            print(f"Found region: {brain_regions[target_region]['name']}")
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a RegionId with the specified value. Each region ID is unique.")
        .def_readwrite("value", &RegionId::value,
             "The internal integer value of the RegionId")
        .def("index", &RegionId::index,
             "Get the zero-based index of this RegionId (value)")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionIds are not equal")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "Return string representation of the RegionId");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with comprehensive documentation.
        
        PopulationId represents a unique identifier for neuron populations in the NLM system.
        Neuron populations are groups of neurons with similar properties (e.g., excitatory,
        inhibitory, sensory, motor). Each population has a unique PopulationId used for
        population-specific operations and analysis.
        
        ## Key Features:
        - **Unique identification**: Each population has a distinct ID
        - **Index operations**: Convert ID to zero-based index for array access
        - **Comparison operators**: Support for equality and inequality
        - **String representation**: Human-readable format for debugging
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create population IDs
        excitatory_pop = pynlm.PopulationId()
        inhibitory_pop = pynlm.PopulationId(1)
        sensory_pop = pynlm.PopulationId(2)
        
        # Get population index for array operations
        index = excitatory_pop.index()
        print(f"Population index: {index}")
        
        # Build population-specific data structures
        neuron_populations = {}
        population_types = {
            "excitatory": 0,
            "inhibitory": 1,
            "sensory": 2,
            "motor": 3,
            "internal": 4
        }
        
        for name, pop_type in population_types.items():
            pop_id = pynlm.PopulationId(pop_type)
            neuron_populations[pop_id] = {
                "name": name,
                "neuron_count": 1000 if name != "internal" else 100,
                "firing_rate": 0.1 if name == "sensory" else 0.05,
                "connection_probability": 0.05 if name == "motor" else 0.1
            }
        
        # Analyze population properties
        for pop_id, properties in neuron_populations.items():
            if properties["neuron_count"] > 500:
                print(f"Large population {pop_id.name}: {properties['neuron_count']} neurons")
        
        # Find specific populations
        target_pop = pynlm.PopulationId(2)
        if target_pop in neuron_populations:
            print(f"Found sensory population with {neuron_populations[target_pop]['neuron_count']} neurons")
        ```
    )pbdoc")
        "PopulationId", "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a PopulationId with the specified value. Each population ID is unique.")
        .def_readwrite("value", &PopulationId::value,
             "The internal integer value of the PopulationId")
        .def("index", &PopulationId::index,
             "Get the zero-based index of this PopulationId (value)")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationIds are not equal");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration with comprehensive documentation.
        
        NeuronType defines the different types of neurons in the NLM system.
        Each neuron type has specific properties, functions, and behaviors that
        determine how it processes and transmits information in the neural network.
        
        ## Neuron Types and Their Functions:
        
        - **Excitatory**: Neurons that increase the firing probability of other neurons
          - Release glutamate
          - Primary driver of neural activity
          - Most common neuron type in cortex
          - Examples: Pyramidal cells, most cortical neurons
        
        - **Inhibitory**: Neurons that decrease the firing probability of other neurons
          - Release GABA
          - Critical for balancing excitation/inhibition
          - Prevents runaway excitation
          - Examples: Fast-spiking interneurons
        
        - **Modulatory**: Neurons that affect multiple other neurons through diffuse projections
          - Release neuromodulators like dopamine, serotonin
          - Regulate global brain state
          - Affect plasticity and learning
          - Examples: brainstem neuromodulatory nuclei
        
        - **Sensory**: Neurons that receive external information
          - Process sensory inputs (vision, touch, etc.)
          - Convert environmental data to neural signals
          - Examples: Retinal ganglion cells, somatosensory neurons
        
        - **Motor**: Neurons that control action execution
          - Generate motor commands
          - Interface with world through muscles/ actuators
          - Examples: Motor cortex neurons, spinal motor neurons
        
        - **Internal**: Neurons that represent internal state
          - Encode internal states (hunger, pain, etc.)
          - Monitor bodily condition
          - Examples: Interoceptive neurons, homeostatic neurons
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create a diverse brain with different neuron types
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        # Manually set neuron types for different populations
        # This would typically be done during brain initialization
        brain = pynlm.createBrain(config)
        
        # Simulate different neuron types
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Internal
        ]
        
        # Create neurons of different types
        for neuron_type in neuron_types:
            print(f"Created {neuron_type} neuron")
            
            # Use neuron type for classification and processing
            if neuron_type == pynlm.NeuronType.Excitatory:
                print("  This neuron will drive activity in connected neurons")
            elif neuron_type == pynlm.NeuronType.Inhibitory:
                print("  This neuron will suppress activity in connected neurons")
            elif neuron_type == pynlm.NeuronType.Sensory:
                print("  This neuron processes external sensory inputs")
            elif neuron_type == pynlm.NeuronType.Motor:
                print("  This neuron will generate motor commands")
            elif neuron_type == pynlm.NeuronType.Modulatory:
                print("  This neuron will modulate global brain state")
            elif neuron_type == pynlm.NeuronType.Internal:
                print("  This neuron represents internal body state")
        
        # Filter neurons by type
        def is_excitatory(neuron_type):
            return neuron_type == pynlm.NeuronType.Excitatory
            
        excitatory_count = sum(1 for nt in neuron_types if is_excitatory(nt))
        print(f"Total excitatory neurons: {excitatory_count}")
        ```
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration with comprehensive documentation.
        
        SynapseType defines the different types of synaptic connections in the NLM system.
        Each synapse type has specific mechanisms, properties, and effects on neural
        communication and learning. Understanding synapse types is crucial for
        implementing appropriate plasticity rules and understanding network dynamics.
        
        ## Synapse Types and Their Properties:
        
        - **Excitatory**: Synaptic connections that increase postsynaptic neuron firing
          - Release excitatory neurotransmitters (glutamate)
          - Depolarize postsynaptic membrane
          - Most common synapse type in cortex
          - Mediates most neural computation
        
        - **Inhibitory**: Synaptic connections that decrease postsynaptic neuron firing
          - Release inhibitory neurotransmitters (GABA)
          - Hyperpolarize postsynaptic membrane
          - Critical for network stability and timing
          - Examples: Perisomatic inhibition, dendrite inhibition
        
        - **Modulatory**: Synaptic connections that affect multiple neurons diffusely
          - Release neuromodulators (dopamine, serotonin, acetylcholine)
          - Affect global brain state and plasticity
          - Long-range projections
          - Examples: Neuromodulatory pathways from brainstem
        
        - **Electrical**: Direct electrical coupling between neurons
          - Gap junctions allow ion flow
          - Faster than chemical synapses
          - Bidirectional communication
          - Examples: Electrical synapses in cardiac tissue, some neural circuits
        
        - **GapJunction**: Specific type of electrical synapse
          - Connexon channels connect neuron membranes
          - Allows small molecules and ions to pass
          - Regulates network synchronization
          - Examples: Interneurons in epilepsy models
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create and analyze different synapse types
        synapse_types = [
            pynlm.SynapseType.Excitatory,
            pynlm.SynapseType.Inhibitory,
            pynlm.SynapseType.Modulatory,
            pynlm.SynapseType.Electrical,
            pynlm.SynapseType.GapJunction
        ]
        
        # Build a connectivity graph with different synapse types
        neural_network = {}
        
        for synapse_type in synapse_types:
            if synapse_type == pynlm.SynapseType.Excitatory:
                neural_network[synapse_type] = {
                    "strength_range": (0.1, 1.0),
                    "plasticity_rules": ["STDP", "Hebbian"],
                    "function": "drive neural activity",
                    "examples": ["Cortical pyramidal connections"]
                }
            elif synapse_type == pynlm.SynapseType.Inhibitory:
                neural_network[synapse_type] = {
                    "strength_range": (0.0, 0.8),
                    "plasticity_rules": ["STDP", "GABAergic plasticity"],
                    "function": "maintain stability and timing",
                    "examples": ["Interneuron to pyramidal connections"]
                }
            elif synapse_type == pynlm.SynapseType.Modulatory:
                neural_network[synapse_type] = {
                    "strength_range": (0.05, 0.3),
                    "plasticity_rules": ["Dopamine modulation"],
                    "function": "global state regulation",
                    "examples": ["Dopamine pathways"]
                }
            elif synapse_type == pynlm.SynapseType.Electrical:
                neural_network[synapse_type] = {
                    "strength_range": (0.0, 1.0),
                    "plasticity_rules": ["Electrical synapse remodeling"],
                    "function": "fast synchronization",
                    "examples": ["Interneurons, cardiac tissue"]
                }
            elif synapse_type == pynlm.SynapseType.GapJunction:
                neural_network[synapse_type] = {
                    "strength_range": (0.0, 0.5),
                    "plasticity_rules": ["Connexon expression control"],
                    "function": "coordinated activity",
                    "examples": ["Neural synchronization structures"]
                }
        
        # Analyze network properties
        total_synapses = sum(len(data["examples"]) for data in neural_network.values())
        print(f"Total synapse examples in network: {total_synapses}")
        
        # Classify synapses by function
        functional_categories = {}
        for synapse_type, data in neural_network.items():
            if "stability" in data["function"]:
                functional_categories.setdefault("stability", []).append(synapse_type)
            elif "computation" in data["function"]:
                functional_categories.setdefault("computation", []).append(synapse_type)
            elif "regulation" in data["function"]:
                functional_categories.setdefault("regulation", []).append(synapse_type)
        
        for category, types in functional_categories.items():
            print(f"{category.capitalize()} synapses: {[str(t) for t in types]}")
        
        # Create synapses for specific network components
        def create_synapse_for_type(synapse_type, target_neuron_type):
            synapse_config = {
                "type": synapse_type,
                "target_neuron": target_neuron_type,
                "weight": 0.5,
                "delay": 0.1,
                "plasticity_enabled": True
            }
            
            # Adjust parameters based on synapse type
            if synapse_type == pynlm.SynapseType.Modulatory:
                synapse_config["plasticity_enabled"] = False
                synapse_config["weight_range"] = (0.1, 0.5)
            elif synapse_type == pynlm.SynapseType.Inhibitory:
                synapse_config["weight_range"] = (0.0, 0.8)
            
            return synapse_config
        
        # Example: Create excitatory and inhibitory synapses
        excitatory_synapse = create_synapse_for_type(
            pynlm.SynapseType.Excitatory,
            pynlm.NeuronType.Excitatory
        )
        inhibitory_synapse = create_synapse_for_type(
            pynlm.SynapseType.Inhibitory,
            pynlm.NeuronType.Inhibitory
        )
        
        print(f"Excitatory synapse config: {excitatory_synapse}")
        print(f"Inhibitory synapse config: {inhibitory_synapse}")
        ```
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration with comprehensive documentation.
        
        DevelopmentalStage defines the different phases of neural development in the NLM system.
        These stages represent the progression from initial brain formation through maturation
        to adult functionality, with each stage having distinct plasticity rules and behaviors.
        This framework models biological brain development and its impact on learning capabilities.
        
        ## Developmental Stages and Their Characteristics:
        
        - **Initial** (Stage 1): Foundation and structure formation
          - High plasticity rate
          - Rapid synaptogenesis
          - Basic circuit assembly
          - Learning from sensory experience
          - Critical period for sensory systems
        
        - **CriticalPeriod** (Stage 2): Sensitive learning windows
          - Peak plasticity for specific functions
          - Strong Hebbian learning
          - Experience-dependent refinement
          - Critical for language and skill acquisition
          - Example: Visual system development in mammals
        
        - **Maturation** (Stage 3): Fine-tuning and optimization
          - Moderate plasticity rate
          - Refinement of established circuits
          - Energy efficiency optimization
          - Skill consolidation
          - Competitive learning processes
          - Example: Adolescent brain development
        
        - **Adult** (Stage 4): Stable functionality
          - Low baseline plasticity
          - Learning through reinforcement
          - Maintenance and repair
          - Efficient information processing
          - Long-term memory consolidation
          - Example: Adult cognitive maintenance
        
        - **Aging** (Stage 5): Decline and adaptation
          - Further reduced plasticity
          - Compensatory mechanisms
          - Knowledge retention despite reduced learning speed
          - Increased reliance on established patterns
          - Example: Cognitive changes in elderly
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create a brain that develops over time
        config = pynlm.createDefaultConfig()
        config.set("development.max_stage", pynlm.DevelopmentalStage.Aging)
        brain = pynlm.createBrain(config)
        
        # Simulate development across different stages
        developmental_stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
        
        # Track development and learning efficiency
        development_tracker = {}
        
        for stage in developmental_stages:
            stage_info = {
                "name": str(stage),
                "learning_efficiency": calculate_efficiency(stage),
                "plasticity_rate": get_plasticity_rate(stage),
                "key_skills": get_key_skills(stage),
                "critical_windows": get_critical_windows(stage)
            }
            development_tracker[stage] = stage_info
            
            print(f"Development Stage: {stage}")
            print(f"  Learning Efficiency: {stage_info['learning_efficiency']:.2f}")
            print(f"  Plasticity Rate: {stage_info['plasticity_rate']:.3f}")
            print(f"  Key Skills: {', '.join(stage_info['key_skills'])}")
        
        # Analyze stage-specific learning capabilities
        def is_critical_learning_stage(stage):
            return stage in [
                pynlm.DevelopmentalStage.CriticalPeriod,
                pynlm.DevelopmentalStage.Maturation
            ]
        
        critical_stages = [s for s in developmental_stages if is_critical_learning_stage(s)]
        print(f"Critical learning stages: {[str(s) for s in critical_stages]}")
        
        # Optimize learning strategies by developmental stage
        def get_optimal_learning_strategy(stage):
            if stage == pynlm.DevelopmentalStage.Initial:
                return {
                    "method": "exploratory learning",
                    "repetition": "high",
                    "feedback": "immediate",
                    "focus": "basic pattern recognition"
                }
            elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
                return {
                    "method": "specialized training",
                    "repetition": "moderate",
                    "feedback": "structured",
                    "focus": "skill-specific refinement"
                }
            elif stage == pynlm.DevelopmentalStage.Maturation:
                return {
                    "method": "refined practice",
                    "repetition": "low",
                    "feedback": "comparative",
                    "focus": "efficiency optimization"
                }
            elif stage == pynlm.DevelopmentalStage.Adult:
                return {
                    "method": "reinforcement learning",
                    "repetition": "maintenance",
                    "feedback": "reward-based",
                    "focus": "performance enhancement"
                }
            else:  # Aging
                return {
                    "method": "consolidation learning",
                    "repetition": "review",
                    "feedback": "memory-based",
                    "focus": "knowledge application"
                }
        
        # Demonstrate stage-specific learning strategies
        for stage in developmental_stages:
            strategy = get_optimal_learning_strategy(stage)
            print(f"\n{stage} Optimal Learning Strategy:")
            for key, value in strategy.items():
                print(f"  {key.replace('_', ' ').title()}: {value}")
        
        # Create age-appropriate training programs
        def create_development_program(stage):
            program = {
                "stage": stage,
                "duration": get_stage_duration(stage),
                "intensity": get_stage_intensity(stage),
                "objectives": get_stage_objectives(stage)
            }
            
            return program
        
        # Generate development programs for each stage
        development_programs = {}
        for stage in developmental_stages:
            program = create_development_program(stage)
            development_programs[stage] = program
            print(f"\n{stage} Development Program:")
            print(f"  Duration: {program['duration']} hours")
            print(f"  Intensity: {program['intensity']}")
            print(f"  Objectives: {', '.join(program['objectives'])}")
        
        # Example helper functions (these would be defined elsewhere)
        def calculate_efficiency(stage):
            efficiencies = {
                pynlm.DevelopmentalStage.Initial: 0.3,
                pynlm.DevelopmentalStage.CriticalPeriod: 0.8,
                pynlm.DevelopmentalStage.Maturation: 0.7,
                pynlm.DevelopmentalStage.Adult: 0.5,
                pynlm.DevelopmentalStage.Aging: 0.3
            }
            return efficiencies.get(stage, 0.5)
        
        def get_plasticity_rate(stage):
            rates = {
                pynlm.DevelopmentalStage.Initial: 1.0,
                pynlm.DevelopmentalStage.CriticalPeriod: 0.8,
                pynlm.DevelopmentalStage.Maturation: 0.4,
                pynlm.DevelopmentalStage.Adult: 0.2,
                pynlm.DevelopmentalStage.Aging: 0.1
            }
            return rates.get(stage, 0.2)
        
        def get_key_skills(stage):
            skills = {
                pynlm.DevelopmentalStage.Initial: ["pattern recognition", "basic associations"],
                pynlm.DevelopmentalStage.CriticalPeriod: ["language", "fine motor skills"],
                pynlm.DevelopmentalStage.Maturation: ["complex reasoning", "social cognition"],
                pynlm.DevelopmentalStage.Adult: ["expertise", "problem solving"],
                pynlm.DevelopmentalStage.Aging: ["wisdom", "pattern completion"]
            }
            return skills.get(stage, [])
        
        def get_critical_windows(stage):
            windows = {
                pynlm.DevelopmentalStage.Initial: [" sensory exposure"],
                pynlm.DevelopmentalStage.CriticalPeriod: ["language acquisition", "motor skill learning"],
                pynlm.DevelopmentalStage.Maturation: ["abstract reasoning"],
                pynlm.DevelopmentalStage.Adult: [],
                pynlm.DevelopmentalStage.Aging: []
            }
            return windows.get(stage, [])
        
        def get_stage_duration(stage):
            durations = {
                pynlm.DevelopmentalStage.Initial: 3.0,  # months
                pynlm.DevelopmentalStage.CriticalPeriod: 6.0,
                pynlm.DevelopmentalStage.Maturation: 5.0,
                pynlm.DevelopmentalStage.Adult: "lifetime",
                pynlm.DevelopmentalStage.Aging: "lifetime"
            }
            return durations.get(stage, "lifetime")
        
        def get_stage_intensity(stage):
            intensities = {
                pynlm.DevelopmentalStage.Initial: "high",
                pynlm.DevelopmentalStage.CriticalPeriod: "very high",
                pynlm.DevelopmentalStage.Maturation: "moderate",
                pynlm.DevelopmentalStage.Adult: "low",
                pynlm.DevelopmentalStage.Aging: "very low"
            }
            return intensities.get(stage, "low")
        
        def get_stage_objectives(stage):
            objectives = {
                pynlm.DevelopmentalStage.Initial: ["survival skills", "basic navigation"],
                pynlm.DevelopmentalStage.CriticalPeriod: ["language mastery", "coordination"],
                pynlm.DevelopmentalStage.Maturation: ["creativity", "social competence"],
                pynlm.DevelopmentalStage.Adult: ["productivity", "reproduction"],
                pynlm.DevelopmentalStage.Aging: ["legacy building", "wisdom transmission"]
            }
            return objectives.get(stage, ["survival"])
        ```
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration with comprehensive documentation.
        
        FiringState defines the different functional states of neurons in the NLM system.
        These states represent the dynamic activity patterns that neurons exhibit during
        neural computation, learning, and information processing. Understanding firing states
        is crucial for analyzing neural network behavior and implementing appropriate control mechanisms.
        
        ## Firing States and Their Characteristics:
        
        - **Resting**: Neuron's baseline, inactive state
          - Membrane potential at resting level (-70mV)
          - No ion channel activity
          - Ready to receive inputs
          - Lowest energy consumption
          - Example: Most neurons between spikes
        
        - **Active**: Neuron is generating action potentials
          - Membrane potential above threshold
          - Rapid depolarization and repolarization
          - High firing rates possible
          - Maximum information transmission
          - Example: Neuron actively communicating with other neurons
        
        - **Refractory**: Post-spike recovery period
          - Absolute refractory: Cannot fire again
          - Relative refractory: Can fire with stronger input
          - Ion channel recovery and resetting
          - Prevents excessive firing rates
          - Example: 2-5ms after an action potential
        
        - **Inhibited**: Neuron is suppressed by inhibitory inputs
          - Membrane potential below threshold
          - Hyperpolarization by inhibitory inputs
          - Reduced firing probability
          - Network regulation mechanism
          - Example: Interneuron suppression of pyramidal cells
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Monitor neuron firing states during network activity
        neuron_states = [
            pynlm.FiringState.Resting,
            pynlm.FiringState.Active,
            pynlm.FiringState.Refractory,
            pynlm.FiringState.Inhibited
        ]
        
        # Analyze firing state dynamics in a neural network
        neural_dynamics = {}
        
        for firing_state in neuron_states:
            state_info = {
                "name": str(firing_state),
                "energy_consumption": get_energy_consumption(firing_state),
                "information_transmission": get_information_transmission(firing_state),
                "recovery_time": get_recovery_time(firing_state),
                "network_role": get_network_role(firing_state)
            }
            neural_dynamics[firing_state] = state_info
            
            print(f"Firing State: {firing_state}")
            print(f"  Energy Consumption: {state_info['energy_consumption']:.2f} units")
            print(f"  Information Transmission: {state_info['information_transmission']:.2f}")
            print(f"  Recovery Time: {state_info['recovery_time']:.3f} seconds")
            print(f"  Network Role: {state_info['network_role']}")
        
        # Analyze state transition patterns
        def analyze_state_transitions(firing_states):
            transitions = []
            
            for i in range(len(firing_states) - 1):
                current_state = firing_states[i]
                next_state = firing_states[i + 1]
                
                # Determine transition type
                if current_state == pynlm.FiringState.Resting and next_state == pynlm.FiringState.Active:
                    transition_type = "arousal"
                elif current_state == pynlm.FiringState.Active and next_state == pynlm.FiringState.Refractory:
                    transition_type = "spike propagation"
                elif current_state == pynlm.FiringState.Refractory and next_state == pynlm.FiringState.Resting:
                    transition_type = "recovery"
                elif current_state == pynlm.FiringState.Inhibited and next_state == pynlm.FiringState.Active:
                    transition_type = "disinhibition"
                else:
                    transition_type = "unknown"
                
                transitions.append({
                    "from": current_state,
                    "to": next_state,
                    "type": transition_type,
                    "probability": get_transition_probability(current_state, next_state)
                })
            
            return transitions
        
        # Simulate neural state transitions
        simulation_states = [
            pynlm.FiringState.Resting,
            pynlm.FiringState.Active,
            pynlm.FiringState.Refractory,
            pynlm.FiringState.Inhibited,
            pynlm.FiringState.Resting
        ]
        
        transitions = analyze_state_transitions(simulation_states)
        
        print("\nState Transitions Analysis:")
        for i, transition in enumerate(transitions):
            print(f"  Transition {i+1}: {transition['from']} → {transition['to']}")
            print(f"    Type: {transition['type']}")
            print(f"    Probability: {transition['probability']:.3f}")
        
        # Create state-specific control strategies
        def create_state_control_strategy(firing_state):
            if firing_state == pynlm.FiringState.Resting:
                return {
                    "approach": "stimulate",
                    "method": "sensory input",
                    "intensity": "moderate",
                    "targets": ["excitatory synapses"],
                    "duration": "brief"
                }
            elif firing_state == pynlm.FiringState.Active:
                return {
                    "approach": "maintain",
                    "method": "reinforce",
                    "intensity": "sustained",
                    "targets": ["excitatory connections"],
                    "duration": "continuous"
                }
            elif firing_state == pynlm.FiringState.Refractory:
                return {
                    "approach": "allow recovery",
                    "method": "rest",
                    "intensity": "minimal",
                    "targets": ["all connections"],
                    "duration": "recovery period"
                }
            else:  # Inhibited
                return {
                    "approach": "reduce inhibition",
                    "method": "disinhibition",
                    "intensity": "targeted",
                    "targets": ["inhibitory inputs"],
                    "duration": "temporary"
                }
        
        # Demonstrate state-specific control strategies
        for firing_state in neuron_states:
            strategy = create_state_control_strategy(firing_state)
            print(f"\n{state} Control Strategy:")
            for key, value in strategy.items():
                print(f"  {key.replace('_', ' ').title()}: {value}")
        
        # Analyze network efficiency based on firing states
        def calculate_network_efficiency(firing_states):
            total_efficiency = 0.0
            
            for state in firing_states:
                state_efficiency = calculate_state_efficiency(state)
                total_efficiency += state_efficiency
            
            return total_efficiency / len(firing_states)
        
        # Simulate different network states and calculate efficiency
        resting_network = [pynlm.FiringState.Resting] * 100
        active_network = [pynlm.FiringState.Active] * 50 + [pynlm.FiringState.Resting] * 50
        mixed_network = [
            pynlm.FiringState.Resting, pynlm.FiringState.Active,
            pynlm.FiringState.Refractory, pynlm.FiringState.Inhibited
        ] * 25
        
        resting_efficiency = calculate_network_efficiency(resting_network)
        active_efficiency = calculate_network_efficiency(active_network)
        mixed_efficiency = calculate_network_efficiency(mixed_network)
        
        print("\nNetwork Efficiency Analysis:")
        print(f"  Resting-only Network: {resting_efficiency:.3f}")
        print(f"  Active-dominant Network: {active_efficiency:.3f}")
        print(f"  Mixed Network: {mixed_efficiency:.3f}")
        
        # Example helper functions (these would be defined elsewhere)
        def get_energy_consumption(firing_state):
            consumption = {
                pynlm.FiringState.Resting: 1.0,
                pynlm.FiringState.Active: 10.0,
                pynlm.FiringState.Refractory: 2.0,
                pynlm.FiringState.Inhibited: 1.5
            }
            return consumption.get(firing_state, 1.0)
        
        def get_information_transmission(firing_state):
            transmission = {
                pynlm.FiringState.Resting: 0.0,
                pynlm.FiringState.Active: 1.0,
                pynlm.FiringState.Refractory: 0.0,
                pynlm.FiringState.Inhibited: 0.2
            }
            return transmission.get(firing_state, 0.0)
        
        def get_recovery_time(firing_state):
            recovery = {
                pynlm.FiringState.Resting: 0.0,
                pynlm.FiringState.Active: 0.002,  # 2ms
                pynlm.FiringState.Refractory: 0.003,  # 3ms
                pynlm.FiringState.Inhibited: 0.001  # 1ms
            }
            return recovery.get(firing_state, 0.0)
        
        def get_network_role(firing_state):
            roles = {
                pynlm.FiringState.Resting: "baseline, ready state",
                pynlm.FiringState.Active: "information processing",
                pynlm.FiringState.Refractory: "post-spike recovery",
                pynlm.FiringState.Inhibited: "network regulation"
            }
            return roles.get(firing_state, "unknown")
        
        def get_transition_probability(from_state, to_state):
            # Simplified transition probabilities
            probabilities = {
                (pynlm.FiringState.Resting, pynlm.FiringState.Active): 0.1,
                (pynlm.FiringState.Active, pynlm.FiringState.Refractory): 0.05,
                (pynlm.FiringState.Refractory, pynlm.FiringState.Resting): 0.8,
                (pynlm.FiringState.Inhibited, pynlm.FiringState.Active): 0.3,
                (pynlm.FiringState.Resting, pynlm.FiringState.Inhibited): 0.05
            }
            
            return probabilities.get((from_state, to_state), 0.0)
        
        def calculate_state_efficiency(firing_state):
            base_efficiency = {
                pynlm.FiringState.Resting: 0.1,
                pynlm.FiringState.Active: 1.0,
                pynlm.FiringState.Refractory: 0.3,
                pynlm.FiringState.Inhibited: 0.4
            }
            return base_efficiency.get(firing_state, 0.5)
        ```
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration with comprehensive documentation.
        
        ActionType defines the different types of actions that the NLM system can execute
        in simulated environments. These actions represent the fundamental behaviors that
        an agent can perform to interact with its environment and achieve goals. Each action
        type has specific parameters, effects, and implementations in the motor system.
        
        ## Action Types and Their Descriptions:
        
        - **MoveForward**: Move the agent forward in the current direction
          - Motor command: Increase forward velocity
          - Energy cost: Moderate
          - Typical use: Navigating toward goals, exploration
          - Examples: Walking forward, swimming forward
        
        - **MoveBackward**: Move the agent backward in the current direction
          - Motor command: Decrease forward velocity (negative)
          - Energy cost: High
          - Typical use: Retreat from dangers, repositioning
          - Examples: Walking backward, swimming backward
        
        - **MoveLeft**: Rotate the agent counter-clockwise
          - Motor command: Apply leftward force/torque
          - Energy cost: Low
          - Typical use: Changing direction, avoiding obstacles
          - Examples: Turning left while walking, rotating in place
        
        - **MoveRight**: Rotate the agent clockwise
          - Motor command: Apply rightward force/torque
          - Energy cost: Low
          - Typical use: Navigation, alignment
          - Examples: Turning right while walking, rotating in place
        
        - **TurnLeft**: Rotate the agent significantly counter-clockwise
          - Motor command: Strong leftward rotation
          - Energy cost: Moderate
          - Typical use: Sharp turns, orientation changes
          - Examples: 90-degree left turns, spiral movements
        
        - **TurnRight**: Rotate the agent significantly clockwise
          - Motor command: Strong rightward rotation
          - Energy cost: Moderate
          - Typical use: Sharp turns, navigation corrections
          - Examples: 90-degree right turns, spiral movements
        
        - **Look**: Point attention in a specific direction
          - Motor command: Head/neck rotation without body movement
          - Energy cost: Very low
          - Typical use: Visual exploration, information gathering
          - Examples: Turning head to look around, scanning environment
        
        - **LookUp**: Point attention upward
          - Motor command: Upward head rotation
          - Energy cost: Very low
          - Typical use: Looking at overhead objects, ceiling
          - Examples: Looking up at hanging objects, ceiling lights
        
        - **LookDown**: Point attention downward
          - Motor command: Downward head rotation
          - Energy cost: Very low
          - Typical use: Looking at ground, feet
          - Examples: Looking down at ground obstacles, feet
        
        - **Interact**: Manipulate or interact with objects
          - Motor command: Complex sequence of movements
          - Energy cost: High
          - Typical use: Tool use, object manipulation, social interaction
          - Examples: Picking up items, opening doors, grooming
        
        - **Eat**: Consume food or nutrients
          - Motor command: Mouth positioning and ingestion
          - Energy cost: Very high
          - Typical use: Survival, energy acquisition
          - Examples: Chewing, swallowing, nutrient absorption
        
        - **Drink**: Consume liquids
          - Motor command: Mouth positioning and sipping
          - Energy cost: High
          - Typical use: Hydration, temperature regulation
          - Examples: Sipping water, drinking from containers
        
        - **Rest**: Stop activity and conserve energy
          - Motor command: Minimal movement, settling
          - Energy cost: Very low
          - Typical use: Recovery, energy conservation, digestion
          - Examples: Sitting down, resting, sleeping preparation
        
        - **Wait**: Pause without changing state
          - Motor command: Maintain current position
          - Energy cost: Minimal
          - Typical use: Strategic waiting, timing coordination
          - Examples: Waiting for opportunity, synchronization
        
        - **Custom**: User-defined action type
          - Motor command: User-specified sequence
          - Energy cost: Variable
          - Typical use: Specialized behaviors, research experiments
          - Examples: Species-specific behaviors, experimental protocols
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create an agent and test different action types
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Define action sequence
        action_sequence = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.Look,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Eat,
            pynlm.ActionType.Rest
        ]
        
        # Execute action sequence with energy monitoring
        energy_before = world.getAgentBody().energy
        total_energy_cost = 0.0
        
        for i, action_type in enumerate(action_sequence):
            # Create and apply action
            action = pynlm.Action(action_type)
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Estimate energy cost
            energy_cost = estimate_action_energy_cost(action_type)
            total_energy_cost += energy_cost
            
            print(f"Step {i+1}: {action_type}")
            print(f"  Energy cost: {energy_cost:.2f} units")
            print(f"  Current agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        
        energy_after = world.getAgentBody().energy
        net_energy_change = energy_after - energy_before
        
        print(f"\nAction Sequence Summary:")
        print(f"  Total energy cost: {total_energy_cost:.2f} units")
        print(f"  Net energy change: {net_energy_change:.2f} units")
        print(f"  Final agent state: energy={world.getAgentBody().energy:.1f}, age={world.getAgentBody().age}")
        
        # Analyze action efficiency
        def analyze_action_efficiency(actions):
            efficiency_metrics = {}
            
            for action in actions:
                metrics = {
                    "action": action,
                    "energy_cost": estimate_action_energy_cost(action),
                    "usefulness": estimate_action_usefulness(action),
                    "time_cost": estimate_action_time_cost(action),
                    "efficiency": calculate_action_efficiency(action)
                }
                efficiency_metrics[action] = metrics
            
            return efficiency_metrics
        
        # Create different action profiles
        exploration_actions = [
            pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft,
            pynlm.ActionType.Look, pynlm.ActionType.MoveForward,
            pynlm.ActionType.TurnRight, pynlm.ActionType.MoveForward
        ]
        
        survival_actions = [
            pynlm.ActionType.Eat, pynlm.ActionType.Rest,
            pynlm.ActionType.Interact, pynlm.ActionType.Drink
        ]
        
        exploration_metrics = analyze_action_efficiency(exploration_actions)
        survival_metrics = analyze_action_efficiency(survival_actions)
        
        print("\nExploration Actions Analysis:")
        for action, metrics in exploration_metrics.items():
            print(f"  {action}:")
            print(f"    Energy: {metrics['energy_cost']:.2f}, Usefulness: {metrics['usefulness']:.2f}")
            print(f"    Efficiency: {metrics['efficiency']:.3f}")
        
        print("\nSurvival Actions Analysis:")
        for action, metrics in survival_metrics.items():
            print(f"  {action}:")
            print(f"    Energy: {metrics['energy_cost']:.2f}, Usefulness: {metrics['usefulness']:.2f}")
            print(f"    Efficiency: {metrics['efficiency']:.3f}")
        
        # Create action-based behavior patterns
        def create_behavior_pattern(pattern_name, actions, repetitions):
            behavior = {
                "name": pattern_name,
                "actions": actions,
                "repetitions": repetitions,
                "total_duration": len(actions) * repetitions,
                "estimated_energy": sum(estimate_action_energy_cost(action) for _ in range(repetitions * len(actions))),
                "primary_function": determine_primary_function(actions)
            }
            
            return behavior
        
        # Define behavior patterns
        exploration_pattern = create_behavior_pattern(
            "Random Exploration",
            [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft, pynlm.ActionType.Look],
            10
        )
        
        foraging_pattern = create_behavior_pattern(
            "Resource Foraging",
            [pynlm.ActionType.MoveForward, pynlm.ActionType.Interact, pynlm.ActionType.Rest],
            5
        )
        
        navigation_pattern = create_behavior_pattern(
            "Goal Navigation",
            [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnRight, pynlm.ActionType.Look],
            8
        )
        
        # Analyze and compare behavior patterns
        behavior_patterns = [exploration_pattern, foraging_pattern, navigation_pattern]
        
        print("\nBehavior Pattern Analysis:")
        for pattern in behavior_patterns:
            print(f"\n{pattern['name']}:")
            print(f"  Total duration: {pattern['total_duration']} steps")
            print(f"  Estimated energy: {pattern['estimated_energy']:.2f} units")
            print(f"  Primary function: {pattern['primary_function']}")
        
        # Example helper functions (these would be defined elsewhere)
        def estimate_action_energy_cost(action_type):
            energy_costs = {
                pynlm.ActionType.MoveForward: 1.0,
                pynlm.ActionType.MoveBackward: 2.0,
                pynlm.ActionType.MoveLeft: 0.8,
                pynlm.ActionType.MoveRight: 0.8,
                pynlm.ActionType.TurnLeft: 1.5,
                pynlm.ActionType.TurnRight: 1.5,
                pynlm.ActionType.Look: 0.1,
                pynlm.ActionType.LookUp: 0.1,
                pynlm.ActionType.LookDown: 0.1,
                pynlm.ActionType.Interact: 3.0,
                pynlm.ActionType.Eat: 4.0,
                pynlm.ActionType.Drink: 2.5,
                pynlm.ActionType.Rest: 0.5,
                pynlm.ActionType.Wait: 0.2,
                pynlm.ActionType.Custom: 2.0
            }
            return energy_costs.get(action_type, 1.0)
        
        def estimate_action_usefulness(action_type):
            usefulness_values = {
                pynlm.ActionType.MoveForward: 0.8,
                pynlm.ActionType.MoveBackward: 0.3,
                pynlm.ActionType.MoveLeft: 0.7,
                pynlm.ActionType.MoveRight: 0.7,
                pynlm.ActionType.TurnLeft: 0.6,
                pynlm.ActionType.TurnRight: 0.6,
                pynlm.ActionType.Look: 0.9,
                pynlm.ActionType.LookUp: 0.6,
                pynlm.ActionType.LookDown: 0.6,
                pynlm.ActionType.Interact: 0.7,
                pynlm.ActionType.Eat: 1.0,
                pynlm.ActionType.Drink: 0.8,
                pynlm.ActionType.Rest: 0.5,
                pynlm.ActionType.Wait: 0.4,
                pynlm.ActionType.Custom: 0.5
            }
            return usefulness_values.get(action_type, 0.5)
        
        def estimate_action_time_cost(action_type):
            time_costs = {
                pynlm.ActionType.MoveForward: 1.0,
                pynlm.ActionType.MoveBackward: 1.5,
                pynlm.ActionType.MoveLeft: 0.8,
                pynlm.ActionType.MoveRight: 0.8,
                pynlm.ActionType.TurnLeft: 1.2,
                pynlm.ActionType.TurnRight: 1.2,
                pynlm.ActionType.Look: 0.3,
                pynlm.ActionType.LookUp: 0.3,
                pynlm.ActionType.LookDown: 0.3,
                pynlm.ActionType.Interact: 2.0,
                pynlm.ActionType.Eat: 2.5,
                pynlm.ActionType.Drink: 1.8,
                pynlm.ActionType.Rest: 0.5,
                pynlm.ActionType.Wait: 0.1,
                pynlm.ActionType.Custom: 1.5
            }
            return time_costs.get(action_type, 1.0)
        
        def calculate_action_efficiency(action_type):
            # Efficiency = usefulness / (energy_cost * time_cost)
            usefulness = estimate_action_usefulness(action_type)
            energy = estimate_action_energy_cost(action_type)
            time = estimate_action_time_cost(action_type)
            
            if energy * time == 0:
                return 0.0
            return usefulness / (energy * time)
        
        def determine_primary_function(actions):
            # Simple heuristic based on most frequent action
            action_counts = {}
            for action in actions:
                action_counts[action] = action_counts.get(action, 0) + 1
            
            if not action_counts:
                return "unknown"
            
            primary_action = max(action_counts.items(), key=lambda x: x[1])[0]
            
            function_map = {
                pynlm.ActionType.Eat: "survival",
                pynlm.ActionType.Drink: "hydration",
                pynlm.ActionType.Rest: "recovery",
                pynlm.ActionType.MoveForward: "navigation",
                pynlm.ActionType.Interact: "object manipulation",
                pynlm.ActionType.Look: "information gathering",
                pynlm.ActionType.MoveBackward: "retreat",
                pynlm.ActionType.TurnLeft: "direction change",
                pynlm.ActionType.TurnRight: "direction change",
                pynlm.ActionType.LookUp: "visual scanning",
                pynlm.ActionType.LookDown: "visual scanning",
                pynlm.ActionType.Wait: "timing coordination",
                pynlm.ActionType.Custom: "specialized behavior"
            }
            
            return function_map.get(primary_action, "unknown")
        ```
    )pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration with comprehensive documentation.
        
        MotorCommand defines the different types of motor commands that the NLM system
        can execute at a low level. These commands represent the fundamental motor
        outputs that drive agent behavior in simulated environments. Each motor command
        type has specific implementations, effects, and relationships to higher-level actions.
        
        ## Motor Command Types and Their Characteristics:
        
        - **MoveForward**: Basic forward movement command
          - Effect: Increase forward velocity
          - Control type: Continuous (speed control)
          - Typical use: Straight-line motion
          - Integration with: ActionType.MoveForward
          - Example: Basic walking, swimming forward
        
        - **MoveBackward**: Basic backward movement command
          - Effect: Decrease forward velocity (negative)
          - Control type: Continuous (speed control)
          - Typical use: Reverse motion, retreat
          - Integration with: ActionType.MoveBackward
          - Example: Walking backward, swimming backward
        
        - **TurnLeft**: Basic left rotation command
          - Effect: Rotate counter-clockwise
          - Control type: Continuous (angular velocity control)
          - Typical use: Gentle turns, gradual direction changes
          - Integration with: ActionType.TurnLeft, ActionType.MoveLeft
          - Example: Smooth left turns, gradual orientation changes
        
        - **TurnRight**: Basic right rotation command
          - Effect: Rotate clockwise
          - Control type: Continuous (angular velocity control)
          - Typical use: Gentle turns, gradual direction changes
          - Integration with: ActionType.TurnRight, ActionType.MoveRight
          - Example: Smooth right turns, gradual orientation changes
        
        - **LookLeft**: Left gaze command
          - Effect: Rotate head/neck left
          - Control type: Discrete (binary or continuous)
          - Typical use: Scanning environment, visual attention
          - Integration with: ActionType.Look
          - Example: Turning head left to scan surroundings
        
        - **LookRight**: Right gaze command
          - Effect: Rotate head/neck right
          - Control type: Discrete (binary or continuous)
          - Typical use: Scanning environment, visual attention
          - Integration with: ActionType.Look
          - Example: Turning head right to scan surroundings
        
        - **Interact**: Interaction command
          - Effect: Complex motor sequence for object manipulation
          - Control type: Complex (multi-degree of freedom)
          - Typical use: Tool use, social interaction, object manipulation
          - Integration with: ActionType.Interact
          - Example: Reaching for objects, opening doors
        
        - **Wait**: Stationary command
          - Effect: Maintain current state, no movement
          - Control type: Discrete (hold)
          - Typical use: Strategic waiting, timing coordination
          - Integration with: ActionType.Wait
          - Example: Pausing for opportunities, synchronization
        
        ## Integration with ActionType:
        
        The ActionType enumeration provides higher-level behavioral commands,
        while MotorCommand provides the actual motor control commands. There's
        typically a mapping between them:
        
        - ActionType.MoveForward → MotorCommand.MoveForward
        - ActionType.MoveBackward → MotorCommand.MoveBackward
        - ActionType.TurnLeft → MotorCommand.TurnLeft
        - ActionCommand.TurnRight → MotorCommand.TurnRight
        - ActionType.Look → MotorCommand.LookLeft or MotorCommand.LookRight
        - ActionType.Interact → MotorCommand.Interact
        - ActionType.Wait → MotorCommand.Wait
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create an agent and test different motor commands
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Define motor command sequence
        motor_sequence = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        
        # Execute motor command sequence with monitoring
        print("Executing Motor Command Sequence:")
        
        for i, motor_cmd in enumerate(motor_sequence):
            # Create action from motor command
            action = pynlm.Action(motor_cmd)
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Monitor agent state
            agent_body = world.getAgentBody()
            
            print(f"Step {i+1}: {motor_cmd}")
            print(f"  Position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
            print(f"  Orientation: {agent_body.orientation:.1f}°")
            print(f"  Velocity: ({agent_body.velocityX:.2f}, {agent_body.velocityY:.2f})")
            print(f"  Energy: {agent_body.energy:.1f}")
            print(f"  Health: {agent_body.health:.1f}")
        
        # Analyze motor command efficiency
        def analyze_motor_command_efficiency(motor_commands):
            efficiency_metrics = {}
            
            for cmd in motor_commands:
                metrics = {
                    "command": cmd,
                    "energy_cost": estimate_motor_command_energy(cmd),
                    "precision": estimate_motor_command_precision(cmd),
                    "speed": estimate_motor_command_speed(cmd),
                    "efficiency": calculate_motor_command_efficiency(cmd)
                }
                efficiency_metrics[cmd] = metrics
            
            return efficiency_metrics
        
        # Create different motor command profiles
        navigation_profile = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.MoveForward
        ]
        
        exploration_profile = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.LookRight,
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.TurnRight
        ]
        
        stationary_profile = [
            pynlm.MotorCommand.Wait,
            pynlm.MotorCommand.Wait,
            pynlm.MotorCommand.Wait
        ]
        
        navigation_metrics = analyze_motor_command_efficiency(navigation_profile)
        exploration_metrics = analyze_motor_command_efficiency(exploration_profile)
        stationary_metrics = analyze_motor_command_efficiency(stationary_profile)
        
        print("\nNavigation Motor Command Analysis:")
        total_navigation_cost = 0.0
        for cmd, metrics in navigation_metrics.items():
            print(f"  {cmd}:")
            print(f"    Energy: {metrics['energy_cost']:.2f}, Precision: {metrics['precision']:.2f}")
            print(f"    Speed: {metrics['speed']:.2f}, Efficiency: {metrics['efficiency']:.3f}")
            total_navigation_cost += metrics['energy_cost']
        
        print("\nExploration Motor Command Analysis:")
        total_exploration_cost = 0.0
        for cmd, metrics in exploration_metrics.items():
            print(f"  {cmd}:")
            print(f"    Energy: {metrics['energy_cost']:.2f}, Precision: {metrics['precision']:.2f}")
            print(f"    Speed: {metrics['speed']:.2f}, Efficiency: {metrics['efficiency']:.3f}")
            total_exploration_cost += metrics['energy_cost']
        
        print("\nStationary Motor Command Analysis:")
        total_stationary_cost = 0.0
        for cmd, metrics in stationary_metrics.items():
            print(f"  {cmd}:")
            print(f"    Energy: {metrics['energy_cost']:.2f}, Precision: {metrics['precision']:.2f}")
            print(f"    Speed: {metrics['speed']:.2f}, Efficiency: {metrics['efficiency']:.3f}")
            total_stationary_cost += metrics['energy_cost']
        
        # Create motor command sequences for specific tasks
        def create_motor_sequence_for_task(task_name, action_sequence):
            sequence = []
            
            for action in action_sequence:
                # Map ActionType to MotorCommand
                if action == pynlm.ActionType.MoveForward:
                    sequence.append(pynlm.MotorCommand.MoveForward)
                elif action == pynlm.ActionType.MoveBackward:
                    sequence.append(pynlm.MotorCommand.MoveBackward)
                elif action == pynlm.ActionType.TurnLeft:
                    sequence.append(pynlm.MotorCommand.TurnLeft)
                elif action == pynlm.ActionType.TurnRight:
                    sequence.append(pynlm.MotorCommand.TurnRight)
                elif action == pynlm.ActionType.Look:
                    sequence.append(pynlm.MotorCommand.LookLeft)
                elif action == pynlm.ActionType.Interact:
                    sequence.append(pynlm.MotorCommand.Interact)
                elif action == pynlm.ActionType.Wait:
                    sequence.append(pynlm.MotorCommand.Wait)
                else:
                    sequence.append(pynlm.MotorCommand.Wait)
            
            task_info = {
                "name": task_name,
                "sequence": sequence,
                "duration": len(sequence),
                "estimated_energy": sum(estimate_motor_command_energy(cmd) for cmd in sequence),
                "task_type": determine_task_type(action_sequence)
            }
            
            return task_info
        
        # Create task-specific motor sequences
        feeding_task = create_motor_sequence_for_task(
            "Foraging for Food",
            [pynlm.ActionType.MoveForward, pynlm.ActionType.Interact, pynlm.ActionType.Eat]
        )
        
        social_task = create_motor_sequence_for_task(
            "Social Interaction",
            [pynlm.ActionType.MoveForward, pynlm.ActionType.Interact, pynlm.ActionType.Rest]
        )
        
        exploration_task = create_motor_sequence_for_task(
            "Environmental Exploration",
            [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft, pynlm.ActionType.Look,
             pynlm.ActionType.MoveForward, pynlm.ActionType.TurnRight, pynlm.ActionType.Look]
        )
        
        print("\nTask-Specific Motor Sequences:")
        for task in [feeding_task, social_task, exploration_task]:
            print(f"\n{task['name']}:")
            print(f"  Duration: {task['duration']} steps")
            print(f"  Estimated energy: {task['estimated_energy']:.2f} units")
            print(f"  Task type: {task['task_type']}")
        
        # Example helper functions (these would be defined elsewhere)
        def estimate_motor_command_energy(motor_cmd):
            energy_costs = {
                pynlm.MotorCommand.MoveForward: 1.0,
                pynlm.MotorCommand.MoveBackward: 1.5,
                pynlm.MotorCommand.TurnLeft: 0.8,
                pynlm.MotorCommand.TurnRight: 0.8,
                pynlm.MotorCommand.LookLeft: 0.3,
                pynlm.MotorCommand.LookRight: 0.3,
                pynlm.MotorCommand.Interact: 2.0,
                pynlm.MotorCommand.Wait: 0.1
            }
            return energy_costs.get(motor_cmd, 0.5)
        
        def estimate_motor_command_precision(motor_cmd):
            precision_values = {
                pynlm.MotorCommand.MoveForward: 0.8,
                pynlm.MotorCommand.MoveBackward: 0.7,
                pynlm.MotorCommand.TurnLeft: 0.9,
                pynlm.MotorCommand.TurnRight: 0.9,
                pynlm.MotorCommand.LookLeft: 1.0,
                pynlm.MotorCommand.LookRight: 1.0,
                pynlm.MotorCommand.Interact: 0.6,
                pynlm.MotorCommand.Wait: 1.0
            }
            return precision_values.get(motor_cmd, 0.5)
        
        def estimate_motor_command_speed(motor_cmd):
            speed_values = {
                pynlm.MotorCommand.MoveForward: 2.0,
                pynlm.MotorCommand.MoveBackward: 1.5,
                pynlm.MotorCommand.TurnLeft: 1.5,
                pynlm.MotorCommand.TurnRight: 1.5,
                pynlm.MotorCommand.LookLeft: 1.0,
                pynlm.MotorCommand.LookRight: 1.0,
                pynlm.MotorCommand.Interact: 0.8,
                pynlm.MotorCommand.Wait: 0.1
            }
            return speed_values.get(motor_cmd, 0.5)
        
        def calculate_motor_command_efficiency(motor_cmd):
            # Efficiency = precision / (energy_cost * (1/speed))
            precision = estimate_motor_command_precision(motor_cmd)
            energy = estimate_motor_command_energy(motor_cmd)
            speed = estimate_motor_command_speed(motor_cmd)
            
            if energy * (1.0/speed) == 0:
                return 0.0
            return precision / (energy * (1.0/speed))
        
        def determine_task_type(actions):
            # Determine primary task based on action composition
            action_counts = {}
            for action in actions:
                action_counts[action] = action_counts.get(action, 0) + 1
            
            if not action_counts:
                return "unknown"
            
            primary_action = max(action_counts.items(), key=lambda x: x[1])[0]
            
            task_map = {
                pynlm.ActionType.Eat: "survival",
                pynlm.ActionType.Interact: "manipulation",
                pynlm.ActionType.MoveForward: "navigation",
                pynlm.ActionType.Look: "observation",
                pynlm.ActionType.Rest: "recovery",
                pynlm.ActionType.TurnLeft: "direction change",
                pynlm.ActionType.TurnRight: "direction change"
            }
            
            return task_map.get(primary_action, "general")
        ```
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration with comprehensive documentation.
        
        WorldObjectType defines the different types of objects that can exist
        in the NLM simulation environment. These object types represent the
        various entities and features that agents can interact with in their environment.
        Understanding object types is crucial for implementing appropriate
        perception, action selection, and learning strategies.
        
        ## World Object Types and Their Characteristics:
        
        - **Empty**: Placeholder or unoccupied space
          - No physical or functional properties
          - Can be traversed without interaction
          - Represents open space or absence of objects
          - Examples: Empty grid cells, unoccupied areas
        
        - **Resource**: Valuable items or substances
          - Provides energy, nutrients, or other benefits
          - Can be consumed or collected
          - Increases agent fitness or capabilities
          - Examples: Food, water, energy crystals, rare minerals
        
        - **Hazard**: Dangerous or harmful elements
          - Causes damage, injury, or negative effects
          - Should be avoided or mitigated
          - Decreases agent fitness or survival chances
          - Examples: Fire, poison, radiation, predators
        
        - **Wall**: Physical barriers or obstacles
          - Cannot be passed through without special abilities
          - Defines environmental boundaries and structure
          - May provide support or shelter
          - Examples: Solid walls, fences, rock formations
        
        - **Marker**: Reference points or landmarks
          - Used for navigation and positioning
          - Provides environmental structure
          - May have symbolic or functional significance
          - Examples: Waypoints, checkpoints, goal markers
        
        ## Interaction Patterns:
        
        Different object types enable different interaction patterns:
        
        - **Empty Space**: Movement, exploration, navigation
        - **Resources**: Consumption, collection, storage
        - **Hazards**: Avoidance, defense, mitigation
        - **Walls**: Navigation around, climbing, breaking
        - **Markers**: Path following, goal achievement, reference
        
        ## Example Usage:
        ```python
        import pynlm
        
        # Create an environment with different object types
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Add various world objects for testing
        world_objects = [
            pynlm.WorldObjectType.Empty,
            pynlm.WorldObjectType.Resource,
            pynlm.WorldObjectType.Hazard,
            pynlm.WorldObjectType.Wall,
            pynlm.WorldObjectType.Marker
        ]
        
        # Create and place different types of objects
        placed_objects = []
        
        for i, obj_type in enumerate(world_objects):
            # Create object based on type
            if obj_type == pynlm.WorldObjectType.Resource:
                obj = pynlm.WorldObject(
                    x=i * 2,
                    y=0,
                    type=obj_type,
                    value=100.0,  # Energy value
                    radius=0.5
                )
                print(f"Placed Resource at ({i*2}, 0): value={obj.value}")
                
            elif obj_type == pynlm.WorldObjectType.Hazard:
                obj = pynlm.WorldObject(
                    x=i * 2,
                    y=10,
                    type=obj_type,
                    value=-50.0,  # Negative value (damage)
                    radius=0.5
                )
                print(f"Placed Hazard at ({i*2}, 10): damage={abs(obj.value)}")
                
            elif obj_type == pynlm.WorldObjectType.Wall:
                obj = pynlm.WorldObject(
                    x=i * 2,
                    y=5,
                    type=obj_type,
                    value=0.0,
                    radius=0.8
                )
                print(f"Placed Wall at ({i*2}, 5): blocks movement")
                
            elif obj_type == pynlm.WorldObjectType.Marker:
                obj = pynlm.WorldObject(
                    x=i * 2,
                    y=15,
                    type=obj_type,
                    value=1.0,  # Marker significance
                    radius=0.3
                )
                print(f"Placed Marker at ({i*2}, 15): navigation point")
            
            # Place object in world if position is valid
            if world.isValidPosition(obj.x, obj.y):
                world.addObject(obj)
                placed_objects.append(obj)
            else:
                print(f"Could not place object at ({obj.x}, {obj.y}): invalid position")
        
        # Analyze object distribution and properties
        def analyze_object_environment(world):
            analysis = {
                "total_objects": len(world.getAgentBody().age),  # Placeholder - should track objects
                "empty_spaces": 0,
                "resources": 0,
                "hazards": 0,
                "walls": 0,
                "markers": 0,
                "resource_value": 0.0,
                "hazard_intensity": 0.0
            }
            
            # This would require iterating through world objects
            # For demonstration, we'll use the simple world API
            agent_body = world.getAgentBody()
            
            # Create a simple environment analysis
            print(f"\nWorld Environment Analysis:")
            print(f"  World dimensions: {world.getWidth()}x{world.getHeight()}")
            print(f"  Agent position: ({agent_body.x}, {agent_body.y})")
            print(f"  Max energy: {world.getMaxEnergy()}")
            print(f"  Energy decay rate: {world.getEnergyDecayRate()}")
            
            return analysis
        
        analysis = analyze_object_environment(world)
        
        # Create agent behavior based on object types
        def determine_agent_behavior(world, agent):
            # Get current agent position
            agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
            
            # Look for nearby objects
            nearby_resources = []
            nearby_hazards = []
            nearby_walls = []
            
            # This would scan world for objects in proximity
            # For demonstration, we'll use the world objects that were placed
            print(f"\nAgent Behavior Analysis:")
            print(f"  Agent at ({agent_x}, {agent_y})")
            
            # Simple behavior logic based on position
            if agent_y > 12:  # Upper area
                print("  Strategy: Approach resources (upper area)")
                recommended_action = "MoveForward"
            elif agent_y < 8:  # Lower area
                print("  Strategy: Avoid hazards (lower area)")
                recommended_action = "MoveBackward"
            elif agent_y < 10:  # Middle-lower area
                print("  Strategy: Navigate around walls")
                recommended_action = "TurnLeft"
            else:  # Middle-upper area
                print("  Strategy: Use markers for navigation")
                recommended_action = "LookLeft"
            
            return recommended_action
        
        # Create agent and analyze behavior
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Simulate agent movement and interaction
        print("\nSimulating Agent-Object Interactions:")
        
        for step in range(20):
            # Get agent recommendations based on current environment
            recommended_action = determine_agent_behavior(world, agent)
            
            # Apply the recommended action
            action = pynlm.Action(getattr(pynlm.ActionType, recommended_action))
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print step information
            agent_body = world.getAgentBody()
            print(f"Step {step+1}: {recommended_action}")
            print(f"  Position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
            print(f"  Energy: {agent_body.energy:.1f}")
            
            # Update world
            world.update(0.1)
        
        # Analyze final environment and outcomes
        print(f"\nFinal Environment Analysis:")
        final_agent = world.getAgentBody()
        print(f"  Final position: ({final_agent.x:.1f}, {final_agent.y:.1f})")
        print(f"  Final energy: {final_agent.energy:.1f}")
        print(f"  Final health: {final_agent.health:.1f}")
        print(f"  Final age: {final_agent.age}")
        
        # Create different environment scenarios
        def create_environment_scenario(scenario_name, object_layout):
            scenario = {
                "name": scenario_name,
                "layout": object_layout,
                "difficulty": calculate_environment_difficulty(object_layout),
                "reward_structure": calculate_reward_structure(object_layout),
                "agent_strategies": generate_agent_strategies(object_layout)
            }
            
            return scenario
        
        # Define different environment scenarios
        resource_rich_environment = [
            (10, 5, pynlm.WorldObjectType.Resource),
            (15, 8, pynlm.WorldObjectType.Resource),
            (8, 12, pynlm.WorldObjectType.Resource)
        ]
        
        hazard_rich_environment = [
            (5, 3, pynlm.WorldObjectType.Hazard),
            (12, 10, pynlm.WorldObjectType.Hazard),
            (18, 7, pynlm.WorldObjectType.Hazard)
        ]
        
        balanced_environment = [
            (7, 4, pynlm.WorldObjectType.Resource),
            (13, 9, pynlm.WorldObjectType.Hazard),
            (3, 15, pynlm.WorldObjectType.Wall),
            (17, 6, pynlm.WorldObjectType.Marker)
        ]
        
        # Create and analyze different scenarios
        scenarios = [
            ("Resource-Rich", resource_rich_environment),
            ("Hazard-Rich", hazard_rich_environment),
            ("Balanced", balanced_environment)
        ]
        
        print("\nEnvironment Scenario Analysis:")
        for scenario_name, layout in scenarios:
            scenario = create_environment_scenario(scenario_name, layout)
            print(f"\n{scenario['name']} Environment:")
            print(f"  Difficulty: {scenario['difficulty']:.2f}")
            print(f"  Reward structure: {scenario['reward_structure']:.2f}")
            print(f"  Agent strategies: {', '.join(scenario['agent_strategies'])}")
        
        # Example helper functions (these would be defined elsewhere)
        def calculate_environment_difficulty(layout):
            # Simple difficulty calculation based on object count and types
            hazard_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Hazard)
            resource_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Resource)
            wall_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Wall)
            
            return (hazard_count * 1.5) + (wall_count * 1.0) + (resource_count * 0.5)
        
        def calculate_reward_structure(layout):
            # Calculate potential rewards based on object distribution
            resource_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Resource)
            marker_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Marker)
            
            return resource_count * 10.0 + marker_count * 5.0
        
        def generate_agent_strategies(layout):
            # Generate agent strategies based on environment characteristics
            strategies = []
            
            hazard_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Hazard)
            resource_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Resource)
            wall_count = sum(1 for _, _, obj_type in layout if obj_type == pynlm.WorldObjectType.Wall)
            
            if hazard_count > resource_count:
                strategies.append("hazard avoidance")
                strategies.append("conservative navigation")
            elif resource_count > hazard_count:
                strategies.append("resource maximization")
                strategies.append("aggressive exploration")
            else:
                strategies.append("balanced exploration")
                strategies.append("adaptive navigation")
            
            if wall_count > 0:
                strategies.append("obstacle negotiation")
            
            if any(obj_type == pynlm.WorldObjectType.Marker for _, _, obj_type in layout):
                strategies.append("waypoint following")
            
            return strategies
        ```
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToString", &Config::saveToString,
             "Save configuration to a JSON string")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get a configuration value as string")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get), py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value as int")
        .def("getFloat", &Config::getFloat, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get a configuration value as float")
        .def("getBool", &Config::getBool, py::arg("key"), py::arg("defaultValue") = false,
             "Get a configuration value as boolean")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("setInt", &Config::setInt, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setFloat", &Config::setFloat, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("setBool", &Config::setBool, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("setString", &Config::setString, py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("validate", &Config::validate,
             "Validate the configuration")
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

    // Convenience factory functions
    m.def("createBrainFromFile", [](const std::string& configFile) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        return std::make_shared<Brain>(config);
    }, py::arg("configFile"), "Create a brain from a configuration file");

    m.def("createBrainFromDict", [](py::dict configDict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Convert Python dict to Config (simplified implementation)
        for (auto it : configDict) {
            std::string key = py::str(it.first);
            if (py::isinstance(it.second, py::int_())) {
                config->setInt(key, it.second.cast<int>());
            } else if (py::isinstance(it.second, py::float_())) {
                config->setFloat(key, it.second.cast<double>());
            } else if (py::isinstance(it.second, py::bool_())) {
                config->setBool(key, it.second.cast<bool>());
            } else if (py::isinstance(it.second, py::str())) {
                config->setString(key, it.second.cast<std::string>());
            }
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configDict"), "Create a brain from a Python dictionary configuration");

    m.def("createAgentBrainFromConfig", [](const std::string& configFile) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("configFile"), "Create an agent brain from a configuration file");

    m.def("createWorldWithConfig", [](size_t width, size_t height, 
                                     size_t visionWidth = 64, size_t visionHeight = 64,
                                     const std::string& configFile = "") -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        if (!configFile.empty()) {
            // Load world-specific configuration if needed
        }
        return world;
    }, py::arg("width"), py::arg("height"), 
       py::arg("visionWidth") = 64, py::arg("visionHeight") = 64,
       py::arg("configFile") = "",
       "Create a world with specified dimensions");

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

    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) -> bool {
        // Basic validation for brain configuration
        if (!config) return false;
        
        // Check for essential keys
        std::vector<std::string> essentialKeys = {
            "neuron_count", "synapse_density", "simulation_time_step"
        };
        
        bool valid = true;
        for (const auto& key : essentialKeys) {
            if (!config->has(key)) {
                py::print("Warning: Essential config key not found: " + key);
                valid = false;
            }
        }
        
        return valid;
    }, py::arg("config"), "Validate a brain configuration");

    m.def("createSensoryPercept", [](py::dict visionDict, py::dict touchDict, 
                                   py::dict internalDict, py::dict audioDict) -> std::shared_ptr<SensoryPercept> {
        auto percept = std::make_shared<SensoryPercept>();
        
        if (!visionDict.empty()) {
            auto vision = std::make_shared<Vision>(
                visionDict.get("width", DEFAULT_VISION_WIDTH).cast<size_t>(),
                visionDict.get("height", DEFAULT_VISION_HEIGHT).cast<size_t>(),
                visionDict.get("channels", 3).cast<size_t>()
            );
            std::vector<float> visionData;
            auto dataList = visionDict.get("data", py::list()).cast<py::list>();
            for (const auto& item : dataList) {
                // Validate float conversion
                try {
                    visionData.push_back(item.cast<float>());
                } catch (const py::cast_error&) {
                    throw py::value_error("Invalid vision data format: expected float values");
                }
            }
            
            // Ensure data size matches expected dimensions
            size_t expectedDataSize = vision->getWidth() * vision->getHeight() * vision->getChannels();
            if (visionData.size() > 0 && visionData.size() != expectedDataSize && expectedDataSize > 0) {
                // Resize data to match expected dimensions if possible
                if (visionData.size() < expectedDataSize) {
                    visionData.resize(expectedDataSize, 0.0f);
                }
            }
            
            vision->setData(visionData);
            percept->setVision(vision);
        }
        
        if (!audioDict.empty()) {
            // Fix: Get audio parameters before creating the audio object
            size_t sampleRate = audioDict.get("sampleRate", DEFAULT_SAMPLE_RATE).cast<size_t>();
            size_t numSamples = audioDict.get("numSamples", DEFAULT_AUDIO_SAMPLES).cast<size_t>();
            
            auto audio = std::make_shared<Audio>(sampleRate, numSamples);
            
            std::vector<float> audioData;
            auto dataList = audioDict.get("data", py::list()).cast<py::list>();
            for (const auto& item : dataList) {
                // Validate float conversion
                try {
                    audioData.push_back(item.cast<float>());
                } catch (const py::cast_error&) {
                    throw py::value_error("Invalid audio data format: expected float values");
                }
            }
            
            // Ensure data size matches expected samples
            if (audioData.size() > 0 && audioData.size() != numSamples && numSamples > 0) {
                // Resize data to match expected samples if possible
                if (audioData.size() < numSamples) {
                    audioData.resize(numSamples, 0.0f);
                }
            }
            
            audio->setData(audioData);
            // Sample rate is already set during construction
            
            percept->setAudio(audio);
        }
        
        return percept;
    }, py::arg("visionDict") = py::dict(),
       py::arg("touchDict") = py::dict(),
       py::arg("internalDict") = py::dict(),
       py::arg("audioDict") = py::dict(),
       "Create a sensory percept from Python dictionaries");

    m.def("createActionFromDict", [](py::dict actionDict) -> std::shared_ptr<Action> {
        ActionType actionType = ActionType::Wait;
        // Validate type string conversion with error handling
        try {
            std::string typeStr = actionDict.get("type", "Wait").cast<std::string>();
            
            // Map string to ActionType
            if (typeStr == "MoveForward") actionType = ActionType::MoveForward;
            else if (typeStr == "MoveBackward") actionType = ActionType::MoveBackward;
            else if (typeStr == "TurnLeft") actionType = ActionType::TurnLeft;
            else if (typeStr == "TurnRight") actionType = ActionType::TurnRight;
            else if (typeStr == "Look") actionType = ActionType::Look;
            else if (typeStr == "LookUp") actionType = ActionType::LookUp;
            else if (typeStr == "LookDown") actionType = ActionType::LookDown;
            else if (typeStr == "Interact") actionType = ActionType::Interact;
            else if (typeStr == "Eat") actionType = ActionType::Eat;
            else if (typeStr == "Drink") actionType = ActionType::Drink;
            else if (typeStr == "Rest") actionType = ActionType::Rest;
            else if (typeStr == "Wait") actionType = ActionType::Wait;
        } catch (const py::cast_error&) {
            throw py::value_error("Invalid action type: expected string value");
        }
        
        std::vector<float> parameters;
        // Safely get parameters list with error handling
        try {
            auto paramsList = actionDict.get("parameters", py::list()).cast<py::list>();
            for (const auto& item : paramsList) {
                parameters.push_back(item.cast<float>());
            }
        } catch (const py::cast_error&) {
            // If parameters cannot be cast, use empty parameters
            // This is more robust than failing entirely
        }
        
        return std::make_shared<Action>(actionType, parameters);
    }, py::arg("actionDict"), "Create an action from a Python dictionary");

    m.def("createWorldObjectFromDict", [](py::dict objDict) -> std::shared_ptr<WorldObject> {
        float x = objDict.get("x", 0.0f).cast<float>();
        float y = objDict.get("y", 0.0f).cast<float>();
        std::string typeStr = objDict.get("type", "Empty").cast<std::string>();
        WorldObjectType objType = WorldObjectType::Empty;
        
        if (typeStr == "Resource") objType = WorldObjectType::Resource;
        else if (typeStr == "Hazard") objType = WorldObjectType::Hazard;
        else if (typeStr == "Wall") objType = WorldObjectType::Wall;
        else if (typeStr == "Marker") objType = WorldObjectType::Marker;
        
        float value = objDict.get("value", 0.0f).cast<float>();
        float radius = objDict.get("radius", 0.5f).cast<float>();
        
        return std::make_shared<WorldObject>(x, y, objType, value, radius);
    }, py::arg("objDict"), "Create a world object from a Python dictionary");

    // Constants for convenience
    m.attr("DEFAULT_VISION_WIDTH") = 64;
    m.attr("DEFAULT_VISION_HEIGHT") = 64;
    m.attr("DEFAULT_SAMPLE_RATE") = 44100;
    m.attr("DEFAULT_AUDIO_SAMPLES") = 1024;
    m.attr("DEFAULT_WORLD_WIDTH") = 100;
    m.attr("DEFAULT_WORLD_HEIGHT") = 100;
    m.attr("DEFAULT_MAX_ENERGY") = 100.0f;
    m.attr("DEFAULT_ENERGY_DECAY_RATE") = 0.01f;

    // Visualization helper functions
    m.def("visualizeBrainStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) return;
        
        auto stats = py::dict();
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        
        py::print("=== Brain Statistics ===");
        py::print("Total Neurons: " + std::to_string(stats["total_neurons"].cast<int>()));
        py::print("Total Synapses: " + std::to_string(stats["total_synapses"].cast<int>()));
        py::print("Active Neurons: " + std::to_string(stats["active_neurons"].cast<int>()));
        py::print("Firing Neurons: " + std::to_string(stats["firing_neurons"].cast<int>()));
        py::print("Average Firing Rate: " + std::to_string(stats["average_firing_rate"].cast<double>()));
        py::print("E/I Ratio: " + std::to_string(stats["excitatory_inhibition_ratio"].cast<double>()));
        py::print("Total Spikes: " + std::to_string(stats["total_spike_count"].cast<int>()));
        
        return stats;
    }, py::arg("brain"), "Get and display brain statistics");

    m.def("visualizeWorldState", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) return py::dict();
        
        auto state = py::dict();
        state["width"] = world->getWidth();
        state["height"] = world->getHeight();
        state["simulation_time"] = world->getSimulationTime();
        state["agent_x"] = world->getAgentBody().x;
        state["agent_y"] = world->getAgentBody().y;
        state["agent_orientation"] = world->getAgentBody().orientation;
        state["agent_energy"] = world->getAgentBody().energy;
        state["agent_health"] = world->getAgentBody().health;
        state["agent_age"] = world->getAgentBody().age;
        
        py::print("=== World State ===");
        py::print("World Size: " + std::to_string(state["width"].cast<int>()) + "x" + std::to_string(state["height"].cast<int>()));
        py::print("Simulation Time: " + std::to_string(state["simulation_time"].cast<double>()));
        py::print("Agent Position: (" + std::to_string(state["agent_x"].cast<double>()) + ", " + std::to_string(state["agent_y"].cast<double>()) + ")");
        py::print("Agent Orientation: " + std::to_string(state["agent_orientation"].cast<double>()));
        py::print("Agent Energy: " + std::to_string(state["agent_energy"].cast<double>()));
        py::print("Agent Health: " + std::to_string(state["agent_health"].cast<double>()));
        py::print("Agent Age: " + std::to_string(state["agent_age"].cast<int>()));
        
        return state;
    }, py::arg("world"), "Get and display world state");

    m.def("visualizePerceptData", [](std::shared_ptr<SensoryPercept> percept) {
        if (!percept) return py::dict();
        
        auto data = py::dict();
        
        auto vision = percept->getVision();
        if (vision) {
            auto visionData = py::dict();
            visionData["width"] = vision->getWidth();
            visionData["height"] = vision->getHeight();
            visionData["channels"] = vision->getChannels();
            
            std::vector<float> rawData = vision->getData();
            std::vector<float> processedData;
            
            // Simple visualization: show first 100 pixels as grayscale
            int sampleSize = std::min(100, static_cast<int>(rawData.size()));
            for (int i = 0; i < sampleSize; i++) {
                processedData.push_back(rawData[i]);
            }
            
            visionData["data_sample"] = processedData;
            data["vision"] = visionData;
        }
        
        auto audio = percept->getAudio();
        if (audio) {
            auto audioData = py::dict();
            audioData["sample_rate"] = audio->getSampleRate();
            audioData["num_samples"] = audio->getNumSamples();
            
            std::vector<float> rawData = audio->getData();
            std::vector<float> processedData;
            
            // Simple visualization: show first 100 samples
            int sampleSize = std::min(100, static_cast<int>(rawData.size()));
            for (int i = 0; i < sampleSize; i++) {
                processedData.push_back(rawData[i]);
            }
            
            audioData["data_sample"] = processedData;
            data["audio"] = audioData;
        }
        
        py::print("=== Percept Data ===");
        if (!data.empty()) {
            py::print("Vision available: " + std::to_string(data.contains("vision")));
            py::print("Audio available: " + std::to_string(data.contains("audio")));
        } else {
            py::print("No sensory data available");
        }
        
        return data;
    }, py::arg("percept"), "Get and display percept data");

    m.def("createSimulationSummary", [](std::shared_ptr<Brain> brain, 
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<SensoryPercept> percept) {
        auto summary = py::dict();
        
        if (brain) {
            auto brainStats = py::dict();
            brainStats["neuron_count"] = brain->getTotalNeuronCount();
            brainStats["synapse_count"] = brain->getTotalSynapseCount();
            brainStats["active_neurons"] = brain->getActiveNeuronCount();
            brainStats["firing_rate"] = brain->getAverageFiringRate();
            summary["brain"] = brainStats;
        }
        
        if (world) {
            auto worldStats = py::dict();
            worldStats["width"] = world->getWidth();
            worldStats["height"] = world->getHeight();
            worldStats["simulation_time"] = world->getSimulationTime();
            worldStats["agent_x"] = world->getAgentBody().x;
            worldStats["agent_y"] = world->getAgentBody().y;
            summary["world"] = worldStats;
        }
        
        if (percept) {
            auto perceptStats = py::dict();
            auto vision = percept->getVision();
            if (vision) {
                perceptStats["vision_width"] = vision->getWidth();
                perceptStats["vision_height"] = vision->getHeight();
                perceptStats["vision_channels"] = vision->getChannels();
            }
            summary["percept"] = perceptStats;
        }
        
        py::print("=== Simulation Summary ===");
        if (brain) {
            py::print("Brain: " + std::to_string(summary["brain"].cast<py::dict>()["neuron_count"].cast<int>()) + " neurons, " + 
                     std::to_string(summary["brain"].cast<py::dict>()["synapse_count"].cast<int>()) + " synapses");
        }
        if (world) {
            py::print("World: " + std::to_string(summary["world"].cast<py::dict>()["width"].cast<int>()) + "x" + 
                     std::to_string(summary["world"].cast<py::dict>()["height"].cast<int>()) + " at t=" + 
                     std::to_string(summary["world"].cast<py::dict>()["simulation_time"].cast<double>()));
        }
        if (percept) {
            py::print("Percept: Vision " + std::to_string(summary["percept"].cast<py::dict>()["vision_width"].cast<int>()) + 
                     "x" + std::to_string(summary["percept"].cast<py::dict>()["vision_height"].cast<int>()));
        }
        
        return summary;
    }, py::arg("brain"), py::arg("world"), py::arg("percept"),
       "Create a comprehensive simulation summary");

    // Add the original constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
