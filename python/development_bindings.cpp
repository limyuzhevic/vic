// Python bindings for NLM plasticity and development systems
// This file contains bindings for plasticity rules, development system, and related components

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

namespace py = pybind11;
namespace nlm {

void bindPlasticityRule(py::module& m);
void bindSTDP(py::module& m);
void bindHebbian(py::module& m);
void bindStructuralPlasticity(py::module& m);
void bindDevelopmentSystem(py::module& m);
void bindSynaptogenesis(py::module& m);
void bindPruning(py::module& m);
void bindMaturation(py::module& m);

void include_plasticity_bindings(py::module& m) {
    bindPlasticityRule(m);
    bindSTDP(m);
    bindHebbian(m);
    bindStructuralPlasticity(m);
}

void include_development_bindings(py::module& m) {
    bindDevelopmentSystem(m);
    bindSynaptogenesis(m);
    bindPruning(m);
    bindMaturation(m);
}

void bindPlasticityRule(py::module& m) {
    // PlasticityRule class - Base plasticity system
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(
        Base plasticity rule for NLM synaptic learning.
        
        The PlasticityRule class provides the foundation for synaptic plasticity
        in the NLM system, implementing:
        - Synaptic weight changes based on neural activity
        - Hebbian learning: "neurons that fire together, wire together"
        - Anti-Hebbian learning: "neurons that fire out of sync, damp each other"
        - Homeostatic plasticity: maintaining stable network activity
        - Neuromodulated plasticity: learning modulated by neuromodulators
        
        Plasticity rules enable the brain to:
        - Learn from experience
        - Adapt neural connections
        - Optimize network performance
        - Maintain stability while learning
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create plasticity system
            >>> plasticity = pynlm.PlasticityRule()
            >>> 
            >>> # Enable STDP
            >>> plasticity.enableSTDP(True)
            >>> 
            >>> # Enable Hebbian learning
            >>> plasticity.enableHebbian(True)
            >>> 
            >>> # Enable structural plasticity
            >>> plasticity.enableStructuralPlasticity(True)
            >>> 
            >>> # Apply plasticity
            >>> plasticity.apply( pre_neuron, post_neuron, pre_spike, post_spike)
            >>> 
            >>> # Get weight change
            >>> delta_weight = plasticity.getWeightChange()
        
        Methods:
            __init__(): Create empty plasticity rule
            apply(pre_neuron, post_neuron, pre_spike, post_spike): Apply plasticity
            getWeightChange(): Get synaptic weight change
            enableSTDP(enable): Enable/disable STDP
            enableHebbian(enable): Enable/disable Hebbian
            enableStructural(enable): Enable/disable structural plasticity
            isSTDP-enabled(): Check if STDP is enabled
            isHebbian-enabled(): Check if Hebbian is enabled
            isStructural-enabled(): Check if structural plasticity is enabled
            reset(): Reset plasticity rule
            getLearningRate(): Get learning rate
            setLearningRate(rate): Set learning rate
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
        
        Attributes:
            weight_change: Current synaptic weight change
            stdp_enabled: Whether STDP is enabled
            hebbian_enabled: Whether Hebbian is enabled
            structural_enabled: Whether structural plasticity is enabled
            learning_rate: Learning rate for plasticity
            
        See Also:
            Brain, STDP, Hebbian, StructuralPlasticity, Synapse
    )pbdoc")
        .def(py::init<>())
        
        // Plasticity application
        .def("apply", &PlasticityRule::apply,
             py::arg("pre_neuron"), py::arg("post_neuron"),
             py::arg("pre_spike"), py::arg("post_spike"),
             "Apply plasticity based on spike timing")
        
        .def("getWeightChange", &PlasticityRule::getWeightChange,
             "Get synaptic weight change")
        
        // Plasticity rule configuration
        .def("enableSTDP", &PlasticityRule::enableSTDP, py::arg("enable"),
             "Enable/disable spike timing dependent plasticity")
        
        .def("enableHebbian", &PlasticityRule::enableHebbian, py::arg("enable"),
             "Enable/disable Hebbian learning")
        
        .def("enableStructuralPlasticity", &PlasticityRule::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        
        // Plasticity rule status
        .def("isSTDPEnabled", &PlasticityRule::isSTDPEnabled,
             "Check if STDP is enabled")
        
        .def("isHebbianEnabled", &PlasticityRule::isHebbianEnabled,
             "Check if Hebbian learning is enabled")
        
        .def("isStructuralPlasticityEnabled", &PlasticityRule::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        
        // System management
        .def("reset", &PlasticityRule::reset,
             "Reset plasticity rule")
        
        // Learning parameters
        .def("getLearningRate", &PlasticityRule::getLearningRate,
             "Get learning rate")
        
        .def("setLearningRate", &PlasticityRule::setLearningRate, py::arg("rate"),
             "Set learning rate")
        
        .def("getStrength", &PlasticityRule::getStrength,
             "Get modulation strength")
        
        .def("setStrength", &PlasticityRule::setStrength, py::arg("strength"),
             "Set modulation strength")
        
        .def("__repr__", [](const PlasticityRule& plasticity) {
            return "<PlasticityRule: learning_rate=" + std::to_string(plasticity.getLearningRate()) +
                   ", stdp=" + std::to_string(plasticity.isSTDPEnabled()) +
                   ", hebbian=" + std::to_string(plasticity.isHebbianEnabled()) +
                   ", structural=" + std::to_string(plasticity.isStructuralPlasticityEnabled()) + ">";
        }, "String representation of plasticity rule");
}

void bindSTDP(py::module& m) {
    // STDP class - Spike timing dependent plasticity
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(
        Spike timing dependent plasticity (STDP) for NLM brain.
        
        The STDP class implements spike timing dependent plasticity, a
        learning rule where the effect of a spike on synaptic weight
        depends on the precise timing relative to postsynaptic spikes:
        
        - Long-term potentiation (LTP): Pre-before-post spikes strengthen synapse
        - Long-term depression (LTD): Post-before-pre spikes weaken synapse
        - Asymmetric temporal windows for LTP and LTD
        - Adaptable time constants for different learning scenarios
        
        STDP is crucial for:
        - Learning temporal sequences
        - Fine-tuning synaptic connections
        - Creating time-based representations
        - Implementing reward prediction errors
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create STDP system
            >>> stdp = pynlm.STDP()
            >>> 
            >>> # Configure STDP parameters
            >>> stdp.setLearningRate(0.01)
            >>> stdp.setTauPlus(20.0)  # LTP time constant
            >>> stdp.setTauMinus(20.0)  # LTD time constant
            >>> 
            >>> # Apply correlated activity
            >>> # Pre fires, then post fires -> LTP
            >>> pre_neuron.injectCurrent(60.0)
            >>> pre_neuron.step()
            >>> 
            >>> post_neuron.injectCurrent(60.0)
            >>> post_neuron.step()
            >>> 
            >>> # Check weight change
            >>> delta = stdp.getWeightChange()
            >>> print(f"LTP: {delta:.4f}")
        
        Methods:
            __init__(): Create empty STDP system
            apply(pre_neuron, post_neuron, pre_spike, post_spike): Apply STDP
            getLearningRate(): Get learning rate
            setLearningRate(rate): Set learning rate
            getTauPlus(): Get LTP time constant
            setTauPlus(tau): Set LTP time constant
            getTauMinus(): Get LTD time constant
            setTauMinus(tau): Set LTD time constant
            getMaxWeightChange(): Get maximum weight change
            setMaxWeightChange(max): Set maximum weight change
            reset(): Reset STDP system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            learning_rate: Learning rate for STDP
            tau_plus: LTP time constant (ms)
            tau_minus: LTD time constant (ms)
            max_weight_change: Maximum weight change
            
        See Also:
            PlasticityRule, Hebbian, StructuralPlasticity, Brain
    )pbdoc")
        .def(py::init<>())
        
        // STDP parameters
        .def("getLearningRate", &STDP::getLearningRate,
             "Get learning rate")
        
        .def("setLearningRate", &STDP::setLearningRate, py::arg("rate"),
             "Set learning rate")
        
        .def("getTauPlus", &STDP::getTauPlus,
             "Get LTP time constant")
        
        .def("setTauPlus", &STDP::setTauPlus, py::arg("tau"),
             "Set LTP time constant")
        
        .def("getTauMinus", &STDP::getTauMinus,
             "Get LTD time constant")
        
        .def("setTauMinus", &STDP::setTauMinus, py::arg("tau"),
             "Set LTD time constant")
        
        .def("getMaxWeightChange", &STDP::getMaxWeightChange,
             "Get maximum weight change")\n        .def("setMaxWeightChange", &STDP::setMaxWeightChange, py::arg("max"),
             "Set maximum weight change")
        
        .def("reset", &STDP::reset,
             "Reset STDP system")
        
        .def("__repr__", [](const STDP& stdp) {
            return "<STDP: learning_rate=" + std::to_string(stdp.getLearningRate()) +
                   ", tau_plus=" + std::to_string(stdp.getTauPlus()) +
                   ", tau_minus=" + std::to_string(stddp.getTauMinus()) + ">";
        }, "String representation of STDP system");
}

void bindHebbian(py::module& m) {
    // Hebbian class - Hebbian plasticity
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(
        Hebbian plasticity for NLM brain.
        
        The Hebbian class implements Hebbian learning based on the principle
        "neurons that fire together, wire together":
        
        - Co-activation strengthens synaptic connections
        - Anti-coactivation weakens synaptic connections
        - Homosynaptic plasticity: affects all synapses on a neuron
        - Heterosynaptic plasticity: affects multiple neurons
        - Temperature-based modulation: activity-dependent regulation
        
        Hebbian learning is fundamental for:
        - Pattern recognition and association
        - Distributed representations
        - Feature extraction and binding
        - Memory consolidation
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create Hebbian system
            >>> hebbian = pynlm.Hebbian()
            >>> 
            >>> # Configure Hebbian parameters
            >>> hebbian.setLearningRate(0.005)
            >>> hebbian.setTemperatureCoefficient(0.01)
            >>> 
            >>> # Apply correlated activity
            >>> # Both pre and post fire simultaneously -> LTP
            >>> pre_neuron.injectCurrent(40.0)
            >>> post_neuron.injectCurrent(40.0)
            >>> 
            >>> pre_neuron.step()
            >>> post_neuron.step()
            >>> 
            >>> # Check weight change
            >>> delta = hebbian.getWeightChange()
            >>> print(f"Hebbian LTP: {delta:.4f}")
        
        Methods:
            __init__(): Create empty Hebbian system
            apply(pre_neuron, post_neuron, pre_spike, post_spike): Apply Hebbian learning
            getLearningRate(): Get learning rate
            setLearningRate(rate): Set learning rate
            getTemperatureCoefficient(): Get temperature coefficient
            setTemperatureCoefficient(coeff): Set temperature coefficient
            getHeterosynapticStrength(): Get heterosynaptic strength
            setHeterosynapticStrength(strength): Set heterosynaptic strength
            reset(): Reset Hebbian system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            learning_rate: Learning rate for Hebbian learning
            temperature_coefficient: Temperature coefficient
            heterosynaptic_strength: Heterosynaptic plasticity strength
            
        See Also:
            PlasticityRule, STDP, StructuralPlasticity, Brain
    )pbdoc")
        .def(py::init<>())
        
        // Hebbian parameters
        .def("getLearningRate", &Hebbian::getLearningRate,
             "Get learning rate")
        
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             "Set learning rate")
        
        .def("getTemperatureCoefficient", &Hebbian::getTemperatureCoefficient,
             "Get temperature coefficient")
        
        .def("setTemperatureCoefficient", &Hebbian::setTemperatureCoefficient,
             py::arg("coeff"),
             "Set temperature coefficient")
        
        .def("getHeterosynapticStrength", &Hebbian::getHeterosynapticStrength,
             "Get heterosynaptic strength")
        
        .def("setHeterosynapticStrength", &Hebbian::setHeterosynapticStrength,
             py::arg("strength"),
             "Set heterosynaptic strength")
        
        .def("reset", &Hebbian::reset,
             "Reset Hebbian system")
        
        .def("__repr__", [](const Hebbian& hebbian) {
            return "<Hebbian: learning_rate=" + std::to_string(hebbian.getLearningRate()) +
                   ", temperature=" + std::to_string(hebbian.getTemperatureCoefficient()) +
                   ", heterosynaptic=" + std::to_string(hebbian.getHeterosynapticStrength()) + ">";
        }, "String representation of Hebbian system");
}

void bindStructuralPlasticity(py::module& m) {
    // StructuralPlasticity class - Structural plasticity
    py::class_<StructuralPlasticity, PlasticityRule>(m, "StructuralPlasticity", R"pbdoc(
        Structural plasticity for NLM brain.
        
        The StructuralPlasticity class implements structural plasticity based on
        activity-dependent growth and pruning of neural connections:
        
        - Synaptogenesis: Formation of new synaptic connections
        - Pruning: Elimination of weak or unused synapses
        - Dendritic spine growth and retraction
        - Axonal branching and guidance
        - Activity-dependent structural changes
        
        Structural plasticity enables the brain to:
        - Adapt its physical structure
        - Optimize network topology
        - Support memory allocation
        - Maintain metabolic efficiency
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create structural plasticity system
            >>> structural = pynlm.StructuralPlasticity()
            >>> 
            >>> # Configure structural parameters
            >>> structural.setSynaptogenesisRate(0.001)
            >>> structural.setPruningRate(0.0001)
            >>> structural.setBranchingRate(0.005)
            >>> 
            >>> # Apply structural plasticity
            >>> # High activity -> synaptogenesis
            >>> for i in range(100):
            ...     # Strong correlated activity
            ...     pre_neuron.injectCurrent(60.0)
            ...     post_neuron.injectCurrent(60.0)
            ...     
            ...     pre_neuron.step()
            ...     post_neuron.step()
            ...     
            >>> # Low activity -> pruning
            >>> for i in range(50):
            ...     # Low correlated activity
            ...     pre_neuron.step()
            ...     post_neuron.step()
            ... 
            >>> 
            >>> # Check structural changes
            >>> synapse_count = structural.getSynapseCount()
            >>> spine_count = structural.getDendriticSpineCount()
        
        Methods:
            __init__(): Create empty structural plasticity system
            apply(pre_neuron, post_neuron, pre_spike, post_spike): Apply structural plasticity
            getSynaptogenesisRate(): Get synaptogenesis rate
            setSynaptogenesisRate(rate): Set synaptogenesis rate
            getPruningRate(): Get pruning rate
            setPruningRate(rate): Set pruning rate
            getBranchingRate(): Get branching rate
            setBranchingRate(rate): Set branching rate
            getSpineGrowthRate(): Get spine growth rate
            setSpineGrowthRate(rate): Set spine growth rate
            getSynapseCount(): Get current synapse count
            getDendriticSpineCount(): Get dendritic spine count
            getAxonalBranchCount(): Get axonal branch count
            reset(): Reset structural plasticity system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            synaptogenesis_rate: Rate of new synapse formation
            pruning_rate: Rate of synapse elimination
            branching_rate: Rate of axonal branching
            spine_growth_rate: Rate of dendritic spine growth
            
        See Also:
            PlasticityRule, Brain, Synaptogenesis, Pruning
    )pbdoc")
        .def(py::init<>())
        
        // Structural parameters
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate,
             py::arg("rate"),
             "Set synaptogenesis rate")
        
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
             "Get pruning rate")
        
