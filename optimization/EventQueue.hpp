// Event Queue System for Optimized Neural Simulation
// File: optimization/EventQueue.hpp

#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <limits>

namespace nlm {

// Event types
enum class EventType {
    SPIKE,
    DELAYED_SPIKE,
    SYNAPTIC_EVENT,
    PLASTICITY_EVENT,
    MEMORY_EVENT,
    TIMING_EVENT,
    CUSTOM
};

// Event priority
enum class EventPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Event with priority
struct Event {
    EventType type;
    EventPriority priority;
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::steady_clock::time_point scheduled_time;
    std::thread::id thread_id;
    std::function<void()> callback;
    std::unordered_map<std::string, std::string> metadata;
    bool processed;
    bool cancelled;
    size_t event_id;
    
    Event(EventType t, EventPriority p, const std::chrono::steady_clock::time_point& time,
          const std::function<void()>& cb, const std::unordered_map<std::string, std::string>& meta = {})
        : type(t), priority(p), timestamp(std::chrono::steady_clock::now()),
          scheduled_time(time), thread_id(std::this_thread::get_id()),
          callback(cb), metadata(meta), processed(false), cancelled(false),
          event_id(next_id_++) {}
    
    static size_t get_next_id() { return next_id_; }
    static void reset_id_counter() { next_id_ = 1; }
    
private:
    static std::atomic<size_t> next_id_;
};

// Ring buffer for high-performance event storage
template<typename T>
class RingBuffer {
public:
    RingBuffer(size_t capacity)
        : capacity_(capacity), size_(0), head_(0), tail_(0), data_(new T[capacity]) {
    }
    
    ~RingBuffer() {
        delete[] data_;
    }
    
    bool push(const T& item) {
        if (size_ >= capacity_) {
            return false;  // Buffer full
        }
        
        size_t index = tail_++;
        if (tail_ >= capacity_) {
            tail_ = 0;
        }
        
        data_[index] = item;
        size_++;
        return true;
    }
    
    bool pop(T& item) {
        if (size_ <= 0) {
            return false;  // Buffer empty
        }
        
        size_t index = head_++;
        if (head_ >= capacity_) {
            head_ = 0;
        }
        
        item = data_[index];
        size_--;
        return true;
    }
    
    bool peek(T& item) const {
        if (size_ <= 0) {
            return false;  // Buffer empty
        }
        
        item = data_[head_];
        return true;
    }
    
    size_t size() const { return size_; }
    bool is_full() const { return size_ >= capacity_; }
    bool is_empty() const { return size_ <= 0; }
    
    void clear() {
        size_ = 0;
        head_ = 0;
        tail_ = 0;
    }
    
    T* data() { return data_; }
    const T* data() const { return data_; }

private:
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    T* data_;
};

// Spike event queue for neural simulation
class SpikeEventQueue {
public:
    SpikeEventQueue(size_t capacity = 1000000)
        : ring_buffer_(capacity), active_spikes_(0), total_spikes_processed_(0),
          max_active_spikes_(0), thread_safe_(true), stats_enabled_(true) {
    }
    
    bool enqueue_spike(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time,
                       EventPriority priority = EventPriority::HIGH) {
        if (stats_enabled_) {
            active_spikes_++;
            max_active_spikes_ = std::max(max_active_spikes_, active_spikes_);
        }
        
        Event spike_event(EventType::SPIKE, priority, scheduled_time,
                         [this, neuron_id]() {
                             process_spike(neuron_id);
                         });
        
        spike_event.metadata["neuron_id"] = std::to_string(neuron_id);
        spike_event.metadata["type"] = "spike";
        
        bool result = ring_buffer_.push(spike_event);
        
        if (!result && stats_enabled_) {
            active_spikes_--;
        }
        
        return result;
    }
    
    bool enqueue_delayed_spike(size_t neuron_id, size_t delay_steps,
                               std::chrono::duration<double> timestep,
                               EventPriority priority = EventPriority::NORMAL) {
        auto scheduled_time = std::chrono::steady_clock::now() + 
                             timestep * delay_steps;
        
        Event delayed_event(EventType::DELAYED_SPIKE, priority, scheduled_time,
                           [this, neuron_id, delay_steps]() {
                               process_delayed_spike(neuron_id, delay_steps);
                           });
        
        delayed_event.metadata["neuron_id"] = std::to_string(neuron_id);
        delayed_event.metadata["delay_steps"] = std::to_string(delay_steps);
        delayed_event.metadata["type"] = "delayed_spike";
        
        return ring_buffer_.push(delayed_event);
    }
    
    bool dequeue_event(Event& event) {
        return ring_buffer_.pop(event);
    }
    
    bool peek_event(Event& event) const {
        return ring_buffer_.peek(event);
    }
    
