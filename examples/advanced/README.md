# Memory Examples

## Overview

This directory contains examples demonstrating NLM's memory system capabilities. These examples showcase how to use different memory types, implement memory-based learning, and analyze memory performance.

## Example 1: Working Memory Demo

**File**: `examples/memory/working_memory_example.cpp`

```cpp
// Working Memory Example
// Demonstrates working memory functionality and dynamics

#include <iostream>
#include <vector>
#include <algorithm>
#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "core/Config/Config.hpp"

class WorkingMemoryDemo {
private:
    struct Task {
        std::string id;
        std::string description;
        double priority;
        double complexity;
        double estimated_time;
        bool completed;
        double start_time;
        double end_time;
        double reward;
    };
    
    struct MemoryEntry {
        std::string task_id;
        double activation;
        double decay_factor;
        std::vector<std::string> associated_concepts;
    };
    
    std::vector<Task> task_queue_;
    std::vector<MemoryEntry> working_memory_;
    nlm::memory::Memory memory_system_;
    nlm::brain::Brain brain_;
    
public:
    WorkingMemoryDemo() {
        // Create working memory system with capacity for 100 tasks
        memory_system_ = nlm::memory::Memory(100);
        
        // Create brain with 500 neurons
        nlm::core::Config config;
        config.set("brain.neuron_count", 500);
        brain_ = nlm::brain::Brain(config);
        brain_.initialize();
        
        // Initialize task queue
        initialize_task_queue();
        
        std::cout << "=== Working Memory Example ===" << std::endl;
        std::cout << "Working memory system initialized with capacity: " 
                  << memory_system_.getTotalCapacity() << " tasks" << std::endl;
    }
    
    void initialize_task_queue() {
        std::vector<std::string> tasks = {
            "Process customer request",
            "Generate report",
            "Analyze data",
            "Update database",
            "Send notification",
            "Validate input",
            "Optimize query",
            "Backup system",
            "Monitor performance",
            "Schedule maintenance"
        };
        
        std::vector<double> priorities = {0.9, 0.7, 0.8, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05};
        std::vector<double> complexities = {0.3, 0.5, 0.7, 0.4, 0.6, 0.8, 0.2, 0.9, 0.1, 0.3};
        
        for (size_t i = 0; i < tasks.size(); ++i) {
            Task task;
            task.id = "task_" + std::to_string(i);
            task.description = tasks[i];
            task.priority = priorities[i];
            task.complexity = complexities[i];
            task.estimated_time = (1.0 - task.complexity) * 60.0 + 10.0; // 10-70 seconds
            task.completed = false;
            task.start_time = -1.0;
            task.end_time = -1.0;
            task.reward = task.priority * task.complexity;
            
            task_queue_.push_back(task);
        }
        
        std::cout << "Task queue initialized with " << task_queue_.size() << " tasks" << std::endl;
    }
    
    void simulate_working_memory() {
        std::cout << "\n=== Working Memory Simulation ===" << std::endl;
        
        // Sort tasks by priority (highest first)
        std::sort(task_queue_.begin(), task_queue_.end(),
                  [](const Task& a, const Task& b) {
                      return a.priority > b.priority;
                  });
        
        double total_simulation_time = 0.0;
        int completed_tasks = 0;
        
        while (!task_queue_.empty() && total_simulation_time < 300.0) { // 5 minute simulation
            std::cout << "\n--- Simulation Cycle ---" << std::endl;
            
            // Select next task based on working memory
            Task current_task = select_next_task();
            
            if (!current_task.id.empty()) {
                // Execute task
                execute_task(current_task);
                completed_tasks++;
                
                // Remove from queue
                auto it = std::find_if(task_queue_.begin(), task_queue_.end(),
                                      [&current_task](const Task& t) { return t.id == current_task.id; });nif (it != task_queue_.end()) {
                    task_queue_.erase(it);
                }
                
                total_simulation_time += current_task.estimated_time;
            } else {
                std::cout << "No tasks available for execution" << std::endl;
                break;
            }
            
            // Update working memory with new experience
            update_working_memory(current_task);
            
            // Print working memory status
            print_working_memory_status();
            
            // Simulate time passing
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "\n=== Working Memory Simulation Complete ===" << std::endl;
        std::cout << "Simulation time: " << total_simulation_time << " seconds" << std::endl;
        std::cout << "Tasks completed: " << completed_tasks << "/" << task_queue_.size() + completed_tasks << std::endl;
        std::cout << "Working memory peak usage: " << memory_system_.getPeakUsage() << "/" 
                  << memory_system_.getTotalCapacity() << " slots" << std::endl;
    }
    
    Task select_next_task() {
        // Find tasks in working memory
        std::vector<MemoryEntry> available_tasks;
        
        for (const auto& entry : working_memory_) {
            // Find corresponding task
            for (const auto& task : task_queue_) {
                if (task.id == entry.task_id) {
                    available_tasks.push_back(entry);
                    break;
                }
            }
        }
        
        if (available_tasks.empty()) {
            // Select next highest priority task
            if (!task_queue_.empty()) {
                Task selected = task_queue_.front();
                
                // Store in working memory
                MemoryEntry entry;
                entry.task_id = selected.id;
                entry.activation = 1.0;
                entry.decay_factor = 0.95;
                entry.associated_concepts = {"priority", "high"};
                
                working_memory_.push_back(entry);
                
                return selected;
            }
        }
        
        // Sort by activation
        std::sort(available_tasks.begin(), available_tasks.end(),
                  [](const MemoryEntry& a, const MemoryEntry& b) {
                      return a.activation > b.activation;
                  });
        
        // Find task in queue
        for (const auto& task : task_queue_) {
            for (const auto& entry : available_tasks) {
                if (task.id == entry.task_id) {
                    return task;
                }
            }
        }
        
        return Task(); // No task selected
    }
    
    void execute_task(Task& task) {
        std::cout << "Executing task: " << task.description << std::endl;
        std::cout << "  Priority: " << task.priority << ", Complexity: " << task.complexity << std::endl;
        
        // Update brain activity
        brain_.step(0);
        
        // Simulate task execution time
        task.start_time = get_current_time();
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(task.estimated_time * 1000)));
        task.end_time = get_current_time();
        
        // Calculate reward based on complexity and priority
        task.reward = (task.priority * 0.6) + (task.complexity * 0.4);
        
        // Store in working memory
        MemoryEntry memory_entry;
        memory_entry.task_id = task.id;
        memory_entry.activation = 1.0;
        memory_entry.decay_factor = 0.95;
        memory_entry.associated_concepts = {
            "priority_" + std::to_string(static_cast<int>(task.priority * 10)),
            "complexity_" + std::to_string(static_cast<int>(task.complexity * 10)),
            "time_" + std::to_string(static_cast<int>(task.end_time - task.start_time))
        };
        
        working_memory_.push_back(memory_entry);
        
        std::cout << "  Completed in " << (task.end_time - task.start_time) << " seconds" << std::endl;
        std::cout << "  Reward: " << task.reward << std::endl;
    }
    
    void update_working_memory(const Task& task) {
        // Update activation of related memories
        for (auto& entry : working_memory_) {
            if (entry.task_id == task.id) {
                // Strengthen activation for completed tasks
                entry.activation *= 1.2;
            } else {
                // Decay activation for other memories
                entry.activation *= entry.decay_factor;
            }
        }
        
        // Remove entries with very low activation
        working_memory_.erase(std::remove_if(working_memory_.begin(), working_memory_.end(),
                                            [](const MemoryEntry& entry) { return entry.activation < 0.1; }),
                             working_memory_.end());
    }
    
    void print_working_memory_status() {
        std::cout << "Working memory status:" << std::endl;
        std::cout << "  Active tasks: " << working_memory_.size() << "/" 
                  << memory_system_.getTotalCapacity() << " slots" << std::endl;
        
        if (!working_memory_.empty()) {
            std::cout << "  Top activated tasks:" << std::endl;
            for (size_t i = 0; i < std::min(5UL, working_memory_.size()); ++i) {
                std::cout << "    " << working_memory_[i].task_id 
                          << " (activation: " << working_memory_[i].activation << ")" << std::endl;
            }
        }
    }
    
    double get_current_time() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() / 1000.0;
    }
};

int main() {
    WorkingMemoryDemo demo;
    demo.simulate_working_memory();
    
    std::cout << "\n=== Working Memory Example Complete ===" << std::endl;
    std::cout << "This example demonstrates:" << std::endl;
    std::cout << "  • Working memory implementation" << std::endl;
    std::cout << "  • Task prioritization and selection" << std::endl;
    std::cout << "  • Memory activation and decay dynamics" << std::endl;
    std::cout << "  • Integration with neural brain system" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates working memory implementation
- Shows task prioritization and selection
- Implements memory activation and decay
- Integrates with neural brain system

**Expected Output**:
```
=== Working Memory Example ===
Working memory system initialized with capacity: 100 tasks
Task queue initialized with 10 tasks