        .def("setPruningRate", &StructuralPlasticity::setPruningRate,
             py::arg("rate"),
             "Set pruning rate")
        
        .def("getBranchingRate", &StructuralPlasticity::getBranchingRate,
             "Get branching rate")
        
        .def("setBranchingRate", &StructuralPlasticity::setBranchingRate,
             py::arg("rate"),
             "Set branching rate")
        
        .def("getSpineGrowthRate", &StructuralPlasticity::getSpineGrowthRate,
             "Get spine growth rate")
        
        .def("setSpineGrowthRate", &StructuralPlasticity::setSpineGrowthRate,
             py::arg("rate"),
             "Set spine growth rate")
        
        .def("getSynapseCount", &StructuralPlasticity::getSynapseCount,
             "Get current synapse count")
        
        .def("getDendriticSpineCount", &StructuralPlasticity::getDendriticSpineCount,
             "Get dendritic spine count")
        
        .def("getAxonalBranchCount", &StructuralPlasticity::getAxonalBranchCount,
             "Get axonal branch count")
        
        .def("reset", &StructuralPlasticity::reset,
             "Reset structural plasticity system")
        
        .def("__repr__", [](const StructuralPlasticity& structural) {
            return "<StructuralPlasticity: synaptogenesis=" + std::to_string(structural.getSynaptogenesisRate()) +
                   ", pruning=" + std::to_string(structural.getPruningRate()) +
                   ", branching=" + std::to_string(structural.getBranchingRate()) + ">";
        }, "String representation of structural plasticity system");
}

