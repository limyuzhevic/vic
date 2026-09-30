# Advanced Examples for NLM Neural Learning Machine

This directory contains advanced examples demonstrating the enhanced Python API for the NLM neural simulation framework.

## Overview

These examples showcase the full capabilities of the enhanced NLM Python interface, including:
- Advanced simulation techniques
- Memory system integration
- Neuromodulation and learning optimization
- Visualization and analysis tools
- Experiment management
- Performance optimization

## Example Structure

```
advanced_examples/
├── brain_simulation/
│   ├── basic_simulation.py
│   ├── learning_simulation.py
│   └── memory_simulation.py
├── agent_training/
│   ├── reinforcement_learning.py
│   ├── curiosity_driven_learning.py
│   └── reward_prediction.py
├── visualization/
│   ├── neural_activity_analysis.py
│   ├── plasticity_evolution.py
│   └── memory_traces.py
├── experiment_management/
│   ├── parameter_sweep.py
│   ├── cross_validation.py
│   └── experiment_tracking.py
├── performance_optimization/
│   ├── profiling.py
│   ├── memory_management.py
│   └── parallel_processing.py
├── advanced_topics/
│   ├── custom_brain_models.py
│   ├── neural_network_analysis.py
│   ├── neuromodulation_studies.py
│   └── developmental_dynamics.py
└── integrated_systems/
    ├── complete_agent_world.py
    ├── memory_prediction.py
    ├── cognitive_planning.py
    └── multi_agent_scenarios.py
```

## Example 1: Advanced Brain Simulation

### File: `advanced_examples/brain_simulation/learning_simulation.py`

