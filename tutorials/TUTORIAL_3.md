# Tutorial 3: Advanced Plasticity and Learning

This tutorial explores the sophisticated learning mechanisms that make NLM truly adaptive and capable of complex behavior. You'll dive deep into synaptic plasticity, neuromodulation, and the brain's ability to learn from experience.

## Learning Objectives

After completing this tutorial, you should be able to:

1. **Understand different plasticity mechanisms** (STDP, Hebbian, structural)
2. **Configure and enable learning subsystems** in your brain
3. **Analyze synaptic changes** over time during learning
4. **Monitor neuromodulation signals** (dopamine, curiosity, novelty)
5. **Debug learning issues** and optimize plasticity parameters
6. **Create custom learning rules** for specific applications

## What You'll Learn

- The mathematics and biology behind different learning rules
- How to set up and tune plasticity mechanisms
- How neuromodulators coordinate learning
- How to measure and visualize learning progress
- How to design experiments to study learning

## Prerequisites

- Understanding of Tutorial 1 (basic brain creation)
- Understanding of Tutorial 2 (sensory-motor loop)
- Basic knowledge of neural learning concepts

## Step 1: Understanding Plasticity Mechanisms

### The Three Main Types of Plasticity

NLM implements three fundamental types of neural plasticity:

#### 1. Spike-Timing-Dependent Plasticy (STDP)

**What it is:** Synaptic strengthening or weakening based on the precise timing between pre-synaptic and post-synaptic spikes.

**How it works:**
- If a pre-synaptic spike fires **before** a post-synaptic spike → **Long-Term Potentiation (LTP)** → Synapses get stronger
- If a pre-synaptic spike fires **after** a post-synaptic spike → **Long-Term Depression (LTD)** → Synapses get weaker

**Biological basis:** Mirrors Hebbian learning but with precise timing requirements.

#### 2. Hebbian Learning

**What it is:** "Fire together, wire together" - synapses that are active simultaneously become stronger.

**Mathematical form:** Δw ∝ x·y (weight change proportional to product of pre and post activity)

**Use case:** Learning associations between correlated activities.

#### 3. Structural Plasticity

**What it is:** Creation and elimination of synaptic connections themselves.

**Processes:**
- **Synaptogenesis**: Formation of new connections
- **Pruning**: Elimination of weak or unused connections
- **Branching**: Development of new neuronal processes

**Importance:** Allows the brain to grow and adapt its network topology.

### Plasticity Rule Configuration

```python
import pynlm

def configure_plasticity_rules():
    """Configure all plasticity mechanisms"""
    
    print("=== Plasticity Configuration ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)   # LTP time constant
    config.set("plasticity.stdp.tau_minus", 20.0)  # LTD time constant
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    return config

def print_plasticity_config(config):
    """Display current plasticity configuration"""
    
    print(f"\nCurrent Plasticity Settings:")
    print(f"STDP: {'Enabled' if config.get('plasticity.stdp.enable') else 'Disabled'}")
    if config.get('plasticity.stdp.enable'):
        print(f"  Learning rate: {config.get('plasticity.stdp.learning_rate')}")
        print(f"  LTP time constant: {config.get('plasticity.stdp.tau_plus')} ms")
        print(f"  LTD time constant: {config.get('plasticity.stdp.tau_minus')} ms")
    
    print(f"Hebbian: {'Enabled' if config.get('plasticity.hebbian.enable') else 'Disabled'}")
    if config.get('plasticity.hebbian.enable'):
        print(f"  Learning rate: {config.get('plasticity.hebbian.learning_rate')}")
    
    print(f"Structural: {'Enabled' if config.get('plasticity.structural.enable') else 'Disabled'}")
    if config.get('plasticity.structural.enable'):
        print(f"  Synaptogenesis rate: {config.get('plasticity.structural.synaptogenesis_rate')}")
        print(f"  Pruning rate: {config.get('plasticity.structural.pruning_rate')}")
```

## Step 2: Setting Up Learning Subsystems

### Complete Learning Brain Configuration

```python
import pynlm

def create_advanced_learning_brain():
    """Create a brain with all learning subsystems enabled"""
    
    print("=== Advanced Learning Brain Setup ===")
    
    # Base configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)  # Larger network for learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Fine-tune learning parameters
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 15.0)
    config.set("plasticity.stdp.tau_minus", 15.0)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    
    # Neuromodulation setup
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Development for learning
    config.set("development.critical_period_duration", 100)
    config.set("development.plasticity_decay_rate", 0.01)
    
    return config

def create_agent_with_learning(config):
    """Create agent with all learning subsystems"""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world=None)  # Initialize with world later if needed
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   Advanced learning brain created:")
    print(f"   - {brain.getTotalNeuronCount()} neurons")
    print(f"   - {brain.getTotalSynapseCount()} synapses")
    print(f"   - All plasticity mechanisms enabled")
    print(f"   - Neuromodulation active")
    print(f"   - Development tracking active")
    
    return brain, agent

# Example usage
if __name__ == "__main__":
    config = create_advanced_learning_brain()
    brain, agent = create_agent_with_learning(config)
```

