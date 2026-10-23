// Advanced configuration bindings implementation
#include "AdvancedConfig.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void bindAdvancedConfig(py::module_ &m) {
    // Network Architecture
    py::class_<AdvancedConfig::NetworkArchitecture>(m, "AdvancedNetworkArchitecture")
        .def(py::init<>())
        .def_readwrite("hidden_layers", &AdvancedConfig::NetworkArchitecture::hidden_layers)
        .def_readwrite("neurons_per_layer", &AdvancedConfig::NetworkArchitecture::neurons_per_layer)
        .def_readwrite("connection_probability", &AdvancedConfig::NetworkArchitecture::connection_probability)
        .def_readwrite("architecture_type", &AdvancedConfig::NetworkArchitecture::architecture_type)
        .def_readwrite("use_skip_connections", &AdvancedConfig::NetworkArchitecture::use_skip_connections)
        .def_readwrite("dropout_rate", &AdvancedConfig::NetworkArchitecture::dropout_rate);
    
    // Advanced Plasticity
    py::class_<AdvancedConfig::AdvancedPlasticity>(m, "AdvancedPlasticity")
        .def(py::init<>())
        .def_readwrite("enable_stdp", &AdvancedConfig::AdvancedPlasticity::enable_stdp)
        .def_readwrite("enable_hebbian", &AdvancedConfig::AdvancedPlasticity::enable_hebbian)
        .def_readwrite("enable_structural", &AdvancedConfig::AdvancedPlasticity::enable_structural)
        .def_readwrite("enable_reward_modulated", &AdvancedConfig::AdvancedPlasticity::enable_reward_modulated)
        .def_readwrite("stdp_learning_rate", &AdvancedConfig::AdvancedPlasticity::stdp_learning_rate)
        .def_readwrite("hebbian_learning_rate", &AdvancedConfig::AdvancedPlasticity::hebbian_learning_rate)
        .def_readwrite("structural_growth_rate", &AdvancedConfig::AdvancedPlasticity::structural_growth_rate)
        .def_readwrite("reward_modulation_strength", &AdvancedConfig::AdvancedPlasticity::reward_modulation_strength);
    
    // Advanced Neuromodulation
    py::class_<AdvancedConfig::AdvancedNeuromodulation>(m, "AdvancedNeuromodulation")
        .def(py::init<>())
        .def_readwrite("enable_dopamine", &AdvancedConfig::AdvancedNeuromodulation::enable_dopamine)
        .def_readwrite("enable_curiosity", &AdvancedConfig::AdvancedNeuromodulation::enable_curiosity)
        .def_readwrite("enable_novelty", &AdvancedConfig::AdvancedNeuromodulation::enable_novelty)
        .def_readwrite("enable_prediction_error", &AdvancedConfig::AdvancedNeuromodulation::enable_prediction_error)
        .def_readwrite("dopamine_scale", &AdvancedConfig::AdvancedNeuromodulation::dopamine_scale)
        .def_readwrite("curiosity_scale", &AdvancedConfig::AdvancedNeuromodulation::curiosity_scale)
        .def_readwrite("novelty_scale", &AdvancedConfig::AdvancedNeuromodulation::novelty_scale)
        .def_readwrite("prediction_error_scale", &AdvancedConfig::AdvancedNeuromodulation::prediction_error_scale);
    
    // Memory Configuration
    py::class_<AdvancedConfig::MemoryConfig>(m, "AdvancedMemoryConfig")
        .def(py::init<>())
        .def_readwrite("enable_working_memory", &AdvancedConfig::MemoryConfig::enable_working_memory)
        .def_readwrite("enable_episodic_memory", &AdvancedConfig::MemoryConfig::enable_episodic_memory)
        .def_readwrite("enable_semantic_memory", &AdvancedConfig::MemoryConfig::enable_semantic_memory)
        .def_readwrite("enable_procedural_memory", &AdvancedConfig::MemoryConfig::enable_procedural_memory)
        .def_readwrite("enable_associative_memory", &AdvancedConfig::MemoryConfig::enable_associative_memory)
        .def_readwrite("working_memory_capacity", &AdvancedConfig::MemoryConfig::working_memory_capacity)
        .def_readwrite("episodic_memory_capacity", &AdvancedConfig::MemoryConfig::episodic_memory_capacity)
        .def_readwrite("semantic_memory_capacity", &AdvancedConfig::MemoryConfig::semantic_memory_capacity)
        .def_readwrite("procedural_memory_capacity", &AdvancedConfig::MemoryConfig::procedural_memory_capacity)
        .def_readwrite("use_consolidation", &AdvancedConfig::MemoryConfig::use_consolidation)
        .def_readwrite("consolidation_rate", &AdvancedConfig::MemoryConfig::consolidation_rate)
        .def_readwrite("use_forgetting", &AdvancedConfig::MemoryConfig::use_forgetting)
        .def_readwrite("forgetting_rate", &AdvancedConfig::MemoryConfig::forgetting_rate);
    
    // Prediction Configuration
    py::class_<AdvancedConfig::PredictionConfig>(m, "AdvancedPredictionConfig")
        .def(py::init<>())
        .def_readwrite("enable_forward_model", &AdvancedConfig::PredictionConfig::enable_forward_model)
        .def_readwrite("enable_inverse_model", &AdvancedConfig::PredictionConfig::enable_inverse_model)
        .def_readwrite("enable_belief_update", &AdvancedConfig::PredictionConfig::enable_belief_update)
        .def_readwrite("prediction_horizon", &AdvancedConfig::PredictionConfig::prediction_horizon)
        .def_readwrite("prediction_precision", &AdvancedConfig::PredictionConfig::prediction_precision)
        .def_readwrite("prediction_update_rate", &AdvancedConfig::PredictionConfig::prediction_update_rate)
        .def_readwrite("use_bayesian_prediction", &AdvancedConfig::PredictionConfig::use_bayesian_prediction)
        .def_readwrite("use_particle_filter", &AdvancedConfig::PredictionConfig::use_particle_filter)
        .def_readwrite("particle_count", &AdvancedConfig::PredictionConfig::particle_count)
        .def_readwrite("use_prefrontal_prediction", &AdvancedConfig::PredictionConfig::use_prefrontal_prediction);
    
    // Cognition Configuration
    py::class_<AdvancedConfig::CognitionConfig>(m, "AdvancedCognitionConfig")
        .def(py::init<>())
        .def_readwrite("enable_neural_planner", &AdvancedConfig::CognitionConfig::enable_neural_planner)
        .def_readwrite("enable_concept_formation", &AdvancedConfig::CognitionConfig::enable_concept_formation)
        .def_readwrite("enable_attentional_selection", &AdvancedConfig::CognitionConfig::enable_attentional_selection)
        .def_readwrite("planning_horizon", &AdvancedConfig::CognitionConfig::planning_horizon)
        .def_readwrite("concept_threshold", &AdvancedConfig::CognitionConfig::concept_threshold)
        .def_readwrite("attention_window", &AdvancedConfig::CognitionConfig::attention_window)
        .def_readwrite("use_reinforcement_learning", &AdvancedConfig::CognitionConfig::use_reinforcement_learning)
        .def_readwrite("use_hierarchical_cognition", &AdvancedConfig::CognitionConfig::use_hierarchical_cognition)
        .def_readwrite("learning_rate", &AdvancedConfig::CognitionConfig::learning_rate)
        .def_readwrite("exploration_rate", &AdvancedConfig::CognitionConfig::exploration_rate);
    
    // Development Configuration
    py::class_<AdvancedConfig::DevelopmentConfig>(m, "AdvancedDevelopmentConfig")
        .def(py::init<>())
        .def_readwrite("enable_developmental_stages", &AdvancedConfig::DevelopmentConfig::enable_developmental_stages)
        .def_readwrite("enable_structural_plasticity", &AdvancedConfig::DevelopmentConfig::enable_structural_plasticity)
        .def_readwrite("enable_activity_dependent_maturation", &AdvancedConfig::DevelopmentConfig::enable_activity_dependent_maturation)
        .def_readwrite("critical_period_start", &AdvancedConfig::DevelopmentConfig::critical_period_start)
        .def_readwrite("critical_period_end", &AdvancedConfig::DevelopmentConfig::critical_period_end)
        .def_readwrite("maturation_rate", &AdvancedConfig::DevelopmentConfig::maturation_rate)
        .def_readwrite("structural_growth_rate", &AdvancedConfig::DevelopmentConfig::structural_growth_rate)
        .def_readwrite("use_aging_model", &AdvancedConfig::DevelopmentConfig::use_aging_model)
        .def_readwrite("aging_rate", &AdvancedConfig::DevelopmentConfig::aging_rate)
        .def_readwrite("use_environmental_influences", &AdvancedConfig::DevelopmentConfig::use_environmental_influences);
    
    // World Configuration
    py::class_<AdvancedConfig::WorldConfig>(m, "AdvancedWorldConfig")
        .def(py::init<>())
        .def_readwrite("world_width", &AdvancedConfig::WorldConfig::world_width)
        .def_readwrite("world_height", &AdvancedConfig::WorldConfig::world_height)
        .def_readwrite("vision_width", &AdvancedConfig::WorldConfig::vision_width)
        .def_readwrite("vision_height", &AdvancedConfig::WorldConfig::vision_height)
        .def_readwrite("vision_quality", &AdvancedConfig::WorldConfig::vision_quality)
        .def_readwrite("enable_physics", &AdvancedConfig::WorldConfig::enable_physics)
        .def_readwrite("physics_gravity", &AdvancedConfig::WorldConfig::physics_gravity)
        .def_readwrite("physics_friction", &AdvancedConfig::WorldConfig::physics_friction)
        .def_readwrite("enable_resources", &AdvancedConfig::WorldConfig::enable_resources)
        .def_readwrite("enable_hazards", &AdvancedConfig::WorldConfig::enable_hazards)
        .def_readwrite("enable_walls", &AdvancedConfig::WorldConfig::enable_walls);
    
    // Simulation Configuration
    py::class_<AdvancedConfig::SimulationConfig>(m, "AdvancedSimulationConfig")
        .def(py::init<>())
        .def_readwrite("timestep", &AdvancedConfig::SimulationConfig::timestep)
        .def_readwrite("simulation_duration", &AdvancedConfig::SimulationConfig::simulation_duration)
        .def_readwrite("enable_checkpointing", &AdvancedConfig::SimulationConfig::enable_checkpointing)
        .def_readwrite("enable_replay", &AdvancedConfig::SimulationConfig::enable_replay)
        .def_readwrite("use_event_driven", &AdvancedConfig::SimulationConfig::use_event_driven)
        .def_readwrite("use_adaptive_timestep", &AdvancedConfig::SimulationConfig::use_adaptive_timestep)
        .def_readwrite("timestep_min", &AdvancedConfig::SimulationConfig::timestep_min)
        .def_readwrite("timestep_max", &AdvancedConfig::SimulationConfig::timestep_max)
        .def_readwrite("enable_profiling", &AdvancedConfig::SimulationConfig::enable_profiling)
        .def_readwrite("enable_logging", &AdvancedConfig::SimulationConfig::enable_logging);
    
    // Research Configuration
    py::class_<AdvancedConfig::ResearchConfig>(m, "AdvancedResearchConfig")
        .def(py::init<>())
        .def_readwrite("experiment_type", &AdvancedConfig::ResearchConfig::experiment_type)
        .def_readwrite("enable_data_logging", &AdvancedConfig::ResearchConfig::enable_data_logging)
        .def_readwrite("enable_visualization", &AdvancedConfig::ResearchConfig::enable_visualization)
        .def_readwrite("enable_statistical_analysis", &AdvancedConfig::ResearchConfig::enable_statistical_analysis)
        .def_readwrite("enable_reproducibility", &AdvancedConfig::ResearchConfig::enable_reproducibility)
        .def_readwrite("output_directory", &AdvancedConfig::ResearchConfig::output_directory)
        .def_readwrite("data_format", &AdvancedConfig::ResearchConfig::data_format)
        .def_readwrite("use_cloud_storage", &AdvancedConfig::ResearchConfig::use_cloud_storage)
        .def_readwrite("use_monitoring", &AdvancedConfig::ResearchConfig::use_monitoring);
    
    // Main AdvancedConfig class
    py::class_<AdvancedConfig>(m, "AdvancedConfig")
        .def(py::init<>())
        .def_readwrite("network", &AdvancedConfig::network)
        .def_readwrite("plasticity", &AdvancedConfig::plasticity)
        .def_readwrite("neuromodulation", &AdvancedConfig::neuromodulation)
        .def_readwrite("memory", &AdvancedConfig::memory)
        .def_readwrite("prediction", &AdvancedConfig::prediction)
        .def_readwrite("cognition", &AdvancedConfig::cognition)
        .def_readwrite("development", &AdvancedConfig::development)
        .def_readwrite("world", &AdvancedConfig::world)
        .def_readwrite("simulation", &AdvancedConfig::simulation)
        .def_readwrite("research", &AdvancedConfig::research)
        .def("applyToBrain", &AdvancedConfig::applyToBrain,
              "Apply this advanced configuration to a brain instance")
        .def("getSummary", &AdvancedConfig::getSummary,
              "Get a human-readable summary of the configuration");
    
    // Factory functions for preset configurations
    m.def("createExpertConfiguration", &AdvancedConfig::createExpertConfiguration,
          "Create an expert-level configuration optimized for maximum performance");
    
    m.def("createResearchConfiguration", &AdvancedConfig::createResearchConfiguration,
          "Create a research configuration optimized for scientific investigation");
    
    m.def("createEducationalConfiguration", &AdvancedConfig::createEducationalConfiguration,
          "Create an educational configuration optimized for learning and teaching");
    
    m.def("createRealisticConfiguration", &AdvancedConfig::createRealisticConfiguration,
          "Create a realistic configuration based on biological constraints");
    
    // Domain-specific configurations
    m.def("createNeuroscienceResearchConfiguration", 
          []() { return AdvancedConfig::domains::createNeuroscienceResearchConfig(); },
          "Create a configuration for neuroscience research");
    
    m.def("createAICodevelopmentConfiguration", 
          []() { return AdvancedConfig::domains::createAICodevelopmentConfig(); },
          "Create a configuration for AI development");
    
    m.def("createCognitiveModelingConfiguration", 
          []() { return AdvancedConfig::domains::createCognitiveModelingConfig(); },
          "Create a configuration for cognitive modeling");
    
    m.def("createDevelopmentalPsychologyConfiguration", 
          []() { return AdvancedConfig::domains::createDevelopmentalPsychologyConfig(); },
          "Create a configuration for developmental psychology research");
}

} // namespace nlm