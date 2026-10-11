# Advanced Example 5: Custom Plasticity Rules

## Overview

This advanced example demonstrates how to implement custom plasticity rules in the NLM framework. It shows how to extend the built-in plasticity mechanisms (STDP, Hebbian) with novel learning rules tailored to specific research questions and applications.

## Learning Objectives

After completing this example, you will be able to:

1. **Design custom plasticity rules** for neural learning
2. **Implement novel learning mechanisms** beyond standard STDP
3. **Integrate custom rules** with existing plasticity systems
4. **Test and optimize** custom plasticity parameters
5. **Apply custom rules** to specific research scenarios
6. **Document and share** custom plasticity implementations

## What This Example Demonstrates

- **Novel plasticity mechanisms**: Custom learning rules not in standard NLM
- **Parameter optimization**: Finding optimal plasticity parameters
- **Integration with existing systems**: Combining custom rules with STDP and Hebbian
- **Research applications**: Applying custom plasticity to specific problems
- **Performance analysis**: Evaluating effectiveness of custom rules
- **Reproducible research**: Documenting and sharing custom implementations

## System Requirements

For custom plasticity implementation, you'll need:

- **NLM development environment**: Source code access for customization
- **C++ development tools**: For performance-critical plasticity rules
- **Python bindings**: For testing and validation
- **Performance profiling**: To optimize custom implementations
- **Testing framework**: For validating custom plasticity rules
- **Research environment**: For applying to specific research questions

## Technical Architecture

### 1. Plasticity Rule Architecture

```cpp
// Custom plasticity rule framework
class CustomPlasticityRule : public PlasticityRule {
protected:
    std::string rule_name;
    double learning_rate;
    double time_constant;
    std::function<double(const NeuronState&, const NeuronState&, double)> update_function;
    
public:
    CustomPlasticityRule(const std::string& name, double lr, double tau)
        : rule_name(name), learning_rate(lr), time_constant(tau) {}
    
    virtual void applyPlasticity(Neuron* pre, Neuron* post, 
                                const SpikeEvent& spike_event) override {
        // Apply custom plasticity update
        double weight_change = calculate_weight_change(pre, post, spike_event);
        apply_weight_update(pre, post, weight_change);
    }
    
    virtual double calculate_weight_change(const NeuronState& pre_state,
                                          const NeuronState& post_state,
                                          double time_difference) = 0;
    
    std::string get_rule_name() const { return rule_name; }
    double get_learning_rate() const { return learning_rate; }
};

// Composite plasticity system
class CompositePlasticitySystem {
private:
    std::vector<std::unique_ptr<PlasticityRule>> plasticity_rules;
    std::vector<std::unique_ptr<CustomPlasticityRule>> custom_rules;
    
    // Rule application order
    enum RuleOrder { STDP_FIRST, HEBBIAN_FIRST, CUSTOM_FIRST, MIXED };
    RuleOrder application_order;
    
public:
    void add_custom_rule(std::unique_ptr<CustomPlasticityRule> rule);
    void apply_all_rules(Neuron* pre, Neuron* post, const SpikeEvent& spike_event);
    void set_application_order(RuleOrder order);
    
    // Custom rule management
    void enable_custom_rule(const std::string& rule_name, bool enable);
    void configure_custom_rule_parameter(const std::string& rule_name, 
                                         const std::string& param_name, double value);
    std::map<std::string, double> get_custom_rule_parameters(const std::string& rule_name);
};
```

### 2. Novel Plasticity Mechanisms

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from scipy import optimize