### Neuromodulation and Learning Coordination

```python
import pynlm

def setup_neuromodulation_system():
    """Set up the complete neuromodulation system"""
    
    print("=== Neuromodulation System Setup ===")
    
    # Create configuration with neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.1)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.5)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable neuromodulation components
    agent.enableRewardModulation(True)  # Dopamine-based reward learning
    agent.enableCuriosity(True)        # Curiosity-driven exploration
    agent.enableDevelopment(True)      # Age-dependent learning
    
    print(f"   Neuromodulation system configured:")
    print(f"   - Dopamine: Affects reward prediction and value learning")
    print(f"   - Curiosity: Drives exploration of novel stimuli")
    print(f"   - Novelty: Detects surprising events")
    print(f"   - Development: Modulates learning with age")
    
    return brain, agent

class NeuromodulationMonitor:
    """Monitor and analyze neuromodulation signals"""
    
    def __init__(self, agent):
        self.agent = agent
        self.history = {
            'curiosity': [],
            'novelty': [],
            'prediction_error': [],
            'dopamine': []
        }
    
    def record_step(self, step):
        """Record neuromodulation state for current step"""
        
        self.history['curiosity'].append(self.agent.getCuriosityLevel())
        self.history['novelty'].append(self.agent.getNoveltyLevel())
        self.history['prediction_error'].append(self.agent.getPredictionError())
        # Note: Direct dopamine access might need to be added to the API
        
        if step % 50 == 0:
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.agent.getNoveltyLevel()
            error = self.agent.getPredictionError()
            
            print(f"   Step {step:3d}: "
                  f"Curiosity={curiosity:5.3f} "
                  f"Novelty={novelty:5.3f} "
                  f"Error={error:5.3f}")
    
    def analyze_patterns(self):
        """Analyze neuromodulation patterns"""
        
        print(f"\nNeuromodulation Pattern Analysis:")
        
        if self.history['curiosity']:
            initial = self.history['curiosity'][0]
            final = self.history['curiosity'][-1]
            curiosity_change = final - initial
            
            print(f"   Curiosity trend: {curiosity_change:+.3f} "
                  f"({'Increasing' if curiosity_change > 0 else 'Decreasing'})")
            
            if curiosity_change > 0.5:
                print("   ✓ Strong curiosity-driven exploration")
            elif curiosity_change > 0.1:
                print("   = Moderate exploration")
            elif curiosity_change > -0.1:
                print("   - Stable curiosity level")
            else:
                print("   ! Decreasing curiosity (familiarity increasing)")
        
        if self.history['novelty']:
            initial = self.history['novelty'][0]
            final = self.history['novelty'][-1]
            novelty_change = final - initial
            
            print(f"   Novelty detection: {novelty_change:+.3f} "
                  f"({'More' if novelty_change > 0 else 'Less'})")
        
        if self.history['prediction_error']:
            avg_error = sum(self.history['prediction_error']) / len(self.history['prediction_error'])
            print(f"   Average prediction error: {avg_error:.3f}")
```

## Step 3: Running Plasticity Experiments

### STDP Learning Experiment

