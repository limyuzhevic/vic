#!/usr/bin/env python3
"""
NLM Simulation Script Templates

This module provides functionality to generate simulation script templates
for NLM, supporting different types of experiments and simulations.
"""

import os
import sys
import json
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime
import random

@dataclass
class SimulationTemplate:
    """Template for NLM simulations"""
    name: str
    description: str
    template_file: str
    variables: Dict[str, Any]
    commands: List[str]

class NLMSimulationTemplateGenerator:
    """Generate simulation script templates for NLM"""
    
    def __init__(self, template_dir: str = "templates"):
        self.template_dir = template_dir
        self.template_definitions = self.load_template_definitions()
        self.ensure_template_directory()
    
    def ensure_template_directory(self):
        """Ensure template directory exists"""
        if not os.path.exists(self.template_dir):
            os.makedirs(self.template_dir)
    
    def load_template_definitions(self) -> Dict[str, Any]:
        """Load template definitions"""
        return {
            "basic_simulation": {
                "name": "Basic Neural Simulation",
                "description": "Simple neural network simulation with basic learning",
                "template_file": "basic_simulation.py",
                "variables": {
                    "duration": 10.0,
                    "timestep": 0.001,
                    "neuron_count": 100,
                    "plasticity_enabled": True,
                    "memory_enabled": False,
                    "neuromodulation_enabled": False,
                    "environment_type": "GridWorld",
                    "log_level": "INFO"
                },
                "commands": [
                    "python simulate_basic.py",
                    "nlm-validate --config config_basic.json",
                    "nlm-benchmark --quick"
                ]
            },
            "plasticity_study": {
                "name": "Plasticity Learning Study",
                "description": "Comprehensive study of plasticity mechanisms",
                "template_file": "plasticity_study.py",
                "variables": {
                    "duration": 50.0,
                    "timestep": 0.0005,
                    "neuron_count": 500,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "environment_type": "PlasticityWorld",
                    "log_level": "DEBUG",
                    "plasticity_rules": ["stdp", "hebbian", "structural"],
                    "stimulation_pattern": "periodic",
                    "recording_frequency": "high"
                },
                "commands": [
                    "python simulate_plasticity.py",
                    "nlm-analyze --type plasticity",
                    "nlm-plot --plasticity",
                    "nlm-benchmark --detailed --neurons 500"
                ]
            },
            "memory_experiment": {
                "name": "Memory Capacity Experiment",
                "description": "Experiment investigating memory systems",
                "template_file": "memory_experiment.py",
                "variables": {
                    "duration": 30.0,
                    "timestep": 0.001,
                    "neuron_count": 200,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "episodic_memory_enabled": True,
                    "working_memory_enabled": True,
                    "semantic_memory_enabled": True,
                    "environment_type": "MemoryWorld",
                    "log_level": "INFO",
                    "memory_load": "high",
                    "recall_test": True
                },
                "commands": [
                    "python simulate_memory.py",
                    "nlm-analyze --type memory",
                    "nlm-plot --memory",
                    "nlm-benchmark --detailed --neurons 200"
                ]
            },
            "neuromodulation_study": {
                "name": "Neuromodulation Investigation",
                "description": "Study of neuromodulation effects on learning",
                "template_file": "neuromodulation_study.py",
                "variables": {
                    "duration": 40.0,
                    "timestep": 0.0005,
                    "neuron_count": 300,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "dopamine_baseline": 0.5,
                    "curiosity_baseline": 0.3,
                    "novelty_baseline": 0.2,
                    "environment_type": "NeuromodWorld",
                    "log_level": "DEBUG",
                    "modulation_effects": ["dopamine", "curiosity", "novelty"],
                    "reward_structure": "sparse"
                },
                "commands": [
                    "python simulate_neuromod.py",
                    "nlm-analyze --type neuromodulation",
                    "nlm-plot --neuromodulation",
                    "nlm-benchmark --detailed --neurons 300"
                ]
            },
            "developmental_simulation": {
                "name": "Developmental Simulation",
                "description": "Simulation of neural development and maturation",
                "template_file": "developmental_simulation.py",
                "variables": {
                    "duration": 100.0,
                    "timestep": 0.001,
                    "neuron_count": 100,
                    "plasticity_enabled": True,
                    "memory_enabled": False,
                    "neuromodulation_enabled": False,
                    "development_enabled": True,
                    "initial_neurons": 50,
                    "synaptogenesis_rate": 0.001,
                    "pruning_rate": 0.0001,
                    "maturation_rate": 0.01,
                    "environment_type": "DevelopmentWorld",
                    "log_level": "INFO",
                    "stage_by_stage": True,
                    "age_effects": True
                },
                "commands": [
                    "python simulate_development.py",
                    "nlm-analyze --type development",
                    "nlm-plot --development",
                    "nlm-benchmark --detailed --neurons 100"
                ]
            },
            "benchmark_suite": {
                "name": "Benchmark Suite",
                "description": "Comprehensive benchmarking of NLM capabilities",
                "template_file": "benchmark_suite.py",
                "variables": {
                    "neuron_counts": [100, 500, 1000, 5000],
                    "simulation_steps": [100, 500, 1000, 5000],
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "environment_type": "BenchmarkWorld",
                    "log_level": "WARNING",
                    "output_format": "json",
                    "parallel_processing": True,
                    "performance_analysis": True
                },
                "commands": [
                    "python run_benchmarks.py",
                    "nlm-benchmark --detailed --neurons 1000 --steps 1000",
                    "nlm-plot --benchmark",
                    "nlm-analyze --type performance"
                ]
            }
        }
    
    def generate_simulation_script(self, template_name: str, output_dir: str = None,
                                   variables: Optional[Dict[str, Any]] = None,
                                   experiment_name: str = None) -> bool:
        """
        Generate a simulation script from a template
        
        Args:
            template_name: Name of template to use
            output_dir: Output directory (uses template_dir if None)
            variables: Override template variables
            experiment_name: Name for the experiment
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if template_name not in self.template_definitions:
                print(f"Error: Unknown template '{template_name}'")
                return False
            
            template = self.template_definitions[template_name]
            target_dir = output_dir or self.template_dir
            
            # Create output directory
            if not os.path.exists(target_dir):
                os.makedirs(target_dir)
            
            # Prepare variables
            template_vars = template["variables"].copy()
            if variables:
                template_vars.update(variables)
            
            # Set experiment name if provided
            if experiment_name:
                template_vars["experiment_name"] = experiment_name
            else:
                template_vars["experiment_name"] = f"{template_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
            
            # Generate script from template
            script_content = self.generate_script_from_template(template, template_vars)
            
            # Write script file
            script_filename = template["template_file"]
            script_path = os.path.join(target_dir, script_filename)
            
            with open(script_path, 'w') as f:
                f.write(script_content)
            
            # Make script executable
            os.chmod(script_path, 0o755)
            
            # Generate configuration file
            config_filename = f"config_{template_name}.json"
            config_path = os.path.join(target_dir, config_filename)
            self.generate_config_file(template_vars, config_path)
            
            # Generate README
            readme_filename = "README.md"
            readme_path = os.path.join(target_dir, readme_filename)
            self.generate_readme(template, template_vars, readme_path)
            
            print(f"Simulation script generated: {script_path}")
            print(f"Configuration file: {config_path}")
            print(f"README: {readme_path}")
            
            return True
            
        except Exception as e:
            print(f"Error generating simulation script: {e}")
            return False
    
    def generate_script_from_template(self, template: Dict[str, Any], variables: Dict[str, Any]) -> str:
        """Generate script content from template and variables"""
        script_template = self.get_script_template(template["name"])
        
        # Replace placeholders in script template
        script_content = script_template
        for key, value in variables.items():
            placeholder = f"{{{{{key}}}}}"
            script_content = script_content.replace(placeholder, str(value))
        
        return script_content
    
    def get_script_template(self, template_name: str) -> str:
        """Get script template content based on template name"""
        if template_name == "basic_simulation":
            return self.get_basic_simulation_template()
        elif template_name == "plasticity_study":
            return self.get_plasticity_study_template()
        elif template_name == "memory_experiment":
            return self.get_memory_experiment_template()
        elif template_name == "neuromodulation_study":
            return self.get_neuromodulation_study_template()
        elif template_name == "developmental_simulation":
            return self.get_developmental_simulation_template()
        elif template_name == "benchmark_suite":
            return self.get_benchmark_suite_template()
        else:
            return self.get_generic_template()
    
    def get_basic_simulation_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
Basic NLM Simulation Script
Generated by NLM Simulation Template Generator
Template: Basic Neural Simulation
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision
from nlm.memory import WorkingMemory, EpisodicMemory
from nlm.neuromodulation import Neuromodulator

def run_simulation():
    """Run the basic NLM simulation"""
    print("Starting NLM Basic Simulation")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print(f"Duration: {duration}s")
    print(f"Time Step: {timestep}s")
    print(f"Neuron Count: {neuron_count}")
    print(f"Environment: {environment_type}")
    print("=" * 50)
    
    # Initialize configuration
    config = Config()
    config.set("random_seed", 42)
    config.set("simulation_timestep", timestep)
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    config.set("connection_probability", 0.15)
    config.set("plasticity_learning_rate", 0.01)
    config.set("log_level", log_level)
    
    # Create brain
    brain = Brain(config)
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Create environment
    world = SimpleWorld(environment_width, environment_height)
    print(f"Environment created: {environment_width}x{environment_height}")
    
    # Create sensory system
    vision = Vision()
    print("Sensory system initialized")
    
    # Create memory systems if enabled
    if {memory_enabled}:
        working_memory = WorkingMemory()
        episodic_memory = EpisodicMemory()
        print("Memory systems initialized")
    
    # Create neuromodulator if enabled
    if {neuromodulation_enabled}:
        neuromodulator = Neuromodulator()
        neuromodulator.set_modulator_level("dopamine", 0.1)
        neuromodulator.set_modulator_level("curiosity", 0.05)
        neuromodulator.set_modulator_level("novelty", 0.02)
        print("Neuromodulator initialized")
    
    # Run simulation
    print("\nStarting simulation...")
    total_steps = int(duration / timestep)
    
    for step in range(total_steps):
        current_time = step * timestep
        
        # Get observations from environment
        observations = vision.get_observations(world)
        
        # Process observations and update brain
        brain.step(step, current_time)
        
        # Take actions
        actions = brain.get_actions()
        world.apply_actions(actions)
        
        # Store experience in memory if enabled
        if {memory_enabled}:
            working_memory.store(observations, actions)
            
            # Occasionally create episodic memory
            if step % 100 == 0 and step > 0:
                memory_content = {
                    "observations": observations,
                    "actions": actions,
                    "outcome": "success"
                }
                episodic_memory.create_experience(memory_content)
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}/{total_steps} ({current_time:.2f}s) - "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Firing Neurons: {brain.getFiringNeuronCount()}")
            
            # Print memory statistics
            if {memory_enabled}:
                print(f"  Working Memory: {len(working_memory.get_content())} items")
                if episodic_memory and episodic_memory.get_experience_count() > 0:
                    print(f"  Episodic Memory: {episodic_memory.get_experience_count()} episodes")
    
    print("\nSimulation completed!")
    brain.log_status()
    
    # Run analysis
    print("\nRunning post-simulation analysis...")
    
    if {memory_enabled}:
        print(f"Memory usage analysis:")
        if working_memory:
            print(f"  Working Memory capacity: {len(working_memory.get_content())} items")
        if episodic_memory:
            print(f"  Episodic Memory episodes: {episodic_memory.get_experience_count()}")
    
    # Save final results
    if {output_format} == "json":
        save_results_as_json(brain, f"results_{experiment_name}.json")
    
    print("\nSimulation results saved!")
    print("Basic simulation completed successfully!")

def save_results_as_json(brain, filename):
    """Save simulation results as JSON"""
    results = {
        "metadata": {
            "experiment": "{experiment_name}",
            "generated_at": "{generation_time}",
            "brain_type": "NLM"
        },
        "statistics": {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "simulation_duration": duration
        }
    }
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Results saved to: {filename}")

if __name__ == "__main__":
    # Set variables for template
    duration = {duration}
    timestep = {timestep}
    neuron_count = {neuron_count}
    environment_width = {environment_width}
    environment_height = {environment_height}
    log_level = "{log_level}"
    memory_enabled = {memory_enabled}
    neuormodulation_enabled = {neuromodulation_enabled}
    output_format = "{output_format}"
    experiment_name = "{experiment_name}"
    
    # Set generation time
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    run_simulation()
'''
    
    def get_plasticity_study_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
Plasticity Learning Study Script
Generated by NLM Simulation Template Generator
Template: Plasticity Learning Study
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision
from nlm.memory import WorkingMemory, EpisodicMemory
from nlm.neuromodulation import Neuromodulator
import random

def run_plasticity_study():
    """Run plasticity learning study"""
    print("Starting NLM Plasticity Learning Study")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print(f"Duration: {duration}s")
    print(f"Time Step: {timestep}s")
    print(f"Neuron Count: {neuron_count}")
    print(f"Plasticity Rules: {plasticity_rules}")
    print(f"Stimulation Pattern: {stimulation_pattern}")
    print("=" * 50)
    
    # Initialize configuration
    config = Config()
    config.set("random_seed", 12345)
    config.set("simulation_timestep", timestep)
    config.set("neuron_count", neuron_count)
    config.set("region_count", 2)
    config.set("connection_probability", 0.05)
    config.set("plasticity_learning_rate", 0.005)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.018)
    config.set("stdp_time_constant", 15.0)
    config.set("synaptogenesis_rate", 0.0005)
    config.set("pruning_rate", 0.00005)
    config.set("log_level", log_level)
    
    # Create brain
    brain = Brain(config)
    brain.initialize()
    
    # Enable all plasticity rules
    if {plasticity_enabled}:
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);  // STDP, Hebbian, Structural
            }
        }
    
    print(f"Brain initialized with {brain->getTotalNeuronCount()} neurons")
    print(f"Plasticity enabled: {plasticity_enabled}")
    
    # Run learning trials
    print("\nRunning learning trials...")
    total_trials = int(duration / timestep)
    
    for trial in range(total_trials):
        current_time = trial * timestep
        
        # Apply learning stimulus based on pattern
        if stimulation_pattern == "periodic":
            if trial % 50 == 0:
                // Sensory input
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                // Motor output
                brain->injectCurrentToNeurons(NeuronType::Motor, 15.0f);
        
        elif stimulation_pattern == "burst":
            if trial % 10 == 0 and trial < 100:
                // Burst input
                for (int i = 0; i < 5; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
                }
        
        elif stimulation_pattern == "random":
            if (rand() % 10) == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, (rand() % 30) + 20);
            }
        
        // Run simulation step
        brain->step(trial, current_time);
        
        // Record data periodically
        if trial % recording_frequency == 0) {
            print(f"Trial {trial}/{total_trials} ({current_time:.2f}s) - "
                  f"Spikes: {brain->getTotalSpikeCount()}, "
                  f"Learning: {calculate_learning_score(brain)}")
        }
        
        // Small delay for observation
        if trial < total_trials - 1) {
            // Sleep for visualization
        }
    
    print("\nLearning study completed!")
    brain->logStatus()
    
    // Analyze plasticity effects
    analyze_plasticity_effects(brain, plasticity_rules);
    
    // Generate plasticity report
    generate_plasticity_report(brain, experiment_name);
    
    print("\nPlasticity learning study completed successfully!")
}

def calculate_learning_score(std::shared_ptr<Brain> brain) {
    // Calculate learning score based on various metrics
    size_t spikes = brain->getTotalSpikeCount();
    
    // Get spike system for analysis
    auto* spikeSystem = brain->getSpikeSystem();
    size_t mostActiveNeurons = 0;
    
    if (spikeSystem) {
        auto mostActive = spikeSystem->getMostActiveNeurons(10);
        mostActiveNeurons = mostActive.size();
    }
    
    // Simple learning score formula
    size_t learningScore = 0;
    
    if (spikes > 100) learningScore += 30;
    if (spikes > 500) learningScore += 40;
    if (mostActiveNeurons > 0) learningScore += 30;
    
    return std::min(learningScore, 100);
}

def analyze_plasticity_effects(std::shared_ptr<Brain> brain, std::vector<std::string> rules) {
    std::cout << "\nPlasticity Analysis:" << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) return;
    
    // Analyze different plasticity rules
    for (const auto& rule : rules) {
        double ruleEffect = 0.0;
        
        if (rule == "stdp") {
            ruleEffect = analyze_stdp_effects(brain);
        } else if (rule == "hebbian") {
            ruleEffect = analyze_hebbian_effects(brain);
        } else if (rule == "structural") {
            ruleEffect = analyze_structural_effects(brain);
        }
        
        std::cout << "  " << rule << " effect: " << ruleEffect << std::endl;
    }
}

def analyze_stdp_effects(std::shared_ptr<Brain> brain) {
    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) return 0.0;
    
    auto spikeHistory = spikeSystem->getSpikeHistory(100);
    if (spikeHistory.empty()) return 0.0;
    
    // Calculate STDP potentiation/depression
    double totalDelta = 0.0;
    for (size_t i = 1; i < spikeHistory.size(); ++i) {
        double delta = spikeHistory[i] - spikeHistory[i - 1];
        totalDelta += delta;
    }
    
    return std::min(std::max(totalDelta, 0.0), 100.0);
}

def analyze_hebbian_effects(std::shared_ptr<Brain> brain) {
    // Simplified Hebbian analysis
    size_t spikeCount = brain->getTotalSpikeCount();
    return std::min(static_cast<double>(spikeCount) / 10.0, 100.0);
}

def analyze_structural_effects(std::shared_ptr<Brain> brain) {
    // Simplified structural plasticity analysis
    return 50.0;  // Placeholder
}

def generate_plasticity_report(std::shared_ptr<Brain> brain, const std::string& experimentName) {
    std::ofstream file("plasticity_report.json");
    if (file.is_open()) {
        file << "{" << std::endl;
        file << "  \"experiment\": \"" << experimentName << "\"," << std::endl;
        file << "  \"timestamp\": \"" << time(0) << "\"," << std::endl;
        file << "  \"plasticity_analysis\": {" << std::endl;
        file << "    \"stdp_effect\": " << analyze_stdp_effects(brain) << "," << std::endl;
        file << "    \"hebbian_effect\": " << analyze_hebbian_effects(brain) << "," << std::endl;
        file << "    \"structural_effect\": " << analyze_structural_effects(brain) << std::endl;
        file << "  }" << std::endl;
        file << "}" << std::endl;
        
        file.close();
        std::cout << "Plasticity report generated: plasticity_report.json" << std::endl;
    }
}

if __name__ == "__main__":
    // Set variables for template
    duration = {duration}
    timestep = {timestep}
    neuron_count = {neuron_count}
    environment_width = {environment_width}
    environment_height = {environment_height}
    log_level = "{log_level}"
    memory_enabled = {memory_enabled}
    neuromodulation_enabled = {neuromodulation_enabled}
    plasticity_enabled = {plasticity_enabled}
    stimulation_pattern = "{stimulation_pattern}"
    recording_frequency = {recording_frequency}
    plasticity_rules = {plasticity_rules}
    
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    experiment_name = "{experiment_name}"
    
    run_plasticity_study()
'''
    
    def get_memory_experiment_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
Memory Capacity Experiment Script
Generated by NLM Simulation Template Generator
Template: Memory Capacity Experiment
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision
from nlm.memory import WorkingMemory, EpisodicMemory, SemanticMemory
from nlm.neuromodulation import Neuromodulator
import json

def run_memory_experiment():
    """Run memory capacity experiment"""
    print("Starting NLM Memory Capacity Experiment")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print(f"Duration: {duration}s")
    print(f"Time Step: {timestep}s")
    print(f"Neuron Count: {neuron_count}")
    print(f"Memory Load: {memory_load}")
    print(f"Recall Test: {recall_test}")
    print("=" * 50)
    
    // Initialize configuration
    config = Config()
    config.set("random_seed", 999)
    config.set("simulation_timestep", timestep)
    config.set("neuron_count", neuron_count)
    config.set("region_count", 3)
    config.set("connection_probability", 0.02)
    config.set("plasticity_learning_rate", 0.001)
    config.set("log_level", log_level)
    
    // Create brain
    brain = Brain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain->getTotalNeuronCount()} neurons")
    
    // Setup memory systems
    working_memory = WorkingMemory()
    episodic_memory = EpisodicMemory()
    semantic_memory = SemanticMemory()
    
    print("Memory systems initialized:")
    print(f"  Working Memory capacity: {working_memory->get_capacity()} items")
    print(f"  Episodic Memory buffer: {episodic_memory->get_buffer_size()} episodes")
    print(f"  Semantic Memory index: {semantic_memory->get_index_size()} entries")
    
    // Run memory capacity test
    print("\nRunning memory capacity test...")
    run_memory_capacity_test(brain, working_memory, episodic_memory, semantic_memory);
    
    // If recall test is enabled
    if {recall_test}) {
        print("\nRunning recall test...")
        run_recall_test(brain, working_memory, episodic_memory);
    }
    
    // Analyze memory performance
    print("\nMemory performance analysis...")
    analyze_memory_performance(brain, working_memory, episodic_memory, semantic_memory);
    
    // Save results
    save_memory_results(brain, working_memory, episodic_memory, semantic_memory, experiment_name);
    
    print("\nMemory capacity experiment completed successfully!")
}

def run_memory_capacity_test(std::shared_ptr<Brain> brain, std::shared_ptr<WorkingMemory> working_memory,
                             std::shared_ptr<EpisodicMemory> episodic_memory,
                             std::shared_ptr<SemanticMemory> semantic_memory) {
    size_t max_steps = int(duration / timestep);
    
    for (size_t step = 0; step < max_steps; ++step) {
        current_time = step * timestep;
        
        // Generate random sensory input
        observations = generate_random_observations(5);
        
        // Process observations and update brain
        brain->step(step, current_time);
        
        // Store in working memory
        working_memory->store(observations, brain->get_actions());
        
        // Periodically create episodic memory
        if (step % 50 == 0 && step > 0) {
            memory_content = {
                "observations": observations,
                "brain_state": "snapshot",
                "timestamp": current_time
            };
            episodic_memory->create_experience(memory_content);
        }
        
        // Occasionally encode semantic memory
        if (step % 100 == 0 && step > 0) {
            // Extract patterns from working memory
            auto patterns = working_memory->extract_patterns();
            
            // Encode into semantic memory
            for (const auto& pattern : patterns) {
                semantic_memory->encode(pattern, current_time);
            }
        }
        
        // Log memory statistics periodically
        if (step % 100 == 0) {
            size_t working_memory_usage = working_memory->get_content_size();
            size_t episodic_memory_usage = episodic_memory->get_experience_count();
            size_t semantic_memory_usage = semantic_memory->get_index_size();
            
            std::cout << "Step " << step << ": "
                      << "Working: " << working_memory_usage << ", "
                      << "Episodic: " << episodic_memory_usage << ", "
                      << "Semantic: " << semantic_memory_usage << std::endl;
        }
        
        // Check memory capacity limits
        if (working_memory_usage > working_memory->get_capacity() * 0.9) {
            std::cout << "Warning: Working memory approaching capacity" << std::endl;
        }
    }
}

def run_recall_test(std::shared_ptr<Brain> brain, std::shared_ptr<WorkingMemory> working_memory,
                    std::shared_ptr<EpisodicMemory> episodic_memory) {
    size_t test_steps = 200;
    
    std::cout << "Running recall test..." << std::endl;
    
    // Store some experiences for recall
    for (size_t i = 0; i < 10; ++i) {
        memory_content = {
            "test_id": i,
            "observations": generate_random_observations(3),
            "actions": {"move", "explore", "interact"}[i % 3],
            "outcome": "success"
        };
        episodic_memory->create_experience(memory_content);
    }
    
    // Try to recall experiences
    for (size_t i = 0; i < 10; ++i) {
        auto recalled = episodic_memory->recall(i);
        if (recalled.has_value()) {
            std::cout << "Recall test " << i << ": SUCCESS - recalled experience" << std::endl;
        } else {
            std::cout << "Recall test " << i << ": FAILED - could not recall experience" << std::endl;
        }
    }
}

def analyze_memory_performance(std::shared_ptr<Brain> brain, std::shared_ptr<WorkingMemory> working_memory,
                                std::shared_ptr<EpisodicMemory> episodic_memory,
                                std::shared_ptr<SemanticMemory> semantic_memory) {
    std::cout << "\nMemory Performance Analysis:" << std::endl;
    
    size_t working_memory_capacity = working_memory->get_capacity();
    size_t working_memory_usage = working_memory->get_content_size();
    size_t episodic_memory_capacity = episodic_memory->get_buffer_size();
    size_t episodic_memory_usage = episodic_memory->get_experience_count();
    size_t semantic_memory_capacity = semantic_memory->get_index_size();
    size_t semantic_memory_usage = semantic_memory->get_index_size();
    
    double working_memory_efficiency = (working_memory_usage / (double)working_memory_capacity) * 100.0;
    double episodic_memory_efficiency = (episodic_memory_usage / (double)episodic_memory_capacity) * 100.0;
    double semantic_memory_efficiency = (semantic_memory_usage / (double)semantic_memory_capacity) * 100.0;
    
    std::cout << "  Working Memory:" << std::endl;
    std::cout << "    Capacity: " << working_memory_capacity << std::endl;
    std::cout << "    Usage: " << working_memory_usage << std::endl;
    std::cout << "    Efficiency: " << working_memory_efficiency << "%" << std::endl;
    
    std::cout << "  Episodic Memory:" << std::endl;
    std::cout << "    Buffer: " << episodic_memory_capacity << std::endl;
    std::cout << "    Episodes: " << episodic_memory_usage << std::endl;
    std::cout << "    Efficiency: " << episodic_memory_efficiency << "%" << std::endl;
    
    std::cout << "  Semantic Memory:" << std::endl;
    std::cout << "    Index: " << semantic_memory_capacity << std::endl;
    std::cout << "    Entries: " << semantic_memory_usage << std::endl;
    std::cout << "    Efficiency: " << semantic_memory_efficiency << "%" << std::endl;
}

def save_memory_results(std::shared_ptr<Brain> brain, std::shared_ptr<WorkingMemory> working_memory,
                        std::shared_ptr<EpisodicMemory> episodic_memory,
                        std::shared_ptr<SemanticMemory> semantic_memory, const std::string& experimentName) {
    std::ofstream file("memory_results.json");
    if (file.is_open()) {
        file << "{" << std::endl;
        file << "  \"experiment\": \"" << experimentName << "\"," << std::endl;
        file << "  \"timestamp\": \"" << time(0) << "\"," << std::endl;
        file << "  \"memory_performance\": {" << std::endl;
        file << "    \"working_memory\": {" << std::endl;
        file << "      \"capacity\": " << working_memory->get_capacity() << "," << std::endl;
        file << "      \"usage\": " << working_memory->get_content_size() << "," << std::endl;
        file << "      \"efficiency\": " << (working_memory->get_content_size() / (double)working_memory->get_capacity() * 100.0) << std::endl;
        file << "    },", std::endl;
        file << "    \"episodic_memory\": {" << std::endl;
        file << "      \"buffer\": " << episodic_memory->get_buffer_size() << "," << std::endl;
        file << "      \"episodes\": " << episodic_memory->get_experience_count() << std::endl;
        file << "    },", std::endl;
        file << "    \"semantic_memory\": {" << std::endl;
        file << "      \"index\": " << semantic_memory->get_index_size() << std::endl;
        file << "    }" << std::endl;
        file << "  }" << std::endl;
        file << "}" << std::endl;
        
        file.close();
        std::cout << "Memory results saved to: memory_results.json" << std::endl;
    }
}

std::vector<std::string> generate_random_observations(size_t count) {
    std::vector<std::string> observations;
    std::vector<std::string> possible_observations = {
        "visual_scene", "auditory_input", "motor_command", "internal_signal",
        "memory_access", "decision", "action", "reward"
    };
    
    for (size_t i = 0; i < count; ++i) {
        observations.push_back(possible_observations[rand() % possible_observations.size()]);
    }
    
    return observations;
}

if __name__ == "__main__":
    duration = {duration}
    timestep = {timestep}
    neuron_count = {neuron_count}
    environment_width = {environment_width}
    environment_height = {environment_height}
    log_level = "{log_level}"
    memory_enabled = {memory_enabled}
    episodic_memory_enabled = {episodic_memory_enabled}
    working_memory_enabled = {working_memory_enabled}
    semantic_memory_enabled = {semantic_memory_enabled}
    neuromodulation_enabled = {neuromodulation_enabled}
    memory_load = "{memory_load}"
    recall_test = {recall_test}
    
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    experiment_name = "{experiment_name}"
    
    run_memory_experiment()
'''
    
    def get_neuromodulation_study_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
Neuromodulation Investigation Script
Generated by NLM Simulation Template Generator
Template: Neuromodulation Investigation
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision
from nlm.memory import WorkingMemory, EpisodicMemory
from nlm.neuromodulation import Neuromodulator
import math

def run_neuromodulation_study():
    """Run neuromodulation investigation"""
    print("Starting NLM Neuromodulation Investigation")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print(f"Duration: {duration}s")
    print(f"Time Step: {timestep}s")
    print(f"Neuron Count: {neuron_count}")
    print(f"Neuromodulation Effects: {neuromodulation_effects}")
    print(f"Reward Structure: {reward_structure}")
    print("=" * 50)
    
    // Initialize configuration
    config = Config()
    config.set("random_seed", 12345)
    config.set("simulation_timestep", timestep)
    config.set("neuron_count", neuron_count)
    config.set("region_count", 2)
    config.set("connection_probability", 0.05)
    config.set("plasticity_learning_rate", 0.005)
    config.set("dopamine_baseline", dopamine_baseline)
    config.set("curiosity_baseline", curiosity_baseline)
    config.set("novelty_baseline", novelty_baseline)
    config.set("log_level", log_level)
    
    // Create brain
    brain = Brain(config)
    brain.initialize()
    
    // Create neuromodulator
    neuromodulator = Neuromodulator()
    neuromodulator.set_modulator_level("dopamine", dopamine_baseline)
    neuromodulator.set_modulator_level("curiosity", curiosity_baseline)
    neuromodulator.set_modulator_level("novelty", novelty_baseline)
    
    print(f"Brain initialized with {brain->getTotalNeuronCount()} neurons")
    print(f"Neuromodulation enabled with:")
    print(f"  Dopamine: {dopamine_baseline}")
    print(f"  Curiosity: {curiosity_baseline}")
    print(f"  Novelty: {novelty_baseline}")
    
    // Run neuromodulation investigation
    print("\nRunning neuromodulation investigation...")
    run_neuromodulation_investigation(brain, neuromodulator);
    
    // Analyze neuromodulation effects
    analyze_neuromodulation_effects(brain, neuromodulator, neuromodulation_effects);
    
    // Generate neuromodulation report
    generate_neuromodulation_report(brain, neuromodulator, experiment_name);
    
    print("\nNeuromodulation investigation completed successfully!")
}

def run_neuromodulation_investigation(std::shared_ptr<Brain> brain, std::shared_ptr<Neuromodulator> neuromodulator) {
    size_t max_steps = int(duration / timestep);
    
    for (size_t step = 0; step < max_steps; ++step) {
        current_time = step * timestep;
        
        // Apply neuromodulation effects
        double current_dopamine = neuromodulator->get_modulator_level("dopamine");
        double current_curiosity = neuromodulator->get_modulator_level("curiosity");
        double current_novelty = neuromodulator->get_modulator_level("novelty");
        
        // Modify neural excitability based on neuromodulators
        if (current_dopamine > 0.0) {
            // Dopamine enhances learning
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f * current_dopamine);
        }
        
        if (current_curiosity > 0.0) {
            // Curiosity promotes exploration
            brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f * current_curiosity);
        }
        
        if (current_novelty > 0.0) {
            // Novelty affects attention and learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f * current_novelty);
        }
        
        // Run simulation step
        brain->step(step, current_time);
        
        // Update neuromodulator levels based on experience
        update_neuromodulator_levels(neuromodulator, step, current_time);
        
        // Record data periodically
        if (step % 50 == 0) {
            print(f"Step {step}/{max_steps} ({current_time:.2f}s) - "
                  f"Dopa: {current_dopamine:.2f}, "
                  f"Curiosity: {current_curiosity:.2f}, "
                  f"Novelty: {current_novelty:.2f}")
        }
        
        // Sleep for visualization
        if (step < max_steps - 1) {
            // Small delay
        }
    }
}

def update_neuromodulator_levels(std::shared_ptr<Neuromodulator> neuromodulator, size_t step, double time) {
    // Simulate neuromodulator level changes over time
    double dopamine = neuromodulator->get_modulator_level("dopamine");
    double curiosity = neuromodulator->get_modulator_level("curiosity");
    double novelty = neuromodulator->get_modulator_level("novelty");
    
    // Simple dynamics: decay toward baseline, occasional spikes
    double new_dopamine = dopamine * 0.95 + (rand() % 10 == 0 ? 0.2 : 0.0);
    double new_curiosity = curiosity * 0.95 + (rand() % 15 == 0 ? 0.3 : 0.0);
    double new_novelty = novelty * 0.95 + (rand() % 20 == 0 ? 0.1 : 0.0);
    
    neuromodulator->set_modulator_level("dopamine", std::min(new_dopamine, 1.0));
    neuromodulator->set_modulator_level("curiosity", std::min(new_curiosity, 1.0));
    neuromodulator->set_modulator_level("novelty", std::min(new_novelty, 1.0));
}

def analyze_neuromodulation_effects(std::shared_ptr<Brain> brain, std::shared_ptr<Neuromodulator> neuromodulator,
                                     std::vector<std::string> effects) {
    std::cout << "\nNeuromodulation Effects Analysis:" << std::endl;
    
    for (const auto& effect : effects) {
        double effect_value = 0.0;
        
        if (effect == "dopamine") {
            effect_value = analyze_dopamine_effects(brain, neuromodulator);
        } else if (effect == "curiosity") {
            effect_value = analyze_curiosity_effects(brain, neuromodulator);
        } else if (effect == "novelty") {
            effect_value = analyze_novelty_effects(brain, neuromodulator);
        }
        
        std::cout << "  " << effect << " effect: " << effect_value << std::endl;
    }
}

def analyze_dopamine_effects(std::shared_ptr<Brain> brain, std::shared_ptr<Neuromodulator> neuromodulator) {
    size_t spike_count = brain->getTotalSpikeCount();
    double dopamine = neuromodulator->get_modulator_level("dopamine");
    
    // Dopamine enhances learning and plasticity
    return (spike_count / 100.0) * dopamine;
}

def analyze_curiosity_effects(std::shared_ptr<Brain> brain, std::shared_ptr<Neuromodulator> neuromodulator) {
    // Curiosity promotes exploration and new experiences
    return neuromodulator->get_modulator_level("curiosity") * 50.0;
}

def analyze_novelty_effects(std::shared_ptr<Brain> brain, std::shared_ptr<Neuromodulator> neuromodulator) {
    // Novelty affects attention and learning speed
    return neuromodulator->get_modulator_level("novelty") * 30.0;
}

def generate_neuromodulation_report(std::shared_ptr<Brain> brain, std::shared_ptr<Neuromodulator> neuromodulator,
                                     const std::string& experimentName) {
    std::ofstream file("neuromodulation_report.json");
    if (file.is_open()) {
        file << "{" << std::endl;
        file << "  \"experiment\": \"" << experimentName << "\"," << std::endl;
        file << "  \"timestamp\": \"" << time(0) << "\"," << std::endl;
        file << "  \"neuromodulation_summary\": {" << std::endl;
        file << "    \"dopamine_level\": " << neuromodulator->get_modulator_level("dopamine") << "," << std::endl;
        file << "    \"curiosity_level\": " << neuromodulator->get_modulator_level("curiosity") << "," << std::endl;
        file << "    \"novelty_level\": " << neuromodulator->get_modulator_level("novelty") << std::endl;
        file << "  }" << std::endl;
        file << "}" << std::endl;
        
        file.close();
        std::cout << "Neuromodulation report generated: neuromodulation_report.json" << std::endl;
    }
}

if __name__ == "__main__":
    duration = {duration}
    timestep = {timestep}
    neuron_count = {neuron_count}
    environment_width = {environment_width}
    environment_height = {environment_height}
    log_level = "{log_level}"
    memory_enabled = {memory_enabled}
    neuromodulation_enabled = {neuromodulation_enabled}
    dopamine_baseline = {dopamine_baseline}
    curiosity_baseline = {curiosity_baseline}
    novelty_baseline = {novelty_baseline}
    reward_structure = "{reward_structure}"
    neuromodulation_effects = {neuromodulation_effects}
    
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    experiment_name = "{experiment_name}"
    
    run_neuromodulation_study()
'''
    
    def get_developmental_simulation_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
Developmental Simulation Script
Generated by NLM Simulation Template Generator
Template: Developmental Simulation
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision
from nlm.memory import WorkingMemory, EpisodicMemory
from nlm.neuromodulation import Neuromodulator
from nlm.development import DevelopmentalSystem
import math

def run_developmental_simulation() {
    """Run developmental simulation"""
    print("Starting NLM Developmental Simulation")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print(f"Duration: {duration}s")
    print(f"Time Step: {timestep}s")
    print(f"Neuron Count: {neuron_count}")
    print(f"Initial Neurons: {initial_neurons}")
    print(f"Development Enabled: {development_enabled}")
    print(f"Age Effects: {age_effects}")
    print("=" * 50)
    
    // Initialize configuration
    config = Config()
    config.set("random_seed", 999)
    config.set("simulation_timestep", timestep)
    config.set("neuron_count", neuron_count)
    config.set("region_count", 2)
    config.set("connection_probability", 0.02)
    config.set("plasticity_learning_rate", 0.001)
    config.set("synaptogenesis_rate", synaptogenesis_rate)
    config.set("pruning_rate", pruning_rate)
    config.set("maturation_rate", maturation_rate)
    config.set("log_level", log_level)
    
    // Create brain
    brain = Brain(config)
    brain.initialize()
    
    // Simulate developmental stages
    if {development_enabled}) {
        auto* development_system = brain->getDevelopmentSystem();
        if (development_system) {
            std::cout << "Developmental system initialized" << std::endl;
            std::cout << "Synaptogenesis rate: " << development_system->getSynaptogenesisRate() << std::endl;
            std::cout << "Pruning rate: " << development_system->getPruningRate() << std::endl;
            std::cout << "Maturation rate: " << development_system->getMaturationRate() << std::endl;
        }
    }
    
    print(f"Brain initialized with {brain->getTotalNeuronCount()} neurons")
    print(f"Development enabled: {development_enabled}")
    
    // Run developmental simulation
    print("\nRunning developmental simulation...")
    run_developmental_stages(brain);
    
    // Analyze development
    analyze_development(brain);
    
    // Generate developmental report
    generate_developmental_report(brain, experiment_name);
    
    print("\nDevelopmental simulation completed successfully!")
}

def run_developmental_stages(std::shared_ptr<Brain> brain) {
    size_t max_steps = int(duration / timestep);
    
    for (size_t step = 0; step < max_steps; ++step) {
        current_time = step * timestep;
        
        // Simulate age effects
        double current_age = current_time / (duration / 10.0);  // Age from 0 to 10
        
        // Apply age-dependent effects
        if {age_effects}) {
            double age_factor = 1.0 - (current_age / 10.0);  // Decreases with age
            
            // Modify brain properties based on age
            if (age_factor < 0.5) {
                // Critical period (high plasticity)
                brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f * age_factor);
            } else {
                // Adult period (lower plasticity)
                brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f * age_factor);
            }
        }
        
        // Run simulation step
        brain->step(step, current_time);
        
        // Track developmental stage
        print(f"Step {step}/{max_steps} ({current_time:.2f}s) - Age: {current_age:.1f}, "
              f"Neurons: {brain->getTotalNeuronCount()}, "
              f"Spikes: {brain->getTotalSpikeCount()}")
        
        // Small delay for visualization
        if (step < max_steps - 1) {
            // Sleep for visualization
        }
    }
}

def analyze_development(std::shared_ptr<Brain> brain) {
    std::cout << "\nDevelopment Analysis:" << std::endl;
    
    auto* development_system = brain->getDevelopmentSystem();
    if (!development_system) return;
    
    size_t final_neurons = brain->getTotalNeuronCount();
    size_t initial_neurons = {initial_neurons};
    
    double growth_rate = (final_neurons - initial_neurons) / (double)(final_neurons);
    double maturation_level = development_system->getMaturationRate();
    
    std::cout << "  Neural Growth:" << std::endl;
    std::cout << "    Initial neurons: " << initial_neurons << std::endl;
    std::cout << "    Final neurons: " << final_neurons << std::endl;
    std::cout << "    Growth rate: " << growth_rate << std::endl;
    
    std::cout << "  Developmental Processes:" << std::endl;
    std::cout << "    Synaptogenesis rate: " << development_system->getSynaptogenesisRate() << std::endl;
    std::cout << "    Pruning rate: " << development_system->getPruningRate() << std::endl;
    std::cout << "    Maturation rate: " << maturation_level << std::endl;
    
    double development_maturity = (maturation_level + growth_rate) / 2.0;
    std::cout << "  Development Maturity: " << development_maturity * 100 << "%" << std::endl;
}

def generate_developmental_report(std::shared_ptr<Brain> brain, const std::string& experimentName) {
    std::ofstream file("developmental_report.json");
    if (file.is_open()) {
        file << "{" << std::endl;
        file << "  \"experiment\": \"" << experimentName << "\"," << std::endl;
        file << "  \"timestamp\": \"" << time(0) << "\"," << std::endl;
        file << "  \"developmental_summary\": {" << std::endl;
        
        auto* development_system = brain->getDevelopmentSystem();
        if (development_system) {
            file << "    \"synaptogenesis_rate\": " << development_system->getSynaptogenesisRate() << "," << std::endl;
            file << "    \"pruning_rate\": " << development_system->getPruningRate() << "," << std::endl;
            file << "    \"maturation_rate\": " << development_system->getMaturationRate() << std::endl;
        } else {
            file << "    \"developmental_system\": \"inactive\"" << std::endl;
        }
        
        file << "  }" << std::endl;
        file << "}" << std::endl;
        
        file.close();
        std::cout << "Developmental report generated: developmental_report.json" << std::endl;
    }
}

if __name__ == "__main__":
    duration = {duration}
    timestep = {timestep}
    neuron_count = {neuron_count}
    initial_neurons = {initial_neurons}
    environment_width = {environment_width}
    environment_height = {environment_height}
    log_level = "{log_level}"
    memory_enabled = {memory_enabled}
    neuromodulation_enabled = {neuromodulation_enabled}
    development_enabled = {development_enabled}
    age_effects = {age_effects}
    
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    experiment_name = "{experiment_name}"
    
    run_developmental_simulation()
'''
    
    def get_benchmark_suite_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
Benchmark Suite Script
Generated by NLM Simulation Template Generator
Template: Benchmark Suite
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision
from nlm.memory import WorkingMemory, EpisodicMemory
from nlm.neuromodulation import Neuromodulator
import time
import json

def run_benchmark_suite():
    """Run comprehensive benchmark suite"""
    print("Starting NLM Benchmark Suite")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print(f"Neuron Counts: {neuron_counts}")
    print(f"Simulation Steps: {simulation_steps}")
    print(f"Environment: {environment_type}")
    print(f"Output Format: {output_format}")
    print(f"Parallel Processing: {parallel_processing}")
    print("=" * 50)
    
    // Generate benchmark results
    benchmark_results = run_comprehensive_benchmarks(
        neuron_counts, simulation_steps, environment_type,
        parallel_processing, output_format, performance_analysis
    );
    
    // Analyze benchmark results
    analyze_benchmark_results(benchmark_results);
    
    // Generate benchmark report
    generate_benchmark_report(benchmark_results, experiment_name);
    
    print("\nBenchmark suite completed successfully!")
}

def run_comprehensive_benchmarks(std::vector<size_t> neuron_counts, std::vector<size_t> simulation_steps,
                                 const std::string& environment_type, bool parallel_processing,
                                 const std::string& output_format, bool performance_analysis) {
    BenchmarkResults results;
    
    for (size_t neuron_count : neuron_counts) {
        for (size_t steps : simulation_steps) {
            print(f"Running benchmark: {neuron_count} neurons, {steps} steps...");
            
            // Initialize configuration
            Config config;
            config.set("random_seed", 12345);
            config.set("simulation_timestep", 0.001);
            config.set("neuron_count", neuron_count);
            config.set("region_count", 2);
            config.set("connection_probability", 0.1);
            config.set("plasticity_learning_rate", 0.01);
            config.set("log_level", "WARNING");
            
            // Create and run brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run benchmark
            auto start_time = std::chrono::high_resolution_clock::now();
            run_benchmark_simulation(brain, steps);
            auto end_time = std::chrono::high_resolution_clock::now();
            
            double execution_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
            
            // Record results
            BenchmarkResult result;
            result.neuron_count = neuron_count;
            result.simulation_steps = steps;
            result.execution_time = execution_time;
            result.total_spikes = brain->getTotalSpikeCount();
            result.firing_rate = (result.total_spikes / execution_time) * 1000.0;
            
            results.benchmark_results.push_back(result);
            
            // Sleep for next benchmark
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    return results;
}

def run_benchmark_simulation(std::shared_ptr<Brain> brain, size_t steps) {
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Occasionally inject input to simulate learning
        if (step % 50 == 0 && step > 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
    }
}

def analyze_benchmark_results(const BenchmarkResults& results) {
    std::cout << "\nBenchmark Analysis:" << std::endl;
    
    double total_time = 0.0;
    double max_spikes = 0.0;
    double min_spikes = std::numeric_limits<size_t>::max();
    
    for (const auto& result : results.benchmark_results) {
        total_time += result.execution_time;
        max_spikes = std::max(max_spikes, static_cast<double>(result.total_spikes));
        min_spikes = std::min(min_spikes, static_cast<double>(result.total_spikes));
        
        std::cout << "  " << result.neuron_count << " neurons, "
                  << result.simulation_steps << " steps: "
                  << result.execution_time << "ms, "
                  << result.total_spikes << " spikes, "
                  << result.firing_rate << " Hz" << std::endl;
    }
    
    double avg_time = total_time / results.benchmark_results.size();
    std::cout << "\n  Average execution time: " << avg_time << "ms" << std::endl;
    std::cout << "  Spike range: " << min_spikes << " - " << max_spikes << std::endl;
}

def generate_benchmark_report(const BenchmarkResults& results, const std::string& experimentName) {
    std::ofstream file("benchmark_report.json");
    if (file.is_open()) {
        file << "{" << std::endl;
        file << "  \"experiment\": \"" << experiment_name << "\"," << std::endl;
        file << "  \"timestamp\": \"" << time(0) << "\"," << std::endl;
        file << "  \"benchmark_results\": [" << std::endl;
        
        for (size_t i = 0; i < results.benchmark_results.size(); ++i) {
            const auto& result = results.benchmark_results[i];
            file << "    {" << std::endl;
            file << "      \"neuron_count\": " << result.neuron_count << "," << std::endl;
            file << "      \"simulation_steps\": " << result.simulation_steps << "," << std::endl;
            file << "      \"execution_time\": " << result.execution_time << "," << std::endl;
            file << "      \"total_spikes\": " << result.total_spikes << "," << std::endl;
            file << "      \"firing_rate\": " << result.firing_rate << std::endl;
            
            if (i < results.benchmark_results.size() - 1) {
                file << "    }," << std::endl;
            } else {
                file << "    }" << std::endl;
            }
        }
        
        file << "  ]" << std::endl;
        file << "}" << std::endl;
        
        file.close();
        std::cout << "Benchmark report generated: benchmark_report.json" << std::endl;
    }
}

struct BenchmarkResult {
    size_t neuron_count;
    size_t simulation_steps;
    double execution_time;
    size_t total_spikes;
    double firing_rate;
};

struct BenchmarkResults {
    std::vector<BenchmarkResult> benchmark_results;
};

if __name__ == "__main__":
    neuron_counts = {neuron_counts}
    simulation_steps = {simulation_steps}
    environment_width = {environment_width}
    environment_height = {environment_height}
    log_level = "{log_level}"
    memory_enabled = {memory_enabled}
    neuromodulation_enabled = {neuromodulation_enabled}
    parallel_processing = {parallel_processing}
    output_format = "{output_format}"
    performance_analysis = {performance_analysis}
    
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    experiment_name = "{experiment_name}"
    
    run_benchmark_suite()
'''
    
    def get_generic_template(self) -> str:
        return '''#!/usr/bin/env python3
"""
NLM Simulation Script
Generated by NLM Simulation Template Generator
Template: Custom Template
Experiment: {experiment_name}
Generated: {generation_time}
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.core import Brain, Config
from nlm.environment import SimpleWorld
from nlm.sensory import Vision

def run_custom_simulation():
    """Run custom NLM simulation"""
    print("Starting NLM Custom Simulation")
    print("=" * 50)
    print(f"Experiment Name: {experiment_name}")
    print("=" * 50)
    
    // Initialize configuration
    Config config;
    config.set("random_seed", 42);
    config.set("simulation_timestep", timestep);
    config.set("neuron_count", neuron_count);
    config.set("region_count", 1);
    config.set("connection_probability", 0.1);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    print(f"Brain initialized with {brain->getTotalNeuronCount()} neurons")
    
    // Run simulation
    print("Starting simulation...");
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step, step * timestep);
        
        if (step % 20 == 0) {
            print(f"Step {step}: {brain->getTotalSpikeCount()} spikes");
        }
    }
    
    print("\nSimulation completed!")
    brain->logStatus();
    
    print("Custom simulation completed successfully!")
}

if __name__ == "__main__":
    timestep = {timestep}
    neuron_count = {neuron_count}
    environment_width = {environment_width}
    environment_height = {environment_height}
    
    generation_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    experiment_name = "{experiment_name}"
    
    run_custom_simulation()
'''
    
    def generate_readme(self, template: Dict[str, Any], variables: Dict[str, Any], readme_path: str):
        """Generate README file for the template"""
        template_name = template["name"]
        description = template["description"]
        
        readme_content = f"""# {template_name}

{description}

## Generated Files

- `{{{template['template_file']}}}` - Main simulation script
- `config_{{{'{template_name}'.replace(' ', '_').lower()}}}.json` - Configuration file

## Usage

Run the simulation:
```bash
python {template['template_file']}
```

## Configuration

The following parameters are configurable:

"""
        
        for key, value in variables.items():
            readme_content += f"- `{key}`: {value}\n"
        
        readme_content += f"""
## Template Commands

The following commands are available for this template:

"""
        
        for command in template["commands"]:
            readme_content += f"- `{command}`\n"
        
        readme_content += f"""
## Notes

This simulation was generated by the NLM Simulation Template Generator.
The script follows standard NLM conventions and includes comprehensive logging.

For more information, refer to the NLM documentation.

## Generated
{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
"""
        
        with open(readme_path, 'w') as f:
            f.write(readme_content)
    
    def generate_config_file(self, variables: Dict[str, Any], config_path: str):
        """Generate configuration file"""
        config = {
            "metadata": {
                "generated_by": "NLM Simulation Template Generator",
                "version": "1.0.0",
                "generated_at": datetime.now().isoformat(),
                "experiment_name": variables.get("experiment_name", "unknown")
            }
        }
        
        # Add all variables except metadata fields
        for key, value in variables.items():
            if key not in ["metadata", "experiment_name", "generation_time"]:
                config[key] = value
        
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)

# Main function for standalone execution
def main():
    """Main function for standalone execution"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Simulation Template Generator"
    )
    
    parser.add_argument(
        "template",
        choices=["basic_simulation", "plasticity_study", "memory_experiment", 
                 "neuromodulation_study", "developmental_simulation", "benchmark_suite"],
        required=True,
        help="Name of template to generate"
    )
    
    parser.add_argument(
        "--output-dir",
        default="templates",
        help="Output directory for templates"
    )
    
    parser.add_argument(
        "--experiment-name",
        help="Name for the experiment"
    )
    
    parser.add_argument(
        "--list",
        action="store_true",
        help="List available templates"
    )
    
    args = parser.parse_args()
    
    if args.list:
        generator = NLMSimulationTemplateGenerator()
        generator.list_templates()
        return
    
    generator = NLMSimulationTemplateGenerator()
    
    # Get template definition
    template_name = args.template
    if template_name not in generator.template_definitions:
        print(f"Error: Unknown template '{template_name}'")
        return 1
    
    template = generator.template_definitions[template_name]
    
    print(f"Generating simulation script: {template_name}")
    print(f"Description: {template['description']}")
    
    # Generate script
    success = generator.generate_simulation_script(
        template_name,
        args.output_dir,
        experiment_name=args.experiment_name
    )
    
    if success:
        print("\nTemplate generation completed successfully!")
        return 0
    else:
        print("\nTemplate generation failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
