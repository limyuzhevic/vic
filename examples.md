# NLM Examples

## Comprehensive Examples for All Skill Levels

This document provides comprehensive examples for the NLM (Neural Learning Machine) project, covering everything from basic usage to advanced features.

## Beginner Examples

### Example 1: Basic Brain Creation and Usage

```python
import nlm

# Create a simple brain
brain = nlm.PythonFactory.createBrain(nlm.PythonFactory.createDefaultConfig())
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete")
print(f"Total neurons: {brain.getStats()['total_neurons']}")
print(f"Total spikes: {brain.getStats()['total_spikes']}")
```

### Example 2: Brain with World Interaction

```python
import nlm

# Setup
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=200, regionCount=2)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=15, height=15, visionWidth=6, visionHeight=6)
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processObservations([("vision", [0.5, 0.3, 0.7])])
    brain.step(step)
    action = agent.generateAction()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Final firing rate: {brain.getStats()['average_firing_rate']:.3f}")
```

### Example 3: Simple Learning Agent

```python
import nlm
import random

# Create learning agent
brain = nlm.PythonFactory.createBrain(nlm.PythonFactory.createDefaultConfig())
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=10, height=10, visionWidth=4, visionHeight=4)
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)
agent.enableLearning(True)

# Learning simulation
for step in range(200):
    # Generate random observations
    vision = [random.random() for _ in range(16)]
    agent.processObservations([("vision", vision)])
    
    # Brain processes and acts
    brain.step(step)
    action = agent.generateAction("exploratory")
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = random.random() * 2.0 - 1.0  # Random reward
    agent.learnFromExperience(reward, step * 0.1)
    
    if step % 50 == 0:
        state = agent.getState()
        print(f"Step {step}: Curiosity={state['curiosity']:.2f}, Novelty={state['novelty']:.2f}")

print("Learning agent complete!")
```

### Example 4: Brain Checkpoint and Restoration

```python
import nlm

# Create and initialize brain
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=500)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

# Save checkpoint
brain_wrapper = nlm.PythonBrainWrapper(brain)
brain_wrapper.saveCheckpoint("initial_brain.pkl", "Initial brain state")

# Run simulation for a while
for step in range(100):
    brain.step(step)

# Load checkpoint
brain_wrapper = nlm.PythonBrainWrapper(brain)
brain_wrapper.saveCheckpoint("intermediate_brain.pkl", "After 100 steps")

# Load from intermediate checkpoint
intermediate_brain = nlm.PythonFactory.createBrain(config)
brain_wrapper = nlm.PythonBrainWrapper(intermediate_brain)
brain_wrapper.loadCheckpoint("intermediate_bbrain.pkl")

print("Checkpointing demo complete")
```

## Intermediate Examples

### Example 5: Advanced Configuration and Management

```python
import nlm
from nlm import ConfigTemplate, ConfigValidator

# Define custom configuration templates
templates = {
    "high_performance": ConfigTemplate(
        neuron_count=5000,
        region_count=5,
        connection_probability=0.15,
        plasticity_learning_rate=0.02
    ),
    
    "balanced": ConfigTemplate(
        neuron_count=1000,
        region_count=3,
        connection_probability=0.1,
        curiosity_baseline=0.5,
        novelty_threshold=0.5
    ),
    
    "learning_focused": ConfigTemplate(
        neuron_count=1000,
        plasticity_stdp_learning_rate=0.05,
        plasticity_hebbian_learning_rate=0.02,
        curiosity_baseline=0.8,
        enable_development=True
    )
}

# Create and validate configuration
config = templates["learning_focused"].create()
config = templates["balanced"].modify(config, neuron_count=1500)

# Validate configuration
validator = ConfigValidator()
errors = validator.validate_config(config)
if errors:
    print("Validation errors:", [e.message for e in errors])

# Use the configuration
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()
print("Advanced configuration demo complete")
```

### Example 6: Simulation Runner with Advanced Features

