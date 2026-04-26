#ifndef FFT_ENGINE_H
#define FFT_ENGINE_H

#include <vector>
#include <complex>
#include <cmath>

namespace edr {

class FFTEngine {
public:
    FFTEngine();
    ~FFTEngine();

    // Perform FFT on real-valued signal
    std::vector<std::complex<double>> compute(const std::vector<double>& signal);
    
    // Get magnitude spectrum
    std::vector<double> getMagnitudeSpectrum(const std::vector<std::complex<double>>& fft_result);
    
    // Detect anomalies based on frequency analysis
    bool detectAnomaly(const std::vector<double>& magnitude, double threshold = 2.0);
    
    // Get dominant frequencies
    std::vector<size_t> getDominantFrequencies(const std::vector<double>& magnitude, size_t top_n = 5);

private:
    void fft(std::vector<std::complex<double>>& data);
    size_t reverseBits(size_t num, size_t bits);
};

} // namespace edr

#endif // FFT_ENGINE_H