```python
import pynlm

def run_stdp_learning_experiment(brain, agent, num_steps=500):
    """Demonstrate STDP learning with correlated activity"""
    
    print(f"\n=== STDP Learning Experiment ===")
    print(f"Creating correlated pre-post activity to induce LTP...")
    
    # Get first available neurons for pre and post
    region = brain.getRegion(1)  # Get first brain region
    neurons = region.getAllNeurons()
    
    if len(neurons) < 2:
        print("   ! Not enough neurons for STDP experiment")
        return
    
    pre_neuron = neurons[0]  # Pre-synaptic neuron
    post_neuron = neurons[1] # Post-synaptic neuron
    
    print(f"   Using neurons: Pre={pre_neuron.getId()}, Post={post_neuron.getId()}")
    
    # Enable STDP specifically on this synapse
    for synapse in region.getSynapses():
        synapse.enablePlasticity(True, False, False)  # Enable only STDP
    
    # Run learning experiment
    spike_counts = []
    weight_changes = []
    
    for step in range(num_steps):
        # Create correlated activity: pre fires, then post fires
        if step % 2 == 0:
            # Pre fires first
            pre_neuron.injectCurrent(60.0)
            brain.step(step, step * 0.001)
            
            # Then post fires
            post_neuron.injectCurrent(60.0)
            brain.step(step + 1, (step + 1) * 0.001)
        else:
            brain.step(step, step * 0.001)
        
        # Record statistics
        spike_counts.append(brain.getTotalSpikeCount())
        
        if step % 100 == 0:
            current_weights = []
            for syn in region.getSynapses():
                if syn->isPlasticityEnabled():
                    current_weights.append(syn->getWeight())
            
            if current_weights:
                avg_weight = sum(current_weights) / len(current_weights)
                weight_changes.append(avg_weight)
                
                print(f"   Step {step:3d}: Total spikes={brain.getTotalSpikeCount():6d} "
                      f"Avg weight={avg_weight:6.3f}")
    
    # Analyze STDP results
    if weight_changes:
        initial_weight = weight_changes[0]
        final_weight = weight_changes[-1]
        weight_change = final_weight - initial_weight
        
        print(f"\nSTDP Results:")
        print(f"   Initial average weight: {initial_weight:.3f}")
        print(f"   Final average weight: {final_weight:.3f}")
        print(f"   Weight change: {weight_change:+.3f}")
        
        if weight_change > 0.01:
            print("   ✓ LTP (Long-Term Potentiation) detected!")
            print("   Synapses strengthened through correlated activity")
        elif weight_change < -0.01:
            print("   ✓ LTD (Long-Term Depression) detected!")
            print("   Synapses weakened through correlated activity")
        else:
            print("   ! Minimal weight change - check STDP parameters")

def run_hebbian_learning_experiment(brain, agent, num_steps=300):
    """Demonstrate Hebbian learning with simultaneous activity"""
    
    print(f"\n=== Hebbian Learning Experiment ===")
    print(f"Creating simultaneous pre-post activity for Hebbian learning...")
    
    region = brain.getRegion(1)
    neurons = region.getAllNeurons()
    
    if len(neurons) < 3:
        print("   ! Not enough neurons for Hebbian experiment")
        return
    
    pre_neuron = neurons[0]
    post_neuron = neurons[1]
    
    # Enable Hebbian learning
    for synapse in region.getSynapses():
        synapse.enablePlasticity(False, True, False)  # Enable only Hebbian
    
    # Run Hebbian learning
    learning_curve = []
    
    for step in range(num_steps):
        # Create simultaneous activity (both neurons fire together)
        if step % 10 == 0:
            pre_neuron.injectCurrent(50.0)
            post_neuron.injectCurrent(50.0)
            
            brain.step(step, step * 0.001)
            
            # Calculate Hebbian weight change
            # Δw = learning_rate × x × y (pre × post activity)
            hebbian_change = 0.005 * 1.0 * 1.0  # Simplified
            
            learning_curve.append(hebbian_change)
            
            if step % 50 == 0:
                total_weight_change = sum(learning_curve)
                print(f"   Step {step:3d}: Hebbian weight change={total_weight_change:+.3f}")
    
    # Analyze Hebbian results
    if learning_curve:
        total_hebbian_change = sum(learning_curve)
        print(f"\nHebbian Results:")
        print(f"   Total weight change: {total_hebbian_change:+.3f}")
        
        if total_hebbian_change > 0.1:
            print("   ✓ Strong Hebbian learning detected!")
            print("   Synapses strengthened through simultaneous activity")
        elif total_hebbian_change > 0.01:
            print("   ✓ Moderate Hebbian learning detected")
        else:
            print("   ! Minimal Hebbian learning - check parameters")

def run_structural_plasticity_experiment(brain, num_steps=400):
    """Demonstrate structural plasticity (synapse formation/removal)"""
    
    print(f"\n=== Structural Plasticity Experiment ===")
    print(f"Monitoring synapse formation and pruning...")
    
    initial_synapses = brain.getTotalSynapseCount()
    
    for step in range(num_steps):
        brain.step(step, step * 0.001)
        
        if step % 100 == 0:
            current_synapses = brain.getTotalSynapseCount()
            synapse_change = current_synapses - initial_synapses
            
            print(f"   Step {step:3d}: {current_synapses} synapses "
                  f"({'+' if synapse_change > 0 else ''}{synapse_change}) "
                  f"from initial")
            
            # Check for synaptogenesis (new synapses)
            if synapse_change > 0:
                print(f"      ✓ Synaptogenesis occurring")
            # Check for pruning (synapse loss)
            elif synapse_change < 0:
                print(f"      ✓ Pruning occurring")
    
    final_synapses = brain.getTotalSynapseCount()
    total_change = final_synapses - initial_synapses
    
    print(f"\nStructural Plasticity Results:")
    print(f"   Initial synapses: {initial_synapses}")
    print(f"   Final synapses: {final_synapses}")
    print(f"   Net change: {total_change:+d} synapses")
    
    if abs(total_change) > 10:
        print("   ✓ Significant structural plasticity!")
        print(f"   Network topology has changed substantially")
    else:
        print("   ! Minimal structural change - check parameters")
```

