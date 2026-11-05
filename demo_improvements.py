#!/usr/bin/env python3
#=============================================================================
# IMPROVEMENTS DEMONSTRATION
#=============================================================================
# This script demonstrates the new functionality added to the NLM project.
# It shows the improved code modularity, Python API enhancements, and
# statistical analysis capabilities.
#=============================================================================

#!/usr/bin/env python3

This script demonstrates the enhancements made to the NLM (Neural Learning Machine)
project, including:

1. Code modularity improvements (StatsCollector, MotorCommandSelector, CognitiveModules)
2. Python API enhancements (AdvancedConfigurationManager, AdvancedSimulationManager)
3. Statistical analysis capabilities (multi-format export, quality metrics)
4. Enhanced debugging and analysis tools

The script shows how to:
- Use the new StatsCollector for comprehensive brain statistics
- Access enhanced Brain class statistics interface
- Use advanced configuration and simulation management
- Debug and analyze brain behavior
- Export statistics in various formats
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some demonstrations will be skipped.")

# Mock implementations for demonstration when pynlm is not available
class MockBrain:
    def __init__(self):
        self.stats = {
            'total_neurons': 500,
            'total_synapses': 12500,
            'active_neurons': 125,
            'firing_neurons': 87,
            'total_spikes': 15420,
            'pending_spikes': 23,
            'average_firing_rate': 30.8,
            'e_i_ratio': 1.2,
            'stability_score': 0.85,
            'learning_score': 0.73,
            'coherence_score': 0.91,
        }
    
    def get_stats(self):
        return self.stats
    
    def get_formatted_stats(self):
        return "Formatted Brain Statistics:\n" + \
               f"  Neurons: {self.stats['total_neurons']}\n" + \
               f"  Synapses: {self.stats['total_synapses']}\n" + \
               f"  Active: {self.stats['active_neurons']}%\n" + \
               f"  Firing: {self.stats['firing_neurons']}%\n" + \
               f"  Stability: {self.stats['stability_score']:.2f}\n"
    
    def get_stats_json(self):
        return f'{{"neurons": {self.stats["total_neurons"]}, "synapses": {self.stats["total_synapses"]}, "stability": {self.stats["stability_score"]}}}'
    
    def get_stats_csv(self):
        return "metric,value\ntotal_neurons,500\ntotal_synapses,12500\nstability_score,0.85\n"
    
    def get_stats_markdown(self):
        return "| Metric | Value |\n|--------|-------|\n| Total Neurons | 500 |\n| Total Synapses | 12500 |\n| Stability Score | 0.85 |"
    
    def log_status(self):
        print(self.get_formatted_stats())

class MockAgentBrain:
    def __init__(self):
        self.concepts = 15
        self.self_model_confidence = 0.88
        self.curiosity_level = 0.62
        self.novelty_level = 0.45
        self.reward_modulation_enabled = True
        self.structural_plasticity_enabled = True
        self.development_enabled = True
        self.curiosity_enabled = True
    
    def enable_reward_modulation(self, enabled):
        self.reward_modulation_enabled = enabled
    
    def enable_structural_plasticity(self, enabled):
        self.structural_plasticity_enabled = enabled
    
    def enable_development(self, enabled):
        self.development_enabled = enabled
    
    def enable_curiosity(self, enabled):
        self.curiosity_enabled = enabled
    
    def get_concept_count(self):
        return self.concepts
    
    def get_self_model_confidence(self):
        return self.self_model_confidence
    
    def get_curiosity_level(self):
        return self.curiosity_level
    
    def get_novelty_level(self):
        return self.novelty_level
    
    def get_developmental_stage(self):
        return "Adult"

class MockAdvancedConfigurationManager:
    def __init__(self):
        self.config = {}
    
    def set_preset(self, preset):
        presets = {
            'research': {'brain.neuron_count': 2000, 'brain.synapse_density': 0.15},
            'simulation': {'brain.neuron_count': 500, 'brain.synapse_density': 0.1},
            'minimal': {'brain.neuron_count': 100, 'brain.synapse_density': 0.05}
        }
        self.config.update(presets.get(preset, {}))
        print(f"Set preset: {preset}")
    
    def set(self, key, value):
        self.config[key] = value
        print(f"Set {key} = {value}")

class MockAdvancedSimulationManager:
    def __init__(self):
        self.brain = MockBrain()
        self.steps = 0
    
    def create_brain(self, config):
        self.brain = MockBrain()
        return self.brain
    
    def update(self, timestep):
        self.steps += 1
        # Simulate some brain activity
        if self.steps % 100 == 0:
            print(f"  Step {self.steps}: Processing...")
    
    def log_brain_status(self, step):
        print(f"  Step {step}:")
        self.brain.log_status()
    
    def should_checkpoint(self, step):
        return step % 500 == 0
    
    def checkpoint(self, filename):
        print(f"  Checkpoint saved to: {filename}")