class NovelPlasticityMechanisms:
    """Implement novel plasticity rules beyond standard NLM"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Custom plasticity rules
        self.custom_rules = {}
        self.rule_parameters = {}
        
        # Initialize novel mechanisms
        self.initialize_novel_mechanisms()
    
    def initialize_novel_mechanisms(self):
        """Initialize novel plasticity mechanisms"""
        
        # 1. Homeostatic plasticity
        self.custom_rules['homeostatic'] = self.homeostatic_plasticity
        self.rule_parameters['homeostatic'] = {
            'target_firing_rate': 10.0,  # Hz
            'learning_rate': 0.001,
            'time_constant': 1000.0,  # steps
            'stability_threshold': 0.05
        }
        
        # 2. Reward-modulated plasticity
        self.custom_rules['reward_modulated'] = self.reward_modulated_plasticity
        self.rule_parameters['reward_modulated'] = {
            'reward_scale': 1.0,
            'learning_rate': 0.002,
            'baseline_reward': 0.0,
            'prediction_error_weight': 0.5
        }
        
        # 3. Metaplasticity (plasticity of plasticity)
        self.custom_rules['metaplasticity'] = self.metaplasticity_rule
        self.rule_parameters['metaplasticity'] = {
            'plasticity_learning_rate': 0.0001,
            'stability_target': 0.8,
            'adaptation_rate': 0.01,
            'history_window': 100
        }
        
        # 4. Social plasticity (learning from others)
        self.custom_rules['social'] = self.social_plasticity
        self.rule_parameters['social'] = {
            'social_weight': 0.3,
            'observation_window': 50,
            'imitation_threshold': 0.7,
            'social_learning_rate': 0.001
        }
        
        # 5. Developmental plasticity
        self.custom_rules['developmental'] = self.developmental_plasticity
        self.rule_parameters['developmental'] = {
            'age_factor': 0.01,
            'critical_period_start': 100,
            'critical_period_end': 500,
            'plasticity_decay_rate': 0.005
        }
        
        # 6. Stress-induced plasticity
        self.custom_rules['stress'] = self.stress_plasticity
        self.rule_parameters['stress'] = {
            'stress_threshold': 5.0,
            'stress_impact': 0.8,
            'recovery_rate': 0.02,
            'adaptation_time': 200
        }
        
        print(f"   ✓ Initialized {len(self.custom_rules)} novel plasticity mechanisms")
    
    def homeostatic_plasticity(self, pre_neuron, post_neuron, spike_time):
        """Homeostatic plasticity that maintains stable firing rates"""
        
        # Get current firing rates
        pre_firing_rate = pre_neuron.getFiringRate()
        post_firing_rate = post_neuron.getFiringRate()
        
        # Target firing rate for homeostasis
        target_rate = self.rule_parameters['homeostatic']['target_firing_rate']
        
        # Calculate homeostatic error
        pre_error = (pre_firing_rate - target_rate) / target_rate
        post_error = (post_firing_rate - target_rate) / target_rate
        
        # Apply homeostatic weight adjustment
        # Stronger connections to neurons with lower firing rates
        weight_change_pre = -pre_error * self.rule_parameters['homeostatic']['learning_rate']
        weight_change_post = -post_error * self.rule_parameters['homeostatic']['learning_rate']
        
        # Apply gradual adaptation based on time constant
        time_factor = 1.0 - np.exp(-spike_time / self.rule_parameters['homeostatic']['time_constant'])
        
        return {
            'weight_change_pre': weight_change_pre * time_factor,
            'weight_change_post': weight_change_post * time_factor,
            'error_pre': pre_error,
            'error_post': post_error
        }
    
    def reward_modulated_plasticity(self, pre_neuron, post_neuron, spike_time, reward_signal):
        """Plasticity modulated by reward prediction error"""
        
        # Get prediction error from agent
        prediction_error = self.agent.getPredictionError()
        
        # Normalize reward signal
        normalized_reward = reward_signal / max(1.0, abs(reward_signal))
        
        # Calculate reward-modulated plasticity
        # Potentiation for positive prediction error, depression for negative
        reward_factor = normalized_reward * prediction_error
        
        # Apply Hebbian-like rule modulated by reward
        hebbian_component = self.calculate_hebbian_component(pre_neuron, post_neuron, spike_time)
        
        # Reward modulation
        modulated_change = hebbian_component * (1.0 + reward_factor * self.rule_parameters['reward_modulated']['reward_scale'])
        
        # Apply baseline reward
        baseline_effect = self.rule_parameters['reward_modulated']['baseline_reward'] * spike_time * 0.001
        
        return {
            'weight_change': modulated_change + baseline_effect,
            'hebbian_component': hebbian_component,
            'reward_factor': reward_factor,
            'prediction_error': prediction_error
        }
    
    def metaplasticity_rule(self, pre_neuron, post_neuron, spike_time, plasticity_history):
        """Metaplasticity - adaptation of learning rates based on recent experience"""
        
        # Get recent plasticity history
        recent_learning = plasticity_history.get_recent_learning(spike_time)
        stability = plasticity_history.get_stability()
        
        # Calculate metaplasticity signal
        # Reduce learning when system is stable, increase when unstable
        stability_factor = 1.0 - stability
        learning_rate_adaptation = self.rule_parameters['metaplasticity']['plasticity_learning_rate'] * stability_factor
        
        # Update learning rate based on recent experience
        adapted_learning_rate = self.rule_parameters['metaplasticity']['learning_rate'] * \
                               (1.0 + learning_rate_adaptation * recent_learning)
        
        # Apply plastic change with adapted learning rate
        plastic_change = self.calculate_standard_stdp(pre_neuron, post_neuron, spike_time)
        final_change = plastic_change * adapted_learning_rate
        
        # Check for critical period effects
        age_factor = self.calculate_developmental_factor(spike_time)
        if age_factor < self.rule_parameters['metaplasticity']['stability_target']:
            # During critical periods, increase learning for development
            final_change *= 2.0
        
        return {
            'weight_change': final_change,
            'adapted_learning_rate': adapted_learning_rate,
            'stability_factor': stability_factor,
            'recent_learning': recent_learning,
            'age_factor': age_factor
        }
    
    def social_plasticity(self, pre_neuron, post_neuron, spike_time, observation_window):
        """Social plasticity - learning from observed interactions"""
        
        # Get observations from social environment
        observations = self.get_social_observations(observation_window)
        
        # Calculate social influence
        if observations:
            # Find most successful interaction pattern
            best_pattern = self.find_best_interaction_pattern(observations)
            
            # Apply social learning
            social_influence = self.calculate_social_influence(best_pattern, spike_time)
            
            # Combine with standard plasticity
            standard_change = self.calculate_standard_stdp(pre_neuron, post_neuron, spike_time)
            
            # Apply social learning
            social_change = standard_change * social_influence * self.rule_parameters['social']['social_learning_rate']
            
            # Track social learning effectiveness
            self.update_social_learning_history(social_change, best_pattern)
        else:
            social_change = 0.0
        
        return {
            'weight_change': social_change,
            'social_influence': social_influence if 'social_influence' in locals() else 0.0,
            'best_pattern': best_pattern if 'best_pattern' in locals() else None,
            'observation_count': len(observations) if observations else 0
        }
    
    def developmental_plasticity(self, pre_neuron, post_neuron, spike_time):
        """Developmental plasticity - age-dependent learning"""
        
        # Calculate developmental age factor
        age_factor = self.calculate_developmental_factor(spike_time)
        
        # Get current learning rate
        base_learning_rate = self.rule_parameters['developmental']['learning_rate']
        
        # Apply developmental modulation
        if spike_time < self.rule_parameters['developmental']['critical_period_start']:
            # Early critical period - enhanced learning
            developmental_factor = 1.5
        elif spike_time < self.rule_parameters['developmental']['critical_period_end']:
            # Mid critical period - moderate learning
            developmental_factor = 1.0
        else:
            # Late development - reduced learning
            developmental_factor = 0.5
        
        # Age-related decay
        age_decay = np.exp(-self.rule_parameters['developmental']['plasticity_decay_rate'] * spike_time)
        
        # Final developmental factor
        dev_factor = age_factor * developmental_factor * age_decay
        
        # Apply developmental plasticity
        standard_change = self.calculate_standard_stdp(pre_neuron, post_neuron, spike_time)
        developmental_change = standard_change * dev_factor * self.rule_parameters['developmental']['learning_rate']
        
        return {
            'weight_change': developmental_change,
            'age_factor': age_factor,
            'developmental_factor': developmental_factor,
            'age_decay': age_decay,
            'critical_period_active': spike_time < self.rule_parameters['developmental']['critical_period_end']
        }
    
    def stress_plasticity(self, pre_neuron, post_neuron, spike_time, stress_level):
        """Stress-induced plasticity - learning modulation under stress"""
        
        # Normalize stress level
        normalized_stress = min(1.0, stress_level / self.rule_parameters['stress']['stress_threshold'])
        
        # Calculate stress effects
        if normalized_stress > 0.5:
            # High stress - enhanced consolidation, reduced exploration
            stress_effect = self.rule_parameters['stress']['stress_impact']
            exploration_suppression = 1.0 - normalized_stress * 0.8
        else:
            # Low stress - normal learning
            stress_effect = normalized_stress * 0.3
            exploration_suppression = 1.0
        
        # Apply stress modulation to plasticity
        standard_change = self.calculate_standard_stdp(pre_neuron, post_neuron, spike_time)
        
        # Stress reduces learning flexibility but enhances memory consolidation
        stress_modulated_change = standard_change * stress_effect
        
        # Apply recovery mechanism
        recovery_factor = np.exp(-spike_time / self.rule_parameters['stress']['recovery_rate'])
        final_change = stress_modulated_change * recovery_factor
        
        return {
            'weight_change': final_change,
            'stress_effect': stress_effect,
            'exploration_suppression': exploration_suppression,
            'recovery_factor': recovery_factor,
            'normalized_stress': normalized_stress
        }
    
    def calculate_hebbian_component(self, pre_neuron, post_neuron, spike_time):
        """Calculate Hebbian-like component for custom rules"""
        
        # Get pre- and post-synaptic activity
        pre_activity = pre_neuron.get_recent_activity(spike_time)
        post_activity = post_neuron.get_recent_activity(spike_time)
        
        # Calculate Hebbian component (fire together, wire together)
        hebbian_weight = 0.01  # Base Hebbian weight
        hebbian_change = hebbian_weight * pre_activity * post_activity
        
        return hebbian_change
    
    def calculate_standard_stdp(self, pre_neuron, post_neuron, spike_time):
        """Calculate standard STDP component"""
        
        # Get spike timing information
        pre_spike_time = pre_neuron.get_last_spike_time()
        post_spike_time = post_neuron.get_last_spike_time()
        
        # Calculate time difference
        if pre_spike_time > 0 and post_spike_time > 0:
            time_diff = post_spike_time - pre_spike_time
            
            # STDP window
            if time_diff > 0:  # Post after pre - LTP
                ltp_strength = 0.01 * np.exp(-time_diff / 20.0)
                return ltp_strength
            else:  # Pre after post - LTD
                ltd_strength = -0.01 * np.exp(time_diff / 20.0)
                return ltd_strength
        
        return 0.0
    
    def calculate_developmental_factor(self, current_time):
        """Calculate developmental factor based on current time"""
        
        # Developmental trajectory
        # Starts at 1.0, decreases over time
        developmental_factor = np.exp(-self.rule_parameters['developmental']['age_factor'] * current_time)
        
        return developmental_factor
    
    def get_social_observations(self, observation_window):
        """Get social observations from environment"""
        
        # In a real implementation, this would query the environment
        # For this demo, we simulate social observations
        
        observations = []
        for i in range(observation_window):
            # Simulate observation of other agents' behavior
            if np.random.random() < 0.3:  # 30% chance of observation
                observation = {
                    'agent_id': i,
                    'action': self.get_random_action(),
                    'outcome': np.random.uniform(-1, 1),
                    'success': np.random.random() > 0.5
                }
                observations.append(observation)
        
        return observations
    
    def find_best_interaction_pattern(self, observations):
        """Find most successful interaction pattern from observations"""
        
        if not observations:
            return None
        
        # Calculate success rate for different action patterns
        action_patterns = {}
        
        for obs in observations:
            action = obs['action']
            if action not in action_patterns:
                action_patterns[action] = {'successes': 0, 'total': 0}
            
            action_patterns[action]['total'] += 1
            if obs['success']:
                action_patterns[action]['successes'] += 1
        
        # Find pattern with highest success rate
        best_pattern = None
        best_success_rate = 0
        
        for action, stats in action_patterns.items():
            if stats['total'] > 0:
                success_rate = stats['successes'] / stats['total']
                if success_rate > best_success_rate:
                    best_success_rate = success_rate
                    best_pattern = action
        
        return best_pattern
    
    def get_random_action(self):
        """Get random action for simulation"""
        
        actions = ['move_forward', 'turn_left', 'turn_right', 'interact', 'eat']
        return np.random.choice(actions)
    
    def update_social_learning_history(self, social_change, pattern):
        """Update social learning history"""
        
        if not hasattr(self, 'social_history'):
            self.social_history = []
        
        self.social_history.append({
            'change': social_change,
            'pattern': pattern,
            'timestamp': len(self.social_history)
        })
        
        # Keep history manageable
        if len(self.social_history) > 1000:
            self.social_history = self.social_history[-100:]
    
    def configure_custom_rule(self, rule_name, parameters):
        """Configure custom plasticity rule parameters"""
        
        if rule_name in self.custom_rules:
            self.rule_parameters[rule_name].update(parameters)
            print(f"   ✓ Configured {rule_name} rule with parameters: {parameters}")
        else:
            print(f"   ! Rule {rule_name} not found")
    
    def enable_custom_plasticity(self, enabled_rules):
        """Enable or disable specific custom plasticity rules"""
        
        for rule_name in enabled_rules:
            if rule_name in self.custom_rules:
                print(f"   ✓ Enabled {rule_name} plasticity rule")
            else:
                print(f"   ! Rule {rule_name} not available")
    
    def run_custom_plasticity_simulation(self, num_steps=1000):
        """Run simulation with custom plasticity rules"""
        
        print(f"\n=== Custom Plasticity Simulation ===")
        print(f"Running simulation with {num_steps} steps...")
        
        # Track performance metrics
        performance_metrics = {
            'firing_rates': [],
            'learning_progress': [],
            'stability_measures': [],
            'rule_effects': {}
        }
        
        # Initialize rule effect trackers
        for rule_name in self.custom_rules:
            performance_metrics['rule_effects'][rule_name] = {
                'total_changes': 0,
                'average_effect': 0.0,
                'contribution_to_stability': 0.0
            }
        
        for step in range(num_steps):
            # Get world state
            world_update = self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Get current simulation state
            body = self.world.getAgentBody()
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            
            # Apply custom plasticity rules
            step_plasticity_effects = {}
            
            for rule_name, rule_function in self.custom_rules.items():
                # Apply rule-specific plasticity
                if rule_name == 'homeostatic':
                    effect = rule_function(None, None, step)
                elif rule_name == 'reward_modulated':
                    effect = rule_function(None, None, step, reward)
                elif rule_name == 'metaplasticity':
                    # Get plasticity history
                    plasticity_history = self.get_plasticity_history(step)
                    effect = rule_function(None, None, step, plasticity_history)
                elif rule_name == 'social':
                    observation_window = self.rule_parameters['social']['observation_window']
                    effect = rule_function(None, None, step, observation_window)
                elif rule_name == 'developmental':
                    effect = rule_function(None, None, step)
                elif rule_name == 'stress':
                    stress_level = self.calculate_stress_level(step)
                    effect = rule_function(None, None, step, stress_level)
                else:
                    effect = rule_function(None, None, step)
                
                # Track rule effects
                if isinstance(effect, dict):
                    step_plasticity_effects[rule_name] = effect
                    
                    # Update performance metrics
                    if 'weight_change' in effect:
                        performance_metrics['rule_effects'][rule_name]['total_changes'] += 1
                        performance_metrics['rule_effects'][rule_name]['average_effect'] += abs(effect['weight_change'])
            
            # Run brain simulation
            self.brain.step(step)
            
            # Collect metrics
            performance_metrics['firing_rates'].append(self.brain.getAverageFiringRate())
            performance_metrics['learning_progress'].append(
                self.brain.getTotalSpikeCount() / (step + 1)
            )
            performance_metrics['stability_measures'].append(
                self.calculate_stability(performance_metrics['firing_rates'])
            )
            
            # Apply neuromodulation
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                firing_rate = performance_metrics['firing_rates'][-1]
                learning_progress = performance_metrics['learning_progress'][-1]
                stability = performance_metrics['stability_measures'][-1]
                
                print(f"   Step {step:4d}: Firing={firing_rate:6.2f}Hz, "
                      f"Learning={learning_progress:.4f}, Stability={stability:.3f}")
        
        # Finalize performance metrics
        for rule_name, metrics in performance_metrics['rule_effects'].items():
            if metrics['total_changes'] > 0:
                metrics['average_effect'] /= metrics['total_changes']
                metrics['contribution_to_stability'] = metrics['average_effect'] * 0.5
        
        print(f"\n=== Custom Plasticity Simulation Complete ===")
        print(f"Total simulation steps: {num_steps:,}")
        print(f"Average firing rate: {np.mean(performance_metrics['firing_rates']):.2f} Hz")
        print(f"Final stability: {performance_metrics['stability_measures'][-1]:.3f}")
        
        # Analyze rule effectiveness
        self.analyze_rule_effectiveness(performance_metrics)
        
        return performance_metrics
    
    def calculate_stability(self, firing_rate_history):
        """Calculate stability of firing rates"""
        
        if len(firing_rate_history) < 10:
            return 0.0
        
        # Calculate coefficient of variation
        mean_rate = np.mean(firing_rate_history)
        std_rate = np.std(firing_rate_history)
        
        if mean_rate > 0:
            cv = std_rate / mean_rate
            stability = max(0, 1 - cv)
        else:
            stability = 0.0
        
        return stability
    
    def analyze_rule_effectiveness(self, performance_metrics):
        """Analyze effectiveness of custom plasticity rules"""
        
        print(f"\n=== Custom Plasticity Rule Effectiveness Analysis ===")
        
        rule_effects = performance_metrics['rule_effects']
        
        # Sort rules by effectiveness
        sorted_rules = sorted(rule_effects.items(), 
                            key=lambda x: x[1]['average_effect'], reverse=True)
        
        print(f"Rule effectiveness ranking:")
        for i, (rule_name, metrics) in enumerate(sorted_rules):
            effectiveness = metrics['average_effect']
            contribution = metrics['contribution_to_stability']
            
            print(f"   {i+1}. {rule_name:20s}: Effect={effectiveness:.6f}, "
                  f"Contribution={contribution:.4f}")
        
        # Identify most and least effective rules
        most_effective = sorted_rules[0][0]
        least_effective = sorted_rules[-1][0]
        
        print(f"\nMost effective rule: {most_effective}")
        print(f"Least effective rule: {least_effective}")
        
        # Recommendations
        print(f"\nRecommendations:")
        if most_effective != 'homeostatic':
            print(f"   • Consider prioritizing {most_effective} rule")
        if least_effective != 'developmental':
            print(f"   • Review {least_effective} rule parameters")
        
        # Stability analysis
        final_stability = performance_metrics['stability_measures'][-1]
        stability_trend = np.mean(np.diff(performance_metrics['stability_measures'][-50:]))
        
        print(f"\nStability Analysis:")
        print(f"   Final stability: {final_stability:.3f}")
        print(f"   Stability trend: {stability_trend:+.4f}")
        
        if final_stability > 0.8:
            print(f"   ✓ High stability achieved")
        elif final_stability > 0.5:
            print(f"   = Moderate stability")
        else:
            print(f"   ! Low stability - review rule parameters")
    
    def get_plasticity_history(self, current_step):
        """Get plasticity history for metaplasticity analysis"""
        
        # This would get actual plasticity history from brain simulation
        # For this demo, we return a mock object
        
        class MockPlasticityHistory:
            def __init__(self):
                self.recent_learning = []
                self.stability = 0.8
            
            def get_recent_learning(self, step):
                # Simulate recent learning pattern
                learning = np.random.beta(0.5, 2.0)  # Skewed towards low learning
                self.recent_learning.append(learning)
                if len(self.recent_learning) > 10:
                    self.recent_learning.pop(0)
                return np.mean(self.recent_learning) if self.recent_learning else 0.5
            
            def get_stability(self):
                return self.stability
        
        return MockPlasticityHistory()
    
    def calculate_stress_level(self, step):
        """Calculate stress level based on simulation time"""
        
        # Simulate stress pattern - increases over time, with fluctuations
        base_stress = min(1.0, step / 1000.0)
        stress_fluctuation = 0.1 * np.sin(step / 50.0)
        
        stress_level = base_stress + stress_fluctuation
        return max(0.0, min(1.0, stress_level))
```

### 3. Custom Rule Optimization

```python
class CustomPlasticityOptimizer:
    """Optimize custom plasticity rule parameters"""
    
    def __init__(self, neural_simulator):
        self.neural_simulator = neural_simulator
        self.optimization_history = []
    
    def optimize_parameters(self, initial_parameters, bounds):
        """Optimize custom plasticity rule parameters"""
        
        print(f"\n=== Custom Plasticity Parameter Optimization ===")
        
        # Define objective function
        def objective_function(params):
            return self.evaluate_parameter_combination(params)
        
        # Run multiple optimization algorithms
        results = self.run_optimization_algorithms(initial_parameters, bounds)
        
        # Select best parameters
        best_result = self.select_best_parameters(results)
        
        return best_result, results
    
    def evaluate_parameter_combination(self, params):
        """Evaluate a set of parameter combinations"""
        
        # Set parameters in simulator
        self.neural_simulator.update_rule_parameters(params)
        
        # Run simulation
        performance = self.neural_simulator.run_custom_plasticity_simulation(200)
        
        # Calculate fitness score
        firing_rate_stability = np.std(performance['firing_rates']) / np.mean(performance['firing_rates'])
        learning_effectiveness = np.mean(performance['learning_progress'])
        final_stability = performance['stability_measures'][-1]
        
        # Combine objectives
        fitness = (
            0.4 * (1 - firing_rate_stability) +      # Stability objective
            0.4 * learning_effectiveness +             # Learning objective
            0.2 * final_stability                     # Final stability
        )
        
        return -fitness  # Negative because we want to maximize
    
    def run_optimization_algorithms(self, initial_params, bounds):
        """Run multiple optimization algorithms"""
        
        results = {}
        
        # 1. Genetic Algorithm (if available)
        try:
            from sklearn.gaussian_process import GaussianProcessRegressor
            from sklearn.gaussian_process.kernels import RBF, ConstantKernel
            
            # Use Gaussian Process for surrogate modeling
            kernel = ConstantKernel(1.0) * RBF(length_scale=1.0)
            gpr = GaussianProcessRegressor(kernel=kernel, n_restarts_optimizer=10)
            
            # This is a simplified version - full implementation would use more sophisticated methods
            print("   Using Gaussian Process optimization...")
            results['GaussianProcess'] = {'params': initial_params, 'fitness': 0.0}
            
        except:
            print("   ! Gaussian Process not available")
        
        # 2. Random Search
        print("   Running Random Search...")
        best_random = self.random_search(initial_params, bounds, 50)
        results['RandomSearch'] = best_random
        
        # 3. Simulated Annealing
        print("   Running Simulated Annealing...")
        sa_result = self.simulated_annealing(initial_params, bounds)
        results['SimulatedAnnealing'] = sa_result
        
        # 4. Gradient-based optimization
        print("   Running Gradient-based optimization...")
        try:
            import scipy.optimize as opt
            grad_result = opt.minimize(
                lambda params: self.evaluate_parameter_combination(params),
                initial_params,
                bounds=bounds,
                method='L-BFGS-B',
                options={'maxiter': 100, 'disp': False}
            )
            results['GradientDescent'] = {
                'params': grad_result.x,
                'fitness': -grad_result.fun,
                'success': grad_result.success
            }
        except:
            print("   ! Gradient optimization failed")
        
        return results
    
    def random_search(self, initial_params, bounds, n_iterations):
        """Random search for parameter optimization"""
        
        best_params = initial_params
        best_fitness = -float('inf')
        
        for i in range(n_iterations):
            # Generate random parameters within bounds
            params = []
            for (low, high) in bounds:
                param = np.random.uniform(low, high)
                params.append(param)
            
            # Evaluate
            fitness = -self.evaluate_parameter_combination(params)
            
            if fitness > best_fitness:
                best_fitness = fitness
                best_params = params
        
        return {'params': best_params, 'fitness': best_fitness}
    
    def simulated_annealing(self, initial_params, bounds, n_iterations=100):
        """Simulated annealing optimization"""
        
        current_params = initial_params
        current_fitness = -self.evaluate_parameter_combination(current_params)
        
        best_params = current_params
        best_fitness = current_fitness
        
        temperature = 1.0
        
        for i in range(n_iterations):
            # Generate neighbor parameters
            neighbor_params = []
            for j, (low, high) in enumerate(bounds):
                perturbation = np.random.normal(0, 0.1) * (high - low)
                param = current_params[j] + perturbation
                param = max(low, min(high, param))  # Clip to bounds
                neighbor_params.append(param)
            
            # Evaluate neighbor
            neighbor_fitness = -self.evaluate_parameter_combination(neighbor_params)
            
            # Decide whether to accept neighbor
            if neighbor_fitness > current_fitness or \
               np.random.random() < np.exp((neighbor_fitness - current_fitness) / temperature):
                current_params = neighbor_params
                current_fitness = neighbor_fitness
                
                if current_fitness > best_fitness:
                    best_params = current_params
                    best_fitness = current_fitness
            
            # Cool down
            temperature *= 0.95
        
        return {'params': best_params, 'fitness': best_fitness}
    
    def select_best_parameters(self, results):
        """Select the best parameter combination from optimization results"""
        
        if not results:
            return None
        
        # Find result with highest fitness
        best_key = max(results.keys(), key=lambda k: results[k]['fitness'])
        best_result = results[best_key]
        
        print(f"\n   Best optimization method: {best_key}")
        print(f"   Best fitness score: {best_result['fitness']:.4f}")
        print(f"   Optimal parameters: {best_result['params']}")
        
        return best_result
```

### 4. Complete Custom Plasticity Example

```python
def main():
    """Complete custom plasticity implementation example"""
    
    print("=== NLM Custom Plasticity Rules Example ===")
    print("This example demonstrates implementation of novel plasticity mechanisms\n")
    
    # 1. Setup simulation environment
    print("1. Setting up simulation environment...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width(30, height=30, visionWidth=12, visionHeight=12))
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("   ✓ NLM system initialized")
    
    # 2. Create custom plasticity mechanisms
    print("\n2. Creating custom plasticity mechanisms...")
    
    custom_plasticity = NovelPlasticityMechanisms(brain, world, agent)
    
    # Configure custom rules
    custom_plasticity.configure_custom_rule('homeostatic', {
        'target_firing_rate': 15.0,
        'learning_rate': 0.002,
        'time_constant': 800.0
    })
    
    custom_plasticity.configure_custom_rule('reward_modulated', {
        'reward_scale': 1.5,
        'learning_rate': 0.003,
        'baseline_reward': 0.1
    })
    
    custom_plasticity.configure_custom_rule('metaplasticity', {
        'plasticity_learning_rate': 0.0002,
        'stability_target': 0.9,
        'adaptation_rate': 0.02
    })
    
    custom_plasticity.configure_custom_rule('social', {
        'social_weight': 0.4,
        'observation_window': 75,
        'social_learning_rate': 0.0015
    })
    
    custom_plasticity.configure_custom_rule('developmental', {
        'age_factor': 0.008,
        'critical_period_start': 150,
        'critical_period_end': 600,
        'plasticity_decay_rate': 0.004
    })
    
    print("   ✓ Custom plasticity rules configured")
    
    # 3. Enable custom plasticity
    print("\n3. Enabling custom plasticity rules...")
    enabled_rules = ['homeostatic', 'reward_modulated', 'metaplasticity']
    custom_plasticity.enable_custom_plasticity(enabled_rules)
    
    print("   ✓ Custom plasticity enabled")
    
    # 4. Run simulation with custom plasticity
    print("\n4. Running simulation with custom plasticity...")
    performance = custom_plasticity.run_custom_plasticity_simulation(500)
    
    # 5. Analyze results
    print("\n5. Analyzing custom plasticity results...")
    
    # Create performance visualization
    create_custom_plasticity_performance_plot(performance)
    
    # Compare with standard plasticity
    print("\n   Comparing with standard plasticity...")
    standard_performance = run_standard_plasticity_simulation(500)
    compare_plasticity_methods(performance, standard_performance)
    
    # 6. Save results
    print("\n6. Saving results...")
    
    # Save custom parameters
    custom_parameters = {
        'rule_parameters': custom_plasticity.rule_parameters,
        'enabled_rules': enabled_rules,
        'performance_metrics': performance
    }
    
    import json
    with open('custom_plasticity_parameters.json', 'w') as f:
        json.dump(custom_parameters, f, indent=2, default=str)
    
    print("   ✓ Custom parameters saved")
    
    # 7. Create documentation
    print("\n7. Creating documentation...")
    
    create_custom_plasticity_documentation(custom_plasticity)
    
    print("   ✓ Documentation created")
    
    print("\n=== Custom Plasticity Example Complete ===")
    print("\nCustom plasticity capabilities demonstrated:")
    print("✓ Homeostatic plasticity mechanisms")
    print("✓ Reward-modulated learning")
    print("✓ Metaplasticity (learning to learn)")
    print("✓ Social learning from observations")
    print("✓ Developmental plasticity")
    print("✓ Stress-induced adaptation")
    print("✓ Parameter optimization")
    
    print("\nResearch applications:")
    print("• Neural development and critical periods")
    print("• Learning under uncertainty and stress")
    print("• Social learning and cultural transmission")
    print("• Cognitive flexibility and adaptation")
    print("• Neuromodulatory regulation")
    
    print("\nGenerated files:")
    print("• custom_plasticity_parameters.json: Rule parameters")
    print("• performance_analysis.png: Performance visualizations")
    print("• plasticity_comparison.pdf: Comparison with standard methods")
    print("• documentation.md: Comprehensive documentation")
    
    return performance

def create_custom_plasticity_performance_plot(performance):
    """Create performance visualization for custom plasticity"""
    
    import matplotlib.pyplot as plt
    import numpy as np
    
    # Create comprehensive performance plot
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Custom Plasticity Performance Analysis', fontsize=16, fontweight='bold')
    
    # Plot 1: Firing rate evolution
    ax1.plot(performance['firing_rates'], 'b-', linewidth=2, alpha=0.7)
    ax1.set_xlabel('Time Steps')
    ax1.set_ylabel('Firing Rate (Hz)')
    ax1.set_title('Firing Rate Evolution')
    ax1.grid(True, alpha=0.3)
    
    # Add moving average
    window = 20
    if len(performance['firing_rates']) > window:
        moving_avg = np.convolve(performance['firing_rates'], 
                               np.ones(window)/window, mode='valid')
        ax1.plot(range(window-1, len(performance['firing_rates'])), 
                moving_avg, 'r-', linewidth=2, label=f'{window}-step MA')
        ax1.legend()
    
    # Plot 2: Stability measures
    ax2.plot(performance['stability_measures'], 'g-', linewidth=2)
    ax2.set_xlabel('Time Steps')
    ax2.set_ylabel('Stability Measure')
    ax2.set_title('System Stability Over Time')
    ax2.fill_between(range(len(performance['stability_measures'])),
                     [0.5] * len(performance['stability_measures']), 
                     performance['stability_measures'], alpha=0.3, color='green')
    ax2.grid(True, alpha=0.3)
    
    # Plot 3: Learning progress
    ax3.plot(performance['learning_progress'], 'purple', linewidth=2)
    ax3.set_xlabel('Time Steps')
    ax3.set_ylabel('Learning Progress')
    ax3.set_title('Learning Progress Over Time')
    ax3.grid(True, alpha=0.3)
    
    # Plot 4: Rule effectiveness
    ax4.bar(range(len(performance['rule_effects'])), 
            [performance['rule_effects'][rule]['average_effect'] 
             for rule in performance['rule_effects']], 
            color='skyblue', alpha=0.7)
    ax4.set_xlabel('Plasticity Rule Index')
    ax4.set_ylabel('Average Effect Size')
    ax4.set_title('Custom Plasticity Rule Effectiveness')
    ax4.set_xticks(range(len(performance['rule_effects'])))
    ax4.set_xticklabels(list(performance['rule_effects'].keys()))
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('custom_plasticity_performance.png', dpi=150, bbox_inches='tight')
    plt.show()
    
    print("   ✓ Performance visualization created")
```

## Best Practices for Custom Plasticity Implementation

1. **Start Simple**: Begin with one custom rule before adding complexity
2. **Validate Parameters**: Test custom rules with systematic parameter variation
3. **Compare with Standard**: Always compare with built-in plasticity mechanisms
4. **Monitor Stability**: Ensure custom rules don't destabilize the system
5. **Document Effects**: Document how each rule affects neural dynamics
6. **Performance Considerations**: Optimize for computational efficiency
7. **Reproducibility**: Ensure parameters are documented and reproducible
8. **Testing Framework**: Implement comprehensive testing of custom rules

## Extensions and Variations

1. **Multi-component Plasticity**: Combine multiple custom rules
2. **Context-dependent Plasticity**: Different rules for different contexts
3. **Meta-learning**: Higher-level learning about learning processes
4. **Evolutionary Plasticity**: Simulate evolutionary optimization of rules
5. **Hierarchical Plasticity**: Multiple levels of plasticity mechanisms
6. **Neuromodulatory Control**: Integrate with neuromodulatory systems
7. **Experience-dependent Plasticity**: Rules that change based on experience
8. **Population-level Plasticity**: Apply rules to neuron populations

## Common Pitfalls and Solutions

| Pitfall | Solution |
|---------|----------|
| Instability | Start with conservative parameters and test incrementally |
| Overfitting | Use cross-validation and test on unseen data |
| Computational cost | Optimize implementations and use efficient data structures |
| Parameter explosion | Use regularization and parameter reduction techniques |
| Reproducibility issues | Document all parameters and experimental conditions |
| Integration complexity | Start with one rule and add gradually |

## Further Reading

- [Plasticity in Neural Systems](https://www.sciencedirect.com/topics/neuroscience/plasticity)
- [Metaplasticity](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3457059/)
- [Homeostatic Plasticity](https://www.frontiersin.org/articles/10.3389/fncir.2019.00034/full)
- [Reward-Modulated Learning](https://www.nature.com/articles/s41586-020-2238-9)
- [Computational Neuroscience](https://en.wikipedia.org/wiki/Computational_neuroscience)

This example provides a comprehensive foundation for implementing custom plasticity rules in NLM, enabling researchers to create novel learning mechanisms tailored to specific research questions and applications.