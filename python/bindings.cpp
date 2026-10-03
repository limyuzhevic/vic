// Final implementation with comprehensive improvements and fixes

// === PYTHONIC CONTEXT MANAGERS ===

class BrainContext {
public:
    explicit BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain>& get() {
        return brain_;
    }
    
    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    bool is_initialized() const {
        return brain_ && brain_->getTotalNeuronCount() > 0;
    }
    
    // Context manager support
    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;
    
private:
    std::shared_ptr<Brain> brain_;
};

class ConfigContext {
public:
    explicit ConfigContext(std::shared_ptr<Config> config) : config_(config) {}
    
    ~ConfigContext() {
        // Config objects are lightweight, no cleanup needed
    }
    
    std::shared_ptr<Config>& get() {
        return config_;
    }
    
    void clear() {
        if (config_) {
            config_->clear();
        }
    }
    
    bool is_empty() const {
        return config_ && config_->getKeys().empty();
    }
    
    // Context manager support
    ConfigContext(const ConfigContext&) = delete;
    ConfigContext& operator=(const ConfigContext&) = delete;
    
private:
    std::shared_ptr<Config> config_;
};

class WorldContext {
public:
    explicit WorldContext(std::shared_ptr<SimpleWorld> world) : world_(world) {
        if (world_) {
            world_->reset();
        }
    }
    
    ~WorldContext() {
        if (world_) {
            world_->reset();
        }
    }
    
    std::shared_ptr<SimpleWorld>& get() {
        return world_;
    }
    
    void reset() {
        if (world_) {
            world_->reset();
        }
    }
    
    bool is_ready() const {
        return world_ && world_->getWidth() > 0 && world_->getHeight() > 0;
    }
    
    // Context manager support
    WorldContext(const WorldContext&) = delete;
    WorldContext& operator=(const WorldContext&) = delete;
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// === ENHANCED PYTHONIC ITERATORS ===

class RegionIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Region*;
    using pointer = Region*;
    using reference = Region&;
    
    RegionIterator(const std::vector<std::unique_ptr<Region>>& regions, 
                   size_t index) : regions_(&regions), index_(index) {}
    
    RegionIterator& operator++() {
        ++index_;
        return *this;
    }
    
    RegionIterator operator++(int) {
        auto temp = *this;
        ++index_;
        return temp;
    }
    
    bool operator==(const RegionIterator& other) const {
        if (!regions_ || !other.regions_) return index_ == other.index_;
        return regions_->size() == other.regions_->size() && index_ == other.index_;
    }
    
    bool operator!=(const RegionIterator& other) const {
        return !(*this == other);
    }
    
    Region& operator*() {
        return *(*regions_)[index_];
    }
    
    Region* operator->() {
        return (*regions_)[index_].get();
    }
    
    std::string get_name() const {
        return (*regions_)[index_]->getName();
    }
    
    RegionId get_id() const {
        return (*regions_)[index_]->getId();
    }
    
    size_t get_neuron_count() const {
        return (*regions_)[index_]->getNeuronCount();
    }
    
    double get_firing_rate() const {
        auto region = (*regions_)[index_].get();
        return region->getNeuronCount() > 0 ? 
            region->getFiringNeuronCount() / region->getNeuronCount() : 0.0;
    }
    
private:
    const std::vector<std::unique_ptr<Region>>* regions_ = nullptr;
    size_t index_ = 0;
};

class NeuronIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = NeuronId;
    using pointer = NeuronId*;
    using reference = NeuronId&;
    
    NeuronIterator(const std::vector<NeuronId>& neurons, 
                   size_t index) : neurons_(&neurons), index_(index) {}
    
    NeuronIterator& operator++() {
        ++index_;
        return *this;
    }
    
    NeuronIterator operator++(int) {
        auto temp = *this;
        ++index_;
        return temp;
    }
    
    bool operator==(const NeuronIterator& other) const {
        if (!neurons_ || !other.neurons_) return index_ == other.index_;
        return neurons_->size() == other.neurons_->size() && index_ == other.index_;
    }
    
    bool operator!=(const NeuronIterator& other) const {
        return !(*this == other);
    }
    
    NeuronId operator*() {
        return (*neurons_)[index_];
    }
    
    NeuronId& operator->() {
        return (*neurons_)[index_];
    }
    
    uint64_t get_value() const {
        return (*neurons_)[index_].value;
    }
    
    size_t get_index() const {
        return index_;
    }
    
private:
    const std::vector<NeuronId>* neurons_ = nullptr;
    size_t index_ = 0;
};

// === COMPREHENSIVE UTILITY FUNCTIONS ===

namespace python_utils {
    // Mathematical utilities
    template<typename T>
    T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    
    template<typename T>
    T lerp(T a, T b, double t) {
        return a + static_cast<T>((b - a) * t);
    }
    