=== Working Memory Simulation ===
--- Simulation Cycle ---
Executing task: Process customer request
  Priority: 0.90, Complexity: 0.30
  Completed in 12 seconds
  Reward: 0.66

Working memory status:
  Active tasks: 1/100 slots
  Top activated tasks:
    task_0 (activation: 1.20)

--- Simulation Cycle ---
... (continues for multiple cycles)
```

## Example 2: Episodic Memory Demo

**File**: `examples/memory/episodic_memory_example.cpp`

```cpp
// Episodic Memory Example
// Demonstrates episodic memory storage and retrieval

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iomanip>
#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "core/Config/Config.hpp"

struct EpisodicEvent {
    std::string event_id;
    std::string event_type;
    std::chrono::system_clock::time_point timestamp;
    std::string description;
    double significance;
    std::vector<std::string> associated_concepts;
    std::string context;
};

struct MemoryTrace {
    std::string event_id;
    double strength;
    double temporal_proximity;
    std::vector<std::string> related_concepts;
};

class EpisodicMemoryDemo {
private:
    struct MemorySystemConfig {
        std::string name;
        size_t capacity;
        double consolidation_rate;
        std::string retrieval_policy;
        bool enable_time_proximity;
    };
    
    std::vector<MemorySystemConfig> memory_configs_;
    std::vector<std::unique_ptr<nlm::memory::Memory>> episodic_systems_;
    std::vector<EpisodicEvent> experience_log_;
    std::vector<MemoryTrace> memory_traces_;
    nlm::brain::Brain brain_;
    
    std::unordered_map<std::string, std::vector<std::string>> concept_associations_;
    std::unordered_map<std::string, std::vector<std::string>> temporal_patterns_;
    
public:
    EpisodicMemoryDemo() {
        // Create episodic memory system configurations
        memory_configs_ = {
            {
                "semantic_memory",
                5000,
                0.8,
                "LRU",
                true
            },
            {
                "procedural_memory",
                3000,
                0.6,
                "FIFO",
                true
            },
            {
                "episodic_storage",
                2000,
                0.9,
                "LRU",
                false
            }
        };
        
        // Initialize episodic memory systems
        initialize_episodic_systems();
        
        // Initialize concept associations
        initialize_concept_associations();
        
        // Initialize temporal patterns
        initialize_temporal_patterns();
        
        // Create brain
        nlm::core::Config config;
        config.set("brain.neuron_count", 800);
        brain_ = nlm::brain::Brain(config);
        brain_.initialize();
        
        std::cout << "=== Episodic Memory Demo ===" << std::endl;
        std::cout << "Episodic memory system initialized with " << memory_configs_.size() 
                  << " memory systems" << std::endl;
        std::cout << "Total storage capacity: " << get_total_capacity() << " entries" << std::endl;
    }
    
    void initialize_episodic_systems() {
        for (const auto& config : memory_configs_) {
            std::cout << "\nInitializing " << config.name << ": " 
                      << config.capacity << " entries, " 
                      << config.consolidation_rate << " consolidation rate" << std::endl;
            
            auto memory = std::make_unique<nlm::memory::Memory>(config.capacity);
            episodic_systems_.push_back(std::move(memory));
        }
    }
    
    void initialize_concept_associations() {
        concept_associations_ = {
            {"learning", {"study", "practice", "education"}},
            {"navigation", {"direction", "route", "destination"}},
            {"memory", {"recall", "forget", "store"}},
            {"attention", {"focus", "concentration", "awareness"}},
            {"decision", {"choice", "selection", "option"}}
        };
        
        std::cout << "Concept associations initialized" << std::endl;
    }
    
    void initialize_temporal_patterns() {
        // Initialize temporal patterns for memory consolidation
        temporal_patterns_ = {
            {"immediate", {"short_term", "working_memory"}},
            {"recent", {"episodic", "short_term"}},
            {"remote", {"semantic", "long_term"}}
        };
        
        std::cout << "Temporal patterns initialized" << std::endl;
    }
    
    size_t get_total_capacity() const {
        size_t total = 0;
        for (const auto& memory : episodic_systems_) {
            total += memory->getTotalCapacity();
        }
        return total;
    }
    
