// ============================================================================
// NLM Python Bindings
// ============================================================================
// This file provides Python bindings for the NLM (Neural Learning Machine)
// computational brain framework using pybind11.
// ============================================================================

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "motor/MotorSystem.hpp"
#include "environment/Environment.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"

namespace py = pybind11;

// ============================================================================
// Configuration Bindings
// ============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - an experimental computational brain";
    m.attr("__version__") = "0.1.0";

    // ============================================================================
    // Configuration System
    // ============================================================================
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("loadFromArgs", &nlm::Config::loadFromArgs)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("get", &nlm::Config::get<std::string>)
        .def("get", py::overload_cast<const std::string&>(&nlm::Config::getOr<int>, py::const_))
        .def("get", py::overload_cast<const std::string&>(&nlm::Config::getOr<double>, py::const_))
        .def("get", py::overload_cast<const std::string&>(&nlm::Config::getOr<bool>, py::const_))
        .def("set", py::overload_cast<const std::string&, const std::string&>(&nlm::Config::set, py::const_))
        .def("set", py::overload_cast<const std::string&, int>(&nlm::Config::set, py::const_))
        .def("set", py::overload_cast<const std::string&, double>(&nlm::Config::set, py::const_))
        .def("set", py::overload_cast<const std::string&, bool>(&nlm::Config::set, py::const_))
        .def("has", &nlm::Config::has)
        .def("remove", &nlm::Config::remove)
        .def("getKeys", &nlm::Config::getKeys)
        .def("clear", &nlm::Config::clear)
        .def("summary", &nlm::Config::summary);

    m.def("createDefaultConfig", []() {
        return nlm::Config();
    }, "Create a default configuration for NLM");

    // ============================================================================
    // Neuron Types
    // ============================================================================
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    // ============================================================================
    // Developmental Stages
    // ============================================================================
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    // ============================================================================
    // World Object Types
    // ============================================================================
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // ============================================================================
    // Action Types
    // ============================================================================
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("Wait", nlm::ActionType::Wait)
        .export_values();

    // ============================================================================
    // Brain Class
    // ============================================================================
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step)
        .def("stepWithTime", &nlm::Brain::step, py::arg("step"), py::arg("time"))
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
        .def("getDevelopmentalAge", &nlm::Brain::getDevelopmentalAge)
        .def("enableRewardModulation", &nlm::Brain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::Brain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::Brain::enableDevelopment)
        .def("enableCuriosity", &nlm::Brain::enableCuriosity)
        .def("isRewardModulationEnabled", &nlm::Brain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::Brain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::Brain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::Brain::isCuriosityEnabled)
        .def("addRegion", &nlm::Brain::addRegion)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("getRegionCount", &nlm::Brain::getRegionCount)
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput)
        .def("produceAction", &nlm::Brain::produceAction);

    m.def("createBrain", [](nlm::Config config) {
        return std::make_shared<nlm::Brain>(std::move(config));
    }, py::arg("config"), "Create and initialize a brain with given configuration");

    m.def("createBrain", []() {
        return std::make_shared<nlm::Brain>(nlm::Config());
    }, "Create a brain with default configuration");

    // ============================================================================
    // AgentBrain Class
    // ============================================================================
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def(py::init<std::shared_ptr<nlm::Brain>,
                     std::function<nlm::NeuronGroups(const std::vector<nlm::Neuron*>&, const std::string&)},
                     std::function<std::shared_ptr<nlm::Neuromodulator>()>,
                     std::function<std::shared_ptr<nlm::DevelopmentSystem>()>>(
            py::arg("brain"), py::arg("neuron_factory"), py::arg("neuromodulation_factory"), 
            py::arg("development_factory")))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
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

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain that connects a brain to a world");

    // ============================================================================
    // SimpleWorld Class
    // ============================================================================
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody)
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getObjectAt", &nlm::SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed);

    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a simple 2D world for agent interaction");

    // ============================================================================
    // Vision Bindings
    // ============================================================================
    py::class_<nlm::Vision>(m, "Vision")
        .def(py::init<>())
        .def("generateVision", &nlm::Vision::generateVision)
        .def("getVisionData", &nlm::Vision::getVisionData)
        .def("getVisionWidth", &nlm::Vision::getVisionWidth)
        .def("getVisionHeight", &nlm::Vision::getVisionHeight)
        .def("setFOV", &nlm::Vision::setFOV, py::arg("fov"))
        .def("getFOV", &nlm::Vision::getFOV);

    // ============================================================================
    // MotorCommand Bindings
    // ============================================================================
    py::class_<nlm::MotorCommand>(m, "MotorCommand")
        .def(py::init<>())
        .def_readwrite("type", &nlm::MotorCommand::type)
        .def_readwrite("magnitude", &nlm::MotorCommand::magnitude)
        .def_readwrite("duration", &nlm::MotorCommand::duration)
        .def("toString", &nlm::MotorCommand::toString);

    // ============================================================================
    // SensoryPercept Bindings
    // ============================================================================
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def_readwrite("vision", &nlm::SensoryPercept::vision)
        .def_readwrite("touch", &nlm::SensoryPercept::touch)
        .def_readwrite("internal", &nlm::SensoryPercept::internal)
        .def_readwrite("proprioception", &nlm::SensoryPercept::proprioception)
        .def("getVisionData", &nlm::SensoryPercept::getVisionData)
        .def("getTouchData", &nlm::SensoryPercept::getTouchData)
        .def("getInternalData", &nlm::SensoryPercept::getInternalData)
        .def("getProprioceptionData", &nlm::SensoryPercept::getProprioceptionData);

    // ============================================================================
    // ActionResult Bindings
    // ============================================================================
    py::enum_<nlm::ActionResult>(m, "ActionResult")
        .value("Success", nlm::ActionResult::Success)
        .value("Collision", nlm::ActionResult::Collision)
        .value("OutOfBounds", nlm::ActionResult::OutOfBounds)
        .value("InvalidCommand", nlm::ActionResult::InvalidCommand)
        .export_values();

    // ============================================================================
    // AgentBody Bindings
    // ============================================================================
    py::class_<nlm::AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def("getPosition", &nlm::AgentBody::getPosition)
        .def("setPosition", &nlm::AgentBody::setPosition, py::arg("x"), py::arg("y"))
        .def("getVelocity", &nlm::AgentBody::getVelocity)
        .def("setVelocity", &nlm::AgentBody::setVelocity, py::arg("vx"), py::arg("vy"))
        .def("isAlive", &nlm::AgentBody::isAlive);

    // ============================================================================
    // WorldObject Bindings
    // ============================================================================
    py::class_<nlm::WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>())
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active)
        .def("toString", &nlm::WorldObject::toString);

    // ============================================================================
    // Helper Functions
    // ============================================================================
    m.def("run_simulation", [](std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world, 
                             std::shared_ptr<nlm::AgentBrain> agent, uint64_t num_steps) {
        for (uint64_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete simulation loop");

    m.def("create_complete_agent_simulation", []() {
        auto config = nlm::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete agent simulation setup");

    // ============================================================================
    // Statistics and Utilities
    // ============================================================================
    m.def("print_brain_stats", [](const nlm::Brain& brain) {
        std::cout << "=== Brain Statistics ===" << std::endl;
        std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
        std::cout << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
        std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    }, py::arg("brain"), "Print brain statistics to console");

    m.def("brain_summary", [](const nlm::Brain& brain) {
        std::ostringstream oss;
        oss << "Brain: " << brain.getTotalNeuronCount() << " neurons, " 
            << brain.getTotalSynapseCount() << " synapses" << std::endl;
        oss << "Firing: " << brain.getFiringNeuronCount() << "/" << brain.getTotalNeuronCount() 
            << " neurons, rate: " << brain.getAverageFiringRate() << std::endl;
        oss << "Spikes: " << brain.getTotalSpikeCount() << std::endl;
        oss << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        oss << "Development: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        return oss.str();
    }, py::arg("brain"), "Get brain summary as string");

    // ============================================================================
    // Configuration Utilities
    // ============================================================================
    m.def("print_config", [](const nlm::Config& config) {
        std::cout << config.summary() << std::endl;
    }, py::arg("config"), "Print configuration summary to console");

    m.def("create_config_with_values", [](std::unordered_map<std::string, std::variant<int, double, bool, std::string>> values) {
        nlm::Config config;
        for (const auto& pair : values) {
            const std::string& key = pair.first;
            const auto& value = pair.second;
            if (std::holds_alternative<int>(value)) {
                config.set(key, std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                config.set(key, std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                config.set(key, std::get<bool>(value));
            } else if (std::holds_alternative<std::string>(value)) {
                config.set(key, std::get<std::string>(value));
            }
        }
        return config;
    }, py::arg("values"), "Create a configuration with specified values");

    // ============================================================================
    // NeuronGroups Structure for Dependency Injection
    // ============================================================================
    py::class_<nlm::NeuronGroups>(m, "NeuronGroups")
        .def(py::init<>())
        .def_readwrite("forward", &nlm::NeuronGroups::forward)
        .def_readwrite("backward", &nlm::NeuronGroups::backward)
        .def_readwrite("turnLeft", &nlm::NeuronGroups::turnLeft)
        .def_readwrite("turnRight", &nlm::NeuronGroups::turnRight)
        .def_readwrite("interact", &nlm::NeuronGroups::interact)
        .def_readwrite("wait", &nlm::NeuronGroups::wait)
        .def_readwrite("vision", &nlm::NeuronGroups::vision)
        .def_readwrite("touch", &nlm::NeuronGroups::touch)
        .def_readwrite("internal", &nlm::NeuronGroups::internal)
        .def_readwrite("proprioception", &nlm::NeuronGroups::proprioception);

    // ============================================================================
    // Factory Function Types
    // ============================================================================
    m.def("createNeuronGroupFactory", []() {
        return std::function<nlm::NeuronGroups(const std::vector<nlm::Neuron*>&, const std::string&)>();
    }, "Create a default neuron group factory function");

    m.def("createDefaultNeuronGroupFactory", []() {
        return [](const std::vector<nlm::Neuron*>& neurons, const std::string& type) {
            nlm::NeuronGroups groups;
            size_t idx = 0;
            for (nlm::Neuron* n : neurons) {
                if (type == "motor") {
                    switch (idx % 6) {
                        case 0: groups.forward.push_back(n); break;
                        case 1: groups.backward.push_back(n); break;
                        case 2: groups.turnLeft.push_back(n); break;
                        case 3: groups.turnRight.push_back(n); break;
                        case 4: groups.interact.push_back(n); break;
                        case 5: groups.wait.push_back(n); break;
                    }
                } else if (type == "sensory") {
                    switch (idx % 4) {
                        case 0: groups.vision.push_back(n); break;
                        case 1: groups.touch.push_back(n); break;
                        case 2: groups.internal.push_back(n); break;
                        case 3: groups.proprioception.push_back(n); break;
                    }
                }
                idx++;
            }
            return groups;
        };
    }, "Create a default neuron group factory for motor/sensory neuron distribution");

    m.def("createDefaultNeuromodulationFactory", []() {
        return []() {
            return std::make_shared<nlm::Neuromodulator>();
        };
    }, "Create a default neuromodulation factory");

    m.def("createDefaultDevelopmentSystemFactory", []() {
        return []() {
            return std::make_shared<nlm::DevelopmentSystem>();
        };
    }, "Create a default development system factory");

    // ============================================================================
    // Advanced AgentBrain Constructor
    // ============================================================================
    m.def("createAgentBrainWithDependencyInjection", 
          [](std::shared_ptr<nlm::Brain> brain,
             std::function<nlm::NeuronGroups(const std::vector<nlm::Neuron*>&, const std::string&)> neuronFactory,
             std::function<std::shared_ptr<nlm::Neuromodulator>()> neuromodulationFactory,
             std::function<std::shared_ptr<nlm::DevelopmentSystem>()> developmentFactory) {
              return std::make_shared<nlm::AgentBrain>(brain, neuronFactory, neuromodulationFactory, developmentFactory);
          }, py::arg("brain"), py::arg("neuron_factory"), 
          py::arg("neuromodulation_factory"), py::arg("development_factory"),
          "Create an AgentBrain with dependency injection for advanced testing and customization");

    m.def("createDefaultAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a default AgentBrain (backward compatibility)");

    // ============================================================================
    // AgentBrain Configuration and Statistics
    // ============================================================================
    m.def("configureAgentBrain", [](std::shared_ptr<nlm::AgentBrain> agent, bool reward, bool structural, 
                                  bool development, bool curiosity) {
        agent->enableRewardModulation(reward);
        agent->enableStructuralPlasticity(structural);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
    }, py::arg("agent"), py::arg("reward"), py::arg("structural"), 
    py::arg("development"), py::arg("curiosity"),
    "Configure agent brain subsystems for different experimental conditions");

    m.def("printAgentBrainStats", [](const nlm::AgentBrain& agent) {
        std::cout << "=== AgentBrain Statistics ===" << std::endl;
        std::cout << "Neuromodulation Level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "Curiosity Level: " << agent.getCuriosityLevel() << std::endl;
        std::cout << "Novelty Level: " << agent.getNoveltyLevel() << std::endl;
        std::cout << "Prediction Error: " << agent.getPredictionError() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "Reward Enabled: " << (agent.isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
        std::cout << "Development Enabled: " << (agent.isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
        std::cout << "Curiosity Enabled: " << (agent.isCuriosityEnabled() ? "Yes" : "No") << std::endl;
    }, py::arg("agent"), "Print AgentBrain statistics to console");

    m.def("agentBrainSummary", [](const nlm::AgentBrain& agent) {
        std::ostringstream oss;
        oss << "AgentBrain Summary:" << std::endl;
        oss << "  Neuromodulation: " << agent.getNeuromodulationLevel() << std::endl;
        oss << "  Curiosity: " << agent.getCuriosityLevel() << std::endl;
        oss << "  Novelty: " << agent.getNoveltyLevel() << std::endl;
        oss << "  Development Stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        oss << "  Reward Enabled: " << (agent.isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
        oss << "  Development Enabled: " << (agent.isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
        oss << "  Curiosity Enabled: " << (agent.isCuriosityEnabled() ? "Yes" : "No") << std::endl;
        return oss.str();
    }, py::arg("agent"), "Get AgentBrain summary as string");

    // ============================================================================
    // Advanced Simulation Control
    // ============================================================================
    m.def("run_advanced_simulation", 
          [](std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world,
             std::shared_ptr<nlm::AgentBrain> agent, uint64_t num_steps,
             bool enableRewardModulation, bool enableCuriosity) {
              agent->enableRewardModulation(enableRewardModulation);
              agent->enableCuriosity(enableCuriosity);
              
              for (uint64_t step = 0; step < num_steps; ++step) {
                  world->update(0.1);
                  auto percept = world->getSensoryPercept();
                  agent->processSensoryInput(percept);
                  brain->step(step);
                  auto action = agent->decodeMotorCommand();
                  world->applyMotorCommand(action, world->getSimulationTime());
                  
                  if (enableRewardModulation) {
                      float reward = 0.0f;
                      if (world->getSensoryPercept().getInternal().size() > 0) {
                          reward = world->getSensoryPercept().getInternal()[0];
                      }
                      agent->applyRewardModulation(reward, 0.0f);
                  }
                  
                  agent->updateDevelopment(0.1);
              }
          }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("enable_reward_modulation"), py::arg("enable_curiosity"),
          "Run an advanced simulation with full agent capabilities");

    m.def("create_advanced_agent_simulation", []() {
        auto config = nlm::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agent = nlm::createAgentBrainWithDependencyInjection(
            brain,
            nlm::createDefaultNeuronGroupFactory(),
            nlm::createDefaultNeuromodulationFactory(),
            nlm::createDefaultDevelopmentSystemFactory()
        );
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create an advanced agent simulation with dependency injection");

    // ============================================================================
    // Legacy Compatibility Functions
    // ============================================================================
    m.def("createBrain", [](nlm::Config config) {
        return std::make_shared<nlm::Brain>(std::move(config));
    }, py::arg("config"), "Create and initialize a brain with given configuration");

    m.def("createBrain", []() {
        return std::make_shared<nlm::Brain>(nlm::Config());
    }, "Create a brain with default configuration");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain that connects a brain to a world");

    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a simple 2D world for agent interaction");

    m.def("run_simulation", [](std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world, 
                             std::shared_ptr<nlm::AgentBrain> agent, uint64_t num_steps) {
        for (uint64_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete simulation loop (legacy compatibility)");
