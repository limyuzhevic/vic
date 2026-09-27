pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace MemoryStructures {

// Compressed Sparse Row (CSR) matrix format for synaptic connections
struct CSRMatrix {
    // Column indices for each row (compressed)
    std::vector<uint32_t> columns;
    
    // Row pointers - columns[i] represents the range for row i
    // columns[rows[i]] to columns[rows[i+1]] - 1 contains non-zero entries
    std::vector<uint32_t> rows;
    
    // Values (weights)
    std::vector<float> values;
    
    // Metadata
    size_t numRows;
    size_t numColumns;
    size_t nonZeroCount;
    
    CSRMatrix() : numRows(0), numColumns(0), nonZeroCount(0) {}
    
    CSRMatrix(size_t rows, size_t cols) 
        : numRows(rows), numColumns(cols), nonZeroCount(0) {
        rows.resize(numRows + 1, 0);
    }
    
    void clear() {
        columns.clear();
        rows.assign(numRows + 1, 0);
        values.clear();
        nonZeroCount = 0;
    }
    
    // Add a connection (column to row)
    void addConnection(size_t row, size_t column, float weight) {
        if (row >= numRows || column >= numColumns) return;
        
        columns.push_back(static_cast<uint32_t>(column));
        values.push_back(weight);
        rows[row + 1]++;  // Increment column count for this row
        nonZeroCount++;
    }
    
    // Compress the matrix (build row pointers)
    void compress() {
        // Build prefix sum for row pointers
        for (size_t i = 1; i <= numRows; ++i) {
            rows[i] += rows[i - 1];
        }
        
        // Create temporary arrays for new compressed format
        std::vector<uint32_t> newColumns(nonZeroCount);
        std::vector<float> newValues(nonZeroCount);
        
        // Fill compressed columns and values
        std::vector<uint32_t> columnCounts = rows;
        
        for (size_t i = 0; i < columns.size(); ++i) {
            size_t row = 0;
            while (row < numRows && columnCounts[row] <= rows[row + 1]) {
                if (columnCounts[row] < rows[row + 1]) {
                    newColumns[columnCounts[row]] = columns[i];
                    newValues[columnCounts[row]] = values[i];
                    columnCounts[row]++;
                    break;
                }
                row++;
            }
        }
        
        // Replace old data with compressed data
        columns = std::move(newColumns);
        values = std::move(newValues);
        
        // Reset rows to start indices
        for (size_t i = numRows; i > 0; --i) {
            rows[i] = rows[i - 1];
        }
        rows[0] = 0;
    }
    
    // Get all columns for a specific row
    std::vector<std::pair<size_t, float>> getRow(size_t row) const {
        if (row >= numRows) return {};
        
        std::vector<std::pair<size_t, float>> result;
        size_t start = rows[row];
        size_t end = rows[row + 1];
        
        for (size_t i = start; i < end; ++i) {
            result.emplace_back(columns[i], values[i]);
        }
        
        return result;
    }
    
    // Get weight for specific connection
    float getWeight(size_t row, size_t column) const {
        size_t start = rows[row];
        size_t end = rows[row + 1];
        
        for (size_t i = start; i < end; ++i) {
            if (columns[i] == column) {
                return values[i];
            }
        }
        
        return 0.0f; // Default for non-existent connections
    }
    
    // Get memory usage in bytes
    size_t getMemoryUsage() const {
        size_t total = 0;
        total += columns.size() * sizeof(uint32_t);
        total += rows.size() * sizeof(uint32_t);
        total += values.size() * sizeof(float);
        return total;
    }
    
    // Calculate sparsity (ratio of non-zero entries)
    float getSparsity() const {
        if (numRows == 0 || numColumns == 0) return 1.0f;
        return 1.0f - static_cast<float>(nonZeroCount) / (numRows * numColumns);
    }
    
    // Sample distribution of connection weights
    std::string getWeightStats() const {
        std::stringstream ss;
        ss << "CSR Matrix Statistics:\n";
        ss << "  Dimensions: " << numRows << " x " << numColumns << "\n";
        ss << "  Non-zero elements: " << nonZeroCount << "\n";
        ss << "  Sparsity: " << (getSparsity() * 100.0f) << "%\n";
        ss << "  Memory usage: " << getMemoryUsage() << " bytes\n";
        
        if (!values.empty()) {
            float sum = std::accumulate(values.begin(), values.end(), 0.0f);
            float min = *std::min_element(values.begin(), values.end());
            float max = *std::max_element(values.begin(), values.end());
            float mean = sum / values.size();
            
            // Calculate standard deviation
            float sqSum = 0.0f;
            for (float val : values) {
                float diff = val - mean;
                sqSum += diff * diff;
            }
            float stddev = std::sqrt(sqSum / values.size());
            
            ss << "  Weight statistics:\n";
            ss << "    Min: " << min << "\n";
            ss << "    Max: " << max << "\n";
            ss << "    Mean: " << mean << "\n";
            ss << "    Stddev: " << stddev << "\n";
        }
        
        return ss.str();
    }
};

// Compressed Sparse Column (CSC) format - alternative storage
struct CSCMatrix {
    std::vector<uint32_t> rows;  // Row indices for each column
    std::vector<uint32_t> columns;  // Column pointers
    std::vector<float> values;
    
    size_t numRows;
    size_t numColumns;
    size_t nonZeroCount;
    
    CSCMatrix(size_t rows, size_t cols) : numRows(rows), numColumns(cols), nonZeroCount(0) {
        columns.resize(numColumns + 1, 0);
    }
    
    void clear() {
        rows.clear();
        columns.assign(numColumns + 1, 0);
        values.clear();
        nonZeroCount = 0;
    }
    
    void addConnection(size_t row, size_t column, float weight) {
        if (row >= numRows || column >= numColumns) return;
        
        rows.push_back(static_cast<uint32_t>(row));
        values.push_back(weight);
        columns[column + 1]++;
        nonZeroCount++;
    }
    
    void compress() {
        for (size_t i = 1; i <= numColumns; ++i) {
            columns[i] += columns[i - 1];
        }
        
        std::vector<uint32_t> newRows(nonZeroCount);
        std::vector<float> newValues(nonZeroCount);
        
        std::vector<uint32_t> rowCounts = columns;
        
        for (size_t i = 0; i < rows.size(); ++i) {
            size_t col = 0;
            while (col < numColumns && rowCounts[col] <= columns[col + 1]) {
                if (rowCounts[col] < columns[col + 1]) {
                    newRows[rowCounts[col]] = rows[i];
                    newValues[rowCounts[col]] = values[i];
                    rowCounts[col]++;
                    break;
                }
                col++;
            }
        }
        
        rows = std::move(newRows);
        values = std::move(newValues);
        
        for (size_t i = numColumns; i > 0; --i) {
            columns[i] = columns[i - 1];
        }
        columns[0] = 0;
    }
    
    float getWeight(size_t row, size_t column) const {
        size_t start = columns[column];
        size_t end = columns[column + 1];
        
        for (size_t i = start; i < end; ++i) {
            if (rows[i] == row) {
                return values[i];
            }
        }
        
        return 0.0f;
    }
    
    size_t getMemoryUsage() const {
        size_t total = 0;
        total += rows.size() * sizeof(uint32_t);
        total += columns.size() * sizeof(uint32_t);
        total += values.size() * sizeof(float);
        return total;
    }
};

// Run-Length Encoding for temporal data
struct RunLengthEncoder {
    struct RunEntry {
        float value;
        size_t runLength;
        uint64_t timestamp;
        
        RunEntry(float val, size_t length, uint64_t ts)
            : value(val), runLength(length), timestamp(ts) {}
    };
    
    std::vector<RunEntry> runs;
    size_t maxRunLength;
    
    RunLengthEncoder(size_t maxRun = 1000) : maxRunLength(maxRun) {}
    
    void encode(const std::vector<float>& data, uint64_t timestamp) {
        if (data.empty()) return;
        
        runs.clear();
        
        float currentValue = data[0];
        size_t runLength = 1;
        
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i] == currentValue && runLength < maxRunLength) {
                runLength++;
            } else {
                runs.emplace_back(currentValue, runLength, timestamp);
                currentValue = data[i];
                runLength = 1;
            }
        }
        
        runs.emplace_back(currentValue, runLength, timestamp);
    }
    
    std::vector<float> decode() const {
        std::vector<float> result;
        result.reserve(getTotalLength());
        
        for (const auto& run : runs) {
            result.insert(result.end(), run.runLength, run.value);
        }
        
        return result;
    }
    
    size_t getTotalLength() const {
        size_t total = 0;
        for (const auto& run : runs) {
            total += run.runLength;
        }
        return total;
    }
    
    size_t getMemoryUsage() const {
        size_t total = 0;
        for (const auto& run : runs) {
            total += sizeof(RunEntry);
        }
        return total;
    }
    
    float getCompressionRatio(const std::vector<float>& data) const {
        if (data.empty()) return 1.0f;
        size_t originalSize = data.size() * sizeof(float);
        size_t compressedSize = getMemoryUsage();
        return static_cast<float>(originalSize) / compressedSize;
    }
    
    std::string getStats() const {
        std::stringstream ss;
        ss << "Run-Length Encoding Statistics:\n";
        ss << "  Runs: " << runs.size() << "\n";
        ss << "  Total elements: " << getTotalLength() << "\n";
        ss << "  Memory usage: " << getMemoryUsage() << " bytes\n";
        
        if (!runs.empty()) {
            float avgRunLength = static_cast<float>(getTotalLength()) / runs.size();
            ss << "  Average run length: " << avgRunLength << "\n";
            
            size_t maxRun = 0;
            for (const auto& run : runs) {
                if (run.runLength > maxRun) maxRun = run.runLength;
            }
            ss << "  Maximum run length: " << maxRun << "\n";
        }
        
        return ss.str();
    }
};

