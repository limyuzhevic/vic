import pynlm

print("\n=== Neuromodulation Example ===")

brain = pynlm.createBrain()

with brain:
    # Get neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print("Initial Neuromodulation Levels:")
    print(f"  Dopamine: {dopamine.getLevel():.3f}")
    print(f"  Curiosity: {curiosity.getLevel():.3f}")
    print(f"  Novelty: {novelty.getLevel():.3f}")
    print(f"  Prediction Error: {prediction_error.getError():.3f}")
    
    # Simulate some neuromodulation activity
    print("\nSimulating neuromodulation activity...")
    
    # Apply reward modulation
    reward_mod = pynlm.Neuromodulator(pynlm.NeuromodulatorType.Reward, 0.5)
    brain.applyNeuromodulation(reward_mod)
    
    print(f"After reward modulation:")
    print(f"  Dopamine: {dopamine.getLevel():.3f}")
    
    # Apply curiosity modulation
    curiosity_mod = pynlm.Neuromodulator(pynlm.NeuromodulatorType.Curiosity, 0.3)
    brain.applyNeuromodulation(curiosity_mod)
    
    print(f"After curiosity modulation:")
    print(f"  Curiosity: {curiosity.getLevel():.3f}")