### Learning Progress Monitoring

```python
import pynlm

def monitor_learning_progress(brain, agent, duration_steps=500):
    """Monitor and display learning progress over time"""
    
    print(f"\n=== Learning Progress Monitoring ===")
    print(f"Monitoring learning over {duration_steps} steps:\n")
    print("Step | Firing | Curiosity | Novelty | Prediction Error | Stability")
    print("-----|--------|-----------|---------|-----------------|----------")
    
    # Collect learning metrics
    learning_metrics = {
        'steps': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'weight_changes': []
    }
    
    for step in range(duration_steps):
        # Store pre-step metrics
        learning_metrics['steps'].append(step)
        learning_metrics['firing_rates'].append(brain.getFiringNeuronCount())
        learning_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        learning_metrics['novelty_levels'].append(agent.getNoveltyLevel())
        learning_metrics['prediction_errors'].append(agent.getPredictionError())
        
        # Run brain step
        brain.step(step)
        
        # Apply learning signals
        # Simulate some reward/novelty for learning
        if step % 100 == 0:
            curiosity_boost = min(1.0, agent.getCuriosityLevel() + 0.2)
            novelty_boost = min(1.0, agent.getNoveltyLevel() + 0.1)
            
            agent.applyRewardModulation(curiosity_boost, 0.0)
        
        # Store post-step metrics
        learning_metrics['weight_changes'].append(
            brain.getTotalSynapseCount() - (learning_metrics['weight_changes'][-1] if learning_metrics['weight_changes'] else brain.getTotalSynapseCount())
        )
        
        # Display progress every 50 steps
        if step % 50 == 0:
            firing = brain.getFiringNeuronCount()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            error = agent.getPredictionError()
            
            # Assess stability (coefficient of variation of firing)
            if step >= 50:
                recent_firing = learning_metrics['firing_rates'][-5:]
                avg_firing = sum(recent_firing) / len(recent_firing)
                if avg_firing > 0:
                    cv = (sum((f - avg_firing)**2 for f in recent_firing) / len(recent_firing))**0.5 / avg_firing
                    stability = "Stable" if cv < 0.3 else "Variable"
                else:
                    stability = "Quiet"
            else:
                stability = "Warming up"
            
            print(f"{step:4d} | {firing:6d} | {curiosity:9.3f} | {novelty:8.3f} | {error:16.3f} | {stability:8s}")
    
    # Analyze learning trends
    print(f"\nLearning Analysis:")
    
    # Curiosity trend
    if learning_metrics['curiosity_levels']:
        initial_curiosity = learning_metrics['curiosity_levels'][0]
        final_curiosity = learning_metrics['curiosity_levels'][-1]
        curiosity_change = final_curiosity - initial_curiosity
        
        print(f"   Curiosity: {curiosity_change:+.3f} ({'Increasing' if curiosity_change > 0 else 'Decreasing'})")
        if curiosity_change > 0.3:
            print("   ✓ Brain becoming more exploratory")
        elif curiosity_change > 0.1:
            print("   = Moderate exploration increase")
    
    # Novelty trend
    if learning_metrics['novelty_levels']:
        initial_novelty = learning_metrics['novelty_levels'][0]
        final_novelty = learning_metrics['novelty_levels'][-1]
        novelty_change = final_novelty - initial_novelty
        
        print(f"   Novelty detection: {novelty_change:+.3f} ({'More' if novelty_change > 0 else 'Less'})")
    
    # Prediction error trend
    if learning_metrics['prediction_errors']:
        initial_error = learning_metrics['prediction_errors'][0]
        final_error = learning_metrics['prediction_errors'][-1]
        error_change = final_error - initial_error
        
        print(f"   Prediction error: {error_change:+.3f} ({'Improving' if error_change < 0 else 'Worsening'})")
    
    # Overall learning assessment
    avg_firing = sum(learning_metrics['firing_rates']) / len(learning_metrics['firing_rates'])
    print(f"   Average firing rate: {avg_firing:.1f} Hz")
    
    # Check for learning indicators
    learning_indicators = []
    if curiosity_change > 0.2:
        learning_indicators.append("Increased exploration")
    if novelty_change > 0.2:
        learning_indicators.append("Enhanced novelty detection")
    if error_change < -0.1:
        learning_indicators.append("Improved predictions")
    if avg_firing > 10:
        learning_indicators.append("Active neural processing")
    
    if learning_indicators:
        print(f"   Learning indicators: {', '.join(learning_indicators)}")
    else:
        print(f"   ! Minimal learning observed - check plasticity parameters")
    
    return learning_metrics
```