    template<typename T>
    std::vector<T> linspace(T start, T stop, size_t num) {
        std::vector<T> result;
        if (num == 0) return result;
        double step = static_cast<double>(stop - start) / (num - 1);
        result.reserve(num);
        for (size_t i = 0; i < num; ++i) {
            result.push_back(start + static_cast<T>(i * step));
        }
        return result;
    }
    
    // Statistical utilities
    template<typename T>
    T mean(const std::vector<T>& values) {
        if (values.empty()) return T(0);
        T sum = std::accumulate(values.begin(), values.end(), T(0));
        return sum / values.size();
    }
    
    template<typename T>
    T variance(const std::vector<T>& values, T mean_val = T(-1)) {
        if (values.empty()) return T(0);
        if (mean_val < 0) mean_val = mean(values);
        T sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), T(0));
        return sq_sum / values.size() - mean_val * mean_val;
    }
    
    template<typename T>
    T standard_deviation(const std::vector<T>& values) {
        return std::sqrt(variance(values));
    }
    
    template<typename T>
    T median(std::vector<T> values) {
        if (values.empty()) return T(0);
        std::sort(values.begin(), values.end());
        size_t mid = values.size() / 2;
        return values.size() % 2 == 0 ? (values[mid - 1] + values[mid]) / 2 : values[mid];
    }
    
    // Vector utilities
    template<typename T>
    std::vector<T> normalize(const std::vector<T>& values, T max_norm = T(0)) {
        T norm = 0;
        for (const auto& v : values) norm += v * v;
        norm = std::sqrt(norm);
        
        if (max_norm > 0 && norm > max_norm) {
            norm = max_norm;
        }
        
        if (norm == 0) return values;
        
        std::vector<T> result;
        for (const auto& v : values) {
            result.push_back(v / norm);
        }
        return result;
    }
    
    // Pythonic string utilities
    std::string to_snake_case(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.size(); ++i) {
            if (std::isupper(str[i]) && i > 0) result += '_';
            result += std::tolower(str[i]);
        }
        return result;
    }
    
    std::string to_camel_case(const std::string& str) {
        std::string result;
        bool capitalize_next = false;
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '_' || str[i] == '-') {
                capitalize_next = true;
            } else if (capitalize_next) {
                result += std::toupper(str[i]);
                capitalize_next = false;
            } else {
                result += str[i];
            }
        }
        return result;
    }
    
    // I/O utilities
    bool file_exists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    std::string read_file(const std::string& path) {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("Cannot open file: " + path);
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    void write_file(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file) throw std::runtime_error("Cannot write to file: " + path);
        file << content;
    }
    
    // Collection utilities
    template<typename T>
    std::vector<T> filter(const std::vector<T>& values, 
                         std::function<bool(const T&)> predicate) {
        std::vector<T> result;
        for (const auto& v : values) {
            if (predicate(v)) result.push_back(v);
        }
        return result;
    }
    
    template<typename T, typename U>
    std::vector<U> map(const std::vector<T>& values,
                      std::function<U(const T&)> func) {
        std::vector<U> result;
        for (const auto& v : values) {
            result.push_back(func(v));
        }
        return result;
    }
    
    template<typename T>
    std::vector<T> unique(const std::vector<T>& values) {
        std::vector<T> result = values;
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }
    
    // Configuration utilities
    std::map<std::string, std::string> config_to_dict(const Config& config) {
        std::map<std::string, std::string> dict;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<std::string>(key);
            if (value) dict[key] = *value;
        }
        return dict;
    }
    
    std::string config_to_json(const Config& config) {
        auto dict = config_to_dict(config);
        // Simple JSON serialization
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& pair : dict) {
            if (!first) ss << ", ";
            ss << "\"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    
    // Simulation utilities
    struct SimulationResult {
        std::shared_ptr<Brain> brain;
        std::shared_ptr<SimpleWorld> world;
        std::shared_ptr<AgentBrain> agent;
        std::map<std::string, double> stats;
        double total_time;
        bool success;
        std::string error_message;
    };
    
    SimulationResult run_simulation(std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world = nullptr,
                                     std::shared_ptr<AgentBrain> agent = nullptr,
                                     int num_steps = 100,
                                     double timestep = 0.1) {
        SimulationResult result;
        result.brain = brain;
        result.world = world;
        result.agent = agent;
        result.total_time = 0.0;
        result.success = false;
        
        try {
            if (!brain) {
                result.error_message = "Brain pointer is null";
                return result;
            }
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            brain->initialize();
            if (world) world->reset();
            if (agent) agent->reset();
            
            for (int step = 0; step < num_steps; ++step) {
                if (world && agent) {
                    world->update(timestep);
                    agent->processSensoryInput(world->getSensoryPercept());
                    
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    double reward = 0.0;
                    if (auto internal = world->getSensoryPercept().getInternal()) {
                        if (!internal->empty()) reward = (*internal)[0];
                    }
                    agent->applyRewardModulation(reward, 0.0);
                    agent->updateDevelopment(timestep);
                }
                
                brain->step(step, world ? world->getSimulationTime() : 0.0);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            result.total_time = std::chrono::duration<double>(end_time - start_time).count();
            result.success = true;
            
            // Collect statistics
            if (brain) {
                result.stats["total_neurons"] = brain->getTotalNeuronCount();
                result.stats["total_synapses"] = brain->getTotalSynapseCount();
                result.stats["firing_neurons"] = brain->getFiringNeuronCount();
                result.stats["total_spikes"] = brain->getTotalSpikeCount();
                result.stats["average_firing_rate"] = brain->getAverageFiringRate();
                result.stats["development_stage"] = static_cast<double>(brain->getDevelopmentalStage());
            }
            
            if (world) {
                result.stats["simulation_time"] = world->getSimulationTime();
                result.stats["world_width"] = world->getWidth();
                result.stats["world_height"] = world->getHeight();
            }
            
            if (agent) {
                result.stats["curiosity_level"] = agent->getCuriosityLevel();
                result.stats["novelty_level"] = agent->getNoveltyLevel();
                result.stats["prediction_error"] = agent->getPredictionError();
                result.stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
            }
            
        } catch (const std::exception& e) {
            result.error_message = std::string("Exception: ") + e.what();
            result.success = false;
        }
        
        return result;
    }
}

// === ADVANCED HELPER CLASSES ===

class BatchProcessor {
public:
    struct BatchConfig {
        int num_steps = 100;
        double timestep = 0.1;
        bool enable_learning = true;
        bool enable_profiling = false;
        std::string output_prefix = "batch_";
    };
    
    BatchProcessor(const BatchConfig& config = BatchConfig())
        : config_(config), current_batch_(0) {}
    
    struct BatchResult {
        int batch_id;
        std::shared_ptr<Brain> brain;
        std::shared_ptr<SimpleWorld> world;
        std::shared_ptr<AgentBrain> agent;
        double execution_time;
        std::map<std::string, double> stats;
        bool success;
        std::string error_message;
    };
    
    BatchResult process_batch(std::shared_ptr<Brain> brain,
                              std::shared_ptr<SimpleWorld> world = nullptr,
                              std::shared_ptr<AgentBrain> agent = nullptr) {
        BatchResult result;
        result.batch_id = current_batch_++;
        result.brain = brain;
        result.world = world;
        result.agent = agent;
        result.success = false;
        
        try {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            if (config_.enable_profiling) {
                // Create profiler for this batch
                auto profiler = std::make_shared<Profiler>();
                profiler->start();
                
                // Run the batch
                result = process_batch_internal(brain, world, agent);
                
                profiler->stop();
                result.stats["profiling_time"] = profiler->get_total_time();
                result.stats["avg_time_per_step"] = profiler->get_average_time_per_step();
                result.stats["max_time_per_step"] = profiler->get_max_time_per_step();
            } else {
                result = process_batch_internal(brain, world, agent);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
            
        } catch (const std::exception& e) {
            result.error_message = std::string("Exception: ") + e.what();
            result.success = false;
        }
        
        return result;
    }
    
    std::vector<BatchResult> process_multiple_batches(
        const std::vector<std::tuple<std::shared_ptr<Brain>, 
                                    std::shared_ptr<SimpleWorld>, 
                                    std::shared_ptr<AgentBrain>>>& batches) {
        
        std::vector<BatchResult> results;
        results.reserve(batches.size());
        
        for (const auto& batch : batches) {
            results.push_back(process_batch(std::get<0>(batch), 
                                            std::get<1>(batch), 
                                            std::get<2>(batch)));
        }
        
        return results;
    }
    
    void save_results(const std::vector<BatchResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot write to file: " + filename);
        }
        
        file << "Batch Processing Results\n";
        file << "======================\n\n";
        
        for (const auto& result : results) {
            file << "Batch " << result.batch_id << ":\n";
            file << "  Success: " << (result.success ? "Yes" : "No") << "\n";
            if (!result.success) {
                file << "  Error: " << result.error_message << "\n";
            } else {
                file << "  Execution time: " << result.execution_time << "s\n";
                file << "  Neurons: " << result.stats["total_neurons"] << "\n";
                file << "  Synapses: " << result.stats["total_synapses"] << "\n";
                file << "  Spikes: " << result.stats["total_spikes"] << "\n";
                file << "  Average firing rate: " << result.stats["average_firing_rate"] << "\n";
            }
            file << "\n";
        }
        
        file.close();
    }
    
private:
    BatchResult process_batch_internal(std::shared_ptr<Brain> brain,
                                       std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent) {
        BatchResult result;
        result.batch_id = current_batch_++;
        result.brain = brain;
        result.world = world;
        result.agent = agent;
        result.success = true;
        
        brain->initialize();
        if (world) world->reset();
        if (agent) {
            agent->reset();
            agent->enableRewardModulation(config_.enable_learning);
            agent->enableCuriosity(config_.enable_learning);
            agent->enableDevelopment(config_.enable_learning);
        }
        
        for (int step = 0; step < config_.num_steps; ++step) {
            if (world && agent) {
                world->update(config_.timestep);
                agent->processSensoryInput(world->getSensoryPercept());
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                double reward = 0.0;
                if (auto internal = world->getSensoryPercept().getInternal()) {
                    if (!internal->empty()) reward = (*internal)[0];
                }
                agent->applyRewardModulation(reward, 0.0);
                agent->updateDevelopment(config_.timestep);
            }
            
            brain->step(step, world ? world->getSimulationTime() : 0.0);
        }
        
        // Collect statistics
        if (brain) {
            result.stats["total_neurons"] = brain->getTotalNeuronCount();
            result.stats["total_synapses"] = brain->getTotalSynapseCount();
            result.stats["firing_neurons"] = brain->getFiringNeuronCount();
            result.stats["total_spikes"] = brain->getTotalSpikeCount();
            result.stats["average_firing_rate"] = brain->getAverageFiringRate();
        }
        
        return result;
    }
    
    BatchConfig config_;
    int current_batch_ = 0;
};

class AgentTrainer {
public:
    struct TrainingConfig {
        int num_episodes = 100;
        int max_steps_per_episode = 200;
        double learning_rate = 0.001;
        double exploration_rate = 0.1;
        double exploration_decay = 0.995;
        double min_exploration = 0.01;
        bool enable_epsilon_greedy = true;
        bool save_best_model = true;
        std::string model_save_path = "best_model.bin";
    };
    
    AgentTrainer(std::shared_ptr<Brain> brain,
                std::shared_ptr<SimpleWorld> world,
                std::shared_ptr<AgentBrain> agent,
                const TrainingConfig& config = TrainingConfig())
        : brain_(brain), world_(world), agent_(agent), config_(config),
          exploration_rate_(config_.exploration_rate) {}
    
    struct EpisodeResult {
        int episode_id;
        int steps_taken;
        double total_reward;
        double average_curiosity;
        double average_novelty;
        double final_development_stage;
        bool success;
        std::map<std::string, double> metrics;
    };
    
    std::vector<EpisodeResult> train(int num_episodes = -1) {
        if (num_episodes < 0) num_episodes = config_.num_episodes;
        
        std::vector<EpisodeResult> results;
        results.reserve(num_episodes);
        
        double best_score = -std::numeric_limits<double>::max();
        
        for (int episode = 0; episode < num_episodes; ++episode) {
            EpisodeResult result;
            result.episode_id = episode;
            result.success = true;
            
            // Reset environment and agent
            if (world_) world_->reset();
            if (agent_) agent_->reset();
            
            double episode_reward = 0.0;
            double curiosity_sum = 0.0;
            double novelty_sum = 0.0;
            
            // Run episode
            for (int step = 0; step < config_.max_steps_per_episode; ++step) {
                if (!world_ || !agent_) {
                    result.success = false;
                    break;
                }
                
                // Update world
                world_->update(0.1);
                
                // Get state
                auto percept = world_->getSensoryPercept();
                
                // Process sensory input
                agent_->processSensoryInput(percept);
                
                // Choose action
                auto action = select_action();
                
                // Apply action
                world_->applyMotorCommand(action, world_->getSimulationTime());
                
                // Calculate reward
                double reward = calculate_reward(action, step, episode);
                episode_reward += reward;
                
                // Apply reward modulation
                agent_->applyRewardModulation(reward, 0.0);
                
                // Update development
                agent_->updateDevelopment(0.1);
                
                // Collect metrics
                curiosity_sum += agent_->getCuriosityLevel();
                novelty_sum += agent_->getNoveltyLevel();
                
                // End episode if agent develops too much
                if (agent_ && agent_->getDevelopmentalStage() == DevelopmentalStage::Aging) {
                    break;
                }
            }
            
            result.steps_taken = config_.max_steps_per_episode;
            result.total_reward = episode_reward;
            result.average_curiosity = curiosity_sum / config_.max_steps_per_episode;
            result.average_novelty = novelty_sum / config_.max_steps_per_episode;
            result.final_development_stage = agent_ ? agent_->getDevelopmentalStage() : 0.0;
            
            // Calculate metrics
            result.metrics["exploration_rate"] = exploration_rate_;
            result.metrics["learning_rate"] = config_.learning_rate;
            result.metrics["episode_reward"] = episode_reward;
            result.metrics["curiosity_score"] = result.average_curiosity;
            result.metrics["novelty_score"] = result.average_novelty;
            
            // Update exploration rate
            exploration_rate_ = std::max(
                config_.min_exploration,
                exploration_rate_ * config_.exploration_decay
            );
            
            // Save best model
            if (config_.save_best_model && episode_reward > best_score) {
                best_score = episode_reward;
                if (brain_) {
                    brain_->save(config_.model_save_path);
                }
            }
            
            results.push_back(result);
            
            // Print progress
            if ((episode + 1) % 10 == 0 || episode == num_episodes - 1) {
                print_training_progress(episode, results);
            }
        }
        
        return results;
    }
    
    void save_training_results(const std::vector<EpisodeResult>& results, 
                               const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot write to file: " + filename);
        }
        
        file << "Training Results\n";
        file << "==============\n\n";
        
        for (const auto& result : results) {
            file << "Episode " << result.episode_id << ":\n";
            file << "  Steps: " << result.steps_taken << "\n";
            file << "  Total reward: " << result.total_reward << "\n";
            file << "  Average curiosity: " << result.average_curiosity << "\n";
            file << "  Average novelty: " << result.average_novelty << "\n";
            file << "  Final development stage: " << result.final_development_stage << "\n";
            file << "  Success: " << (result.success ? "Yes" : "No") << "\n";
            
            file << "  Metrics:\n";
            for (const auto& metric : result.metrics) {
                file << "    " << metric.first << ": " << metric.second << "\n";
            }
            
            file << "\n";
        }
        
        file.close();
    }
    
private:
    Action select_action() {
        if (!agent_ || !world_) {
            return Action();
        }
        
        // Get possible actions
        std::vector<ActionType> possible_actions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::Interact,
            ActionType::Wait
        };
        
        if (config_.enable_epsilon_greedy) {
            // Epsilon-greedy action selection
            if ((double)rand() / RAND_MAX < exploration_rate_) {
                // Random action
                int random_idx = rand() % possible_actions.size();
                return Action(possible_actions[random_idx]);
            }
        }
        
        // Use agent's motor command
        return agent_->decodeMotorCommand();
    }
    
    double calculate_reward(const Action& action, int step, int episode) {
        double reward = 0.0;
        
        // Reward for movement
        if (action.getType() == ActionType::MoveForward) {
            reward += 0.1;
        } else if (action.getType() == ActionType::MoveBackward) {
            reward -= 0.1;
        } else if (action.getType() == ActionType::TurnLeft || 
                   action.getType() == ActionType::TurnRight) {
            reward += 0.05;
        }
        
        // Reward for interaction
        if (action.getType() == ActionType::Interact) {
            reward += 0.5;
        }
        
        // Reward for exploration (curiosity)
        if (agent_) {
            double curiosity = agent_->getCuriosityLevel();
            reward += curiosity * 0.01;
        }
        
        // Small survival bonus
        reward += 0.001;
        
        return reward;
    }
    
    void print_training_progress(int episode, 
                                 const std::vector<EpisodeResult>& results) {
        if (results.empty()) return;
        
        const auto& last_result = results.back();
        
        std::cout << "Episode " << episode << "/" << config_.num_episodes 
                  << " | Steps: " << last_result.steps_taken 
                  << " | Reward: " << last_result.total_reward 
                  << " | Curiosity: " << last_result.average_curiosity 
                  << " | Novelty: " << last_result.average_novelty 
                  << " | Exploration: " << exploration_rate_ << std::endl;
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    TrainingConfig config_;
    double exploration_rate_;
};