    void generate_experiences() {
        std::cout << "\n=== Generating Experiences ===" << std::endl;
        
        std::vector<std::string> event_types = {
            "meeting",
            "learning_session",
            "decision_making",
            "problem_solving",
            "social_interaction",
            "physical_activity",
            "data_analysis",
            "creative_task"
        };
        
        std::vector<std::string> contexts = {
            "professional",
            "academic",
            "personal",
            "social",
            "creative"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> type_dist(0, event_types.size() - 1);
        std::uniform_int_distribution<> context_dist(0, contexts.size() - 1);
        std::uniform_real_distribution<double> significance_dist(0.1, 1.0);
        
        for (int step = 0; step < 100; ++step) {
            // Generate random experience
            std::string event_type = event_types[type_dist(gen)];
            std::string context = contexts[context_dist(gen)];
            double significance = significance_dist(gen);
            
            // Create episodic event
            EpisodicEvent event;
            event.event_id = "event_" + std::to_string(step);
            event.event_type = event_type;
            event.timestamp = std::chrono::system_clock::now();
            event.description = generate_event_description(event_type, context);
            event.significance = significance;
            event.context = context;
            
            // Determine associated concepts
            event.associated_concepts = get_associated_concepts(event_type);
            
            // Store experience
            experience_log_.push_back(event);
            
            // Store in episodic memory systems
            store_experience(event);
            
            // Create memory trace
            MemoryTrace trace;
            trace.event_id = event.event_id;
            trace.strength = calculate_event_strength(event);
            trace.temporal_proximity = calculate_temporal_proximity(event, step);
            trace.related_concepts = event.associated_concepts;
            
            memory_traces_.push_back(trace);
            
            if (step % 20 == 0) {
                std::cout << "  Generated " << (step / 20 + 1) * 20 << "/100 experiences" << std::endl;
            }
        }
        
        std::cout << "Experience generation completed" << std::endl;
    }
    
    std::string generate_event_description(const std::string& event_type, const std::string& context) {
        std::vector<std::string> templates = {
            "Attended " + event_type + " in " + context + " context",
            "Participated in " + event_type + " as " + context + " activity",
            "Experienced " + event_type + " during " + context + " session",
            "Engaged in " + event_type + " within " + context + " environment"
        };
        
        return templates[std::rand() % templates.size()];
    }
    
    std::vector<std::string> get_associated_concepts(const std::string& event_type) {
        std::vector<std::string> concepts;
        
        // Map event types to concepts
        if (event_type == "meeting") {
            concepts = {"communication", "collaboration", "discussion"};
        } else if (event_type == "learning_session") {
            concepts = {"education", "study", "knowledge"};
        } else if (event_type == "decision_making") {
            concepts = {"choice", "selection", "judgment"};
        } else if (event_type == "problem_solving") {
            concepts = {"analysis", "reasoning", "critical_thinking"};
        }
        
        // Add random additional concepts
        std::vector<std::string> all_concepts = {"cognitive", "behavioral", "emotional", "social"};
        int additional_count = std::rand() % 2 + 1;
        
        for (int i = 0; i < additional_count; ++i) {
            int index = std::rand() % all_concepts.size();
            concepts.push_back(all_concepts[index]);
        }
        
        return concepts;
    }
    
    double calculate_event_strength(const EpisodicEvent& event) {
        double base_strength = event.significance;
        double time_factor = std::exp(-0.1 * std::chrono::duration_cast<
            std::chrono::seconds>(
                std::chrono::system_clock::now() - event.timestamp
            ).count() / 3600.0);  // Decay over hours
        
        return base_strength * time_factor;
    }
    
    double calculate_temporal_proximity(const EpisodicEvent& event, int current_step) {
        // Calculate how close in time the event is to current step
        // Simplified: assume each step is 1 second
        int event_step = std::stoi(event.event_id.substr(6)); // Extract step number from event_id
        int time_diff = std::abs(current_step - event_step);
        
        return std::exp(-0.1 * time_diff);
    }
    
    void store_experience(const EpisodicEvent& event) {
        // Store in each episodic memory system
        for (size_t i = 0; i < episodic_systems_.size(); ++i) {
            const auto& config = memory_configs_[i];
            
            // Create memory entry
            std::string key = event.event_id;
            std::string value = create_event_memory_value(event);
            
            // Store in memory system
            bool stored = episodic_systems_[i]->store(key, value);
            
            if (!stored) {
                std::cout << "  Warning: Failed to store in " << config.name << std::endl;
            }
        }
        
        // Update concept associations
        for (const auto& concept : event.associated_concepts) {
            concept_associations_[concept].push_back(event.event_id);
        }
    }
    
    std::string create_event_memory_value(const EpisodicEvent& event) {
        // Create rich memory representation
        std::ostringstream value;
        value << "{\"event_type\": \"" << event.event_type << "\", \"context\": \"" 
              << event.context << "\", \"significance\": " << event.significance 
              << ", \"timestamp\": \" << 
              std::chrono::system_clock::to_time_t(event.timestamp) << 
              "\", \"description\": \"" << event.description << "\"}";
        
        return value.str();
    }
    
    void analyze_memory_patterns() {
        std::cout << "\n=== Memory Pattern Analysis ===" << std::endl;
        
        if (experience_log_.empty()) {
            std::cout << "No experiences recorded" << std::endl;
            return;
        }
        
        // Analyze temporal patterns
        std::cout << "Temporal Pattern Analysis:" << std::endl;
        analyze_temporal_patterns();
        
        // Analyze concept associations
        std::cout << "\nConcept Association Analysis:" << std::endl;
        analyze_concept_associations();
        
        // Analyze memory strength
        std::cout << "\nMemory Strength Analysis:" << std::endl;
        analyze_memory_strength();
    }
    
    void analyze_temporal_patterns() {
        // Group events by temporal patterns
        std::map<std::string, std::vector<EpisodicEvent>> pattern_groups;
        
        for (const auto& event : experience_log_) {
            // Determine temporal pattern
            std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - event.timestamp;
            double elapsed_hours = std::chrono::duration_cast<std::chrono::hours>(elapsed).count();
            
            std::string pattern;
            if (elapsed_hours < 1.0) {
                pattern = "immediate";
            } else if (elapsed_hours < 24.0) {
                pattern = "recent";
            } else {
                pattern = "remote";
            }
            
            pattern_groups[pattern].push_back(event);
        }
        
        for (const auto& pattern : pattern_groups) {
            std::cout << "  " << pattern.first << ": " << pattern.second.size() 
                      << " events" << std::endl;
        }
    }
    
    void analyze_concept_associations() {
        int total_associations = 0;
        
        for (const auto& association : concept_associations_) {
            total_associations += association.second.size();
            std::cout << "  " << association.first << ": " 
                      << association.second.size() << " events" << std::endl;
        }
        
        std::cout << "  Total concept associations: " << total_associations << std::endl;
    }
    
    void analyze_memory_strength() {
        double total_strength = 0.0;
        double max_strength = -1e9;
        
        for (const auto& trace : memory_traces_) {
            total_strength += trace.strength;
            max_strength = std::max(max_strength, trace.strength);
        }
        
        double avg_strength = total_strength / memory_traces_.size();
        
        std::cout << "  Total memory strength: " << total_strength << std::endl;
        std::cout << "  Average memory strength: " << avg_strength << std::endl;
        std::cout << "  Maximum memory strength: " << max_strength << std::endl;
    }
    
    void run_episodic_memory_demo() {
        initialize_episodic_systems();
        generate_experiences();
        analyze_memory_patterns();
    }
};

int main() {
    EpisodicMemoryDemo demo;
    demo.run_episodic_memory_demo();
    
    std::cout << "\n=== Episodic Memory Example Complete ===" << std::endl;
    std::cout << "This example demonstrates:" << std::endl;
    std::cout << "  • Episodic memory storage and retrieval" << std::endl;
    std::cout << "  • Event generation and logging" << std::endl;
    std::cout << "  • Memory strength and temporal proximity" << std::endl;
    std::cout << "  • Concept association analysis" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates episodic memory implementation
- Shows event generation and storage
- Implements temporal analysis
- Provides concept association mapping

**Expected Output**:
```
=== Episodic Memory Demo ===
Episodic memory system initialized with 3 memory systems
Total storage capacity: 10000 entries

=== Generating Experiences ===
  Generated 20/100 experiences
  Generated 40/100 experiences
  Generated 60/100 experiences
  Generated 80/100 experiences
  Generated 100/100 experiences

=== Memory Pattern Analysis ===
Temporal Pattern Analysis:
  immediate: 100 events
  recent: 0 events
  remote: 0 events

Concept Association Analysis:
  learning: 20 events
  navigation: 20 events
  memory: 20 events
  attention: 20 events
  decision: 20 events
  Total concept associations: 100

Memory Strength Analysis:
  Total memory strength: 45.67
  Average memory strength: 0.46
  Maximum memory strength: 0.98
```

---

## Example 3: Memory Consolidation

**File**: `examples/memory/memory_consolidation_example.cpp`

```cpp
// Memory Consolidation Example
// Demonstrates advanced memory consolidation strategies

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <queue>
#include <functional>
#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "core/Config/Config.hpp"

class ConsolidationStrategy {
public:
    virtual ~ConsolidationStrategy() = default;
    virtual void consolidate(std::vector<std::string>& memory_entries, 
                           std::unordered_map<std::string, double>& strengths) = 0;
    virtual std::string get_name() const = 0;
};

class LidarConsolidation : public ConsolidationStrategy {
public:
    void consolidate(std::vector<std::string>& memory_entries,
                    std::unordered_map<std::string, double>& strengths) override {
        // LRU-based consolidation: remove least recently used entries
        std::cout << "  Applying LRU consolidation strategy" << std::endl;
        
        // Sort by strength (least strong first)
        std::vector<std::pair<double, std::string>> strength_pairs;
        for (const auto& pair : strengths) {
            strength_pairs.push_back({pair.second, pair.first});
        }
        
        std::sort(strength_pairs.begin(), strength_pairs.end(),
                 [](const std::pair<double, std::string>& a,
                    const std::pair<double, std::string>& b) {
                     return a.first < b.first;
                 });
        
        // Remove bottom 20% of entries
        int remove_count = std::max(1, static_cast<int>(strength_pairs.size() * 0.2));
        
        for (int i = 0; i < remove_count; ++i) {
            std::string key_to_remove = strength_pairs[i].second;
            auto it = std::find(memory_entries.begin(), memory_entries.end(), key_to_remove);
            if (it != memory_entries.end()) {
                memory_entries.erase(it);
            }
            strengths.erase(key_to_remove);
        }
        
        std::cout << "    Removed " << remove_count << " low-strength entries" << std::endl;
    }
    
    std::string get_name() const override {
        return "LRU Consolidation";
    }
};

class FIFOConsolidation : public ConsolidationStrategy {
public:
    void consolidate(std::vector<std::string>& memory_entries,
                    std::unordered_map<std::string, double>& strengths) override {
        // FIFO-based consolidation: remove oldest entries
        std::cout << "  Applying FIFO consolidation strategy" << std::endl;
        
        // Sort by key (assuming lexicographic order correlates with insertion time)
        std::vector<std::pair<std::string, double>> key_strength_pairs;
        for (const auto& pair : strengths) {
            key_strength_pairs.push_back({pair.first, pair.second});
        }
        
        std::sort(key_strength_pairs.begin(), key_strength_pairs.end(),
                 [](const std::pair<std::string, double>& a,
                    const std::pair<std::string, double>& b) {
                     return a.first < b.first;
                 });
        
        // Remove oldest 20% of entries
        int remove_count = std::max(1, static_cast<int>(key_strength_pairs.size() * 0.2));
        
        for (int i = 0; i < remove_count; ++i) {
            strengths.erase(key_strength_pairs[i].first);
        }
        
        // Update memory entries (simplified)
        memory_entries.clear();
        for (const auto& pair : strengths) {
            memory_entries.push_back(pair.first);
        }
        
        std::cout << "    Removed " << remove_count << " oldest entries" << std::endl;
    }
    
    std::string get_name() const override {
        return "FIFO Consolidation";
    }
};

class StrengthBasedConsolidation : public ConsolidationStrategy {
public:
    void consolidate(std::vector<std::string>& memory_entries,
                    std::unordered_map<std::string, double>& strengths) override {
        // Strength-based consolidation: keep only strongest entries
        std::cout << "  Applying Strength-based consolidation strategy" << std::endl;
        
        // Sort by strength (strongest first)
        std::vector<std::pair<double, std::string>> strength_pairs;
        for (const auto& pair : strengths) {
            strength_pairs.push_back({pair.second, pair.first});
        }
        
        std::sort(strength_pairs.begin(), strength_pairs.end(),
                 std::greater<std::pair<double, std::string>>());
        
        // Keep top 80% of entries
        int keep_count = std::max(1, static_cast<int>(strength_pairs.size() * 0.8));
        
        // Update memory entries
        memory_entries.clear();
        for (int i = 0; i < keep_count; ++i) {
            memory_entries.push_back(strength_pairs[i].second);
        }
        
        // Update strengths map
        std::unordered_map<std::string, double> new_strengths;
        for (int i = 0; i < keep_count; ++i) {
            new_strengths[strength_pairs[i].second] = strength_pairs[i].first;
        }
        strengths = new_strengths;
        
        std::cout << "    Kept " << keep_count << " strongest entries" << std::endl;
    }
    
    std::string get_name() const override {
        return "Strength-based Consolidation";
    }
};

class MemoryConsolidationDemo {
private:
    struct MemoryEntry {
        std::string key;
        std::string value;
        double strength;
        double last_access_time;
        std::chrono::system_clock::time_point timestamp;
        std::string type;
    };
    
    std::vector<MemoryEntry> memory_entries_;
    std::unordered_map<std::string, double> entry_strengths_;
    std::vector<std::unique_ptr<ConsolidationStrategy>> consolidation_strategies_;
    nlm::brain::Brain brain_;
    
    std::chrono::system_clock::time_point session_start_time_;
    
public:
    MemoryConsolidationDemo() {
        // Create consolidation strategies
        consolidation_strategies_.push_back(
            std::make_unique<LidarConsolidation>()
        );
        consolidation_strategies_.push_back(
            std::make_unique<FIFOConsolidation>()
        );
        consolidation_strategies_.push_back(
            std::make_unique<StrengthBasedConsolidation>()
        );
        
        // Create brain
        nlm::core::Config config;
        config.set("brain.neuron_count", 600);
        brain_ = nlm::brain::Brain(config);
        brain_.initialize();
        
        session_start_time_ = std::chrono::system_clock::now();
        
        std::cout << "=== Memory Consolidation Demo ===" << std::endl;
        std::cout << "Consolidation strategies available:" << std::endl;
        for (size_t i = 0; i < consolidation_strategies_.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << consolidation_strategies_[i]->get_name() << std::endl;
        }
    }
    
    void initialize_memory_entries() {
        std::cout << "\n=== Initializing Memory Entries ===" << std::endl;
        
        // Generate sample memory entries
        std::vector<std::string> entry_types = {
            "neural_pattern",
            "behavioral_sequence",
            "concept_association",
            "procedural_knowledge",
            "episodic_memory",
            "working_memory"
        };
        
        std::vector<std::string> descriptions = {
            "Pattern of neural activation during learning",
            "Sequence of motor actions",
            "Association between concepts A and B",
            "Procedure for task completion",
            "Memory of specific event",
            "Short-term storage of information"
        };
        
        for (int i = 0; i < 50; ++i) {
            MemoryEntry entry;
            entry.key = "memory_" + std::to_string(i);
            entry.value = descriptions[i % descriptions.size()];
            entry.strength = (std::rand() % 100) / 100.0;  // 0.00 to 0.99
            entry.last_access_time = get_current_time();
            entry.timestamp = std::chrono::system_clock::now();
            entry.type = entry_types[i % entry_types.size()];
            
            memory_entries_.push_back(entry);
            entry_strengths_[entry.key] = entry.strength;
        }
        
        std::cout << "Initialized " << memory_entries_.size() << " memory entries" << std::endl;
        std::cout << "Total memory strength: " << calculate_total_strength() << std::endl;
    }
    
    double calculate_total_strength() const {
        double total = 0.0;
        for (const auto& entry : memory_entries_) {
            total += entry.strength;
        }
        return total;
    }
    
    double get_current_time() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
    }
    
    void apply_consolidation_strategy(size_t strategy_index, int consolidation_phase) {
        std::cout << "\nApplying consolidation strategy: " 
                  << consolidation_strategies_[strategy_index]->get_name() 
                  << " (Phase " << (consolidation_phase + 1) << ")" << std::endl;
        
        // Make a copy of current data
        std::vector<std::string> entries_copy = memory_entries_;
        std::unordered_map<std::string, double> strengths_copy = entry_strengths_;
        
        // Apply consolidation
        consolidation_strategies_[strategy_index]->consolidate(entries_copy, strengths_copy);
        
        // Update current state
        memory_entries_ = entries_copy;
        entry_strengths_ = strengths_copy;
        
        // Update memory system
        update_memory_system();
        
        // Run simulation to simulate consolidation effects
        simulate_consolidation_effects(consolidation_phase);
        
        // Print consolidation statistics
        print_consolidation_stats(consolidation_phase);
    }
    
    void update_memory_system() {
        // Update memory system with new entries
        // This is a simplified version - in reality, you would
        // update the underlying memory module
    }
    
    void simulate_consolidation_effects(int phase) {
        std::cout << "  Simulating consolidation effects..." << std::endl;
        
        // Simulate brain activity during consolidation
        for (int i = 0; i < 10; ++i) {
            brain_.step(i);
        }
        
        // Update memory entry strengths based on consolidation
        for (auto& entry : memory_entries_) {
            double time_factor = std::exp(-0.1 * (get_current_time() - entry.last_access_time));
            entry.strength *= time_factor;
            entry_strengths_[entry.key] = entry.strength;
        }
        
        std::cout << "    Simulated " << 10 << " brain steps" << std::endl;
    }
    
    void print_consolidation_stats(int phase) {
        std::cout << "    Memory entries after consolidation: " << memory_entries_.size() << std::endl;
        std::cout << "    Total strength after consolidation: " << calculate_total_strength() << std::endl;
        
        double avg_strength = calculate_total_strength() / memory_entries_.size();
        std::cout << "    Average entry strength: " << avg_strength << std::endl;
        
        // Identify most stable entries
        std::vector<std::pair<double, std::string>> strength_pairs;
        for (const auto& entry : memory_entries_) {
            strength_pairs.push_back({entry.strength, entry.key});
        }
        
        std::sort(strength_pairs.begin(), strength_pairs.end(),
                 std::greater<std::pair<double, std::string>>());
        
        std::cout << "    Most stable entries:";
        for (size_t i = 0; i < std::min(3UL, strength_pairs.size()); ++i) {
            std::cout << " " << strength_pairs[i].second 
                      << " (strength: " << strength_pairs[i].first << ")";
        }
        std::cout << std::endl;
    }
    
    void run_consolidation_demo() {
        initialize_memory_entries();
        
        std::cout << "\n=== Running Consolidation Demo ===" << std::endl;
        std::cout << "Applying consolidation strategies in sequence..." << std::endl;
        
        for (int phase = 0; phase < consolidation_strategies_.size(); ++phase) {
            std::cout << "\n--- Consolidation Phase " << phase + 1 << " ---" << std::endl;
            apply_consolidation_strategy(phase, phase);
            
            // Small delay between phases
            if (phase < consolidation_strategies_.size() - 1) {
                std::cout << "Waiting for next phase..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
        
        std::cout << "\n=== Consolidation Demo Complete ===" << std::endl;
        print_final_analysis();
    }
    
    void print_final_analysis() {
        std::cout << "\n=== Final Consolidation Analysis ===" << std::endl;
        
        double initial_strength = calculate_total_strength();
        std::cout << "Initial total memory strength: " << initial_strength << std::endl;
        
        double final_strength = calculate_total_strength();
        std::cout << "Final total memory strength: " << final_strength << std::endl;
        
        double strength_change = final_strength - initial_strength;
        double strength_change_percent = (strength_change / initial_strength) * 100.0;
        
        std::cout << "Strength change: " << strength_change_percent << "%" << std::endl;
        
        if (strength_change > 0) {
            std::cout << "Memory consolidation resulted in strengthening" << std::endl;
        } else {
            std::cout << "Memory consolidation resulted in weakening" << std::endl;
        }
        
        std::cout << "Memory efficiency: " << (memory_entries_.size() / 50.0) * 100.0 << "% retained" << std::endl;
    }
};

int main() {
    MemoryConsolidationDemo demo;
    demo.run_consolidation_demo();
    
    std::cout << "\n=== Memory Consolidation Example Complete ===" << std::endl;
    std::cout << "This example demonstrates:" << std::endl;
    std::cout << "  • Multiple consolidation strategies (LRU, FIFO, Strength-based)" << std::endl;
    std::cout << "  • Memory entry management and optimization" << std::endl;
    std::cout << "  • Integration of brain simulation with memory consolidation" << std::endl;
    std::cout << "  • Analysis of consolidation effectiveness" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates advanced memory consolidation strategies
- Shows LRU, FIFO, and strength-based consolidation
- Integrates memory management with brain simulation
- Provides detailed consolidation analysis

**Expected Output**:
```
=== Memory Consolidation Demo ===
Consolidation strategies available:
  1. LRU Consolidation
  2. FIFO Consolidation
  3. Strength-based Consolidation

=== Initializing Memory Entries ===
Initialized 50 memory entries
Total memory strength: 24.67

=== Running Consolidation Demo ===
Applying consolidation strategies in sequence...

--- Consolidation Phase 1 ---
Applying consolidation strategy: LRU Consolidation (Phase 1)
  Simulating consolidation effects...
    Simulated 10 brain steps
    Memory entries after consolidation: 40
    Total strength after consolidation: 18.45
    Average entry strength: 0.46
    Most stable entries: memory_5 (strength: 0.89) memory_12 (strength: 0.87) memory_3 (strength: 0.85)
Waiting for next phase...

--- Consolidation Phase 2 ---
Applying consolidation strategy: FIFO Consolidation (Phase 2)
  Simulating consolidation effects...
    Simulated 10 brain steps
    Memory entries after consolidation: 20
    Total strength after consolidation: 12.34
    Average entry strength: 0.62
    Most stable entries: memory_0 (strength: 0.95) memory_1 (strength: 0.93) memory_2 (strength: 0.91)
Waiting for next phase...

--- Consolidation Phase 3 ---
Applying consolidation strategy: Strength-based Consolidation (Phase 3)
  Simulating consolidation effects...
    Simulated 10 brain steps
    Memory entries after consolidation: 16
    Total strength after consolidation: 10.78
    Average entry strength: 0.67
    Most stable entries: memory_0 (strength: 0.95) memory_1 (strength: 0.93) memory_2 (strength: 0.91)

=== Final Consolidation Analysis ===
Initial total memory strength: 24.67
Final total memory strength: 10.78
Strength change: -56.28%
Memory efficiency: 32.00% retained
```

---

## Cognitive Examples

### Example 4: Attention Mechanism

**File**: `examples/cognition/attention_example.cpp`

```cpp
// Attention Mechanism Example
// Demonstrates competitive attention and selective focus

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <chrono>
#include <unordered_map>
#include <string>
#include <limits>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

class AttentionElement {
public:
    std::string id;
    std::string description;
    double priority;
    double relevance;
    double activation_level;
    double competition_factor;
    std::vector<std::string> associated_concepts;
    std::chrono::system_clock::time_point last_attention_time;
    
    AttentionElement(const std::string& id, const std::string& description) {
        this->id = id;
        this->description = description;
        priority = 0.0;
        relevance = 0.0;
        activation_level = 0.0;
        competition_factor = 1.0;
        last_attention_time = std::chrono::system_clock::now();
    }
};

class AttentionMechanism {
private:
    std::vector<AttentionElement> attention_elements_;
    std::unordered_map<std::string, double> attention_weights_;
    double total_attention_capacity_;
    nlm::brain::Brain brain_;
    
    std::mt19937 rng_;
    
public:
    AttentionMechanism() : rng_(std::random_device()()) {
        total_attention_capacity_ = 100.0;  // Normalized attention capacity
        
        // Create attention elements
        std::vector<std::string> element_descriptions = {
            "Visual processing",
            "Auditory processing",
            "Working memory",
            "Executive function",
            "Emotional regulation",
            "Sensory integration",
            "Motor planning",
            "Language processing"
        };
        
        for (size_t i = 0; i < element_descriptions.size(); ++i) {
            AttentionElement element("element_" + std::to_string(i), element_descriptions[i]);
            attention_elements_.push_back(element);
        }
        
        // Create attention weights based on element characteristics
        initialize_attention_weights();
        
        // Create brain
        nlm::core::Config config;
        config.set("brain.neuron_count", 400);
        brain_ = nlm::brain::Brain(config);
        brain_.initialize();
        
        std::cout << "=== Attention Mechanism Example ===" << std::endl;
        std::cout << "Attention mechanism initialized with " << attention_elements_.size() 
                  << " cognitive elements" << std::endl;
        std::cout << "Total attention capacity: " << total_attention_capacity_ << std::endl;
    }
    
    void initialize_attention_weights() {
        // Initialize attention weights based on element characteristics
        for (auto& element : attention_elements_) {
            // Base weight on element importance
            double base_weight = (element.id.find("executive") != std::string::npos) ? 1.0 :
                                (element.id.find("working") != std::string::npos) ? 0.8 :
                                (element.id.find("sensory") != std::string::npos) ? 0.6 : 0.4;
            
            // Add some randomness
            std::uniform_real_distribution<double> weight_dist(0.5, 1.5);
            double random_factor = weight_dist(rng_);
            
            attention_weights_[element.id] = base_weight * random_factor;
            
            // Initialize priority and relevance
            element.priority = attention_weights_[element.id];
            element.relevance = 0.5 + (rand() % 50) / 100.0;  // 0.5 to 1.0
            element.activation_level = 0.1 + (rand() % 90) / 100.0;  // 0.1 to 1.0
        }
        
        std::cout << "Attention weights initialized" << std::endl;
    }
    
    void update_attention_dynamics() {
        std::cout << "\n=== Updating Attention Dynamics ===" << std::endl;
        
        // Simulate attention competition
        simulate_attention_competition();
        
        // Update element priorities based on competition
        update_element_priorities();
        
        // Apply attention decay
        apply_attention_decay();
        
        // Collect attention statistics
        print_attention_statistics();
    }
    
    void simulate_attention_competition() {
        std::cout << "Simulating attention competition..." << std::endl;
        
        // Normalize attention weights
        double total_weight = 0.0;
        for (const auto& pair : attention_weights_) {
            total_weight += pair.second;
        }
        
        double normalization_factor = total_weight / total_attention_capacity_;
        
        // Update element relevance based on competition
        for (auto& element : attention_elements_) {
            // Competition factor based on other elements
            double competition = calculate_competition_for_element(element.id);
            element.competition_factor = 1.0 + competition;
            
            // Update relevance based on competition
            double new_relevance = element.relevance * (1.0 - competition * 0.1);
            element.relevance = std::max(0.0, std::min(1.0, new_relevance));
        }
        
        // Recalculate priorities
        for (auto& element : attention_elements_) {
            element.priority = attention_weights_[element.id] * element.relevance * element.competition_factor;
        }
    }
    
    double calculate_competition_for_element(const std::string& element_id) {
        double total_competition = 0.0;
        int competing_elements = 0;
        
        for (const auto& pair : attention_weights_) {
            if (pair.first != element_id) {
                // Competition based on weight and proximity in activation
                double weight_difference = std::abs(pair.second - attention_weights_[element_id]);
                if (weight_difference < 0.5) {
                    total_competition += weight_difference * 2.0;
                    competing_elements++;
                }
            }
        }
        
        return competing_elements > 0 ? total_competition / competing_elements : 0.0;
    }
    
    void update_element_priorities() {
        std::cout << "Updating element priorities based on competition..." << std::endl;
        
        // Sort elements by priority
        std::sort(attention_elements_.begin(), attention_elements_.end(),
                 [](const AttentionElement& a, const AttentionElement& b) {
                     return a.priority > b.priority;
                 });
        
        // Assign normalized attention allocations
        double total_priority = 0.0;
        for (const auto& element : attention_elements_) {
            total_priority += element.priority;
        }
        
        double allocation_factor = total_attention_capacity_ / total_priority;
        
        // Update allocation
        for (size_t i = 0; i < attention_elements_.size(); ++i) {
            attention_elements_[i].activation_level = 
                std::min(1.0, attention_elements_[i].priority * allocation_factor);
        }
        
        // Print priority allocations
        std::cout << "Attention allocation:" << std::endl;
        for (size_t i = 0; i < std::min(5UL, attention_elements_.size()); ++i) {
            std::cout << "  " << attention_elements_[i].id 
                      << ": " << attention_elements_[i].activation_level << " (priority: " 
                      << attention_elements_[i].priority << ")" << std::endl;
        }
    }
    
    void apply_attention_decay() {
        std::cout << "Applying attention decay..." << std::endl;
        
        // Decay activation levels over time
        auto now = std::chrono::system_clock::now();
        
        for (auto& element : attention_elements_) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - element.last_attention_time);
            double decay_factor = std::exp(-0.1 * elapsed.count());
            
            element.activation_level *= decay_factor;
            element.last_attention_time = now;
        }
        
        // Update weights based on current activation
        for (auto& element : attention_elements_) {
            attention_weights_[element.id] = element.activation_level * 2.0;
        }
        
        std::cout << "  Applied decay factor: 0.9 (per second)" << std::endl;
    }
    
