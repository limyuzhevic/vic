#include "Vision.hpp"
#include "../../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Vision::Impl {
    size_t width;
    size_t height;
    size_t channels;
    std::vector<float> data;
    std::vector<float> processedFeatures;
    
    Impl(size_t w, size_t h, size_t c) : width(w), height(h), channels(c), 
                                       data(w * h * c, 0.0f), processedFeatures() {}
};

Vision::Vision() : pImpl(new Impl(0, 0, 0)) {}

Vision::Vision(size_t width, size_t height, size_t channels) 
    : pImpl(new Impl(width, height, channels)) {}

Vision::~Vision() = default;

const char* Vision::getType() const {
    return "Vision";
}

const std::vector<float>& Vision::getData() const {
    return pImpl->data;
}

size_t Vision::getDimensions() const {
    return pImpl->width * pImpl->height * pImpl->channels;
}

std::unique_ptr<SensoryInput> Vision::clone() const {
    auto vision = std::make_unique<Vision>(pImpl->width, pImpl->height, pImpl->channels);
    vision->setData(pImpl->data);
    vision->pImpl->processedFeatures = pImpl->processedFeatures;
    return vision;
}

void Vision::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
        // Process data through neural networks
        processWithNeurons({});
    }
}

void Vision::setData(float* data, size_t size) {
    if (size == pImpl->data.size()) {
        pImpl->data.assign(data, data + size);
        // Process data through neural networks
        processWithNeurons({});
    }
}

size_t Vision::getWidth() const { return pImpl->width; }
size_t Vision::getHeight() const { return pImpl->height; }
size_t Vision::getChannels() const { return pImpl->channels; }

void Vision::processWithNeurons(const std::vector<Neuron*>& featureNeurons) {
    // Convert raw image data to feature vector using neural processing
    pImpl->processedFeatures.clear();
    
    if (featureNeurons.empty() || pImpl->data.empty()) {
        // Fallback: compute simple image statistics
        std::vector<float> gray;
        if (pImpl->channels >= 3) {
            // Simple grayscale conversion
            size_t pixels = pImpl->width * pImpl->height;
            gray.reserve(pixels);
            for (size_t i = 0; i < pixels; ++i) {
                size_t idx = i * pImpl->channels;
                // RGB to grayscale
                float grayVal = 0.299f * pImpl->data[idx] + 
                                0.587f * pImpl->data[idx + 1] + 
                                0.114f * pImpl->data[idx + 2];
                gray.push_back(grayVal);
            }
        } else {
            gray = pImpl->data;
        }
        
        // Compute features from grayscale image
        // Feature 1: Average brightness
        float avgBrightness = std::accumulate(gray.begin(), gray.end(), 0.0f) / gray.size();
        pImpl->processedFeatures.push_back(avgBrightness);
        
        // Feature 2: Contrast (standard deviation)
        float sum = 0.0f, sumSq = 0.0f;
        for (float val : gray) {
            sum += val;
            sumSq += val * val;
        }
        float mean = sum / gray.size();
        float variance = (sumSq / gray.size()) - (mean * mean);
        pImpl->processedFeatures.push_back(std::sqrt(std::max(0.0f, variance)));
        
        // Feature 3: Edge density (simple gradient computation)
        float edgeCount = 0.0f;
        for (size_t y = 1; y < pImpl->height; ++y) {
            for (size_t x = 1; x < pImpl->width; ++x) {
                size_t idx1 = (y - 1) * pImpl->width + (x - 1);
                size_t idx2 = y * pImpl->width + x;
                float diff = std::abs(gray[idx1] - gray[idx2]);
                if (diff > 0.1f) edgeCount++;
            }
        }
        pImpl->processedFeatures.push_back(edgeCount / (pImpl->width * pImpl->height));
        
        return;
    }
    
    // Neural feature extraction using provided neurons
    for (Neuron* neuron : featureNeurons) {
        // Simulate neural response to visual input
        float neuronResponse = 0.0f;
        
        // Get neuron type and apply appropriate processing
        NeuronType type = neuron->getType();
        
        switch (type) {
            case NeuronType::Sensory:
                // Vision neurons respond to edge detection
                // Simple Sobel-like edge detection
                float edgeResponse = 0.0f;
                for (size_t y = 0; y < pImpl->height; ++y) {
                    for (size_t x = 0; x < pImpl->width; ++x) {
                        // Compute gradient magnitude
                        float gx = 0.0f, gy = 0.0f;
                        
                        // Central differences for gradient
                        if (x > 0 && x < pImpl->width - 1) {
                            gx = pImpl->data[(y * pImpl->width + x + 1) * pImpl->channels + 0] -
                                 pImpl->data[(y * pImpl->width + x - 1) * pImpl->channels + 0];
                        }
                        if (y > 0 && y < pImpl->height - 1) {
                            gy = pImpl->data[((y + 1) * pImpl->width + x) * pImpl->channels + 0] -
                                 pImpl->data[((y - 1) * pImpl->width + x) * pImpl->channels + 0];
                        }
                        
                        float gradient = std::sqrt(gx * gx + gy * gy);
                        if (gradient > 0.1f) edgeResponse += gradient;
                    }
                }
                neuronResponse = edgeResponse / (pImpl->width * pImpl->height);
                break;
                
            case NeuronType::Internal:
                // Internal neurons compute abstract features
                // Simple histogram-based features
                float hist[4] = {0.0f};  // 4 histogram bins
                for (float val : gray) {
                    int bin = static_cast<int>(val * 4.0f);
                    if (bin >= 0 && bin < 4) hist[bin]++;
                }
                // Normalize histogram
                float histSum = std::accumulate(hist, hist + 4, 0.0f);
                if (histSum > 0) {
                    for (int i = 0; i < 4; ++i) {
                        neuronResponse += hist[i] / histSum * (i + 1);
                    }
                }
                break;
                
            case NeuronType::Motor:
                // Motor neurons integrate global features
                neuronResponse = avgBrightness * 0.4f + contrast * 0.3f + edgeDensity * 0.3f;
                break;
                
            default:
                neuronResponse = avgBrightness;
                break;
        }
        
        pImpl->processedFeatures.push_back(neuronResponse);
    }
}

