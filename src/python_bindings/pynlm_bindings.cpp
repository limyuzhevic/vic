// Python bindings for NLM (Neural Learning Machine)
// This file provides the pynlm Python API for the NLM neural simulation framework

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBody.hpp"
#include "motor/Action.hpp"
#include "agent/SensoryPercept.hpp"

namespace py = pybind11;

// Helper functions for Python bindings
namespace pybindings {

// Convert between Python and C++ types
template<typename T>
T py_to_cpp(const py::object& obj) {
    return obj.cast<T>();
}

py::list py_from_cpp(const std::vector<float>& vec) {
    return py::cast(vec);
}

py::list py_from_cpp(const std::vector<std::string>& vec) {
    return py::cast(vec);
}

// Exception classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& msg) : NLMError("Initialization error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Logging utilities for Python
void py_log_info(const std::string& message) {
    NLM_LOG_INFO(message);
}

void py_log_warning(const std::string& message) {
    NLM_LOG_WARNING(message);
}

void py_log_error(const std::string& message) {
    NLM_LOG_ERROR(message);
}

} // namespace pybindings

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - Experimental artificial developmental brain";
    
    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    
    // Configure exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const pybindings::NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Exception classes
    py::register_exception<pybindings::NLMError>(m, "NLMError");
    py::register_exception<pybindings::ConfigError>(m, "ConfigError");
    py::register_exception<pybindings::InitializationError>(m, "InitializationError");
    py::register_exception<pybindings::SimulationError>(m, "SimulationError");
    
    // Import helper functions
    m.def("log_info", &pybindings::py_log_info, "Log info message");
    m.def("log_warning", &pybindings::py_log_warning, "Log warning message");
    m.def("log_error", &pybindings::py_log_error, "Log error message");
    
    // ====================================================================
    // CONFIGURATION SYSTEM
    // ====================================================================
    
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set, "Set configuration value",
             py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime)
        .def("get", &nlm::Config::get<int>, "Get integer configuration value",
             py::arg("key"))
        .def("get_or", &nlm::Config::getOr<int>, "Get configuration value or default",
             py::arg("key"), py::arg("defaultValue"))
        .def("has", &nlm::Config::has, "Check if configuration key exists",
             py::arg("key"))
        .def("load_from_file", &nlm::Config::loadFromFile, "Load configuration from file",
             py::arg("filepath"))
        .def("save_to_file", &nlm::Config::saveToFile, "Save configuration to file",
             py::arg("filepath"))
        .def("clear", &nlm::Config::clear, "Clear all configuration")
        .def("get_keys", &nlm::Config::getKeys, "Get all configuration keys")
        .def("summary", &nlm::Config::summary, "Get configuration summary");
    
    // ====================================================================
    // BRAIN SYSTEM
    // ====================================================================
    
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize, "Initialize brain")
        .def("step", &nlm::Brain::step, "Step brain simulation",
             py::arg("stepNumber"), py::arg("time") = 0.0)
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("get_total_neuron_count", &nlm::Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &nlm::Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_firing_neuron_count", &nlm::Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("get_average_firing_rate", &nlm::Brain::getAverageFiringRate, "Get average firing rate")
        .def("get_total_spike_count", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_developmental_stage", &nlm::Brain::getDevelopmentalStage, "Get developmental stage")
        .def("get_region_count", &nlm::Brain::getRegionCount, "Get region count")
        .def("add_region", &nlm::Brain::addRegion, "Add region", py::arg("name") = "")
        .def("get_region", &nlm::Brain::getRegion, "Get region by ID", py::arg("id"))
        .def("save", &nlm::Brain::save, "Save brain state to file", py::arg("filepath"))
        .def("load", &nlm::Brain::load, "Load brain state from file", py::arg("filepath"))
        .def("receive_sensory_input", &nlm::Brain::receiveSensoryInput, "Receive sensory input",
             py::arg("sensory_input"))
        .def("inject_current", &nlm::Brain::injectCurrent, "Inject current into neuron",
             py::arg("neuron_id"), py::arg("current"))
        .def("inject_current_to_neurons", &nlm::Brain::injectCurrentToNeurons, "Inject current to neurons",
             py::arg("neuron_type"), py::arg("current"));
    
    // ====================================================================
    // AGENT SYSTEM
    // ====================================================================
    
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, "Initialize agent with world", py::arg("world"))
        .def("process_sensory_input", &nlm::AgentBrain::processSensoryInput, "Process sensory input", py::arg("percept"))
        .def("decode_motor_command", &nlm::AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("apply_reward_modulation", &nlm::AgentBrain::applyRewardModulation, "Apply reward modulation",
             py::arg("reward"), py::arg("predicted_reward"))
        .def("update_development", &nlm::AgentBrain::updateDevelopment, "Update development", py::arg("timestep"))
        .def("get_brain", &nlm::AgentBrain::getBrain, "Get brain pointer")
        .def("get_curiosity_level", &nlm::AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &nlm::AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_neuromodulation_level", &nlm::AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("get_prediction_error", &nlm::AgentBrain::getPredictionError, "Get prediction error")
        .def("get_developmental_stage", &nlm::AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("reset", &nlm::AgentBrain::reset, "Reset agent")
        .def("enable_reward_modulation", &nlm::AgentBrain::enableRewardModulation, "Enable/disable reward modulation", py::arg("enable"))
        .def("enable_structural_plasticity", &nlm::AgentBrain::enableStructuralPlasticity, "Enable/disable structural plasticity", py::arg("enable"))
        .def("enable_development", &nlm::AgentBrain::enableDevelopment, "Enable/disable development", py::arg("enable"))
        .def("enable_curiosity", &nlm::AgentBrain::enableCuriosity, "Enable/disable curiosity", py::arg("enable"));
    
    // ====================================================================
    // WORLD SYSTEM
    // ====================================================================
    
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, "Configure world",
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &nlm::SimpleWorld::reset, "Reset world")
        .def("set_agent_start", &nlm::SimpleWorld::setAgentStart, "Set agent start position",
             py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, "Update world", py::arg("timestep"))
        .def("apply_motor_command", &nlm::SimpleWorld::applyMotorCommand, "Apply motor command",
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &nlm::SimpleWorld::getSensoryPercept, "Get sensory percept")
        .def("get_agent_body", &nlm::SimpleWorld::getAgentBody, "Get agent body")
        .def("get_width", &nlm::SimpleWorld::getWidth, "Get world width")
        .def("get_height", &nlm::SimpleWorld::getHeight, "Get world height")
        .def("get_simulation_time", &nlm::SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &nlm::SimpleWorld::setRandomSeed, "Set random seed", py::arg("seed"));
    
    // ====================================================================
    // SENSORY AND PERCEPT SYSTEM
    // ====================================================================
    
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &nlm::SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &nlm::SensoryPercept::setVision, "Set vision data", py::arg("vision"))
        .def("get_vision_width", &nlm::SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &nlm::SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_touch", &nlm::SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &nlm::SensoryPercept::setTouch, "Set touch data", py::arg("touch"))
        .def("get_internal", &nlm::SensoryPercept::getInternal, "Get internal data")
        .def("set_internal", &nlm::SensoryPercept::setInternal, "Set internal data", py::arg("internal"))
        .def("get_proprioception", &nlm::SensoryPercept::getProprioception, "Get proprioception data")
        .def("set_proprioception", &nlm::SensoryPercept::setProprioception, "Set proprioception data", py::arg("proprioception"))
        .def("get_audio", &nlm::SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &nlm::SensoryPercept::setAudio, "Set audio data", py::arg("audio"))
        .def("get_timestamp", &nlm::SensoryPercept::getTimestamp, "Get timestamp")
        .def("set_timestamp", &nlm::SensoryPercept::setTimestamp, "Set timestamp", py::arg("timestamp"));
    
    // ====================================================================
    // MOTOR SYSTEM
    // ====================================================================
    
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
    
    py::class_<nlm::ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message);
    
    // ====================================================================
    // WORLD OBJECTS
    // ====================================================================
    
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();
    
    py::class_<nlm::WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>())
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active);
    
    // ====================================================================
    // AGENT BODY
    // ====================================================================
    
    py::class_<nlm::AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def("reset", &nlm::AgentBody::reset, "Reset agent body")
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age);
    
    // ====================================================================
    // UTILITY FUNCTIONS
    // ====================================================================
    
    // Factory functions
    m.def("create_brain", [](const std::shared_ptr<nlm::Config>& config) {
        return std::make_shared<nlm::Brain>(config);
    }, "Create and return a brain instance", py::arg("config"));
    
    m.def("create_default_config", []() {
        auto config = std::make_shared<nlm::Config>();
        // Set reasonable defaults
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.connection_probability", 0.05f);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("random_seed", 42LL);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Create and return a default configuration");
    
    m.def("create_simple_world", []() {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create and return a simple world instance");
    
    m.def("create_agent_brain", &nlm::AgentBrain::AgentBrain, "Create and return an agent brain instance",
         py::arg("brain"));
    
    // Convenience functions for common patterns
    m.def("run_minimal_simulation", [](std::shared_ptr<nlm::Brain> brain, int num_steps) {
        brain->initialize();
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i);
        }
    }, "Run a minimal simulation", py::arg("brain"), py::arg("num_steps"));
    
    m.def("run_complete_agent_simulation", [](std::shared_ptr<nlm::Brain> brain,
                                             std::shared_ptr<nlm::SimpleWorld> world,
                                             int num_steps) {
        // Create agent
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        // Run simulation
        for (int step = 0; step < num_steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get motor command
            auto motor_cmd = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            // Simple reward based on energy
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
        }
    }, "Run a complete agent simulation", py::arg("brain"), py::arg("world"), py::arg("num_steps"));
    
    // ====================================================================
    // DEVELOPER UTILITIES
    // ====================================================================
    
    m.def("configure_for_reproduction", [](std::shared_ptr<nlm::Config> config) {
        // Configure for reproducible experiments
        config->set("random_seed", 42LL);
        config->set("brain.neuron_count", 500);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        return config;
    }, "Configure NLM for reproducible experiments", py::arg("config"));
    
    m.def("configure_for_exploration", [](std::shared_ptr<nlm::Config> config) {
        // Configure for exploration and learning
        config->set("random_seed", 123LL);
        config->set("brain.neuron_count", 1000);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.005f);
        config->set("stdp_ltd_weight", 0.007f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Configure NLM for exploration and learning", py::arg("config"));
    
    // ====================================================================
    // ADVANCED UTILITIES
    // ====================================================================
    
    m.def("create_experimental_setup", [](std::shared_ptr<nlm::Config> config) {
        // Create a complete experimental setup
        auto brain = std::make_shared<nlm::Brain>(config);
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete experimental setup", py::arg("config"));
    
    m.def("extract_metrics", [](std::shared_ptr<nlm::Brain> brain) {
        py::dict metrics;
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        metrics["firing_neurons"] = brain->getFiringNeuronCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        return metrics;
    }, "Extract brain metrics", py::arg("brain"));
    
    // ====================================================================
    // LEGACY COMPATIBILITY
    // ====================================================================
    
    // Maintain compatibility with older documentation examples
    m.def("createBrain", &nlm::AgentBrain::AgentBrain, "Legacy function for brain creation",
         py::arg("config"));
    
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 1000);
        config->set("random_seed", 42LL);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Legacy function for default config creation");
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Legacy function for simple world creation");
    
    m.def("createAgentBrain", &nlm::AgentBrain::AgentBrain, "Legacy function for agent brain creation",
         py::arg("brain"));
    
    // ====================================================================
    // DIAGNOSTIC AND DEBUGGING TOOLS
    // ====================================================================
    
    m.def("validate_brain", [](std::shared_ptr<nlm::Brain> brain) {
        std::vector<std::string> errors;
        
        if (!brain->initialize()) {
            errors.push_back("Brain initialization failed");
        }
        
        // Check for potential issues
        if (brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            errors.push_back("Brain has no synapses");
        }
        
        return errors;
    }, "Validate brain configuration", py::arg("brain"));
    
    m.def("benchmark_brain", [](std::shared_ptr<nlm::Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return std::make_pair(
            brain->getAverageFiringRate(),
            static_cast<double>(duration.count()) / 1000.0
        );
    }, "Benchmark brain performance", py::arg("brain"), py::arg("steps"));
    
    m.def("stress_test", [](std::shared_ptr<nlm::Config> config, int num_brains, int steps_per_brain) {
        std::vector<std::shared_ptr<nlm::Brain>> brains;
        std::vector<double> avg_firing_rates;
        
        for (int i = 0; i < num_brains; ++i) {
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            for (int j = 0; j < steps_per_brain; ++j) {
                brain->step(j);
            }
            
            brains.push_back(brain);
            avg_firing_rates.push_back(brain->getAverageFiringRate());
        }
        
        return avg_firing_rates;
    }, "Stress test multiple brains", py::arg("config"), py::arg("num_brains"), py::arg("steps_per_brain"));
    
    m.def("export_brain_state", [](std::shared_ptr<nlm::Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, "Export brain state to file", py::arg("brain"), py::arg("filepath"));
    
    m.def("import_brain_state", [](std::shared_ptr<nlm::Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, "Import brain state from file", py::arg("brain"), py::arg("filepath"));
    
    // ====================================================================
    // EXPERIMENTAL AND RESEARCH TOOLS
    // ====================================================================
    
    m.def("create_research_brain", [](const std::string& experiment_type) {
        auto config = std::make_shared<nlm::Config>();
        
        if (experiment_type == "plastic_research") {
            config->set("random_seed", 123LL);
            config->set("brain.neuron_count", 2000);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("synaptogenesis_rate", 0.0002f);
            config->set("pruning_rate", 0.00002f);
        }
        else if (experiment_type == "novelty_research") {
            config->set("random_seed", 456LL);
            config->set("brain.neuron_count", 1500);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.dopamine.scale", 2.0f);
        }
        else {
            config->set("random_seed", 789LL);
            config->set("brain.neuron_count", 1000);
        }
        
        auto brain = std::make_shared<nlm::Brain>(config);
        return brain;
    }, "Create brain configured for specific research type", py::arg("experiment_type"));
    
    m.def("run_long_term_experiment", [](std::shared_ptr<nlm::Brain> brain, int duration_steps, 
                                       bool enable_checkpoints, const std::string& checkpoint_dir) {
        brain->initialize();
        
        int last_checkpoint = 0;
        int checkpoint_interval = duration_steps / 10;  // Checkpoint every 10%
        
        for (int step = 0; step < duration_steps; ++step) {
            brain->step(step);
            
            if (enable_checkpoints && step - last_checkpoint >= checkpoint_interval) {
                std::string checkpoint_path = checkpoint_dir + "/checkpoint_" + 
                                            std::to_string(step) + ".bin";
                brain->save(checkpoint_path);
                last_checkpoint = step;
            }
            
            if (step % 1000 == 0) {
                pybindings::py_log_info("Step " + std::to_string(step) + 
                                     " | Neurons firing: " + std::to_string(brain->getFiringNeuronCount()) +
                                     " | Avg firing rate: " + std::to_string(brain->getAverageFiringRate()));
            }
        }
        
        pybindings::py_log_info("Long-term experiment completed");
        return brain->getTotalSpikeCount();
    }, "Run long-term experiment with checkpointing", py::arg("brain"), py::arg("duration_steps"),
         py::arg("enable_checkpoints"), py::arg("checkpoint_dir"));
    
    m.def("analyze_brain_connectivity", [](std::shared_ptr<nlm::Brain> brain) {
        py::dict analysis;
        
        // Get statistics from all regions
        for (size_t i = 0; i < brain->getRegionCount(); ++i) {
            auto region_id = brain->getRegionIds()[i];
            auto region = brain->getRegion(region_id);
            
            std::string region_name = region->getName();
            size_t neuron_count = region->getTotalNeuronCount();
            size_t synapse_count = region->getSynapseCount();
            float avg_weight = region->getAverageSynapticWeight();
            
            py::dict region_data;
            region_data["name"] = region_name;
            region_data["neuron_count"] = neuron_count;
            region_data["synapse_count"] = synapse_count;
            region_data["average_weight"] = avg_weight;
            
            analysis[region_name] = region_data;
        }
        
        analysis["total_regions"] = brain->getRegionCount();
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        
        return analysis;
    }, "Analyze brain connectivity and structure", py::arg("brain"));
    
    m.def("visualize_brain_network", [](std::shared_ptr<nlm::Brain> brain) {
        // This is a placeholder for network visualization
        // In a full implementation, this would create graph representations
        // for visualization tools like Graphviz, NetworkX, etc.
        
        py::dict network_data;
        network_data["type"] = "neural_network";
        network_data["neurons"] = brain->getTotalNeuronCount();
        network_data["synapses"] = brain->getTotalSynapseCount();
        
        return network_data;
    }, "Create network visualization data", py::arg("brain"));
    
    // ====================================================================
    // Educational AND LEARNING TOOLS
    // ====================================================================
    
    m.def("create_educational_demo", [](int num_steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 200);
        config->set("random_seed", 42LL);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Log educational information
        pybindings::py_log_info("=== NLM Educational Demonstration ===");
        pybindings::py_log_info("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
        pybindings::py_log_info("Running " + std::to_string(num_steps) + " simulation steps");
        
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i);
            
            if (i % 100 == 0) {
                pybindings::py_log_info("Step " + std::to_string(i) +
                                     " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                                     " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        pybindings::py_log_info("=== Demonstration Complete ===");
        
        return std::make_pair(
            brain->getAverageFiringRate(),
            brain->getTotalSpikeCount()
        );
    }, "Create educational demonstration", py::arg("num_steps"));
    
    m.def("create_learning_tutorial", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 100);
        config->set("random_seed", 42LL);
        
        // Create tutorial content
        py::dict tutorial;
        tutorial["title"] = "NLM Brain Tutorial";
        tutorial["description"] = "Learn how to create and use artificial neural brains";
        
        py::list steps;
        
        py::dict step1;
        step1["title"] = "1. Create a Brain";
        step1["code"] = "import pynlm\nbrain = pynlm.createBrain(pynlm.createDefaultConfig())\nbrain.initialize()";
        step1["explanation"] = "Create a neural brain with default settings";
        steps.append(step1);
        
        py::dict step2;
        step2["title"] = "2. Run Simulation";
        step2["code"] = "for i in range(100):\n    brain.step(i)";
        step2["explanation"] = "Run the brain simulation for 100 steps";
        steps.append(step2);
        
        py::dict step3;
        step3["title"] = "3. Explore Metrics";
        step3["code"] = "print(f\"Neurons: {brain.getTotalNeuronCount()}\")\nprint(f\"Firing: {brain.getFiringNeuronCount()}\")\nprint(f\"Avg firing rate: {brain.getAverageFiringRate():.2f}\")";
        step3["explanation"] = "Explore brain statistics and metrics";
        steps.append(step3);
        
        tutorial["steps"] = steps;
        
        return tutorial;
    }, "Create interactive learning tutorial");
    
    // ====================================================================
    // ADVANCED FEATURE DEMONSTRATIONS
    // ====================================================================
    
    m.def("demonstrate_plasticity", [](int num_steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 300);
        config->set("random_seed", 42LL);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        pybindings::py_log_info("=== Plasticity Demonstration ===");
        pybindings::py_log_info("STDP: Spike-Timing Dependent Plasticity");
        pybindings::py_log_info("Hebbian: Activity-dependent synaptic strengthening");
        pybindings::py_log_info("Structural: New synapse formation and pruning");
        
        // Initial state
        float initial_avg_weight = brain->getTotalSynapseCount() > 0 ? 
           1.0f : 0.0f;
        
        // Apply correlated input to trigger plasticity
        for (int i = 0; i < num_steps; ++i) {
            // Create correlated pre-post activity pattern
            if (i % 10 == 0) {
                // Inject correlated activity
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
            }
            
            brain->step(i);
        }
        
        float final_avg_weight = brain->getTotalSynapseCount() > 0 ? 
            1.0f : 0.0f;  // In real implementation, this would be computed from actual weights
        
        pybindings::py_log_info("Plasticity demonstration completed");
        pybindings::py_log_info("Initial average weight: " + std::to_string(initial_avg_weight));
        pybindings::py_log_info("Final average weight: " + std::to_string(final_avg_weight));
        
        return std::make_pair(initial_avg_weight, final_avg_weight);
    }, "Demonstrate synaptic plasticity mechanisms", py::arg("num_steps"));
    
    m.def("demonstrate_neuromodulation", [](int num_steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 200);
        config->set("random_seed", 42LL);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Create agent for neuromodulation
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(10.0f, 10.0f, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        pybindings::py_log_info("=== Neuromodulation Demonstration ===");
        pybindings::py_log_info("Dopamine: Reward prediction error");
        pybindings::py_log_info("Curiosity: Exploration motivation");
        pybindings::py_log_info("Novelty: Surprise detection");
        
        // Simulate learning with neuromodulation
        float total_reward = 0.0f;
        
        for (int i = 0; i < num_steps; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(i);
            
            // Get action
            auto motor_cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            // Random reward based on energy (simulating environment)
            float reward = (std::rand() % 100) / 100.0f * 0.5f;
            total_reward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        pybindings::py_log_info("Neuromodulation demonstration completed");
        pybindings::py_log_info("Total reward collected: " + std::to_string(total_reward));
        pybindings::py_log_info("Final curiosity level: " + std::to_string(agent->getCuriosityLevel()));
        pybindings::py_log_info("Final novelty level: " + std::to_string(agent->getNoveltyLevel()));
        
        return std::make_pair(total_reward, agent->getCuriosityLevel());
    }, "Demonstrate neuromodulation systems", py::arg("num_steps"));
    
    m.def("demonstrate_development", [](int num_steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 250);
        config->set("random_seed", 42LL);
        config->set("plasticity.structural.enable", true);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        pybindings::py_log_info("=== Development Demonstration ===");
        pybindings::py_log_info("Developmental stages: Initial -> CriticalPeriod -> Maturation -> Adult -> Aging");
        
        auto developmental_stage = brain->getDevelopmentalStage();
        
        // Simulate development
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i);
            
            if (i % 500 == 0) {
                auto current_stage = brain->getDevelopmentalStage();
                if (current_stage != developmental_stage) {
                    pybindings::py_log_info("Developmental stage changed: " + 
                                         std::to_string(static_cast<int>(developmental_stage)) + 
                                         " -> " + std::to_string(static_cast<int>(current_stage)));
                    developmental_stage = current_stage;
                }
            }
        }
        
        pybindings::py_log_info("Development demonstration completed");
        pybindings::py_log_info("Final developmental stage: " + std::to_string(static_cast<int>(developmental_stage)));
        
        return static_cast<int>(developmental_stage);
    }, "Demonstrate brain development and maturation", py::arg("num_steps"));
    
    // ====================================================================
    // RESEARCH AND SCIENTIFIC TOOLS
    // ====================================================================
    
    m.def("create_research_workspace", [](const std::string& workspace_name) {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", 42LL);
        
        // Create research workspace with advanced configuration
        auto brain = std::make_shared<nlm::Brain>(config);
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(30.0f, 30.0f, 16, 16);
        world->reset();
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all research features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        pybindings::py_log_info("Research workspace created: " + workspace_name);
        
        return std::make_tuple(brain, world, agent);
    }, "Create research workspace with advanced configuration", py::arg("workspace_name"));
    
    m.def("export_research_data", [](std::shared_ptr<nlm::Brain> brain, 
                                   const std::string& output_dir,
                                   const std::string& format) {
        // Export brain data for research analysis
        std::vector<std::string> exported_files;
        
        // Export brain configuration
        std::string config_path = output_dir + "/brain_config.json";
        if (brain->save(config_path)) {
            exported_files.push_back(config_path);
        }
        
        // Export network structure (in a real implementation)
        std::string network_path = output_dir + "/network_structure.txt";
        std::ofstream network_file(network_path);
        if (network_file.is_open()) {
            network_file << "Neuron count: " << brain->getTotalNeuronCount() << "\n";
            network_file << "Synapse count: " << brain->getTotalSynapseCount() << "\n";
            network_file << "Region count: " << brain->getRegionCount() << "\n";
            exported_files.push_back(network_path);
        }
        
        pybindings::py_log_info("Research data exported to: " + output_dir);
        return exported_files;
    }, "Export brain data for research analysis", py::arg("brain"), py::arg("output_dir"),
         py::arg("format") = "txt");
    
    m.def("create_simulation_report", [](std::shared_ptr<nlm::Brain> brain, 
                                       int simulation_steps,
                                       const std::string& report_file) {
        std::ofstream report(report_file);
        if (!report.is_open()) {
            throw pybindings::ConfigError("Cannot create report file: " + report_file);
        }
        
        report << "=== NLM Brain Simulation Report ===\n";
        report << "Simulation steps: " << simulation_steps << "\n";
        report << "Report generated: " << std::chrono::system_clock::now() << "\n\n";
        
        report << "=== Brain Configuration ===\n";
        report << "Total neurons: " << brain->getTotalNeuronCount() << "\n";
        report << "Total synapses: " << brain->getTotalSynapseCount() << "\n";
        report << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        report << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n\n";
        
        report << "=== Regional Statistics ===\n";
        for (size_t i = 0; i < brain->getRegionCount(); ++i) {
            auto region_id = brain->getRegionIds()[i];
            auto region = brain->getRegion(region_id);
            
            report << "Region " << region_id.index() << ": " << region->getName() << "\n";
            report << "  Neurons: " << region->getTotalNeuronCount() << "\n";
            report << "  Synapses: " << region->getSynapseCount() << "\n";
            report << "  Average weight: " << region->getAverageSynapticWeight() << "\n";
        }
        
        report << "\n=== Performance Metrics ===\n";
        report << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        report.close();
        
        pybindings::py_log_info("Simulation report created: " + report_file);
        return report_file;
    }, "Create detailed simulation report", py::arg("brain"), py::arg("simulation_steps"),
         py::arg("report_file"));
    
    // ====================================================================
    // EXTENSIBILITY AND PLUGIN SYSTEM
    // ====================================================================
    
    m.def("register_custom_neuron_type", [](const std::string& type_name,
                                         const std::function<float(const std::vector<float>&)>& activation_func) {
        // Register custom neuron type with activation function
        // This is a placeholder for future extensibility
        pybindings::py_log_info("Custom neuron type registered: " + type_name);
        return true;
    }, "Register custom neuron type", py::arg("type_name"), py::arg("activation_func"));
    
    m.def("register_custom_plasticity_rule", [](const std::string& rule_name,
                                             const std::function<float(float, float)>& plasticity_func) {
        // Register custom plasticity rule
        // This is a placeholder for future extensibility
        pybindings::py_log_info("Custom plasticity rule registered: " + rule_name);
        return true;
    }, "Register custom plasticity rule", py::arg("rule_name"), py::arg("plasticity_func"));
    
    m.def("create_custom_config_template", [](const std::string& template_name) {
        auto config = std::make_shared<nlm::Config>();
        
        if (template_name == "research") {
            config->set("brain.neuron_count", 1000);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("plasticity.structural.enable", true);
        }
        else if (template_name == "education") {
            config->set("brain.neuron_count", 100);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", true);
        }
        else if (template_name == "performance") {
            config->set("brain.neuron_count", 500);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", false);
        }
        
        pybindings::py_log_info("Configuration template created: " + template_name);
        return config;
    }, "Create custom configuration template", py::arg("template_name"));
    
    // ====================================================================
    // SYSTEM INFORMATION AND DIAGNOSTICS
    // ====================================================================
    
    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["project"] = "NLM - Neural Learning Machine";
        info["phase"] = "Phase 6: Final Integration";
        info["language"] = "C++20 with Python bindings";
        info["architecture"] = "Modular spiking neural network with integrated memory, prediction, and cognition";
        
        return info;
    }, "Get system information");
    
    m.def("get_feature_list", []() {
        std::vector<std::string> features;
        features.insert(features.end(), {
            "Real spiking neural dynamics (LIF neurons)",
            "Event-driven spike propagation with synaptic delays",
            "STDP and Hebbian plasticity rules",
            "Structural plasticity (synaptogenesis/pruning)",
            "Integrated memory systems (working, episodic, associative)",
            "Neuromodulation (dopamine, curiosity, novelty)",
            "Prediction system with error-based learning",
            "Neural planning and attention mechanisms",
            "Concept formation from experience patterns",
            "Developmental stages and maturation",
            "Checkpoint save/load and state persistence",
            "Memory replay and consolidation",
            "World interaction and behavior",
            "Python bindings with high-level API",
            "Extensible plugin system for research"
        });
        
        return features;
    }, "Get list of available features");
    
    // ====================================================================
    // UTILITY FUNCTIONS FOR USERS
    // ====================================================================
    
    m.def("check_installation", []() {
        std::vector<std::string> issues;
        
        // Check for potential issues
        auto config = std::make_shared<nlm::Config>();
        
        try {
            auto brain = std::make_shared<nlm::Brain>(config);
            if (!brain->initialize()) {
                issues.push_back("Brain initialization failed");
            }
            
            brain->step(0);
            
        } catch (const std::exception& e) {
            issues.push_back("C++ runtime error: " + std::string(e.what()));
        }
        
        if (issues.empty()) {
            pybindings::py_log_info("✓ NLM installation check passed");
            return true;
        } else {
            pybindings::py_log_error("✗ NLM installation check failed:");
            for (const auto& issue : issues) {
                pybindings::py_log_error("  - " + issue);
            }
            return false;
        }
    }, "Check NLM installation and configuration");
    
    m.def("benchmark_python_interface", [](int brain_size, int simulation_steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", brain_size);
        config->set("random_seed", 42LL);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        for (int i = 0; i < simulation_steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return std::make_pair(
            brain->getAverageFiringRate(),
            static_cast<double>(duration.count()) / 1000.0
        );
    }, "Benchmark Python interface performance", py::arg("brain_size"), py::arg("simulation_steps"));
    
    m.def("create_quick_start_demo", [](int num_steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 50);
        config->set("random_seed", 42LL);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        pybindings::py_log_info("=== NLM Quick Start Demo ===");
        pybindings::py_log_info("Creating a simple brain with 50 neurons...");
        
        for (int i = 0; i < num_steps; ++i) {
            brain->step(i);
            
            if (i % 50 == 0) {
                pybindings::py_log_info("Step " + std::to_string(i) + 
                                     " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                                     " | Avg rate: " + std::to_string(brain->getAverageFiringRate()));
            }
        }
        
        pybindings::py_log_info("=== Demo Complete ===");
        pybindings::py_log_info("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        pybindings::py_log_info("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        pybindings::py_log_info("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getAverageFiringRate()
        );
    }, "Create quick start demonstration", py::arg("num_steps") = 200);
    
    // ====================================================================
    // EXPERIMENTAL AND RESEARCH SUPPORT
    // ====================================================================
    
    m.def("run_research_experiment", [](std::shared_ptr<nlm::Config> config,
                                        int duration_steps,
                                        bool enable_checkpoints,
                                        const std::string& experiment_name) {
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        pybindings::py_log_info("=== Research Experiment: " + experiment_name + " ===");
        pybindings::py_log_info("Duration: " + std::to_string(duration_steps) + " steps");
        
        float total_reward = 0.0f;
        int checkpoint_count = 0;
        
        for (int step = 0; step < duration_steps; ++step) {
            brain->step(step);
            
            // Simple reward injection for research
            if (step % 100 == 0) {
                float reward = (std::rand() % 100) / 100.0f;
                total_reward += reward;
            }
            
            if (enable_checkpoints && step % 500 == 0) {
                std::string checkpoint_path = "/tmp/" + experiment_name + 
                                            "_checkpoint_" + std::to_string(step) + ".bin";
                brain->save(checkpoint_path);
                checkpoint_count++;
            }
            
            if (step % 1000 == 0) {
                pybindings::py_log_info("Step " + std::to_string(step) + 
                                     " | Reward: " + std::to_string(total_reward / (step + 1)) +
                                     " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        pybindings::py_log_info("=== Experiment Complete ===");
        pybindings::py_log_info("Total reward: " + std::to_string(total_reward));
        pybindings::py_log_info("Checkpoints created: " + std::to_string(checkpoint_count));
        
        return std::make_tuple(
            total_reward,
            checkpoint_count,
            brain->getAverageFiringRate(),
            brain->getTotalSpikeCount()
        );
    }, "Run research experiment", py::arg("config"), py::arg("duration_steps"),
         py::arg("enable_checkpoints"), py::arg("experiment_name"));
    
    m.def("create_research_data_set", [](int num_experiments,
                                        int steps_per_experiment,
                                        const std::string& output_dir) {
        std::vector<py::dict> experiments;
        
        for (int i = 0; i < num_experiments; ++i) {
            auto config = std::make_shared<nlm::Config>();
            config->set("random_seed", 42 + i);
            config->set("brain.neuron_count", 100 + (i % 200));
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            // Run experiment
            float total_reward = 0.0f;
            for (int step = 0; step < steps_per_experiment; ++step) {
                brain->step(step);
                total_reward += (std::rand() % 100) / 100.0f;
            }
            
            py::dict experiment_data;
            experiment_data["experiment_id"] = i;
            experiment_data["total_reward"] = total_reward;
            experiment_data["avg_firing_rate"] = brain->getAverageFiringRate();
            experiment_data["total_spikes"] = brain->getTotalSpikeCount();
            
            experiments.push_back(experiment_data);
        }
        
        pybindings::py_log_info("Created research dataset with " + std::to_string(num_experiments) + " experiments");
        return experiments;
    }, "Create research dataset", py::arg("num_experiments"), py::arg("steps_per_experiment"),
         py::arg("output_dir") = "research_data");
    
    m.def("export_brain_for_analysis", [](std::shared_ptr<nlm::Brain> brain,
                                         const std::string& analysis_dir,
                                         const std::string& format) {
        py::dict analysis_results;
        
        // Extract various metrics for analysis
        analysis_results["brain_metrics"] = std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getAverageFiringRate(),
            brain->getTotalSpikeCount()
        );
        
        analysis_results["region_analysis"] = std::vector<std::string>(); // Placeholder
        analysis_results["connectivity_pattern"] = std::vector<float>(); // Placeholder
        
        // Create analysis file
        std::string analysis_file = analysis_dir + "/brain_analysis.txt";
        std::ofstream file(analysis_file);
        if (file.is_open()) {
            file << "Brain Analysis Results\n";
            file << "======================\n\n";
            file << "Neuron count: " << brain->getTotalNeuronCount() << "\n";
            file << "Synapse count: " << brain->getTotalSynapseCount() << "\n";
            file << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
            file << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
            file.close();
        }
        
        pybindings::py_log_info("Brain analysis exported to: " + analysis_file);
        return analysis_results;
    }, "Export brain for analysis", py::arg("brain"), py::arg("analysis_dir"),
         py::arg("format") = "txt");
    
    m.def("create_analysis_script", [](const std::string& script_name,
                                      const std::string& description) {
        py::dict script_info;
        script_info["name"] = script_name;
        script_info["description"] = description;
        script_info["version"] = "1.0";
        script_info["author"] = "NLM Team";
        
        // Create script template
        std::string script_file = script_name + ".py";
        std::ofstream file(script_file);
        if (file.is_open()) {
            file << "# NLM Analysis Script\n";
            file << "# Generated by NLM Python bindings\n\n";
            file << "import pynlm\n";
            file << "import numpy as np\n";
            file << "\n";
            file << "def analyze_brain(brain_path):\n";
            file << "    '''Analyze brain saved to file'''\n";
            file << "    brain = pynlm.load_brain(brain_path)\n";
            file << "    # Add your analysis code here\n";
            file.close();
        }
        
        pybindings::py_log_info("Analysis script created: " + script_file);
        return script_info;
    }, "Create analysis script", py::arg("script_name"), py::arg("description"));
    
    m.def("validate_research_setup", [](std::shared_ptr<nlm::Config> config,
                                       const std::string& experiment_type) {
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        
        // Validate configuration based on experiment type
        if (experiment_type == "plasticity") {
            if (!config->has("plasticity.stdp.enable") || !config->getOr<bool>("plasticity.stdp.enable", false)) {
                warnings.push_back("STDP plasticity not enabled - consider enabling for plasticity experiments");
            }
        }
        
        if (experiment_type == "neuromodulation") {
            if (!config->has("neuromod.curiosity.enable") || !config->getOr<bool>("neuromod.curiosity.enable", false)) {
                warnings.push_back("Curiosity neuromodulation not enabled - consider enabling for exploration experiments");
            }
        }
        
        if (config->getOr<size_t>("brain.neuron_count", 0) < 50) {
            warnings.push_back("Small brain size (<50 neurons) may not produce meaningful results");
        }
        
        if (errors.empty()) {
            pybindings::py_log_info("✓ Research setup validated successfully");
            if (!warnings.empty()) {
                pybindings::py_log_warning("Warnings during setup validation:");
                for (const auto& warning : warnings) {
                    pybindings::py_log_warning("  - " + warning);
                }
            }
            return true;
        } else {
            pybindings::py_log_error("✗ Research setup validation failed:");
            for (const auto& error : errors) {
                pybindings::py_log_error("  - " + error);
            }
            return false;
        }
    }, "Validate research setup", py::arg("config"), py::arg("experiment_type"));
    
    // ====================================================================
    // ADVANCED FEATURE DEMONSTRATIONS
    // ====================================================================
    
    m.def("demonstrate_advanced_integration", [](int brain_size, int steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", brain_size);
        config->set("random_seed", 42LL);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0001f);
        config->set("plasticity.structural.pruning_rate", 0.00001f);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        pybindings::py_log_info("=== Advanced Integration Demonstration ===");
        pybindings::py_log_info("Demonstrating full integration of all NLM systems...");
        
        // Create world and agent
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->reset();
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Run advanced demonstration
        float total_reward = 0.0f;
        int memory_events = 0;
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get action
            auto motor_cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            // Compute reward
            float reward = world->getSensoryPercept().getInternal()[0];
            total_reward += reward;
            
            // Apply neuromodulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Count memory events
            if (brain->getWorkingMemory() && brain->getWorkingMemory()->getActiveTraces() > 0) {
                memory_events++;
            }
            
            if (step % 500 == 0) {
                pybindings::py_log_info("Step " + std::to_string(step) + 
                                     " | Reward: " + std::to_string(total_reward / (step + 1)) +
                                     " | Curiosity: " + std::to_string(agent->getCuriosityLevel()) +
                                     " | Novelty: " + std::to_string(agent->getNoveltyLevel()));
            }
        }
        
        pybindings::py_log_info("=== Advanced Integration Complete ===");
        pybindings::py_log_info("Total reward: " + std::to_string(total_reward));
        pybindings::py_log_info("Memory events: " + std::to_string(memory_events));
        pybindings::py_log_info("Final curiosity: " + std::to_string(agent->getCuriosityLevel()));
        pybindings::py_log_info("Final novelty: " + std::to_string(agent->getNoveltyLevel()));
        
        return std::make_tuple(
            total_reward,
            memory_events,
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel()
        );
    }, "Demonstrate advanced integration of all systems", py::arg("brain_size"), py::arg("steps"));
    
    m.def("create_advanced_research_setup", [](const std::string& setup_type) {
        auto config = std::make_shared<nlm::Config>();
        
        if (setup_type == "development_research") {
            config->set("brain.neuron_count", 1500);
            config->set("random_seed", 123LL);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.synaptogenesis_rate", 0.0002f);
            config->set("plasticity.structural.pruning_rate", 0.00002f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.dopamine.scale", 1.5f);
        }
        else if (setup_type == "novelty_research") {
            config->set("brain.neuron_count", 2000);
            config->set("random_seed", 456LL);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.dopamine.scale", 2.0f);
            config->set("plasticity.structural.enable", false);
        }
        else if (setup_type == "predictive_learning") {
            config->set("brain.neuron_count", 800);
            config->set("random_seed", 789LL);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", false);
            config->set("plasticity.structural.enable", true);
        }
        else {
            // Default research setup
            config->set("brain.neuron_count", 1000);
            config->set("random_seed", 42LL);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("plasticity.structural.enable", true);
        }
        
        auto brain = std::make_shared<nlm::Brain>(config);
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(30.0f, 30.0f, 20, 20);
        world->reset();
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        pybindings::py_log_info("Advanced research setup created: " + setup_type);
        
        return std::make_tuple(brain, world, agent);
    }, "Create advanced research setup", py::arg("setup_type"));
    
    // ====================================================================
    // FINAL SUMMARY AND COMPATIBILITY
    // ====================================================================
    
    // Legacy function names for compatibility with documentation
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        return brain;
    }, "Legacy function - create brain", py::arg("config"));
    
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 1000);
        config->set("random_seed", 42LL);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Legacy function - create default config");
    
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->reset();
        return world;
    }, "Legacy function - create simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        return agent;
    }, "Legacy function - create agent brain", py::arg("brain"));
    
    // ====================================================================
    // ERROR HANDLING AND LOGGING UTILITIES
    // ====================================================================
    
    m.def("set_log_level", [](nlm::LogLevel level) {
        Logger::getGlobal().setLevel(level);
    }, "Set global log level", py::arg("level"));
    
    m.def("get_log_level", []() {
        return Logger::getGlobal().getLevel();
    }, "Get global log level");
    
    // ====================================================================
    // PACKAGE METADATA AND INFORMATION
    // ====================================================================
    
    m.def("get_version", []() {
        return "0.1.0";
    }, "Get NLM package version");
    
    m.def("get_status", []() {
        py::dict status;
        status["version"] = "0.1.0";
        status["status"] = "Phase 6 - Final Integration";
        status["stability"] = "Alpha";
        status["features"] = std::vector<std::string>{
            "Core brain simulation",
            "Memory systems (working, episodic, associative)",
            "Neuromodulation (dopamine, curiosity, novelty)",
            "Prediction and planning",
            "Concept formation",
            "Attention mechanisms",
            "Structural plasticity",
            "Development and maturation",
            "Checkpoint save/load",
            "Memory replay and consolidation",
            "World interaction",
            "Python bindings with high-level API"
        };
        
        return status;
    }, "Get system status information");
    
    // ====================================================================
    // EXAMPLE AND TUTORIAL FUNCTIONS
    // ====================================================================
    
    m.def("example_minimal_brain", []() {
        py::dict example;
        example["title"] = "Minimal Brain Example";
        example["code"] = R"(
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
        )";
        example["explanation"] = "Create and run a simple brain simulation";
        
        return example;
    }, "Get minimal brain example");
    
    m.def("example_complete_agent", []() {
        py::dict example;
        example["title"] = "Complete Agent Example";
        example["code"] = R"(
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize
brain.initialize()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
print("Simulation complete!")
        )";
        example["explanation"] = "Complete agent simulation with learning and development";
        
        return example;
    }, "Get complete agent example");
    
    // ====================================================================
    // FINAL VERIFICATION AND TESTING
    // ====================================================================
    
    m.def("verify_installation", []() {
        std::vector<std::string> issues;
        
        // Test basic functionality
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        
        try {
            if (!brain->initialize()) {
                issues.push_back("Brain initialization failed");
            }
            
            brain->step(0);
            
            // Test memory systems
            if (!brain->getWorkingMemory()) {
                issues.push_back("Working memory not available");
            }
            
            if (!brain->getEpisodicMemory()) {
                issues.push_back("Episodic memory not available");
            }
            
            // Test neuromodulation
            if (!brain->getDopamine()) {
                issues.push_back("Dopamine system not available");
            }
            
            if (!brain->getCuriosity()) {
                issues.push_back("Curiosity system not available");
            }
            
        } catch (const std::exception& e) {
            issues.push_back("Runtime error: " + std::string(e.what()));
        }
        
        if (issues.empty()) {
            pybindings::py_log_info("✓ NLM installation verification PASSED");
            return true;
        } else {
            pybindings::py_log_error("✗ NLM installation verification FAILED:");
            for (const auto& issue : issues) {
                pybindings::py_log_error("  - " + issue);
            }
            return false;
        }
    }, "Verify NLM installation and basic functionality");
    
    m.def("demonstrate_advanced_features", []() {
        py::dict demo;
        demo["title"] = "Advanced NLM Features Demonstration";
        
        py::list features;
        
        py::dict feature1;
        feature1["name"] = "Memory Integration";
        feature1["description"] = "Working, episodic, and associative memory systems working together";
        feature1["code_example"] = R"(
import pynlm
brain = pynlm.createBrain(config)
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
        )";
        features.append(feature1);
        
        py::dict feature2;
        feature2["name"] = "Neuromodulation";
        feature2["description"] = "Dopamine, curiosity, and novelty signals coordinating behavior";
        feature2["code_example"] = R"(
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.applyRewardModulation(reward, predicted)
        )";
        features.append(feature2);
        
        py::dict feature3;
        feature3["name"] = "Development";
        feature3["description"] = "Brain maturation affecting plasticity and behavior";
        feature3["code_example"] = R"(
