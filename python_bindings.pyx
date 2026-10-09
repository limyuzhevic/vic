#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/nlm/Brain.hpp"
#include "../include/nlm/AgentBrain.hpp"
#include "../include/nlm/SimpleWorld.hpp"
#include "../include/nlm/Config.hpp"
#include "../include/nlm/Types.hpp"
#include "../include/nlm/Memory.hpp"
#include "../include/nlm/PredictionSystem.hpp"
#include "../include/nlm/Cognition.hpp"
#include "../include/nlm/Neuromodulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM (熙然) - Neural Learning Machine Python bindings";
    
    // Configuration
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set)
        .def("get", &nlm::Config::get)
        .def("has", &nlm::Config::has)
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("getKeys", &nlm::Config::getKeys);
    
    m.def("createDefaultConfig", &nlm::createDefaultConfig);
    
    // Brain class
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step))
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step))
        .def("reset", &nlm::Brain::reset)
        .def("save", &nlm::Brain::save)
        .def("load", &nlm::Brain::load)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount)
        .def("produceAction", &nlm::Brain::produceAction)
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput)
        .def("injectCurrent", &nlm::Brain::injectCurrent)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons)
        .def("getRegionCount", &nlm::Brain::getRegionCount)
        .def("addRegion", &nlm::Brain::addRegion)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("getRegions", &nlm::Brain::getRegions)
        .def("getRegionIds", &nlm::Brain::getRegionIds)
        .def("addInterRegionConnection", &nlm::Brain::addInterRegionConnection)
        .def("removeInterRegionConnection", &nlm::Brain::removeInterRegionConnection)
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
        .def("getDevelopmentStage", &nlm::Brain::getDevelopmentalStage)
        .def("setDevelopmentStage", &nlm::Brain::setDevelopmentalStage)
        .def("getConfig", &nlm::Brain::getConfig)
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator)
        .def("getSpikeSystem", &nlm::Brain::getSpikeSystem)
        .def("getSTDP", &nlm::Brain::getSTDP)
        .def("getHebbian", &nlm::Brain::getHebbian)
        .def("getStructuralPlasticity", &nlm::Brain::getStructuralPlasticity)
        .def("logStatus", &nlm::Brain::logStatus);
    
    // AgentBrain class
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("getDevelopmentStage", &nlm::AgentBrain::getDevelopmentStage)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("reset", &nlm::AgentBrain::reset)
        .def("getBrain", &nlm::AgentBrain::getBrain)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled);
    
    // SimpleWorld class
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld")
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
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy)
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed)
        .def("addObject", &nlm::SimpleWorld::addObject)
        .def("removeObject", &nlm::SimpleWorld::removeObject)
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition);
    
    m.def("createBrain", &nlm::createBrain);
    m.def("createSimpleWorld", &nlm::createSimpleWorld);
    m.def("createAgentBrain", &nlm::createAgentBrain);
    
    // Memory classes
    py::class_<nlm::WorkingMemory, std::shared_ptr<nlm::WorkingMemory>>(m, "WorkingMemory")
        .def("store", &nlm::WorkingMemory::store)
        .def("retrieve", &nlm::WorkingMemory::retrieve)
        .def("contains", &nlm::WorkingMemory::contains)
        .def("clear", &nlm::WorkingMemory::clear)
        .def("getCapacity", &nlm::WorkingMemory::getCapacity)
        .def("getCurrentSize", &nlm::WorkingMemory::getCurrentSize)
        .def("decay", &nlm::WorkingMemory::decay)
        .def("storeToNeuron", &nlm::WorkingMemory::storeToNeuron)
        .def("retrieve", &nlm::WorkingMemory::retrieve)
        .def("update", &nlm::WorkingMemory::update)
        .def("boostCapacity", &nlm::WorkingMemory::boostCapacity)
        .def("setStabilityFactor", &nlm::WorkingMemory::setStabilityFactor);
    
    py::class_<nlm::EpisodicMemory, std::shared_ptr<nlm::EpisodicMemory>>(m, "EpisodicMemory")
        .def("storeEpisode", &nlm::EpisodicMemory::storeEpisode)
        .def("retrieveEpisode", &nlm::EpisodicMemory::retrieveEpisode)
        .def("getEpisodeCount", &nlm::EpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &nlm::EpisodicMemory::getRecentEpisodes)
        .def("clear", &nlm::EpisodicMemory::clear)
        .def("consolidate", &nlm::EpisodicMemory::consolidate)
        .def("setConsolidationFactor", &nlm::EpisodicMemory::setConsolidationFactor)
        .def("getAllEpisodes", &nlm::EpisodicMemory::getAllEpisodes)
        .def("getCurrentEpisodeIndex", &nlm::EpisodicMemory::getCurrentEpisodeIndex);
    
    // Prediction system
    py::class_<nlm::PredictionSystem, std::shared_ptr<nlm::PredictionSystem>>(m, "PredictionSystem")
        .def("update", &nlm::PredictionSystem::update)
        .def("updateWithContext", &nlm::PredictionSystem::updateWithContext)
        .def("getPredictionError", &nlm::PredictionSystem::getPredictionError)
        .def("getConfidence", &nlm::PredictionSystem::getConfidence);
    
    // Cognitive classes
    py::class_<nlm::NeuralPlanner, std::shared_ptr<nlm::NeuralPlanner>>(m, "NeuralPlanner")
        .def("update", &nlm::NeuralPlanner::update)
        .def("updateWithContext", &nlm::NeuralPlanner::updateWithContext)
        .def("setPlanningDepth", &nlm::NeuralPlanner::setPlanningDepth)
        .def("getPlan", &nlm::NeuralPlanner::getPlan);
    
    py::class_<nlm::ConceptFormation, std::shared_ptr<nlm::ConceptFormation>>(m, "ConceptFormation")
        .def("update", &nlm::ConceptFormation::update)
        .def("getConcepts", &nlm::ConceptFormation::getConcepts)
        .def("getConceptStrength", &nlm::ConceptFormation::getConceptStrength);
    
    py::class_<nlm::AttentionalSelection, std::shared_ptr<nlm::AttentionalSelection>>(m, "AttentionalSelection")
        .def("update", &nlm::AttentionalSelection::update)
        .def("processCompetition", &nlm::AttentionalSelection::processCompetition)
        .def("updateWithMemoryContext", &nlm::AttentionalSelection::updateWithMemoryContext)
        .def("setInhibitionStrength", &nlm::AttentionalSelection::setInhibitionStrength)
        .def("setExcitationStrength", &nlm::AttentionalSelection::setExcitationStrength);
    
    // Neuromodulation classes
    py::class_<nlm::Dopamine, std::shared_ptr<nlm::Dopamine>>(m, "Dopamine")
        .def("getLevel", &nlm::Dopamine::getLevel)
        .def("setLevel", &nlm::Dopamine::setLevel)
        .def("getPlasticityFactor", &nlm::Dopamine::getPlasticityFactor)
        .def("update", &nlm::Dopamine::update)
        .def("signalReward", &nlm::Dopamine::signalReward)
        .def("signalRewardPredictionError", &nlm::Dopamine::signalRewardPredictionError)
        .def("setBaseline", &nlm::Dopamine::setBaseline)
        .def("setPeak", &nlm::Dopamine::setPeak)
        .def("setDecayRate", &nlm::Dopamine::setDecayRate)
        .def("setReleaseRate", &nlm::Dopamine::setReleaseRate)
        .def("reset", &nlm::Dopamine::reset);
    
    py::class_<nlm::Curiosity, std::shared_ptr<nlm::Curiosity>>(m, "Curiosity")
        .def("getLevel", &nlm::Curiosity::getLevel)
        .def("update", &nlm::Curiosity::update);
    
    py::class_<nlm::Novelty, std::shared_ptr<nlm::Novelty>>(m, "Novelty")
        .def("getLevel", &nlm::Novelty::getLevel)
        .def("update", &nlm::Novelty::update);
    
    py::class_<nlm::PredictionError, std::shared_ptr<nlm::PredictionError>>(m, "PredictionError")
        .def("getLevel", &nlm::PredictionError::getLevel)
        .def("update", &nlm::PredictionError::update);
    
    // Neuron types and enums
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal);
    
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("Wait", nlm::ActionType::Wait)
        .value("LookLeft", nlm::ActionType::LookLeft)
        .value("LookRight", nlm::ActionType::LookRight);
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging);
    
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker);
    
    // Advanced functions for power users
    m.def("runAdvancedSimulation", [](std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world, 
                                  std::shared_ptr<nlm::AgentBrain> agent, size_t numSteps, 
                                  std::function<void(size_t)> callback) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            callback(step);
        }
    });
    
    m.def("runLearningEpisode", [](std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world,
                                  std::shared_ptr<nlm::AgentBrain> agent, size_t numSteps,
                                  float explorationRate) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            
            // Add curiosity-driven exploration
            if (agent->getCuriosityLevel() > explorationRate) {
                // Random exploration
                float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
                if (r < explorationRate) {
                    action = nlm::ActionType::MoveForward; // Simplified exploration
                }
            }
            
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward and update development
            float reward = 0.0f;
            if (action == nlm::ActionType::Interact) reward = 1.0f;
            if (action == nlm::ActionType::MoveForward) reward = 0.1f;
            
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
    });
    
    m.def("createComplexConfig", [](size_t neuronCount, size_t regionCount, 
                                   float connectionProbability, float stdpLtpWeight,
                                   float stdpLtdWeight, float synaptogenesisRate,
                                   float pruningRate) {
        auto config = nlm::createDefaultConfig();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProbability);
        config->set("stdp_ltp_weight", stdpLtpWeight);
        config->set("stdp_ltd_weight", stdpLtdWeight);
        config->set("synaptogenesis_rate", synaptogenesisRate);
        config->set("pruning_rate", pruningRate);
        return config;
    });
    
    m.def("runPhase6IntegrationTest", [](std::shared_ptr<nlm::Brain> brain) {
        // Run a comprehensive integration test
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step);
            
            if (step % 10 == 0) {
                brain->logStatus();
            }
        }
    });
    
    // Error handling
    py::register_exception<nlm::NLMException, std::runtime_error>(m, "NLMException");
    
    // Version info
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Advanced Neural Learning Machine with integrated memory, prediction, and cognitive systems";
}