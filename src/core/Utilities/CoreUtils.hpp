#pragma once

#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <memory>
#include <unordered_map>
#include <set>
#include <limits>

namespace nlm {

// High-performance spike history manager with advanced optimizations
class SpikeHistoryManager {
public:
    // Optimized spike history with ring buffer for constant-time operations
    class RingBuffer {
    public:
        RingBuffer(size_t capacity = 1000) : capacity_(capacity), size_(0), head_(0) {
            buffer_.resize(capacity);
        }
        
        void push(const Timestamp& timestamp) {
            buffer_[head_] = timestamp;
            head_ = (head_ + 1) % capacity_;
            if (size_ < capacity_) size_++;
        }
        
        const Timestamp& front() const {
            return buffer_[(head_ + capacity_ - size_) % capacity_];
        }
        
        bool empty() const { return size_ == 0; }
        size_t size() const { return size_; }
        size_t capacity() const { return capacity_; }
        void clear() { size_ = 0; head_ = 0; }
        
        // Iterator support for range-based operations
        class iterator {
            const RingBuffer* buffer_ = nullptr;
            size_t index_ = 0;
            size_t size_ = 0;
            size_t head_ = 0;
        public:
            iterator(const RingBuffer* buffer, size_t start, size_t size, size_t head)
                : buffer_(buffer), index_(start), size_(size), head_(head) {}
            
            const Timestamp& operator*() const {
                return buffer_->buffer_[(buffer_->head_ + buffer_->capacity_ - buffer_->size_ + index_) % buffer_->capacity_];
            }
            
            iterator& operator++() {
                index_++;
                return *this;
            }
            
            bool operator!=(const iterator& other) const {
                return index_ != other.index_;
            }
        };
        
        iterator begin() const {
            return iterator(this, 0, size_, head_);
        }
        
        iterator end() const {
            return iterator(this, size_, size_, head_);
        }
        
    private:
        std::vector<Timestamp> buffer_;
        size_t capacity_;
        size_t size_;
        size_t head_;
    };
    
    // Enhanced spike statistics with running calculations
    struct SpikeStatistics {
        size_t total_spikes = 0;
        float firing_rate = 0.0f;
        float average_interval = 0.0f;
        float max_interval = 0.0f;
        float min_interval = 0.0f;
        size_t window_size = 100;
        
        SpikeStatistics() = default;
        explicit SpikeStatistics(size_t window) : window_size(window) {}
        
        void update(const std::vector<Timestamp>& spikes, Timestamp current_time) {
            total_spikes = spikes.size();
            if (total_spikes > 1) {
                float total_interval = 0.0f;
                float max_interval = 0.0f;
                float min_interval = std::numeric_limits<float>::max();
                
                for (size_t i = 1; i < total_spikes; ++i) {
                    float interval = static_cast<float>(spikes[i] - spikes[i-1]);
                    total_interval += interval;
                    if (interval > max_interval) max_interval = interval;
                    if (interval < min_interval) min_interval = interval;
                }
                
                average_interval = total_interval / (total_spikes - 1);
                this->max_interval = max_interval;
                this->min_interval = (min_interval == std::numeric_limits<float>::max()) ? 0.0f : min_interval;
                
                float duration = (total_spikes > 1) ? static_cast<float>(current_time - spikes[0]) : 0.0f;
                firing_rate = (duration > 0.0f) ? (total_spikes / duration) : 0.0f;
            }
        }
        
        void reset() {
            total_spikes = 0;
            firing_rate = 0.0f;
            average_interval = 0.0f;
            max_interval = 0.0f;
            min_interval = 0.0f;
        }
    };
    
    // Factory method for creating optimized spike history
    static std::unique_ptr<SpikeHistoryManager> createOptimizedManager(
        size_t max_history_size = 100,
        bool use_ring_buffer = true) {
        auto manager = std::make_unique<SpikeHistoryManager>();
        manager->max_history_size_ = max_history_size;
        manager->use_ring_buffer_ = use_ring_buffer;
        manager->statistics_.window_size = max_history_size;
        return manager;
    }
    