```python
import nlm
import time

# Create simulation runner
runner = nlm.SimulationRunner()

# Setup components
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=800, regionCount=3)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=30, height=30, visionWidth=10, visionHeight=10)

agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)
agent.enableLearning(True)
agent.enableCuriosity(True)

# Initialize runner
runner.initialize(brain, world, agent)
runner.setLearningMode(True)

# Run with performance monitoring
start_time = time.time()
results = runner.runWithLearning(500, verbose=True)
end_time = time.time()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average reward: {results['average_reward']:.3f}")
print(f"Average firing rate: {results['average_firing_rate']:.3f}")
print(f"Final spikes: {results['final_spikes']}")
```

### Example 7: Visualization and Debugging

```python
import nlm

# Create visualization interface
viz = nlm.VisualizationInterface()

// Initialize with options
viz.initialize({
    "update_rate": "30",
    "mode": "complete",
    "debug": "true"
})

// Create brain and setup
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=500)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

// Enable debug logging
viz.enableDebugMode(true)
viz.logEvent("brain_created", "Brain was successfully initialized", "INFO")

// Run simulation with visualization
for step in range(100):
    viz.update()
    viz.render()
    
    brain.step(step)
    
    if step % 20 == 0:
        viz.logEvent("step_complete", f"Completed step {step}", "INFO")
        viz.visualizeActivity(brain)

// Get debug information
debug_info = viz.getDebugInfo()
print(f"Debug info: {debug_info}")

// Export visualization data
viz.exportNetworkGraph("network_graph.json")
viz.exportActivityData("activity_data.csv")
viz.exportDebugLog("debug_log.json")

viz.close()
```

### Example 8: Configuration Management

```python
import nlm
from nlm import ConfigManager

// Create configuration manager
manager = ConfigManager()

// Create multiple configurations
learning_config = manager.create_config("learning_experiment", "learning_focused")
performance_config = manager.create_config("performance_test", "optimized")
realistic_config = manager.create_config("realistic_brain", "realistic")

// Validate configurations
learning_errors = manager.validate_config("learning_experiment")
performance_errors = manager.validate_config("performance_test")
realistic_errors = manager.validate_config("realistic_brain")

// Modify configurations
manager.modify_config("learning_experiment", {
    "brain": {"neuron_count": 2000},
    "world": {"width": 40, "height": 40}
})

// Compare configurations
comparison = manager.compare_configs("learning_experiment", "performance_test")

// Merge configurations
merged = manager.merge_configs(["learning_experiment", "performance_test"], "combined_config")

print("Configuration management demo complete")
```

## Advanced Examples

### Example 9: Multi-Agent System

```python
import nlm
import numpy as np

class MultiAgentSystem:
    def __init__(self, num_agents=3):
        self.agents = []
        self.brains = []
        self.worlds = []
        self.config = nlm.PythonFactory.createDefaultConfig()
        
        // Create agents with different configurations
        for i in range(num_agents):
            agent_config = self.config.copy()
            // Vary configuration for each agent
            agent_config.neuron_count = 300 + i * 100
            agent_config.region_count = 2 + i
            agent_config.connection_probability = 0.1 + i * 0.02
            
            brain = nlm.PythonFactory.createBrain(agent_config)
            brain.initialize()
            
            world = nlm.PythonFactory.createWorld()
            nlm.PythonFactory.setupEnvironment(world, width=20, height=20, visionWidth=6, visionHeight=6)
            
            agent = nlm.PythonFactory.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableLearning(True)
            
            self.agents.append(agent)
            self.brains.append(brain)
            self.worlds.append(world)
    
    def run_synchronized(self, num_steps=100):
        // Run all agents with synchronized updates
        for step in range(num_steps):
            // Update all worlds
            for world in self.worlds:
                world.update(0.1)
            
            // Process observations for all agents
            for i, agent in enumerate(self.agents):
                percept = self.worlds[i].getSensoryPercept()
                agent.processSensoryInput(percept)
            
            // Update all brains
            for i, brain in enumerate(self.brains):
                brain.step(step)
            
            // Generate actions for all agents
            for i, agent in enumerate(self.agents):
                action = agent.generateAction()
                self.worlds[i].applyMotorCommand(action, self.worlds[i].getSimulationTime())
            
            // Apply learning for all agents
            for i, agent in enumerate(self.agents):
                reward = self.calculateReward(i)
                agent.learnFromExperience(reward, step * 0.1)
    
    def calculateReward(self, agent_index):
        // Calculate reward based on agent performance
        world = self.worlds[agent_index]
        agent = self.agents[agent_index]
        
        // Simple reward calculation
        state = agent.getState()
        reward = state['curiosity'] * 0.5 + state['novelty'] * 0.3
        
        return reward

// Use the multi-agent system
system = MultiAgentSystem(3)
system.run_synchronized(200)
print("Multi-agent system demo complete")
```

