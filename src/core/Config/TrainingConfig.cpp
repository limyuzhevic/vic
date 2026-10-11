#include "TrainingConfig.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct TrainingConfig::Impl {
    double learningRate = 0.001;
    int batchSize = 32;
    int epochs = 100;
    std::string optimizerType = "adam";
    double l1Regularization = 0.0;
    double l2Regularization = 0.0;
    double dropoutRate = 0.1;
    int earlyStoppingPatience = 10;
    std::string learningRule = "stdp";
    std::string learningRateSchedule = "exponential";
    std::string minibatchScheduling = "sequential";
};

TrainingConfig::TrainingConfig() {
    pImplTraining = std::make_unique<Impl>();
    
    // Set up default configuration
    setLearningRate(pImplTraining->learningRate);
    setBatchSize(pImplTraining->batchSize);
    setEpochs(pImplTraining->epochs);
    setOptimizerType(pImplTraining->optimizerType);
    
    setL1Regularization(pImplTraining->l1Regularization);
    setL2Regularization(pImplTraining->l2Regularization);
    setDropoutRate(pImplTraining->dropoutRate);
    setEarlyStoppingPatience(pImplTraining->earlyStoppingPatience);
    
    setLearningRule(pImplTraining->learningRule);
    setLearningRateSchedule(pImplTraining->learningRateSchedule);
    setMinibatchScheduling(pImplTraining->minibatchScheduling);
}

TrainingConfig::~TrainingConfig() = default;

TrainingConfig::TrainingConfig(TrainingConfig&&) noexcept = default;
TrainingConfig& TrainingConfig::operator=(TrainingConfig&&) noexcept = default;

void TrainingConfig::setLearningRate(double rate) {
    pImplTraining->learningRate = rate;
    set("tr_learning_rate", rate, ConfigSource::Runtime,
        "Learning rate for training", 
        TRAINING_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void TrainingConfig::setBatchSize(int size) {
    pImplTraining->batchSize = size;
    set("tr_batch_size", size, ConfigSource::Runtime,
        "Batch size for training", 
        TRAINING_CATEGORY, TYPE_INT, 1, 10000);
}

void TrainingConfig::setEpochs(int epochs) {
    pImplTraining->epochs = epochs;
    set("tr_epochs", epochs, ConfigSource::Runtime,
        "Number of training epochs", 
        TRAINING_CATEGORY, TYPE_INT, 1, 10000);
}

void TrainingConfig::setOptimizerType(const std::string& optimizer) {
    pImplTraining->optimizerType = optimizer;
    set("tr_optimizer_type", optimizer, ConfigSource::Runtime,
        "Optimizer type for training", 
        TRAINING_CATEGORY, TYPE_STRING);
}

void TrainingConfig::setL1Regularization(double lambda) {
    pImplTraining->l1Regularization = lambda;
    set("tr_l1_regularization", lambda, ConfigSource::Runtime,
        "L1 regularization strength", 
        TRAINING_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void TrainingConfig::setL2Regularization(double lambda) {
    pImplTraining->l2Regularization = lambda;
    set("tr_l2_regularization", lambda, ConfigSource::Runtime,
        "L2 regularization strength", 
        TRAINING_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void TrainingConfig::setDropoutRate(double rate) {
    pImplTraining->dropoutRate = rate;
    set("tr_dropout_rate", rate, ConfigSource::Runtime,
        "Dropout rate for regularization", 
        TRAINING_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void TrainingConfig::setEarlyStoppingPatience(int patience) {
    pImplTraining->earlyStoppingPatience = patience;
    set("tr_early_stopping_patience", patience, ConfigSource::Runtime,
        "Patience for early stopping (epochs)", 
        TRAINING_CATEGORY, TYPE_INT, 0, 100);
}

void TrainingConfig::setLearningRule(const std::string& rule) {
    pImplTraining->learningRule = rule;
    set("tr_learning_rule", rule, ConfigSource::Runtime,
        "Learning rule for synaptic updates", 
        TRAINING_CATEGORY, TYPE_STRING);
}

void TrainingConfig::setLearningRateSchedule(const std::string& schedule) {
    pImplTraining->learningRateSchedule = schedule;
    set("tr_learning_rate_schedule", schedule, ConfigSource::Runtime,
        "Learning rate schedule type", 
        TRAINING_CATEGORY, TYPE_STRING);
}

void TrainingConfig::setMinibatchScheduling(const std::string& scheduling) {
    pImplTraining->minibatchScheduling = scheduling;
    set("tr_minibatch_scheduling", scheduling, ConfigSource::Runtime,
        "Minibatch scheduling strategy", 
        TRAINING_CATEGORY, TYPE_STRING);
}

std::string TrainingConfig::getTrainingSummary() const {
    std::ostringstream oss;
    oss << "Training Configuration:";
    oss << "\n  Learning rate: " << pImplTraining->learningRate;
    oss << "\n  Batch size: " << pImplTraining->batchSize;
    oss << "\n  Epochs: " << pImplTraining->epochs;
    oss << "\n  Optimizer: " << pImplTraining->optimizerType;
    oss << "\n  L1 regularization: " << pImplTraining->l1Regularization;
    oss << "\n  L2 regularization: " << pImplTraining->l2Regularization;
    oss << "\n  Dropout rate: " << pImplTraining->dropoutRate;
    oss << "\n  Early stopping patience: " << pImplTraining->earlyStoppingPatience;
    oss << "\n  Learning rule: " << pImplTraining->learningRule;
    oss << "\n  Learning rate schedule: " << pImplTraining->learningRateSchedule;
    oss << "\n  Minibatch scheduling: " << pImplTraining->minibatchScheduling;
    oss << "\n  Validation: " << (validateWithErrors().empty() ? "PASS" : "FAIL");
    return oss.str();
}

} // namespace nlm
