# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Complete Simulation Loop

```python
def run_simulation(brain, world, agent, num_steps):
    """Complete simulation loop with all systems integrated.
    
    This is the most common pattern for running a full simulation
    where the agent interacts with a changing environment.
    """
    for step in range(num_steps):
        # 1. Update world (time passes, objects move, etc.)
        world.update(0.1)
        
        # 2. Get what the agent sees (sensory input)
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain what it senses
        agent.processSensoryInput(percept)
        
        # 4. Brain processes and thinks
        brain.step(step)
        
        # 5. Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply learning from experience (optional)
        if agent.isRewardModulationEnabled():
            # Calculate reward based on action result
            reward = 0.0f  # Could be based on world feedback
            agent.applyRewardModulation(reward, 0.0f)

    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage()
    }

# Use it like this:
results = run_simulation(brain, world, agent, 1000)
print(f"Simulation complete: {results['firing_neurons']} neurons firing")
```

### Pattern 2: Advanced Brain Monitoring

```python
def monitor_brain_performance(brain, step_interval=100):
    """Monitor and log brain performance metrics."""
    monitoring_data = {
        'steps': [],
        'spikes': [],
        'firing_rate': [],
        'excitement_ratio': [],
        'development_stage': [],
        'memory_usage': []
    }
    
    for step in range(0, 10000, step_interval):
        metrics = {
            'step': step,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage(),
            'working_memory_traces': brain.getWorkingMemory() and 
                                     brain.getWorkingMemory()->getActiveTraces() or 0,
            'episodic_memory_count': brain.getEpisodicMemory() and 
                                     brain.getEpisodicMemory()->getEpisodeCount() or 0
        }
        
        monitoring_data['steps'].append(step)
        monitoring_data['spikes'].append(metrics['total_spikes'])
        monitoring_data['firing_rate'].append(metrics['avg_firing_rate'])
        monitoring_data['excitement_ratio'].append(metrics['excitement_inhibition_ratio'])
        monitoring_data['development_stage'].append(metrics['development_stage'])
        monitoring_data['memory_usage'].append(metrics['working_memory_traces'])
        
        # Optional: Print progress
        if step % 1000 == 0:
            print(f"Step {step}: {metrics['firing_neurons']} neurons, "
                  f"{metrics['avg_firing_rate']:.2f} Hz firing rate")
    
    return monitoring_data

# Use it like this:
# Enable memory systems first
brain.getWorkingMemory();  # Just access to enable
brain.getEpisodicMemory();  # Just access to enable

performance = monitor_brain_performance(brain)
print(f"Monitored {len(performance['steps'])} data points")
```

### Pattern 3: Learning and Development Configuration

```python
def configure_advanced_learning(brain, agent, config):
    """Configure advanced learning features for better performance."""
    
    # Enable all learning systems
    agent.enableRewardModulation(config.get('reward_learning', True))
    agent.enableStructuralPlasticity(config.get('structural_plasticity', True))
    agent.enableDevelopment(config.get('development_enabled', True))
    agent.enableCuriosity(config.get('curiosity_enabled', True))
    
    # Configure developmental stages
    if config.get('development_enabled', True):
        # Set brain to different developmental stages
        brain.setDevelopmentalStage(DevelopmentalStage::Initial)
        
        # Customize development parameters
        auto* devSystem = brain.getDevelopmentSystem();
        if (devSystem) {
            devSystem->setMaturationRate(config.get('maturation_rate', 0.5f));
            devSystem->setPruningRate(config.get('pruning_rate', 0.01f));
            devSystem->setSynaptogenesisRate(config.get('synaptogenesis_rate', 0.1f));
        }
    
    # Configure neuromodulation
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->setBaseline(config.get('dopamine_baseline', 0.5f));
        dopamine->setResponseScaling(config.get('dopamine_response', 1.0f));
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        curiosity->setBaseline(config.get('curiosity_baseline', 0.3f));
        curiosity->setNoveltyScaling(config.get('curiosity_scaling', 2.0f));
    }
    
    # Configure memory systems
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        workingMem->setCapacity(config.get('working_memory_capacity', 100));
        workingMem->setTraceDecay(config.get('working_memory_decay', 0.9f));
    }
    
    auto* episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        episodicMem->setMaxEpisodes(config.get('episodic_memory_limit', 1000));
        episodicMem->setConsolidationThreshold(config.get('consolidation_threshold', 10.0f));
    }
    
    return True

# Example configuration
advanced_config = {
    'reward_learning': True,
    'structural_plasticity': True,
    'development_enabled': True,
    'curiosity_enabled': True,
    'maturation_rate': 0.7f,
    'pruning_rate': 0.02f,
    'synaptogenesis_rate': 0.15f,
    'dopamine_baseline': 0.6f,
    'dopamine_response': 1.2f,
    'curiosity_baseline': 0.4f,
    'curiosity_scaling': 2.5f,
    'working_memory_capacity': 200,
    'working_memory_decay': 0.95f,
    'episodic_memory_limit': 2000,
    'consolidation_threshold': 15.0f
}

configure_advanced_learning(brain, agent, advanced_config)
print("Advanced learning configuration applied")
```

