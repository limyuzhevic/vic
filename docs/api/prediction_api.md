# NLM Prediction System API Reference

## Overview

The Prediction System API provides access to NLM's comprehensive prediction capabilities. NLM implements multiple prediction systems that work together to enable intelligent behavior:

1. **Temporal Prediction** - Learn temporal sequences and predict future states
2. **Action Consequence Prediction** - Understand effects of actions on environment
3. **Prediction Error Signals** - Monitor and use prediction accuracy
4. **Confidence Management** - Track prediction uncertainty
5. **Planning Integration** - Use predictions for action selection
6. **Adaptive Prediction** - Update predictions based on experience

## Temporal Prediction API

### NeuralPrediction Class

The `NeuralPrediction` class (in `src/prediction/NeuralPrediction.hpp` and `src/prediction/NeuralPrediction.cpp`) provides temporal sequence prediction capabilities.

#### NeuralPrediction(std::shared_ptr<Brain> brain)

**Description**: Creates a NeuralPrediction instance with reference to brain.

**Parameters**:
- `brain`: Reference to brain instance

**Example**:
```cpp
auto predictionSystem = std::make_shared<NeuralPrediction>(brain);
```

#### std::vector<float> predictNextEvents(const std::vector<float>& state) const

**Description**: Predicts next events based on current state.

**Parameters**:
- `state`: Current neural state (typically 5-10 elements)

**Returns**: Vector of predicted future states.

**Example**:
```cpp
std::vector<float> current_state = {0.5f, 0.7f, 0.3f, 0.8f, 0.4f};
std::vector<float> predictions = predictionSystem->predictNextEvents(current_state);
```

#### float getPredictionError() const

**Description**: Gets current prediction error.

**Returns**: Absolute prediction error (0.0-1.0).

**Example**:
```cpp
float error = predictionSystem->getPredictionError();
```

#### float getPredictionConfidence() const

**Description**: Gets confidence in current predictions.

**Returns**: Prediction confidence (0.0-1.0).

**Example**:
```cpp
float confidence = predictionSystem->getPredictionConfidence();
```

#### float getPredictionAccuracy() const

**Description**: Gets overall prediction accuracy.

**Returns**: Prediction accuracy (0.0-1.0).

**Example**:
```cpp
float accuracy = predictionSystem->getPredictionAccuracy();
```

#### PredictionResults getPredictionResults() const

**Description**: Gets complete prediction results.

**Returns**: PredictionResults structure.

**Example**:
```cpp
PredictionResults results = predictionSystem->getPredictionResults();
```

#### void updatePrediction(const std::vector<float>& state, const std::vector<float>& target, float reward)

**Description**: Updates prediction based on actual outcome.

**Parameters**:
- `state`: Current state
- `target`: Actual outcome (target state)
- `reward`: Reward signal

**Example**:
```cpp
predictionSystem->updatePrediction(current_state, target_state, 1.0f);
```

#### void enablePredictionLearning(bool enable)

**Description**: Enables or disables prediction learning.

**Parameters**:
- `enable`: Whether to enable prediction learning

**Example**:
```cpp
predictionSystem->enablePredictionLearning(true);
```

#### void setPredictionHorizon(size_t horizon)

**Description**: Sets prediction time horizon.

**Parameters**:
- `horizon`: Number of steps to predict ahead

**Example**:
```cpp
predictionSystem->setPredictionHorizon(10);
```

#### void setPredictionConfidenceThreshold(float threshold)

**Description**: Sets minimum confidence for action selection.

**Parameters**:
- `threshold`: Minimum confidence (0.0-1.0)

**Example**:
```cpp
predictionSystem->setPredictionConfidenceThreshold(0.7f);
```

### PredictionResults Structure

```cpp
struct PredictionResults {
    std::vector<float> predictions;              // Predicted future states
    std::vector<float> prediction_errors;       // Errors for each prediction
    std::vector<float> confidences;             // Confidence for each prediction
    std::vector<float> rewards;                 // Expected rewards
    float overall_accuracy;                     // Overall prediction accuracy
    float average_confidence;                   // Average prediction confidence
    float prediction_error_magnitude;          // Overall error magnitude
    size_t horizon;                             // Prediction horizon
    float learning_rate;                        // Current learning rate
};
```