const std::vector<float>& Vision::getProcessedFeatures() const {
    return pImpl->processedFeatures;
}

void Vision::applyEdgeDetection(float threshold) {
    // Apply Sobel edge detection to raw data
    std::vector<float> gray;
    if (pImpl->channels >= 3) {
        // Convert to grayscale
        size_t pixels = pImpl->width * pImpl->height;
        gray.reserve(pixels);
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * pImpl->channels;
            float grayVal = 0.299f * pImpl->data[idx] + 
                            0.587f * pImpl->data[idx + 1] + 
                            0.114f * pImpl->data[idx + 2];
            gray.push_back(grayVal);
        }
    } else {
        gray = pImpl->data;
    }
    
    // Simple edge detection using neighbor differences
    for (size_t y = 0; y < pImpl->height; ++y) {
        for (size_t x = 0; x < pImpl->width; ++x) {
            float left = (x > 0) ? gray[y * pImpl->width + (x - 1)] : gray[y * pImpl->width + x];
            float right = (x < pImpl->width - 1) ? gray[y * pImpl->width + (x + 1)] : gray[y * pImpl->width + x];
            float up = (y > 0) ? gray[(y - 1) * pImpl->width + x] : gray[y * pImpl->width + x];
            float down = (y < pImpl->height - 1) ? gray[(y + 1) * pImpl->width + x] : gray[y * pImpl->width + x];
            
            float gradient = std::sqrt(std::pow(right - left, 2) + std::pow(down - up, 2));
            if (gradient > threshold) {
                // Set edge pixel
                size_t idx = (y * pImpl->width + x) * pImpl->channels;
                for (size_t c = 0; c < pImpl->channels; ++c) {
                    pImpl->data[idx + c] = 1.0f;
                }
            }
        }
    }
}

void Vision::applyGaussianBlur(float sigma) {
    // Simple Gaussian blur
    float sigma2 = sigma * sigma;
    float norm = 1.0f / (2.0f * M_PI * sigma2);
    
    std::vector<float> blurred(pImpl->data.size(), 0.0f);
    
    for (size_t y = 0; y < pImpl->height; ++y) {
        for (size_t x = 0; x < pImpl->width; ++x) {
            float sum = 0.0f, weightSum = 0.0f;
            
            // Apply Gaussian kernel
            for (int ky = -2; ky <= 2; ++ky) {
                int ny = y + ky;
                if (ny < 0 || ny >= static_cast<int>(pImpl->height)) continue;
                
                for (int kx = -2; kx <= 2; ++kx) {
                    int nx = x + kx;
                    if (nx < 0 || nx >= static_cast<int>(pImpl->width)) continue;
                    
                    float weight = norm * std::exp(-(ky*ky + kx*kx) / (2.0f * sigma2));
                    size_t srcIdx = (ny * pImpl->width + nx) * pImpl->channels;
                    size_t dstIdx = (y * pImpl->width + x) * pImpl->channels;
                    
                    for (size_t c = 0; c < pImpl->channels; ++c) {
                        sum += pImpl->data[srcIdx + c] * weight;
                        weightSum += weight;
                    }
                }
            }
            
            if (weightSum > 0.0f) {
                size_t dstIdx = (y * pImpl->width + x) * pImpl->channels;
                for (size_t c = 0; c < pImpl->channels; ++c) {
                    blurred[dstIdx + c] = sum / weightSum;
                }
            }
        }
    }
    
    pImpl->data = blurred;
}

