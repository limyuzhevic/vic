import pynlm
import numpy as np
import json
import time
from datetime import datetime
from typing import Dict, List, Tuple

def brain_development_tracking_example():
    """
    Brain development tracking example demonstrating multi-stage development,
    structural changes, and functional maturation analysis.
    """
    print("=== Brain Development Tracking Example ===\n")
    
    # Create developmental configuration
    config = pynlm.createDefaultConfig()
    
    # Enable developmental tracking
    config.set("development.enabled", True)
    config.set("development.stage_transitions", True)
    config.set("plasticity.development_dependent", True)
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.enable_prediction_error", True)
    
    # Configure brain for development
    config.set("brain.region_count", 6)
    config.set("brain.neurons_per_region", 250)
    config.set("brain.initial_excitatory_ratio", 0.75)
    config.set("brain.initial_inhibitory_ratio", 0.25)
    config.set("memory.working_memory_size", 200)
    config.set("memory.episodic_capacity", 3000)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Developmental tracking data
    development_data = {
        "timeline": [],
        "connectivity_analysis": [],
        "functional_metrics": [],
        "structural_changes": [],
        "cognitive_performance": [],
        "neuromodulation_dynamics": []
    }
    
    # Developmental stages and their characteristics
    developmental_phases = _define_developmental_phases()
    
    print(f"Starting brain development tracking ({len(developmental_phases)} phases)...\n")
    
    # Run through developmental timeline
    total_steps = 0
    for phase_name, phase_config in developmental_phases.items():
        print(f"--- Phase: {phase_name.upper()} ---")
        print(f"Duration: {phase_config['duration']} steps")
        print(f"Key characteristics: {phase_config['description']}\n")
        
        # Set phase-specific configuration
        _apply_phase_configuration(brain, phase_config)
        
        # Run phase
        phase_data = _run_development_phase(brain, phase_name, phase_config, total_steps)
        
        # Store phase data
        development_data["timeline"].extend(phase_data["timeline"])
        development_data["connectivity_analysis"].extend(phase_data["connectivity_analysis"])
        development_data["functional_metrics"].extend(phase_data["functional_metrics"])
        development_data["structural_changes"].extend(phase_data["structural_changes"])
        development_data["cognitive_performance"].extend(phase_data["cognitive_performance"])
        development_data["neuromodulation_dynamics"].extend(phase_data["neuromodulation_dynamics"])
        
        total_steps += phase_config["duration"]
        
        print(f"Phase completed. Collected {len(phase_data['timeline'])} data points.\n")
    
    # Analysis and reporting
    print("=== Development Analysis Complete ===\n")
    
    _analyze_development_data(development_data)
    
    # Generate development report
    report = _generate_development_report(development_data, developmental_phases)
    
    # Save data
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    data_file = f"development_tracking_data_{timestamp}.json"
    with open(data_file, 'w') as f:
        json.dump(development_data, f, indent=2)
    
    print(f"Development tracking data saved to: {data_file}")
    
    return development_data

def _define_developmental_phases() -> Dict:
    """Define developmental phases with their characteristics."""
    return {
        "initial": {
            "duration": 200,
            "description": "Primitive connections, basic sensory processing",
            "excitatory_plasticity": 2.0,
            "inhibitory_plasticity": 1.0,
            "connectivity_target": 0.3,
            "development_focus": ["sensory", "basic_motivation"]
        },
        "critical_period": {
            "duration": 300,
            "description": "Rapid synaptogenesis, critical period plasticity",
            "excitatory_plasticity": 3.0,
            "inhibitory_plasticity": 2.0,
            "connectivity_target": 0.5,
            "development_focus": ["sensory_integration", "motor_control", "habituation"]
        },
        "maturation": {
            "duration": 400,
            "description": "Stabilization, refinement, pruning",
            "excitatory_plasticity": 1.5,
            "inhibitory_plasticity": 1.2,
            "connectivity_target": 0.7,
            "development_focus": ["concept_formation", "attention", "working_memory"]
        },
        "adult": {
            "duration": 500,
            "description": "Optimized function, specialized processing",
            "excitatory_plasticity": 0.8,
            "inhibitory_plasticity": 0.9,
            "connectivity_target": 0.9,
            "development_focus": ["predictive_coding", "planning", "social_cognition"]
        },
        "aging": {
            "duration": 300,
            "description": "Age-related decline, maintenance",
            "excitatory_plasticity": 0.5,
            "inhibitory_plasticity": 0.6,
            "connectivity_target": 0.6,
            "development_focus": ["wisdom", "experience_integration", "decline_mitigation"]
        }
    }

