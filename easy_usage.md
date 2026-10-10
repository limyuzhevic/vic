---

## Advanced Features

For more advanced usage, the NLM framework includes:

### 1. Neural Development
```python
# Brains change over time
agent.enableDevelopment(True)
print(f"Current stage: {brain.getDevelopmentalStage()}")
```

### 2. Learning from Rewards
```python
# Brains learn from success/failure
agent.applyRewardModulation(10.0, 0.0)  # Positive reward
agent.applyRewardModulation(-5.0, 0.0) # Negative reward
```

### 3. Curiosity and Exploration
```python
# Brains explore what's novel
agent.enableCuriosity(True)
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### 4. Memory Systems
```python
# Brains store and recall experiences
print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
```

### 5. Complex Behaviors
```python
# Create brains with specific personalities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.2)
brain = pynlm.createBrain(config)
brain.initialize()
```

### 6. Experiment Framework
```python
# Access built-in experiment systems
from nlm.experiments import Phase3Demo, Phase4Demo, Phase6IntegratedExperiment
```

### 7. Checkpoint and Save/Load
```python
# Save and restore brain states
brain.save("my_brain_checkpoint.bin")
# ... later ...
brain.load("my_brain_checkpoint.bin")
```