## Action Consequence Prediction API

### ActionConsequencePredictor Class

The `ActionConsequencePredictor` class (in `src/prediction/ActionConsequencePredictor.hpp` and `src/prediction/ActionConsequencePredictor.cpp`) provides action consequence prediction.

#### ActionConsequencePredictor(std::shared_ptr<Brain> brain)

**Description**: Creates ActionConsequencePredictor with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### std::vector<float> predictActionConsequences(const std::vector<float>& state) const

**Description**: Predicts consequences of possible actions.

**Parameters**:
- `state`: Current state

**Returns**: Predicted outcomes for each possible action.

**Example**:
```cpp
std::vector<float> consequences = predictor->predictActionConsequences(current_state);
```

#### ActionType selectBestAction(const std::vector<float>& consequences) const

**Description**: Selects best action based on predicted consequences.

**Parameters**:
- `consequences`: Predicted outcomes for each action

**Returns**: Best action to take.

**Example**:
```cpp
ActionType best_action = predictor->selectBestAction(consequences);
```

#### float getActionPredictionAccuracy() const

**Description**: Gets action prediction accuracy.

**Returns**: Action prediction accuracy (0.0-1.0).

**Example**:
```cpp
float accuracy = predictor->getActionPredictionAccuracy();
```

#### void updateActionPrediction(const std::vector<float>& state, ActionType action, float actual_outcome)

**Description**: Updates action prediction based on actual outcome.

**Parameters**:
- `state`: Current state
- `action`: Action taken
- `actual_outcome`: Actual outcome

**Example**:
```cpp
predictor->updateActionPrediction(state, ActionType::MoveForward, actual_outcome);
```

#### void enableActionPrediction(bool enable)

**Description**: Enables or disables action prediction.

**Parameters**:
- `enable`: Whether to enable action prediction

## Prediction Error API

### PredictionErrorSystem Class

The `PredictionErrorSystem` class (in `src/prediction/PredictionErrorSystem.hpp` and `src/prediction/PredictionErrorSystem.cpp`) provides prediction error monitoring and learning.

#### PredictionErrorSystem(std::shared_ptr<Brain> brain)

**Description**: Creates PredictionErrorSystem with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float calculatePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const

**Description**: Calculates prediction error between predicted and actual values.

**Parameters**:
- `predicted`: Predicted values
- `actual`: Actual values

**Returns**: Prediction error (absolute difference).

**Example**:
```cpp
float error = errorSystem->calculatePredictionError(predictions, actuals);
```

#### void applyPredictionErrorLearning(float error, const std::vector<float>& gradient)

**Description**: Applies prediction error for learning.

**Parameters**:
- `error`: Prediction error
- `gradient`: Gradient for learning update

**Example**:
```cpp
errorSystem->applyPredictionErrorLearning(error, gradient);
```

#### float getTemporalPredictionError() const

**Description**: Gets temporal prediction error.

**Returns**: Temporal prediction error (0.0-1.0).

**Example**:
```cpp
float temporal_error = errorSystem->getTemporalPredictionError();
```

#### float getOutcomePredictionError() const

**Description**: Gets outcome prediction error.

**Returns**: Outcome prediction error (0.0-1.0).

**Example**:
```cpp
float outcome_error = errorSystem->getOutcomePredictionError();
```

#### void enablePredictionErrorLearning(bool enable)

**Description**: Enables or disables prediction error learning.

**Parameters**:
- `enable`: Whether to enable prediction error learning

#### void setErrorGain(float gain)

**Description**: Sets prediction error gain.

**Parameters**:
- `gain`: Error gain (typically 0.5-2.0)

## Confidence Management API

### PredictionConfidence Class

The `PredictionConfidence` class (in `src/prediction/PredictionConfidence.hpp` and `src/prediction/PredictionConfidence.cpp`) provides confidence management for predictions.

#### PredictionConfidence(std::shared_ptr<Brain> brain)

**Description**: Creates PredictionConfidence with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float calculateConfidence(const std::vector<float>& predictions) const

**Description**: Calculates confidence for predicted values.

**Parameters**:
- `predictions`: Predicted values

