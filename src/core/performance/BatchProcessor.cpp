#include "BatchProcessor.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

BatchProcessor::BatchProcessor(size_t defaultBatchSize)
    : defaultBatchSize_(defaultBatchSize)
    , batchProcessingEnabled_(true)
    , lastBatchTime_(std::chrono::steady_clock::now())
{
    // Initialize statistics
    stats_.totalBatches = 0;
    stats_.completedBatches = 0;
    stats_.pendingBatches = 0;
}

BatchProcessor::~BatchProcessor() {
    // Clean up is automatic
}

template<typename Operation>
void BatchProcessor::addOperation(std::unique_ptr<Operation> operation) {
    if (!operation || !batchProcessingEnabled_) return;
    
    // Check if operation should be batched
    if (shouldBatchOperation(*operation)) {
        pendingOperations_.push_back(std::move(operation));
    } else {
        // Execute immediately
        executeBatchOperation(*operation);
    }
}

void BatchProcessor::addNeuronBatchOperation(const NeuronBatchOperation& operation) {
    // Convert to a generic batch operation
    auto neuronOp = std::make_unique<NeuronBatchOperation>(operation);
    addOperation(std::move(neuronOp));
}

void BatchProcessor::addSynapseBatchOperation(const SynapseBatchOperation& operation) {
    // Convert to a generic batch operation
    auto synapseOp = std::make_unique<SynapseBatchOperation>(operation);
    addOperation(std::move(synapseOp));
}

void BatchProcessor::addSTDPBatchOperation(const STDPBatchOperation& operation) {
    // Convert to a generic batch operation
    auto stdpOp = std::make_unique<STDPBatchOperation>(operation);
    addOperation(std::move(stdpOp));
}

void BatchProcessor::processReadyOperations() {
    if (!batchProcessingEnabled_) return;
    
    // Group operations into batches
    groupOperationsIntoBatches();
    
    // Execute all ready batches
    for (auto& batch : readyBatches_) {
        executeBatchOperation(*batch);
        stats_.completedBatches++;
    }
    
    // Clear ready batches
    readyBatches_.clear();
    
    // Update timing
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastBatchTime_).count();
    lastBatchTime_ = now;
    
    if (elapsed > 0) {
        stats_.avgBatchTime = stats_.totalBatchTime / stats_.completedBatches;
    }
}

bool BatchProcessor::hasPendingOperations() const {
    return !pendingOperations_.empty() || !readyBatches_.empty();
}

size_t BatchProcessor::getPendingOperationCount() const {
    return pendingOperations_.size() + readyBatches_.size();
}

BatchStats BatchProcessor::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void BatchProcessor::setBatchSize(size_t batchSize) {
    defaultBatchSize_ = batchSize;
    
    // Regroup operations with new batch size
    groupOperationsIntoBatches();
}

std::unique_ptr<BatchOperation> BatchProcessor::getNextReadyBatch() {
    if (readyBatches_.empty()) return nullptr;
    
    auto batch = std::move(readyBatches_.back());
    readyBatches_.pop_back();
    return batch;
}

void BatchProcessor::reset() {
    pendingOperations_.clear();
    readyBatches_.clear();
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_.totalBatches = 0;
    stats_.completedBatches = 0;
    stats_.pendingBatches = 0;
    stats_.totalBatchTime = 0.0;
    stats_.avgBatchTime = 0.0;
    stats_.operationsProcessed = 0;
    stats_.operationsCompleted = 0;
}

void BatchProcessor::cleanup() {
    // Reset all operations
    for (auto& operation : pendingOperations_) {
        operation->reset();
    }
    
    for (auto& batch : readyBatches_) {
        batch->reset();
    }
    
    pendingOperations_.clear();
    readyBatches_.clear();
}

size_t BatchProcessor::calculateOptimalBatchSize() const {
    // Simple heuristic based on available operations
    size_t operationCount = getPendingOperationCount();
    
    if (operationCount < 10) {
        return 1;
    } else if (operationCount < 50) {
        return 10;
    } else if (operationCount < 100) {
        return 25;
    } else if (operationCount < 500) {
        return 50;
    } else {
        return 100;
    }
}