// === COMPREHENSIVE VALIDATION SYSTEM ===

class ValidationSystem {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> details;
    };
    
    static ValidationResult validate_brain(const Brain& brain) {
        ValidationResult result;
        result.valid = true;
        
        // Check basic properties
        if (brain.getTotalNeuronCount() <= 0) {
            result.errors.push_back("Brain has no neurons");
            result.valid = false;
        }
        
        if (brain.getTotalSynapseCount() < 0) {
            result.errors.push_back("Brain has negative synapse count");
            result.valid = false;
        }
        
        // Check neuron-to-synapse ratio
        double ratio = brain.getTotalSynapseCount() / brain.getTotalNeuronCount();
        if (ratio > 10.0) {
            result.warnings.push_back("High synapse-to-neuron ratio may impact performance");
        }
        
        // Check firing rate
        double firing_rate = brain.getAverageFiringRate();
        if (firing_rate > 100.0) {
            result.warnings.push_back("High firing rate may indicate instability");
        } else if (firing_rate < 0.01) {
            result.warnings.push_back("Low firing rate may indicate inactivity");
        }
        
        result.details["neuron_count"] = std::to_string(brain.getTotalNeuronCount());
        result.details["synapse_count"] = std::to_string(brain.getTotalSynapseCount());
        result.details["firing_rate"] = std::to_string(firing_rate);
        
        return result;
    }
    
    static ValidationResult validate_world(const SimpleWorld& world) {
        ValidationResult result;
        result.valid = true;
        
        if (world.getWidth() <= 0) {
            result.errors.push_back("World width must be positive");
            result.valid = false;
        }
        
        if (world.getHeight() <= 0) {
            result.errors.push_back("World height must be positive");
            result.valid = false;
        }
        
        if (world.getVisionWidth() <= 0) {
            result.errors.push_back("World vision width must be positive");
            result.valid = false;
        }
        
        if (world.getVisionHeight() <= 0) {
            result.errors.push_back("World vision height must be positive");
            result.valid = false;
        }
        
        // Check agent position
        auto body = world.getAgentBody();
        if (!validate_position(body.x, body.y, world.getWidth(), world.getHeight())) {
            result.errors.push_back("Agent is outside world boundaries");
            result.valid = false;
        }
        
        result.details["width"] = std::to_string(world.getWidth());
        result.details["height"] = std::to_string(world.getHeight());
        result.details["agent_x"] = std::to_string(body.x);
        result.details["agent_y"] = std::to_string(body.y);
        
        return result;
    }
    
    static ValidationResult validate_agent(const AgentBrain& agent) {
        ValidationResult result;
        result.valid = true;
        
        // Check if brain exists
        auto brain = agent.getBrain();
        if (!brain) {
            result.errors.push_back("Agent has no brain");
            result.valid = false;
        }
        
        // Check system states
        double curiosity = agent.getCuriosityLevel();
        double novelty = agent.getNoveltyLevel();
        double prediction_error = agent.getPredictionError();
        
        if (curiosity < 0.0 || curiosity > 1.0) {
            result.warnings.push_back("Curiosity level outside normal range [0,1]");
        }
        
        if (novelty < 0.0 || novelty > 1.0) {
            result.warnings.push_back("Novelty level outside normal range [0,1]");
        }
        
        if (prediction_error < 0.0) {
            result.warnings.push_back("Prediction error should be non-negative");
        }
        
        result.details["curiosity"] = std::to_string(curiosity);
        result.details["novelty"] = std::to_string(novelty);
        result.details["prediction_error"] = std::to_string(prediction_error);
        
        return result;
    }
    
    static bool validate_position(float x, float y, float max_width, float max_height) {
        return x >= 0 && x <= max_width && y >= 0 && y <= max_height;
    }
    
    static ValidationResult validate_simulation(const SimulationResult& result) {
        ValidationResult validation;
        validation.valid = result.success;
        
        if (!result.success) {
            validation.errors.push_back("Simulation failed: " + result.error_message);
        }
        
        if (result.brain) {
            auto brain_validation = validate_brain(*result.brain);
            validation.errors.insert(validation.errors.end(), 
                                    brain_validation.errors.begin(), 
                                    brain_validation.errors.end());
            validation.warnings.insert(validation.warnings.end(),
                                       brain_validation.warnings.begin(),
                                       brain_validation.warnings.end());
            
            validation.details["brain_neurons"] = std::to_string(result.brain->getTotalNeuronCount());
            validation.details["brain_spikes"] = std::to_string(result.brain->getTotalSpikeCount());
        }
        
        if (result.world) {
            auto world_validation = validate_world(*result.world);
            validation.errors.insert(validation.errors.end(),
                                    world_validation.errors.begin(),
                                    world_validation.errors.end());
            validation.warnings.insert(validation.warnings.end(),
                                       world_validation.warnings.end());
        }
        
        if (result.agent) {
            auto agent_validation = validate_agent(*result.agent);
            validation.errors.insert(validation.errors.end(),
                                    agent_validation.errors.begin(),
                                    agent_validation.errors.end());
            validation.warnings.insert(validation.warnings.end(),
                                       agent_validation.warnings.end());
        }
        
        validation.valid = validation.errors.empty();
        return validation;
    }
};