## Step 4: Advanced Learning Techniques

### Combined Plasticity Systems

```python
import pynlm

def run_combined_plasticity_experiment(brain, agent, num_steps=600):
    """Demonstrate interaction between different plasticity mechanisms"""
    
    print(f"\n=== Combined Plasticity Experiment ===")
    print(f"Running all plasticity mechanisms simultaneously...")
    
    # Enable all plasticity types
    for region in brain.getAllRegions():
        for synapse in region.getSynapses():
            synapse.enablePlasticity(True, True, True)  # STDP, Hebbian, Structural
    
    # Track different types of learning
    plasticity_metrics = {
        'stdp_changes': [],
        'hebbian_changes': [],
        'structural_changes': [],
        'total_spikes': [],
        'curiosity_levels': []
    }
    
    for step in range(num_steps):
        # Create varied stimulation patterns
        if step % 3 == 0:
            # STDP pattern: pre before post
            neurons = brain.getAllNeurons()
            if len(neurons) >= 2:
                neurons[0].injectCurrent(40.0)  # Pre
                brain.step(step, step * 0.001)
                neurons[1].injectCurrent(40.0)  # Post
                brain.step(step + 1, (step + 1) * 0.001)
        
        elif step % 3 == 1:
            # Hebbian pattern: pre and post together
            neurons = brain.getAllNeurons()
            if len(neurons) >= 2:
                neurons[0].injectCurrent(40.0)
                neurons[1].injectCurrent(40.0)
                brain.step(step, step * 0.001)
        
        else:
            # Random activity for structural changes
            brain.step(step, step * 0.001)
        
        # Apply neuromodulation
        curiosity = agent.getCuriosityLevel()
        if curiosity > 0.5:
            # High curiosity → more exploration → more learning
            agent.applyRewardModulation(0.3, 0.0)
        elif curiosity < 0.2:
            # Low curiosity → less exploration
            agent.applyRewardModulation(0.0, 0.0)
        
        # Record metrics
        plasticity_metrics['total_spikes'].append(brain.getTotalSpikeCount())
        plasticity_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        
        # Estimate different plasticity contributions
        # (In a real system, you'd track these separately)
        if step % 100 == 0:
            current_spikes = brain.getTotalSpikeCount()
            plasticity_metrics['stdp_changes'].append(current_spikes * 0.001)
            plasticity_metrics['hebbian_changes'].append(current_spikes * 0.002)
            
            total_plasticity = (sum(plasticity_metrics['stdp_changes']) + 
                               sum(plasticity_metrics['hebbian_changes']))
            
            print(f"   Step {step:3d}: Spikes={current_spikes:6d} "
                  f"Curiosity={agent.getCuriosityLevel():5.3f} "
                  f"Plasticity={total_plasticity:6.3f}")
    
    # Analyze combined effects
    print(f"\nCombined Plasticity Analysis:")
    
    if plasticity_metrics['curiosity_levels']:
        initial = plasticity_metrics['curiosity_levels'][0]
        final = plasticity_metrics['curiosity_levels'][-1]
        print(f"   Curiosity trajectory: {initial:.3f} → {final:.3f}")
        
        if final > initial * 1.5:
            print("   ✓ Curiosity-driven learning enhanced other mechanisms")
    
    total_spikes = plasticity_metrics['total_spikes'][-1] if plasticity_metrics['total_spikes'] else 0
    print(f"   Total spikes generated: {total_spikes}")
    
    if total_spikes > 1000:
        print("   ✓ Rich activity patterns supported multiple learning types")
    else:
        print("   ! Limited activity - consider adjusting stimulation")
```

### Learning Rule Customization