### Example 10: Advanced ML Integration

```python
import nlm
import json
import pandas as pd
from sklearn.preprocessing import StandardScaler

class NLMMLIntegration:
    def __init__(self):
        self.model_path = "ml_models/nlm_neural_network.pkl"
        self.feature_extractor = None
        self.scaler = StandardScaler()
        self.training_data = []
    
    def extract_features(self, brain, world, agent):
        // Extract features from brain state
        features = {
            "firing_rate": brain.getStats()['average_firing_rate'],
            "excitatory_inhibition_ratio": brain.getStats()['excitatory_inhibition_ratio'],
            "total_spikes": brain.getStats()['total_spikes'],
            "curiosity_level": agent.getState()['curiosity'],
            "novelty_level": agent.getState()['novelty'],
            "prediction_error": agent.getState()['prediction_error'],
            "world_energy": world.getMaxEnergy(),
            "simulation_time": world.getSimulationTime(),
            "developmental_stage": agent.getState()['developmental_stage']
        }
        
        return features
    
    def prepare_training_data(self, num_episodes=1000):
        // Generate training data
        for episode in range(num_episodes):
            config = nlm.PythonFactory.createDefaultConfig()
            nlm.PythonFactory.configureBrain(config, neuronCount=500)
            
            brain = nlm.PythonFactory.createBrain(config)
            brain.initialize()
            
            world = nlm.PythonFactory.createWorld()
            nlm.PythonFactory.setupEnvironment(world, width=15, height=15)
            
            agent = nlm.PythonFactory.createAgentBrain(brain)
            agent.initialize(world)
            
            // Run episode
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.generateAction()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if step % 10 == 0:
                    features = self.extract_features(brain, world, agent)
                    self.training_data.append(features)
    
    def train_ml_model(self):
        // Prepare training data
        if not self.training_data:
            self.prepare_training_data()
        
        // Convert to DataFrame
        df = pd.DataFrame(self.training_data)
        
        // Scale features
        X = df.drop(['reward', 'action'], axis=1) if 'reward' in df.columns else df
        y = df['reward'] if 'reward' in df.columns else None
        
        X_scaled = self.scaler.fit_transform(X)
        
        // Train model (placeholder for actual ML training)
        print(f"Training ML model with {X_scaled.shape[0]} samples and {X_scaled.shape[1]} features")
        
        // Save model
        import pickle
        with open(self.model_path, 'wb') as f:
            pickle.dump({
                'scaler': self.scaler,
                'model_type': 'random_forest',  // Placeholder
                'feature_importance': np.random.rand(X_scaled.shape[1])
            }, f)
    
    def apply_ml_insights(self, brain, world, agent):
        // Apply ML model insights to brain behavior
        import pickle
        with open(self.model_path, 'rb') as f:
            model_data = pickle.load(f)
        
        // Extract features
        features = self.extract_features(brain, world, agent)
        features_scaled = self.scaler.transform([features])
        
        // Get predictions
        predictions = model_data['model'].predict(features_scaled)
        feature_importance = model_data['feature_importance']
        
        // Apply insights to neuromodulation
        for i, feature in enumerate(features.keys()):
            if feature_importance[i] > 0.5:  // High importance features
                // Adjust neuromodulation based on feature importance
                if feature == "curiosity_level":
                    agent.setCuriosityLevel(feature_importance[i])
                elif feature == "prediction_error":
                    agent.setPredictionError(feature_importance[i])
    
    def run_ml_enhanced_simulation(self, num_steps=500):
        // Run simulation with ML enhancements
        config = nlm.PythonFactory.createDefaultConfig()
        nlm.PythonFactory.configureBrain(config, neuronCount=600)
        
        brain = nlm.PythonFactory.createBrain(config)
        brain.initialize()
        
        world = nlm.PythonFactory.createWorld()
        nlm.PythonFactory.setupEnvironment(world, width=25, height=25)
        
        agent = nlm.PythonFactory.createAgentBrain(brain)
        agent.initialize(world)
        
        // Train ML model if not already trained
        if not hasattr(self, 'model_trained') or not self.model_trained:
            self.train_ml_model()
            self.model_trained = True
        
        // Run simulation
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.generateAction()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            // Apply ML insights every 50 steps
            if step % 50 == 0:
                self.apply_ml_insights(brain, world, agent)
            
            if step % 100 == 0:
                features = self.extract_features(brain, world, agent)
                print(f"Step {step}: {features}")
        
        return brain, agent, world

// Use the ML integration
ml_integration = NLMMLIntegration()
brain, agent, world = ml_integration.run_ml_enhanced_simulation(300)
print("ML integration demo complete")
```

