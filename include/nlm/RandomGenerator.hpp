#pragma once

#include <random>
#include <chrono>
#include <memory>
#include <string>

namespace nlm {

/**
 * Random number generator with various distributions
 * Thread-safe and efficient random number generation
 */
class RandomGenerator {
public:
    RandomGenerator();
    RandomGenerator(uint32_t seed);
    
    // Get the next random number in [0, 1)
    double next();
    
    // Get random integer in [min, max]
    int nextInt(int min, int max);
    
    // Get random float in [min, max]
    float nextFloat(float min, float max);
    
    // Get random double in [min, max]
    double nextDouble(double min, double max);
    
    // Get standard normal distribution
    double nextNormal();
    
    // Get normal distribution with mean and stddev
    double nextNormal(double mean, double stddev);
    
    // Get exponential distribution
    double nextExponential(double lambda);
    
    // Get gamma distribution
    double nextGamma(double shape, double scale);
    
    // Get uniform distribution
    double nextUniform();
    
    // Get uniform distribution in [min, max]
    double nextUniform(double min, double max);
    
    // Get beta distribution
    double nextBeta(double alpha, double beta);
    
    // Get binomial distribution
    int nextBinomial(int n, double p);
    
    // Get Poisson distribution
    int nextPoisson(double lambda);
    
    // Get chi-squared distribution
    double nextChiSquare(double degreesOfFreedom);
    
    // Get F distribution
    double nextF(double df1, double df2);
    
    // Get Student t distribution
    double nextStudentT(double degreesOfFreedom);
    
    // Get beta prime distribution
    double nextBetaPrime(double alpha, double beta);
    
    // Get Laplace distribution
    double nextLaplace(double mu, double b);
    
    // Get logistic distribution
    double nextLogistic(double mu, double s);
    
    // Get Weibull distribution
    double nextWeibull(double shape, double scale);
    
    // Get Cauchy distribution
    double nextCauchy(double location, double scale);
    
    // Get double exponential distribution
    double nextDoubleExponential(double lambda1, double lambda2);
    
    // Get discrete uniform distribution
    int nextDiscrete(const std::vector<int>& values);
    
    // Get categorical distribution
    int nextCategorical(const std::vector<double>& probabilities);
    
    // Get multinomial distribution
    std::vector<int> nextMultinomial(int n, const std::vector<double>& probabilities);
    
    // Get hypergeometric distribution
    int nextHypergeometric(int N, int K, int n);
    
    // Get geometric distribution
    int nextGeometric(double p);
    
    // Get Pascal distribution
    int nextPascal(int r, double p);
    
    // Get negative binomial distribution
    int nextNegativeBinomial(double p, int r);
    
    // Get Poisson binomial distribution
    int nextPoissonBinomial(const std::vector<double>& probabilities);
    
    // Get Wright-Fisher distribution
    int nextWrightFisher(int N, double p);
    
    // Get Moran distribution
    int nextMoran(int N, double p);
    
    // Get Ewens distribution
    int nextEwens(double theta, int n);
    
    // Get beta-binomial distribution
    int nextBetaBinomial(int n, double alpha, double beta);
    
    // Get beta-negative binomial distribution
    int nextBetaNegativeBinomial(int r, double alpha, double beta);
    
    // Get gamma-binomial distribution
    int nextGammaBinomial(int n, double alpha, double beta);
    
    // Get binomial-beta distribution
    int nextBinomialBeta(double p, double alpha, double beta);
    
    // Get mixture distribution
    double nextMixture(const std::vector<double>& weights,
                       const std::vector<std::function<double()>>& generators);
    
    // Get mixture distribution with given parameters
    double nextMixture(const std::vector<double>& weights,
                       const std::vector<std::function<double(double)>> generators,
                       const std::vector<std::vector<double>>& params);
    
    // Get mixture of normals
    double nextMixtureOfNormals(const std::vector<double>& weights,
                                const std::vector<double>& means,
                                const std::vector<double>& stddevs);
    
    // Get mixture of exponentials
    double nextMixtureOfExponentials(const std::vector<double>& weights,
                                     const std::vector<double>& lambdas);
    
    // Get mixture of gammas
    double nextMixtureOfGammas(const std::vector<double>& weights,
                               const std::vector<double>& shapes,
                               const std::vector<double>& scales);
    
    // Get mixture of betas
    double nextMixtureOfBetas(const std::vector<double>& weights,
                              const std::vector<double>& alphas,
                              const std::vector<double>& betas);
    