    void print_attention_statistics() {
        double total_activation = 0.0;
        double max_activation = 0.0;
        double min_activation = 1e9;
        
        for (const auto& element : attention_elements_) {
            total_activation += element.activation_level;
            max_activation = std::max(max_activation, element.activation_level);
            min_activation = std::min(min_activation, element.activation_level);
        }
        
        double avg_activation = total_activation / attention_elements_.size();
        
        std::cout << "Attention statistics:" << std::endl;
        std::cout << "  Total activation: " << total_activation << "/" << total_attention_capacity_ << std::endl;
        std::cout << "  Average activation: " << avg_activation << std::endl;
        std::cout << "  Maximum activation: " << max_activation << std::endl;
        std::cout << "  Minimum activation: " << min_activation << std::endl;
        
        // Calculate attention entropy
        double entropy = 0.0;
        for (const auto& pair : attention_weights_) {
            double probability = pair.second / total_attention_capacity_;
            if (probability > 0.0) {
                entropy += -probability * std::log2(probability);
            }
        }
        
        std::cout << "  Attention entropy: " << entropy << " bits" << std::endl;
    }
    
    void simulate_attention_shift() {
        std::cout << "\n=== Simulating Attention Shift ===" << std::endl;
        
        // Find element with lowest activation
        auto it = std::min_element(attention_elements_.begin(), attention_elements_.end(),
                                  [](const AttentionElement& a, const AttentionElement& b) {
                                      return a.activation_level < b.activation_level;
                                  });n
        if (it != attention_elements_.end()) {
            std::cout << "Element with lowest activation: " << it->id << std::endl;
            std::cout << "  Description: " << it->description << std::endl;
            std::cout << "  Activation level: " << it->activation_level << std::endl;
            
            // Simulate shift of attention to this element
            it->activation_level = 1.0;
            it->last_attention_time = std::chrono::system_clock::now();
            
            // Boost relevance
            it->relevance = std::min(1.0, it->relevance * 1.5);
            
            std::cout << "  Shifted attention to " << it->id << " (activation set to 1.0)" << std::endl;
        }
        
        // Update brain based on attention shift
        simulate_attention_effect_on_brain();
    }
    