void Vision::applySobelOperator() {
    // Apply Sobel edge detection
    std::vector<float> gray;
    if (pImpl->channels >= 3) {
        // Convert to grayscale
        size_t pixels = pImpl->width * pImpl->height;
        gray.reserve(pixels);
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * pImpl->channels;
            float grayVal = 0.299f * pImpl->data[idx] + 
                            0.587f * pImpl->data[idx + 1] + 
                            0.114f * pImpl->data[idx + 2];
            gray.push_back(grayVal);
        }
    } else {
        gray = pImpl->data;
    }
    
    std::vector<float> edges(pImpl->width * pImpl->height, 0.0f);
    
    // Sobel operators
    const int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    for (size_t y = 0; y < pImpl->height; ++y) {
        for (size_t x = 0; x < pImpl->width; ++x) {
            float gx_val = 0.0f, gy_val = 0.0f;
            
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int ny = y + ky;
                    int nx = x + kx;
                    if (ny < 0 || ny >= static_cast<int>(pImpl->height) ||
                        nx < 0 || nx >= static_cast<int>(pImpl->width)) continue;
                    
                    float pixel = gray[ny * pImpl->width + nx];
                    gx_val += pixel * gx[ky + 1][kx + 1];
                    gy_val += pixel * gy[ky + 1][kx + 1];
                }
            }
            
            float magnitude = std::sqrt(gx_val * gx_val + gy_val * gy_val);
            edges[y * pImpl->width + x] = magnitude;
        }
    }
    
    // Normalize and store as magnitude
    float max_edge = *std::max_element(edges.begin(), edges.end());
    if (max_edge > 0.0f) {
        for (float& edge : edges) {
            edge /= max_edge;
        }
        
        // Convert to grayscale image
        for (size_t i = 0; i < edges.size(); ++i) {
            float edge_val = edges[i];
            size_t idx = i * pImpl->channels;
            for (size_t c = 0; c < pImpl->channels; ++c) {
                pImpl->data[idx + c] = edge_val;
            }
        }
    }
}

std::vector<float> Vision::getFeatureVector() const {
    std::vector<float> features;
    
    // Add basic image statistics
    if (!pImpl->data.empty()) {
        // Grayscale conversion and features
        std::vector<float> gray;
        if (pImpl->channels >= 3) {
            size_t pixels = pImpl->width * pImpl->height;
            gray.reserve(pixels);
            for (size_t i = 0; i < pixels; ++i) {
                size_t idx = i * pImpl->channels;
                float grayVal = 0.299f * pImpl->data[idx] + 
                                0.587f * pImpl->data[idx + 1] + 
                                0.114f * pImpl->data[idx + 2];
                gray.push_back(grayVal);
            }
        } else {
            gray = pImpl->data;
        }
        
        // Compute basic features
        float avg = std::accumulate(gray.begin(), gray.end(), 0.0f) / gray.size();
        float variance = 0.0f;
        for (float val : gray) {
            variance += (val - avg) * (val - avg);
        }
        variance /= gray.size();
        
        features.push_back(avg);  // brightness
        features.push_back(std::sqrt(variance));  // contrast
        
        // Edge density
        float edgeCount = 0.0f;
        for (size_t y = 1; y < pImpl->height; ++y) {
            for (size_t x = 1; x < pImpl->width; ++x) {
                float diff = std::abs(gray[y * pImpl->width + x] - 
                                     gray[(y-1) * pImpl->width + x]);
                if (diff > 0.1f) edgeCount++;
            }
        }
        features.push_back(edgeCount / (pImpl->width * pImpl->height));
    }
    
    // Add neural features if available
    features.insert(features.end(), pImpl->processedFeatures.begin(), 
                   pImpl->processedFeatures.end());
    
    return features;
}

} // namespace nlm