void bindDevelopmentSystem(py::module& m) {
    // DevelopmentSystem class - Neural development
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(
        Neural development and maturation system for NLM brain.
        
        The DevelopmentSystem class implements neural development and maturation
        based on activity-dependent and genetically programmed processes:
        
        - Critical periods: Periods of high plasticity
        - Maturation: Gradual development of neural capabilities
        - Synaptogenesis: Formation of initial connections
        - Pruning: Elimination of excess connections
        - Myelination: Insulation of neural pathways
        - Synapse density regulation: Maintaining optimal connectivity
        
        Development enables the brain to:
        - Grow from primitive state to mature network
        - Adapt development based on experience
        - Maintain optimal connectivity
        - Support age-dependent changes
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create development system
            >>> development = pynlm.DevelopmentSystem()
            >>> 
            >>> # Configure development parameters
            >>> development.setCriticalPeriodDuration(100.0)
            >>> development.setMaturationRate(0.01)
            >>> development.setSynaptogenesisRate(0.1)
            >>> development.setPruningRate(0.05)
            >>> 
            >>> # Simulate development
            >>> for step in range(500):
            ...     # Apply developmental changes
            ...     development.update(step, 0.1)
            ...     
            ...     # Update brain development
            ...     brain.updateDevelopment(0.1)
            ...     
            ...     # Check developmental stage
            ...     stage = brain.getDevelopmentalStage()
            ...     
            ...     if stage == pynlm.DevelopmentalStage.Adult:
            ...         print("Brain reached adult stage!")
            ...         break
        
        Methods:
            __init__(): Create empty development system
            update(step, dt): Update development system
            getDevelopmentalStage(): Get current developmental stage
            getCriticalPeriodDuration(): Get critical period duration
            setCriticalPeriodDuration(duration): Set critical period duration
            getMaturationRate(): Get maturation rate
            setMaturationRate(rate): Set maturation rate
            getSynaptogenesisRate(): Get synaptogenesis rate
            setSynaptogenesisRate(rate): Set synaptogenesis rate
            getPruningRate(): Get pruning rate
            setPruningRate(rate): Set pruning rate
            isInCriticalPeriod(): Check if in critical period
            getDevelopmentalAge(): Get developmental age
            getPlasticityModifier(): Get plasticity modifier
            getStructuralPlasticityRate(): Get structural plasticity rate
            setStructuralPlasticityRate(rate): Set structural plasticity rate
            reset(): Reset development system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            developmental_stage: Current developmental stage
            critical_period_duration: Critical period duration
            maturation_rate: Rate of maturation
            synaptogenesis_rate: Rate of synaptogenesis
            pruning_rate: Rate of pruning
            
        See Also:
            Brain, Synaptogenesis, Pruning, Maturation, DevelopmentalStage
    )pbdoc")
        .def(py::init<>())
        
        // Development updates
        .def("update", &DevelopmentSystem::update,
             py::arg("step"), py::arg("dt"),
             "Update development system")
        
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Development parameters
        .def("getCriticalPeriodDuration", &DevelopmentSystem::getCriticalPeriodDuration,
             "Get critical period duration")
        
        .def("setCriticalPeriodDuration", &DevelopmentSystem::setCriticalPeriodDuration,
             py::arg("duration"),
             "Set critical period duration")
        
        .def("getMaturationRate", &DevelopmentSystem::getMaturationRate,
             "Get maturation rate")
        
        .def("setMaturationRate", &DevelopmentSystem::setMaturationRate,
             py::arg("rate"),
             "Set maturation rate")
        
        .def("getSynaptogenesisRate", &DevelopmentSystem::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        
        .def("setSynaptogenesisRate", &DevelopmentSystem::setSynaptogenesisRate,
             py::arg("rate"),
             "Set synaptogenesis rate")
        
        .def("getPruningRate", &DevelopmentSystem::getPruningRate,
             "Get pruning rate")
        
        .def("setPruningRate", &DevelopmentSystem::setPruningRate,
             py::arg("rate"),
             "Set pruning rate")
        
        // Development status
        .def("isInCriticalPeriod", &DevelopmentSystem::isInCriticalPeriod,
             "Check if in critical period")
        
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier")
        
        .def("getStructuralPlasticityRate", &DevelopmentSystem::getStructuralPlasticityRate,
             "Get structural plasticity rate")
        
        .def("setStructuralPlasticityRate", &DevelopmentSystem::setStructuralPlasticityRate,
             py::arg("rate"),
             "Set structural plasticity rate")
        
        .def("reset", &DevelopmentSystem::reset,
             "Reset development system")
        
        .def("__repr__", [](const DevelopmentSystem& development) {
            return "<DevelopmentSystem: stage=" + std::to_string(static_cast<int>(development.getDevelopmentalStage())) +
                   ", age=" + std::to_string(development.getDevelopmentalAge()) + ">";
        }, "String representation of development system");
}

void bindSynaptogenesis(py::module& m) {
    // Synaptogenesis class - Synaptic formation
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(
        Synaptic formation for NLM brain.
        
        The Synaptogenesis class implements synaptic formation based on
        activity-dependent and activity-independent processes:
        
        - Spontaneous synaptogenesis: Initial connection formation
        - Activity-dependent synaptogenesis: Strengthening connections through use
        - Activity-independent synaptogenesis: Genetic programming of connections
        - Synaptic specificity: Targeted connection formation
        - Competition for resources: Limited resources among synapses
        
        Synaptogenesis enables the brain to:
        - Build initial neural networks
        - Establish functional connectivity
        - Adapt connectivity based on experience
        - Maintain optimal network architecture
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create synaptogenesis system
            >>> synaptogenesis = pynlm.Synaptogenesis()
            >>> 
            >>> # Configure synaptogenesis parameters
            >>> synaptogenesis.setSpontaneousRate(0.1)
            >>> synaptogenesis.setActivityDependentRate(0.05)
            >>> synaptogenesis.setTargetConnectionDensity(0.2)
            >>> 
            >>> # Simulate synaptogenesis
            >>> for step in range(200):
            ...     # Apply synaptogenesis
            ...     synaptogenesis.update(step, 0.1)
            ...     
            ...     # Update brain synaptogenesis
            ...     brain.updateSynaptogenesis(0.1)
            ...     
            >>> 
            >>> # Check synapse formation
            >>> synapse_count = synaptogenesis.getSynapseCount()
            >>> connection_density = synaptogenesis.getConnectionDensity()
        
        Methods:
            __init__(): Create empty synaptogenesis system
            update(step, dt): Update synaptogenesis system
            getSynapseCount(): Get current synapse count
            getSpontaneousRate(): Get spontaneous synaptogenesis rate
            setSpontaneousRate(rate): Set spontaneous synaptogenesis rate
            getActivityDependentRate(): Get activity-dependent rate
            setActivityDependentRate(rate): Set activity-dependent rate
            getTargetConnectionDensity(): Get target connection density
            setTargetConnectionDensity(density): Set target connection density
            getConnectionDensity(): Get current connection density
            getSynapticSpecification(): Get synaptic specification
            setSynapticSpecification(spec): Set synaptic specification
            reset(): Reset synaptogenesis system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            synapse_count: Current number of synapses
            spontaneous_rate: Spontaneous synaptogenesis rate
            activity_dependent_rate: Activity-dependent synaptogenesis rate
            
        See Also:
            DevelopmentSystem, StructuralPlasticity, Pruning, Brain
    )pbdoc")
        .def(py::init<>())
        
        // Synaptogenesis updates
        .def("update", &Synaptogenesis::update,
             py::arg("step"), py::arg("dt"),
             "Update synaptogenesis system")
        
        .def("getSynapseCount", &Synaptogenesis::getSynapseCount,
             "Get current synapse count")
        
        // Synaptogenesis parameters
        .def("getSpontaneousRate", &Synaptogenesis::getSpontaneousRate,
             "Get spontaneous synaptogenesis rate")
        
        .def("setSpontaneousRate", &Synaptogenesis::setSpontaneousRate,
             py::arg("rate"),
             "Set spontaneous synaptogenesis rate")
        
        .def("getActivityDependentRate", &Synaptogenesis::getActivityDependentRate,
             "Get activity-dependent rate")
        
        .def("setActivityDependentRate", &Synaptogenesis::setActivityDependentRate,
             py::arg("rate"),
             "Set activity-dependent rate")
        
        .def("getTargetConnectionDensity", &Synaptogenesis::getTargetConnectionDensity,
             "Get target connection density")
        
        .def("setTargetConnectionDensity", &Synaptogenesis::setTargetConnectionDensity,
             py::arg("density"),
             "Set target connection density")
        
        .def("getConnectionDensity", &Synaptogenesis::getConnectionDensity,
             "Get current connection density")
        
        .def("getSynapticSpecification", &Synaptogenesis::getSynapticSpecification,
             "Get synaptic specification")
        
        .def("setSynapticSpecification", &Synaptogenesis::setSynapticSpecification,
             py::arg("spec"),
             "Set synaptic specification")
        
        .def("reset", &Synaptogenesis::reset,
             "Reset synaptogenesis system")
        
        .def("__repr__", [](const Synaptogenesis& synaptogenesis) {
            return "<Synaptogenesis: synapses=" + std::to_string(synaptogenesis.getSynapseCount()) +
                   ", spontaneous=" + std::to_string(synaptogenesis.getSpontaneousRate()) +
                   ", activity_dependent=" + std::to_string(synaptogenesis.getActivityDependentRate()) + ">";
        }, "String representation of synaptogenesis system");
}

void bindPruning(py::module& m) {
    // Pruning class - Synaptic pruning
    py::class_<Pruning>(m, "Pruning", R"pbdoc(
        Synaptic pruning for NLM brain.
        
        The Pruning class implements synaptic pruning based on activity-dependent
        and activity-independent processes:
        
        - Activity-dependent pruning: Weak connections eliminated through use
        - Activity-independent pruning: Genetic programming of connections
        - Competition-based pruning: Limited resources among synapses
        - Critical period pruning: Focused elimination during critical periods
        - Homeostatic pruning: Maintaining network stability
        
        Pruning enables the brain to:
        - Eliminate unnecessary connections
        - Optimize network efficiency
        - Maintain optimal connectivity
        - Support developmental changes
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create pruning system
            >>> pruning = pynlm.Pruning()
            >>> 
            >>> # Configure pruning parameters
            >>> pruning.setPruningRate(0.05)
            >>> pruning.setActivityThreshold(0.1)
            >>> pruning.setCriticalPeriodPruning(True)
            >>> pruning.setHomeostaticPruning(True)
            >>> 
            >>> # Simulate pruning
            >>> for step in range(300):
            ...     # Apply pruning
            ...     pruning.update(step, 0.1)
            ...     
            ...     # Update brain pruning
            ...     brain.updatePruning(0.1)
            ...     
            >>> 
            >>> # Check pruning results
            >>> synapse_count = pruning.getSynapseCount()
            >>> pruned_count = pruning.getPrunedSynapseCount()
            >>> activity_threshold = pruning.getActivityThreshold()
        
        Methods:
            __init__(): Create empty pruning system
            update(step, dt): Update pruning system
            getSynapseCount(): Get current synapse count
            getPruningRate(): Get pruning rate
            setPruningRate(rate): Set pruning rate
            getActivityThreshold(): Get activity threshold
            setActivityThreshold(threshold): Set activity threshold
            getCriticalPeriodPruning(): Get critical period pruning flag
            setCriticalPeriodPruning(enable): Set critical period pruning
            getHomeostaticPruning(): Get homeostatic pruning flag
            setHomeostaticPruning(enable): Set homeostatic pruning
            getPrunedSynapseCount(): Get pruned synapse count
            getPrunedFraction(): Get pruned fraction
            getPruningEfficiency(): Get pruning efficiency
            reset(): Reset pruning system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            synapse_count: Current number of synapses
            pruning_rate: Pruning rate
            activity_threshold: Activity threshold for pruning
            
        See Also:
            DevelopmentSystem, StructuralPlasticity, Synaptogenesis, Brain
    )pbdoc")
        .def(py::init<>())
        
        // Pruning updates
        .def("update", &Pruning::update,
             py::arg("step"), py::arg("dt"),
             "Update pruning system")
        
        .def("getSynapseCount", &Pruning::getSynapseCount,
             "Get current synapse count")
        
        // Pruning parameters
        .def("getPruningRate", &Pruning::getPruningRate,
             "Get pruning rate")
        
        .def("setPruningRate", &Pruning::setPruningRate,
             py::arg("rate"),
             "Set pruning rate")
        
        .def("getActivityThreshold", &Pruning::getActivityThreshold,
             "Get activity threshold")
        
        .def("setActivityThreshold", &Pruning::setActivityThreshold,
             py::arg("threshold"),
             "Set activity threshold")
        
        .def("getCriticalPeriodPruning", &Pruning::getCriticalPeriodPruning,
             "Get critical period pruning flag")
        
        .def("setCriticalPeriodPruning", &Pruning::setCriticalPeriodPruning,
             py::arg("enable"),
             "Set critical period pruning")
        
        .def("getHomeostaticPruning", &Pruning::getHomeostaticPruning,
             "Get homeostatic pruning flag")
        
        .def("setHomeostaticPruning", &Pruning::setHomeostaticPruning,
             py::arg("enable"),
             "Set homeostatic pruning")
        
        .def("getPrunedSynapseCount", &Pruning::getPrunedSynapseCount,
             "Get pruned synapse count")
        
        .def("getPrunedFraction", &Pruning::getPrunedFraction,
             "Get pruned fraction")
        
        .def("getPruningEfficiency", &Pruning::getPruningEfficiency,
             "Get pruning efficiency")
        
        .def("reset", &Pruning::reset,
             "Reset pruning system")
        
        .def("__repr__", [](const Pruning& pruning) {
            return "<Pruning: synapses=" + std::to_string(pruning.getSynapseCount()) +
                   ", rate=" + std::to_string(pruning.getPruningRate()) +
                   ", activity_threshold=" + std::to_string(pruning.getActivityThreshold()) + ">";
        }, "String representation of pruning system");
}

void bindMaturation(py::module& m) {
    // Maturation class - Neural maturation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(
        Neural maturation for NLM brain.
        
        The Maturation class implements neural maturation and development
        based on activity-dependent and activity-independent processes:
        
        - Synaptic strengthening: Enhanced connections through activity
        - Dendritic growth: Increased dendritic complexity
        - Axonal elongation: Extended neural connections
        - Myelination: Insulating axons for faster transmission
        - Synapse elimination: Removing unused connections
        - Structural reorganization: Reconfiguring neural architecture
        
        Maturation enables the brain to:
        - Develop functional capabilities
        - Optimize neural efficiency
        - Support behavioral specialization
        - Adapt to environmental demands
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create maturation system
            >>> maturation = pynlm.Maturation()
            >>> 
            >>> # Configure maturation parameters
            >>> maturation.setMaturationRate(0.02)
            >>> maturation.setMyelinationRate(0.01)
            >>> maturation.setDendriticGrowthRate(0.03)
            >>> maturation.setSynapticOptimizationRate(0.02)
            >>> 
            >>> # Simulate maturation
            >>> for step in range(400):
            ...     # Apply maturation
            ...     maturation.update(step, 0.1)
            ...     
            ...     # Update brain maturation
            ...     brain.updateMaturation(0.1)
            ...     
            ...     # Check developmental progress
            ...     stage = brain.getDevelopmentalStage()
            ...     if stage == pynlm.DevelopmentalStage.Maturation:
            ...         print(f"Brain in maturation stage: {stage}")
            ...     elif stage == pynlm.DevelopmentalStage.Adult:
            ...         print("Brain reached adult stage!")
            ...         break
        
        Methods:
            __init__(): Create empty maturation system
            update(step, dt): Update maturation system
            getDevelopmentalStage(): Get current developmental stage
            getMaturationRate(): Get maturation rate
            setMaturationRate(rate): Set maturation rate
            getMyelinationRate(): Get myelination rate
            setMyelinationRate(rate): Set myelination rate
            getDendriticGrowthRate(): Get dendritic growth rate
            setDendriticGrowthRate(rate): Set dendritic growth rate
            getSynapticOptimizationRate(): Get synaptic optimization rate
            setSynapticOptimizationRate(rate): Set synaptic optimization rate
            getMaturationProgress(): Get maturation progress
            getFunctionalMaturation(): Get functional maturation status
            getStructuralMaturation(): Get structural maturation status
            reset(): Reset maturation system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
            
        Attributes:
            developmental_stage: Current developmental stage
            maturation_rate: Maturation rate
            myelination_rate: Myelination rate
            dendritic_growth_rate: Dendritic growth rate
            
        See Also:
            DevelopmentSystem, Synaptogenesis, Pruning, Brain
    )pbdoc")
        .def(py::init<>())
        
        // Maturation updates
        .def("update", &Maturation::update,
             py::arg("step"), py::arg("dt"),
             "Update maturation system")
        
        .def("getDevelopmentalStage", &Maturation::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Maturation parameters
        .def("getMaturationRate", &Maturation::getMaturationRate,
             "Get maturation rate")
        
        .def("setMaturationRate", &Maturation::setMaturationRate,
             py::arg("rate"),
             "Set maturation rate")
        
        .def("getMyelinationRate", &Maturation::getMyelinationRate,
             "Get myelination rate")
        
        .def("setMyelinationRate", &Maturation::setMyelinationRate,
             py::arg("rate"),
             "Set myelination rate")
        
        .def("getDendriticGrowthRate", &Maturation::getDendriticGrowthRate,
             "Get dendritic growth rate")
        
        .def("setDendriticGrowthRate", &Maturation::setDendriticGrowthRate,
             py::arg("rate"),
             "Set dendritic growth rate")
        
        .def("getSynapticOptimizationRate", &Maturation::getSynapticOptimizationRate,
             "Get synaptic optimization rate")
        
        .def("setSynapticOptimizationRate", &Maturation::setSynapticOptimizationRate,
             py::arg("rate"),
             "Set synaptic optimization rate")
        
        .def("getMaturationProgress", &Maturation::getMaturationProgress,
             "Get maturation progress")
        
        .def("getFunctionalMaturation", &Maturation::getFunctionalMaturation,
             "Get functional maturation status")
        
        .def("getStructuralMaturation", &Maturation::getStructuralMaturation,
             "Get structural maturation status")
        
        .def("reset", &Maturation::reset,
             "Reset maturation system")
        
        .def("__repr__", [](const Maturation& maturation) {
            return "<Maturation: stage=" + std::to_string(static_cast<int>(maturation.getDevelopmentalStage())) +
                   ", progress=" + std::to_string(maturation.getMaturationProgress()) + ">";
        }, "String representation of maturation system");
}