// Block-based memory allocator for neural data
class BlockBasedAllocator {
private:
    struct MemoryBlock {
        void* data;
        size_t size;
        bool isFree;
        uint64_t blockId;
        
        MemoryBlock(void* ptr, size_t sz, uint64_t id)
            : data(ptr), size(sz), isFree(true), blockId(id) {}
    };
    
    std::vector<MemoryBlock> blocks;
    size_t blockSize;
    size_t totalMemory;
    uint64_t nextBlockId;
    
public:
    BlockBasedAllocator(size_t blockSize = 64 * 1024)  // 64KB blocks
        : blockSize(blockSize), totalMemory(0), nextBlockId(0) {}
    
    ~BlockBasedAllocator() {
        for (auto& block : blocks) {
            std::free(block.data);
        }
    }
    
    void* allocate(size_t size) {
        size_t alignedSize = ((size + 15) & ~15);  // 16-byte alignment
        size_t numBlocks = (alignedSize + blockSize - 1) / blockSize;
        size_t totalBlockSize = numBlocks * blockSize;
        
        // Find first suitable free block
        for (auto& block : blocks) {
            if (block.isFree && block.size >= totalBlockSize) {
                block.isFree = false;
                return block.data;
            }
        }
        
        // Need to allocate new blocks
        void* data = std::malloc(totalBlockSize);
        if (!data) return nullptr;
        
        MemoryBlock newBlock(data, totalBlockSize, nextBlockId++);
        newBlock.isFree = false;
        blocks.push_back(newBlock);
        totalMemory += totalBlockSize;
        
        return data;
    }
    