void BatchProcessor::groupOperationsIntoBatches() {
    // Simple grouping strategy
    std::vector<std::unique_ptr<BatchOperation>> groupedBatches;
    
    // Group by operation type
    std::vector<std::unique_ptr<BatchOperation>> neuronOps;
    std::vector<std::unique_ptr<BatchOperation>> synapseOps;
    std::vector<std::unique_ptr<BatchOperation>> stdpOps;
    
    for (auto& operation : pendingOperations_) {
        switch (operation->getType()) {
            case BatchOperationType::NEURON_UPDATE:
                neuronOps.push_back(std::move(operation));
                break;
            case BatchOperationType::SYNAPSE_UPDATE:
                synapseOps.push_back(std::move(operation));
                break;
            case BatchOperationType::STDP_CALCULATION:
                stdpOps.push_back(std::move(operation));
                break;
            default:
                groupedBatches.push_back(std::move(operation));
                break;
        }
    }
    
    // Process each operation type
    for (auto& ops : {neuronOps, synapseOps, stdpOps}) {
        if (ops.empty()) continue;
        
        // Sort operations by estimated cost
        std::sort(ops.begin(), ops.end(),
                 [](const auto& a, const auto& b) {
                     return a->getEstimatedCost() < b->getEstimatedCost();
                 });
        
        // Group into batches
        while (!ops.empty()) {
            auto batch = std::make_unique<NeuronBatchOperation>();  // Simplified - use appropriate type
            
            size_t batchSize = 0;
            double batchCost = 0.0;
            
            // Add operations to batch until full
            while (!ops.empty() && batchSize < defaultBatchSize_) {
                auto& op = ops.back();
                
                // Check if operation fits in batch
                if (batchSize + op->getOperationCount() <= defaultBatchSize_ &&
                    batchCost + op->getEstimatedCost() <= 1.0) {  // Cost limit
                    
                    batch->addOperation(std::move(op));
                    batchSize += op->getOperationCount();
                    batchCost += op->getEstimatedCost();
                    ops.pop_back();
                } else {
                    break;
                }
            }
            
            if (batch->isReady()) {
                groupedBatches.push_back(std::move(batch));
            }
        }
    }
    
    // Add remaining operations
    for (auto& op : neuronOps) {
        groupedBatches.push_back(std::move(op));
    }
    
    for (auto& op : synapseOps) {
        groupedBatches.push_back(std::move(op));
    }
    
    for (auto& op : stdpOps) {
        groupedBatches.push_back(std::move(op));
    }
    
    // Update pending operations and ready batches
    pendingOperations_ = std::move(groupedBatches);
    readyBatches_ = std::move(pendingOperations_);
    pendingOperations_.clear();
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_.pendingBatches = readyBatches_.size();
}

void BatchProcessor::executeBatchOperation(BatchOperation& operation) {
    auto startTime = std::chrono::steady_clock::now();
    
    operation.execute();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_.totalBatches++;
    stats_.totalBatchTime += duration / 1000000.0; // Convert to seconds
    stats_.operationsCompleted += operation.getOperationCount();
}

bool BatchProcessor::shouldBatchOperation(const BatchOperation& operation) const {
    return batchProcessingEnabled_ && operation.isReady() &&
           (operation.getOperationCount() > 1 || operation.getEstimatedCost() > 0.1);
}

double BatchProcessor::estimateOperationCost(const BatchOperation& operation) const {
    // Simple cost estimation based on operation count and type
    double baseCost = static_cast<double>(operation.getOperationCount()) * 0.01;
    
    switch (operation.getType()) {
        case BatchOperationType::STDP_CALCULATION:
            return baseCost * 2.0;  // STDP is more expensive
        case BatchOperationType::NEURON_UPDATE:
            return baseCost;
        case BatchOperationType::SYNAPSE_UPDATE:
            return baseCost * 1.5;
        default:
            return baseCost;
    }
}

} // namespace nlm