    // Record spike with automatic history management
    void recordSpike(Timestamp timestamp) {
        if (use_ring_buffer_) {
            // Use ring buffer for O(1) operations
            if (ring_buffer_.size() >= max_history_size_) {
                // Buffer full, shift (ring buffer handles this automatically)
            }
            ring_buffer_.push(timestamp);
        } else {
            // Use vector with bounds checking
            SpikeHistoryManager::recordSpike(history_, timestamp, max_history_size_);
        }
        
        // Update spike history for internal calculations
        history_.push_back(timestamp);
        if (history_.size() > max_history_size_ * 2) {
            history_.erase(history_.begin(), history_.begin() + history_.size() - max_history_size_);
        }
        
        // Update statistics
        statistics_.update(history_, timestamp);
    }
    
    // Get spike history (O(1) with ring buffer, O(n) with vector)
    const std::vector<Timestamp>& getHistory() const {
        if (use_ring_buffer_) {
            // Convert ring buffer to vector (O(n))
            history_.clear();
            for (auto it = ring_buffer_.begin(); it != ring_buffer_.end(); ++it) {
                history_.push_back(*it);
            }
        }
        return history_;
    }
    
    // Get spike count
    size_t getSpikeCount() const {
        return use_ring_buffer_ ? ring_buffer_.size() : history_.size();
    }
    
    // Get statistics
    const SpikeStatistics& getStatistics() const { return statistics_; }
    
    // Clear history
    void clear() {
        history_.clear();
        ring_buffer_.clear();
        statistics_.reset();
    }
    
    // Check if history contains valid spikes
    bool hasValidSpikes() const {
        return !history_.empty() && 
               std::any_of(history_.begin(), history_.end(),
                          [](Timestamp t) { return t >= 0; });
    }
    
private:
    std::vector<Timestamp> history_;
    RingBuffer ring_buffer_;
    SpikeStatistics statistics_;
    size_t max_history_size_ = 100;
    bool use_ring_buffer_ = true;
    
    // Fallback implementation for vector-based history
    static void recordSpike(std::vector<Timestamp>& history, Timestamp timestamp, size_t max_history_size) {
        if (history.size() >= max_history_size) {
            history.erase(history.begin());
        }
        history.push_back(timestamp);
    }
};

// Memory-efficient spike time calculator
class SpikeTimeCalculator {
public:
    // Calculate inter-spike intervals efficiently
    static std::vector<float> calculateIntervals(const std::vector<Timestamp>& spikes) {
        std::vector<float> intervals;
        intervals.reserve(spikes.size() - 1);
        
        for (size_t i = 1; i < spikes.size(); ++i) {
            float interval = static_cast<float>(spikes[i] - spikes[i-1]);
            intervals.push_back(interval);
        }
        
        return intervals;
    }
    
    // Calculate average interval with overflow protection
    static float calculateAverageInterval(const std::vector<Timestamp>& spikes) {
        if (spikes.size() < 2) return 0.0f;
        
        float total = 0.0f;
        for (size_t i = 1; i < spikes.size(); ++i) {
            total += static_cast<float>(spikes[i] - spikes[i-1]);
        }
        
        return total / (spikes.size() - 1);
    }
    
    // Calculate firing rate with precision
    static float calculateFiringRate(size_t spike_count, float duration) {
        return (duration > 0.0f) ? (static_cast<float>(spike_count) / duration) : 0.0f;
    }
    
    // Find maximum inter-spike interval
    static float findMaxInterval(const std::vector<Timestamp>& spikes) {
        if (spikes.size() < 2) return 0.0f;
        
        float max_interval = 0.0f;
        for (size_t i = 1; i < spikes.size(); ++i) {
            float interval = static_cast<float>(spikes[i] - spikes[i-1]);
            if (interval > max_interval) max_interval = interval;
        }
        
        return max_interval;
    }
    