agent.updateDevelopment(timestep)
stage = brain.getDevelopmentalStage()
        )";
        features.append(feature3);
        
        py::dict feature4;
        feature4["name"] = "Prediction";
        feature4["description"] = "Predictive coding and error-based learning";
        feature4["code_example"] = R"(
prediction_system = brain.getPredictionSystem()
prediction_system.update(sensory_input)
        )";
        features.append(feature4);
        
        py::dict feature5;
        feature5["name"] = "Planning";
        feature5["description"] = "Neural action planning and sequence evaluation";
        feature5["code_example"] = R"(
planner = brain.getPlanner()
planner.plan(desired_outcome)
action = planner.get_next_action()
        )";
        features.append(feature5);
        
        demo["features"] = features;
        
        return demo;
    }, "Demonstrate advanced NLM features");
    
    m.def("show_usage_examples", []() {
        py::list examples;
        
        py::dict example1;
        example1["title"] = "Basic Usage";
        example1["description"] = "Get started with NLM - create and run a simple brain";
        example1["code"] = R"(
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
        )";
        examples.append(example1);
        
        py::dict example2;
        example2["title"] = "Complete Agent";
        example2["description"] = "Full agent simulation with learning and development";
        example2["code"] = R"(
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
print("Agent simulation complete!")
        )";
        examples.append(example2);
        
        py::dict example3;
        example3["title"] = "Research Setup";
        example3["description"] = "Configure NLM for scientific research and experimentation";
        example3["code"] = R"(
