# Advanced NLM User Guide

**Expert-Level Usage and Advanced Features**

## Overview

This guide provides advanced usage patterns, expert commands, and complex scenarios for the NLM neural simulation framework. It targets users with prior experience who want to push the limits of the system, create custom experiments, and optimize performance.

## Advanced Command-Line Usage

### Complex Configuration Management

#### Multi-File Configuration
```bash
# Combine multiple configuration files
mkdir experiments/exp1
cd experiments/exp1

# Create base configuration
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
config.set('brain.neuron_count', 5000)
config.set('plasticity.stdp.learning_rate', 0.001)
config.set('neuromod.curiosity.enable', True)
config.saveToFile('base_config.cfg')
"

# Create environment-specific configurations
cat > environment_config.cfg << EOF
# Environment-specific settings
brain.synapse_density = 0.08
neuromod.dopamine.scale = 1.5
development.enable = True
development.initial_plasticity = 1.2
EOF

# Merge configurations for experiment
python3 << EOF
import pynlm
import json

# Load and merge configurations
base_config = pynlm.createDefaultConfig()
base_config.loadFromFile('base_config.cfg')

env_config = pynlm.createDefaultConfig()
env_config.loadFromFile('environment_config.cfg')

# Override with environment settings
for key in env_config.getKeys():
    if env_config.has(key):
        base_config.set(key, env_config.get(key), nlm.ConfigSource::Runtime)

# Save merged configuration
base_config.saveToFile('experiment_config.cfg')
print('Merged configuration saved')
EOF
```

#### Command-Line Argument Automation
```bash
#!/bin/bash
# Advanced command-line argument processor

EXPERIMENT_NAME="${1:-default_experiment}"
NEURON_COUNT="${2:-2000}"
LEARNING_RATE="${3:-0.001}"
STAGES="${4:-3}"

# Build command with dynamic parameters
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_EXPERIMENT_NAME="$EXPERIMENT_NAME" \
    -DNLM_NEURON_COUNT=$NEURON_COUNT \
    -DNLM_LEARNING_RATE=$LEARNING_RATE \
    -DNLM_DEVELOPMENT_STAGES=$STAGES

# Compile with custom optimizations
make -j$(nproc) OPTIMIZATION_FLAGS="-O3 -march=native -flto"

# Run with performance monitoring
/usr/bin/time -v ./nlm \
    --config=experiments/configs/$EXPERIMENT_NAME.cfg \
    --log_level=DEBUG \
    --performance_metrics

# Generate performance report
generate_performance_report.py \
    --input=experiments/results/$EXPERIMENT_NAME/performance.log \
    --output=reports/$EXPERIMENT_NAME/performance_analysis.html
```

### Advanced Brain Control Scripts