    void free(void* ptr) {
        for (auto& block : blocks) {
            if (block.data == ptr) {
                block.isFree = true;
                return;
            }
        }
    }
    
    size_t getTotalMemory() const {
        return totalMemory;
    }
    
    size_t getUsedMemory() const {
        size_t used = 0;
        for (const auto& block : blocks) {
            if (!block.isFree) {
                used += block.size;
            }
        }
        return used;
    }
    
    float getUtilization() const {
        if (totalMemory == 0) return 0.0f;
        return static_cast<float>(getUsedMemory()) / totalMemory;
    }
    
    std::string getStats() const {
        std::stringstream ss;
        ss << "Block-Based Allocator Statistics:\n";
        ss << "  Block size: " << blockSize << " bytes\n";
        ss << "  Number of blocks: " << blocks.size() << "\n";
        ss << "  Total allocated memory: " << totalMemory << " bytes\n";
        ss << "  Used memory: " << getUsedMemory() << " bytes\n";
        ss << "  Utilization: " << (getUtilization() * 100.0f) << "%\n";
        ss << "  Free blocks: " << countFreeBlocks() << "/" << blocks.size() << "\n";
        
        return ss.str();
    }
    
private:
    size_t countFreeBlocks() const {
        size_t count = 0;
        for (const auto& block : blocks) {
            if (block.isFree) count++;
        }
        return count;
    }
};

// Neural connectivity sparse representation
struct NeuralConnectivity {
    // For large neural networks, use sparse representations
    struct Connection {
        uint64_t preNeuron;
        uint64_t postNeuron;
        float weight;
        uint64_t delay;
        float eligibility;
        
        Connection(uint64_t pre, uint64_t post, float w, uint64_t d = 0, float e = 0.0f)
            : preNeuron(pre), postNeuron(post), weight(w), delay(d), eligibility(e) {}
    };
    
    // Different sparse storage formats
    enum class StorageFormat {
        List,           // Simple list of connections
        CSR,            // Compressed Sparse Row
        CSC,            // Compressed Sparse Column
        Dictionary,     // Hash map from pair to connection
        BlockMap       // Block-based sparse representation
    };
    
    StorageFormat format;
    