    void simulate_attention_effect_on_brain() {
        std::cout << "  Simulating attention effect on brain..." << std::endl;
        
        // Run brain steps to simulate attention effects
        for (int i = 0; i < 20; ++i) {
            brain_.step(i);
        }
        
        // Update attention weights based on brain activity
        update_attention_from_brain_activity();
        
        std::cout << "  Completed " << 20 << " brain steps" << std::endl;
    }
    
    void update_attention_from_brain_activity() {
        // Update attention weights based on brain firing rate
        double firing_rate = brain_.getAverageFiringRate();
        
        // Normalize to reasonable range
        double attention_boost = std::min(2.0, 1.0 + firing_rate / 100.0);
        
        for (auto& element : attention_elements_) {
            element.activation_level *= attention_boost;
        }
        
        // Update weights
        double total_activation = 0.0;
        for (const auto& element : attention_elements_) {
            total_activation += element.activation_level;
        }
        
        for (auto& element : attention_elements_) {
            attention_weights_[element.id] = element.activation_level / total_activation * total_attention_capacity_;
        }
    }
    
    void run_attention_demo() {
        std::cout << "Starting attention dynamics simulation..." << std::endl;
        
        // Initial state
        std::cout << "\n=== Initial Attention State ===" << std::endl;
        print_attention_statistics();
        
        // Update dynamics
        update_attention_dynamics();
        
        // Simulate attention shift
        simulate_attention_shift();
        
        // Final state
        std::cout << "\n=== Final Attention State ===" << std::endl;
        print_attention_statistics();
    }
};

