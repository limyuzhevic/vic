#pragma once

#include <functional>
#include <string>
#include <chrono>

namespace nlm {

// Forward declarations
class SensoryInput;
class TrainingData;

// Simulation options for controlling brain simulation behavior
struct SimulationOptions {
    // Basic simulation control
    size_t maxSteps = 10000;              // Maximum number of simulation steps
    size_t timeout = 0;                   // Timeout in milliseconds (0 = no timeout)
    bool earlyTermination = false;        // Whether to terminate early based on conditions
    
    // Output control
    bool verbose = false;                 // Enable verbose logging
    bool saveResults = false;             // Save simulation results to file
    
    // Early termination predicate
    std::function<bool(const SimulationResult&)> earlyTerminationPredicate;
    
    // Monitoring control
    bool enableMonitoring = false;         // Enable real-time monitoring
    size_t monitoringInterval = 100;       // Monitoring interval in steps
    
    // Resource control
    size_t maxMemoryMB = 1024;            // Maximum memory usage in MB
    float maxCPUUsage = 0.95f;            // Maximum CPU usage (0.0 - 1.0)
    
    SimulationOptions() : maxSteps(10000), timeout(0), earlyTermination(false),
                         verbose(false), saveResults(false), earlyTerminationPredicate(nullptr),
                         enableMonitoring(false), monitoringInterval(100),
                         maxMemoryMB(1024), maxCPUUsage(0.95f) {}
};

// Training options for supervised learning
struct TrainingOptions {
    // Training data control
    size_t batchSize = 32;                // Number of samples per batch
    size_t epochs = 10;                   // Number of training epochs
    bool shuffleData = true;              // Whether to shuffle training data
    
    // Learning parameters
    float learningRate = 0.01f;           // Learning rate
    float regularization = 0.001f;        // L2 regularization coefficient
    float dropoutRate = 0.5f;             // Dropout rate for neural networks
    
    // Optimization
    std::string optimizer = "adam";      // Optimizer type (adam, sgd, rmsprop)
    float momentum = 0.9f;                // Momentum for SGD
    float beta1 = 0.9f;                   // Adam beta1
    float beta2 = 0.999f;                 // Adam beta2
    float epsilon = 1e-8f;                // Adam epsilon for numerical stability
    
    // Stopping criteria
    float minImprovement = 1e-4f;         // Minimum improvement to continue training
    size_t patience = 100;                // Patience for early stopping
    size_t maxNoImprovement = 50;        // Maximum steps without improvement
    
    // Output control
    bool verbose = false;                 // Enable verbose training output
    bool saveModel = false;               // Save trained model to file
    size_t evaluationInterval = 10;       // How often to evaluate on validation set
    
    // Learning behavior
    bool useCurriculum = false;            // Use curriculum learning
    float curriculumStart = 0.1f;         // Start difficulty for curriculum
    float curriculumEnd = 1.0f;           // End difficulty for curriculum
    
    SimulationOptions simulationOptions;  // Embedded simulation options
    
    TrainingOptions() : batchSize(32), epochs(10), shuffleData(true),
                       learningRate(0.01f), regularization(0.001f), dropoutRate(0.5f),
                       optimizer("adam"), momentum(0.9f), beta1(0.9f), beta2(0.999f), epsilon(1e-8f),
                       minImprovement(1e-4f), patience(100), maxNoImprovement(50),
                       verbose(false), saveModel(false), evaluationInterval(10),
                       useCurriculum(false), curriculumStart(0.1f), curriculumEnd(1.0f) {}
};

// Evaluation options for testing model performance
struct EvaluationOptions {
    // Evaluation data
    size_t batchSize = 64;                // Evaluation batch size
    bool shuffle = true;                  // Whether to shuffle evaluation data
    
    // Metrics computation
    bool computeAccuracy = true;           // Compute classification accuracy
    bool computePrecision = true;          // Compute precision
    bool computeRecall = true;             // Compute recall
    bool computeF1Score = true;            // Compute F1 score
    bool computeAUC = false;               // Compute Area Under Curve
    bool computeConfusionMatrix = false;   // Compute confusion matrix
    
    // Performance metrics
    float minAccuracy = 0.0f;              // Minimum acceptable accuracy
    float maxError = 1.0f;                 // Maximum acceptable error
    