**Returns**: Confidence score (0.0-1.0).

**Example**:
```cpp
float confidence = confidenceCalculator->calculateConfidence(predictions);
```

#### void updateConfidence(float confidence, float learning_rate)

**Description**: Updates confidence based on learning.

**Parameters**:
- `confidence`: Current confidence
- `learning_rate`: Learning rate for confidence update

**Example**:
```cpp
confidenceCalculator->updateConfidence(confidence, 0.01f);
```

#### float getPredictionConfidenceDistribution() const

**Description**: Gets distribution of prediction confidences.

**Returns**: Vector of confidence values for each prediction.

**Example**:
```cpp
std::vector<float> confidences = confidenceCalculator->getPredictionConfidenceDistribution();
```

#### void enableConfidenceAdaptation(bool enable)

**Description**: Enables or disables confidence adaptation.

**Parameters**:
- `enable`: Whether to enable confidence adaptation

#### void setConfidenceUncertainty(float uncertainty)

**Description**: Sets confidence uncertainty.

**Parameters**:
- `uncertainty`: Confidence uncertainty (0.0-1.0)

## Planning Integration API

### ActionPlanner Class

The `ActionPlanner` class (in `src/planning/ActionPlanner.hpp` and `src/planning/ActionPlanner.cpp`) provides action planning integration with predictions.

#### ActionPlanner(std::shared_ptr<Brain> brain, std::shared_ptr<NeuralPrediction> prediction)

**Description**: Creates ActionPlanner with brain and prediction references.

**Parameters**:
- `brain`: Reference to brain instance
- `prediction`: Reference to prediction system

#### std::vector<ActionType> planActions(const std::vector<float>& state, size_t horizon) const

**Description**: Plans sequence of actions based on predictions.

**Parameters**:
- `state`: Current state
- `horizon`: Planning horizon

**Returns**: Planned sequence of actions.

**Example**:
```cpp
std::vector<ActionType> plan = planner->planActions(current_state, 5);
```

#### float evaluateActionPlan(const std::vector<ActionType>& plan, const std::vector<float>& state) const

**Description**: Evaluates action plan effectiveness.

**Parameters**:
- `plan`: Action plan to evaluate
- `state`: Starting state

**Returns**: Plan evaluation score.

**Example**:
```cpp
float score = planner->evaluateActionPlan(plan, current_state);
```

#### void updatePlanningBasedOnPrediction(const std::vector<ActionType>& new_plan)

**Description**: Updates planning based on prediction changes.

**Parameters**:
- `new_plan`: Updated action plan

**Example**:
```cpp
planner->updatePlanningBasedOnPrediction(new_plan);
```

## Prediction Integration API

### Brain Prediction Operations

#### std::shared_ptr<NeuralPrediction> createPredictionSystem(std::shared_ptr<Brain> brain)

**Description**: Creates prediction system with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

**Returns**: Prediction system instance.

**Example**:
```cpp
auto prediction = createPredictionSystem(brain);
```

#### std::shared_ptr<ActionConsequencePredictor> createActionPredictor(std::shared_ptr<Brain> brain)

**Description**: Creates action consequence predictor with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

**Returns**: Action predictor instance.

#### bool isPredictionEnabled() const

**Description**: Checks if prediction system is enabled.

**Returns**: Whether prediction is enabled.

**Example**:
```cpp
bool prediction_enabled = brain->isPredictionEnabled();
```

#### void enablePrediction(bool enable)

**Description**: Enables or disables prediction system.

**Parameters**:
- `enable`: Whether to enable prediction

#### PredictionStats getPredictionStats() const

**Description**: Gets prediction system statistics.

**Returns**: Prediction statistics.

## Prediction Statistics API

### PredictionStats Structure

```cpp
struct PredictionStats {
    float overall_accuracy;              // Overall prediction accuracy (0.0-1.0)
    float average_confidence;            // Average prediction confidence (0.0-1.0)
    float temporal_prediction_error;    // Temporal prediction error (0.0-1.0)
    float outcome_prediction_error;      // Outcome prediction error (0.0-1.0)
    float learning_rate;                 // Current learning rate
    size_t prediction_count;             // Number of predictions made
    size_t successful_predictions;      // Number of successful predictions
    float prediction_horizon;            // Average prediction horizon
    float confidence_calibration;        // Confidence calibration accuracy (0.0-1.0)
};
```

