// Development system bindings

#include "bindings.h"

namespace nlm {

void bind_development_module(py::module_& m) {
    // DevelopmentSystem class
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(
        Development system orchestrating neural development.
        
        The development system manages:
        - Developmental stages (Initial, CriticalPeriod, Maturation, Adult, Aging)
        - Plasticity modulation over time
        - Structural changes and synaptic reorganization
        - Age-dependent learning capabilities
        
        Key features:
        - Stage-based plasticity modulation
        - Critical period detection
        - Synaptogenesis (formation of new connections)
        - Pruning (elimination of weak connections)
        - Maturation (refinement and specialization)
        - Aging effects on neural function
    )pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, SimulationStep)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"), "Update development (simple version)")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if critical period is active")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress (0.0 to 1.0)")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age (in simulation time)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    // Synaptogenesis class
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(
        Synaptogenesis system for forming new neural connections.
        
        Synaptogenesis provides:
        - Formation of new synaptic connections
        - Pattern-specific connection formation
        - Activity-dependent synaptogenesis
        - Structural network growth
        
        Key features:
        - Form connections between neurons
        - Use activity patterns to guide connection
        - Create recurrent connections
        - Support network expansion
    )pbdoc")
        .def(py::init<>())
        .def("addConnection", &Synaptogenesis::addConnection, py::arg("preNeuron"), py::arg("postNeuron"),
             py::arg("initialWeight") = 1.0f, "Add a new synaptic connection")
        .def("strengthenConnection", &Synaptogenesis::strengthenConnection, py::arg("preNeuron"), py::arg("postNeuron"),
             py::arg("factor") = 1.0f, "Strengthen existing connection")
        .def("weakenConnection", &Synaptogenesis::weakenConnection, py::arg("preNeuron"), py::arg("postNeuron"),
             py::arg("factor") = 1.0f, "Weaken existing connection")
        .def("removeConnection", &Synaptogenesis::removeConnection, py::arg("preNeuron"), py::arg("postNeuron"),
             "Remove synaptic connection")
        .def("getConnectionCount", &Synaptogenesis::getConnectionCount, "Get total number of connections")
        .def("getConnectionsForNeuron", &Synaptogenesis::getConnectionsForNeuron, py::arg("neuron"),
             py::return_value_policy::reference_internal, "Get connections for specific neuron")
        .def("getTotalConnectionWeight", &Synaptogenesis::getTotalConnectionWeight, "Get total synaptic weight")
        .def("clear", &Synaptogenesis::clear, "Clear all connections");

    // Maturation class
    py::class_<Maturation>(m, "Maturation", R"pbdoc(
        Maturation system for neural refinement and specialization.
        
        Maturation provides:
        - Refinement of existing connections
        - Specialization of neuron functions
        - Balance optimization
        - Efficiency improvements
        
        Key features:
        - Optimize synaptic weights
        - Specialize neuron types
        - Refine tuning curves
        - Improve computational efficiency
    )pbdoc")
        .def(py::init<>())
        .def("refineConnections", &Maturation::refineConnections,
             py::arg("neurons"), py::arg("regions"), "Refine synaptic connections")
        .def("specializeNeuron", &Maturation::specializeNeuron, py::arg("neuron"), py::arg("type"),
             "Specialize neuron function and properties")
        .def("optimizeTuning", &Maturation::optimizeTuning, "Optimize tuning curves")
        .def("getSpecializations", &Maturation::getSpecializations, py::return_value_policy::reference_internal,
             "Get list of specialized neurons")
        .def("getRefinementProgress", &Maturation::getRefinementProgress, "Get refinement progress (0.0 to 1.0)")
        .def("clear", &Maturation::clear, "Clear all specializations");

    // Pruning class
    py::class_<Pruning>(m, "Pruning", R"pbdoc(
        Pruning system for eliminating weak neural connections.
        
        Pruning provides:
        - Elimination of weak synaptic connections
        - Network efficiency improvement
        - Noise reduction
        - Memory consolidation
        
        Key features:
        - Remove unused connections
        - Eliminate connections with low activity
        - Consolidate important memories
        - Improve computational efficiency
    )pbdoc")
        .def(py::init<>())
        .def("pruneConnections", &Pruning::pruneConnections, py::arg("activityThreshold") = 0.1f,
             "Prune connections below activity threshold")
        .def("consolidateMemory", &Pruning::consolidateMemory,
             py::arg("memoryImportanceThreshold") = 0.8f, "Consolidate important memories")
        .def("getPrunedCount", &Pruning::getPrunedCount, "Get number of connections pruned")
        .def("getConsolidatedCount", &Pruning::getConsolidatedCount, "Get number of memories consolidated")
        .def("clear", &Pruning::clear, "Clear pruning statistics");
}

} // namespace nlm