    // Get mixture of binomials
    int nextMixtureOfBinomials(int n,
                               const std::vector<double>& weights,
                               const std::vector<double>& ps);
    
    // Get mixture of Poissons
    int nextMixtureOfPoissons(const std::vector<double>& weights,
                             const std::vector<double>& lambdas);
    
    // Get mixture of chi-squares
    double nextMixtureOfChiSquares(const std::vector<double>& weights,
                                   const std::vector<double>& dfs);
    
    // Get mixture of F's
    double nextMixtureOfFs(const std::vector<double>& weights,
                           const std::vector<double>& df1s,
                           const std::vector<double>& df2s);
    
    // Get mixture of Student t's
    double nextMixtureOfStudentTs(const std::vector<double>& weights,
                                  const std::vector<double>& dfs);
    
    // Get mixture of beta primes
    double nextMixtureOfBetaPrimes(const std::vector<double>& weights,
                                    const std::vector<double>& alphas,
                                    const std::vector<double>& betas);
    
    // Get mixture of Laplace distributions
    double nextMixtureOfLaplaces(const std::vector<double>& weights,
                                 const std::vector<double>& mus,
                                 const std::vector<double>& bs);
    
    // Get mixture of logistic distributions
    double nextMixtureOfLogistics(const std::vector<double>& weights,
                                  const std::vector<double>& mus,
                                  const std::vector<double>& ss);
    
    // Get mixture of Weibulls
    double nextMixtureOfWeibulls(const std::vector<double>& weights,
                                 const std::vector<double>& shapes,
                                 const std::vector<double>& scales);
    
    // Get mixture of Cauchy distributions
    double nextMixtureOfCauchys(const std::vector<double>& weights,
                                const std::vector<double>& locations,
                                const std::vector<double>& scales);
    
    // Get mixture of double exponentials
    double nextMixtureOfDoubleExponentials(const std::vector<double>& weights,
                                           const std::vector<double>& lambda1s,
                                           const std::vector<double>& lambda2s);
    
    // Get uniform distribution
    double next() { return uniformDist(generator); }
    
    // Get normal distribution
    double nextNormal() { return normalDist(generator); }
    
    // Get exponential distribution
    double nextExponential() { return exponentialDist(generator); }
    
    // Get gamma distribution
    double nextGamma() { return gammaDist(generator); }
    
    // Get beta distribution
    double nextBeta() { return betaDist(generator); }
    
    // Get chi-squared distribution
    double nextChiSquare() { return chiSquareDist(generator); }
    
    // Get F distribution
    double nextF() { return fDist(generator); }
    
    // Get Student t distribution
    double nextStudentT() { return studentTDist(generator); }
    
    // Get beta prime distribution
    double nextBetaPrime() { return betaPrimeDist(generator); }
    
    // Get Laplace distribution
    double nextLaplace() { return laplaceDist(generator); }
    
    // Get logistic distribution
    double nextLogistic() { return logisticDist(generator); }
    
    // Get Weibull distribution
    double nextWeibull() { return weibullDist(generator); }
    
    // Get Cauchy distribution
    double nextCauchy() { return cauchyDist(generator); }
    
    // Get double exponential distribution
    double nextDoubleExponential() { return doubleExponentialDist(generator); }
    
    // Get discrete uniform distribution
    int nextDiscrete(const std::vector<int>& values) {
        std::uniform_int_distribution<int> dist(0, values.size() - 1);
        return values[dist(generator)];
    }
    
    // Get categorical distribution
    int nextCategorical(const std::vector<double>& probabilities) {
        std::discrete_distribution<int> dist(probabilities.begin(), probabilities.end());
        return dist(generator);
    }
    
    // Get binomial distribution
    int nextBinomial(int n, double p) {
        std::binomial_distribution<int> dist(n, p);
        return dist(generator);
    }
    
    // Get Poisson distribution
    int nextPoisson(double lambda) {
        std::poisson_distribution<int> dist(lambda);
        return dist(generator);
    }
    
    // Get hypergeometric distribution
    int nextHypergeometric(int N, int K, int n) {
        std::hypergeometric_distribution<int> dist(N, K, n);
        return dist(generator);
    }
    
    // Get geometric distribution
    int nextGeometric(double p) {
        std::geometric_distribution<int> dist(p);
        return dist(generator);
    }
    