    // Output control
    bool verbose = false;                 // Enable verbose evaluation output
    bool saveReport = false;               // Save evaluation report to file
    std::string reportFormat = "json";     // Report format (json, csv, txt)
    
    // Cross-validation
    bool crossValidation = false;          // Perform cross-validation
    size_t cvFolds = 5;                    // Number of cross-validation folds
    bool stratified = true;                // Use stratified sampling
    
    SimulationOptions simulationOptions;  // Embedded simulation options
    
    EvaluationOptions() : batchSize(64), shuffle(true),
                         computeAccuracy(true), computePrecision(true), computeRecall(true),
                         computeF1Score(true), computeAUC(false), computeConfusionMatrix(false),
                         minAccuracy(0.0f), maxError(1.0f),
                         verbose(false), saveReport(false), reportFormat("json"),
                         crossValidation(false), cvFolds(5), stratified(true) {}
};

// Batch processing options for large-scale simulation
struct BatchOptions {
    // Batch control
    size_t batchSize = 10;                // Number of sequences per batch
    size_t maxBatches = 0;                // Maximum number of batches (0 = unlimited)
    bool processSequences = true;          // Process sequences as units
    
    // Timing and synchronization
    size_t timeoutPerSequence = 0;       // Timeout per sequence in milliseconds
    bool parallelProcessing = false;       // Enable parallel processing
    size_t numThreads = 4;                 // Number of threads for parallel processing
    
    // Resource management
    bool enableCheckpointing = false;      // Enable checkpointing for large batches
    std::string checkpointDir = "./checkpoints";  // Checkpoint directory
    size_t maxCheckpointSizeMB = 1000;     // Maximum checkpoint size in MB
    
    // Output control
    bool verbose = false;                 // Enable verbose batch output
    bool saveBatchResults = false;         // Save batch results to file
    
    // Performance optimization
    bool vectorization = true;             // Enable SIMD vectorization
    bool caching = true;                   // Enable caching of intermediate results
    float compressionRatio = 0.8f;         // Compression ratio for storing results
    
    BatchOptions() : batchSize(10), maxBatches(0), processSequences(true),
                    timeoutPerSequence(0), parallelProcessing(false), numThreads(4),
                    enableCheckpointing(false), checkpointDir("./checkpoints"),
                    maxCheckpointSizeMB(1000), verbose(false), saveBatchResults(false),
                    vectorization(true), caching(true), compressionRatio(0.8f) {}
};

// Checkpoint options for save/load operations
struct CheckpointOptions {
    // Checkpoint control
    bool compress = true;                  // Enable compression
    std::string compressionAlgorithm = "zstd";  // Compression algorithm (zstd, gzip, none)
    int compressionLevel = 3;             // Compression level (1-22 for zstd)
    
    // Metadata
    bool saveMetadata = true;              // Save metadata (timestamp, config, etc.)
    bool saveStatistics = true;            // Save statistics (spike counts, firing rates, etc.)
    bool saveDebugInfo = false;            // Save debug information
    
    // Validation
    bool validateChecksum = true;          // Validate checksum on load
    bool autoValidate = true;              // Automatically validate checkpoints
    
    // Performance
    bool asyncIO = true;                   // Use asynchronous I/O
    bool bufferedIO = true;                // Use buffered I/O
    size_t bufferSize = 64 * 1024;         // Buffer size in bytes (64KB)
    
    // Error handling
    bool retryOnFailure = true;             // Retry failed operations
    size_t maxRetries = 3;                 // Maximum number of retries
    double retryDelay = 0.1;               // Delay between retries in seconds
    
    // Security
    bool encrypt = false;                   // Encrypt checkpoint files
    std::string encryptionKey = "";        // Encryption key (empty = no encryption)
    
    CheckpointOptions() : compress(true), compressionAlgorithm("zstd"), compressionLevel(3),
                          saveMetadata(true), saveStatistics(true), saveDebugInfo(false),
                          validateChecksum(true), autoValidate(true), asyncIO(true),
                          bufferedIO(true), bufferSize(64 * 1024), retryOnFailure(true),
                          maxRetries(3), retryDelay(0.1), encrypt(false), encryptionKey("") {}
};

// Monitoring configuration

} // namespace nlm