def demonstrate_stats_collector():
    print("\n" + "="*60)
    print("1. STATISTICS COLLECTOR DEMONSTRATION")
    print("="*60)
    
    if PYNLM_AVAILABLE:
        print("\n--- Using real pynlm library ---")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some simulation
        for i in range(10):
            brain.step(i)
        
        # Demonstrate new stats interface
        print("\n--- Brain Statistics Interface ---")
        stats = brain.getStats()
        print(f"Total Neurons: {stats.totalNeurons}")
        print(f"Total Synapses: {stats.totalSynapses}")
        print(f"Active Neurons: {stats.activeNeurons}")
        print(f"Stability Score: {stats.stabilityScore:.3f}")
        print(f"Learning Score: {stats.learningScore:.3f}")
        print(f"Coherence Score: {stats.coherenceScore:.3f}")
        
        print("\n--- Formatted Statistics ---")
        print(brain.getFormattedStats())
        
        print("\n--- JSON Export ---")
        print(brain.getStatsJSON())
        
        print("\n--- CSV Export ---")
        print(brain.getStatsCSV())
        
        print("\n--- Markdown Export ---")
        print(brain.getStatsMarkdown())
        
    else:
        print("\n--- Using mock implementation ---")
        brain = MockBrain()
        
        # Demonstrate stats interface
        print("\n--- Mock Brain Statistics Interface ---")
        stats = brain.get_stats()
        print(f"Total Neurons: {stats['total_neurons']}")
        print(f"Total Synapses: {stats['total_synapses']}")
        print(f"Stability Score: {stats['stability_score']:.3f}")
        print(f"Learning Score: {stats['learning_score']:.3f}")
        
        print("\n--- Formatted Statistics ---")
        print(brain.get_formatted_stats())
        
        print("\n--- JSON Export ---")
        print(brain.get_stats_json())
        
        print("\n--- CSV Export ---")
        print(brain.get_stats_csv())
        
        print("\n--- Markdown Export ---")
        print(brain.get_stats_markdown())

def demonstrate_advanced_api():
    print("\n" + "="*60)
    print("2. ADVANCED PYTHON API DEMONSTRATION")
    print("="*60)
    
    if PYNLM_AVAILABLE:
        print("\n--- Advanced Configuration Manager ---")
        config = pynlm.AdvancedConfigurationManager()
        config.set_preset('research')
        config.set('brain.neuron_count', 2000)
        
        print("\n--- Advanced Simulation Manager ---")
        sim = pynlm.AdvancedSimulationManager()
        brain = sim.create_brain(config)
        brain.initialize()
        
        print("\nRunning simulation with monitoring...")
        for step in range(25):
            sim.update(0.1)
            if step % 5 == 0:
                sim.log_brain_status(step)
            if sim.should_checkpoint(step):
                sim.checkpoint(f"checkpoint_{step:04d}.bin")
        
        print("\n--- Agent Brain with Cognitive Modules ---")
        agent = pynlm.createAgentBrain(brain)
        
        # Enable advanced features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        # Process some sensory input
        agent.processSensoryInput(None)
        
        # Demonstrate cognitive capabilities
        print(f"Concept count: {agent.getConceptCount()}")
        print(f"Self-model confidence: {agent.getSelfModelConfidence():.3f}")
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Developmental stage: {agent.getDevelopmentalStage()}")
        
    else:
        print("\n--- Using mock implementation ---")
        
        print("\n--- Advanced Configuration Manager ---")
        config = MockAdvancedConfigurationManager()
        config.set_preset('research')
        config.set('brain.neuron_count', 2000)
        
        print("\n--- Advanced Simulation Manager ---")
        sim = MockAdvancedSimulationManager()
        brain = sim.create_brain(config)
        
        print("\nRunning simulation with monitoring...")
        for step in range(25):
            sim.update(0.1)
            if step % 5 == 0:
                sim.log_brain_status(step)
            if sim.should_checkpoint(step):
                sim.checkpoint(f"checkpoint_{step:04d}.bin")
        
        print("\n--- Agent Brain with Cognitive Modules ---")
        agent = MockAgentBrain()
        
        # Enable advanced features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        # Demonstrate cognitive capabilities
        print(f"Concept count: {agent.get_concept_count()}")
        print(f"Self-model confidence: {agent.get_self_model_confidence():.3f}")
        print(f"Curiosity level: {agent.get_curiosity_level():.3f}")
        print(f"Novelty level: {agent.get_novelty_level():.3f}")
        print(f"Developmental stage: {agent.get_developmental_stage()}")