```python
import nlm
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple

class AdvancedBrainSimulation:
    """
    Advanced brain simulation demonstrating enhanced learning capabilities.
    
    This example showcases:
    - Custom brain configurations
    - Multiple learning mechanisms
    - Performance monitoring
    - Visualization of neural dynamics
    """
    
    def __init__(self, config: Dict = None):
        """Initialize advanced brain simulation.
        
        Args:
            config: Configuration dictionary with simulation parameters
        """
        self.config = config or self._get_default_config()
        self.brain = None
        self.world = None
        self.agent = None
        self.simulation_results = {}
        self.performance_metrics = {}
    
    def _get_default_config(self) -> Dict:
        """Get default configuration for advanced simulation."""
        return {
            'brain': {
                'neuron_count': 5000,
                'connection_density': 0.15,
                'plasticity_enabled': True,
                'development_enabled': True,
                'memory_capacity': 5000,
                'simulation': {
                    'timestep': 0.01,
                    'max_steps': 5000,
                    'random_seed': 42
                }
            },
            'world': {
                'width': 30,
                'height': 30,
                'vision_width': 12,
                'vision_height': 12,
                'hazard_density': 0.1,
                'resource_density': 0.3,
                'wall_density': 0.2
            },
            'agent': {
                'learning_rate': 0.1,
                'enable_reward_modulation': True,
                'enable_curiosity': True,
                'enable_structural_plasticity': True,
                'enable_development': True,
                'enable_novelty_detection': True
            },
            'training': {
                'episodes': 100,
                'max_steps_per_episode': 500,
                'eval_interval': 10,
                'save_interval': 25
            }
        }
    
    def setup_simulation(self):
        """Setup the complete simulation environment."""
        print("Setting up advanced brain simulation...")
        
        # Create configuration
        config = nlm.create_default_config()
        
        # Configure brain with advanced parameters
        brain_config = self.config['brain']
        config.set("neuron_count", brain_config['neuron_count'])
        config.set("connection_density", brain_config['connection_density'])
        config.set("plasticity_enabled", brain_config['plasticity_enabled'])
        config.set("development_enabled", brain_config['development_enabled'])
        config.set("memory_capacity", brain_config['memory_capacity'])
        
        # Create brain
        self.brain = nlm.Brain(config)
        self.brain.initialize()
        
        # Create world with enhanced features
        world_config = self.config['world']
        self.world = nlm.SimpleWorld(
            width=world_config['width'],
            height=world_config['height'],
            vision_width=world_config['vision_width'],
            vision_height=world_config['vision_height']
        )
        
        # Configure world with advanced parameters
        self.world.setMaxEnergy(100.0)
        self.world.setEnergyDecayRate(0.01)
        self.world.setHazardDensity(world_config['hazard_density'])
        self.world.setResourceDensity(world_config['resource_density'])
        self.world.setWallDensity(world_config['wall_density'])
        self.world.setRandomSeed(self.config['brain']['simulation']['random_seed'])
        
        # Create agent with advanced learning capabilities
        agent_config = self.config['agent']
        self.agent = nlm.createAgentBrain(self.brain, self.world)
        
        # Enable advanced learning features
        self.agent.enable_features([
            'reward_modulation',
            'curiosity',
            'structural_plasticity',
            'development',
            'novelty_detection'
        ])
        
        # Configure learning parameters
        self.agent.enable_reward_modulation(agent_config['enable_reward_modulation'])
        self.agent.enable_curiosity(agent_config['enable_curiosity'])
        self.agent.enable_structural_plasticity(agent_config['enable_structural_plasticity'])
        self.agent.enable_development(agent_config['enable_development'])
        self.agent.enable_novelty_detection(agent_config['enable_novelty_detection'])
        
        print("✓ Simulation setup complete")
    
    def run_advanced_simulation(self) -> Dict:
        """Run the advanced simulation with comprehensive analysis.
        
        Returns:
            Dictionary containing simulation results and performance metrics
        """
        print(f"Running advanced simulation for {self.config['training']['episodes']} episodes...")
        
        training_config = self.config['training']
        eval_interval = training_config['eval_interval']
        save_interval = training_config['save_interval']
        
        # Initialize tracking
        episode_rewards = []
        episode_novelty = []
        episode_curiosity = []
        episode_steps = []
        performance_over_time = []
        
        for episode in range(training_config['episodes']):
            print(f"Episode {episode + 1}/{training_config['episodes']}")
            
            # Run episode
            episode_result = self._run_episode(
                max_steps=training_config['max_steps_per_episode'],
                verbose=True
            )
            
            # Collect metrics
            episode_rewards.append(episode_result['total_reward'])
            episode_novelty.append(episode_result['novelty_level'])
            episode_curiosity.append(episode_result['curiosity_level'])
            episode_steps.append(episode_result['steps_taken'])
            
            # Evaluate performance periodically
            if (episode + 1) % eval_interval == 0:
                performance = self._evaluate_performance(episode + 1)
                performance_over_time.append(performance)
                self._plot_progress(episode + 1, episode_rewards, episode_novelty, episode_curiosity)
            
            # Save model periodically
            if (episode + 1) % save_interval == 0:
                filename = f"advanced_brain_episode_{episode + 1}.nlm"
                self.brain.save(filename)
                print(f"  Model saved to {filename}")
        
        # Compile final results
        results = self._compile_results(
            episode_rewards, episode_novelty, episode_curiosity, episode_steps,
            performance_over_time
        )
        
        # Generate comprehensive analysis
        self._generate_comprehensive_analysis(results)
        
        print("✓ Advanced simulation complete")
        return results
    
    def _run_episode(self, max_steps: int, verbose: bool = False) -> Dict:
        """Run a single training episode.
        
        Args:
            max_steps: Maximum number of steps per episode
            verbose: Whether to print verbose output
            
        Returns:
            Dictionary containing episode results
        """
        self.world.reset()
        self.agent.reset()
        
        double total_reward = 0.0;
        double novelty_level = 0.0;
        double curiosity_level = 0.0;
        size_t steps_taken = 0;
        
        for step in range(max_steps) {
            if (verbose && step % 100 == 0) {
                std::cout << "  Step " << step << "/" << max_steps << std::endl;
            }
            
            // Update world
            self.world.update(0.1);
            
            // Get sensory input
            auto percept = self.world.getSensoryPercept();
            
            // Process sensory input
            self.agent.processSensoryInput(percept);
            
            // Brain thinks
            self.brain.step(step);
            
            // Get action
            auto action = self.agent.decodeMotorCommand();
            
            // Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime());
            
            // Collect rewards
            double reward = self.world.getReward();
            total_reward += reward;
            
            // Collect neuromodulation metrics
            novelty_level += self.agent.getNoveltyLevel();
            curiosity_level += self.agent.getCuriosityLevel();
            
            steps_taken++;
            
            if (self.world.isEpisodeFinished()) {
                break;
            }
        }
        
        return {
            'total_reward': total_reward,
            'novelty_level': novelty_level / std::max(steps_taken, 1),
            'curiosity_level': curiosity_level / std::max(steps_taken, 1),
            'steps_taken': steps_taken,
            'final_brain_state': std::make_pair(self.brain.getTotalNeuronCount(), self.brain.getAverageFiringRate())
        };
    }
    
    def _evaluate_performance(self, episode: int) -> Dict:
        """Evaluate current performance.
        
        Args:
            episode: Current episode number
            
        Returns:
            Dictionary containing performance metrics
        """
        auto brain_stats = this->brain->getStatistics();
        auto agent_state = this->agent->getCurrentState();
        
        return {
            'episode': episode,
            'brain_stats': brain_stats,
            'agent_state': agent_state,
            'learning_efficiency': this->_calculate_learning_efficiency(),
            'novelty_detection_rate': this->_calculate_novelty_detection_rate(),
            'curiosity_driven_exploration': this->_calculate_curiosity_exploration()
        };
    }
    
    void _plot_progress(int episode, const std::vector<double>& rewards,
                       const std::vector<double>& novelty, const std::vector<double>& curiosity) {
        // Create progress visualization
        plt.figure(figsize=(15, 10));
        
        plt.subplot(3, 1, 1);
        plt.plot(rewards);
        plt.title('Episode Rewards');
        plt.ylabel('Reward');
        plt.grid(true);
        
        plt.subplot(3, 1, 2);
        plt.plot(novelty);
        plt.title('Novelty Levels');
        plt.ylabel('Novelty');
        plt.grid(true);
        
        plt.subplot(3, 1, 3);
        plt.plot(curiosity);
        plt.title('Curiosity Levels');
        plt.ylabel('Curiosity');
        plt.xlabel('Episode');
        plt.grid(true);
        
        std::string filename = "advanced_simulation_progress_episode_" + std::to_string(episode) + ".png";
        plt.savefig(filename, dpi=150, bbox_inches='tight');
        plt.close();
        
        std::cout << "  Progress plot saved to " << filename << std::endl;
    }
    
    void _compile_results(const std::vector<double>& rewards,
                         const std::vector<double>& novelty,
                         const std::vector<double>& curiosity,
                         const std::vector<size_t>& steps,
                         const std::vector<std::map<std::string, py::object>>& performance) {
        // Compile final results
        this->simulation_results["rewards"] = rewards;
        this->simulation_results["novelty"] = novelty;
        this->simulation_results["curiosity"] = curiosity;
        this->simulation_results["steps"] = steps;
        this->simulation_results["performance"] = performance;
        
        // Calculate summary statistics
        double mean_reward = std::accumulate(rewards.begin(), rewards.end(), 0.0) / rewards.size();
        double std_reward = this->_calculate_standard_deviation(rewards, mean_reward);
        
        this->performance_metrics["mean_reward"] = mean_reward;
        this->performance_metrics["reward_std"] = std_reward;
        this->performance_metrics["max_reward"] = *std::max_element(rewards.begin(), rewards.end());
        this->performance_metrics["min_reward"] = *std::min_element(rewards.begin(), rewards.end());
        
        double mean_novelty = std::accumulate(novelty.begin(), novelty.end(), 0.0) / novelty.size();
        double mean_curiosity = std::accumulate(curiosity.begin(), curiosity.end(), 0.0) / curiosity.size();
        
        this->performance_metrics["mean_novelty"] = mean_novelty;
        this->performance_metrics["mean_curiosity"] = mean_curiosity;
        
        double total_steps = std::accumulate(steps.begin(), steps.end(), 0.0);
        this->performance_metrics["total_steps"] = total_steps;
        this->performance_metrics["steps_per_episode"] = total_steps / steps.size();
    }
    
    void _generate_comprehensive_analysis(const std::map<std::string, py::object>& results) {
        // Generate comprehensive analysis
        std::cout << "\n=== ADVANCED SIMULATION ANALYSIS ===" << std::endl;
        std::cout << "Total Episodes: " << results.at("rewards").size() << std::endl;
        std::cout << "Mean Reward: " << this->performance_metrics.at("mean_reward") << std::endl;
        std::cout << "Reward Standard Deviation: " << this->performance_metrics.at("reward_std") << std::endl;
        std::cout << "Max Reward: " << this->performance_metrics.at("max_reward") << std::endl;
        std::cout << "Min Reward: " << this->performance_metrics.at("min_reward") << std::endl;
        
        std::cout << "\nNeuromodulation Metrics:" << std::endl;
        std::cout << "Mean Novelty: " << this->performance_metrics.at("mean_novelty") << std::endl;
        std::cout << "Mean Curiosity: " << this->performance_metrics.at("mean_curiosity") << std::endl;
        
        std::cout << "\nLearning Efficiency: " << this->performance_metrics.at("learning_efficiency") << std::endl;
        std::cout << "Novelty Detection Rate: " << this->performance_metrics.at("novelty_detection_rate") << std::endl;
        std::cout << "Curiosity-Driven Exploration: " << this->performance_metrics.at("curiosity_driven_exploration") << std::endl;
    }
    
    double _calculate_learning_efficiency() {
        // Calculate learning efficiency
        double total_reward = 0.0;
        for (const auto& reward : this->simulation_results.at("rewards")) {
            total_reward += reward;
        }
        
        double mean_reward = total_reward / this->simulation_results.at("rewards").size();
        return mean_reward / this->config.at("agent").at("learning_rate");
    }
    
    double _calculate_novelty_detection_rate() {
        // Calculate novelty detection rate
        double total_novelty = 0.0;
        for (const auto& novelty : this->simulation_results.at("novelty")) {
            total_novelty += novelty;
        }
        
        return total_novelty / this->simulation_results.at("novelty").size();
    }
    
    double _calculate_curiosity_exploration() {
        // Calculate curiosity-driven exploration
        double total_curiosity = 0.0;
        for (const auto& curiosity : this->simulation_results.at("curiosity")) {
            total_curiosity += curiosity;
        }
        
        return total_curiosity / this->simulation_results.at("curiosity").size();
    }
    
    double _calculate_standard_deviation(const std::vector<double>& data, double mean) {
        double variance = 0.0;
        for (const auto& value : data) {
            variance += (value - mean) * (value - mean);
        }
        return std::sqrt(variance / data.size());
    }
};

// Main execution
int main() {
    try {
        // Create advanced simulation
        AdvancedBrainSimulation simulation;
        
        // Setup simulation
        simulation.setupSimulation();
        
        // Run advanced simulation
        auto results = simulation.runAdvancedSimulation();
        
        // Display results
        std::cout << "\n=== SIMULATION COMPLETE ===" << std::endl;
        std::cout << "Final Brain State: " << results.at("final_brain_state") << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Example 2: Memory-Enhanced Agent Training

### File: `advanced_examples/agent_training/reinforcement_learning.py`

```python
import nlm
import numpy as np
from typing import Dict, List, Tuple
import matplotlib.pyplot as plt
import seaborn as sns

