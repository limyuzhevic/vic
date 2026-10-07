// Memory system bindings

#include "bindings.h"

namespace nlm {

void bind_memory_module(py::module_& m) {
    // NeuralWorkingMemory class
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(
        Working memory system for temporary active information storage.
        
        This system provides:
        - Store/retrieve key-value pairs by neuron ID
        - Capacity-limited storage with automatic decay
        - Efficient lookup for active working sets
        - Integration with neural activity patterns
        
        Key mechanisms:
        - Persistent neural activity (reverberating activity)
        - Recurrent synaptic connections for maintaining information
        - Activity-dependent short-term plasticity
        - Neural competition for selective retention
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory (encoded as neural activity pattern)")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"), "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             py::return_value_policy::reference_internal, "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition, "Competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"), "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity, "Get overall memory activity level");

    // NeuralEpisodicMemory class
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(
        Episodic memory system for experience storage and replay.
        
        This system provides:
        - Store/retrieve episodic experiences
        - Replay during simulation for consolidation
        - Integration with working memory
        - Importance-based prioritization
        
        Key mechanisms:
        - Temporal context encoding
        - Pattern completion for retrieval
        - Replay during rest/sleep phases
        - Synaptic consolidation through replay
    )pbdoc")
        .def(py::init<>())
        .def("storeExperience", &NeuralEpisodicMemory::storeExperience,
             py::arg("state"), py::arg("action"), py::arg("reward"), py::arg("nextState"),
             "Store an episodic experience")
        .def("retrieveExperience", &NeuralEpisodicMemory::retrieveExperience,
             py::arg("state"), "Retrieve similar experiences")
        .def("replay", &NeuralEpisodicMemory::replay, "Replay important episodes for consolidation")
        .def("getExperienceCount", &NeuralEpisodicMemory::getExperienceCount, "Get number of stored experiences")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodic memories")
        .def("getMostImportant", &NeuralEpisodicMemory::getMostImportant,
             py::return_value_policy::reference_internal, "Get most important experiences");

    // SemanticMemory class
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(
        Semantic memory system for knowledge and concepts.
        
        This system provides:
        - Store/retrieve facts and general knowledge
        - Concept formation from patterns
        - Rule-based reasoning
        - Abstraction levels
        
        Key mechanisms:
        - Pattern association and generalization
        - Hierarchical knowledge organization
        - Inference and prediction
        - Integration with episodic memory
    )pbdoc")
        .def(py::init<>())
        .def("storeConcept", &SemanticMemory::storeConcept, py::arg("pattern"), py::arg("concept"),
             "Store a pattern-concept association")
        .def("retrieveConcept", &SemanticMemory::retrieveConcept, py::arg("pattern"),
             "Retrieve concepts for given pattern")
        .def("getConcept", &SemanticMemory::getConcept, py::arg("name"),
             py::return_value_policy::reference_internal, "Get specific concept by name")
        .def("listConcepts", &SemanticMemory::listConcepts, "List all stored concepts")
        .def("clear", &SemanticMemory::clear, "Clear all semantic memory");

    // ProceduralMemory class
    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(
        Procedural memory system for skills and behaviors.
        
        This system provides:
        - Learn and store action sequences
        - Skill acquisition and refinement
        - Motor pattern memory
        - Reinforcement learning integration
        
        Key mechanisms:
        - Hebbian learning for action patterns
        - Gradual skill refinement
        - Context-dependent retrieval
        - Error-based optimization
    )pbdoc")
        .def(py::init<>())
        .def("storeSkill", &ProceduralMemory::storeSkill, py::arg("actionSequence"), py::arg("quality"),
             "Store a skill (action sequence with quality)")
        .def("retrieveSkill", &ProceduralMemory::retrieveSkill, py::arg("context"),
             "Retrieve best skill for given context")
        .def("improveSkill", &ProceduralMemory::improveSkill, py::arg("actionSequence"),
             "Improve existing skill based on performance")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("actionSequence"),
             py::return_value_policy::reference_internal, "Get specific skill")
        .def("getAllSkills", &ProceduralMemory::getAllSkills, "List all stored skills")
        .def("clear", &ProceduralMemory::clear, "Clear all procedural memory");

    // AssociativeMemory class
    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(
        Associative memory system for pattern associations.
        
        This system provides:
        - Store/retrieve pattern associations
        - Hebbian learning for associations
        - Pattern completion
        - Sparse connectivity for efficiency
        
        Key mechanisms:
        - Bidirectional associations
        - Competitive learning
        - Sparse encoding
        - Rapid association recall
    )pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("pattern1"), py::arg("pattern2"),
             "Create association between two patterns")
        .def("retrieve", &AssociativeMemory::retrieve, py::arg("pattern"),
             "Retrieve associated patterns")
        .def("removeAssociation", &AssociativeMemory::removeAssociation, py::arg("pattern1"), py::arg("pattern2"),
             "Remove association between patterns")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("pattern"),
             py::return_value_policy::reference_internal, "Get all associations for a pattern")
        .def("clear", &AssociativeMemory::clear, "Clear all associations");
}

} // namespace nlm