#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(std::make_unique<Impl>(std::random_device{}())) {}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void RandomGenerator::seed(uint64_t seed) {
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->gen);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::gamma(double shape, double scale) {
    if (shape <= 0.0) {
        throw std::invalid_argument("Gamma shape parameter must be > 0");
    }
    if (scale <= 0.0) {
        throw std::invalid_argument("Gamma scale parameter must be > 0");
    }
    std::gamma_distribution<double> dist(shape, scale);
    return dist(pImpl->gen);
}

double RandomGenerator::weibull(double scale, double shape) {
    if (scale <= 0.0) {
        throw std::invalid_argument("Weibull scale parameter must be > 0");
    }
    if (shape <= 0.0) {
        throw std::invalid_argument("Weibull shape parameter must be > 0");
    }
    std::weibull_distribution<double> dist(scale, shape);
    return dist(pImpl->gen);
}

double RandomGenerator::beta(double alpha, double beta) {
    if (alpha <= 0.0) {
        throw std::invalid_argument("Beta alpha parameter must be > 0");
    }
    if (beta <= 0.0) {
        throw std::invalid_argument("Beta beta parameter must be > 0");
    }
    std::beta_distribution<double> dist(alpha, beta);
    return dist(pImpl->gen);
}

double RandomGenerator::chiSquared(double degreesOfFreedom) {
    if (degreesOfFreedom <= 0.0) {
        throw std::invalid_argument("Chi-squared degrees of freedom must be > 0");
    }
    std::chi_squared_distribution<double> dist(degreesOfFreedom);
    return dist(pImpl->gen);
}

double RandomGenerator::studentT(double degreesOfFreedom) {
    if (degreesOfFreedom <= 0.0) {
        throw std::invalid_argument("Student's t degrees of freedom must be > 0");
    }
    std::student_t_distribution<double> dist(degreesOfFreedom);
    return dist(pImpl->gen);
}

double RandomGenerator::fDistribution(double numDof, double denDof) {
    if (numDof <= 0.0) {
        throw std::invalid_argument("F-distribution numerator degrees of freedom must be > 0");
    }
    if (denDof <= 0.0) {
        throw std::invalid_argument("F-distribution denominator degrees of freedom must be > 0");
    }
    std::fisher_f_distribution<double> dist(numDof, denDof);
    return dist(pImpl->gen);
}

double RandomGenerator::cauchy(double location, double scale) {
    if (scale <= 0.0) {
        throw std::invalid_argument("Cauchy scale parameter must be > 0");
    }
    std::cauchy_distribution<double> dist(location, scale);
    return dist(pImpl->gen);
}

double RandomGenerator::laplace(double mu, double b) {
    if (b <= 0.0) {
        throw std::invalid_argument("Laplace b parameter must be > 0");
    }
    std::laplace_distribution<double> dist(mu, b);
    return dist(pImpl->gen);
}

double RandomGenerator::logNormal(double mean, double stddev) {
    if (stddev <= 0.0) {
        throw std::invalid_argument("Log-normal standard deviation must be > 0");
    }
    std::lognormal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

uint64_t RandomGenerator::geometric(double p) {
    if (p <= 0.0 || p > 1.0) {
        throw std::invalid_argument("Geometric probability p must be in range (0, 1]");
    }
    std::geometric_distribution<uint64_t> dist(p);
    return dist(pImpl->gen);
}

std::string RandomGenerator::randomString(size_t length, const std::string& charset) {
    if (charset.empty()) {
        throw std::invalid_argument("Charset cannot be empty");
    }
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        size_t index = pickRandomIndex(charset.size());
        result += charset[index];
    }
    
    return result;
}

std::string RandomGenerator::randomUUID() {
    // Generate UUID version 4 (random)
    std::string uuid;
    uuid.reserve(36); // 36 chars for standard UUID format
    
    // Format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    // Generate random hex digits
    auto randomHex = [this]() {
        uint32_t val = uniformInt(0, 0x0FFFFFFF);
        return std::hex << (val & 0xF); // Only use lower 4 bits
    };
    
    // First block: 8 hex digits
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    
    // Add dash
    uuid += '-';
    
    // Second block: 4 hex digits
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    
    // Add dash
    uuid += '-';
    
    // Third block: 4 hex digits with version 4
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)]; // Set bit 7 of third byte to 4 (version 4)
    uuid += '8'; // Set bits 6 and 7 of fourth byte to 8 and 4 (variant)
    uuid += "89ab"[uniformInt(0, 3)];
    
    // Add dash
    uuid += '-';
    
    // Fourth block: 4 hex digits
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    
    // Add dash
    uuid += '-';
    
    // Fifth block: 12 hex digits
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    uuid += "0123456789abcdef"[uniformInt(0, 15)];
    
    return uuid;
}

std::string RandomGenerator::randomHexColor() {
    // Generate random hex color in format #RRGGBB
    uint32_t r = uniformInt(0, 255);
    uint32_t g = uniformInt(0, 255);
    uint32_t b = uniformInt(0, 255);
    
    std::stringstream ss;
    ss << "#" 
       << std::hex << std::setfill('0') << std::setw(2) << r
       << std::setw(2) << g
       << std::setw(2) << b;
    
    return ss.str();
}

template<typename Container>
auto RandomGenerator::randomFromList(const Container& list) -> typename Container::const_reference {
    return pickRandom(list);
}

bool RandomGenerator::isInConfidenceInterval(double value, double mean, double stddev, double confidence) {
    if (stddev <= 0.0) {
        throw std::invalid_argument("Standard deviation must be > 0");
    }
    if (confidence <= 0.0 || confidence >= 1.0) {
        throw std::invalid_argument("Confidence level must be in range (0, 1)");
    }
    
    // Calculate z-score for given confidence level
    // For 95% confidence: z = 1.96
    // For 99% confidence: z = 2.58
    double zScore = std::erfc(std::sqrt(2.0) * std::erfc_inv(2.0 * (1.0 - confidence) / 2.0)) * std::sqrt(2.0);
    
    double marginOfError = zScore * stddev;
    double lowerBound = mean - marginOfError;
    double upperBound = mean + marginOfError;
    
    return (value >= lowerBound && value <= upperBound);
}

} // namespace nlm