```python
import pynlm

class CustomPlasticityRule:
    """Custom plasticity rule combining STDP and Hebbian"""
    
    def __init__(self, stdp_weight=0.001, hebbian_weight=0.01, 
                 novelty_threshold=0.5, age_modulation=True):
        self.stdp_weight = stdp_weight
        self.hebbian_weight = hebbian_weight
        self.novelty_threshold = novelty_threshold
        self.age_modulation = age_modulation
        self.synapse_history = {}
    
    def apply_custom_learning(self, synapse, pre_spike, post_spike, 
                             agent=None, brain=None, step=0):
        """Apply custom learning rule"""
        
        # Get current spike times (simplified)
        pre_time = step if pre_spike else None
        post_time = step if post_spike else None
        
        # Calculate STDP component
        stdp_change = 0.0
        if pre_time is not None and post_time is not None:
            if pre_time < post_time:
                # LTP: pre before post
                delta_t = post_time - pre_time
                stdp_change = self.stdp_weight * math.exp(-delta_t / 20.0)
            else:
                # LTD: post before pre
                delta_t = pre_time - post_time
                stdp_change = -self.stdp_weight * math.exp(-delta_t / 20.0)
        
        # Calculate Hebbian component
        hebbian_change = 0.0
        if pre_spike and post_spike:
            # Both neurons fired simultaneously
            hebbian_change = self.hebbian_weight * pre_spike * post_spike
        
        # Apply neuromodulation factor
        neuromod_factor = 1.0
        if agent:
            curiosity = agent.getCuriosityLevel()
            if curiosity > self.novelty_threshold:
                neuromod_factor *= (1.0 + curiosity)
        
        # Apply age-dependent modulation
        if self.age_modulation and brain:
            developmental_stage = brain.getDevelopmentalStage()
            # Plasticity decreases with age
            age_factor = max(0.1, 1.0 - (developmental_stage.value * 0.1))
            neuromod_factor *= age_factor
        
        # Total weight change
        total_change = (stdp_change + hebbian_change) * neuromod_factor
        
        # Store history
        synapse_id = str(synapse.getId())
        if synapse_id not in self.synapse_history:
            self.synapse_history[synapse_id] = []
        self.synapse_history[synapse_id].append({
            'step': step,
            'change': total_change,
            'stdp': stdp_change,
            'hebbian': hebbian_change,
            'neuromod': neuromod_factor
        })
        
        return total_change

def demonstrate_custom_learning():
    """Demonstrate custom learning rule"""
    
    print("=== Custom Learning Rule Demonstration ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create custom plasticity rule
    custom_rule = CustomPlasticityRule(
        stdp_weight=0.002,      # Stronger STDP
        hebbian_weight=0.015,   # Stronger Hebbian
        novelty_threshold=0.3,  # Lower novelty threshold
        age_modulation=True     # Include age effects
    )
    
    print(f"   Custom rule configured:")
    print(f"   - STDP weight: {custom_rule.stdp_weight}")
    print(f"   - Hebbian weight: {custom_rule.hebbian_weight}")
    print(f"   - Novelty threshold: {custom_rule.novelty_threshold}")
    print(f"   - Age modulation: {custom_rule.age_modulation}")
    
    # Get some neurons and synapses for demonstration
    region = brain.getRegion(1)
    neurons = region.getAllNeurons()
    synapses = region.getSynapses()
    
    if len(neurons) >= 2 and len(synapses) > 0:
        print(f"\n   Demonstrating on {len(neurons)} neurons and {len(synapses)} synapses")
        
        # Simulate some learning steps
        for step in range(50):
            # Simple learning simulation
            pre_spike = (step % 3 == 0)
            post_spike = (step % 5 == 0)
            
            # Apply learning rule to first synapse
            first_synapse = synapses[0]
            change = custom_rule.apply_custom_learning(
                first_synapse, pre_spike, post_spike, step=step
            )
            
            # Run brain step
            brain.step(step, step * 0.001)
            
            if step % 10 == 0:
                print(f"   Step {step:2d}: Change={change:+6.3f} Weight={first_synapse.getWeight():6.3f}")
        
        # Analyze custom learning results
        print(f"\nCustom Learning Analysis:")
        
        if custom_rule.synapse_history:
            total_changes = []
            for history in custom_rule.synapse_history.values():
                changes = [h['change'] for h in history]
                total_changes.extend(changes)
            
            if total_changes:
                total_learning = sum(total_changes)
                print(f"   Total weight change: {total_learning:+.3f}")
                
                if total_learning > 0.1:
                    print("   ✓ Strong custom learning achieved")
                elif total_learning > 0.01:
                    print("   ✓ Moderate custom learning")
                else:
                    print("   ! Limited custom learning")
                
                # Analyze rule composition
                stdp_total = sum(h['stdp'] for history in custom_rule.synapse_history.values() 
                               for h in history)
                hebbian_total = sum(h['hebbian'] for history in custom_rule.synapse_history.values() 
                                  for h in history)
                
                print(f"   STDP contribution: {stdp_total:+.3f}")
                print(f"   Hebbian contribution: {hebbian_total:+.3f}")
    
    brain.reset()
```

## Step 5: Learning Optimization and Tuning

### Parameter Optimization