### Example 11: Real-Time Visualization Server

```python
import nlm
import threading
import time
import json
from http.server import HTTPServer, BaseHTTPRequestHandler
import socketserver

class VisualizationServer(BaseHTTPRequestHandler):
    def __init__(self, *args, viz_interface=None, **kwargs):
        self.viz = viz_interface
        super().__init__(*args, **kwargs)
    
    def do_GET(self):
        if self.path == "/api/debug-info":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            
            if self.viz:
                debug_info = self.viz.getDebugInfo()
                self.wfile.write(json.dumps(debug_info, indent=2).encode())
        
        elif self.path == "/api/stats":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            
            if self.viz:
                stats = {
                    "visualization_active": self.viz.isActive(),
                    "update_rate": self.viz.getUpdateRate(),
                    "mode": str(self.viz.getVisualizationMode())
                }
                self.wfile.write(json.dumps(stats, indent=2).encode())
        
        elif self.path == "/api/control":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            
            self.wfile.write(json.dumps({
                "status": "server_running",
                "endpoints": ["/api/debug-info", "/api/stats", "/api/control"]
            }, indent=2).encode())
        
        else:
            self.send_response(404)
            self.end_headers()
    
    def do_POST(self):
        if self.path == "/api/control":
            content_length = int(self.headers['Content-Length'])
            post_data = self.r.read(content_length).decode('utf-8')
            
            try:
                command = json.loads(post_data)
                
                if command.get("action") == "set_mode":
                    mode_str = command.get("mode", "complete")
                    mode_map = {
                        "network": nlm.VisualizationMode.Network,
                        "activity": nlm.VisualizationMode.Activity,
                        "spikes": nlm.VisualizationMode.Spikes,
                        "weights": nlm.VisualizationMode.Weights,
                        "learning": nlm.VisualizationMode.Learning,
                        "memory": nlm.VisualizationMode.Memory,
                        "neuromodulation": nlm.VisualizationMode.Neuromodulation,
                        "development": nlm.VisualizationMode.Development,
                        "complete": nlm.VisualizationMode.Complete
                    }
                    
                    if self.viz:
                        self.viz.setVisualizationMode(mode_map.get(mode_str, nlm.VisualizationMode.Complete))
                
                elif command.get("action") == "toggle_debug":
                    if self.viz:
                        self.viz.enableDebugMode(command.get("enable", True))
                
                elif command.get("action") == "update_rate":
                    if self.viz:
                        self.viz.setUpdateRate(command.get("rate", 30.0))
                
                response = {"status": "success", "message": f"Executed: {command.get('action')}"}
                self.send_response(200)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(json.dumps(response, indent=2).encode())
                
            except json.JSONDecodeError:
                self.send_response(400)
                self.end_headers()

def start_visualization_server(brain, world, agent, port=8080):
    // Create visualization interface
    viz = nlm.VisualizationInterface()
    viz.initialize({
        "update_rate": "30",
        "mode": "complete",
        "debug": "true"
    }
    
    // Start server in background thread
    server = HTTPServer(("", port), lambda *args: VisualizationServer(*args, viz_interface=viz))
    
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True
    server_thread.start()
    
    print(f"Visualization server started on http://localhost:{port}")
    
    // Enable debug logging
    viz.enableDebugMode(true)
    viz.logEvent("server_started", f"Visualization server started on port {port}", "INFO")
    
    // Run simulation
    for step in range(200):
        viz.update()
        viz.render()
        
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.generateAction()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            viz.logEvent("step_complete", f"Completed step {step}", "INFO")
    
    // Shutdown
    server.shutdown()
    viz.close()
    print("Visualization server shutdown complete")

// Create components and start server
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=500)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=20, height=20)

agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)
agent.enableLearning(True)

start_visualization_server(brain, world, agent, 8080)
```