#### Brain State Manipulation
```python
import pynlm
import numpy as np
import json

class AdvancedBrainController:
    """Advanced brain controller with expert features."""
    
    def __init__(self, config_file=None):
        if config_file:
            self.config = pynlm.createDefaultConfig()
            self.config.loadFromFile(config_file)
        else:
            self.config = pynlm.createDefaultConfig()
        
        self.brain = None
        self.agent = None
        self.world = None
    
    def create_expert_brain(self, expertise_type='research'):
        """Create brain optimized for specific expertise."""
        configs = {
            'research': {
                'neuron_count': 10000,
                'plasticity.stdp.learning_rate': 0.0005,
                'plasticity.hebbian.learning_rate': 0.001,
                'development.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'prediction.enabled': True
            },
            'production': {
                'neuron_count': 2000,
                'plasticity.stdp.enable': True,
                'plasticity.structural.enable': False,
                'development.enable': False,
                'neuromod.curiosity.enable': False,
                'performance.mode': True
            },
            'exploration': {
                'neuron_count': 5000,
                'plasticity.stdp.learning_rate': 0.002,
                'plasticity.hebbian.learning_rate': 0.005,
                'development.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.5
            },
            'stable': {
                'neuron_count': 1000,
                'plasticity.stdp.learning_rate': 0.0001,
                'plasticity.hebbian.learning_rate': 0.0001,
                'development.enable': True,
                'development.plasticity_decay_rate': 0.99,
                'neuromod.curiosity.enable': False
            }
        }
        
        expert_config = configs.get(expertise_type, configs['research'])
        
        for key, value in expert_config.items():
            self.config.set(key, value, nlm.ConfigSource::Runtime)
        
        self._initialize_systems()
        return self.brain
    
    def _initialize_systems(self):
        """Initialize brain, agent, and world systems."""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=50, 
            height=50, 
            visionWidth=16, 
            visionHeight=16
        )
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all subsystems for advanced usage
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_advanced_simulation(self, duration_steps, strategy='balanced'):
        """Run advanced simulation with custom strategy."""
        simulation_data = {
            'steps': [],
            'rewards': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'development_stages': [],
            'performance_metrics': {}
        }
        
        # Configure strategy
        if strategy == 'aggressive':
            self._configure_aggressive_strategy()
        elif strategy == 'conservative':
            self._configure_conservative_strategy()
        elif strategy == 'curiosity_driven':
            self._configure_curiosity_driven_strategy()
        
        # Run simulation with data collection
        for step in range(duration_steps):
            # Advanced world update with environmental dynamics
            time_step = 0.1
            world_delta = self._compute_world_dynamics(step)
            self.world.update(time_step + world_delta)
            
            # Get advanced sensory processing
            percept = self.world.getSensoryPercept()
            advanced_percept = self._enhance_sensory_processing(percept, step)
            
            # Process with agent brain
            self.agent.processSensoryInput(advanced_percept)
            
            # Brain computation with advanced timing
            self.brain.step(step, step * 0.1)
            
            # Advanced action selection
            action = self._advanced_action_selection(step)
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Advanced reward computation
            reward = self._compute_advanced_reward(percept, action, step)
            self.agent.applyRewardModulation(reward, reward * 0.8)
            
            # Development update
            self.agent.updateDevelopment(time_step)
            
            # Collect advanced metrics
            simulation_data['steps'].append(step)
            simulation_data['rewards'].append(reward)
            simulation_data['firing_rates'].append(self.brain.getAverageFiringRate())
            simulation_data['curiosity_levels'].append(self.agent.getCuriosityLevel())
            simulation_data['novelty_levels'].append(self.agent.getNoveltyLevel())
            simulation_data['development_stages'].append(self.brain.getDevelopmentalStage())
        
        return simulation_data
    
    def _configure_aggressive_strategy(self):
        """Configure brain for aggressive learning."""
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        # High learning rates
        self.config.set('plasticity.stdp.learning_rate', 0.005, nlm.ConfigSource::Runtime)
        self.config.set('plasticity.hebbian.learning_rate', 0.01, nlm.ConfigSource::Runtime)
    
    def _configure_conservative_strategy(self):
        """Configure brain for conservative learning."""
        self.agent.enableCuriosity(False)
        self.agent.enableStructuralPlasticity(False)
        # Low learning rates for stability
        self.config.set('plasticity.stdp.learning_rate', 0.00005, nlm.ConfigSource::Runtime)
        self.config.set('plasticity.hebbian.learning_rate', 0.0001, nlm.ConfigSource::Runtime)
    
    def _configure_curiosity_driven_strategy(self):
        """Configure brain for curiosity-driven exploration."""
        self.agent.enableCuriosity(True)
        self.agent.enableNovelty(True)
        # Enhanced novelty detection
        self.config.set('neuromod.novelty.sensitivity', 0.8, nlm.ConfigSource::Runtime)
        self.config.set('neuromod.curiosity.base_rate', 0.02, nlm.ConfigSource::Runtime)
    
    def _compute_world_dynamics(self, step):
        """Compute complex world dynamics."""
        # Simulate environmental changes
        time_factor = (step % 100) / 100.0
        difficulty = 0.5 + 0.5 * np.sin(step * 0.1)
        resource_density = 0.3 + 0.2 * np.cos(step * 0.05)
        
        # Return time adjustment based on dynamics
        return 0.1 * (1.0 + time_factor * 0.2)
    
    def _enhance_sensory_processing(self, percept, step):
        """Apply advanced sensory processing enhancements."""
        # Simulate enhanced sensory processing
        enhanced = percept
        
        # Add temporal filtering
        if hasattr(percept, 'getTemporalSmoothed'):
            enhanced = percept.getTemporalSmoothed(0.5)
        
        # Add novelty weighting
        novelty_factor = 1.0 + self.agent.getNoveltyLevel() * 0.5
        if hasattr(enhanced, 'applyWeighting'):
            enhanced.applyWeighting(novelty_factor)
        
        return enhanced
    
    def _advanced_action_selection(self, step):
        """Advanced action selection using neural activity patterns."""
        # Analyze neural activity patterns for decision making
        firing_pattern = self._extract_firing_pattern()
        attention_weights = self._analyze_attention_weights()
        
        # Advanced action selection algorithm
        action_scores = self._compute_action_scores(firing_pattern, attention_weights)
        selected_action = self._select_best_action(action_scores)
        
        return selected_action
    
    def _extract_firing_pattern(self):
        """Extract complex firing patterns from brain activity."""
        pattern = {}
        
        # Get activity from different neuron populations
        for region_id in range(self.brain.getRegionCount()):
            region = self.brain.getRegion(nlm::RegionId(region_id))
            if region:
                neurons = region->getAllNeurons();
                if neurons.size() > 0:
                    firing_rates = [];
                    for neuron : neurons {
                        firing_rates.append(neuron->getFiringRate());
                    }
                    
                    pattern[region_id] = {
                        'mean_rate': np.mean(firing_rates),
                        'std_rate': np.std(firing_rates),
                        'max_rate': np.max(firing_rates),
                        'min_rate': np.min(firing_rates)
                    }
        
        return pattern
    
    def _analyze_attention_weights(self):
        """Analyze attention system weights."""
        attention_weights = {}
        
        auto* attention_system = self.brain.getAttention();
        if (attention_system) {
            attention_weights = attention_system->getAttentionWeights();
        }
        
        return attention_weights
    
    def _compute_action_scores(self, firing_pattern, attention_weights):
        """Compute scores for different actions based on neural patterns."""
        action_scores = {};
        
        # Get all available actions
        actions = list(pynlm.ActionType)
        
        for action in actions {
            score = 0.0;
            
            # Score based on firing patterns
            for region_id, pattern in firing_pattern.items() {
                region_score = pattern['mean_rate'] * pattern['std_rate'];
                score += region_score * 0.1;
            }
            
            # Score based on attention weights
            for area, weight in attention_weights.items() {
                score += weight * 0.3;
            }
            
            # Apply strategy modifiers
            score *= self._get_strategy_modifier(action);
            
            action_scores[action] = score;
        }
        
        return action_scores
    
    def _select_best_action(self, action_scores):
        """Select best action based on computed scores."n        best_action = None;
        best_score = -999999.0;
        
        for action, score in action_scores.items() {
            if score > best_score {
                best_score = score;
                best_action = action;
            }
        }
        
        return best_action;
    
    def _get_strategy_modifier(self, action):
        """Get strategy modifier for action."""
        return 1.0; // Override in derived classes for specific strategies
    
    def _compute_advanced_reward(self, percept, action, step):
        """Compute advanced reward with multiple factors."""
        base_reward = 1.0;
        
        // Add novelty bonus
        novelty_bonus = self.agent.getNoveltyLevel() * 0.5;
        
        // Add curiosity bonus
        curiosity_bonus = self.agent.getCuriosityLevel() * 0.3;
        
        // Add developmental bonus
        dev_stage = self.brain.getDevelopmentalStage();
        development_bonus = (float(dev_stage) / 5.0) * 0.2;
        
        // Add action-specific bonus
        action_bonus = self._get_action_bonus(action);
        
        total_reward = base_reward + novelty_bonus + curiosity_bonus + development_bonus + action_bonus;
        
        return total_reward;
    
    def _get_action_bonus(self, action):
        """Get bonus for specific action."""
        bonuses = {
            pynlm.ActionType.MoveForward: 0.1,
            pynlm.ActionType.TurnLeft: 0.05,
            pynlm.ActionType.TurnRight: 0.05,
            pynlm.ActionType.Interact: 0.15,
            pynlm.ActionType.Eat: 0.2
        };
        
        return bonuses.get(action, 0.0);
    
    def save_simulation_data(self, data, filename):
        """Save simulation data to file."""
        import json
        
        // Convert data to JSON-serializable format
        json_data = {
            'steps': data['steps'],
            'rewards': data['rewards'],
            'firing_rates': data['firing_rates'],
            'curiosity_levels': data['curiosity_levels'],
            'novelty_levels': data['novelty_levels'],
            'development_stages': [str(stage) for stage in data['development_stages']]
        };
        
        with open(filename, 'w') as f {
            json.dump(json_data, f, indent=2);
        }
        
        print(f"Simulation data saved to {filename}");
    
    def analyze_simulation_results(self, data):
        """Analyze and print simulation results."""
        if not data['rewards']:
            print("No simulation data to analyze");
            return;
        
        print("Simulation Analysis Results:", file=sys.stderr);
        print("======================", file=sys.stderr);
        
        // Performance metrics
        avg_reward = sum(data['rewards']) / len(data['rewards']);
        max_reward = max(data['rewards']);
        min_reward = min(data['rewards']);
        reward_std = np.std(data['rewards']);
        
        print(f"Average Reward: {avg_reward:.3f}", file=sys.stderr);
        print(f"Max Reward: {max_reward:.3f}", file=sys.stderr);
        print(f"Min Reward: {min_reward:.3f}", file=sys.stderr);
        print(f"Reward Volatility (std): {reward_std:.3f}", file=sys.stderr);
        
        // Neural activity metrics
        avg_firing_rate = sum(data['firing_rates']) / len(data['firing_rates']);
        firing_rate_std = np.std(data['firing_rates']);
        
        print(f"Average Firing Rate: {avg_firing_rate:.3f} Hz", file=sys.stderr);
        print(f"Firing Rate Volatility: {firing_rate_std:.3f} Hz", file=sys.stderr);
        
        // Exploration metrics
        avg_curiosity = sum(data['curiosity_levels']) / len(data['curiosity_levels']);
        avg_novelty = sum(data['novelty_levels']) / len(data['novelty_levels']);
        
        print(f"Average Curiosity: {avg_curiosity:.3f}", file=sys.stderr);
        print(f"Average Novelty: {avg_novelty:.3f}", file=sys.stderr);
        
        // Development metrics
        final_stage = data['development_stages'][-1];
        print(f"Final Development Stage: {final_stage}", file=sys.stderr);
        
        // Success criteria
        success_criteria = {
            'learning_detected': reward_std > 0.1,
            'exploration_present': avg_curiosity > 0.3,
            'stability_good': firing_rate_std < avg_firing_rate * 0.5,
            'progress_made': final_stage != nlm.DevelopmentalStage::Initial
        };
        
        print("\nSuccess Criteria:", file=sys.stderr);
        for criterion, met in success_criteria.items() {
            status = "✓" if met else "✗";
            print(f"  {status} {criterion}", file=sys.stderr);
        }
        
        overall_success = all(success_criteria.values());
        print(f"\nOverall Success: {'YES' if overall_success else 'NO'}", file=sys.stderr);
    
    def create_research_protocol(self, protocol_name, parameters):
        """Create research protocol for reproducibility."""
        protocol = {
            'protocol_name': protocol_name,
            'timestamp': datetime.now().isoformat(),
            'parameters': parameters,
            'config': self.config,
            'simulation_results': {}
        };
        
        // Save protocol
        with open(f"protocols/{protocol_name}.json", 'w') as f {
            json.dump(protocol, f, indent=2, default=str);
        }
        
        print(f"Research protocol saved: protocols/{protocol_name}.json");
        return protocol;
}

// Usage examples
if __name__ == "__main__":
    print("=== Advanced NLM User Examples ===");
    print("");
    
    print("Example 1: Creating expert brain for research");
    print("```python");
    print("controller = AdvancedBrainController('base_config.cfg');");
    print("brain = controller.create_expert_brain('research');");
    print("```");
    print("");
    
    print("Example 2: Running aggressive simulation");
    print("```python");
    print("controller = AdvancedBrainController();");
    print("brain = controller.create_expert_brain('exploration');");
    print("data = controller.run_advanced_simulation(1000, 'aggressive');");
    print("controller.analyze_simulation_results(data);");
    print("```");
    print("");
    
    print("Example 3: Research protocol creation");
    print("```python");
    print("protocol_params = {");
    print("    'neuron_count': 5000,");
    print("    'learning_rate': 0.001,");
    print("    'plasticity_enabled': True,");
    print("    'development_stages': 3");
    print("};");
    print("controller = AdvancedBrainController();");
    print("protocol = controller.create_research_protocol('experiment_1', protocol_params);");
    print("```");
    print("");
    
    print("Example 4: Configuration management");
    print("```bash");
    print("# Create expert configuration for different scenarios");
    print("generate_expert_config.py --type research > research_config.cfg");
    print("generate_expert_config.py --type production > production_config.cfg");
    print("generate_expert_config.py --type exploration > exploration_config.cfg");
    print("```");
    print("");
    
    print("=== End of Advanced User Guide ===");
    print("");
    print("For more advanced examples, see:");
    print("- python/README.md for Python advanced features");
    print("- docs/ARCHITECTURE.md for system architecture");
    print("- docs/SCIENCE.md for scientific background");
    print("- docs/EXPERIMENTS.md for experiment examples");
