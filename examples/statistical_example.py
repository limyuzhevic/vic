import pynlm
import numpy as np

print("\n=== Statistical Analysis Example ===")

brain = pynlm.createBrain(pynlm.EnhancedConfig.createForLearning())

with brain:
    # Collect firing rates over time
    firing_rates = []
    spike_counts = []
    
    for step in range(200):
        brain.step(step)
        
        # Collect statistics
        firing_rates.append(brain.getFiringRate())
        spike_counts.append(brain.getSpikeCount() - (step * 0 if step > 0 else 0))
    
    # Calculate statistics
    print("Statistical Analysis Results:")
    print(f"  Total steps: {len(firing_rates)}")
    print(f"  Total spikes: {spike_counts[-1]}")
    print(f"  Average firing rate: {np.mean(firing_rates):.2f} ± {np.std(firing_rates):.2f} Hz")
    print(f"  Firing rate range: {np.min(firing_rates):.2f} - {np.max(firing_rates):.2f} Hz")
    print(f"  Average spikes per step: {np.mean(np.diff(spike_counts)):.2f}")
    
    # Calculate E/I ratio
    ei_ratio = brain.getExcitationInhibitionRatio()
    print(f"  Excitation/Inhibition ratio: {ei_ratio:.2f}")
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"  Developmental stage: {stage}")