    // Find minimum inter-spike interval
    static float findMinInterval(const std::vector<Timestamp>& spikes) {
        if (spikes.size() < 2) return 0.0f;
        
        float min_interval = std::numeric_limits<float>::max();
        for (size_t i = 1; i < spikes.size(); ++i) {
            float interval = static_cast<float>(spikes[i] - spikes[i-1]);
            if (interval < min_interval) min_interval = interval;
        }
        
        return (min_interval == std::numeric_limits<float>::max()) ? 0.0f : min_interval;
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpu_usage = 0.0;
        double memory_usage = 0.0;
        double simulation_time = 0.0;
        size_t spike_events = 0;
        size_t neuron_count = 0;
        size_t synapse_count = 0;
        size_t active_spikes = 0;
    };
    
    // Simple performance logging
    class PerformanceLogger {
    public:
        void logMetrics(const std::string& name, const PerformanceMetrics& metrics) {
            metrics_history_[name].push_back(metrics);
            
            // Keep only recent metrics
            if (metrics_history_[name].size() > 1000) {
                metrics_history_[name].erase(metrics_history_[name].begin());
            }
            
            NLM_LOG_INFO(std::string("Performance metrics for ") + name + ": " +
                         "CPU: " + std::to_string(metrics.cpu_usage) + "%, " +
                         "Memory: " + std::to_string(metrics.memory_usage) + "MB, " +
                         "Time: " + std::to_string(metrics.simulation_time) + "s, " +
                         "Spikes: " + std::to_string(metrics.spike_events));
        }
        
        // Get average metrics for a time window
        PerformanceMetrics getAverageMetrics(const std::string& name, size_t window_size = 100) const {
            PerformanceMetrics avg;
            size_t count = 0;
            
            auto& history = metrics_history_.at(name);
            size_t start_idx = std::max<size_t>(0, history.size() - window_size);
            
            for (size_t i = start_idx; i < history.size(); ++i) {
                const auto& metrics = history[i];
                avg.cpu_usage += metrics.cpu_usage;
                avg.memory_usage += metrics.memory_usage;
                avg.simulation_time += metrics.simulation_time;
                avg.spike_events += metrics.spike_events;
                avg.neuron_count += metrics.neuron_count;
                avg.synapse_count += metrics.synapse_count;
                avg.active_spikes += metrics.active_spikes;
                count++;
            }
            
            if (count > 0) {
                avg.cpu_usage /= count;
                avg.memory_usage /= count;
                avg.simulation_time /= count;
                avg.spike_events /= count;
                avg.neuron_count /= count;
                avg.synapse_count /= count;
                avg.active_spikes /= count;
            }
            
            return avg;
        }
        
    private:
        std::unordered_map<std::string, std::vector<PerformanceMetrics>> metrics_history_;
    };
    
    static PerformanceLogger& getLogger() {
        static PerformanceLogger logger;
        return logger;
    }
};

// Advanced neural simulation utilities
class NeuralSimulationUtils {
public:
    // Efficient neuron spike detection using vectorized operations
    template<typename Iterator>
    static std::vector<size_t> detectSpikes(Iterator begin, Iterator end, float threshold) {
        std::vector<size_t> spike_indices;
        spike_indices.reserve(std::distance(begin, end) / 10); // Estimate
        
        size_t index = 0;
        for (auto it = begin; it != end; ++it, ++index) {
            if (*it >= threshold) {
                spike_indices.push_back(index);
            }
        }
        
        return spike_indices;
    }
    
    // Optimized synaptic weight normalization
    static void normalizeWeights(std::vector<float>& weights, float target_mean = 0.0f, float target_std = 1.0f) {
        if (weights.empty()) return;
        
        // Calculate current statistics
        float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
        float mean = sum / weights.size();
        
        float sq_sum = 0.0f;
        for (float w : weights) {
            sq_sum += (w - mean) * (w - mean);
        }
        float stddev = std::sqrt(sq_sum / weights.size());
        
        // Normalize
        float scale = (stddev > 0.0f) ? (target_std / stddev) : 1.0f;
        float shift = target_mean - mean * scale;
        
        for (float& w : weights) {
            w = w * scale + shift;
        }
    }
    
    // Fast synaptic pruning based on weight threshold
    static size_t pruneSmallWeights(std::vector<float>& weights, float threshold) {
        size_t pruned_count = 0;
        for (auto& w : weights) {
            if (std::abs(w) < threshold) {
                w = 0.0f;
                pruned_count++;
            }
        }
        return pruned_count;
    }
    
    // Memory-efficient spike compression
    struct SpikeCompression {
        std::vector<Timestamp> compressed_spikes;
        std::vector<size_t> spike_counts;
        float compression_ratio = 1.0f;
        
        void compress(const std::vector<Timestamp>& original_spikes, size_t max_compressed_size = 1000) {
            compressed_spikes.clear();
            spike_counts.clear();
            
            if (original_spikes.empty()) return;
            
            size_t original_size = original_spikes.size();
            if (original_size <= max_compressed_size) {
                compressed_spikes = original_spikes;
                spike_counts = std::vector<size_t>(1, original_size);
                return;
            }
            
            // Simple downsampling
            size_t step = original_size / max_compressed_size;
            size_t count = 0;
            
            for (size_t i = 0; i < original_size; i += step) {
                compressed_spikes.push_back(original_spikes[i]);
                spike_counts.push_back(count++);
            }
            
            compression_ratio = static_cast<float>(original_size) / compressed_spikes.size();
        }
        
        // Decompress spikes (approximate)
        std::vector<Timestamp> decompress() const {
            std::vector<Timestamp> result;
            if (compressed_spikes.empty()) return result;
            
            for (size_t i = 0; i < compressed_spikes.size(); ++i) {
                size_t count = (i < spike_counts.size()) ? spike_counts[i] : 1;
                for (size_t j = 0; j < count; ++j) {
                    result.push_back(compressed_spikes[i] + j);
                }
            }
            
            return result;
        }
    };
    
    // Parallel spike processing helper
    template<typename Function>
    static void parallelForEachSpike(const std::vector<Timestamp>& spikes, Function func) {
        // Single-threaded implementation - could be parallelized with OpenMP
        for (size_t i = 0; i < spikes.size(); ++i) {
            func(spikes[i], i);
        }
    }
    
    // Spike rate limiting to prevent overflow
    class SpikeRateLimiter {
    public:
        SpikeRateLimiter(float max_rate = 1000.0f, size_t max_history = 1000)
            : max_rate_(max_rate), max_history_(max_history) {}
        
        bool shouldProcess(Timestamp timestamp) {
            history_.push_back(timestamp);
            
            // Maintain history size
            if (history_.size() > max_history_) {
                history_.erase(history_.begin(), history_.begin() + (history_.size() - max_history_));
            }
            
            if (history_.size() < 2) return true;
            
            // Calculate rate
            float duration = static_cast<float>(history_.back() - history_.front());
            float rate = static_cast<float>(history_.size() - 1) / duration;
            
            // Rate limiting
            return rate <= max_rate_;
        }
        
        void clear() { history_.clear(); }
        
    private:
        std::vector<Timestamp> history_;
        float max_rate_;
        size_t max_history_;
    };
    
    // Utility for spike synchronization
    static std::vector<Timestamp> synchronizeSpikes(const std::vector<Timestamp>& source_spikes,
                                                     const std::vector<Timestamp>& target_spikes,
                                                     float tolerance = 0.001f) {
        std::vector<Timestamp> synchronized;
        synchronized.reserve(source_spikes.size());
        
        for (Timestamp spike : source_spikes) {
            bool found = false;
            for (Timestamp target : target_spikes) {
                if (std::abs(spike - target) <= tolerance) {
                    found = true;
                    break;
                }
            }
            if (found) {
                synchronized.push_back(spike);
            }
        }
        
        return synchronized;
    }
};

} // namespace nlm