class MemoryEnhancedAgentTraining:
    """
    Memory-enhanced agent training demonstrating advanced reinforcement learning.
    
    This example showcases:
    - Working memory for temporal processing
    - Episodic memory for experience storage
    - Associative memory for pattern recognition
    - Advanced reward prediction and credit assignment
    - Curiosity-driven exploration
    """
    
    def __init__(self, config: Dict = None):
        """Initialize memory-enhanced agent training.
        
        Args:
            config: Configuration dictionary with training parameters
        """
        this->config = config or this->_get_default_config();
        this->brain = nullptr;
        this->world = nullptr;
        this->agent = nullptr;
        this->working_memory = nullptr;
        this->episodic_memory = nullptr;
        this->associative_memory = nullptr;
        this->training_history = {};
        this->reward_history = {};
        this->novelty_history = {};
    
    dict _get_default_config() {
        return {
            'brain': {
                'neuron_count': 3000,
                'connection_density': 0.12,
                'plasticity_enabled': True,
                'development_enabled': True
            },
            'world': {
                'width': 20,
                'height': 20,
                'vision_width': 8,
                'vision_height': 8
            },
            'memory': {
                'working_capacity': 500,
                'working_decay': 0.05,
                'episodic_capacity': 1000,
                'associative_capacity': 2000
            },
            'training': {
                'episodes': 200,
                'max_steps': 400,
                'learning_rate': 0.05,
                'discount_factor': 0.95,
                'exploration_rate': 0.1
            }
        };
    }
    
    void setup_memory_enhanced_agent() {
        std::cout << "Setting up memory-enhanced agent..." << std::endl;
        
        // Create brain
        auto config = nlm.create_default_config();
        config.set("neuron_count", this->config.at("brain").at("neuron_count"));
        config.set("connection_density", this->config.at("brain").at("connection_density"));
        
        this->brain = nlm.Brain(config);
        this->brain.initialize();
        
        // Create world
        auto world_config = this->config.at("world");
        this->world = nlm.SimpleWorld(
            world_config.at("width"),
            world_config.at("height"),
            world_config.at("vision_width"),
            world_config.at("vision_height")
        );
        
        // Create agent
        this->agent = nlm.createAgentBrain(this->brain, this->world);
        this->agent.enable_features({
            "reward_modulation", "curiosity", "structural_plasticity", "development"
        });
        
        // Create memory systems
        auto memory_config = this->config.at("memory");
        this->working_memory = nlm.create_working_memory(
            memory_config.at("working_capacity"),
            memory_config.at("working_decay")
        );
        
        this->episodic_memory = nlm.create_episodic_memory(
            memory_config.at("episodic_capacity")
        );
        
        this->associative_memory = nlm.create_associative_memory(
            memory_config.at("associative_capacity")
        );
        
        std::cout << "✓ Memory-enhanced agent setup complete" << std::endl;
    }
    
    tuple run_memory_enhanced_training() {
        std::cout << "Running memory-enhanced agent training..." << std::endl;
        
        auto training_config = this->config.at("training");
        int episodes = training_config.at("episodes");
        int max_steps = training_config.at("max_steps");
        
        for (int episode = 0; episode < episodes; ++episode) {
            std::cout << "Episode " << (episode + 1) << "/" << episodes << std::endl;
            
            // Run episode
            auto episode_result = this->run_memory_enhanced_episode(
                max_steps, training_config.at("learning_rate")
            );
            
            // Store training history
            this->training_history[episode] = episode_result;
            this->reward_history[episode] = episode_result.at("total_reward");
            this->novelty_history[episode] = episode_result.at("novelty_level");
            
            // Display progress
            if ((episode + 1) % 20 == 0) {
                this->display_training_progress(episode + 1);
            }
        }
        
        // Generate analysis
        auto analysis = this->analyze_memory_enhanced_performance();
        this->generate_comprehensive_plots();
        
        std::cout << "✓ Memory-enhanced training complete" << std::endl;
        
        return {this->agent, this->world, this->training_history, analysis};
    }
    
    map<string, double> run_memory_enhanced_episode(int max_steps, double learning_rate) {
        // Reset environment
        this->world.reset();
        this->agent.reset();
        
        // Initialize episode tracking
        map<string, double> episode_stats;
        episode_stats["total_reward"] = 0.0;
        episode_stats["novelty_level"] = 0.0;
        episode_stats["curiosity_level"] = 0.0;
        episode_stats["steps_taken"] = 0;
        episode_stats["learning_efficiency"] = 0.0;
        
        // Experience replay buffer
        vector<map<string, double>> experience_buffer;
        
        for (int step = 0; step < max_steps; ++step) {
            // Get sensory input
            auto percept = this->world.getSensoryPercept();
            
            // Store experience in working memory
            this->working_memory.add_item(
                "current_percept",
                this->convert_percept_to_features(percept),
                learning_rate
            );
            
            // Retrieve relevant past experiences
            auto relevant_experiences = this->associative_memory.search(
                this->working_memory.get_item("current_percept"),
                10 // top 10 matches
            );
            
            // Process sensory input with memory integration
            this->process_sensory_input_with_memory(percept, relevant_experiences);
            
            // Brain thinks
            this->brain.step(step);
            
            // Get action
            auto action = this->agent.decode_motor_command();
            
            // Apply action to world
            this->world.applyMotorCommand(action, this->world.getSimulationTime());
            
            // Get reward
            double reward = this->world.getReward();
            episode_stats["total_reward"] += reward;
            
            // Store experience in episodic memory
            this->episodic_memory.record_episode(
                percept, action, reward, this->world.getSimulationTime()
            );
            
            // Calculate prediction error
            double predicted_reward = this->calculate_predicted_reward(relevant_experiences);
            double prediction_error = nlm.calculate_reward_prediction_error(reward, predicted_reward);
            
            // Apply reward modulation
            this->agent.applyRewardModulation(reward, predicted_reward);
            
            // Store experience in experience buffer
            experience_buffer.push_back({
                {"percept", this->convert_percept_to_features(percept)},
                {"action", this->action_to_features(action)},
                {"reward", reward},
                {"prediction_error", prediction_error},
                {"step", step}
            });
            
            // Perform experience replay periodically
            if (step % 50 == 0 && step > 0) {
                this->perform_experience_replay(experience_buffer, learning_rate);
            }
            
            // Collect neuromodulation metrics
            episode_stats["novelty_level"] += this->agent.getNoveltyLevel();
            episode_stats["curiosity_level"] += this->agent.getCuriosityLevel();
            
            episode_stats["steps_taken"]++;
            
            if (this->world.isEpisodeFinished()) {
                break;
            }
        }
        
        // Calculate final metrics
        episode_stats["learning_efficiency"] = this->calculate_episode_learning_efficiency(episode_stats, episode_stats["steps_taken"]);
        episode_stats["novelty_level"] /= max(episode_stats["steps_taken"], 1);
        episode_stats["curiosity_level"] /= max(episode_stats["steps_taken"], 1);
        
        return episode_stats;
    }
    
    void process_sensory_input_with_memory(const SensoryPercept& percept,
                                           const vector<map<string, double>>& relevant_experiences) {
        // Convert percept to features
        auto features = this->convert_percept_to_features(percept);
        
        // Integrate with relevant past experiences
        for (const auto& experience : relevant_experiences) {
            // Apply Hebbian learning to strengthen connections
            this->apply_hebbian_learning(features, experience);
            
            // Update internal state based on past experiences
            this->update_internal_state_from_experience(features, experience);
        }
        
        // Process through agent brain
        this->agent.processSensoryInput(percept);
    }
    
    double calculate_predicted_reward(const vector<map<string, double>>& experiences) {
        if (experiences.empty()) {
            return 0.0;
        }
        
        double total_reward = 0.0;
        for (const auto& experience : experiences) {
            total_reward += experience.at("reward");
        }
        
        return total_reward / experiences.size();
    }
    
    void perform_experience_replay(const vector<map<string, double>>& experience_buffer, 
                                   double learning_rate) {
        // Randomly sample experiences for replay
        int sample_size = min(20, (int)experience_buffer.size() / 10);
        vector<map<string, double>> replay_batch;
        
        // Simple random sampling
        for (int i = 0; i < sample_size; ++i) {
            int index = rand() % experience_buffer.size();
            replay_batch.push_back(experience_buffer[index]);
        }
        
        // Update associative memory with replayed experiences
        for (const auto& experience : replay_batch) {
            this->associative_memory.associate(
                experience.at("percept"),
                experience.at("action"),
                experience.at("reward") * learning_rate
            );
        }
    }
    
    double calculate_episode_learning_efficiency(const map<string, double>& stats, size_t steps) {
        double reward_efficiency = stats.at("total_reward") / steps;
        double novelty_efficiency = stats.at("novelty_level");
        double curiosity_efficiency = stats.at("curiosity_level");
        
        return (reward_efficiency + novelty_efficiency + curiosity_efficiency) / 3.0;
    }
    
    void display_training_progress(int episode) {
        std::cout << "  Progress: " << episode << "/" << this->config.at("training").at("episodes") << 
                  " | Reward: " << this->reward_history.at(episode - 1) << 
                  " | Novelty: " << this->novelty_history.at(episode - 1) << 
                  " | Curiosity: " << this->agent.getCuriosityLevel() << std::endl;
    }
    
    map<string, py::object> analyze_memory_enhanced_performance() {
        map<string, py::object> analysis;
        
        // Calculate reward statistics
        double mean_reward = 0.0;
        double std_reward = 0.0;
        double max_reward = std::numeric_limits<double>::lowest();
        double min_reward = std::numeric_limits<double>::max();
        
        for (const auto& reward : this->reward_history) {
            mean_reward += reward;
            max_reward = std::max(max_reward, reward);
            min_reward = std::min(min_reward, reward);
        }
        
        mean_reward /= this->reward_history.size();
        for (const auto& reward : this->reward_history) {
            std_reward += (reward - mean_reward) * (reward - mean_reward);
        }
        std_reward = std::sqrt(std_reward / this->reward_history.size());
        
        analysis["mean_reward"] = mean_reward;
        analysis["reward_std"] = std_reward;
        analysis["max_reward"] = max_reward;
        analysis["min_reward"] = min_reward;
        
        // Calculate novelty statistics
        double mean_novelty = 0.0;
        for (const auto& novelty : this->novelty_history) {
            mean_novelty += novelty;
        }
        mean_novelty /= this->novelty_history.size();
        
        analysis["mean_novelty"] = mean_novelty;
        
        // Calculate learning efficiency
        double total_efficiency = 0.0;
        for (const auto& episode_stats : this->training_history) {
            total_efficiency += episode_stats.at("learning_efficiency");
        }
        
        analysis["mean_learning_efficiency"] = total_efficiency / this->training_history.size();
        
        return analysis;
    }
    
    void generate_comprehensive_plots() {
        int num_episodes = this->reward_history.size();
        
        // Create figure with multiple subplots
        plt.figure(figsize=(18, 12));
        
        // Plot 1: Reward history
        plt.subplot(3, 3, 1);
        plt.plot(this->reward_history.keys(), this->reward_history.values());
        plt.title('Reward History');
        plt.xlabel('Episode');
        plt.ylabel('Reward');
        plt.grid(true);
        
        // Plot 2: Novelty history
        plt.subplot(3, 3, 2);
        plt.plot(this->novelty_history.keys(), this->novelty_history.values());
        plt.title('Novelty History');
        plt.xlabel('Episode');
        plt.ylabel('Novelty');
        plt.grid(true);
        
        // Plot 3: Learning efficiency
        plt.subplot(3, 3, 3);
        vector<double> efficiency_values;
        for (const auto& episode_stats : this->training_history) {
            efficiency_values.push_back(episode_stats.at("learning_efficiency"));
        }
        plt.plot(efficiency_values);
        plt.title('Learning Efficiency');
        plt.xlabel('Episode');
        plt.ylabel('Efficiency');
        plt.grid(true);
        
        // Plot 4: Reward distribution
        plt.subplot(3, 3, 4);
        plt.hist(this->reward_history.values(), bins=20, alpha=0.7, density=true);
        plt.title('Reward Distribution');
        plt.xlabel('Reward');
        plt.ylabel('Probability Density');
        plt.grid(true);
        
        // Plot 5: Novelty distribution
        plt.subplot(3, 3, 5);
        plt.hist(this->novelty_history.values(), bins=20, alpha=0.7, density=true);
        plt.title('Novelty Distribution');
        plt.xlabel('Novelty');
        plt.ylabel('Probability Density');
        plt.grid(true);
        
        // Plot 6: Cumulative reward
        plt.subplot(3, 3, 6);
        vector<double> cumulative_rewards;
        double cumulative = 0.0;
        for (const auto& reward : this->reward_history.values()) {
            cumulative += reward;
            cumulative_rewards.push_back(cumulative);
        }
        plt.plot(cumulative_rewards);
        plt.title('Cumulative Reward');
        plt.xlabel('Episode');
        plt.ylabel('Cumulative Reward');
        plt.grid(true);
        
        // Plot 7: Memory usage
        plt.subplot(3, 3, 7);
        vector<double> working_memory_usage;
        vector<double> episodic_memory_usage;
        vector<double> associative_memory_usage;
        
        for (const auto& episode_stats : this->training_history) {
            working_memory_usage.push_back(this->working_memory.get_usage() * 100);
            episodic_memory_usage.push_back(this->episodic_memory.get_usage() * 100);
            associative_memory_usage.push_back(this->associative_memory.get_usage() * 100);
        }
        
        plt.plot(working_memory_usage, label='Working Memory');
        plt.plot(episodic_memory_usage, label='Episodic Memory');
        plt.plot(associative_memory_usage, label='Associative Memory');
        plt.title('Memory Usage');
        plt.xlabel('Episode');
        plt.ylabel('Memory Usage (%)');
        plt.legend();
        plt.grid(true);
        
        // Plot 8: Curiosity and novelty correlation
        plt.subplot(3, 3, 8);
        vector<double> curiosity_levels;
        for (const auto& episode_stats : this->training_history) {
            curiosity_levels.push_back(episode_stats.at("curiosity_level"));
        }
        plt.scatter(this->novelty_history.values(), curiosity_levels, alpha=0.6);
        plt.title('Curiosity vs Novelty');
        plt.xlabel('Novelty');
        plt.ylabel('Curiosity');
        plt.grid(true);
        
        // Plot 9: Learning rate decay
        plt.subplot(3, 3, 9);
        vector<double> learning_rates;
        double base_rate = this->config.at("training").at("learning_rate");
        for (int i = 0; i < num_episodes; ++i) {
            double decay_factor = this->config.at("training").at("discount_factor");
            learning_rates.push_back(base_rate * std::pow(decay_factor, i));
        }
        plt.plot(learning_rates);
        plt.title('Learning Rate Decay');
        plt.xlabel('Episode');
        plt.ylabel('Learning Rate');
        plt.grid(true);
        
        plt.tight_layout();
        std::string filename = "memory_enhanced_training_plots.png";
        plt.savefig(filename, dpi=150, bbox_inches='tight');
        plt.show();
        
        std::cout << "Comprehensive plots saved to " << filename << std::endl;
    }
    
    vector<double> convert_percept_to_features(const SensoryPercept& percept) {
        vector<double> features;
        
        // Vision features
        auto vision = percept.getVision();
        features.insert(features.end(), vision.begin(), vision.end());
        
        // Touch features
        auto touch = percept.getTouch();
        features.insert(features.end(), touch.begin(), touch.end());
        
        // Internal features
        auto internal = percept.getInternal();
        features.insert(features.end(), internal.begin(), internal.end());
        
        // Proprioception features
        auto proprioception = percept.getProprioception();
        features.insert(features.end(), proprioception.begin(), proprioception.end());
        
        // Audio features
        auto audio = percept.getAudio();
        features.insert(features.end(), audio.begin(), audio.end());
        
        // Normalize features
        double max_val = *std::max_element(features.begin(), features.end());
        double min_val = *std::min_element(features.begin(), features.end());
        
        if (max_val - min_val > 0) {
            for (double& feature : features) {
                feature = (feature - min_val) / (max_val - min_val);
            }
        }
        
        return features;
    }
    
    vector<double> action_to_features(const Action& action) {
        vector<double> features;
        
        // One-hot encoding of action type
        features.resize(13, 0.0); // 13 action types
        features[static_cast<int>(action.getType())] = 1.0;
        
        // Add action parameters
        auto params = action.getParameters();
        features.insert(features.end(), params.begin(), params.end());
        
        return features;
    }
    
    void apply_hebbian_learning(const vector<double>& input_features, 
                                const map<string, double>& target) {
        // Simplified Hebbian learning rule
        for (size_t i = 0; i < input_features.size() && i < target.size(); ++i) {
            double weight_change = input_features[i] * target.at(std::to_string(i));
            // Apply weight change to associative memory
        }
    }
    
    void update_internal_state_from_experience(const vector<double>& features,
                                               const map<string, double>& experience) {
        // Update internal state based on past experiences
        // This would involve updating neuromodulation levels, memory traces, etc.
    }
};