## Usage Tips

### For Beginners:
1. Start with simple examples (Examples 1-3)
2. Use factory functions (`nlm.PythonFactory.createBrain()`, `nlm.PythonFactory.createWorld()`)
3. Enable learning gradually (`agent.enableLearning(True)`)
4. Start with small configurations (fewer neurons, smaller world)
5. Use the enhanced easy usage guide (`easy_usage_enhanced.md`)

### For Intermediate Users:
1. Try advanced configuration and management (Example 5)
2. Use simulation runner for complex simulations (Example 6)
3. Explore visualization and debugging (Example 7)
4. Learn configuration templates and validation
5. Experiment with multi-agent systems (Example 9)

### For Advanced Users:
1. Implement ML integration (Example 10)
2. Create real-time visualization servers (Example 11)
3. Use configuration versioning and management
4. Implement custom learning algorithms
5. Explore performance optimization

## Troubleshooting

### Common Issues:
1. **Brain not thinking**: Ensure `brain.initialize()` is called
2. **Agent not acting**: Check `world.applyMotorCommand()` is called
3. **No learning**: Enable learning with `agent.enableLearning(True)`
4. **Visualization not working**: Check `viz.initialize()` and ensure `viz.render()` is called
5. **Configuration errors**: Use `ConfigValidator` to check configuration

### Performance Tips:
1. Use smaller configurations for testing
2. Disable unnecessary visualization features
3. Use appropriate simulation timestep
4. Monitor and optimize memory usage
5. Use configuration templates for common setups

### Best Practices:
1. Always validate configurations before use
2. Use factory functions for consistent setup
3. Implement proper error handling
4. Use checkpointing for long simulations
5. Document your configuration choices

## Conclusion

This comprehensive examples guide provides NLM users with practical code examples for all skill levels. From basic brain creation to advanced ML integration and real-time visualization servers, these examples demonstrate the full capabilities of the enhanced NLM Python bindings.

Key takeaways:
1. Start simple and gradually add complexity
2. Use factory functions and templates for consistency
3. Leverage visualization for debugging and analysis
4. Implement proper configuration management
5. Explore advanced features as you become more comfortable

The examples showcase NLM's flexibility, from simple neural simulations to complex multi-agent systems with machine learning integration. Each example builds on the previous ones, allowing you to progressively master the system's capabilities.

Remember to always:
- Initialize components before use
- Enable learning when needed
- Validate configurations
- Use appropriate visualization for debugging
- Document your experiments and results

Happy experimenting with NLM!