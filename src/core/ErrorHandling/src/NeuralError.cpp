// NeuralError implementation
namespace nlm {

NeuralError::NeuralError(const std::string& message,
                        NeuralErrorType type,
                        const std::string& operation,
                        const std::string& suggestion)
    : NLMError("Neural Error: " + message, ErrorSeverity::Critical, ErrorCategory::Neural),
      neuralErrorType_(type),
      operation_(operation),
      suggestion_(suggestion) {
    setRecoveryActions();
}

std::string NeuralError::getRecoveryHint() const {
    switch (neuralErrorType_) {
        case NeuralErrorType::NeuronInitializationFailed:
            return "Check neuron parameters. Use default parameters or check initialization logic.";
        case NeuralErrorType::SynapseCreationFailed:
            return "Check synapse parameters. Verify connection parameters and plasticity rules.";
        case NeuralErrorType::NetworkTopologyError:
            return "Check network topology. Verify neuron connections and region configuration.";
        case NeuralErrorType::SpikePropagationFailed:
            return "Check spike propagation logic. Verify neuron dynamics and connectivity.";
        case NeuralErrorType::PlasticityRuleFailed:
            return "Check plasticity rule parameters. Verify STDP or Hebbian rule parameters.";
        case NeuralErrorType::MemoryAllocationFailed:
            return "Check memory allocation. Increase memory pool or reduce network size.";
        case NeuralErrorType::ConvergenceFailed:
            return "Increase iterations or adjust learning rate. Check convergence criteria.";
        case NeuralErrorType::TrainingFailed:
            return "Check training data. Verify training parameters and check for learning issues.";
        case NeuralErrorType::InferenceFailed:
            return "Check inference input. Verify input format and check inference logic.";
        case NeuralErrorType::WeightUpdateFailed:
            return "Check weight update logic. Verify synaptic update rules and parameters.";
        case NeuralErrorType::ActivationFunctionError:
            return "Check activation function parameters. Verify function implementation and parameters.";
        default:
            return "Check neural network parameters and logic. Verify implementation and configuration.";
    }
}

} // namespace nlm