    // Get Pascal distribution
    int nextPascal(int r, double p) {
        std::negative_binomial_distribution<int> dist(r, p);
        return dist(generator);
    }
    
    // Get negative binomial distribution
    int nextNegativeBinomial(double p, int r) {
        std::negative_binomial_distribution<int> dist(r, p);
        return dist(generator);
    }
    
    // Get Poisson binomial distribution
    int nextPoissonBinomial(const std::vector<double>& probabilities) {
        int result = 0;
        for (double p : probabilities) {
            if (p > 0 && p <= 1) {
                std::bernoulli_distribution dist(p);
                if (dist(generator)) {
                    result++;
                }
            }
        }
        return result;
    }
    
    // Get Wright-Fisher distribution
    int nextWrightFisher(int N, double p) {
        int count = 0;
        for (int i = 0; i < N; i++) {
            if (std::bernoulli_distribution(p)(generator)) {
                count++;
            }
        }
        return count;
    }
    
    // Get Moran distribution
    int nextMoran(int N, double p) {
        int old = std::bernoulli_distribution(p)(generator);
        int newCount = 0;
        for (int i = 0; i < N - 1; i++) {
            if (std::bernoulli_distribution(p)(generator)) {
                newCount++;
            }
        }
        if (old == 1) {
            newCount++;
        } else {
            newCount--;
        }
        return newCount;
    }
    
    // Get Ewens distribution
    int nextEwens(double theta, int n) {
        // Simplified implementation - use random sampling
        int sum = 0;
        for (int i = 0; i < n; i++) {
            double p = theta / (theta + i);
            if (std::bernoulli_distribution(p)(generator)) {
                sum++;
            }
        }
        return sum;
    }
    
    // Get beta-binomial distribution
    int nextBetaBinomial(int n, double alpha, double beta) {
        double p = nextBeta(alpha, beta);
        return nextBinomial(n, p);
    }
    
    // Get beta-negative binomial distribution
    int nextBetaNegativeBinomial(int r, double alpha, double beta) {
        double p = nextBeta(alpha, beta);
        return nextNegativeBinomial(p, r);
    }
    
    // Get gamma-binomial distribution
    int nextGammaBinomial(int n, double alpha, double beta) {
        double p = nextGamma(alpha, beta);
        // Normalize to [0, 1]
        p = std::min(1.0, p / (1.0 + p));
        return nextBinomial(n, p);
    }
    
    // Get binomial-beta distribution
    int nextBinomialBeta(double p, double alpha, double beta) {
        double alphaPrime = nextBeta(alpha, beta);
        return nextBinomial(p, alphaPrime);
    }
    
private:
    std::mt19937 generator;
    std::uniform_real_distribution<double> uniformDist;
    std::normal_distribution<double> normalDist;
    std::exponential_distribution<double> exponentialDist;
    std::gamma_distribution<double> gammaDist;
    std::beta_distribution<double> betaDist;
    std::chi_squared_distribution<double> chiSquareDist;
    std::student_t_distribution<double> studentTDist;
    std::weibull_distribution<double> weibullDist;
    std::extreme_value_distribution<double> extremeValueDist;
    std::cauchy_distribution<double> cauchyDist;
    std::logistic_distribution<double> logisticDist;
    std::pareto_distribution<double> paretoDist;
    std::geometric_distribution<int> geometricDist;
    std::binomial_distribution<int> binomialDist;
    std::poisson_distribution<int> poissonDist;
    std::negative_binomial_distribution<int> negativeBinomialDist;
    std::uniform_int_distribution<int> uniformIntDist;
    
    std::function<double()> getRandomGenerator(const std::string& distribution);
    
    // Statistical methods
    double nextBeta(double alpha, double beta);
    double nextGamma(double shape, double scale);
    double nextChiSquare(double degreesOfFreedom);
    double nextF(double df1, double df2);
    double nextStudentT(double degreesOfFreedom);
    double nextBetaPrime(double alpha, double beta);
    double nextWeibull(double shape, double scale);
    double nextCauchy(double location, double scale);
    double nextLaplace(double mu, double b);
    double nextLogistic(double mu, double s);
    double nextDoubleExponential(double lambda1, double lambda2);
    double nextMixture(const std::vector<double>& weights,
                       const std::vector<std::function<double()>>& generators,
                       const std::vector<std::vector<double>>& params);
    
    std::mt19937 createGenerator(uint32_t seed);
};
}