// Python bindings for NLM Phase 6 Integration
// This provides comprehensive Python API for the integrated artificial brain

#include "bindings.cpp"
#include <pybind11/pybind11.h>

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM - Neural Learning Machine (熙然)\n"
              "An experimental artificial developmental brain implementing Phase 6: Final Integration.\n\n"
              "Key features:\n"
              "- Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics\n"
              "- Multiple memory systems: Working, Episodic, and Associative memory\n"
              "- Neuromodulation: Dopamine, Curiosity, Novelty, Prediction Error\n"
              "- Cognitive systems: Planning, Attention, Concept Formation\n"
              "- Development: Age-dependent plasticity stages\n"
              "- Learning: STDP, Hebbian, and reward-modulated plasticity\n"
              "- World interaction through agents\n\n"
              "The brain operates as a closed-loop system:\n"
              "SENSORY INPUT → NEURAL PROCESSING → MEMORY/PREDICTION →\n"
              "NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD/PRECISION → PLASTICITY → CHANGED BRAIN → CHANGED BEHAVIOR\n";

    // Register the brain core module
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step)
        .def("step", [](nlm::Brain &self, size_t step) {
            self.step(step, step * 0.001);
        })
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput)
        .def("injectCurrent", &nlm::Brain::injectCurrent)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount)
        .def("produceAction", &nlm::Brain::produceAction)
        .def("applyNeuromodulation", &nlm::Brain::applyNeuromodulation)
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory)
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &nlm::Brain::getAssociativeMemory)
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem)
        .def("getPlanner", &nlm::Brain::getPlanner)
        .def("getConceptFormation", &nlm::Brain::getConceptFormation)
        .def("getAttention", &nlm::Brain::getAttention)
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem)
        .def("getDopamine", &nlm::Brain::getDopamine)
        .def("getCuriosity", &nlm::Brain::getCuriosity)
        .def("getNovelty", &nlm::Brain::getNovelty)
        .def("getPredictionErrorSignal", &nlm::Brain::getPredictionErrorSignal)
        .def("getConfig", &nlm::Brain::getConfig)
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator)
        .def("getSpikeSystem", &nlm::Brain::getSpikeSystem)
        .def("getSTDP", &nlm::Brain::getSTDP)
        .def("getHebbian", &nlm::Brain::getHebbian)
        .def("getStructuralPlasticity", &nlm::Brain::getStructuralPlasticity)
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage)
        .def("reset", &nlm::Brain::reset)
        .def("save", &nlm::Brain::save)
        .def("load", &nlm::Brain::load)
        .def("addRegion", &nlm::Brain::addRegion)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("getRegionCount", &nlm::Brain::getRegionCount)
        .def("getRegionIds", &nlm::Brain::getRegionIds)
        .def("logStatus", &nlm::Brain::logStatus)
        .def("step", &nlm::Brain::step);

    // Register the agent brain interface
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("reset", &nlm::AgentBrain::reset)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel);

    // Register the world environment
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand)
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody)
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy)
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed)
        .def("isDone", &nlm::SimpleWorld::isDone)
        .def("getState", &nlm::SimpleWorld::getState)
        .def("getLastReward", &nlm::SimpleWorld::getLastReward)
        .def("getActionSpaceSize", &nlm::SimpleWorld::getActionSpaceSize)
        .def("getObservationSpaceSize", &nlm::SimpleWorld::getObservationSpaceSize);

    // Register the sensory percept
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def_readwrite("vision", &nlm::SensoryPercept::vision)
        .def_readwrite("touch", &nlm::SensoryPercept::touch)
        .def_readwrite("internal", &nlm::SensoryPercept::internal)
        .def_readwrite("proprioception", &nlm::SensoryPercept::proprioception)
        .def("getVisionSize", &nlm::SensoryPercept::getVisionSize)
        .def("getTouchSize", &nlm::SensoryPercept::getTouchSize)
        .def("getInternalSize", &nlm::SensoryPercept::getInternalSize)
        .def("getProprioceptionSize", &nlm::SensoryPercept::getProprioceptionSize)
        .def("getTotalSize", &nlm::SensoryPercept::getTotalSize);

    // Register the action type
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("Wait", nlm::ActionType::Wait)
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("LookLeft", nlm::ActionType::LookLeft)
        .value("LookRight", nlm::ActionType::LookRight)
        .export_values();

    // Register the developmental stage
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    // Register the neuron type
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    // Register the world object type
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // Create factory functions
    m.def("createBrain", &nlm::createBrain, "Create a brain with default configuration");
    m.def("createDefaultConfig", &nlm::createDefaultConfig, "Create default configuration");
    m.def("createSimpleWorld", &nlm::createSimpleWorld, "Create a simple world environment");
    m.def("createAgentBrain", &nlm::createAgentBrain, "Create an agent brain interface");
    m.def("createAgentBody", &nlm::createAgentBody, "Create an agent body");

    // Add version and information
    m.attr("__version__") = "Phase 6.0 - Final Integration";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Neural Learning Machine - An experimental artificial developmental brain";
}

// Export the init function for module initialization
PYBIND11_PLUGIN_API void init_pynlm(py::module_ &m) {
    init_python_bindings(m);
}
