#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM - Neural Learning Machine - Python bindings";
    
    // Enums
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("Debug", nlm::LogLevel::Debug)
        .value("Info", nlm::LogLevel::Info)
        .value("Warning", nlm::LogLevel::Warning)
        .value("Error", nlm::LogLevel::Error)
        .value("Critical", nlm::LogLevel::Critical)
        .export_values();
    
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Internal", nlm::NeuronType::Internal)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .export_values();
    
    py::enum_<nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .export_values();
    
    py::enum_<nlm::FiringState>(m, "FiringState")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .export_values();
    
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("Wait", nlm::ActionType::Wait)
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .export_values();
    
    // Core Types
    py::class_<nlm::NeuronId>(m, "NeuronId")
        .def(py::init<uint64_t>())
        .def("value", &nlm::NeuronId::value)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__hash__", &nlm::NeuronId::operator Hash);
    
    py::class_<nlm::SynapseId>(m, "SynapseId")
        .def(py::init<uint64_t>())
        .def("value", &nlm::SynapseId::value)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__hash__", &nlm::SynapseId::operator Hash);
    
    py::class_<nlm::RegionId>(m, "RegionId")
        .def(py::init<uint64_t>())
        .def("value", &nlm::RegionId::value)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__hash__", &nlm::RegionId::operator Hash);
    
    py::class_<nlm::PopulationId>(m, "PopulationId")
        .def(py::init<uint64_t>())
        .def("value", &nlm::PopulationId::value)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__hash__", &nlm::PopulationId::operator Hash);
    
    // Neuron class
    py::class_<nlm::Neuron>(m, "Neuron")
        .def(py::init<nlm::NeuronId>())
        .def("getId", &nlm::Neuron::getId)
        .def("getType", &nlm::Neuron::getType)
        .def("setType", &nlm::Neuron::setType)
        .def("getMembranePotential", &nlm::Neuron::getMembranePotential)
        .def("setMembranePotential", &nlm::Neuron::setMembranePotential)
        .def("addToMembranePotential", &nlm::Neuron::addToMembranePotential)
        .def("getThreshold", &nlm::Neuron::getThreshold)
        .def("setThreshold", &nlm::Neuron::setThreshold)
        .def("isFiring", &nlm::Neuron::isFiring)
        .def("isRefractory", &nlm::Neuron::isRefractory)
        .def("setFiringState", &nlm::Neuron::setFiringState)
        .def("decrementRefractory", &nlm::Neuron::decrementRefractory)
        .def("getFiringRate", &nlm::Neuron::getFiringRate)
        .def("setFiringRate", &nlm::Neuron::setFiringRate)
        .def("getLeakConductance", &nlm::Neuron::getLeakConductance)
        .def("setLeakConductance", &nlm::Neuron::setLeakConductance)
        .def("getRefractoryPeriod", &nlm::Neuron::getRefractoryPeriod)
        .def("setRefractoryPeriod", &nlm::Neuron::setRefractoryPeriod)
        .def("getRestingPotential", &nlm::Neuron::getRestingPotential)
        .def("setRestingPotential", &nlm::Neuron::setRestingPotential)
        .def("getResetPotential", &nlm::Neuron::getResetPotential)
        .def("setResetPotential", &nlm::Neuron::setResetPotential)
        .def("checkThreshold", &nlm::Neuron::checkThreshold)
        .def("getLastSpikeTime", &nlm::Neuron::getLastSpikeTime)
        .def("receiveExcitatoryInput", &nlm::Neuron::receiveExcitatoryInput)
        .def("receiveInhibitoryInput", &nlm::Neuron::receiveInhibitoryInput)
        .def("receiveModulatoryInput", &nlm::Neuron::receiveModulatoryInput)
        .def("injectCurrent", &nlm::Neuron::injectCurrent)
        .def("setTotalCurrent", &nlm::Neuron::setTotalCurrent)
        .def("getTotalCurrent", &nlm::Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &nlm::Neuron::clearTotalCurrent)
        .def("recordSpike", &nlm::Neuron::recordSpike)
        .def("getSpikeHistory", &nlm::Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &nlm::Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &nlm::Neuron::addIncomingSynapse)
        .def("addOutgoingSynapse", &nlm::Neuron::addOutgoingSynapse)
        .def("getIncomingSynapses", &nlm::Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &nlm::Neuron::getOutgoingSynapses)
        .def("getPlasticityFlags", &nlm::Neuron::getPlasticityFlags)
        .def("enablePlasticity", &nlm::Neuron::enablePlasticity)
        .def("setRegionId", &nlm::Neuron::setRegionId)
        .def("getRegionId", &nlm::Neuron::getRegionId)
        .def("setPopulationId", &nlm::Neuron::setPopulationId)
        .def("getPopulationId", &nlm::Neuron::getPopulationId)
        .def("step", &nlm::Neuron::step)
        .def("reset", &nlm::Neuron::reset)
        .def("initializeRandom", &nlm::Neuron::initializeRandom)
        .def("__repr__", [](const nlm::Neuron& n) {
            return "Neuron(id=" + std::to_string(n.getId().value) + ")";
        });
    
    // Synapse class
    py::class_<nlm::Synapse>(m, "Synapse")
        .def(py::init<nlm::SynapseId, nlm::NeuronId, nlm::NeuronId>())
        .def("getId", &nlm::Synapse::getId)
        .def("getSourceNeuron", &nlm::Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &nlm::Synapse::getDestinationNeuron)
        .def("getWeight", &nlm::Synapse::getWeight)
        .def("setWeight", &nlm::Synapse::setWeight)
        .def("addToWeight", &nlm::Synapse::addToWeight)
        .def("getDelay", &nlm::Synapse::getDelay)
        .def("setDelay", &nlm::Synapse::setDelay)
        .def("getType", &nlm::Synapse::getType)
        .def("setType", &nlm::Synapse::setType)
        .def("isExcitatory", &nlm::Synapse::isExcitatory)
        .def("isInhibitory", &nlm::Synapse::isInhibitory)
        .def("recordPreSpike", &nlm::Synapse::recordPreSpike)
        .def("recordPostSpike", &nlm::Synapse::recordPostSpike)
        .def("getPreSpikeHistory", &nlm::Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &nlm::Synapse::getPostSpikeHistory)
        .def("clearHistory", &nlm::Synapse::clearHistory)
        .def("getPlasticityFlags", &nlm::Synapse::getPlasticityFlags)
        .def("enablePlasticity", &nlm::Synapse::enablePlasticity)
        .def("getEligibilityTrace", &nlm::Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &nlm::Synapse::setEligibilityTrace)
        .def("decayEligibilityTrace", &nlm::Synapse::decayEligibilityTrace)
        .def("getEfficacy", &nlm::Synapse::getEfficacy)
        .def("setEfficacy", &nlm::Synapse::setEfficacy)
        .def("step", &nlm::Synapse::step)
        .def("reset", &nlm::Synapse::reset)
        .def("initializeRandom", &nlm::Synapse::initializeRandom)
        .def("__repr__", [](const nlm::Synapse& s) {
            return "Synapse(id=" + std::to_string(s.getId().value) + ")";
        });
    
    // RandomGenerator class
    py::class_<nlm::RandomGenerator>(m, "RandomGenerator")
        .def(py::init<uint64_t>())
        .def("uniformReal", &nlm::RandomGenerator::uniformReal)
        .def("uniformInt", &nlm::RandomGenerator::uniformInt)
        .def("gaussian", &nlm::RandomGenerator::gaussian)
        .def("setSeed", &nlm::RandomGenerator::setSeed);
    
    // Logger class
    py::class_<nlm::Logger>(m, "Logger")
        .def("debug", &nlm::Logger::debug)
        .def("info", &nlm::Logger::info)
        .def("warning", &nlm::Logger::warning)
        .def("error", &nlm::Logger::error)
        .def("critical", &nlm::Logger::critical)
        .def("isEnabled", &nlm::Logger::isEnabled)
        .def("setLevel", &nlm::Logger::setLevel)
        .def("getGlobal", &nlm::Logger::getGlobal)
        .def("setGlobal", &nlm::Logger::setGlobal);
    
    // ConsoleLogger class
    py::class_<nlm::ConsoleLogger, nlm::ILogger>(m, "ConsoleLogger")
        .def(py::init<>())
        .def(py::init<nlm::LogLevel>())
        .def("log", &nlm::ConsoleLogger::log)
        .def("setLevel", &nlm::ConsoleLogger::setLevel)
        .def("getLevel", &nlm::ConsoleLogger::getLevel)
        .def("setUseColors", &nlm::ConsoleLogger::setUseColors)
        .def("getUseColors", &nlm::ConsoleLogger::getUseColors)
        .def("setOutputFile", &nlm::ConsoleLogger::setOutputFile)
        .def("closeOutputFile", &nlm::ConsoleLogger::closeOutputFile);
    
    // Config class
    py::class_<nlm::Config>(m, "Config")
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("loadFromArgs", &nlm::Config::loadFromArgs)
        .def("set", &nlm::Config::set)
        .def("getOr", py::overload_cast<const std::string&, int64_t>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, double>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, float>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, bool>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&nlm::Config::getOr));
    
    // Brain class
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step)
        .def("reset", &nlm::Brain::reset)
        .def("getRegionCount", &nlm::Brain::getRegionCount)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("injectCurrent", &nlm::Brain::injectCurrent)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons)
        .def("logStatus", &nlm::Brain::logStatus)
        .def("__repr__", [](const nlm::Brain& b) {
            return "Brain(neurons=" + std::to_string(b.getTotalNeuronCount()) + 
                   ", synapses=" + std::to_string(b.getTotalSynapseCount()) + ")";
        });
    
    // WorkingMemory class
    py::class_<nlm::WorkingMemory>(m, "WorkingMemory")
        .def("store", &nlm::WorkingMemory::store)
        .def("retrieve", &nlm::WorkingMemory::retrieve)
        .def("contains", &nlm::WorkingMemory::contains)
        .def("clear", &nlm::WorkingMemory::clear)
        .def("getCapacity", &nlm::WorkingMemory::getCapacity)
        .def("getCurrentSize", &nlm::WorkingMemory::getCurrentSize)
        .def("decay", &nlm::WorkingMemory::decay);
    
    // EpisodicMemory class
    py::class_<nlm::EpisodicMemory>(m, "EpisodicMemory")
        .def("storeEpisode", &nlm::EpisodicMemory::storeEpisode)
        .def("retrieveEpisode", &nlm::EpisodicMemory::retrieveEpisode)
        .def("getEpisodeCount", &nlm::EpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &nlm::EpisodicMemory::getRecentEpisodes)
        .def("clear", &nlm::EpisodicMemory::clear)
        .def("consolidate", &nlm::EpisodicMemory::consolidate);
    
    // SemanticMemory class
    py::class_<nlm::SemanticMemory>(m, "SemanticMemory")
        .def("storeFact", &nlm::SemanticMemory::storeFact)
        .def("retrieveFact", &nlm::SemanticMemory::retrieveFact)
        .def("hasFact", &nlm::SemanticMemory::hasFact)
        .def("getAllFacts", &nlm::SemanticMemory::getAllFacts)
        .def("clear", &nlm::SemanticMemory::clear);
    
    // ProceduralMemory class
    py::class_<nlm::ProceduralMemory>(m, "ProceduralMemory")
        .def("learnSkill", &nlm::ProceduralMemory::learnSkill)
        .def("getSkill", &nlm::ProceduralMemory::getSkill)
        .def("getAllSkills", &nlm::ProceduralMemory::getAllSkills)
        .def("updateProficiency", &nlm::ProceduralMemory::updateProficiency)
        .def("clear", &nlm::ProceduralMemory::clear);
    
    // AssociativeMemory class
    py::class_<nlm::AssociativeMemory>(m, "AssociativeMemory")
        .def("associate", &nlm::AssociativeMemory::associate)
        .def("getAssociations", &nlm::AssociativeMemory::getAssociations)
        .def("getAssociationStrength", &nlm::AssociativeMemory::getAssociationStrength)
        .def("updateAssociation", &nlm::AssociativeMemory::updateAssociation)
        .def("clear", &nlm::AssociativeMemory::clear);
    
    // NeuralWorkingMemory class
    py::class_<nlm::NeuralWorkingMemory>(m, "NeuralWorkingMemory")
        .def("initialize", &nlm::NeuralWorkingMemory::initialize)
        .def("store", &nlm::NeuralWorkingMemory::store)
        .def("storeToNeuron", &nlm::NeuralWorkingMemory::storeToNeuron)
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve)
        .def("contains", &nlm::NeuralWorkingMemory::contains)
        .def("update", &nlm::NeuralWorkingMemory::update)
        .def("clear", &nlm::NeuralWorkingMemory::clear)
        .def("strengthenMemory", &nlm::NeuralWorkingMemory::strengthenMemory)
        .def("runCompetition", &nlm::NeuralWorkingMemory::runCompetition)
        .def("getMemoryActivity", &nlm::NeuralWorkingMemory::getMemoryActivity);
    
    // NeuralEpisodicMemory class
    py::class_<nlm::NeuralEpisodicMemory>(m, "NeuralEpisodicMemory")
        .def("initialize", &nlm::NeuralEpisodicMemory::initialize)
        .def("storeEpisode", &nlm::NeuralEpisodicMemory::storeEpisode)
        .def("retrieveSimilar", &nlm::NeuralEpisodicMemory::retrieveSimilar)
        .def("retrieveTemporal", &nlm::NeuralEpisodicMemory::retrieveTemporal)
        .def("retrieveByLocation", &nlm::NeuralEpisodicMemory::retrieveByLocation)
        .def("retrieveAfterAction", &nlm::NeuralEpisodicMemory::retrieveAfterAction)
        .def("replayEpisode", &nlm::NeuralEpisodicMemory::replayEpisode)
        .def("updateRelevance", &nlm::NeuralEpisodicMemory::updateRelevance)
        .def("getEpisode", &nlm::NeuralEpisodicMemory::getEpisode)
        .def("getEpisodeCount", &nlm::NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &nlm::NeuralEpisodicMemory::getRecentEpisodes)
        .def("getAverageReward", &nlm::NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &nlm::NeuralEpisodicMemory::consolidate)
        .def("clear", &nlm::NeuralEpisodicMemory::clear)
        .def("enableReplay", &nlm::NeuralEpisodicMemory::enableReplay)
        .def("isReplayEnabled", &nlm::NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &nlm::NeuralEpisodicMemory::getEpisodesForReplay)
        .def("replaySequence", &nlm::NeuralEpisodicMemory::replaySequence);
    
    // NeuralPlanner class
    py::class_<nlm::NeuralPlanner>(m, "NeuralPlanner")
        .def("initialize", &nlm::NeuralPlanner::initialize)
        .def("plan", &nlm::NeuralPlanner::plan)
        .def("evaluateSequence", &nlm::NeuralPlanner::evaluateSequence)
        .def("getCurrentPlan", &nlm::NeuralPlanner::getCurrentPlan)
        .def("setGoal", &nlm::NeuralPlanner::setGoal)
        .def("getGoal", &nlm::NeuralPlanner::getGoal)
        .def("reset", &nlm::NeuralPlanner::reset);
    
    // VisualizationInterface class
    py::class_<nlm::VisualizationInterface>(m, "VisualizationInterface")
        .def("update", &nlm::VisualizationInterface::update)
        .def("render", &nlm::VisualizationInterface::render)
        .def("setWindowSize", &nlm::VisualizationInterface::setWindowSize)
        .def("close", &nlm::VisualizationInterface::close);
    
    // SimulationClock class
    py::class_<nlm::SimulationClock>(m, "SimulationClock")
        .def("getTime", &nlm::SimulationClock::getTime)
        .def("step", &nlm::SimulationClock::step)
        .def("reset", &nlm::SimulationClock::reset)
        .def("getTimestep", &nlm::SimulationClock::getTimestep)
        .def("setTimestep", &nlm::SimulationClock::setTimestep);
    
    // Convenience functions
    m.def("createBrain", &createBrain, "Create a brain with default configuration");
    
    // Configuration examples
    m.def("getDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        return config;
    });
    
    // Logging utilities
    m.def("setupLogger", []() {
        auto logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
    });
    
    m.def("logNeuronState", [](const nlm::Neuron& neuron) {
        std::cout << "Neuron " << neuron.getId().value << 
                  " (V=" << neuron.getMembranePotential() << ", "
                  "thresh=" << neuron.getThreshold() << ", "
                  "firing=" << (neuron.isFiring() ? "true" : "false") << ")";
    });
    
    // Benchmarking utility
    m.def("run_benchmark", [](size_t neuron_count, size_t steps) {
        // Simplified benchmark - in a real implementation, this would run the actual benchmark
        std::cout << "Running benchmark with " << neuron_count << " neurons and " << steps << " steps...";
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::cout << " Done!" << std::endl;
        
        return py::dict(
            "neurons", brain->getTotalNeuronCount(),
            "synapses", brain->getTotalSynapseCount(),
            "spikes", brain->getTotalSpikeCount(),
            "steps_per_second", static_cast<double>(steps) / (steps * 0.001)
        );
    });
    
    // Example usage
    m.def("example_tutorial", []() {
        std::cout << "=== NLM Python Tutorial ===" << std::endl;
        std::cout << "1. Create a brain:" << std::endl;
        std::cout << "   config = nlm.getDefaultConfig()" << std::endl;
        std::cout << "   brain = nlm.Brain(config)" << std::endl;
        std::cout << "2. Initialize the brain:" << std::endl;
        std::cout << "   brain.initialize()" << std::endl;
        std::cout << "3. Run simulation:" << std::endl;
        std::cout << "   for step in range(1000):" << std::endl;
        std::cout << "       brain.step(step)" << std::endl;
        std::cout << "4. Check results:" << std::endl;
        std::cout << "   print(f'Neurons: {brain.getTotalNeuronCount()}')" << std::endl;
        std::cout << "   print(f'Synapses: {brain.getTotalSynapseCount()}')" << std::endl;
        std::cout << "   print(f'Spikes: {brain.getTotalSpikeCount()}')" << std::endl;
        std::cout << "=== Tutorial complete ===" << std::endl;
    });
}