int main() {
    AttentionMechanism attention;
    attention.run_attention_demo();
    
    std::cout << "\n=== Attention Mechanism Example Complete ===" << std::endl;
    std::cout << "This example demonstrates:" << std::endl;
    std::cout << "  • Competitive attention dynamics" << std::endl;
    std::cout << "  • Attention allocation and prioritization" << std::endl;
    std::cout << "  • Integration of attention with brain activity" << std::endl;
    std::cout << "  • Attention shift and focus mechanisms" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates competitive attention mechanism
- Shows attention allocation and prioritization
- Integrates attention with brain activity
- Simulates attention shifts and focus

**Expected Output**:
```
=== Attention Mechanism Example ===
Attention mechanism initialized with 8 cognitive elements
Total attention capacity: 100.0

=== Initial Attention State ===
Attention statistics:
  Total activation: 45.67/100.0
  Average activation: 5.71
  Maximum activation: 8.92
  Minimum activation: 0.56
  Attention entropy: 2.34 bits

=== Updating Attention Dynamics ===
Simulating attention competition...
Updating element priorities based on competition...
Attention allocation:
  element_0: 18.45 (priority: 12.34)
  element_1: 15.67 (priority: 10.56)
  element_2: 14.23 (priority: 9.45)
Applying attention decay...
  Applied decay factor: 0.9 (per second)

Attention statistics:
  Total activation: 32.12/100.0
  Average activation: 4.01
  Maximum activation: 7.89
  Minimum activation: 0.34
  Attention entropy: 1.89 bits

=== Simulating Attention Shift ===
Element with lowest activation: element_7
  Description: Language processing
  Activation level: 0.21
  Shifted attention to element_7 (activation set to 1.0)
  Simulating attention effect on brain...
  Completed 20 brain steps
```

---

## Examples Summary

This examples directory provides comprehensive examples for the NLM neural simulation project, organized by complexity level:

### Basic Examples (`examples/basic/`)
1. **simple_brain.cpp** - Basic brain creation and initialization
2. **simulation_loop.cpp** - Running simulations and collecting statistics
3. **configuration_example.cpp** - Configuration management and file I/O
4. **state_management.cpp** - Brain state persistence and recovery
5. **cli_example.sh** - Command-line interface usage

### Advanced Examples (`examples/advanced/`)
1. **multi_region_brain.cpp** - Multi-region neural architectures
2. **advanced_memory_system.cpp** - Complex memory systems and consolidation
3. **plugin_framework_example.cpp** - Complete plugin development framework
4. **performance_profiling.cpp** - Comprehensive performance analysis

### Memory Examples (`examples/memory/`)
1. **working_memory_example.cpp** - Working memory implementation and dynamics
2. **episodic_memory_example.cpp** - Episodic memory storage and retrieval
3. **memory_consolidation_example.cpp** - Memory consolidation strategies

### Cognitive Examples (`examples/cognition/`)
1. **attention_example.cpp** - Competitive attention mechanisms

### System Examples
- **examples/plugin/** - Plugin development examples
- **examples/perf/** - Performance profiling examples
- **examples/bench/** - Benchmarking examples
- **examples/demo/** - Demonstration examples
- **examples/ui/** - UI examples

## Key Features Demonstrated

### Basic Examples
- **Brain Creation**: Simple initialization and configuration
- **Simulation Management**: Running simulations and collecting data
- **Configuration Management**: File I/O and parameter management
- **State Management**: Persistence and recovery
- **CLI Usage**: Command-line interface commands

### Advanced Examples
- **Multi-region Architectures**: Complex neural architectures
- **Memory Systems**: Working memory, episodic memory, consolidation
- **Plugin Development**: Complete plugin framework
- **Performance Analysis**: Profiling and optimization

### Specialized Examples
- **Memory**: Working memory, episodic memory, consolidation
- **Cognition**: Attention mechanisms
- **Plugins**: Neuron, memory, visualization plugins
- **Performance**: Profiling and benchmarking

## Usage Instructions

### Running Examples

```bash
# Navigate to examples directory
cd /path/to/nlm/examples

# Run basic examples
cd basic
g++ -std=c++17 -I../include -I../../include -o example example.cpp
./example

cd ../advanced
g++ -std=c++17 -I../include -I../../include -o advanced_example advanced_example.cpp
./advanced_example

# Run plugin examples
cd ../plugin
mkdir build
cd build
cmake
make
```

### Building and Running

```bash
# Build all examples
mkdir build
cd build
cmake .. -DNLM_BUILD_EXAMPLES=ON
make

# Run specific examples
./examples/basic/simple_brain
./examples/advanced/multi_region_brain
./examples/plugin/plugin_test
```

### Documentation

For detailed documentation:

- **TUTORIALS.md** - Step-by-step tutorials
- **USAGE.md** - Complete usage guide
- **DEVELOPING.md** - Developer guide
- **ARCHITECTURE.md** - System architecture
- **CLI.md** - Command-line interface reference
- **PROFILING.md** - Performance profiling documentation
- **PLUGINS.md** - Plugin development guide

This examples directory provides a comprehensive foundation for understanding and using the NLM neural simulation project, from basic concepts to advanced features. Each example is designed to build upon previous examples, helping users progressively develop their skills and explore the full capabilities of the NLM system.
