# Enhanced Brain Example
"""
Example demonstrating the EnhancedBrain wrapper with Pythonic interfaces.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Enhanced Brain Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use enhanced wrapper for Pythonic interface
    enhanced_brain = pynlm.EnhancedBrain(brain)
    
    # Property-based access (Pythonic)
    print(f"Neuron count: {enhanced_brain.neuron_count}")
    print(f"Synapse count: {enhanced_brain.synapse_count}")
    print(f"Firing count: {enhanced_brain.firing_count}")
    print(f"Firing rate: {enhanced_brain.firing_rate}")
    print(f"Is region active: {enhanced_brain.is_region_active(brain.getRegion(brain.getRegionIds()[0]))}")
    
    # Method chaining
    enhanced_brain.setDevelopmentStage(pynlm.DevelopmentalStage.Maturation).logStatus()
    
    # Compute comprehensive metrics
    metrics = enhanced_brain.compute_metrics()
    print(f"System metrics: {metrics}")
    
    # Iteration over regions (new feature)
    print("Regions in brain:")
    for region_id in enhanced_brain.region_ids:
        print(f"  Region {region_id.index}: {enhanced_brain.get_region_stats(region_id)}")

if __name__ == "__main__":
    main()