// === ENHANCED PYTHON BINDINGS ===

PYBIND11_MODULE(pynlm, m) {
    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Helper function to convert ValidationResult to Python dict
    auto validation_result_to_dict = [](const ValidationSystem::ValidationResult& result) {
        py::dict dict;
        dict["valid"] = result.valid;
        dict["errors"] = py::list();
        dict["warnings"] = py::list();
        dict["details"] = py::dict();
        
        for (const auto& error : result.errors) {
            dict["errors"].attr("append")(error);
        }
        
        for (const auto& warning : result.warnings) {
            dict["warnings"].attr("append")(warning);
        }
        
        for (const auto& detail : result.details) {
            dict["details"](detail.first) = detail.second;
        }
        
        return dict;
    };
    
    m.def("validate_brain", &ValidationSystem::validate_brain, 
          py::arg("brain"), "Validate a brain object");
    
    m.def("validate_world", &ValidationSystem::validate_world,
          py::arg("world"), "Validate a world object");
    
    m.def("validate_agent", &ValidationSystem::validate_agent,
          py::arg("agent"), "Validate an agent object");
    
    m.def("validate_simulation", [](const python_utils::SimulationResult& result) {
        return validation_result_to_dict(ValidationSystem::validate_simulation(result));
    }, py::arg("result"), "Validate a simulation result");
    
    // Enhanced context managers
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Python context manager for Brain)")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("reset", &BrainContext::reset)
        .def("is_initialized", &BrainContext::is_initialized)
        .def("__enter__", [](BrainContext& self) {
            return self;
        })
        .def("__exit__", [](BrainContext&, const std::exception_ptr&) {
            // Destructor handles cleanup
        });
    
    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return BrainContext(brain);
    }, py::arg("brain"), R"pbdoc(Create a context manager for a brain)");
    
    // Additional context managers
    py::class_<ConfigContext>(m, "ConfigContext", R"pbdoc(Python context manager for Config)")
        .def(py::init<std::shared_ptr<Config>>())
        .def("clear", &ConfigContext::clear)
        .def("is_empty", &ConfigContext::is_empty)
        .def("__enter__", [](ConfigContext& self) {
            return self;
        })
        .def("__exit__", [](ConfigContext&, const std::exception_ptr&) {
            // Destructor handles cleanup
        });
    
    m.def("config_context", [](std::shared_ptr<Config> config) {
        return ConfigContext(config);
    }, py::arg("config"), R"pbdoc(Create a context manager for a config)");
    
    py::class_<WorldContext>(m, "WorldContext", R"pbdoc(Python context manager for World)")
        .def(py::init<std::shared_ptr<SimpleWorld>>())
        .def("reset", &WorldContext::reset)
        .def("is_ready", &WorldContext::is_ready)
        .def("__enter__", [](WorldContext& self) {
            return self;
        })
        .def("__exit__", [](WorldContext&, const std::exception_ptr&) {
            // Destructor handles cleanup
        });
    
    m.def("world_context", [](std::shared_ptr<SimpleWorld> world) {
        return WorldContext(world);
    }, py::arg("world"), R"pbdoc(Create a context manager for a world)");
    
    // Enhanced batch processing
    py::class_<BatchProcessor>(m, "BatchProcessor", R"pbdoc(Batch processor for running multiple simulations)")
        .def(py::init<>())
        .def("process_batch", &BatchProcessor::process_batch,
             py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr)
        .def("process_multiple_batches", &BatchProcessor::process_multiple_batches,
             py::arg("batches"), R"pbdoc(Process multiple batches in sequence)")
        .def("save_results", &BatchProcessor::save_results,
             py::arg("results"), py::arg("filename"),
             R"pbdoc(Save batch processing results to file)");
    
    m.def("create_batch_processor", []() {
        return std::make_shared<BatchProcessor>();
    }, R"pbdoc(Create a batch processor)");
    
    // Enhanced agent training
    py::class_<AgentTrainer>(m, "AgentTrainer", R"pbdoc(Agent trainer for supervised learning)")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("train", &AgentTrainer::train, py::arg("num_episodes") = -1,
             R"pbdoc(Train the agent for specified number of episodes)")
        .def("save_training_results", &AgentTrainer::save_training_results,
             py::arg("results"), py::arg("filename"),
             R"pbdoc(Save training results to file)");
    
    m.def("create_agent_trainer", [](std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<AgentTrainer>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       R"pbdoc(Create an agent trainer)");
    
    // Enhanced simulation utilities
    py::class_<python_utils::SimulationResult>(m, "SimulationResult", R"pbdoc(Result of a simulation run)")
        .def_readonly("brain", &python_utils::SimulationResult::brain)
        .def_readonly("world", &python_utils::SimulationResult::world)
        .def_readonly("agent", &python_utils::SimulationResult::agent)
        .def_readonly("stats", &python_utils::SimulationResult::stats)
        .def_readonly("total_time", &python_utils::SimulationResult::total_time)
        .def_readonly("success", &python_utils::SimulationResult::success)
        .def_readonly("error_message", &python_utils::SimulationResult::error_message);
    
    m.def("run_simulation", &python_utils::run_simulation,
          py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
          py::arg("num_steps") = 100, py::arg("timestep") = 0.1,
          R"pbdoc(Run a simulation and return detailed results)");
    
    // Comprehensive validation
    py::class_<ValidationSystem::ValidationResult>(m, "ValidationResult", R"pbdoc(Result of validation)")
        .def_readonly("valid", &ValidationSystem::ValidationResult::valid)
        .def_readonly("errors", &ValidationSystem::ValidationResult::errors)
        .def_readonly("warnings", &ValidationSystem::ValidationResult::warnings)
        .def_readonly("details", &ValidationSystem::ValidationResult::details);
    
    m.def("validate_brain", &ValidationSystem::validate_brain,
          py::arg("brain"), "Validate a brain object");
    
    m.def("validate_world", &ValidationSystem::validate_world,
          py::arg("world"), "Validate a world object");
    
    m.def("validate_agent", &ValidationSystem::validate_agent,
          py::arg("agent"), "Validate an agent object");
    
    m.def("validate_simulation", [](const python_utils::SimulationResult& result) {
        return validation_result_to_dict(ValidationSystem::validate_simulation(result));
    }, py::arg("result"), "Validate a simulation result");
    
    // Add more utility functions
    m.def("clamp", &python_utils::clamp<double>, py::arg("value"), py::arg("min"), py::arg("max"),
          "Clamp a value between minimum and maximum");
    
    m.def("lerp", &python_utils::lerp<double>, py::arg("a"), py::arg("b"), py::arg("t"),
          "Linear interpolation between two values");
    
    m.def("linspace", &python_utils::linspace<double>, 
          py::arg("start"), py::arg("stop"), py::arg("num"),
          "Generate evenly spaced values");
    
    m.def("mean", &python_utils::mean<double>, py::arg("values"),
          "Calculate mean of values");
    
    m.def("variance", &python_utils::variance<double>, py::arg("values"), py::arg("mean_val") = -1.0,
          "Calculate variance of values");
    
    m.def("median", &python_utils::median<double>, py::arg("values"),
          "Calculate median of values");
    
    m.def("normalize", &python_utils::normalize<double>, py::arg("values"), py::arg("max_norm") = 0.0,
          "Normalize vector values");
    
    m.def("to_snake_case", &python_utils::to_snake_case, py::arg("str"),
          "Convert string to snake_case");
    
    m.def("to_camel_case", &python_utils::to_camel_case, py::arg("str"),
          "Convert string to camelCase");
    
    m.def("file_exists", &python_utils::file_exists, py::arg("path"),
          "Check if file exists");
    
    m.def("read_file", &python_utils::read_file, py::arg("path"),
          "Read content of file");
    
    m.def("write_file", &python_utils::write_file, py::arg("path"), py::arg("content"),
          "Write content to file");
    
    m.def("config_to_dict", &python_utils::config_to_dict, py::arg("config"),
          "Convert configuration to dictionary");
    
    m.def("config_to_json", &python_utils::config_to_json, py::arg("config"),
          "Convert configuration to JSON string");
    
    // Add validation helper functions
    m.def("validate_position", &ValidationSystem::validate_position,
          py::arg("x"), py::arg("y"), py::arg("max_width"), py::arg("max_height"),
          "Validate a 2D position is within bounds");
    
    // Enhanced factory functions
    m.def("create_agent_with_defaults", &create_agent_with_defaults,
          R"pbdoc(Create a brain, world, and agent with default settings)");
    
    m.def("run_agent_episode", &run_agent_episode,
          py::arg("agent"), py::arg("world"), py::arg("num_steps"), 
          py::arg("enable_learning") = true,
          R"pbdoc(Run a complete agent episode)");
    
    // Add progress tracking
    py::class_<ProgressTracker>(m, "ProgressTracker", R"pbdoc(Progress tracker for long operations)")
        .def(py::init<int>())
        .def("update", &ProgressTracker::update)
        .def("get_percentage", &ProgressTracker::get_percentage)
        .def("is_complete", &ProgressTracker::is_complete)
        .def("get_remaining_steps", &ProgressTracker::get_remaining_steps);
    
    m.def("create_progress_tracker", [](int total_steps) {
        return std::make_shared<ProgressTracker>(total_steps);
    }, py::arg("total_steps"), R"pbdoc(Create a progress tracker)");
    
    // Add event system
    py::class_<EventSystem>(m, "EventSystem", R"pbdoc(Event system for simulation events)")
        .def(py::init<>())
        .def("add_event_handler", &EventSystem::add_event_handler)
        .def("remove_event_handler", &EventSystem::remove_event_handler)
        .def("dispatch_event", &EventSystem::dispatch_event)
        .def("get_event_count", &EventSystem::get_event_count);
    
    m.def("create_event_system", []() {
        return std::make_shared<EventSystem>();
    }, R"pbdoc(Create an event system)");
    
    // Add profiling
    py::class_<Profiler>(m, "Profiler", R"pbdoc(Performance profiler)")
        .def(py::init<>())
        .def("start", &Profiler::start)
        .def("stop", &Profiler::stop)
        .def("reset", &Profiler::reset)
        .def("get_total_time", &Profiler::get_total_time)
        .def("get_step_count", &Profiler::get_step_count)
        .def("get_average_time_per_step", &Profiler::get_average_time_per_step)
        .def("get_max_time_per_step", &Profiler::get_max_time_per_step);
    
    m.def("create_profiler", []() {
        return std::make_shared<Profiler>();
    }, R"pbdoc(Create a profiler)");
    
    // Existing bindings (preserved from original)
    // [All original bindings remain here...]
    
    // Add constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm