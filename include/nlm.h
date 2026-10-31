#pragma once

// Core headers for NLM
#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"

// Sensory headers
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"

// Motor headers
#include "motor/Action.hpp"
#include "motor/MotorSystem.hpp"

// Environment headers
#include "environment/Environment.hpp"
#include "environment/WorldObject.hpp"
#include "environment/Observation.hpp"

// Agent headers
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"

// World headers
#include "world/SimpleWorld.hpp"

// Development headers
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"

// Neuromodulation headers
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"

// Memory headers
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/ProceduralMemory.hpp"
#include "memory/AssociativeMemory.hpp"

// Prediction headers
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"

// Cognition headers
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"

// Dynamics headers
#include "dynamics/NeuralDynamics.hpp"
#include "dynamics/SpikeSystem.hpp"

// Plasticity headers
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"

// Visualization headers
#include "visualization/VisualizationInterface.hpp"

// Experiments headers
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