    // Storage based on format
    std::vector<Connection> connections;           // List format
    CSRMatrix csrMatrix;                           // CSR format
    CSCMatrix cscMatrix;                           // CSC format
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, Connection>> dict; // Dictionary format
    std::vector<std::vector<Connection>> blockMap; // Block map format
    
    // Metadata
    uint64_t totalNeurons;
    uint64_t maxConnectionsPerNeuron;
    float averageConnectionsPerNeuron;
    
    NeuralConnectivity(StorageFormat fmt = StorageFormat::CSR) 
        : format(fmt), totalNeurons(0), maxConnectionsPerNeuron(0), averageConnectionsPerNeuron(0.0f) {}
    
    void clear() {
        connections.clear();
        csrMatrix.clear();
        cscMatrix.clear();
        dict.clear();
        blockMap.clear();
        totalNeurons = 0;
        maxConnectionsPerNeuron = 0;
        averageConnectionsPerNeuron = 0.0f;
    }
    
    // Add a connection
    void addConnection(uint64_t pre, uint64_t post, float weight, uint64_t delay = 0) {
        if (format == StorageFormat::List) {
            connections.emplace_back(pre, post, weight, delay);
        } else if (format == StorageFormat::CSR) {
            csrMatrix.addConnection(pre, post, weight);
        } else if (format == StorageFormat::CSC) {
            cscMatrix.addConnection(pre, post, weight);
        } else if (format == StorageFormat::Dictionary) {
            dict[pre][post] = Connection(pre, post, weight, delay);
        } else if (format == StorageFormat::BlockMap) {
            // For block map, we'd need to determine block size based on neuron count
            // For simplicity, we'll just use a simple approach
            size_t blockSize = 100; // Example block size
            size_t preBlock = pre / blockSize;
            size_t postBlock = post / blockSize;
            
            while (blockMap.size() <= preBlock) {
                blockMap.push_back(std::vector<Connection>());
            }
            
            if (blockMap[preBlock].size() <= postBlock) {
                blockMap[preBlock].resize(postBlock + 1);
            }
            
            blockMap[preBlock][postBlock] = Connection(pre, post, weight, delay);
        }
        
        totalNeurons = std::max(totalNeurons, std::max(pre, post) + 1);
    }
    
    // Get connection weight
    float getWeight(uint64_t pre, uint64_t post) const {
        switch (format) {
            case StorageFormat::List:
                for (const auto& conn : connections) {
                    if (conn.preNeuron == pre && conn.postNeuron == post) {
                        return conn.weight;
                    }
                }
                return 0.0f;
                
            case StorageFormat::CSR:
                return csrMatrix.getWeight(pre, post);
                
            case StorageFormat::CSC:
                return cscMatrix.getWeight(pre, post);
                
            case StorageFormat::Dictionary:
                auto it1 = dict.find(pre);
                if (it1 != dict.end()) {
                    auto it2 = it1->second.find(post);
                    if (it2 != it1->second.end()) {
                        return it2->second.weight;
                    }
                }
                return 0.0f;
                
            case StorageFormat::BlockMap: {
                size_t blockSize = 100;
                size_t preBlock = pre / blockSize;
                if (preBlock >= blockMap.size()) return 0.0f;
                size_t postBlock = post / blockSize;
                if (postBlock >= blockMap[preBlock].size()) return 0.0f;
                return blockMap[preBlock][postBlock].weight;
            }
            
            default:
                return 0.0f;
        }
    }
    
    // Get all outgoing connections for a neuron
    std::vector<Connection> getOutgoingConnections(uint64_t neuron) const {
        std::vector<Connection> result;
        
        switch (format) {
            case StorageFormat::List:
                for (const auto& conn : connections) {
                    if (conn.preNeuron == neuron) {
                        result.push_back(conn);
                    }
                }
                break;
                
            case StorageFormat::CSR: {
                // CSR format is more efficiently accessed by iterating through rows
                // Since CSR stores by row, we can directly access row 'neuron'
                auto rowData = csrMatrix.getRow(neuron);
                for (const auto& pair : rowData) {
                    result.emplace_back(neuron, pair.first, pair.second);
                }
                break;
            }
                
            case StorageFormat::CSC:
                for (const auto& conn : connections) {
                    if (conn.preNeuron == neuron) {
                        result.push_back(conn);
                    }
                }
                break;
                
            case StorageFormat::Dictionary: {
                auto it = dict.find(neuron);
                if (it != dict.end()) {
                    for (const auto& pair : it->second) {
                        result.push_back(pair.second);
                    }
                }
                break;
            }
                
            case StorageFormat::BlockMap: {
                size_t blockSize = 100;
                size_t preBlock = neuron / blockSize;
                if (preBlock < blockMap.size()) {
                    for (size_t postBlock = 0; postBlock < blockMap[preBlock].size(); ++postBlock) {
                        uint64_t post = postBlock * blockSize;
                        result.push_back(blockMap[preBlock][postBlock]);
                    }
                }
                break;
            }
            
            default:
                break;
        }
        
        return result;
    }
    