    size_t size() const { return ring_buffer_.size(); }
    bool is_empty() const { return ring_buffer_.is_empty(); }
    bool is_full() const { return ring_buffer_.is_full(); }
    
    void clear() {
        ring_buffer_.clear();
        active_spikes_ = 0;
    }
    
    // Statistics
    size_t get_active_spikes() const { return active_spikes_; }
    size_t get_total_spikes_processed() const { return total_spikes_processed_; }
    size_t get_max_active_spikes() const { return max_active_spikes_; }
    
    // Process events
    void process_events(std::function<void(const Event&)> processor) {
        Event event;
        while (ring_buffer_.pop(event)) {
            if (!event.cancelled) {
                event.processed = true;
                processor(event);
                
                if (stats_enabled_) {
                    total_spikes_processed_++;
                    active_spikes_--;
                }
            }
        }
    }
    
    void process_next_event(std::function<void(const Event&)> processor) {
        Event event;
        if (ring_buffer_.pop(event)) {
            if (!event.cancelled) {
                event.processed = true;
                processor(event);
                
                if (stats_enabled_) {
                    total_spikes_processed_++;
                    active_spikes_--;
                }
            } else if (stats_enabled_) {
                active_spikes_--;
            }
        }
    }
    
    void cancel_event(size_t event_id) {
        // Implementation would need to scan buffer
        // For performance, this is simplified
    }
    
    // Configuration
    void set_thread_safe(bool enabled) { thread_safe_ = enabled; }
    void set_statistics_enabled(bool enabled) { stats_enabled_ = enabled; }

private:
    void process_spike(size_t neuron_id) {
        // Process spike event
        // Implementation depends on neural simulation specifics
    }
    
    void process_delayed_spike(size_t neuron_id, size_t delay_steps) {
        // Process delayed spike event
        // Implementation depends on neural simulation specifics
    }
    
    RingBuffer<Event> ring_buffer_;
    std::atomic<size_t> active_spikes_;
    std::atomic<size_t> total_spikes_processed_;
    std::atomic<size_t> max_active_spikes_;
    bool thread_safe_;
    bool stats_enabled_;
};

// Time-bucketed spike queue for efficient scheduling
class TimeBucketedSpikeQueue {
public:
    TimeBucketedSpikeQueue(size_t num_buckets = 100, size_t max_spikes_per_bucket = 10000)
        : num_buckets_(num_buckets), max_spikes_per_bucket_(max_spikes_per_bucket),
          current_bucket_(0), thread_safe_(true) {
        
        buckets_.resize(num_buckets);
        for (auto& bucket : buckets_) {
            bucket.reserve(max_spikes_per_bucket);
        }
        
        // Initialize bucket distribution
        bucket_distribution_.resize(num_buckets, 0.0);
    }
    
    void add_spike(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time) {
        size_t bucket_index = calculate_bucket_index(scheduled_time);
        
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        
        if (buckets_[bucket_index].size() >= max_spikes_per_bucket_) {
            // Bucket full, drop spike
            return;
        }
        
        buckets_[bucket_index].push_back({neuron_id, scheduled_time});
        bucket_distribution_[bucket_index]++;
    }
    
    bool process_bucket(size_t bucket_index) {
        if (bucket_index >= num_buckets_) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        
        if (buckets_[bucket_index].empty()) {
            return false;
        }
        
        // Process all spikes in this bucket
        for (const auto& spike : buckets_[bucket_index]) {
            // Process spike
            process_spike(spike.neuron_id, spike.scheduled_time);
        }
        
        // Clear bucket
        buckets_[bucket_index].clear();
        bucket_distribution_[bucket_index] = 0.0;
        
        return true;
    }
    
    void advance_buckets(std::chrono::steady_clock::time_point current_time) {
        size_t current_time_bucket = calculate_bucket_index(current_time);
        
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        
        if (current_time_bucket <= current_bucket_) {
            return;  // Not enough time passed
        }
        
        // Process intermediate buckets
        for (size_t bucket = current_bucket_ + 1; bucket <= current_time_bucket; ++bucket) {
            if (bucket < num_buckets_) {
                process_bucket(bucket);
            }
        }
        
        current_bucket_ = current_time_bucket;
    }
    
    size_t get_current_bucket() const { return current_bucket_; }
    size_t get_bucket_size(size_t bucket_index) const {
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        if (bucket_index >= num_buckets_) return 0;
        return buckets_[bucket_index].size();
    }
    
    double get_bucket_distribution(size_t bucket_index) const {
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        if (bucket_index >= num_buckets_) return 0.0;
        return bucket_distribution_[bucket_index];
    }
    
    size_t get_num_buckets() const { return num_buckets_; }
    
