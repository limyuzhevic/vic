#!/usr/bin/env python3
"""
Example: Neural Processing Pipeline
===================================

This example demonstrates the neural-based sensory processing capabilities
of the enhanced NLM Python bindings. It shows how to use VisionProcessor,
AudioProcessor, and InternalSignalsProcessor for advanced neural processing.

The pipeline includes:
- Creating and processing vision data with neural processor
- Processing audio data with neural processor
- Using internal signals processor for homeostasis
- Integrating all components into a complete processing pipeline
"""

import pynlm as nlm
import math
import random

def create_vision_pipeline():
    """
    Demonstrate neural vision processing pipeline.
    
    Shows how to:
    - Create vision input buffers
    - Process with neural VisionProcessor
    - Extract features and dimensionalities
    - Apply neural transformations
    """
    print("=== NEURAL VISION PROCESSING PIPELINE ===")
    
    # Create vision input buffer
    vision = nlm.createVisionInput(width=64, height=64, channels=1)
    print(f"1. Created vision input: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Create neural vision processor
    vision_processor = nlm.VisionProcessor()
    print("2. Created VisionProcessor for neural vision processing")
    
    # Generate test vision data (simulating camera input)
    vision_data = []
    for y in range(64):
        for x in range(64):
            # Create a pattern: center bright, edges dark
            distance = math.sqrt((x - 32) ** 2 + (y - 32) ** 2)
            intensity = max(0, 1.0 - distance / 40.0)
            vision_data.append(intensity)
    
    print(f"3. Generated {len(vision_data)} vision data points")
    
    # Process vision through neural processor
    vision.setData(vision_data)
    vision_processor.process(vision)
    
    print("4. Processed vision through neural processor:")
    
    # Get processed features
    features = vision_processor.getFeatures()
    feature_dims = vision_processor.getFeatureDimensions()
    
    print(f"   - Processed features: {len(features)} values")
    print(f"   - Feature dimensionality: {feature_dims}")
    print(f"   - Feature statistics:")
    print(f"     * Mean: {sum(features) / len(features):.3f}")
    print(f"     * Max: {max(features):.3f}")
    print(f"     * Min: {min(features):.3f}")
    
    # Demonstrate neural feature analysis
    print("\n5. Neural feature analysis:")
    
    # Calculate basic neural statistics
    if features:
        feature_variance = sum((f - sum(features) / len(features)) ** 2 for f in features) / len(features)
        feature_std = math.sqrt(feature_variance)
        
        print(f"   - Feature variance: {feature_variance:.3f}")
        print(f"   - Feature standard deviation: {feature_std:.3f}")
        
        # Check for neural activation patterns
        high_activation = sum(1 for f in features if f > 0.5)
        medium_activation = sum(1 for f in features if 0.2 < f <= 0.5)
        low_activation = sum(1 for f in features if f <= 0.2)
        
        print(f"   - Neural activation levels:")
        print(f"     * High (>0.5): {high_activation} features ({high_activation/len(features)*100:.1f}%)")
        print(f"     * Medium (0.2-0.5): {medium_activation} features ({medium_activation/len(features)*100:.1f}%)")
        print(f"     * Low (≤0.2): {low_activation} features ({low_activation/len(features)*100:.1f}%)")
    
    return vision_processor, features

def create_audio_pipeline():
    """
    Demonstrate neural audio processing pipeline.
    
    Shows how to:
    - Create audio input buffers
    - Process with neural AudioProcessor
    - Extract audio features
    - Apply neural transformations
    """
    print("\n=== NEURAL AUDIO PROCESSING PIPELINE ===")
    
    # Create audio input buffer
    audio = nlm.createAudioInput(sampleRate=22050, numSamples=1024)
    print(f"1. Created audio input: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
    
    # Create neural audio processor
    audio_processor = nlm.AudioProcessor()
    print("2. Created AudioProcessor for neural audio processing")
    
    # Generate test audio data (simulating speech/music)
    audio_data = []
    for i in range(1024):
        # Create a complex tone pattern
        t = i / 22050.0  # time in seconds
        frequency = 440.0 * (1 + 0.1 * math.sin(2 * math.pi * 5 * t))  # modulated frequency
        amplitude = 0.5 + 0.5 * math.sin(2 * math.pi * 2 * t)  # varying amplitude
        
        sample = amplitude * math.sin(2 * math.pi * frequency * t)
        audio_data.append(sample)
    
    print(f"3. Generated {len(audio_data)} audio samples")
    
    # Process audio through neural processor
    audio.setData(audio_data)
    audio_processor.process(audio)
    
    print("4. Processed audio through neural processor:")
    
    # Get processed features
    features = audio_processor.getFeatures()
    feature_dims = audio_processor.getFeatureDimensions()
    
    print(f"   - Processed features: {len(features)} values")
    print(f"   - Feature dimensionality: {feature_dims}")
    
    if features:
        # Calculate audio feature statistics
        feature_mean = sum(features) / len(features)
        feature_max = max(features)
        feature_min = min(features)
        
        print(f"   - Feature statistics:")
        print(f"     * Mean: {feature_mean:.3f}")
        print(f"     * Max: {feature_max:.3f}")
        print(f"     * Min: {feature_min:.3f}")
        
        # Check for neural activation patterns
        active_features = sum(1 for f in features if abs(f) > 0.3)
        print(f"   - Active neural features (|f| > 0.3): {active_features} ({active_features/len(features)*100:.1f}%)")
    
    return audio_processor, features

def create_internal_signals_pipeline():
    """
    Demonstrate internal signals processing pipeline.
    
    Shows how to:
    - Create internal signals buffers
    - Process through InternalSignalsProcessor
    - Get homeostasis and processed signals
    - Monitor neural homeostasis
    """
    print("\n=== INTERNAL SIGNALS PROCESSING PIPELINE ===")
    
    # Create internal signals buffer
    internal_signals = nlm.createInternalSignals()
    print(f"1. Created internal signals buffer: {internal_signals.getDimensions()} dimensions")
    
    # Create neural internal signals processor
    processor = nlm.InternalSignalsProcessor()
    print("2. Created InternalSignalsProcessor for homeostasis monitoring")
    
    # Add test signals representing neural homeostasis
    print("3. Adding neural homeostasis signals:")
    
    # Homeostatic load (energy needs)
    homeostatic_load = 0.8
    internal_signals.addSignal(homeostatic_load)
    print(f"   - Added homeostatic load: {homeostatic_load:.2f}")
    
    # Metabolic state
    metabolic_state = 0.6
    internal_signals.addSignal(metabolic_state)
    print(f"   - Added metabolic state: {metabolic_state:.2f}")
    
    # Neural activity level
    neural_activity = 0.9
    internal_signals.addSignal(neural_activity)
    print(f"   - Added neural activity: {neural_activity:.2f}")
    
    # Sleep/wake state
    sleep_state = 0.3
    internal_signals.addSignal(sleep_state)
    print(f"   - Added sleep state: {sleep_state:.2f}")
    
    # Process internal signals through neural processor
    processor.process(internal_signals)
    
    print("4. Processed internal signals through neural processor:")
    
    # Get homeostasis and processed signals
    homeostasis_signals = processor.getHomeostaticSignals()
    processed_signals = processor.getProcessedSignals()
    
    print(f"   - Homeostasis signals: {len(homeostasis_signals)} values")
    print(f"   - Processed signals: {len(processed_signals)} values")
    
    if homeostasis_signals:
        print(f"   - Homeostasis statistics:")
        print(f"     * Mean: {sum(homeostasis_signals) / len(homeostasis_signals):.3f}")
        print(f"     * Max: {max(homeostasis_signals):.3f}")
        print(f"     * Min: {min(homeostasis_signals):.3f}")
    
    if processed_signals:
        print(f"   - Processed signal statistics:")
        print(f"     * Mean: {sum(processed_signals) / len(processed_signals):.3f}")
        print(f"     * Max: {max(processed_signals):.3f}")
    
    # Demonstrate homeostasis regulation
    print("\n5. Homeostasis regulation:")
    
    # Check if homeostasis is in balance
    avg_homeostasis = sum(homeostasis_signals) / len(homeostasis_signals)
    
    if 0.4 <= avg_homeostasis <= 0.6:
        regulation_status = "Balanced"
        print(f"   - Homeostasis regulation: {regulation_status} (avg: {avg_homeostasis:.2f})")
        print(f"   - Neural system is in stable state")
    elif avg_homeostasis > 0.6:
        regulation_status = "Overactive"
        print(f"   - Homeostasis regulation: {regulation_status} (avg: {avg_homeostasis:.2f})")
        print(f"   - Neural system needs rest/regulation")
    else:
        regulation_status = "Underactive"
        print(f"   - Homeostasis regulation: {regulation_status} (avg: {avg_homeostasis:.2f})")
        print(f"   - Neural system needs stimulation")
    
    return processor, homeostasis_signals, processed_signals

def integrate_all_processors():
    """
    Demonstrate integration of all neural processors.
    
    Shows how to:
    - Combine vision, audio, and internal signal processing
    - Create a complete neural processing pipeline
    - Integrate all modalities for comprehensive brain input
    """
    print("\n" + "="*70)
    print("INTEGRATING ALL NEURAL PROCESSORS - COMPREHENSIVE PIPELINE")
    print("="*70)
    
    print("\nCreating comprehensive neural processing pipeline...")
    
    # Create all sensory processors
    vision_processor, vision_features = create_vision_pipeline()
    audio_processor, audio_features = create_audio_pipeline()
    internal_processor, homeostasis, processed_internal = create_internal_signals_pipeline()
    
    # Create integrated percept
    percept = nlm.createSensoryPercept()
    
    # Set all sensory modalities
    print(f"\n6. Integrating all modalities into sensory percept:")
    
    # Create synthetic vision data (64x64)
    vision_data = [0.5 + 0.5 * math.sin(x/10.0) * math.sin(y/10.0) 
                   for y in range(16) for x in range(16)]
    percept.setVision(vision_data)
    print(f"   - Vision: {len(vision_data)} values (16x16)")
    
    # Create synthetic touch data
    touch_data = [0.1, 0.3, 0.5, 0.2, 0.8, 0.4]
    percept.setTouch(touch_data)
    print(f"   - Touch: {len(touch_data)} values")
    
    # Create synthetic internal data
    internal_data = [homeostasis[0] if homeostasis else 0.5,
                     homeostasis[1] if len(homeostasis) > 1 else 0.3,
                     0.7]
    percept.setInternal(internal_data)
    print(f"   - Internal: {len(internal_data)} homeostatic signals")
    
    # Create synthetic proprioception data
    proprioception_data = [0.2, 0.8, 0.3, 0.6, 0.1, 0.9]
    percept.setProprioception(proprioception_data)
    print(f"   - Proprioception: {len(proprioception_data)} body signals")
    
    # Create synthetic audio data
    audio_data = [0.3 * math.sin(2 * math.pi * 440 * i / 22050) for i in range(1024)]
    percept.setAudio(audio_data)
    print(f"   - Audio: {len(audio_data)} samples")
    
    # Set timestamp
    percept.setTimestamp(1.0)
    
    # Get all signals concatenated
    all_signals = percept.getAllSignals()
    
    print(f"\n7. Integrated sensory percept:")
    print(f"   - Total signal count: {len(all_signals)}")
    print(f"   - Signal breakdown: {len(vision_data)} vision + {len(touch_data)} touch + "
          f"{len(internal_data)} internal + {len(proprioception_data)} proprioception + "
          f"{len(audio_data)} audio")
    
    # Calculate integrated signal statistics
    if all_signals:
        integrated_mean = sum(all_signals) / len(all_signals)
        integrated_max = max(all_signals)
        integrated_min = min(all_signals)
        
        print(f"   - Integrated signal statistics:")
        print(f"     * Mean: {integrated_mean:.3f}")
        print(f"     * Max: {integrated_max:.3f}")
        print(f"     * Min: {integrated_min:.3f}")
        
        # Calculate signal entropy (measure of information content)
        from collections import Counter
        signal_bins = [int(s * 10) / 10.0 for s in all_signals]  # 0.0, 0.1, 0.2, ..., 1.0
        bin_counts = Counter(signal_bins)
        entropy = -sum((count / len(all_signals)) * math.log2(count / len(all_signals)) 
                      for count in bin_counts.values() if count > 0)
        
        print(f"   - Signal entropy (information content): {entropy:.3f} bits")
    
    print(f"\n8. Neural processing pipeline integration:")
    print(f"   - Vision processor: {len(vision_features)} features extracted")
    print(f"   - Audio processor: {len(audio_features)} features extracted")
    print(f"   - Internal processor: {len(homeostasis)} homeostasis signals")
    print(f"   - Integrated percept: {len(all_signals)} total input signals")
    print(f"   - Pipeline ready for brain input")
    
    return percept, all_signals

def demonstrate_advanced_neural_integration():
    """
    Demonstrate advanced neural integration with multiple agents.
    
    Shows how to:
    - Create multiple agent systems
    - Share brain resources
    - Coordinate multiple processing pipelines
    - Implement advanced neuromodulation
    """
    print("\n" + "="*70)
    print("ADVANCED NEURAL INTEGRATION - MULTI-AGENT SYSTEM")
    print("="*70)
    
    print("\nCreating multi-agent neural integration system...")
    
    # Create shared brain (neural integration)
    print("1. Creating shared brain for neural integration:")
    shared_brain = nlm.createBrain(nlm.createDefaultConfig())
    shared_brain.initialize()
    print(f"   - Created brain with {shared_brain.getTotalNeuronCount()} neurons")
    
    # Create shared world
    shared_world = nlm.createSimpleWorld()
    shared_world.configure(100.0, 100.0, 64, 64)
    shared_world.setRandomSeed(123)
    print("   - Created shared simulation world")
    
    # Create multiple agents
    agents = []
    print(f"\n2. Creating {3} agent systems with neural sharing:")
    
    for i in range(3):
        agent = nlm.initializeAgentSystem(shared_brain, shared_world)
        agent.initialize(shared_world)
        agents.append(agent)
        
        print(f"   - Agent {i + 1}: {agent.getSensoryInputSize()} input, {agent.getMotorOutputSize()} output")
    
    # Demonstrate neural sharing
    print(f"\n3. Neural resource sharing:")
    print(f"   - All agents share the same brain instance")
    print(f"   - Neural computation is distributed across agents")
    print(f"   - Neuromodulation is shared system-wide")
    
    # Demonstrate shared neuromodulation
    print(f"\n4. Shared neuromodulation system:")
    
    for i, agent in enumerate(agents):
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        neuromodulation = agent.getNeuromodulationLevel()
        
        print(f"   - Agent {i + 1}:")
        print(f"     * Curiosity: {curiosity:.2f}")
        print(f"     * Novelty: {novelty:.2f}")
        print(f"     * Prediction Error: {prediction_error:.2f}")
        print(f"     * Neuromodulation: {neuromodulation:.2f}")
    
    # Demonstrate coordinated processing
    print(f"\n5. Coordinated neural processing:")
    
    for step in range(5):
        print(f"   Step {step + 1}:")
        
        # Each agent processes its own sensory input
        for i, agent in enumerate(agents):
            # Create agent-specific sensory input (different perspectives)
            percept = nlm.createSensoryPercept()
            
            # Simulate different agent viewpoints
            offset_x = i * 20.0
            vision_data = []
            
            for y in range(16):
                for x in range(16):
                    # Agent-specific view based on position
                    dist = math.sqrt((x - 8 + offset_x) ** 2 + (y - 8) ** 2)
                    intensity = max(0, 1.0 - dist / 25.0)
                    vision_data.append(intensity)
            
            percept.setVision(vision_data)
            agent.processSensoryInput(percept)
            
            # Get agent's action
            action = agent.decodeMotorCommand()
            shared_world.applyMotorCommand(action, 0.01)
            
            # Show agent's processing
            curiosity = agent.getCuriosityLevel()
            print(f"     * Agent {i + 1}: Curiosity={curiosity:.2f}, Action type={action.getType()}")
        
        # Update world
        body = shared_world.getAgentBody()
        print(f"     * World position: ({body.x:.1f}, {body.y:.1f})")
        
        # Get system stats
        neurons, synapses, active, firing_rate, e_i_ratio = nlm.getSystemStats(shared_brain)
        print(f"     * System stats: {neurons} neurons, {synapses} synapses, {firing_rate:.2f} firing rate")
    
    print(f"\n6. Advanced integration features:")
    print(f"   - Neural computation sharing across agents")
    print(f"   - Shared neuromodulation and homeostasis")
    print(f"   - Coordinated sensory processing")
    print(f"   - Distributed action selection")
    print(f"   - System-wide performance monitoring")
    
    return shared_brain, shared_world, agents

def main():
    """
    Main function demonstrating neural processing pipelines.
    """
    print("\n" + "="*80)
    print("ENHANCED NLM - NEURAL PROCESSING PIPELINE DEMONSTRATION")
    print("="*80)
    print("\nThis demonstration showcases the neural-based sensory processing")
    print("capabilities of the enhanced NLM Python bindings.\n")
    
    try:
        # Individual processor demonstrations
        vision_processor, vision_features = create_vision_pipeline()
        audio_processor, audio_features = create_audio_pipeline()
        internal_processor, homeostasis, processed_internal = create_internal_signals_pipeline()
        
        # Integration demonstration
        percept, all_signals = integrate_all_processors()
        
        # Advanced integration demonstration
        shared_brain, shared_world, agents = demonstrate_advanced_neural_integration()
        
        # Summary
        print("\n" + "="*80)
        print("NEURAL PROCESSING PIPELINE DEMONSTRATION SUMMARY")
        print("="*80)
        print(f"✓ Vision processing pipeline: {len(vision_features)} features extracted")
        print(f"✓ Audio processing pipeline: {len(audio_features)} features extracted")
        print(f"✓ Internal signals pipeline: {len(homeostasis)} homeostasis signals")
        print(f"✓ Integrated sensory percept: {len(all_signals)} total input signals")
        print(f"✓ Multi-agent system: {len(agents)} agents with shared brain")
        print("\nThe enhanced Python bindings provide:")
        print("  - Neural-based vision processing with feature extraction")
        print("  - Neural-based audio processing with pattern recognition")
        print("  - Internal signals processing for homeostasis monitoring")
        print("  - Complete integration of all sensory modalities")
        print("  - Multi-agent neural sharing and coordination")
        print("  - Advanced neural processing capabilities")
        
    except Exception as e:
        print(f"\nError during demonstration: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())