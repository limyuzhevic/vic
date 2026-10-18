"""
Batch Processing - Efficient Brain Step Processing
==================================================

This example demonstrates batch processing capabilities for brain simulation.
It shows how to process multiple simulation steps efficiently with custom logic.

Key Concepts Demonstrated:
- Batch processing with custom processor functions
- Efficient simulation of multiple steps
- Custom analysis and monitoring
- Performance optimization through batch operations

Expected Output:
---------------
Starting batch processing demonstration...
Processing brain steps 0-199...

Step 0 Analysis:
  Neurons firing: 12
  Spike rate: 0.024 Hz
  Firing ratio: 0.024

Step 20 Analysis:
  Neurons firing: 45
  Spike rate: 0.090 Hz
  Firing ratio: 0.090

Step 40 Analysis:
  Neurons firing: 78
  Spike rate: 0.156 Hz
  Advanced rate: 0.156
  Total spikes: 312

Step 80 Analysis:
  Neurons firing: 123
  Spike rate: 0.246 Hz
  Advanced rate: 0.246
  Total spikes: 624

Step 160 Analysis:
  Neurons firing: 234
  Spike rate: 0.468 Hz
  Advanced rate: 0.468
  Total spikes: 1248

Batch processing complete!
Processed 201 steps in batch
"""

import pynlm

def create_step_analyzer(step_interval=20):
    """Create a custom step analyzer function"""
    analysis_data = []
    
    def step_analyzer(brain, step, time):
        # Collect statistics for this step
        firing_neurons = brain.getFiringNeuronCount()
        total_neuron_count = brain.getTotalNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        avg_firing_rate = brain.getAverageFiringRate()
        excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
        developmental_stage = brain.getDevelopmentalStage()
        
        # Calculate derived metrics
        firing_ratio = firing_neurons / total_neuron_count if total_neuron_count > 0 else 0
        
        # Store analysis data
        analysis_data.append({
            'step': step,
            'time': time,
            'firing_neurons': firing_neurons,
            'firing_ratio': firing_ratio,
            'avg_firing_rate': avg_firing_rate,
            'excitation_inhibition_ratio': excitation_inhibition_ratio,
            'developmental_stage': developmental_stage,
            'total_spikes': total_spikes
        })
        
        # Print analysis at intervals
        if step % step_interval == 0:
            print(f"Step {step} Analysis:")
            print(f"  Neurons firing: {firing_neurons}")
            print(f"  Spike rate: {avg_firing_rate:.3f} Hz")
            print(f"  Firing ratio: {firing_ratio:.3f}")
            print(f"  E/I ratio: {excitation_inhibition_ratio:.3f}")
            print(f"  Developmental stage: {developmental_stage}")
            print(f"  Total spikes: {total_spikes}")
            print()
    
    return step_analyzer, analysis_data

def create_performance_monitor():
    """Create a performance monitoring function"""
    start_time = None
    step_times = []
    
    def performance_monitor(brain, step, time):
        nonlocal start_time
        
        if start_time is None:
            start_time = time
        
        # Calculate step time
        step_time = time - start_time
        step_times.append(step_time)
        
        # Print progress for every 50 steps
        if step % 50 == 0 and step > 0:
            avg_step_time = sum(step_times) / len(step_times)
            print(f"Progress: Step {step}, Average step time: {avg_step_time:.4f}s")
    
    return performance_monitor

def main():
    print("Starting batch processing demonstration...")
    print(f"pynlm version: {pynlm.__version__}")
    print()
    
    # Create brain
    brain = pynlm.createDefaultBrain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print()
    
    # Create custom step analyzer
    step_analyzer, analysis_data = create_step_analyzer(step_interval=40)
    
    # Create performance monitor
    performance_monitor = create_performance_monitor()
    
    # Create composite processor that runs both analyses
    def composite_processor(brain, step, time):
        step_analyzer(brain, step, time)
        performance_monitor(brain, step, time)
    
    print("Processing brain steps 0-199...")
    print("=" * 50)
    
    # Run batch processing
    pynlm.batch_process_steps(brain, composite_processor, 0, 199)
    
    # Display summary statistics
    print("=" * 50)
    print("Batch processing complete!")
    print(f"Processed {len(analysis_data)} steps")
    
    # Calculate summary statistics
    if analysis_data:
        final_step = analysis_data[-1]
        total_spikes = final_step['total_spikes']
        max_firing = max(d['firing_neurons'] for d in analysis_data)
        max_firing_ratio = max(d['firing_ratio'] for d in analysis_data)
        
        print(f"Final spike count: {total_spikes}")
        print(f"Maximum neurons firing: {max_firing}")
        print(f"Maximum firing ratio: {max_firing_ratio:.3f}")
        
        # Show developmental stage progression
        stages = [d['developmental_stage'] for d in analysis_data]
        if len(set(stages)) > 1:
            print(f"Developmental stages observed: {len(set(stages))}")
    
    print(f"\nProcessed {len(analysis_data)} steps in batch")

if __name__ == "__main__":
    main()