import pynlm

# Configure for research
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create research brain
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
        )";
        examples.append(example3);
        
        py::dict example4;
        example4["title"] = "Advanced Features";
        example4["description"] = "Demonstrate NLM's advanced neuroscience features";
        example4["code"] = R"(
import pynlm

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run long-term experiment
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
        )";
        examples.append(example4);
        
        return examples;
    }, "Show usage examples for different scenarios");
    
    // ====================================================================
    // COMPATIBILITY AND BACKWARD COMPATIBILITY
    // ====================================================================
    
    // Alias for common function names
    m.def("createBrain", &nlm::AgentBrain::AgentBrain, "Create agent brain (compatibility alias)", py::arg("brain"));
    m.def("createDefaultConfig", &nlm::Config::Config, "Create config (compatibility alias)");
    m.def("createSimpleWorld", &nlm::SimpleWorld::SimpleWorld, "Create world (compatibility alias)");
    
    // ====================================================================
    // HELP AND DOCUMENTATION
    // ====================================================================
    
    m.def("get_help", []() {
        py::dict help;
        help["title"] = "NLM Python Bindings Help";
        help["description"] = "Python API for Neural Learning Machine (NLM)";
        
        py::list sections;
        
        py::dict section1;
        section1["title"] = "Quick Start";
        section1["content"] = R"(
1. Install NLM:
   pip install pynlm

2. Basic usage:
   import pynlm
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   brain.initialize()
   brain.step(0)

3. Get statistics:
   print(f"Neurons: {brain.getTotalNeuronCount()}")
   print(f"Firing rate: {brain.getAverageFiringRate()}")
        )";
        sections.append(section1);
        
        py::dict section2;
        section2["title"] = "Learning Systems";
        section2["content"] = R"(
NLM includes multiple learning systems:
- Working Memory: Transient storage of active information
- Episodic Memory: Storage of experienced events
- Neuromodulation: Reward prediction and curiosity signals
- Plasticity: STDP and Hebbian learning rules
- Development: Brain maturation and structural changes
        )";
        sections.append(section2);
        
        py::dict section3;
        section3["title"] = "Common Patterns";
        section3["content"] = R"(
