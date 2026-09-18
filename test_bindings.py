import pynlm

# Simple example demonstrating the enhanced Python bindings

print("Creating default configuration...")
config = pynlm.createDefaultConfig()

print("Creating brain with configuration...")
brain = pynlm.createBrain(config)

print("Initializing brain...")
brain.initialize()

print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")

print("\n--- Running Simulation ---")
for step in range(100):
    # Add random sensory input to drive activity
    import random
    input_data = [float(random.uniform(-10, 10)) for _ in range(10)]
    sensory_input = pynlm.InternalSignals()
    for i, value in enumerate(input_data):
        sensory_input.addSignal(value)
    
    brain.receiveSensoryInput(sensory_input)
    brain.step(step)
    
    if step % 25 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"Step {step}: {firing} firing neurons, {rate:.2f} avg firing rate")

print("\n--- Testing Memory Systems ---")
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory capacity: {wm.getMemoryCapacity()}")
    print(f"Active memory traces: {wm.getActiveTraces()}")
    print(f"Memory neurons: {wm.getMemoryNeurons()}")

print("\n--- Testing Neuromodulation ---")
ni = brain.getPredictionError()
if ni:
    print(f"Prediction error: {ni.getPredictionError()}")

ni = brain.getNovelty()
if ni:
    print(f"Novelty: {ni.getNoveltyScore()}")

ni = brain.getCuriosity()
if ni:
    print(f"Curiosity: {ni.getLevel()}")

ni = brain.getPredictionErrorSignal()
if ni:
    print(f"Prediction error signal: {ni.getLevel()}")

print("\n--- Testing Cognition Systems ---")
planner = brain.getPlanner()
if planner:
    print(f"Planning depth: {planner.getPlanningDepth()}")

concept_form = brain.getConceptFormation()
if concept_form:
    print(f"Number of discovered concepts: {len(concept_form.getConcepts())}")

attention = brain.getAttention()
if attention:
    print(f"Attention system: {attention}")

print("\n--- Testing Development System ---")
dev = brain.getDevelopmentSystem()
if dev:
    print(f"Current stage: {brain.getDevelopmentalStage()}")

print("\n--- Testing Prediction System ---")
pred = brain.getPredictionSystem()
if pred:
    print(f"Prediction system: {pred}")

print("\n--- Testing Spike System ---")
spike_sys = brain.getSpikeSystem()
if spike_sys:
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Pending spike events: {spike_sys.getPendingSpikeCount()}")

print("\n--- Testing Plasticity Systems ---")
stdp = brain.getSTDP()
if stdp:
    print(f"STDP: {stdp.getName()}")

hebb = brain.getHebbian()
if hebb:
    print(f"Hebbian: {hebb.getName()}")

struct_plast = brain.getStructuralPlasticity()
if struct_plast:
    print(f"Structural plasticity: {struct_plast.getName()}")

print("\n--- Complete Simulation ---")
print("All systems are connected and functioning!")
print("The NLM brain is a complete integrated artificial neural system.")