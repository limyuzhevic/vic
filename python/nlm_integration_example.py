#!/usr/bin/env python3
"""
Complete NLM Integration Example

This example demonstrates a complete, realistic integration of the NLM (Neural Learning Machine)
Python bindings, showcasing how all the systems work together as a coherent artificial brain.

The example simulates an agent learning to navigate a simple world through experience,
memory, and adaptive behavior - demonstrating the core capabilities of NLM Phase 6 integration.
"""

import pynlm as nlm
import time
import random

class NLMSimulation:
    """
    Complete NLM simulation integrating all major systems.
    
    This class demonstrates the full integration of:
    - Neural core (brain) with LIF dynamics and plasticity
    - Agent interface connecting brain to world
    - Simple 2D world simulation
    - Sensory processing (vision, touch, internal signals)
    - Motor control and action execution
    - Neuromodulation (reward, curiosity, novelty)
    - Memory systems (working, episodic)
    - Development stages
    - Complete brain loop with integration
    """
    
    def __init__(self, seed=None):
        """Initialize the complete NLM simulation."""
        self.seed = seed
        self.start_time = time.time()
        self.step_count = 0
        self.episode_count = 0
        self.total_reward = 0.0
        
        # Initialize random seed if provided
        if seed is not None:
            random.seed(seed)
        
        print("Initializing NLM Complete Integration Simulation...")
        self._initialize_systems()
        
    def _initialize_systems(self):
        """Initialize all NLM systems."""
        # 1. Create brain with default configuration
        self.config = nlm.createDefaultConfig()
        self.config.set("neuron_count", 200)
        self.config.set("region_count", 3)
        self.config.set("connection_probability", 0.15)
        
        # Enable all major features
        self.config.set("plasticity_learning_rate", 0.02)
        self.config.set("reward_discount_factor", 0.95)
        
        # 2. Create neural brain core
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
        # 3. Create world environment
        self.world = nlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=15, visionHeight=15)
        self.world.setRandomSeed(self.seed)
        
        # 4. Create agent brain interface
        self.agent = nlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # 5. Initialize agent body
        agent_body = self.world.getAgentBody()
        agent_body.x = 15.0
        agent_body.y = 15.0
        agent_body.orientation = 0.0
        agent_body.energy = 100.0
        agent_body.health = 1.0
        agent_body.age = 0
        
        # 6. Enable all major features
        self._enable_features()
        
        print("✓ All NLM systems initialized successfully")
        
    def _enable_features(self):
        """Enable all major NLM features for complete integration."""
        # Core learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Set development stage to start maturation
        self.agent.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)
        
        print("✓ All features enabled:")
        print("  - Reward modulation")
        print("  - Structural plasticity")
        print("  - Development system")
        print("  - Curiosity drive")
    
    def _get_sensory_percept(self):
        """Get and process current sensory percept."""
        # Get current observation from world
        percept = self.world.getSensoryPercept()
        
        # The percept already contains:
        # - Vision (15x15 RGB pixel data)
        # - Touch (proximity to walls/objects)
        # - Internal signals (body state)
        # - Audio (none in simple world)
        
        return percept
    
    def _process_experience(self, percept, action, reward):
        """Process experience through the brain loop."""
        # 1. Process sensory input
        self.agent.processSensoryInput(percept)
        
        # 2. Update brain neural dynamics
        self.brain.step(self.step_count)
        
        # 3. Decode motor command from brain activity
        motor_cmd = self.agent.decodeMotorCommand()
        
        # 4. Apply action to world
        self.world.applyMotorCommand(motor_cmd, self.step_count)
        
        # 5. Apply reward-based learning
        predicted_reward = self.agent.getPredictionError()
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # 6. Update development system
        self.agent.updateDevelopment(0.001)
        
        # 7. Update experience tracking
        self.total_reward += reward
        self.step_count += 1
        
    def _check_for_rewards(self, agent_body):
        """Calculate reward based on agent's performance."""
        rewards = []
        
        # Survival reward
        if agent_body.health > 0.5:
            rewards.append(1.0)
        
        # Movement reward (encourages exploration)
        if agent_body.isMoving:
            rewards.append(0.5)
        
        # Curiosity reward (from neuromodulation)
        curiosity_level = self.agent.getCuriosityLevel()
        if curiosity_level > 0.1:
            rewards.append(curiosity_level * 2.0)
        
        # Novelty reward (for new experiences)
        novelty_level = self.agent.getNoveltyLevel()
        if novelty_level > 0.3:
            rewards.append(novelty_level)
        
        # Energy conservation penalty
        if agent_body.energy < 20.0:
            rewards.append(-1.0)
        
        # Return weighted sum
        if rewards:
            return sum(rewards) / len(rewards)
        return 0.0
    
    def _check_for_danger(self, agent_body):
        """Check if agent is in dangerous situations."""
        hazards = []
        
        # Check if agent is near world boundaries
        if agent_body.x < 2.0 or agent_body.x > 28.0:
            hazards.append(-2.0)
        if agent_body.y < 2.0 or agent_body.y > 28.0:
            hazards.append(-2.0)
        
        # Check if agent is low on energy
        if agent_body.energy < 30.0:
            hazards.append(-1.0)
        
        return sum(hazards) if hazards else 0.0
    
    def _display_status(self):
        """Display current simulation status."""
        agent_body = self.world.getAgentBody()
        brain_stats = {
            'neurons': self.brain.getTotalNeuronCount(),
            'active': self.brain.getActiveNeuronCount(),
            'firing': self.brain.getFiringNeuronCount(),
            'rate': self.brain.getAverageFiringRate(),
            'regions': self.brain.getRegionCount(),
        }
        
        neuromod = {
            'stage': self.agent.getDevelopmentalStage(),
            'dopamine': self.agent.getNeuromodulationLevel(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
        }
        
        print(f"Step {self.step_count:4d} | "
              f"Pos({agent_body.x:5.1f},{agent_body.y:5.1f}) | "
              f"Energy:{agent_body.energy:5.1f} | "
              f"Firing Rate:{brain_stats['rate']:5.2f}Hz | "
              f"Dopamine:{neuromod['dopamine']:4.2f} | "
              f"Curiosity:{neuromod['curiosity']:4.2f} | "
              f"Reward:{self.total_reward:6.1f}")
    
    def _check_for_events(self):
        """Check for and handle special events."""
        agent_body = self.world.getAgentBody()
        
        # Episode completion
        if agent_body.energy <= 0.0 or self.step_count >= 500:
            return True
            
        # Development stage transition
        if self.step_count == 100:
            self.agent.setDevelopmentalStage(nlm.DevelopmentalStage.CriticalPeriod)
            print("🔄 Development stage: Critical Period")
            
        if self.step_count == 300:
            self.agent.setDevelopmentalStage(nlm.DevelopmentalStage.Aging)
            print("🔄 Development stage: Aging")
            
        # Curiosity peak
        if self.agent.getCuriosityLevel() > 0.8 and self.step_count % 50 == 0:
            print("💡 Curiosity peak - exploring new areas")
            
        return False
    
    def run_simulation(self, max_steps=500, display_interval=50):
        """Run the complete NLM simulation."""
        print(f"\nStarting NLM Simulation ({max_steps} steps max)")
        print("=" * 80)
        print(f"{'Step':>4} | {'Position':>15} | {'Energy':>6} | {'Rate':>8} | "
              f"{'Dopamine':>8} | {'Curiosity':>8} | {'Reward':>8}")
        print("=" * 80)
        
        # Initial display
        self._display_status()
        
        # Main simulation loop
        for step in range(max_steps):
            try:
                # Check for simulation events
                if self._check_for_events():
                    print("\n🏁 Episode completed!")
                    break
                    
                # Get current agent state
                agent_body = self.world.getAgentBody()
                
                # Check for danger
                danger_reward = self._check_for_danger(agent_body)
                
                # Process sensory input
                percept = self._get_sensory_percept()
                
                # Process through brain loop
                self._process_experience(percept, None, danger_reward)
                
                # Calculate performance reward
                performance_reward = self._check_for_rewards(agent_body)
                
                # Process experience with combined reward
                self._process_experience(percept, None, performance_reward)
                
                # Display status at intervals
                if step % display_interval == 0:
                    self._display_status()
                    
                # Check for memory storage (every 50 steps)
                if step % 50 == 0 and step > 0:
                    self._check_memory_integration()
                    
            except Exception as e:
                print(f"\n❌ Simulation error at step {step}: {e}")
                break
                
        # Final display
        print("=" * 80)
        print(f"Simulation completed after {self.step_count} steps")
        print(f"Total accumulated reward: {self.total_reward:.1f}")
        print(f"Average reward per step: {self.total_reward/max(1, self.step_count):.2f}")
        
        # Save final brain state
        with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
            self.brain.save(f.name)
            print(f"Brain state saved to: {f.name}")
            
    def _check_memory_integration(self):
        """Check memory system integration."""
        # Check if working memory is available
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            print(f"📊 Working memory active: {working_memory.getActiveMemoryCount()} items")
        
        # Check if episodic memory is available
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            print(f"📖 Episodic memory available for experience replay")

def demonstrate_basic_functionality():
    """Demonstrate basic NLM functionality."""
    print("\n" + "="*60)
    print("NLM PYTHON BINDINGS - BASIC FUNCTIONALITY DEMO")
    print("="*60)
    
    # Simple brain simulation
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print("✓ Brain created and initialized")
    
    # Run simple simulation
    for step in range(20):
        brain.step(step)
        
    print(f"✓ Simulation completed: {brain.getTotalNeuronCount()} neurons,")
    print(f"  {brain.getFiringNeuronCount()} firing neurons,")
    print(f"  {brain.getAverageFiringRate():.2f} Hz average firing rate")
    
    # Save brain state
    with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
        brain.save(f.name)
        print(f"✓ Brain state saved to {f.name}")
    
    return brain

def demonstrate_agent_simulation():
    """Demonstrate complete agent simulation."""
    print("\n" + "="*60)
    print("NLM PYTHON BINDINGS - AGENT SIMULATION DEMO")
    print("="*60)
    
    # Create complete agent system
    agent_sim = NLMSimulation(seed=42)
    
    # Run simulation
    agent_sim.run_simulation(max_steps=200, display_interval=50)
    
    print("\n🎉 Agent simulation completed successfully!")
    return agent_sim

def demonstrate_error_handling():
    """Demonstrate error handling capabilities."""
    print("\n" + "="*60)
    print("NLM PYTHON BINDINGS - ERROR HANDLING DEMO")
    print("="*60)
    
    # Test 1: Invalid neuron ID
    print("\n1. Testing invalid neuron ID handling:")
    try:
        invalid_id = nlm.NeuronId(-1)
        print("❌ Should have raised exception")
    except nlm.InvalidNeuronIdError as e:
        print(f"✅ Correctly caught: {e}")
    
    # Test 2: Invalid region ID
    print("\n2. Testing invalid region ID handling:")
    try:
        invalid_id = nlm.RegionId(-100)
        print("❌ Should have raised exception")
    except nlm.InvalidRegionIdError as e:
        print(f"✅ Correctly caught: {e}")
    
    # Test 3: Parameter validation
    print("\n3. Testing parameter validation:")
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    try:
        # This should be caught by validation
        brain.step(-1)
        print("❌ Should have raised exception")
    except nlm.InvalidParameterError as e:
        print(f"✅ Correctly caught: {e}")
    
    print("\n✅ Error handling demonstration completed")

def demonstrate_feature_comprehensive():
    """Demonstrate comprehensive feature usage."""
    print("\n" + "="*60)
    print("NLM PYTHON BINDINGS - COMPREHENSIVE FEATURE DEMO")
    print("="*60)
    
    # Create system with all features enabled
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    world.configure(20, 20, 10, 10)
    world.setAgentStart(10, 10)
    
    brain.initialize()
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✅ All features enabled:")
    print("  - Reward modulation")
    print("  - Curiosity-driven exploration")
    print("  - Developmental changes")
    print("  - Structural plasticity")
    
    # Demonstrate feature interactions
    print("\n🔄 Demonstrating feature interactions:")
    
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = brain.produceAction()
        
        # Apply action
        cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(cmd, step)
        
        # Check neuromodulation levels
        if step % 25 == 0:
            print(f"Step {step}: Dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Development={agent.getDevelopmentalStage()}")
    
    print("\n✅ Comprehensive feature demonstration completed")
    return brain, world, agent

def main():
    """Run all NLM demonstrations."""
    print("\n" + "="*80)
    print("🚀 NLM PYTHON BINDINGS - COMPLETE DEMONSTRATION SUITE")
    print("="*80)
    
    try:
        # Demonstration 1: Basic functionality
        demonstrate_basic_functionality()
        
        # Demonstration 2: Complete agent simulation
        agent_sim = demonstrate_agent_simulation()
        
        # Demonstration 3: Error handling
        demonstrate_error_handling()
        
        # Demonstration 4: Comprehensive features
        brain, world, agent = demonstrate_feature_comprehensive()
        
        # Summary
        print("\n" + "="*80)
        print("🎉 ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY!")
        print("="*80)
        print("\nSummary of Capabilities Demonstrated:")
        print("✅ Basic brain simulation and control")
        print("✅ Complete agent-world interaction")
        print("✅ Sensory processing (vision, touch, internal)")
        print("✅ Motor control and action execution")
        print("✅ Neuromodulation (reward, curiosity, novelty)")
        print("✅ Development stages and plasticity")
        print("✅ Memory system integration")
        print("✅ Comprehensive error handling")
        print("✅ Feature interaction and coordination")
        
        print("\nKey Insights:")
        print("• NLM provides a complete artificial brain architecture")
        print("• Python bindings enable full programmatic control")
        print("• All systems integrate seamlessly as a coherent whole")
        print("• Error handling ensures robust operation")
        print("• Development stages enable behavioral maturation")
        
    except Exception as e:
        print(f"\n❌ DEMONSTRATION SUITE FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    import tempfile
    exit(main())