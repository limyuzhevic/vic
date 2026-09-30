#pragma once

#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <deque>
#include <stack>
#include <queue>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <random>
#include <thread>
#include <future>
#include <type_traits>
#include <concepts>

namespace nlm {
namespace Core {
namespace Utilities {

/**
 * Container utility functions for neural network simulations
 */
class ContainerUtils {
public:
    // Vector utilities
    template<typename T>
    static std::vector<T> toVector(const std::list<T>& list);

    template<typename T>
    static std::vector<T> toVector(const std::set<T>& set);

    template<typename K, typename V>
    static std::vector<std::pair<K, V>> toVector(const std::map<K, V>& map);

    template<typename T>
    static std::vector<T> flatten(const std::vector<std::vector<T>>& nested);

    template<typename T>
    static std::vector<T> shuffle(std::vector<T> vec);

    template<typename T>
    static std::vector<T> shuffle(std::vector<T> vec, std::mt19937_64& rng);

    template<typename T>
    static std::vector<T> shuffle(std::vector<T> vec, uint64_t seed);

    template<typename T>
    static std::vector<T> reverse(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> sort(const std::vector<T>& vec, bool ascending = true);

    template<typename T>
    static std::vector<T> unique(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> removeDuplicates(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> filter(const std::vector<T>& vec, 
                                std::function<bool(const T&)> predicate);

    template<typename T>
    static std::vector<T> map(const std::vector<T>& vec, 
                             std::function<T(const T&)> func);

    template<typename T>
    static std::vector<T> transform(const std::vector<T>& vec, 
                                    std::function<T(const T&)> func);

    template<typename T>
    static std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b);

    template<typename T>
    static std::vector<T> slice(const std::vector<T>& vec, size_t start, size_t end = -1);

    template<typename T>
    static std::vector<T> subvector(const std::vector<T>& vec, size_t start, size_t length);

    template<typename T>
    static T first(const std::vector<T>& vec);

    template<typename T>
    static T last(const std::vector<T>& vec);

    template<typename T>
    static size_t find(const std::vector<T>& vec, const T& value);

    template<typename T>
    static bool contains(const std::vector<T>& vec, const T& value);

    template<typename T>
    static size_t count(const std::vector<T>& vec, const T& value);

    template<typename T>
    static size_t countIf(const std::vector<T>& vec, std::function<bool(const T&)> predicate);

    template<typename T>
    static std::vector<T> copy(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> move(const std::vector<T>& vec);

    template<typename T>
    static void resizeVector(std::vector<T>& vec, size_t newSize, T defaultValue = T{});

    template<typename T>
    static void reserveVector(std::vector<T>& vec, size_t capacity);

    template<typename T>
    static void shrinkToFit(std::vector<T>& vec);

    template<typename T>
    static void clearVector(std::vector<T>& vec);

    template<typename T>
    static bool isEmpty(const std::vector<T>& vec);

    template<typename T>
    static size_t size(const std::vector<T>& vec);

    // Matrix operations
    template<typename T>
    static std::vector<std::vector<T>> matrixMultiply(const std::vector<std::vector<T>>& a, 
                                                     const std::vector<std::vector<T>>& b);

    template<typename T>
    static std::vector<std::vector<T>> matrixTranspose(const std::vector<std::vector<T>>& matrix);

    template<typename T>
    static std::vector<std::vector<T>> matrixAdd(const std::vector<std::vector<T>>& a, 
                                                const std::vector<std::vector<T>>& b);

    template<typename T>
    static std::vector<std::vector<T>> matrixSubtract(const std::vector<std::vector<T>>& a, 
                                                     const std::vector<std::vector<T>>& b);

    template<typename T>
    static std::vector<std::vector<T>> matrixScalarMultiply(const std::vector<std::vector<T>>& matrix, 
                                                           T scalar);

    template<typename T>
    static std::vector<std::vector<T>> identityMatrix(size_t size);

    template<typename T>
    static std::vector<std::vector<T>> randomMatrix(size_t rows, size_t cols, 
                                                    T min = T{0}, T max = T{1});

    // Set utilities
    template<typename T>
    static std::set<T> toSet(const std::vector<T>& vec);

    template<typename T>
    static std::set<T> unionSet(const std::set<T>& a, const std::set<T>& b);

    template<typename T>
    static std::set<T> intersectionSet(const std::set<T>& a, const std::set<T>& b);

    template<typename T>
    static std::set<T> differenceSet(const std::set<T>& a, const std::set<T>& b);

    template<typename T>
    static std::set<T> symmetricDifferenceSet(const std::set<T>& a, const std::set<T>& b);

    template<typename T>
    static bool containsAll(const std::set<T>& set, const std::vector<T>& values);

    template<typename T>
    static bool containsAny(const std::set<T>& set, const std::vector<T>& values);

    // Map utilities
    template<typename K, typename V>
    static std::map<K, V> toMap(const std::vector<std::pair<K, V>>& pairs);

    template<typename K, typename V>
    static std::map<K, V> filterMap(const std::map<K, V>& map, 
                                   std::function<bool(const V&)> predicate);

    template<typename K, typename V>
    static std::map<K, V> mapValues(const std::map<K, V>& map, 
                                    std::function<V(const V&)> func);

    template<typename K, typename V>
    static std::map<K, V> invertMap(const std::map<K, V>& map);

    // Queue and stack utilities
    template<typename T>
    static std::queue<T> toQueue(const std::vector<T>& vec);

    template<typename T>
    static std::queue<T> toStack(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> toVector(const std::queue<T>& queue);

    template<typename T>
    static std::vector<T> toVector(const std::stack<T>& stack);

    // Array utilities
    template<typename T, size_t N>
    static std::vector<T> toVector(const std::array<T, N>& arr);

    template<typename T, size_t N>
    static std::array<T, N> toArray(const std::vector<T>& vec);

    // Container comparisons
    template<typename T, typename U>
    static bool vectorsEqual(const std::vector<T>& a, const std::vector<U>& b);

    template<typename K, typename V>
    static bool mapsEqual(const std::map<K, V>& a, const std::map<K, V>& b);

    template<typename T>
    static bool setsEqual(const std::set<T>& a, const std::set<T>& b);

    // Container copying
    template<typename T>
    static std::vector<T> deepCopy(const std::vector<T>& vec);

    template<typename K, typename V>
    static std::map<K, V> deepCopy(const std::map<K, V>& map);

    template<typename T>
    static std::set<T> deepCopy(const std::set<T>& set);

    // Container merging
    template<typename T>
    static std::vector<T> mergeVectors(const std::vector<T>& a, const std::vector<T>& b);

    template<typename T>
    static std::vector<T> mergeSorted(const std::vector<T>& a, const std::vector<T>& b);

    // Performance optimized container operations
    template<typename T>
    static std::vector<T> efficientCopy(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> efficientConcat(const std::vector<T>& a, const std::vector<T>& b);

    // Neural network specific container operations
    template<typename T>
    static std::vector<T> createNeuralLayer(size_t neurons, T defaultValue = T{0});

    template<typename T>
    static std::vector<std::vector<T>> createNeuralNetwork(size_t layers, 
                                                             const std::vector<size_t>& neuronsPerLayer);

    template<typename T>
    static std::vector<std::vector<T>> createWeightMatrix(size_t inputSize, size_t outputSize);

    // Random container generation
    template<typename T>
    static std::vector<T> randomVector(size_t size, T min = T{0}, T max = T{1});

    template<typename T>
    static std::vector<std::vector<T>> randomMatrix(size_t rows, size_t cols, 
                                                    T min = T{0}, T max = T{1});

    // Container statistics
    template<typename T>
    static T sum(const std::vector<T>& values);

    template<typename T>
    static T product(const std::vector<T>& values);

    template<typename T>
    static double mean(const std::vector<T>& values);

    template<typename T>
    static T median(std::vector<T> values);

    template<typename T>
    static T mode(const std::vector<T>& values);

    // Container safety utilities
    template<typename Container, typename... Args>
    static bool isSafeToAccess(const Container& container, Args... indices);

    template<typename Container>
    static bool isWithinBounds(const Container& container, size_t index);

    template<typename Container>
    static bool isNotEmpty(const Container& container);

    template<typename Container>
    static size_t safeSize(const Container& container);

    // Container memory management
    template<typename T>
    static void reserveMemory(std::vector<T>& vec, size_t capacity);

    template<typename T>
    static void optimizeMemory(std::vector<T>& vec);

    template<typename K, typename V>
    static void optimizeMemory(std::map<K, V>& map);

    // Container thread safety utilities
    template<typename T>
    static std::vector<T> threadSafeCopy(const std::vector<T>& vec);

    template<typename T>
    static std::vector<T> threadSafeMerge(const std::vector<T>& a, const std::vector<T>& b);

    // Container conversion utilities
    template<typename T, typename U>
    static std::vector<U> convertVector(const std::vector<T>& vec, 
                                        std::function<U(const T&)> converter);

    // Neural network specific container utilities
    template<typename T>
    static std::vector<T> getNeuronInputs(const std::vector<std::vector<T>>& weights, 
                                          const std::vector<T>& activations);

    template<typename T>
    static std::vector<T> getNeuronOutputs(const std::vector<std::vector<T>>& weights, 
                                           const std::vector<T>& inputs);

    // Container validation
    template<typename T>
    static bool validateVector(const std::vector<T>& vec, 
                              std::function<bool(const T&)> validator);

    template<typename T>
    static bool validateMatrix(const std::vector<std::vector<T>>& matrix,
                              std::function<bool(const T&)> validator);

    // Container monitoring
    template<typename Container>
    static double calculateLoadFactor(const Container& container);

    template<typename T>
    static double calculateUtilization(const std::vector<T>& container,
                                      double capacity = 1.0);

    // Container lifecycle management
    template<typename T>
    static void cleanup(std::vector<T>& vec);

    template<typename T>
    static void reset(std::vector<T>& vec, T defaultValue = T{});

    // Neural network specific container operations
    template<typename T>
    static std::vector<T> createSynapticWeights(size_t inputSize, size_t outputSize);

    template<typename T>
    static std::vector<std::vector<T>> createConnectionMatrix(size_t neuronCount);

    template<typename T>
    static std::vector<T> getLayerOutput(const std::vector<std::vector<T>>& weights,
                                         const std::vector<T>& inputs);

    template<typename T>
    static void updateLayer(std::vector<std::vector<T>>& weights, const std::vector<T>& errors,
                            const std::vector<T>& inputs, double learningRate);

    // Container performance monitoring
    template<typename Container>
    static double calculateAccessTime(const Container& container, size_t iterations = 1000);

    template<typename T>
    static double calculateMemoryUsage(const std::vector<T>& vec);

    template<typename K, typename V>
    static double calculateMemoryUsage(const std::map<K, V>& map);
};

} // namespace Utils
} // namespace Core
} // namespace nlm