### Prediction Statistics Operations

#### PredictionStats getPredictionStats() const

**Description**: Gets comprehensive prediction statistics.

**Returns**: Prediction statistics.

**Example**:
```cpp
PredictionStats stats = brain->getPredictionStats();
```

#### float calculatePredictionEfficiency() const

**Description**: Calculates prediction system efficiency.

**Returns**: Efficiency score (0.0-1.0).

**Example**:
```cpp
float efficiency = brain->calculatePredictionEfficiency();
```

#### size_t getTotalPredictionsMade() const

**Description**: Gets total number of predictions made.

**Returns**: Total predictions count.

**Example**:
```cpp
 size_t total_predictions = brain->getTotalPredictionsMade();
```

#### float getPredictionSuccessRate() const

**Description**: Gets prediction success rate.

**Returns**: Success rate (0.0-1.0).

**Example**:
```cpp
float success_rate = brain->getPredictionSuccessRate();
```

## Python Prediction API

The Python prediction API provides simplified access to NLM prediction systems:

### NeuralPrediction

```python
class NeuralPrediction:
    def __init__(self, brain):
        self.brain = brain
        
    def predict_next_events(self, state):
        # Predict next events based on current state
        
    def get_prediction_error(self):
        # Get current prediction error
        
    def get_prediction_confidence(self):
        # Get prediction confidence
        
    def get_prediction_results(self):
        # Get complete prediction results
```

### ActionConsequencePredictor

```python
class ActionConsequencePredictor:
    def __init__(self, brain):
        self.brain = brain
        
    def predict_action_consequences(self, state):
        # Predict consequences of actions
        
    def select_best_action(self, consequences):
        # Select best action based on predictions
        
    def get_action_prediction_accuracy(self):
        # Get action prediction accuracy
```

### PredictionErrorSystem

```python
class PredictionErrorSystem:
    def __init__(self, brain):
        self.brain = brain
        
    def calculate_prediction_error(self, predicted, actual):
        # Calculate prediction error
        
    def apply_prediction_error_learning(self, error, gradient):
        # Apply prediction error for learning
        
    def get_temporal_prediction_error(self):
        # Get temporal prediction error
        
    def get_outcome_prediction_error(self):
        # Get outcome prediction error
```

### PredictionConfidence

```python
class PredictionConfidence:
    def __init__(self, brain):
        self.brain = brain
        
    def calculate_confidence(self, predictions):
        # Calculate confidence for predictions
        
    def update_confidence(self, confidence, learning_rate):
        # Update confidence based on learning
        
    def get_prediction_confidence_distribution(self):
        # Get confidence distribution
```

### ActionPlanner

```python
class ActionPlanner:
    def __init__(self, brain, prediction):
        self.brain = brain
        self.prediction = prediction
        
    def plan_actions(self, state, horizon):
        # Plan sequence of actions
        
    def evaluate_action_plan(self, plan, state):
        # Evaluate action plan effectiveness
```

### PredictionManager

```python
class PredictionManager:
    def __init__(self, brain):
        self.brain = brain
        self.neural_prediction = NeuralPrediction(brain)
        self.action_consequence_predictor = ActionConsequencePredictor(brain)
        self.prediction_error_system = PredictionErrorSystem(brain)
        self.confidence_calculator = PredictionConfidence(brain)
        self.action_planner = ActionPlanner(brain, self.neural_prediction)
        
    def get_prediction_stats(self):
        # Get prediction system statistics
        
    def get_prediction_efficiency(self):
        # Get prediction system efficiency
```

## Prediction Configuration API

### PredictionConfig Structure

```cpp
struct PredictionConfig {
    bool enable_temporal_prediction;            // Enable temporal prediction
    bool enable_action_consequence_prediction;  // Enable action consequence prediction
    size_t prediction_horizon;                 // Prediction time horizon
    float prediction_confidence_threshold;     // Minimum confidence for action selection
    float prediction_error_gain;               // Prediction error gain
    float prediction_learning_rate;            // Prediction learning rate
    bool enable_prediction_error_learning;     // Enable prediction error learning
    bool enable_confidence_adaptation;         // Enable confidence adaptation
    float confidence_uncertainty;              // Confidence uncertainty
    bool enable_action_planning;               // Enable action planning
    size_t planning_horizon;                   // Planning horizon
    float action_evaluation_threshold;         // Action evaluation threshold
};
```