    // Compress the connectivity (convert to more efficient format)
    void compress() {
        switch (format) {
            case StorageFormat::List:
                // Convert to CSR if more efficient
                if (totalNeurons > 0 && connections.size() > totalNeurons * 10) {
                    format = StorageFormat::CSR;
                    for (const auto& conn : connections) {
                        csrMatrix.addConnection(conn.preNeuron, conn.postNeuron, conn.weight);
                    }
                    connections.clear();
                    csrMatrix.compress();
                }
                break;
                
            case StorageFormat::Dictionary: {
                // Convert to CSR if density is high
                float density = static_cast<float>(connections.size()) / (totalNeurons * totalNeurons);
                if (density > 0.1f) {  // 10% threshold
                    format = StorageFormat::CSR;
                    for (const auto& conn : connections) {
                        csrMatrix.addConnection(conn.preNeuron, conn.postNeuron, conn.weight);
                    }
                    connections.clear();
                    csrMatrix.compress();
                }
                break;
            }
                
            default:
                break;
        }
    }
    
    // Get memory usage
    size_t getMemoryUsage() const {
        switch (format) {
            case StorageFormat::List:
                return connections.size() * sizeof(Connection);
                
            case StorageFormat::CSR:
                return csrMatrix.getMemoryUsage();
                
            case StorageFormat::CSC:
                return cscMatrix.getMemoryUsage();
                
            case StorageFormat::Dictionary: {
                size_t total = 0;
                for (const auto& outer : dict) {
                    total += outer.second.size() * sizeof(Connection);
                    total += sizeof(uint64_t);  // For the outer key
                }
                return total;
            }
                
            case StorageFormat::BlockMap: {
                size_t total = 0;
                for (const auto& row : blockMap) {
                    total += row.size() * sizeof(Connection);
                }
                return total;
            }
            
            default:
                return 0;
        }
    }
    
    std::string getStats() const {
        std::stringstream ss;
        ss << "Neural Connectivity Statistics:\n";
        ss << "  Storage format: " << storageFormatToString(format) << "\n";
        ss << "  Total neurons: " << totalNeurons << "\n";
        ss << "  Total connections: " << connections.size() << "\n";
        ss << "  Maximum connections per neuron: " << maxConnectionsPerNeuron << "\n";
        ss << "  Average connections per neuron: " << averageConnectionsPerNeuron << "\n";
        ss << "  Memory usage: " << getMemoryUsage() << " bytes\n";
        
        if (totalNeurons > 0 && connections.size() > 0) {
            float density = static_cast<float>(connections.size()) / (totalNeurons * totalNeurons);
            ss << "  Connection density: " << (density * 100.0f) << "%\n";
        }
        
        return ss.str();
    }
    
private:
    std::string storageFormatToString(StorageFormat fmt) {
        switch (fmt) {
            case StorageFormat::List: return "List";
            case StorageFormat::CSR: return "CSR";
            case StorageFormat::CSC: return "CSC";
            case StorageFormat::Dictionary: return "Dictionary";
            case StorageFormat::BlockMap: return "BlockMap";
            default: return "Unknown";
        }
    }
};

// Global memory structure instances
inline CSRMatrix& getGlobalCSR() {
    static CSRMatrix matrix;
    return matrix;
}

inline RunLengthEncoder& getGlobalRLE() {
    static RunLengthEncoder encoder;
    return encoder;
}

inline BlockBasedAllocator& getGlobalBlockAllocator() {
    static BlockBasedAllocator allocator;
    return allocator;
}

// Convenience functions for memory-efficient neural operations
inline void compressConnectivity(NeuralConnectivity& connectivity) {
    connectivity.compress();
}

inline void addNeuralConnection(NeuralConnectivity& connectivity, uint64_t pre, uint64_t post, float weight) {
    connectivity.addConnection(pre, post, weight);
}

inline float getConnectionWeight(const NeuralConnectivity& connectivity, uint64_t pre, uint64_t post) {
    return connectivity.getWeight(pre, post);
}

inline std::string getNeuralConnectivityStats(const NeuralConnectivity& connectivity) {
    return connectivity.getStats();
}

} // namespace MemoryStructures
} // namespace nlm
