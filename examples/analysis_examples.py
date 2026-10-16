"""Analysis and visualization examples."""

import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from pynlm.enhanced import (
    create_enhanced_brain,
    create_enhanced_agent,
    create_enhanced_world,
    create_enhanced_config,
    VisualizationTools
)

print("Analysis and Visualization Examples - Enhanced NLM API")
print("=" * 60)

# Example 1: Basic analysis
print("\n1. Basic brain analysis:")
config = create_enhanced_config()
brain = create_enhanced_brain(config, name="AnalysisBrain")
brain.initialize()

# Run simulation
for i in range(10):
    brain.step(i)
    if i % 3 == 0:
        brain.step(i + 1)

analysis = brain.analyze()
print(f"   Analysis: {analysis}")
print(f"   Stability: {analysis.get_stability():.2f}")
print(f"   Spike pattern: {analysis.get_spike_pattern()}")
print(f"   Development trajectory length: {len(analysis.get_development_trajectory())}")

# Example 2: Development trajectory analysis
print("\n2. Development trajectory analysis:")
trajectory = analysis.get_development_trajectory()
stage_counts = {}
for entry in trajectory:
    stage = entry['stage']
    stage_name = stage.name if hasattr(stage, 'name') else str(stage)
    stage_counts[stage_name] = stage_counts.get(stage_name, 0) + 1
print(f"   Stage counts: {stage_counts}")

# Example 3: Enhanced agent analysis
print("\n3. Enhanced agent analysis:")
brain2 = create_enhanced_brain(config, name="AgentAnalysisBrain")
brain2.initialize()

world = create_enhanced_world()
world.configure_with_defaults(width=15, height=15, vision_width=8, vision_height=8)
world.reset()

agent = create_enhanced_agent(brain2, name="AnalysisAgent")
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
episode_stats = agent.run_episode(world, max_steps=50)

print(f"   Episode stats: {episode_stats}")
print(f"   Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")

# Example 4: Visualization (if matplotlib is available)
print("\n4. Visualization:")
try:
    # Try to create visualization
    VisualizationTools.plot_firing_dynamics(
        brain._state_history, 
        output_file="/tmp/firing_dynamics.png"
    )
    print("   Created firing dynamics plot")
except Exception as e:
    print(f"   Visualization skipped: {e}")

# Example 5: Data export/import
print("\n5. Data export/import:")
export_file = "/tmp/brain_history.json"
VisualizationTools.export_to_json(brain._state_history, export_file)
print(f"   Exported brain history to {export_file}")

imported_history = VisualizationTools.import_from_json(export_file)
print(f"   Imported {len(imported_history)} history entries")

# Example 6: Comprehensive analysis
print("\n6. Comprehensive analysis:")
from pynlm.enhanced import BrainAnalysis

# Create synthetic history for demonstration
history = [
    {
        'step': i,
        'active_neurons': 100 + (i % 50),
        'total_spikes': i * 10,
        'average_firing_rate': 5.0 + (i % 20) / 10.0,
        'developmental_stage': 0,  # Placeholder
        'simulation_time': i * 0.1,
    }
    for i in range(20)
]

stats = {
    'total_steps': len(history),
    'final_active_neurons': history[-1]['active_neurons'],
    'peak_active_neurons': max(h['active_neurons'] for h in history),
    'total_spikes': sum(h['total_spikes'] for h in history),
}

# Create mock brain for analysis
class MockBrain:
    def __init__(self):
        pass

mock_brain = MockBrain()
analysis = BrainAnalysis(history, stats, mock_brain)

print(f"   Total steps analyzed: {analysis.stats.get('total_steps', 0)}")
print(f"   Peak active neurons: {analysis.stats.get('peak_active_neurons', 0)}")
print(f"   Stability metric: {analysis.get_stability():.2f}")
print(f"   Analysis string: {analysis}")

print("\n" + "=" * 60)
print("Analysis and visualization examples completed successfully!")