### Configuration Operations

#### PredictionConfig createDefaultPredictionConfig()

**Description**: Creates default prediction configuration.

**Returns**: Default prediction configuration.

#### PredictionConfig createResearchPredictionConfig()

**Description**: Creates prediction configuration optimized for research.

**Returns**: Research-oriented prediction configuration.

#### void applyPredictionConfig(const PredictionConfig& config)

**Description**: Applies prediction configuration to brain.

**Parameters**:
- `config`: Prediction configuration

## Prediction Example Programs

### Example 1: Temporal Prediction Demonstration

```cpp
// temporal_prediction_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "prediction/NeuralPrediction.hpp"

int main() {
    // Create brain and prediction system
    auto config = std::make_shared<Config>();
    config->set("prediction.enable", true);
    config->set("prediction.horizon", 5);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto predictionSystem = std::make_shared<NeuralPrediction>(brain);
    
    // Demonstrate temporal prediction
    std::cout << "Temporal Prediction Example" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Make predictions
    std::vector<float> state1 = {0.5f, 0.7f, 0.3f, 0.8f, 0.4f};
    std::vector<float> predictions = predictionSystem->predictNextEvents(state1);
    
    std::cout << "Current state: [0.5, 0.7, 0.3, 0.8, 0.4]" << std::endl;
    std::cout << "Predictions: ";
    for (float pred : predictions) {
        std::cout << pred << " ";
    }
    std::cout << std::endl;
    
    // Get prediction statistics
    float error = predictionSystem->getPredictionError();
    float confidence = predictionSystem->getPredictionConfidence();
    
    std::cout << "Prediction error: " << error << std::endl;
    std::cout << "Prediction confidence: " << confidence << std::endl;
    
    return 0;
}
```

### Example 2: Action Consequence Prediction Demonstration

```cpp
// action_consequence_prediction_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "prediction/ActionConsequencePredictor.hpp"

int main() {
    // Create brain and action predictor
    auto config = std::make_shared<Config>();
    config->set("prediction.enable", true);
    config->set("action_prediction.enable", true);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto actionPredictor = std::make_shared<ActionConsequencePredictor>(brain);
    
    // Demonstrate action consequence prediction
    std::cout << "Action Consequence Prediction Example" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Get current state
    std::vector<float> state = {0.5f, 0.7f, 0.3f, 0.8f, 0.4f};
    
    // Predict action consequences
    std::vector<float> consequences = actionPredictor->predictActionConsequences(state);
    
    std::cout << "State: [0.5, 0.7, 0.3, 0.8, 0.4]" << std::endl;
    std::cout << "Action consequences: ";
    for (float cons : consequences) {
        std::cout << cons << " ";
    }
    std::cout << std::endl;
    
    // Select best action
    ActionType best_action = actionPredictor->selectBestAction(consequences);
    std::cout << "Best action: " << static_cast<int>(best_action) << std::endl;
    
    // Get prediction accuracy
    float accuracy = actionPredictor->getActionPredictionAccuracy();
    std::cout << "Action prediction accuracy: " << accuracy << std::endl;
    
    return 0;
}
```

### Example 3: Prediction Error Learning Demonstration

```cpp
// prediction_error_learning_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "prediction/PredictionErrorSystem.hpp"

int main() {
    // Create brain and error system
    auto config = std::make_shared<Config>();
    config->set("prediction.enable", true);
    config->set("prediction_error.enable", true);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto errorSystem = std::make_shared<PredictionErrorSystem>(brain);
    
    // Demonstrate prediction error learning
    std::cout << "Prediction Error Learning Example" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Calculate prediction errors
    std::vector<float> predicted = {0.6f, 0.8f, 0.4f, 0.7f, 0.5f};
    std::vector<float> actual = {0.7f, 0.6f, 0.5f, 0.9f, 0.3f};
    
    float temporal_error = errorSystem->getTemporalPredictionError();
    float outcome_error = errorSystem->getOutcomePredictionError();
    
    std::cout << "Temporal prediction error: " << temporal_error << std::endl;
    std::cout << "Outcome prediction error: " << outcome_error << std::endl;
    
    // Apply prediction error learning
    std::vector<float> gradient = {0.1f, 0.2f, 0.1f, 0.1f, 0.1f};
    errorSystem->applyPredictionErrorLearning(temporal_error + outcome_error, gradient);
    
    std::cout << "Applied prediction error learning" << std::endl;
    
    return 0;
}
```

