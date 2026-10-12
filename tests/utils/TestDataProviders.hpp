#ifndef TEST_DATAPROVIDERS_HPP
#define TEST_DATAPROVIDERS_HPP

#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>

/**
 * Test data generators for NLM testing
 */

namespace TestDataProviders {
    
    // Random number generation utilities
    class RandomGenerator {
    private:
        std::mt19937 rng_;
        std::uniform_real_distribution<double> uniform_dist_;
        std::uniform_int_distribution<int> int_dist_;
        std::normal_distribution<double> normal_dist_;
        
    public:
        RandomGenerator(unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count())
            : rng_(seed) {}
        
        double uniform(double min = 0.0, double max = 1.0) {
            std::uniform_real_distribution<double> dist(min, max);
            return dist(rng_);
        }
        
        int integer(int min = 0, int max = 100) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng_);
        }
        
        double gaussian(double mean = 0.0, double stddev = 1.0) {
            std::normal_distribution<double> dist(mean, stddev);
            return dist(rng_);
        }
        
        double exponential(double lambda = 1.0) {
            std::exponential_distribution<double> dist(lambda);
            return dist(rng_);
        }
        
        double gamma(double shape, double scale) {
            std::gamma_distribution<double> dist(shape, scale);
            return dist(rng_);
        }
        
        double bernoulli(double p) {
            std::bernoulli_distribution dist(p);
            return dist(rng_) ? 1.0 : 0.0;
        }
        
        double beta(double alpha, double beta) {
            std::beta_distribution<double> dist(alpha, beta);
            return dist(rng_);
        }
        
        // Fisher-Yates shuffle for shuffling vectors
        template<typename T>
        void shuffle(std::vector<T>& vec) {
            std::shuffle(vec.begin(), vec.end(), rng_);
        }
        
        // Permutation generation
        template<typename T>
        std::vector<size_t> permute(size_t n) {
            std::vector<size_t> indices(n);
            std::iota(indices.begin(), indices.end(), 0);
            std::shuffle(indices.begin(), indices.end(), rng_);
            return indices;
        }
    };
    
    // Neural simulation data generators
    struct NeuralData {
        std::vector<double> spikes;
        std::vector<std::vector<double>> voltages;
        std::vector<std::vector<double>> currents;
        std::vector<std::vector<double>> weights;
        std::vector<std::vector<double>> biases;
        std::vector<std::vector<double>> connections;
    };
    
    // Test data generators for different components
    class NeuronDataGenerator {
    public:
        static NeuralData generateRandomNeurons(int num_neurons, int time_steps, double dt = 0.001) {
            NeuralData data;
            RandomGenerator rng;
            
            data.spikes.resize(num_neurons, 0.0);
            data.voltages.resize(num_neurons, std::vector<double>(time_steps, 0.0));
            data.currents.resize(num_neurons, std::vector<double>(time_steps, 0.0));
            data.weights.resize(num_neurons, std::vector<double>(num_neurons, 0.0));
            data.biases.resize(num_neurons, std::vector<double>(num_neurons, 0.0));
            
            // Generate random neuron parameters
            for (int i = 0; i < num_neurons; ++i) {
                double v_rest = rng.uniform(-70.0, -60.0);
                double v_threshold = rng.uniform(-55.0, -45.0);
                double v_reset = v_rest;
                double c_m = rng.uniform(0.1, 1.0);
                double g_l = rng.uniform(0.01, 0.1);
                double tau_m = rng.uniform(5.0, 20.0);
                double refractory_period = rng.uniform(1.0, 5.0);
                double spike_adaptation = rng.uniform(0.01, 0.1);
                double max_spikes_per_step = rng.integer(1, 10);
                
                // Generate spike times
                for (int t = 0; t < time_steps; ++t) {
                    double time = t * dt;
                    double probability = 0.01 + 0.1 * std::sin(time * 10.0);
                    if (rng.uniform(0.0, 1.0) < probability) {
                        data.spikes[i] = time;
                        break; // Only one spike per neuron for this test
                    }
                }
                
                // Generate voltages
                for (int t = 0; t < time_steps; ++t) {
                    double time = t * dt;
                    double voltage = v_rest + rng.uniform(-10.0, 10.0) * std::exp(-time / tau_m);
                    data.voltages[i][t] = voltage;
                    
                    // Add spikes
                    if (std::abs(data.spikes[i] - time) < dt) {
                        data.voltages[i][t] = v_reset;
                    }
                }
                
                // Generate random inputs
                for (int t = 0; t < time_steps; ++t) {
                    data.currents[i][t] = rng.uniform(-10.0, 10.0);
                }
            }
            
            // Generate random connectivity
            for (int i = 0; i < num_neurons; ++i) {
                for (int j = 0; j < num_neurons; ++j) {
                    if (i != j) {
                        double weight = rng.uniform(-1.0, 1.0);
                        double bias = rng.uniform(-0.5, 0.5);
                        data.weights[i][j] = weight;
                        data.biases[i][j] = bias;
                        data.connections[i][j] = (rng.uniform(0.0, 1.0) < 0.2) ? 1.0 : 0.0; // 20% connectivity
                    }
                }
            }
            
            return data;
        }
    };
    
    class MemoryDataGenerator {
    public:
        static NeuralData generateWorkingMemoryData(int num_items, int pattern_size, int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate random patterns for items
            for (int i = 0; i < num_items; ++i) {
                std::vector<double> pattern;
                for (int j = 0; j < pattern_size; ++j) {
                    pattern.push_back(rng.uniform(0.0, 1.0));
                }
                
                // Encode pattern with item ID
                data.voltages.push_back(pattern);
                
                // Generate retrieval strength
                double retrieval_strength = rng.uniform(0.1, 1.0);
                data.spikes.push_back(retrieval_strength);
            }
            
            // Generate time series data
            for (int t = 0; t < time_steps; ++t) {
                double decay = std::exp(-t * 0.01);
                std::vector<double> time_series(pattern_size, 0.0);
                
                for (int i = 0; i < num_items; ++i) {
                    for (int j = 0; j < pattern_size; ++j) {
                        time_series[j] += data.voltages[i][j] * decay * (i + 1) / num_items;
                    }
                }
                
                data.currents.push_back(time_series);
            }
            
            return data;
        }
        
        static NeuralData generateEpisodicMemoryData(int num_episodes, int pattern_size) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate episodes with unique IDs
            for (int i = 0; i < num_episodes; ++i) {
                // Episode structure
                std::vector<double> episode_pattern;
                for (int j = 0; j < pattern_size; ++j) {
                    episode_pattern.push_back(rng.uniform(0.0, 1.0));
                }
                
                data.voltages.push_back(episode_pattern);
                data.spikes.push_back(static_cast<double>(i)); // Episode ID as spike strength
                data.biases.push_back(episode_pattern); // Use pattern as bias
            }
            
            // Generate replay sequences
            for (int i = 0; i < num_episodes; ++i) {
                // Replay episode i with some variations
                int replay_target = (i + rng.integer(1, 5)) % num_episodes;
                for (int j = 0; j < pattern_size; ++j) {
                    double variation = rng.uniform(-0.1, 0.1);
                    data.currents.push_back({data.voltages[replay_target][j] + variation});
                }
            }
            
            return data;
        }
    };
    
    class NeuromodulationDataGenerator {
    public:
        static NeuralData generateRewardData(int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate reward signal over time
            for (int t = 0; t < time_steps; ++t) {
                double time = t * 0.001;
                double reward = rng.uniform(0.0, 1.0) * std::exp(-time / 5.0);
                
                data.voltages.push_back({reward});
                data.spikes.push_back(reward);
                data.currents.push_back({reward * 0.1});
            }
            
            return data;
        }
        
        static NeuralData generatePredictionErrorData(int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate prediction error signal
            for (int t = 0; t < time_steps; ++t) {
                double time = t * 0.001;
                double prediction = rng.uniform(0.0, 1.0);
                double outcome = rng.uniform(0.0, 1.0);
                double error = std::abs(outcome - prediction);
                
                data.voltages.push_back({error});
                data.spikes.push_back(error);
                data.currents.push_back({error * 0.1});
            }
            
            return data;
        }
        
        static NeuralData generateCuriosityData(int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate curiosity signal
            for (int t = 0; t < time_steps; ++t) {
                double time = t * 0.001;
                double novelty = rng.uniform(0.0, 1.0) * std::exp(-time / 10.0);
                double uncertainty = rng.uniform(0.0, 1.0);
                double curiosity = novelty * uncertainty;
                
                data.voltages.push_back({curiosity});
                data.spikes.push_back(curiosity);
                data.currents.push_back({curiosity * 0.1});
            }
            
            return data;
        }
        
        static NeuralData generateNoveltyData(int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate novelty signal
            for (int t = 0; t < time_steps; ++t) {
                double time = t * 0.001;
                double stimulus = rng.uniform(0.0, 1.0);
                double history_mean = t > 10 ? rng.uniform(0.0, 1.0) : 0.0;
                double novelty = std::abs(stimulus - history_mean);
                
                data.voltages.push_back({novelty});
                data.spikes.push_back(novelty);
                data.currents.push_back({novelty * 0.1});
            }
            
            return data;
        }
    };
    
    class PlasticityDataGenerator {
    public:
        static NeuralData generateSTDPData(int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate spike trains for STDP
            for (int t = 0; t < time_steps; ++t) {
                double time = t * 0.001;
                
                // Pre-synaptic spike
                double pre_prob = 0.1 * (1.0 + std::sin(time * 5.0)) / 2.0;
                if (rng.uniform(0.0, 1.0) < pre_prob) {
                    data.spikes.push_back(1.0); // Pre-synaptic spike
                } else {
                    data.spikes.push_back(0.0);
                }
                
                // Post-synaptic spike
                double post_prob = 0.1 * (1.0 + std::cos(time * 5.0)) / 2.0;
                if (rng.uniform(0.0, 1.0) < post_prob) {
                    data.voltages.push_back({1.0}); // Post-synaptic spike
                } else {
                    data.voltages.push_back({0.0});
                }
                
                // Weight update based on spike timing
                data.currents.push_back({rng.uniform(-0.1, 0.1)}); // Weight change
            }
            
            return data;
        }
        
        static NeuralData generateHebbianData(int time_steps) {
            NeuralData data;
            RandomGenerator rng;
            
            // Generate correlated activity patterns
            std::vector<double> pattern1(time_steps, 0.0);
            std::vector<double> pattern2(time_steps, 0.0);
            
            for (int t = 0; t < time_steps; ++t) {
                double time = t * 0.001;
                double amplitude = std::sin(time * 2.0);
                
                pattern1[t] = amplitude;
                pattern2[t] = amplitude * 0.8 + rng.uniform(-0.1, 0.1); // Correlated with noise
            }
            
            data.spikes.push_back(pattern1);
            data.voltages.push_back(pattern2);
            
            // Generate Hebbian weight updates
            for (int t = 0; t < time_steps; ++t) {
                double hebbian_update = pattern1[t] * pattern2[t];
                data.currents.push_back({hebbian_update});
            }
            
            return data;
        }
    };
    
    class BenchmarkDataGenerator {
    public:
        static std::vector<std::vector<double>> generateMatrix(int rows, int cols, double min_val = 0.0, double max_val = 1.0) {
            std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
            RandomGenerator rng;
            
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    matrix[i][j] = rng.uniform(min_val, max_val);
                }
            }
            
            return matrix;
        }
        
        static std::vector<double> generateVector(int size, double min_val = 0.0, double max_val = 1.0) {
            std::vector<double> vector(size);
            RandomGenerator rng;
            
            for (int i = 0; i < size; ++i) {
                vector[i] = rng.uniform(min_val, max_val);
            }
            
            return vector;
        }
        
        static std::vector<double> generateGaussianVector(int size, double mean = 0.0, double stddev = 1.0) {
            std::vector<double> vector(size);
            RandomGenerator rng;
            
            for (int i = 0; i < size; ++i) {
                vector[i] = rng.gaussian(mean, stddev);
            }
            
            return vector;
        }
        
        static std::vector<std::vector<double>> generateSparseMatrix(int rows, int cols, double sparsity = 0.1, double density = 1.0) {
            std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols, 0.0));
            RandomGenerator rng;
            
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    if (rng.uniform(0.0, 1.0) < sparsity) {
                        matrix[i][j] = rng.uniform(-density, density);
                    }
                }
            }
            
            return matrix;
        }
        
        static std::vector<std::vector<double>> generateSymmetricMatrix(int size, double min_val = 0.0, double max_val = 1.0) {
            std::vector<std::vector<double>> matrix(size, std::vector<double>(size));
            RandomGenerator rng;
            
            for (int i = 0; i < size; ++i) {
                for (int j = i; j < size; ++j) {
                    double value = rng.uniform(min_val, max_val);
                    matrix[i][j] = value;
                    matrix[j][i] = value; // Symmetric
                }
            }
            
            return matrix;
        }
        
        static std::vector<std::vector<double>> generateToeplitzMatrix(int size, double min_val = 0.0, double max_val = 1.0) {
            std::vector<std::vector<double>> matrix(size, std::vector<double>(size));
            RandomGenerator rng;
            
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    matrix[i][j] = rng.uniform(min_val, max_val) * std::pow(0.9, std::abs(i - j));
                }
            }
            
            return matrix;
        }
        
        static std::vector<std::vector<double>> generateCirculantMatrix(int size, double min_val = 0.0, double max_val = 1.0) {
            std::vector<std::vector<double>> matrix(size, std::vector<double>(size));
            RandomGenerator rng;
            
            // Generate first row
            std::vector<double> first_row(size);
            for (int j = 0; j < size; ++j) {
                first_row[j] = rng.uniform(min_val, max_val);
            }
            
            // Generate matrix from first row
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    matrix[i][j] = first_row[(j - i + size) % size];
                }
            }
            
            return matrix;
        }
        
        static std::vector<std::vector<double>> generateRandomWalkMatrix(int size, double min_val = 0.0, double max_val = 1.0) {
            std::vector<std::vector<double>> matrix(size, std::vector<double>(size));
            RandomGenerator rng;
            
            // Generate random walk coefficients
            std::vector<double> walk_coeffs(size);
            for (int i = 0; i < size; ++i) {
                walk_coeffs[i] = rng.uniform(min_val, max_val);
            }
            
            // Generate matrix
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    if (i == j) {
                        matrix[i][j] = 1.0 - rng.uniform(0.0, 0.2); // Diagonal
                    } else {
                        matrix[i][j] = rng.uniform(0.0, 0.1) * walk_coeffs[j];
                    }
                }
            }
            
            return matrix;
        }
    };
    
    // Performance benchmark data generators
    class PerformanceBenchmarkGenerator {
    public:
        static std::vector<std::vector<double>> generateBenchmarkInput(int size, double sparsity = 0.1) {
            RandomGenerator rng;
            std::vector<std::vector<double>> matrix(size, std::vector<double>(size, 0.0));
            
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    if (rng.uniform(0.0, 1.0) < sparsity) {
                        matrix[i][j] = rng.uniform(-1.0, 1.0);
                    }
                }
            }
            
            return matrix;
        }
        
        static std::vector<double> generateBenchmarkVector(int size, double mean = 0.0, double stddev = 1.0) {
            RandomGenerator rng;
            std::vector<double> vector(size);
            
            for (int i = 0; i < size; ++i) {
                vector[i] = rng.gaussian(mean, stddev);
            }
            
            return vector;
        }
        
        static std::vector<std::vector<double>> generateBenchmarkMatrix(int size, double condition = 1.0) {
            RandomGenerator rng;
            std::vector<std::vector<double>> matrix(size, std::vector<double>(size));
            
            // Generate well-conditioned matrix
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    double value = rng.uniform(0.0, 1.0);
                    if (i == j) {
                        value = value * condition + (1.0 - condition) * 1.0; // Diagonal dominance
                    }
                    matrix[i][j] = value;
                }
            }
            
            return matrix;
        }
    };
    
    // Utility functions for data manipulation
    namespace Utils {
        template<typename T>
        static std::vector<T> toVector(const std::list<T>& list) {
            return std::vector<T>(list.begin(), list.end());
        }
        
        template<typename T>
        static std::vector<T> toVector(const std::set<T>& set) {
            return std::vector<T>(set.begin(), set.end());
        }
        
        template<typename T>
        static std::vector<T> toVector(const std::unordered_set<T>& set) {
            return std::vector<T>(set.begin(), set.end());
        }
        
        template<typename T>
        static std::vector<T> concatenate(const std::vector<T>& a, const std::vector<T>& b) {
            std::vector<T> result;
            result.reserve(a.size() + b.size());
            result.insert(result.end(), a.begin(), a.end());
            result.insert(result.end(), b.begin(), b.end());
            return result;
        }
        
        template<typename T>
        static std::vector<T> repeat(const T& value, size_t count) {
            return std::vector<T>(count, value);
        }
        
        template<typename T>
        static std::vector<T> repeat(const std::vector<T>& vec, size_t count) {
            std::vector<T> result;
            result.reserve(vec.size() * count);
            for (size_t i = 0; i < count; ++i) {
                result.insert(result.end(), vec.begin(), vec.end());
            }
            return result;
        }
        
        template<typename T>
        static bool contains(const std::vector<T>& container, const T& element) {
            return std::find(container.begin(), container.end(), element) != container.end();
        }
        
        template<typename T>
        static size_t count(const std::vector<T>& container, const T& element) {
            return std::count(container.begin(), container.end(), element);
        }
        
        template<typename T>
        static void remove(std::vector<T>& container, const T& element) {
            container.erase(std::remove(container.begin(), container.end(), element), container.end());
        }
        
        template<typename T>
        static void removeAll(std::vector<T>& container, const T& element) {
            container.erase(std::remove(container.begin(), container.end(), element), container.end());
        }
        
        template<typename T>
        static void shuffle(std::vector<T>& vec) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(vec.begin(), vec.end(), gen);
        }
        
        template<typename T>
        static void sort(std::vector<T>& vec) {
            std::sort(vec.begin(), vec.end());
        }
        
        template<typename T>
        static void sortDescending(std::vector<T>& vec) {
            std::sort(vec.begin(), vec.end(), std::greater<T>());
        }
        
        template<typename T>
        static void unique(std::vector<T>& vec) {
            std::sort(vec.begin(), vec.end());
            vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
        }
        
        template<typename T>
        static std::vector<T> getUnique(const std::vector<T>& vec) {
            std::vector<T> result = vec;
            unique(result);
            return result;
        }
        
        template<typename T>
        static T sum(const std::vector<T>& vec) {
            T result = T();
            for (const auto& element : vec) {
                result += element;
            }
            return result;
        }
        
        template<typename T>
        static T product(const std::vector<T>& vec) {
            T result = T(1);
            for (const auto& element : vec) {
                result *= element;
            }
            return result;
        }
        
        template<typename T>
        static double mean(const std::vector<T>& vec) {
            if (vec.empty()) return 0.0;
            return static_cast<double>(sum(vec)) / vec.size();
        }
        
        template<typename T>
        static double variance(const std::vector<T>& vec, double mean = -1.0) {
            if (vec.empty()) return 0.0;
            if (mean < 0.0) mean = mean(vec);
            
            double sum_sq = 0.0;
            for (const auto& element : vec) {
                sum_sq += static_cast<double>(element) - mean;
            }
            return sum_sq / vec.size();
        }
        
        template<typename T>
        static double standardDeviation(const std::vector<T>& vec, double variance = -1.0) {
            if (variance < 0.0) variance = variance(vec);
            return std::sqrt(variance);
        }
        
        template<typename T>
        static T minimum(const std::vector<T>& vec) {
            if (vec.empty()) throw std::runtime_error("Cannot find minimum of empty vector");
            return *std::min_element(vec.begin(), vec.end());
        }
        
        template<typename T>
        static T maximum(const std::vector<T>& vec) {
            if (vec.empty()) throw std::runtime_error("Cannot find maximum of empty vector");
            return *std::max_element(vec.begin(), vec.end());
        }
        
        template<typename T>
        static T median(std::vector<T> vec) {
            if (vec.empty()) throw std::runtime_error("Cannot find median of empty vector");
            std::sort(vec.begin(), vec.end());
            size_t n = vec.size();
            if (n % 2 == 0) {
                return (vec[n/2 - 1] + vec[n/2]) / 2;
            } else {
                return vec[n/2];
            }
        }
        
        template<typename T>
        static T percentile(std::vector<T> vec, double percentile) {
            if (vec.empty()) throw std::runtime_error("Cannot find percentile of empty vector");
            if (percentile < 0.0 || percentile > 100.0) throw std::invalid_argument("Percentile must be between 0 and 100");
            
            std::sort(vec.begin(), vec.end());
            size_t n = vec.size();
            double index = (percentile / 100.0) * (n - 1);
            size_t lower = static_cast<size_t>(std::floor(index));
            size_t upper = static_cast<size_t>(std::ceil(index));
            
            if (lower == upper) {
                return vec[lower];
            }
            
            double weight = index - lower;
            return static_cast<T>((1.0 - weight) * vec[lower] + weight * vec[upper]);
        }
        
        template<typename T>
        static std::vector<T> slice(const std::vector<T>& vec, size_t start, size_t end) {
            if (start > vec.size() || end > vec.size() || start > end) {
                throw std::out_of_range("Invalid slice indices");
            }
            return std::vector<T>(vec.begin() + start, vec.begin() + end);
        }
        
        template<typename T>
        static std::vector<T> head(const std::vector<T>& vec, size_t n) {
            if (n > vec.size()) {
                throw std::invalid_argument("n larger than vector size");
            }
            return std::vector<T>(vec.begin(), vec.begin() + n);
        }
        
        template<typename T>
        static std::vector<T> tail(const std::vector<T>& vec, size_t n) {
            if (n > vec.size()) {
                throw std::invalid_argument("n larger than vector size");
            }
            return std::vector<T>(vec.end() - n, vec.end());
        }
        
        template<typename T>
        static std::vector<T> drop(const std::vector<T>& vec, size_t n) {
            if (n > vec.size()) {
                throw std::invalid_argument("n larger than vector size");
            }
            return std::vector<T>(vec.begin() + n, vec.end());
        }
        
        template<typename T>
        static std::vector<T> take(const std::vector<T>& vec, size_t n) {
            return head(vec, n);
        }
        
        template<typename T>
        static std::vector<T> filter(const std::vector<T>& vec, std::function<bool(const T&)> predicate) {
            std::vector<T> result;
            for (const auto& element : vec) {
                if (predicate(element)) {
                    result.push_back(element);
                }
            }
            return result;
        }
        
        template<typename T>
        static std::vector<T> map(const std::vector<T>& vec, std::function<T(const T&)> func) {
            std::vector<T> result;
            result.reserve(vec.size());
            for (const auto& element : vec) {
                result.push_back(func(element));
            }
            return result;
        }
        
        template<typename T>
        static std::vector<T> flatMap(const std::vector<std::vector<T>>& vec, std::function<T(const T&)> func) {
            std::vector<T> result;
            for (const auto& inner_vec : vec) {
                for (const auto& element : inner_vec) {
                    result.push_back(func(element));
                }
            }
            return result;
        }
        
        template<typename T>
        static bool all(const std::vector<T>& vec, std::function<bool(const T&)> predicate) {
            return std::all_of(vec.begin(), vec.end(), predicate);
        }
        
        template<typename T>
        static bool any(const std::vector<T>& vec, std::function<bool(const T&)> predicate) {
            return std::any_of(vec.begin(), vec.end(), predicate);
        }
        
        template<typename T>
        static bool none(const std::vector<T>& vec, std::function<bool(const T&)> predicate) {
            return std::none_of(vec.begin(), vec.end(), predicate);
        }
        
        template<typename T>
        static size_t countIf(const std::vector<T>& vec, std::function<bool(const T&)> predicate) {
            return std::count_if(vec.begin(), vec.end(), predicate);
        }
        
        template<typename T>
        static size_t findFirst(const std::vector<T>& vec, const T& element) {
            auto it = std::find(vec.begin(), vec.end(), element);
            return it != vec.end() ? std::distance(vec.begin(), it) : static_cast<size_t>(-1);
        }
        
        template<typename T>
        static std::vector<size_t> findAll(const std::vector<T>& vec, const T& element) {
            std::vector<size_t> indices;
            for (size_t i = 0; i < vec.size(); ++i) {
                if (vec[i] == element) {
                    indices.push_back(i);
                }
            }
            return indices;
        }
        
        template<typename T>
        static void reverse(std::vector<T>& vec) {
            std::reverse(vec.begin(), vec.end());
        }
        
        template<typename T>
        static std::vector<T> reversed(const std::vector<T>& vec) {
            std::vector<T> result = vec;
            reverse(result);
            return result;
        }
        
        template<typename T>
        static void rotate(std::vector<T>& vec, size_t positions) {
            std::rotate(vec.begin(), vec.begin() + positions, vec.end());
        }
        
        template<typename T>
        static void shuffle(std::vector<T>& vec) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(vec.begin(), vec.end(), gen);
        }
        
        template<typename T>
        static bool isSorted(const std::vector<T>& vec, bool ascending = true) {
            if (ascending) {
                return std::is_sorted(vec.begin(), vec.end());
            } else {
                return std::is_sorted(vec.begin(), vec.end(), std::greater<T>());
            }
        }
        
        template<typename T>
        static bool isUnique(const std::vector<T>& vec) {
            return std::is_sorted(vec.begin(), vec.end()) && 
                   std::unique(vec.begin(), vec.end()) == vec.end();
        }
        
        template<typename T>
        static void removeDuplicates(std::vector<T>& vec) {
            std::sort(vec.begin(), vec.end());
            vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
        }
        
        template<typename T>
        static std::vector<T> removeDuplicates(const std::vector<T>& vec) {
            std::vector<T> result = vec;
            removeDuplicates(result);
            return result;
        }
        
        template<typename T>
        static void removeAll(std::vector<T>& vec, const T& element) {
            vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
        }
        
        template<typename T>
        static std::vector<T> removeAll(const std::vector<T>& vec, const T& element) {
            std::vector<T> result;
            for (const auto& e : vec) {
                if (e != element) {
                    result.push_back(e);
                }
            }
            return result;
        }
        
        template<typename T>
        static void insertSorted(std::vector<T>& vec, const T& element) {
            auto it = std::lower_bound(vec.begin(), vec.end(), element);
            vec.insert(it, element);
        }
        
        template<typename T>
        static size_t lowerBound(const std::vector<T>& vec, const T& value) {
            return std::lower_bound(vec.begin(), vec.end(), value) - vec.begin();
        }
        
        template<typename T>
        static size_t upperBound(const std::vector<T>& vec, const T& value) {
            return std::upper_bound(vec.begin(), vec.end(), value) - vec.begin();
        }
        
        template<typename T>
        static size_t binarySearch(const std::vector<T>& vec, const T& value) {
            auto it = std::binary_search(vec.begin(), vec.end(), value);
            return it ? std::distance(vec.begin(), it) : static_cast<size_t>(-1);
        }
        
        template<typename T>
        static bool binarySearch(const std::vector<T>& vec, const T& value, size_t& index) {
            auto it = std::lower_bound(vec.begin(), vec.end(), value);
            if (it != vec.end() && !(value < *it)) {
                index = std::distance(vec.begin(), it);
                return true;
            }
            return false;
        }
        
        template<typename T>
        static std::vector<T> merge(const std::vector<T>& a, const std::vector<T>& b) {
            std::vector<T> result;
            result.reserve(a.size() + b.size());
            std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
            return result;
        }
        
        template<typename T>
        static std::vector<T> mergeSorted(const std::vector<T>& a, const std::vector<T>& b) {
            std::vector<T> result;
            result.reserve(a.size() + b.size());
            std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
            return result;
        }
        
        template<typename T>
        static std::vector<T> intersect(const std::vector<T>& a, const std::vector<T>& b) {
            std::vector<T> result;
            std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
            return result;
        }
        
        template<typename T>
        static std::vector<T> difference(const std::vector<T>& a, const std::vector<T>& b) {
            std::vector<T> result;
            std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
            return result;
        }
        
        template<typename T>
        static std::vector<T> symmetricDifference(const std::vector<T>& a, const std::vector<T>& b) {
            std::vector<T> result;
            std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
            return result;
        }
        
        template<typename T>
        static size_t editDistance(const std::string& a, const std::string& b) {
            // Levenshtein distance
            size_t m = a.size();
            size_t n = b.size();
            std::vector<size_t> previous(n + 1, 0);
            std::vector<size_t> current(n + 1, 0);
            
            for (size_t i = 0; i <= m; ++i) {
                current[0] = i;
                for (size_t j = 0; j <= n; ++j) {
                    size_t cost = (i > 0 && j > 0 && a[i-1] != b[j-1]) ? 1 : 0;
                    current[j] = std::min({current[j-1] + 1, previous[j] + 1, previous[j-1] + cost});
                }
                previous = current;
            }
            
            return previous[n];
        }
    };
}

#endif // TEST_DATAPROVIDERS_HPP