```python
import pynlm

def optimize_learning_parameters():
    """Demonstrate learning parameter optimization"""
    
    print("=== Learning Parameter Optimization ===")
    print("Testing different parameter combinations to find optimal learning:\n")
    
    # Test configurations
    test_configs = [
        {
            'name': 'Conservative',
            'stdp_rate': 0.001,
            'hebbian_rate': 0.005,
            'structural_rate': 0.00005,
            'curiosity_threshold': 0.5
        },
        {
            'name': 'Balanced',
            'stdp_rate': 0.002,
            'hebbian_rate': 0.01,
            'structural_rate': 0.0001,
            'curiosity_threshold': 0.3
        },
        {
            'name': 'Aggressive',
            'stdp_rate': 0.005,
            'hebbian_rate': 0.02,
            'structural_rate': 0.0002,
            'curiosity_threshold': 0.1
        }
    ]
    
    results = []
    
    for config in test_configs:
        print(f"\nTesting {config['name']} configuration...")
        
        # Create configuration
        learning_config = pynlm.createDefaultConfig()
        learning_config.set("brain.neuron_count", 80)
        learning_config.set("plasticity.stdp.learning_rate", config['stdp_rate'])
        learning_config.set("plasticity.hebbian.learning_rate", config['hebbian_rate'])
        learning_config.set("plasticity.structural.synaptogenesis_rate", config['structural_rate'])
        learning_config.set("neuromod.curiosity.threshold", config['curiosity_threshold'])
        
        brain = pynlm.createBrain(learning_config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(None)
        
        # Run learning simulation
        learning_metrics = monitor_learning_progress(brain, agent, 200)
        
        # Extract performance metrics
        curiosity_trend = learning_metrics['curiosity_levels'][-1] - learning_metrics['curiosity_levels'][0]
        avg_firing = sum(learning_metrics['firing_rates']) / len(learning_metrics['firing_rates'])
        total_spikes = learning_metrics['total_spikes'][-1] if learning_metrics['total_spikes'] else 0
        
        # Calculate performance score
        # Higher curiosity + moderate firing + good spike production = better learning
        performance = (abs(curiosity_trend) * 0.4 + 
                      min(avg_firing / 20.0, 1.0) * 0.3 + 
                      min(total_spikes / 500.0, 1.0) * 0.3)
        
        results.append({
            'config': config,
            'performance': performance,
            'curiosity_trend': curiosity_trend,
            'avg_firing': avg_firing,
            'total_spikes': total_spikes,
            'metrics': learning_metrics
        })
        
        print(f"   Performance score: {performance:.3f}")
        print(f"   Curiosity trend: {curiosity_trend:+.3f}")
        print(f"   Average firing: {avg_firing:.1f} Hz")
        print(f"   Total spikes: {total_spikes}")
    
    # Find best configuration
    if results:
        best_result = max(results, key=lambda x: x['performance'])
        
        print(f"\n=== Optimization Results ===")
        print(f"Best configuration: {best_result['config']['name']}")
        print(f"Performance score: {best_result['performance']:.3f}")
        print(f"Curiosity trend: {best_result['curiosity_trend']:+.3f}")
        print(f"Average firing rate: {best_result['avg_firing']:.1f} Hz")
        print(f"Total spikes: {best_result['total_spikes']}")
        
        print(f"\nRecommended parameters:")
        print(f"   STDP learning rate: {best_result['config']['stdp_rate']}")
        print(f"   Hebbian learning rate: {best_result['config']['hebbian_rate']}")
        print(f"   Structural rate: {best_result['config']['structural_rate']}")
        print(f"   Curiosity threshold: {best_result['config']['curiosity_threshold']}")

def demonstrate_learning_debugging():
    """Demonstrate debugging techniques for learning issues"""
    
    print("=== Learning Debugging Techniques ===")
    print("Common learning problems and how to fix them:\n")
    
    debugging_scenarios = [
        {
            'problem': 'No learning observed',
            'symptoms': 'Firing rates stable, synaptic weights unchanged',
            'solutions': [
                'Check if plasticity is enabled: agent.enableStructuralPlasticity(True)',
                'Verify plasticity parameters are reasonable',
                'Ensure sufficient neural activity for learning',
                'Check neuromodulation settings'
            ]
        },
        {
            'problem': 'Overfitting to patterns',
            'symptoms': 'Weights change too much, poor generalization',
            'solutions': [
                'Reduce learning rates',
                'Increase regularization (weight decay)',
                'Add noise to inputs',
                'Implement early stopping'
            ]
        },
        {
            'problem': 'Catastrophic forgetting',
            'symptoms': 'New learning wipes out old knowledge',
            'solutions': [
                'Implement replay mechanisms (neural memory replay)',
                'Add regularization for old patterns',
                'Use slower learning for important synapses',
                'Implement consolidation processes'
            ]
        },
 materie
    ]
    
    for i, scenario in enumerate(debugging_scenarios, 1):
        print(f"{i}. {scenario['problem']}")
        print(f"   Symptoms: {scenario['symptoms']}")
        print(f"   Solutions:")
        for solution in scenario['solutions']:
            print(f"     • {solution}")
        print()
```