## Prediction API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Temporal Prediction | `NeuralPrediction` | `predictNextEvents()`, `getPredictionError()`, `getPredictionConfidence()` |
| Action Consequence Prediction | `ActionConsequencePredictor` | `predictActionConsequences()`, `selectBestAction()`, `getActionPredictionAccuracy()` |
| Prediction Error | `PredictionErrorSystem` | `calculatePredictionError()`, `applyPredictionErrorLearning()`, `getTemporalPredictionError()` |
| Confidence Management | `PredictionConfidence` | `calculateConfidence()`, `updateConfidence()`, `getPredictionConfidenceDistribution()` |
| Planning Integration | `ActionPlanner` | `planActions()`, `evaluateActionPlan()`, `updatePlanningBasedOnPrediction()` |
| Statistics | `PredictionStats` | `getPredictionStats()`, `calculatePredictionEfficiency()` |

## Python Prediction API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Temporal Prediction | `NeuralPrediction` | `predict_next_events()`, `get_prediction_error()`, `get_prediction_confidence()` |
| Action Consequence Prediction | `ActionConsequencePredictor` | `predict_action_consequences()`, `select_best_action()`, `get_action_prediction_accuracy()` |
| Prediction Error | `PredictionErrorSystem` | `calculate_prediction_error()`, `apply_prediction_error_learning()`, `get_temporal_prediction_error()` |
| Confidence Management | `PredictionConfidence` | `calculate_confidence()`, `update_confidence()`, `get_prediction_confidence_distribution()` |
| Planning Integration | `ActionPlanner` | `plan_actions()`, `evaluate_action_plan()`, `update_planning_based_on_prediction()` |
| Statistics | `PredictionManager` | `get_prediction_stats()`, `get_prediction_efficiency()` |

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/prediction_system.cpp`
2. **Explore Documentation**: Read `configuration.md` for prediction configuration details
3. **Run Demos**: Execute prediction-specific demo programs
4. **Create Experiments**: Design prediction-specific NLM experiments
5. **Extend Prediction**: Add new prediction features to NLM

The Prediction API provides comprehensive access to NLM's prediction capabilities. Whether you're interested in temporal prediction, action consequence prediction, prediction error signals, confidence management, or planning integration, the API provides the tools you need to create sophisticated prediction experiments.

---

*Prediction System API Reference*
- **Location**: `docs/api/prediction_api.md`
- **Source**: Various prediction system headers in `src/prediction/`
- **Python Bindings**: Prediction API in Python bindings
- **Documentation**: Complete prediction API reference with examples

*Ready to start using the Prediction API?*

1. **Read the examples**: Check `docs/examples/prediction_system.cpp`
2. **Try Python API**: Use `configuration.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom prediction NLM experiments
5. **Contribute**: Share your prediction API usage with the community

The Prediction API is essential for NLM's intelligent behavior. It provides access to all prediction systems that enable NLM to:
- **Anticipate Future Events**: Predict what will happen next
- **Understand Action Effects**: Learn consequences of actions
- **Monitor Prediction Accuracy**: Track prediction performance
- **Manage Confidence**: Track prediction uncertainty
- **Plan Actions**: Use predictions for action selection
- **Learn from Errors**: Use prediction errors to improve performance

Your prediction API research will help advance our understanding of how prediction capabilities support intelligent behavior!

---

*Prediction API enables NLM to:
• Predict future events using temporal prediction
• Understand action consequences using action consequence prediction
• Monitor prediction errors for learning
• Manage prediction confidence for uncertainty
• Plan actions based on predictions
• Improve through prediction error learning

Your prediction API research contributions will help advance neural system research!

---