    // Configuration
    void set_thread_safe(bool enabled) { thread_safe_ = enabled; }
    void set_num_buckets(size_t num_buckets) {
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        num_buckets_ = num_buckets;
        buckets_.resize(num_buckets_);
        bucket_distribution_.resize(num_buckets_);
        for (auto& bucket : buckets_) {
            bucket.reserve(max_spikes_per_bucket_);
        }
    }

private:
    struct SpikeEntry {
        size_t neuron_id;
        std::chrono::steady_clock::time_point scheduled_time;
        
        SpikeEntry(size_t id, const std::chrono::steady_clock::time_point& time)
            : neuron_id(id), scheduled_time(time) {}
    };
    
    size_t calculate_bucket_index(std::chrono::steady_clock::time_point time) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            time.time_since_epoch()
        );
        
        // Normalize to bucket index
        uint64_t total_microseconds = static_cast<uint64_t>(duration.count());
        return (total_microseconds / bucket_duration_) % num_buckets_;
    }
    
    void process_spike(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time) {
        // Process individual spike
        // Implementation depends on neural simulation specifics
    }
    
    std::vector<std::vector<SpikeEntry>> buckets_;
    std::vector<double> bucket_distribution_;
    size_t num_buckets_;
    size_t max_spikes_per_bucket_;
    size_t current_bucket_;
    bool thread_safe_;
    
    static constexpr uint64_t bucket_duration_ = 1000000LL;  // 1 second buckets
    mutable std::mutex bucket_mutex_;
};

// Event system statistics
struct EventSystemStats {
    size_t total_events_enqueued;
    size_t total_events_processed;
    size_t active_events;
    size_t max_active_events;
    double processing_rate;  // events per second
    double average_delay;   // average event delay
    size_t ring_buffer_size;
    size_t ring_buffer_capacity;
    double ring_buffer_utilization;
    size_t time_bucket_size;
    size_t current_time_bucket;
    std::vector<double> bucket_utilization;
    std::chrono::steady_clock::time_point last_update;
    std::chrono::steady_clock::time_point first_event_time;
    std::chrono::steady_clock::time_point last_event_time;
};

// Event system configuration
struct EventSystemConfig {
    size_t ring_buffer_capacity;
    size_t time_bucket_count;
    size_t max_spikes_per_bucket;
    bool enable_statistics;
    bool enable_thread_safety;
    double bucket_duration;
    bool enable_priority_scheduling;
    size_t max_event_latency_microseconds;
    std::string output_format;
    std::string output_file;
};

// Event system for neural simulation
class EventSystem {
public:
    EventSystem(const EventSystemConfig& config)
        : config_(config), stats_(), running_(false), thread_count_(0),
          stats_enabled_(config.enable_statistics) {
        
        // Initialize spike event queues
        spike_queue_ = std::make_unique<SpikeEventQueue>(
            config.ring_buffer_capacity / 2
        );
        
        // Initialize time-bucketed queue
        time_bucketed_queue_ = std::make_unique<TimeBucketedSpikeQueue>(
            config.time_bucket_count,
            config.max_spikes_per_bucket
        );
        
        // Initialize statistics
        stats_.total_events_enqueued = 0;
        stats_.total_events_processed = 0;
        stats_.active_events = 0;
        stats_.max_active_events = 0;
        stats_.ring_buffer_size = 0;
        stats_.ring_buffer_capacity = config.ring_buffer_capacity / 2;
        stats_.time_bucket_size = config.time_bucket_count;
        stats_.current_time_bucket = 0;
        stats_.bucket_utilization.resize(config.time_bucket_count, 0.0);
        stats_.last_update = std::chrono::steady_clock::now();
        stats_.first_event_time = std::chrono::steady_clock::now();
        stats_.last_event_time = stats_.first_event_time;
    }
    
    ~EventSystem() {
        stop();
    }
    
    // Event management
    bool enqueue_spike(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time,
                       EventPriority priority = EventPriority::HIGH) {
        bool result = spike_queue_->enqueue_spike(neuron_id, scheduled_time, priority);
        if (result) {
            stats_.total_events_enqueued++;
            stats_.active_events++;
            stats_.last_event_time = std::chrono::steady_clock::now();
            
            if (stats_enabled_) {
                stats_.max_active_events = std::max(stats_.max_active_events, stats_.active_events);
            }
        }
        
        return result;
    }
    
    bool enqueue_delayed_spike(size_t neuron_id, size_t delay_steps,
                               std::chrono::duration<double> timestep,
                               EventPriority priority = EventPriority::NORMAL) {
        bool result = spike_queue_->enqueue_delayed_spike(
            neuron_id, delay_steps, timestep, priority
        );
        if (result) {
            stats_.total_events_enqueued++;
            stats_.active_events++;
            stats_.last_event_time = std::chrono::steady_clock::now();
            
            if (stats_enabled_) {
                stats_.max_active_events = std::max(stats_.max_active_events, stats_.active_events);
            }
        }
        
        return result;
    }
    