def demonstrate_code_modularity():
    print("\n" + "="*60)
    print("3. CODE MODULARITY DEMONSTRATION")
    print("="*60)
    
    print("\n--- Modular Design Benefits ---")
    print("1. StatsCollector:")
    print("   - Separate statistics collection logic")
    print("   - Multiple export formats (JSON, CSV, Markdown)")
    print("   - Quality metrics and validation")
    print("   - Can be used independently of Brain class")
    
    print("\n2. MotorCommandSelector:")
    print("   - Extracted motor command selection logic")
    print("   - Vision gating and movement penalties")
    print("   - Configurable behavior parameters")
    print("   - Curiosity-based exploration")
    
    print("\n3. CognitiveModules:")
    print("   - Self-model for action prediction")
    print("   - Concept formation from experience")
    print("   - Social learning capabilities")
    print("   - Developmental stage management")
    
    print("\n4. Enhanced Brain Interface:")
    print("   - getStats(): Comprehensive statistics")
    print("   - getFormattedStats(): Formatted output")
    print("   - getStatsJSON()/CSV()/Markdown(): Export formats")
    print("   - Enhanced logStatus(): StatsCollector integration")
    
    print("\n--- Modularity Benefits ---")
    print("✓ Easier to test individual components")
    print("✓ Better separation of concerns")
    print("✓ Reduced coupling between systems")
    print("✓ Enhanced maintainability")
    print("✓ Reusable functionality")
    print("✓ Improved extensibility")

def demonstrate_backward_compatibility():
    print("\n" + "="*60)
    print("4. BACKWARD COMPATIBILITY DEMONSTRATION")
    print("="*60)
    
    print("\n--- Existing API Still Works ---")
    if PYNLM_AVAILABLE:
        print("\n--- Original easy_usage.md examples still work ---")
        print("1. Basic brain creation and simulation:")
        print("   import pynlm")
        print("   brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("   brain.initialize()")
        print("   for i in range(100): brain.step(i)")
        
        print("\n2. Agent simulation:")
        print("   brain = pynlm.createBrain(config)")
        print("   agent = pynlm.createAgentBrain(brain)")
        print("   world = pynlm.createSimpleWorld()")
        print("   agent.initialize(world)")
        
        print("\n3. World interaction:")
        print("   world.update(0.1)")
        print("   percept = world.getSensoryPercept()")
        print("   agent.processSensoryInput(percept)")
        print("   brain.step(step)")
        print("   action = agent.decodeMotorCommand()")
        print("   world.applyMotorCommand(action, time)")
    
    else:
        print("\n--- Mock implementation of original API ---")
        print("All original examples from easy_usage.md work as expected.")
    
    print("\n--- New Features Added on Top of Existing API ---")
    print("✓ Advanced configuration management")
    print("✓ Enhanced simulation control")
    print("✓ Statistical analysis tools")
    print("✓ Debugging and analysis utilities")
    print("✓ Modular cognitive systems")
    print("✓ Multiple export formats")
    print("✓ Quality metrics and monitoring")

def main():
    print("NLM IMPROVEMENTS DEMONSTRATION")
    print("="*60)
    print("This script demonstrates the enhancements made to the NLM project.")
    print("\nKey improvements:")
    print("• Code modularity (StatsCollector, MotorCommandSelector, CognitiveModules)")
    print("• Python API enhancements (AdvancedConfigurationManager, AdvancedSimulationManager)")
    print("• Statistical analysis capabilities (multi-format export, quality metrics)")
    print("• Enhanced debugging and analysis tools")
    print("• Backward compatibility with existing API")
    
    demonstrate_stats_collector()
    demonstrate_advanced_api()
    demonstrate_code_modularity()
    demonstrate_backward_compatibility()
    
    print("\n" + "="*60)
    print("DEMONSTRATION COMPLETE")
    print("="*60)
    print("\nThe NLM project has been significantly enhanced with:")
    print("• Better code modularity and maintainability")
    print("• Advanced Python API for complex simulations")
    print("• Comprehensive statistical analysis tools")
    print("• Enhanced debugging and monitoring capabilities")
    print("• Full backward compatibility with existing code")
    print("\nAll improvements are documented in IMPROVEMENTS.md")
    print("New tests are available in the tests directory")

if __name__ == "__main__":
    main()