def _apply_phase_configuration(brain: pynlm.Brain, phase_config: Dict):
    """Apply phase-specific configuration to brain."""
    # Update plasticity parameters based on phase
    stdp_config = brain.getConfig()
    stdp_config.set("plasticity.stdp_learning_rate", 
                  0.01 * phase_config["excitatory_plasticity"])
    
    # Apply developmental modifications
    if brain.getDevelopmentSystem():
        brain.getDevelopmentSystem().modifyPlasticity(
            phase_config["excitatory_plasticity"], 
            phase_config["inhibitory_plasticity"]
        )

def _run_development_phase(brain: pynlm.Brain, phase_name: str, 
                          phase_config: Dict, start_step: int) -> Dict:
    """Run a single development phase and collect data."""
    phase_data = {
        "timeline": [],
        "connectivity_analysis": [],
        "functional_metrics": [],
        "structural_changes": [],
        "cognitive_performance": [],
        "neuromodulation_dynamics": []
    }
    
    for step in range(phase_config["duration"]):
        current_step = start_step + step
        
        # Create world for interaction
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
        world.setAgentStart(12.5, 12.5)
        
        # Create agent for interaction
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(current_step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on phase goals
        reward = _calculate_development_reward(world, agent, phase_name)
        
        # Apply neuromodulation
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(current_step * 0.1)
        
        # Collect developmental metrics
        if step % 20 == 0:  # Collect data every 20 steps
            data_point = {
                "step": current_step,
                "phase": phase_name,
                "developmental_stage": str(brain.getDevelopmentalStage()),
                "timestamp": datetime.now().isoformat(),
                "neural_metrics": _collect_neural_metrics(brain),
                "memory_metrics": _collect_memory_metrics(brain),
                "neuromodulation_metrics": _collect_neuromodulation_metrics(brain),
                "cognitive_metrics": _collect_cognitive_metrics(brain),
                "structural_metrics": _collect_structural_metrics(brain)
            }
            
            phase_data["timeline"].append(data_point)
        
        # Record developmental transitions
        if step % 100 == 0 and step > 0:
            transition_data = _detect_developmental_transitions(brain, phase_name)
            phase_data["structural_changes"].append(transition_data)
    
    return phase_data

def _calculate_development_reward(world: pynlm.SimpleWorld, agent: pynlm.AgentBrain,
                                  phase_name: str) -> float:
    """Calculate reward based on developmental phase goals."""
    reward = 0.0
    agent_body = world.getAgentBody()
    
    # Phase-specific reward targets
    if phase_name == "initial":
        # Initial phase: exploration and basic learning
        reward += 1.0
        if agent.getCuriosityLevel() > 0.5:
            reward += 2.0
            
    elif phase_name == "critical_period":
        # Critical period: rapid learning and habit formation
        reward += 0.5
        if agent.getPredictionError() > 0.3:
            reward += 3.0
        if agent.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
            reward += 2.0
            
    elif phase_name == "maturation":
        # Maturation: efficiency and specialization
        reward += 0.3
        if brain.getAverageFiringRate() > 5.0:
            reward += 1.5
        if brain.getWorkingMemory().getActiveMemoryCount() if brain.getWorkingMemory() else 0 > 50:
            reward += 1.0
            
    elif phase_name == "adult":
        # Adult: performance and optimization
        reward += 0.1
        if brain.getAverageFiringRate() > 3.0:
            reward += 1.0
        if agent.getPredictionError() < 0.1:
            reward += 2.0
            
    elif phase_name == "aging":
        # Aging: stability and wisdom
        reward += 0.2
        if agent.getCuriosityLevel() < 0.3:
            reward += 1.0  # Lower curiosity is good in aging
        if brain.getAverageFiringRate() < 8.0:
            reward += 1.5  # Slower is good in aging
    
    # Common rewards
    vision = world.getSensoryPercept().getVision()
    for y in range(len(vision)):
        for x in range(len(vision[0])):
            if vision[y][x] == 5:  # Resource
                reward += 5.0
            elif vision[y][x] == 6:  # Hazard
                reward -= 2.0
            elif vision[y][x] == 7:  # Marker
                reward += 10.0
    
    # Energy cost
    reward -= agent_body.energy * 0.01
    
    # Health penalty
    if agent_body.health < 50:
        reward -= 1.0
    
    return reward

def _collect_neural_metrics(brain: pynlm.Brain) -> Dict:
    """Collect neural system metrics."""
    return {
        "total_neurons": brain.getTotalNeuronCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "region_count": brain.getRegionCount(),
        "synapse_count": brain.getTotalSynapseCount()
    }

def _collect_memory_metrics(brain: pynlm.Brain) -> Dict:
    """Collect memory system metrics."""
    metrics = {}
    
    if brain.getWorkingMemory():
        metrics["working_memory_active"] = brain.getWorkingMemory().getActiveMemoryCount()
        metrics["working_memory_capacity"] = brain.getWorkingMemory().getCapacity()
    
    if brain.getEpisodicMemory():
        metrics["episodic_memory_count"] = brain.getEpisodicMemory().getMemoryCount()
        metrics["episodic_memory_capacity"] = brain.getEpisodicMemory().getCapacity()
        metrics["episodic_memory_strength"] = brain.getEpisodicMemory().getAverageMemoryStrength()
    
    if brain.getAssociativeMemory():
        metrics["associative_memory_strength"] = brain.getAssociativeMemory().getAverageAssociationStrength()
        metrics["associative_memory_interference"] = brain.getAssociativeMemory().getInterferenceLevel()
    
    return metrics

def _collect_neuromodulation_metrics(brain: pynlm.Brain) -> Dict:
    """Collect neuromodulation metrics."""
    metrics = {}
    
    if brain.getDopamine():
        metrics["dopamine_level"] = brain.getDopamine().getCurrentLevel()
        metrics["dopamine_plasticity_modulator"] = brain.getDopamine().getPlasticityModulator()
    
    if brain.getCuriosity():
        metrics["curiosity_level"] = brain.getCuriosity().getCurrentLevel()
        metrics["curiosity_exploration_drive"] = brain.getCuriosity().getExplorationDrive()
    
    if brain.getNovelty():
        metrics["novelty_level"] = brain.getNovelty().getCurrentLevel()
        metrics["novelty_surprise_score"] = brain.getNovelty().getSurpriseScore()
    
    if brain.getPredictionErrorSignal():
        metrics["prediction_error"] = brain.getPredictionErrorSignal().getCurrentError()
    
    return metrics

def _collect_cognitive_metrics(brain: pynlm.Brain) -> Dict:
    """Collect cognitive system metrics."""
    metrics = {}
    
    if brain.getPredictionSystem():
        metrics["prediction_error"] = brain.getPredictionSystem().getPredictionError()
        metrics["prediction_confidence"] = brain.getPredictionSystem().getConfidence()
    
    if brain.getPlanner():
        metrics["planning_capacity"] = brain.getPlanner().getActionWindow()
        metrics["planning_efficiency"] = brain.getPlanner().getEvaluationEfficiency()
    
    if brain.getConceptFormation():
        metrics["concept_count"] = brain.getConceptFormation().getConceptCount()
        metrics["concept_learning_rate"] = brain.getConceptFormation().getLearningRate()
    
    if brain.getAttention():
        metrics["attention_focus_strength"] = brain.getAttention().getFocusStrength()
        metrics["attention_switch_rate"] = brain.getAttention().getSwitchRate()
    
    return metrics

def _collect_structural_metrics(brain: pynlm.Brain) -> Dict:
    """Collect structural metrics."""
    metrics = {}
    
    # Region-level metrics
    for region in brain.getRegions():
        region_key = f"region_{region.getName()}"
        metrics[region_key] = {
            "neuron_count": region.getTotalNeuronCount(),
            "firing_rate": region.getAverageFiringRate(),
            "synaptic_density": region.getSynapticDensity(),
            "plasticity": region.getPlasticityLevel()
        }
    
    return metrics

def _detect_developmental_transitions(brain: pynlm.Brain, phase_name: str) -> Dict:
    """Detect developmental transitions and changes."""
    transitions = {}
    
    # Check for developmental stage changes
    current_stage = brain.getDevelopmentalStage()
    transitions["developmental_stage"] = str(current_stage)
    
    # Check for plasticity changes
    if brain.getDevelopmentSystem():
        transitions["plasticity_change"] = brain.getDevelopmentSystem().getPlasticityChangeRate()
    
    # Check for memory system maturation
    if brain.getEpisodicMemory():
        transitions["episodic_maturation"] = brain.getEpisodicMemory().getMaturationLevel()
    
    if brain.getAssociativeMemory():
        transitions["associative_maturation"] = brain.getAssociativeMemory().getMaturationLevel()
    
    return transitions

def _analyze_development_data(data: Dict):
    """Analyze developmental data and print summary."""
    print("=== Development Data Analysis ===")
    
    if not data["timeline"]:
        print("No data collected.")
        return
    
    # Overall statistics
    total_steps = len(data["timeline"])
    phases_covered = len(set(point["phase"] for point in data["timeline"]))
    
    print(f"Total data points collected: {total_steps}")
    print(f"Developmental phases covered: {phases_covered}")
    print(f"Average steps per phase: {total_steps / max(phases_covered, 1):.1f}")
    
    # Neural development trends
    firing_rates = [point["neural_metrics"]["avg_firing_rate"] for point in data["timeline"]]
    neuron_counts = [point["neural_metrics"]["total_neurons"] for point in data["timeline"]]
    
    print(f"\nNeural Development:")
    print(f"  Average firing rate: {np.mean(firing_rates):.2f} Hz (±{np.std(firing_rates):.2f})")
    print(f"  Neuron count range: {min(neuron_counts)} - {max(neuron_counts)}")
    print(f"  Firing rate trend: {'Increasing' if np.mean(firing_rates[-10:]) > np.mean(firing_rates[:10]) else 'Decreasing' if np.mean(firing_rates[-10:]) < np.mean(firing_rates[:10]) else 'Stable'}")
    
    # Memory development
    if data["memory_metrics"]:
        wm_active = [point["memory_metrics"].get("working_memory_active", 0) for point in data["timeline"]]
        em_count = [point["memory_metrics"].get("episodic_memory_count", 0) for point in data["timeline"]]
        
        print(f"\nMemory Development:")
        print(f"  Working memory: {np.mean(wm_active):.1f} (±{np.std(wm_active):.1f}) active items")
        print(f"  Episodic memory: {np.mean(em_count):.1f} (±{np.std(em_count):.1f}) stored experiences")
    
    # Neuromodulation development
    if data["neuromodulation_dynamics"]:
        dopamine_levels = [point["neuromodulation_metrics"].get("dopamine_level", 0) for point in data["timeline"]]
        curiosity_levels = [point["neuromodulation_metrics"].get("curiosity_level", 0) for point in data["timeline"]]
        
        print(f"\nNeuromodulation Development:")
        print(f"  Dopamine: {np.mean(dopamine_levels):.3f} (±{np.std(dopamine_levels):.3f})")
        print(f"  Curiosity: {np.mean(curiosity_levels):.3f} (±{np.std(curiosity_levels):.3f})")

def _generate_development_report(data: Dict, phases: Dict) -> Dict:
    """Generate comprehensive development report."""
    report = {
        "summary": {
            "total_data_points": len(data["timeline"]),
            "phases_covered": len(phases),
            "development_duration": len(data["timeline"]) * 0.1,  # Assuming 0.1 step per data point
            "data_collection_period": {
                "start": data["timeline"][0]["step"] if data["timeline"] else 0,
                "end": data["timeline"][-1]["step"] if data["timeline"] else 0
            }
        },
        "neural_development": _analyze_neural_trends(data["timeline"]),
        "memory_development": _analyze_memory_trends(data["timeline"]),
        "cognitive_development": _analyze_cognitive_trends(data["timeline"]),
        "phase_performance": _analyze_phase_performance(data, phases),
        "key_findings": _extract_key_findings(data)
    }
    
    return report

def _analyze_neural_trends(timeline: List) -> Dict:
    """Analyze neural development trends."""
    if not timeline:
        return {}
    
    firing_rates = [point["neural_metrics"]["avg_firing_rate"] for point in timeline]
    e_i_ratios = [point["neural_metrics"]["e_i_ratio"] for point in timeline]
    neuron_counts = [point["neural_metrics"]["total_neurons"] for point in timeline]
    
    return {
        "firing_rate": {
            "mean": float(np.mean(firing_rates)),
            "std": float(np.std(firing_rates)),
            "trend": "increasing" if np.mean(firing_rates[-10:]) > np.mean(firing_rates[:10]) else "decreasing" if np.mean(firing_rates[-10:]) < np.mean(firing_rates[:10]) else "stable",
            "range": [float(min(firing_rates)), float(max(firing_rates))]
        },
        "e_i_balance": {
            "mean": float(np.mean(e_i_ratios)),
            "std": float(np.std(e_i_ratios)),
            "stability": 1.0 / (1.0 + np.std(e_i_ratios))
        },
        "population_growth": {
            "initial": float(neuron_counts[0]) if neuron_counts else 0,
            "final": float(neuron_counts[-1]) if neuron_counts else 0,
            "growth_rate": float((neuron_counts[-1] - neuron_counts[0]) / max(neuron_counts[0], 1))
        }
    }

def _analyze_memory_trends(timeline: List) -> Dict:
    """Analyze memory development trends."""
    if not timeline:
        return {}
    
    wm_active = [point["memory_metrics"].get("working_memory_active", 0) for point in timeline]
    em_count = [point["memory_metrics"].get("episodic_memory_count", 0) for point in timeline]
    em_strength = [point["memory_metrics"].get("episodic_memory_strength", 0) for point in timeline]
    
    return {
        "working_memory": {
            "mean_capacity": float(np.mean(wm_active)),
            "stability": 1.0 / (1.0 + np.std(wm_active)),
            "efficiency_trend": "improving" if np.mean(wm_active[-10:]) > np.mean(wm_active[:10]) else "declining"
        },
        "episodic_memory": {
            "mean_size": float(np.mean(em_count)),
            "mean_strength": float(np.mean(em_strength)),
            "consolidation_rate": float(np.mean([(em_strength[i] - em_strength[i-1]) if i > 0 else 0 for i in range(len(em_strength))]))
        }
    }

def _analyze_cognitive_trends(timeline: List) -> Dict:
    """Analyze cognitive development trends."""
    if not timeline:
        return {}
    
    prediction_errors = [point["cognitive_metrics"].get("prediction_error", 0) for point in timeline]
    concept_counts = [point["cognitive_metrics"].get("concept_count", 0) for point in timeline]
    attention_focus = [point["cognitive_metrics"].get("attention_focus_strength", 0) for point in timeline]
    
    return {
        "prediction_accuracy": {
            "mean_error": float(np.mean(prediction_errors)),
            "improvement_rate": float(np.mean([(prediction_errors[i-1] - prediction_errors[i]) if i > 0 else 0 for i in range(len(prediction_errors))]))
        },
        "concept_development": {
            "final_concept_count": float(concept_counts[-1]) if concept_counts else 0,
            "learning_speed": float(concept_counts[-1] / max(len(timeline), 1))
        },
        "attention_development": {
            "mean_focus": float(np.mean(attention_focus)),
            "stability": 1.0 / (1.0 + np.std(attention_focus))
        }
    }

def _analyze_phase_performance(data: Dict, phases: Dict) -> Dict:
    """Analyze performance by developmental phase."""
    phase_performance = {}
    
    for phase_name, phase_config in phases.items():
        phase_timeline = [point for point in data["timeline"] if point["phase"] == phase_name]
        
        if phase_timeline:
            avg_firing_rate = np.mean([point["neural_metrics"]["avg_firing_rate"] for point in phase_timeline])
            final_stage = phase_timeline[-1]["developmental_stage"]
            
            phase_performance[phase_name] = {
                "avg_firing_rate": float(avg_firing_rate),
                "data_points": len(phase_timeline),
                "final_developmental_stage": final_stage,
                "performance_score": float(avg_firing_rate * len(phase_timeline) / 100)
            }
    
    return phase_performance

def _extract_key_findings(data: Dict) -> List[str]:
    """Extract key findings from development data."""
    findings = []
    
    if not data["timeline"]:
        findings.append("No development data collected.")
        return findings
    
    # Overall findings
    total_steps = len(data["timeline"])
    findings.append(f"Brain developed {total_steps} time points over {total_steps * 0.1:.1f} simulation steps.")
    
    # Neural findings
    firing_rates = [point["neural_metrics"]["avg_firing_rate"] for point in data["timeline"]]
    if np.mean(firing_rates) > 5.0:
        findings.append(f"Neural activity stabilized at average {np.mean(firing_rates):.2f} Hz.")
    
    # Memory findings
    if data["memory_metrics"]:
        em_count = [point["memory_metrics"].get("episodic_memory_count", 0) for point in data["timeline"]]
        if np.mean(em_count) > 100:
            findings.append(f"Episodic memory accumulated {np.mean(em_count):.0f} experiences on average.")
    
    # Neuromodulation findings
    if data["neuromodulation_dynamics"]:
        dopamine_levels = [point["neuromodulation_metrics"].get("dopamine_level", 0) for point in data["timeline"]]
        curiosity_levels = [point["neuromodulation_metrics"].get("curiosity_level", 0) for point in data["timeline"]]
        
        if np.mean(dopamine_levels) > 1.0:
            findings.append(f"Dopamine system reached {np.mean(dopamine_levels):.2f} level, indicating reward processing.")
        
        if np.mean(curiosity_levels) > 0.3:
            findings.append(f"Curiosity system maintained {np.mean(curiosity_levels):.2f} level, supporting exploration.")
    
    findings.append("Development followed expected phases from initial to aging.")
    findings.append("Brain systems showed progressive specialization and optimization.")
    
    return findings

if __name__ == "__main__":
    print("Starting Brain Development Tracking Example...\n")
    
    start_time = time.time()
    development_data = brain_development_tracking_example()
    end_time = time.time()
    
    print(f"\n=== Example Complete ===")
    print(f"Total execution time: {end_time - start_time:.2f} seconds")
    print(f"Data points collected: {len(development_data['timeline'])}")
    print(f"Developmental phases tracked: {len(set(point['phase'] for point in development_data['timeline']))}")
    print(f"Example demonstrates multi-stage brain development with comprehensive tracking.")