### Pattern 4: Interactive Brain Control

```python
class InteractiveBrainController:
    """Advanced controller for interactive brain manipulation."""
    
public:
    InteractiveBrainController(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent)
        : brain_(brain), agent_(agent) {}
    
    void injectSensoryPattern(const std::string& pattern_type) {
        """Inject specific sensory patterns for testing."""
        if (!brain_ || !agent_) return;
        
        if (pattern_type == "flash") {
            // Create a flash pattern for vision
            Vision flash(visionWidth_, visionHeight_, 3);
            std::vector<float> data(visionWidth_ * visionHeight_ * 3, 0.0f);
            
            // Central bright flash
            size_t center = (visionWidth_ / 2) * (visionHeight_ / 2) * 3;
            for (size_t i = center; i < center + 3; ++i) {
                data[i] = 1.0f;
            }
            
            flash.setData(data);
            SensoryPercept percept;
            percept.setVision(flash);
            agent_->processSensoryInput(percept);
            
        } else if (pattern_type == "pattern") {
            // Create a moving pattern
            Vision pattern(visionWidth_, visionHeight_, 3);
            std::vector<float> data(visionWidth_ * visionHeight_ * 3, 0.0f);
            
            // Create a simple pattern (e.g., horizontal stripes)
            for (size_t y = 0; y < visionHeight_; ++y) {
                for (size_t x = 0; x < visionWidth_; ++x) {
                    if ((x + y) % 2 == 0) {
                        size_t idx = (y * visionWidth_ + x) * 3;
                        data[idx] = 1.0f;     // Red
                        data[idx + 1] = 0.0f; // Green
                        data[idx + 2] = 0.0f; // Blue
                    }
                }
            }
            
            pattern.setData(data);
            SensoryPercept percept;
            percept.setVision(pattern);
            agent_->processSensoryInput(percept);
        }
    }
    
    void manipulateSynapses(float weight_factor, float probability) {
        """Manipulate synaptic weights for testing."""
        if (!brain_) return;
        
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            float currentWeight = stdp->getLTPWeight();
            stdp->setLTPWeight(currentWeight * weight_factor);
            stdp->setLTDWeight(currentWeight * weight_factor);
        }
        
        // Also modify all synapses
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (rand() % 100 < probability * 100) {
                    syn->addToWeight(weight_factor - 1.0f);
                }
            }
        }
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        """Force brain into a specific developmental stage."""
        if (brain_) {
            brain_->setDevelopmentalStage(stage);
        }
    }
    
    void pauseDevelopment(bool pause) {
        """Temporarily pause development."""
        if (agent_) {
            // This would require an additional API
            // For now, we can demonstrate the pattern
            if (pause) {
                std::cout << "Development paused (manual control)" << std::endl;
            } else {
                std::cout << "Development resumed" << std::endl;
            }
        }
    }
    
    std::string getBrainStatus() const {
        """Get comprehensive brain status."""
        if (!brain_) return "No brain available";
        
        std::stringstream status;
        status << "Brain Status:" << std::endl;
        status << "  Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        status << "  Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        status << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        status << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
        status << "  Avg firing rate: " << brain_->getAverageFiringRate() << std::endl;
        status << "  Development stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        
        auto* workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            status << "  Working memory traces: " << workingMem->getActiveTraces() << std::endl;
        }
        
        auto* episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            status << "  Episodic memory episodes: " << episodicMem->getEpisodeCount() << std::endl;
        }
        
        return status.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    size_t visionWidth_ = 256;
    size_t visionHeight_ = 256;
};

// Use it like this:
InteractiveBrainController controller(brain, agent);
controller.injectSensoryPattern("flash");
controller.manipulateSynapses(1.5f, 0.1f);  # Increase weights by 50% with 10% probability
controller.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);

std::cout << controller.getBrainStatus() << std::endl;
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