int main() {
    try {
        // Create memory-enhanced agent training
        MemoryEnhancedAgentTraining training;
        
        // Setup memory-enhanced agent
        training.setupMemoryEnhancedAgent();
        
        // Run memory-enhanced training
        auto results = training.runMemoryEnhancedTraining();
        
        // Display results
        auto analysis = std::get<3>(results);
        std::cout << "\n=== MEMORY-ENHANCED TRAINING RESULTS ===" << std::endl;
        std::cout << "Mean Reward: " << std::get<double>(analysis.at("mean_reward")) << std::endl;
        std::cout << "Reward Standard Deviation: " << std::get<double>(analysis.at("reward_std")) << std::endl;
        std::cout << "Mean Novelty: " << std::get<double>(analysis.at("mean_novelty")) << std::endl;
        std::cout << "Mean Learning Efficiency: " << std::get<double>(analysis.at("mean_learning_efficiency")) << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Advanced Features Documentation

### 1. Enhanced Error Handling

The enhanced NLM Python bindings provide comprehensive error handling with custom exception classes:

```python
class NLMError(Exception):
    """Base exception for all NLM errors."""

class ConfigurationError(NLMError):
    """Exception related to configuration errors."""

class BrainError(NLMError):
    """Exception related to brain simulation errors."""

class WorldError(NLMError):
    """Exception related to world simulation errors."""

class AgentError(NLMError):
    """Exception related to agent brain errors."""

class MemoryError(NLMError):
    """Exception related to memory system errors."""
```

### 2. Pythonic Convenience Methods

The enhanced bindings provide Pythonic convenience methods for common operations:

```python
# Configuration convenience methods
brain = nlm.Brain()
brain.config.set("neuron_count", 1000)
brain.config.set("connection_density", 0.1)
brain.config.set("plasticity_enabled", True)

# Brain convenience methods
brain = nlm.Brain()
brain.initialize()
brain.step(10)
brain.getTotalNeurons()  # Pythonic alias for getTotalNeuronCount()
brain.getFiringRate()    # Pythonic alias for getAverageFiringRate()
brain.run_simulation(100)  # Run complete simulation
brain.getStatistics()     # Get comprehensive brain statistics

# Agent convenience methods
agent = nlm.createAgentBrain(brain, world)
agent.enable_features(["reward_modulation", "curiosity"])
agent.getCurrentState()   # Get current agent state
agent.is_feature_enabled("reward_modulation")  # Check feature status

# World convenience methods
world = nlm.SimpleWorld(20, 20)
world.configure(30, 30, 12, 12)
world.update(0.1)
world.getReward()          # Get current reward
world.isEpisodeFinished()   # Check if episode is finished
```

### 3. Memory System Utilities

The enhanced bindings provide comprehensive memory system utilities:

```python
# Working memory
working_memory = nlm.create_working_memory(1000, 0.1)
working_memory.add_item("key", [1.0, 2.0, 3.0])
value = working_memory.get_item("key")

# Episodic memory
episodic_memory = nlm.create_episodic_memory(5000)
episodic_memory.record_episode(percept, action, reward, timestamp)
episodic_memory.replay_episode(episode_id)

# Associative memory
associative_memory = nlm.create_associative_memory(10000)
associative_memory.associate(input_features, output_features, strength)
relevant_experiences = associative_memory.search(input_features, top_k=10)
```

### 4. Neuromodulation Utilities

The enhanced bindings provide neuromodulation utilities:

```python
# Reward prediction error
rpe = nlm.calculate_reward_prediction_error(reward, predicted_reward)

# Curiosity calculation
curiosity = nlm.calculate_curiosity(novelty, prediction_error, base_curiosity=1.0)

# Novelty detection
novelty = nlm.calculate_novelty(current_percept, previous_percept)
```

### 5. Advanced Brain Analysis

The enhanced bindings provide advanced brain analysis tools:

```python
# Topology analysis
topology = nlm.analyze_brain_topology(brain)

# Connectivity calculation
connectivity = nlm.calculate_brain_connectivity(brain)

# Statistics
stats = brain.getStatistics()
```

### 6. Configuration Management

The enhanced bindings provide advanced configuration management:

```python
# Create configuration from dictionary
config_dict = {
    "neuron_count": 1000,
    "connection_density": 0.1,
    "plasticity_enabled": True,
    "development_enabled": True
}
config = nlm.create_config_from_dict(config_dict)

# Convert configuration to dictionary
config_dict = config.to_dict()

# Validate configuration
is_valid = config.validate()
```

### 7. Simulation Management

The enhanced bindings provide comprehensive simulation management:

```python
# Simple simulation
brain, world, agent = nlm.run_simple_simulation(100, verbose=True)

# Training episode
agent, world, total_reward = nlm.run_training_episode(agent, world, 200)

# Advanced simulation with callback
nlm.run_simulation_with_callback(brain, world, agent, 500, callback=my_callback)
```

## Installation and Usage

### System Dependencies

```bash
# Required dependencies
python3>=3.8
gcc>=7.0 (or other C++ compiler)

# Optional dependencies for visualization and experiments
pip install matplotlib pandas scipy numpy seaborn
```

### Installation from Source

```bash
# Clone the repository
# Ensure build directory exists
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python package
pip install -e .
```

### Quick Development Setup

```bash
# Clone and setup in one go
mkdir nlm_project
cd nlm_project
# Copy from source location
# Run build script
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e .[dev]
```

## Testing

### Running Tests

```bash
# Run all tests
pytest

# Run specific test category
pytest tests/test_basic_api.py
pytest tests/test_brain_simulation.py
pytest tests/test_agent_behavior.py

# Run with verbose output
pytest -v

# Run with coverage
pytest --cov=nlm --cov-report=html
```

### Test Categories

1. **Basic API Tests**: Core functionality verification
2. **Brain Simulation Tests**: Neural network simulation
3. **Agent Behavior Tests**: World interaction
4. **Memory System Tests**: Working and episodic memory
5. **Plasticity Tests**: Synaptic learning mechanisms

## Advanced Features Examples

### 1. Custom Brain Models

```python
import nlm

# Create custom brain with specific configuration
config = nlm.create_default_config()
config.set("neuron_count", 3000)
config.set("connection_density", 0.2)
config.set("plasticity_enabled", True)

brain = nlm.Brain(config)
brain.initialize()

# Add custom regions
brain.add_region("sensory_cortex", 500, nlm.NeuronType.Sensory)
brain.add_region("motor_cortex", 300, nlm.NeuronType.Motor)
```

### 2. Neural Network Analysis

```python
import nlm

# Analyze brain topology
topology = nlm.analyze_brain_topology(brain)

# Calculate connectivity
connectivity = nlm.calculate_brain_connectivity(brain)

# Visualize network
nlm.visualize_neural_network(brain, layout="spring")
```

### 3. Neuromodulation Studies

```python
import nlm

# Study reward prediction
rpe = nlm.calculate_reward_prediction_error(reward, predicted_reward)

# Study curiosity
curiosity = nlm.calculate_curiosity(novelty, prediction_error)

# Study novelty detection
novelty = nlm.calculate_novelty(percept1, percept2)
```

### 4. Developmental Dynamics

```python
import nlm

# Study developmental stages
brain = nlm.Brain()
brain.initialize()

for step in range(100):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage}")
```

## Performance Optimization

### Tips for Faster Simulations

1. **Enable optimization flags**
   ```python
   nlm.enable_optimization('simd')
   ```

2. **Use simulation pools**
   ```python
   sim_pool = nlm.SimulationPool(size=4)
   sim_pool.add_brain(brain)
   sim_pool.run_synchronously(steps=1000)
   ```

3. **Memory management**
   ```python
   memory_manager = nlm.MemoryManager(brain)
   memory_manager.enable_pool_allocation()
   ```

### Profiling

```python
import nlm

# Create performance profiler
profiler = nlm.PerformanceProfiler(brain)

# Profile different aspects
profiler.profile_neural_dynamics()
profiler.profile_plasticity()
profiler.profile_memory_systems()

# Compare configurations
comparison = nlm.PerformanceComparison()
comparison.add_configuration(brain, 'original')
comparison.add_configuration(brain_optimized, 'optimized')
comparison.compare_results()
```

## Future Improvements

### Planned Features

1. **Distributed Computing**: Parallel simulation across multiple machines
2. **GPU Acceleration**: CUDA support for faster neural computations
3. **Web Interface**: Real-time visualization through web browser
4. **Mobile Support**: Lightweight version for mobile devices
5. **Cloud Integration**: Easy deployment on cloud platforms
6. **Plugin System**: Extensible plugin architecture
7. **Advanced Analytics**: Machine learning for pattern detection

### Community Contributions

The library is designed for easy contribution:

- **New Brain Models**: Add custom neural dynamics
- **Visualization Plugins**: Create new visualization tools
- **Experiment Templates**: Share common experimental setups
- **Documentation**: Add examples and tutorials
- **Bug Fixes**: Report and fix issues
- **Performance Optimizations**: Improve simulation speed

## License and Credits

This is an experimental neural simulation framework. Please reference the original NLM documentation for more information about the underlying C++ implementation.

## Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Check this file and README.md for details
- **Examples**: Run the examples in the examples/ directory
- **Community**: Join the NLM community for discussions

That's it! You now have a comprehensive set of advanced examples demonstrating the full capabilities of the enhanced NLM Python interface. These examples showcase the advanced features, Pythonic APIs, and comprehensive functionality available in the enhanced bindings.