#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// Thread-local storage for RandomGenerator
thread_local RandomGenerator::ThreadLocalState RandomGenerator::threadLocal_;

bool RandomGenerator::isInitialized() const {
    return initialized_.load(std::memory_order_acquire);
}

uint64_t RandomGenerator::nextSeed() {
    std::lock_guard<std::mutex> lock(seedsMutex_);
    
    // Use high-quality seed (current time + process id + thread id)
    uint64_t seed = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    #ifdef _WIN32
        seed ^= static_cast<uint64_t>(GetCurrentProcessId()) << 32;
    #else
        seed ^= static_cast<uint64_t>(getpid()) << 32;
    #endif
    seed ^= static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    return seed;
}

RandomGenerator::RandomGenerator() : generator_(), initialized_(false) {
    // Thread-safe initialization
    std::call_once(onceFlag_, [this]() {
        seed_ = nextSeed();
        generator_.seed(seed_);
        initialized_.store(true, std::memory_order_release);
    });
}

RandomGenerator::~RandomGenerator() = default;

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (!initialized_.load(std::memory_order_acquire)) {
        std::call_once(onceFlag_, [this]() {
            seed_ = nextSeed();
            generator_.seed(seed_);
            initialized_.store(true, std::memory_order_release);
        });
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(generator_);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (!initialized_.load(std::memory_order_acquire)) {
        std::call_once(onceFlag_, [this]() {
            seed_ = nextSeed();
            generator_.seed(seed_);
            initialized_.store(true, std::memory_order_release);
        });
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator_);
}

int64_t RandomGenerator::uniformInt64(int64_t min, int64_t max) {
    if (!initialized_.load(std::memory_order_acquire)) {
        std::call_once(onceFlag_, [this]() {
            seed_ = nextSeed();
            generator_.seed(seed_);
            initialized_.store(true, std::memory_order_release);
        });
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::uniform_int_distribution<int64_t> dist(min, max);
    return dist(generator_);
}

void RandomGenerator::shuffle(uint32_t* array, size_t n) {
    if (!initialized_.load(std::memory_order_acquire)) {
        std::call_once(onceFlag_, [this]() {
            seed_ = nextSeed();
            generator_.seed(seed_);
            initialized_.store(true, std::memory_order_release);
        });
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = uniformInt(0, i);
        std::swap(array[i], array[j]);
    }
}

void RandomGenerator::shuffle(float* array, size_t n) {
    if (!initialized_.load(std::memory_order_acquire)) {
        std::call_once(onceFlag_, [this]() {
            seed_ = nextSeed();
            generator_.seed(seed_);
            initialized_.store(true, std::memory_order_release);
        });
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = uniformInt(0, i);
        std::swap(array[i], array[j]);
    }
}

uint32_t RandomGenerator::randomBit() {
    if (!initialized_.load(std::memory_order_acquire)) {
        std::call_once(onceFlag_, [this]() {
            seed_ = nextSeed();
            generator_.seed(seed_);
            initialized_.store(true, std::memory_order_release);
        });
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::uniform_int_distribution<uint32_t> dist(0, 1);
    return dist(generator_);
}

RandomGenerator::ThreadLocalState::ThreadLocalState() {
    // Thread-local state for improved performance
    localSeed = 0;
    localIndex = 0;
}

RandomGenerator::ThreadLocalState::~ThreadLocalState() = default;

uint32_t RandomGenerator::getThreadLocalRandom() {
    return threadLocal_.localSeed++;
}

void RandomGenerator::reset(uint64_t seed) {
    std::lock_guard<std::mutex> lock(mutex_);
    seed_ = seed;
    generator_.seed(seed);
}

uint64_t RandomGenerator::getSeed() const {
    return seed_;
}

bool RandomGenerator::isSameSeed(const RandomGenerator& other) const {
    return seed_ == other.seed_;
}

RandomGenerator& RandomGenerator::getGlobalInstance() {
    static RandomGenerator instance;
    return instance;
}

RandomGenerator RandomGenerator::getThreadInstance() {
    return RandomGenerator();
}

} // namespace nlm