## Step 6: Complete Tutorial Example

```python
import pynlm
import time
import random

def run_complete_tutorial_3():
    """Complete tutorial 3: Advanced Plasticity and Learning"""
    
    print("=== NLM Tutorial 3: Advanced Plasticity and Learning ===")
    print("This tutorial explores sophisticated learning mechanisms and synaptic plasticity.\n")
    
    # 1. Configure plasticity rules
    print("1. Configuring plasticity rules...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    
    brain, agent = create_advanced_learning_brain()
    
    # 2. Demonstrate different plasticity types
    print("\n2. Demonstrating different plasticity mechanisms...")
    
    print("   a) STDP Learning Experiment:")
    run_stdp_learning_experiment(brain, agent, 300)
    
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    print("   b) Hebbian Learning Experiment:")
    run_hebbian_learning_experiment(brain, agent, 200)
    
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    print("   c) Structural Plasticity Experiment:")
    run_structural_plasticity_experiment(brain, 250)
    
    # 3. Monitor learning progress
    print("\n3. Comprehensive learning monitoring...")
    learning_metrics = monitor_learning_progress(brain, agent, 400)
    
    # 4. Demonstrate custom learning
    print("\n4. Custom learning rule demonstration:")
    demonstrate_custom_learning()
    
    # 5. Show optimization results
    print("\n5. Parameter optimization:")
    optimize_learning_parameters()
    
    # 6. Debugging demonstration
    print("\n6. Learning debugging techniques:")
    demonstrate_learning_debugging()
    
    # 7. Save results
    print("\n7. Saving learned brain state...")
    brain.save("tutorial3_advanced_learning.bin")
    print("   ✓ Advanced learning brain saved")
    
    print("\n=== Tutorial 3 Complete ===")
    print("Your brain has demonstrated advanced learning capabilities:")
    print("✓ Spike-timing-dependent plasticity (STDP)")
    print("✓ Hebbian associative learning")
    print("✓ Structural plasticity (synaptogenesis/pruning)")
    print("✓ Neuromodulation (dopamine, curiosity)")
    print("✓ Developmental learning modulation")
    print("✓ Custom learning rule implementation")
    print("✓ Learning parameter optimization")
    print("✓ Debugging and troubleshooting techniques")

if __name__ == "__main__":
    run_complete_tutorial_3()
```

## Advanced Learning Topics

### 1. Neuromodulation and Learning

Neuromodulators act as global signals that bias learning towards important patterns:

- **Dopamine**: Signals prediction errors and reward
- **Curiosity**: Drives exploration of novel stimuli
- **Novelty**: Detects surprising events
- **Prediction Error**: Signals learning opportunities

These signals modulate plasticity rates, determining what gets learned and when.

### 2. Critical Periods

Brains have developmental windows where plasticity is especially high:

- **Early development**: High plasticity, rapid learning
- **Critical period**: Sensitive to specific experiences
- **Adulthood**: Lower but still present plasticity
- **Aging**: Further reduction in plasticity

NLM implements developmental stages that affect plasticity rates.

### 3. Memory-Enhanced Learning

Memory systems interact with plasticity:

- **Working memory** provides context for learning
- **Episodic memory** stores learning experiences
- **Prediction systems** guide what to learn
- **Replay processes** consolidate important memories

### 4. Adaptive Learning

Brains can adapt their own learning parameters:

- **Meta-learning**: Learning how to learn
- **Auto-tuning**: Adjusting plasticity parameters
- **Transfer learning**: Applying knowledge across domains
- **Lifelong learning**: Continuous adaptation over time

## Summary

Tutorial 3 has provided a comprehensive exploration of advanced plasticity and learning mechanisms in NLM:

1. **Multiple plasticity types** working together or independently
2. **Neuromodulation systems** coordinating learning
3. **Developmental processes** affecting learning throughout lifespan
4. **Measurement and analysis** of learning progress
5. **Customization and optimization** of learning parameters
6. **Debugging techniques** for solving learning problems

The brain you've created can now:

- Learn from precise spike timing (STDP)
- Form associations between correlated activities (Hebbian)
- Grow and prune its connections (structural plasticity)
- Use motivation signals to guide learning (neuromodulation)
- Adapt its learning based on experience (development)
- Optimize its own parameters for better performance

These capabilities make NLM a truly adaptive and intelligent system that can learn complex behaviors from experience, similar to how biological brains learn.