    bool process_events(std::function<void(const Event&)> processor) {
        if (!running_.load()) {
            return false;
        }
        
        spike_queue_->process_events(processor);
        
        return true;
    }
    
    bool process_next_event(std::function<void(const Event&)> processor) {
        if (!running_.load()) {
            return false;
        }
        
        return spike_queue_->process_next_event(processor);
    }
    
    // Time-bucketed queue operations
    void advance_time_buckets(std::chrono::steady_clock::time_point current_time) {
        if (!running_.load()) {
            return;
        }
        
        time_bucketed_queue_->advance_buckets(current_time);
        update_stats();
    }
    
    // Configuration
    void start() {
        if (running_.load()) {
            return;
        }
        
        running_.store(true);
        thread_count_ = std::thread::hardware_concurrency();
        
        // Start background processing thread
        processing_thread_ = std::thread(&EventSystem::processing_loop, this);
    }
    
    void stop() {
        if (!running_.load()) {
            return;
        }
        
        running_.store(false);
        
        if (processing_thread_.joinable()) {
            processing_thread_.join();
        }
    }
    
    bool is_running() const { return running_.load(); }
    
    // Statistics
    EventSystemStats get_stats() const {
        EventSystemStats stats = stats_;
        stats_.ring_buffer_size = spike_queue_->size();
        stats_.current_time_bucket = time_bucketed_queue_->get_current_bucket();
        
        for (size_t i = 0; i < config_.time_bucket_count; ++i) {
            stats_.bucket_utilization[i] = time_bucketed_queue_->get_bucket_distribution(i);
        }
        
        if (stats_.first_event_time < stats_.last_event_time) {
            double duration = std::chrono::duration<double>(
                stats_.last_event_time - stats_.first_event_time
            ).count();
            
            if (duration > 0) {
                stats_.processing_rate = stats_.total_events_processed / duration;
                stats_.average_delay = (stats_.last_event_time - stats_.first_event_time).count() / 
                                     static_cast<double>(stats_.total_events_enqueued);
            }
        }
        
        stats_.ring_buffer_utilization = 
            static_cast<double>(stats_.ring_buffer_size) / stats_.ring_buffer_capacity;
        
        stats_.last_update = std::chrono::steady_clock::now();
        
        return stats;
    }
    
    // Export
    void export_stats(const std::string& filename) const {
        // Export statistics to file
        // Implementation depends on output format
    }

protected:
    void processing_loop() {
        while (running_.load()) {
            try {
                // Process events in batches
                process_events_batch();
                
                // Update statistics
                if (stats_enabled_) {
                    update_stats();
                }
                
                // Sleep briefly to avoid busy waiting
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            } catch (const std::exception& e) {
                std::cerr << "Event system processing loop error: " << e.what() << std::endl;
            }
        }
    }
    
    void process_events_batch() {
        // Process a batch of events
        // Implementation depends on processing strategy
    }
    
    void update_stats() {
        // Update statistics
        // Implementation depends on statistics collection
    }
    
    void add_spike_to_time_bucket(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time) {
        time_bucketed_queue_->add_spike(neuron_id, scheduled_time);
    }
    
    void process_spike(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time) {
        // Process individual spike
        // Implementation depends on neural simulation specifics
    }
    
    EventSystemConfig config_;
    EventSystemStats stats_;
    std::atomic<bool> running_;
    size_t thread_count_;
    bool stats_enabled_;
    
    std::unique_ptr<SpikeEventQueue> spike_queue_;
    std::unique_ptr<TimeBucketedSpikeQueue> time_bucketed_queue_;
    std::thread processing_thread_;
};

// Global event system instance for convenience
inline std::unique_ptr<EventSystem> global_event_system_;

// Convenience functions
inline void initialize_event_system(const EventSystemConfig& config) {
    global_event_system_ = std::make_unique<EventSystem>(config);
    global_event_system_->start();
}

inline void shutdown_event_system() {
    if (global_event_system_) {
        global_event_system_->stop();
        global_event_system_.reset();
    }
}

inline bool enqueue_spike(size_t neuron_id, std::chrono::steady_clock::time_point scheduled_time,
                          EventPriority priority = EventPriority::HIGH) {
    if (!global_event_system_) {
        return false;
    }
    return global_event_system_->enqueue_spike(neuron_id, scheduled_time, priority);
}

inline EventSystemStats get_event_system_stats() {
    if (!global_event_system_) {
        return EventSystemStats{};
    }
    return global_event_system_->get_stats();
}

print(f"Event System module loaded. {__name__}");
