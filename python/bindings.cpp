#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>

// Include all NLM headers
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm = nlm;

// Forward declarations for complex types
class PyAgentBrain;
class PyBrain;
class PySimpleWorld;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - Phase 6 Integration";
    
    // Register version
    m.attr("__version__") = "0.1.0";
    
    // ---------- ENUMS ----------
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("LookLeft", nlm::ActionType::LookLeft)
        .value("LookRight", nlm::ActionType::LookRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Wait", nlm::ActionType::Wait)
        .export_values();
    
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();
    
    // ---------- BASIC TYPES ----------
    py::class_<nlm::NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def_readwrite("id", &nlm::NeuronId::id);
    
    py::class_<nlm::RegionId>(m, "RegionId")
        .def(py::init<>())
        .def_readwrite("id", &nlm::RegionId::id);
    
    py::class_<nlm::SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def_readwrite("id", &nlm::SynapseId::id);
    
    // ---------- BRAIN CLASS ----------
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step)
        .def("reset", &nlm::Brain::reset)
        .def("save", &nlm::Brain::save)
        .def("load", &nlm::Brain::load)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage)
        .def("getSpikeSystem", &nlm::Brain::getSpikeSystem)
        .def("getSTDP", &nlm::Brain::getSTDP)
        .def("getHebbian", &nlm::Brain::getHebbian)
        .def("getStructuralPlasticity", &nlm::Brain::getStructuralPlasticity)
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem)
        .def("getDopamine", &nlm::Brain::getDopamine)
        .def("getCuriosity", &nlm::Brain::getCuriosity)
        .def("getPredictionError", &nlm::Brain::getPredictionError)
        .def("getNovelty", &nlm::Brain::getNovelty)
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory)
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &nlm::Brain::getAssociativeMemory)
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem)
        .def("getNeuralPlanner", &nlm::Brain::getNeuralPlanner)
        .def("getConceptFormation", &nlm::Brain::getConceptFormation)
        .def("getAttentionSystem", &nlm::Brain::getAttentionSystem)
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons)
        .def("logStatus", &nlm::Brain::logStatus)
        .def("addRegion", &nlm::Brain::addRegion)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("getRegions", &nlm::Brain::getRegions);
    
    // ---------- CONFIG CLASS ----------
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("has", &nlm::Config::has)
        .def("set", &nlm::Config::set)
        .def("getOr", py::overload_cast<const std::string&, int64_t&>(&nlm::Config::getOr), "getInt64")
        .def("getOr", py::overload_cast<const std::string&, double&>(&nlm::Config::getOr), "getDouble")
        .def("getOr", py::overload_cast<const std::string&, float&>(&nlm::Config::getOr), "getFloat")
        .def("getOr", py::overload_cast<const std::string&, bool&>(&nlm::Config::getOr), "getBool")
        .def("getOr", py::overload_cast<const std::string&, std::string&>(&nlm::Config::getOr), "getString")
        .def("get", py::overload_cast<const std::string&>(&nlm::Config::get), py::arg("key"), "getAny")
        .def("getKeys", &nlm::Config::getKeys)
        .def_static("createDefaultConfig", &nlm::Config::createDefaultConfig);
    
    // ---------- AGENT BRAIN CLASS ----------
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("reset", &nlm::AgentBrain::reset)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def_static("createAgentBrain", &nlm::AgentBrain::createAgentBrain);
    
    // ---------- SIMPLE WORLD CLASS ----------
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update)
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody)
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy)
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed)
        .def("addObject", &nlm::SimpleWorld::addObject)
        .def("removeObject", &nlm::SimpleWorld::removeObject)
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition)
        .def_static("createSimpleWorld", &nlm::SimpleWorld::createSimpleWorld);
    
    // ---------- WORLD OBJECT CLASS ----------
    py::class_<nlm::WorldObject>(m, "WorldObject")
        .def(py::init<double, double, nlm::WorldObjectType, float>())
        .def(py::init<>())
        .def("getX", &nlm::WorldObject::getX)
        .def("getY", &nlm::WorldObject::getY)
        .def("getType", &nlm::WorldObject::getType)
        .def("getValue", &nlm::WorldObject::getValue)
        .def("setX", &nlm::WorldObject::setX)
        .def("setY", &nlm::WorldObject::setY)
        .def("setType", &nlm::WorldObject::setType)
        .def("setValue", &nlm::WorldObject::setValue);
    
    // ---------- SENSORY PERCEPT CLASS ----------
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setVision", &nlm::SensoryPercept::setVision)
        .def("setTouch", &nlm::SensoryPercept::setTouch)
        .def("setInternal", &nlm::SensoryPercept::setInternal)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception);
    
    // ---------- MOTOR COMMAND ----------
    py::enum_<nlm::MotorCommand>(m, "MotorCommand")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();
    
    // ---------- FACTORY FUNCTIONS ----------
    m.def("createBrain", &nlm::Brain::createBrain);
    m.def("createDefaultConfig", &nlm::Config::createDefaultConfig);
    m.def("createAgentBrain", &nlm::AgentBrain::createAgentBrain);
    m.def("createSimpleWorld", &nlm::SimpleWorld::createSimpleWorld);
    
    // ---------- UTILITY FUNCTIONS ----------
    m.def("get_version", []() { return "0.1.0"; });
    m.def("print_banner", []() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Advanced Brain Simulator with All Systems Integrated       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
        )" << std::endl;
    });
    
    // ---------- ADVANCED FUNCTIONS ----------
    m.def("run_phase6_integration_test", [](std::shared_ptr<nlm::Brain> brain) {
        // Run comprehensive integration test
        brain->initialize();
        
        // Enable all subsystems
        auto agent = nlm::AgentBrain::createAgentBrain(brain);
        auto world = nlm::SimpleWorld::createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        // Enable all advanced features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run integration test
        for (int step = 0; step < 1000; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = percept.getInternal()[0] if (!percept.getInternal().empty()) else 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
        }
        
        // Return final statistics
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            agent->getCuriosityLevel(),
            agent->getDevelopmentalStage()
        );
    });
    
    m.def("save_brain_checkpoint", [](std::shared_ptr<nlm::Brain> brain, const std::string& filename) {
        brain->save(filename);
    });
    
    m.def("load_brain_checkpoint", [](const std::string& filename) {
        auto config = nlm::Config::createDefaultConfig();
        auto brain = nlm::Brain::createBrain(config);
        brain->load(filename);
        return brain;
    });
    
    // ---------- EXPERT MODE FUNCTIONS ----------
    m.def("create_advanced_brain", [](int neuronCount = 5000, double connectionDensity = 0.15) {
        auto config = nlm::Config::createDefaultConfig();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.synapse_density", connectionDensity);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.2);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.synaptogenesis_rate", 0.002);
        config->set("development.pruning_rate", 0.0002);
        
        auto brain = nlm::Brain::createBrain(config);
        brain->initialize();
        return brain;
    });
    
    m.def("create_complex_world", [](int width = 50, int height = 50, int visionX = 10, int visionY = 10) {
        auto world = nlm::SimpleWorld::createSimpleWorld();
        world->configure(width, height, visionX, visionY);
        
        // Add complex terrain
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if (x % 7 == 0 && y % 7 == 0) {
                    world->addObject(nlm::WorldObject(x, y, nlm::WorldObjectType::Resource, 1.0f));
                } else if ((x + y) % 11 == 0) {
                    world->addObject(nlm::WorldObject(x, y, nlm::WorldObjectType::Hazard, -0.5f));
                }
            }
        }
        
        return world;
    });
    
    m.def("run_advanced_simulation", [](std::shared_ptr<nlm::Brain> brain, 
                                          std::shared_ptr<nlm::SimpleWorld> world,
                                          std::shared_ptr<nlm::AgentBrain> agent,
                                          int steps, bool verbose = false) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        float maxCuriosity = 0.0f;
        float maxNovelty = 0.0f;
        int learningEvents = 0;
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = !percept.getInternal().empty() ? percept.getInternal()[0] : 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
            
            // Track metrics
            maxCuriosity = std::max(maxCuriosity, agent->getCuriosityLevel());
            maxNovelty = std::max(maxNovelty, agent->getNoveltyLevel());
            if (std::abs(agent->getPredictionError()) > 0.1f) {
                learningEvents++;
            }
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step 
                          << ": Curiosity=" << agent->getCuriosityLevel()
                          << ", Novelty=" << agent->getNoveltyLevel()
                          << ", Learning Events=" << learningEvents
                          << std::endl;
            }
        }
        
        return std::make_tuple(maxCuriosity, maxNovelty, learningEvents);
    });
}