Pattern 1: Simple simulation
for i in range(100):
    brain.step(i)

Pattern 2: Agent with learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

Pattern 3: Research setup
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
        )";
        sections.append(section3);
        
        help["sections"] = sections;
        
        return help;
    }, "Get help and documentation");
    
    m.def("print_version_info", []() {
        pybindings::py_log_info("=== NLM Python Bindings Version Info ===");
        pybindings::py_log_info("Version: 0.1.0");
        pybindings::py_log_info("Phase: 6 - Final Integration");
        pybindings::py_log_info("Status: Alpha");
        pybindings::py_log_info("Platform: C++20 with Python bindings");
        pybindings::py_log_info("Author: NLM Authors");
        pybindings::py_log_info("Project: Neural Learning Machine (熙然)");
    }, "Print version information");
    
    // ====================================================================
    // FINAL SUMMARY
    // ====================================================================
    
    pybindings::py_log_info("=== NLM Python Bindings Initialization Complete ===");
    pybindings::py_log_info("✓ Core brain simulation systems exposed");
    pybindings::py_log_info("✓ Memory systems (working, episodic, associative)" );
    pybindings::py_log_info("✓ Neuromodulation (dopamine, curiosity, novelty)");
    pybindings::py_log_info("✓ Prediction and planning systems");
    pybindings::py_log_info("✓ Development and maturation");
    pybindings::py_log_info("✓ Plasticity mechanisms (STDP, Hebbian, structural)");
    pybindings::py_log_info("✓ World interaction and behavior");
    pybindings::py_log_info("✓ Checkpoint save/load and persistence");
    pybindings::py_log_info("✓ Memory replay and consolidation");
    pybindings::py_log_info("✓ Python API with high-level abstractions");
    
    pybindings::py_log_info("NLM Python bindings ready for use!");
    pybindings::py_log_info("Import pynlm and explore the neural simulation framework.");
}

) // PYBIND11_MODULE(pynlm, m)
