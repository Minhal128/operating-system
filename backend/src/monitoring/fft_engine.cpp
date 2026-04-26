#include "fft_engine.h"
#include <algorithm>
#include <numeric>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace edr {

FFTEngine::FFTEngine() {}
FFTEngine::~FFTEngine() {}

std::vector<std::complex<double>> FFTEngine::compute(const std::vector<double>& signal) {
    size_t n = signal.size();
    
    // Pad to next power of 2
    size_t n_padded = 1;
    while (n_padded < n) {
        n_padded <<= 1;
    }
    
    std::vector<std::complex<double>> data(n_padded);
    for (size_t i = 0; i < n; ++i) {
        data[i] = std::complex<double>(signal[i], 0.0);
    }
    for (size_t i = n; i < n_padded; ++i) {
        data[i] = std::complex<double>(0.0, 0.0);
    }
    
    fft(data);
    return data;
}

void FFTEngine::fft(std::vector<std::complex<double>>& data) {
    size_t n = data.size();
    if (n <= 1) return;
    
    // Bit-reversal permutation
    size_t bits = 0;
    size_t temp = n;
    while (temp > 1) {
        bits++;
        temp >>= 1;
    }
    
    for (size_t i = 0; i < n; ++i) {
        size_t j = reverseBits(i, bits);
        if (j > i) {
            std::swap(data[i], data[j]);
        }
    }
    
    // Cooley-Tukey FFT
    for (size_t s = 1; s <= bits; ++s) {
        size_t m = 1 << s;
        size_t m2 = m >> 1;
        std::complex<double> w(1, 0);
        std::complex<double> wm = std::exp(std::complex<double>(0, -2.0 * M_PI / m));
        
        for (size_t j = 0; j < m2; ++j) {
            for (size_t k = j; k < n; k += m) {
                std::complex<double> t = w * data[k + m2];
                std::complex<double> u = data[k];
                data[k] = u + t;
                data[k + m2] = u - t;
            }
            w *= wm;
        }
    }
}

size_t FFTEngine::reverseBits(size_t num, size_t bits) {
    size_t result = 0;
    for (size_t i = 0; i < bits; ++i) {
        result = (result << 1) | (num & 1);
        num >>= 1;
    }
    return result;
}

std::vector<double> FFTEngine::getMagnitudeSpectrum(const std::vector<std::complex<double>>& fft_result) {
    std::vector<double> magnitude;
    magnitude.reserve(fft_result.size() / 2);
    
    for (size_t i = 0; i < fft_result.size() / 2; ++i) {
        magnitude.push_back(std::abs(fft_result[i]));
    }
    
    return magnitude;
}

bool FFTEngine::detectAnomaly(const std::vector<double>& magnitude, double threshold) {
    if (magnitude.empty()) return false;
    
    // Calculate mean and standard deviation
    double mean = std::accumulate(magnitude.begin(), magnitude.end(), 0.0) / magnitude.size();
    
    double sq_sum = 0.0;
    for (const auto& val : magnitude) {
        sq_sum += (val - mean) * (val - mean);
    }
    double stddev = std::sqrt(sq_sum / magnitude.size());
    
    // Check if any frequency component exceeds threshold
    for (const auto& val : magnitude) {
        if (val > mean + threshold * stddev) {
            return true;
        }
    }
    
    return false;
}

std::vector<size_t> FFTEngine::getDominantFrequencies(const std::vector<double>& magnitude, size_t top_n) {
    std::vector<std::pair<double, size_t>> indexed_mag;
    for (size_t i = 0; i < magnitude.size(); ++i) {
        indexed_mag.push_back({magnitude[i], i});
    }
    
    std::partial_sort(indexed_mag.begin(), 
                     indexed_mag.begin() + std::min(top_n, indexed_mag.size()),
                     indexed_mag.end(),
                     [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<size_t> dominant;
    for (size_t i = 0; i < std::min(top_n, indexed_mag.size()); ++i) {
        dominant.push_back(indexed_mag[i].second);
    }
    
    return dominant;
}

} // namespace edr
