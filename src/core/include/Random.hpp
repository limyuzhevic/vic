// Core random number generation
#ifndef NLM_CORE_RANDOM_HPP
#define NLM_CORE_RANDOM_HPP

#include <random>
#include <chrono>

namespace nlm {
namespace core {

class Random {
public:
    Random();
    ~Random();
    
    double uniform(double min = 0.0, double max = 1.0);
    double gaussian(double mean = 0.0, double stddev = 1.0);
    int integer(int min = 0, int max = 100);
    bool bernoulli(double probability = 0.5);
    
    static Random& getInstance();
    
private:
    std::mt19937 generator;
    std::uniform_real_distribution<double> uniformDist;
    std::normal_distribution<double> normalDist;
    std::bernoulli_distribution bernoulliDist;
};

} // namespace core
} // namespace nlm

#endif // NLM_CORE_RANDOM_HPP
