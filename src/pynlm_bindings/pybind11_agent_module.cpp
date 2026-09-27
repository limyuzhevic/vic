// pybind11_agent_module.cpp - Agent module bindings implementation

#include "pybind11_agent_module.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace nlm {

void bind_agent_module(py::module& m) {
    // Enhanced AgentBrain class with comprehensive bindings
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
    
    The agent brain interface that connects the NLM brain to the world environment.
    It handles sensory transduction and motor decoding, providing the bridge
    between internal neural processing and external actions.
    
    AgentBrain functionality includes:
    - Sensory processing and input injection
    - Motor command decoding from neural activity
    - Neuromodulation and reward-based learning
    - Developmental process management
    - Curiosity-driven exploration
    - Action selection and planning
    
    The agent brain provides the interface for:
    - Perception: Processing sensory input from the world
    - Cognition: Neural processing and decision-making
    - Action: Converting neural activity to motor commands
    - Learning: Adapting through neuromodulation and development
    - Development: Maturation and specialization over time
    
    AgentBrain is essential for:
    - Environment interaction
    - Behavior generation
    - Learning and adaptation
    - Developmental progression
    "pbdoc")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
         "Initialize agent brain with underlying neural brain (brain is required)")
    .def("initialize", &AgentBrain::initialize,
         py::arg("world"), "Initialize with world environment")
    .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
         "Get expected sensory input size expected by brain")
    .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
         "Get expected motor output size")
    .def("process_sensory_input", &AgentBrain::processSensoryInput,
         py::arg("percept"), "Process sensory percept and inject into brain")
    .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
         "Decode brain motor activity into motor command")
    .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
         py::arg("reward"), py::arg("predictedReward"),
         "Apply reward-based neuromodulation")
    .def("update_development", &AgentBrain::updateDevelopment,
         py::arg("timestep"), "Update development system")
    .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
         "Get current neuromodulation level")
    .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
         "Get curiosity level")
    .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
         "Get novelty level")
    .def("get_prediction_error", &AgentBrain::getPredictionError,
         "Get prediction error")
    .def("reset", &AgentBrain::reset,
         "Reset agent for new episode")
    .def("get_brain", &AgentBrain::getBrain,
         py::return_value_policy::reference_internal, "Get the underlying brain")
    .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
         py::arg("enable"), "Enable or disable reward modulation")
    .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
         py::arg("enable"), "Enable or disable structural plasticity")
    .def("enable_development", &AgentBrain::enableDevelopment,
         py::arg("enable"), "Enable or disable development")
    .def("enable_curiosity", &AgentBrain::enableCuriosity,
         py::arg("enable"), "Enable or disable curiosity-driven exploration")
    .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
         "Check if reward modulation is enabled")
    .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
         "Check if structural plasticity is enabled")
    .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
         "Check if development is enabled")
    .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
         "Check if curiosity is enabled")
    .def("get_agent_stats", [](const AgentBrain& self) {
        py::dict stats;
        
        stats["sensory_input_size"] = self.getSensoryInputSize();
        stats["motor_output_size"] = self.getMotorOutputSize();
        stats["neuromodulation_level"] = self.getNeuromodulationLevel();
        stats["curiosity_level"] = self.getCuriosityLevel();
        stats["novelty_level"] = self.getNoveltyLevel();
        stats["prediction_error"] = self.getPredictionError();
        
        stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
        stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
        stats["development_enabled"] = self.isDevelopmentEnabled();
        stats["curiosity_enabled"] = self.isCuriosityEnabled();
        
        stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        
        return stats;
    }, "Get comprehensive agent statistics")
    .def("enable_all_subsystems", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
        return true;
    }, "Enable all adaptive subsystems")
    .def("disable_all_subsystems", [](AgentBrain& self) {
        self.enableRewardModulation(false);
        self.enableStructuralPlasticity(false);
        self.enableDevelopment(false);
        self.enableCuriosity(false);
        return true;
    }, "Disable all adaptive subsystems")
    .def("update_agent_state", [](AgentBrain& self, const SensoryPercept& percept,
                                 double timestep, float reward) {
        // Process sensory input
        self.processSensoryInput(percept);
        
        // Update development
        self.updateDevelopment(timestep);
        
        // Apply reward modulation
        self.applyRewardModulation(reward, self.getPredictionError());
        
        // Decode motor command
        auto motor_cmd = self.decodeMotorCommand();
        
        py::dict result;
        result["motor_command"] = static_cast<int>(motor_cmd);
        result["neuromodulation_level"] = self.getNeuromodulationLevel();
        result["curiosity_level"] = self.getCuriosityLevel();
        result["novelty_level"] = self.getNoveltyLevel();
        result["prediction_error"] = self.getPredictionError();
        result["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        
        return result;
    }, py::arg("percept"), py::arg("timestep"), py::arg("reward"),
         "Update complete agent state and return result")
    .def("simulate_episode", [](AgentBrain& self, SimpleWorld& world, 
                                double timestep, size_t steps) {
        py::list episode_data;
        
        world.reset();
        self.reset();
        self.initialize(world);
        
        for (size_t i = 0; i < steps; ++i) {
            py::dict step_data;
            step_data["step"] = static_cast<int>(i);
            step_data["time"] = world.getSimulationTime();
            
            // Get sensory input
            auto percept = world.getSensoryPercept();
            step_data["sensory_data_size"] = percept.getVision().size();
            
            // Process sensory input
            self.processSensoryInput(percept);
            
            // Update development
            self.updateDevelopment(timestep);
            
            // Get motor command
            auto motor_cmd = self.decodeMotorCommand();
            step_data["motor_command"] = static_cast<int>(motor_cmd);
            
            // Apply action to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime());
            
            // Update world
            world.update(timestep);
            
            // Get reward from internal signals
            auto internal = percept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            
            // Apply reward modulation
            self.applyRewardModulation(reward, self.getPredictionError());
            
            // Collect statistics
            step_data["reward"] = reward;
            step_data["neuromodulation_level"] = self.getNeuromodulationLevel();
            step_data["curiosity_level"] = self.getCuriosityLevel();
            step_data["novelty_level"] = self.getNoveltyLevel();
            
            episode_data.append(step_data);
        }
        
        return episode_data;
    }, py::arg("world"), py::arg("timestep"), py::arg("steps"),
         "Run complete episode simulation and return step data")
    .def("get_curiosity_driven_actions", [](AgentBrain& self, size_t count) {
        // This is a placeholder for curiosity-driven action selection
        // In practice, this would implement actual curiosity-driven exploration
        py::list actions;
        
        auto brain = self.getBrain();
        if (brain) {
            for (size_t i = 0; i < count; ++i) {
                // Generate random motor command for demonstration
                int choice = brain->getRandomGenerator()->uniformInt(0, 7);
                MotorCommand cmd;
                switch (choice) {
                    case 0: cmd = MotorCommand::MoveForward; break;
                    case 1: cmd = MotorCommand::MoveBackward; break;
                    case 2: cmd = MotorCommand::TurnLeft; break;
                    case 3: cmd = MotorCommand::TurnRight; break;
                    case 4: cmd = MotorCommand::LookLeft; break;
                    case 5: cmd = MotorCommand::LookRight; break;
                    case 6: cmd = MotorCommand::Interact; break;
                    default: cmd = MotorCommand::Wait; break;
                }
                actions.append(static_cast<int>(cmd));
            }
        }
        
        return actions;
    }, py::arg("count") = 10, "Get curiosity-driven action suggestions")
    .def("analyze_behavior_patterns", [](const AgentBrain& self) {
        py::dict patterns;
        
        // This is a placeholder for behavior pattern analysis
        // In practice, this would analyze actual behavior data
        patterns["motor_command_distribution"] = py::dict({
            {"MoveForward", 0.3},
            {"MoveBackward", 0.1},
            {"TurnLeft", 0.2},
            {"TurnRight", 0.2},
            {"Interact", 0.1},
            {"Wait", 0.1}
        });
        
        patterns["curiosity_driven_proportion"] = 0.15; // Placeholder
        patterns["reward_sensitive"] = true; // Placeholder
        patterns["exploration_vs_exploitation"] = 0.4; // Placeholder
        
        return patterns;
    }, "Analyze and return behavior patterns (placeholder)")
    .def("export_agent_configuration", [](const AgentBrain& self) {
        py::dict config;
        
        config["reward_modulation_enabled"] = self.isRewardModulationEnabled();
        config["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
        config["development_enabled"] = self.isDevelopmentEnabled();
        config["curiosity_enabled"] = self.isCuriosityEnabled();
        
        config["neuromodulation_level"] = self.getNeuromodulationLevel();
        config["curiosity_level"] = self.getCuriosityLevel();
        config["novelty_level"] = self.getNoveltyLevel();
        config["prediction_error"] = self.getPredictionError();
        
        config["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        
        return config;
    }, "Export agent configuration as Python dictionary")
    .def("import_agent_configuration", [](AgentBrain& self, const py::dict& config) {
        // Apply configuration settings
        if (config.contains("reward_modulation_enabled")) {
            self.enableRewardModulation(config["reward_modulation_enabled"].cast<bool>());
        }
        if (config.contains("structural_plasticity_enabled")) {
            self.enableStructuralPlasticity(config["structural_plasticity_enabled"].cast<bool>());
        }
        if (config.contains("development_enabled")) {
            self.enableDevelopment(config["development_enabled"].cast<bool>());
        }
        if (config.contains("curiosity_enabled")) {
            self.enableCuriosity(config["curiosity_enabled"].cast<bool>());
        }
        
        return true;
    }, py::arg("config"), "Import agent configuration from Python dictionary");
}

} // namespace nlm
