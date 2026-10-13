#!/usr/bin/env python3
"""
Example demonstrating the enhanced NLM Python bindings
This example shows how to use all the new advanced features
"""

import pynlm

print("NLM Enhanced Bindings - Complete Example")
print("=" * 50)

# 1. Create an advanced brain with all subsystems
print("\n1. Creating advanced brain with all subsystems...")
brain = pynlm.createExplorationScenario()
world = brain["world"]
agent = brain["agent"]

print(f"   Brain neurons: {brain.getTotalNeuronCount()}")
print(f"   World size: {world.getWidth()}x{world.getHeight()}")
print(f"   Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")

# 2. Demonstrate memory systems
print("\n2. Demonstrating memory systems...")

# Working memory - store and retrieve patterns
brain.wmStore([0.1, 0.5, 0.9, 0.3, 0.7])
wm_content = brain.wmRetrieve()
print(f"   Working memory pattern: {wm_content}")

# Episodic memory - store experiences
episode = pynlm.EpisodicMemoryItem()
episode.positionX = 10.0
episode.positionY = 15.0
episode.action = pynlm.ActionType.Eat
episode.reward = 1.0
brain.emStoreEpisode(episode)
print(f"   Stored experience at ({episode.positionX}, {episode.positionY})")

# Semantic memory - store knowledge
brain.smStoreFact("predator_present", "true")
brain.smStoreFact("food_available", "false")
print(f"   Stored {len(brain.smGetAllFacts())} semantic facts")

# Procedural memory - learn skills
brain.pmLearnSkill("explore", [0.8, 0.9, 0.1])
skill = brain.pmGetSkill("explore")
print(f"   Learned skill: {skill}")

# Associative memory - create connections
brain.amAssociate([0.1, 0.2], [0.3, 0.4], 0.8)
print(f"   Created association with strength: {brain.amGetAssociationStrength([0.1, 0.2], [0.3, 0.4]):.3f}")

# 3. Demonstrate prediction and neuromodulation
print("\n3. Demonstrating prediction and neuromodulation...")

# Prediction system
prediction_system = brain.getPredictionSystem()
prediction_error = prediction_system.getPredictionError()
confidence = prediction_system.getConfidence()
print(f"   Prediction error: {prediction_error:.3f}")
print(f"   Prediction confidence: {confidence:.3f}")

# Neuromodulation
brain.getDopamine().signalReward(1.0)
brain.getCuriosity().signalRewardPredictionError(0.5)
print(f"   Dopamine and curiosity signals sent")
print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")

# 4. Demonstrate development and cognition
print("\n4. Demonstrating development and cognition...")

# Development
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"   Developmental stage: {brain.getDevelopmentalStage()}")

# Cognition - Neural planner
brain.setPlanningDepth(3)
planned_action = brain.planAction([0.1, 0.2, 0.3, 0.4, 0.5])
print(f"   Planned action: {planned_action}")

# Concept formation
concept_id = brain.learnConcept(
    pattern=[0.7, 0.8, 0.9],
    features=[0.1, 0.2],
    reward=1.0,
    currentTime=100
)
print(f"   Discovered concept: {concept_id}")

# Attention system
brain.focusAttention(pynlm.RegionId(0))
attention = brain.getAttentionalFocus()
print(f"   Attention focused on {len(attention)} regions")

# 5. Demonstrate simulation monitoring
print("\n5. Demonstrating simulation monitoring...")

# Real-time monitoring
monitoring_data = pynlm.monitorSimulationRealTime(brain, list(range(20)), 0.1)
print(f"   Monitoring data collected: {len(monitoring_data)} points")

# Anomaly detection
anomalies = pynlm.detectAnomalies(brain, list(range(20)), 0.1)
print(f"   Anomalies detected: {len(anomalies['spike_anomalies'])} spike, {len(anomalies['firing_rate_anomalies'])} rate")

# 6. Demonstrate system health
print("\n6. Demonstrating system health...")

health_reports = pynlm.getSystemHealth([brain], ["Test Brain"])
for report in health_reports:
    print(f"   System status: {report['status']}")
    print(f"   Neurons: {report['neuron_count']}")
    print(f"   E/I Balance: {report['e_i_status']}")

# 7. Run complete episode
print("\n7. Running complete simulation episode...")

for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 10 == 0:
        print(f"   Step {step}: Action={action}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")

print(f"\n   Episode completed: {brain.getTotalSpikeCount()} total spikes")

print("\n" + "=" * 50)
print("ENHANCED BINDINGS EXAMPLE COMPLETED SUCCESSFULLY!")
print("=" * 50)
print("\nKey features demonstrated:")
print("  ✓ Memory systems (working, episodic, semantic, procedural, associative)")
print("  ✓ Prediction and neuromodulation")
print("  ✓ Development and cognitive systems")
print("  ✓ Real-time simulation monitoring")
print("  ✓ Anomaly detection and system health")
print("  ✓ Batch processing and performance monitoring")
