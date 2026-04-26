import React, { useMemo } from 'react'
import { Bar } from 'react-chartjs-2'
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  Title,
  Tooltip,
  Legend
} from 'chart.js'

ChartJS.register(
  CategoryScale,
  LinearScale,
  BarElement,
  Title,
  Tooltip,
  Legend
)

function FFTSpectrum({ history, mode }) {
  const fftData = useMemo(() => {
    if (history.length < 8) return []
    
    // Simple FFT simulation - take last 64 samples
    const samples = history.slice(-64).map(m => m.processCount)
    
    // Simulate frequency magnitudes
    const frequencies = Array.from({ length: 32 }, (_, i) => {
      const magnitude = Math.abs(
        samples.reduce((sum, val, idx) => 
          sum + val * Math.cos(2 * Math.PI * i * idx / samples.length), 0
        )
      )
      return magnitude / samples.length
    })
    
    return frequencies
  }, [history])

  const data = {
    labels: fftData.map((_, i) => `F${i}`),
    datasets: [
      {
        label: 'Frequency Magnitude',
        data: fftData,
        backgroundColor: mode === 'ATTACK' 
          ? fftData.map(v => v > 5 ? 'rgba(255, 74, 74, 0.8)' : 'rgba(74, 158, 255, 0.8)')
          : 'rgba(74, 158, 255, 0.8)',
        borderColor: mode === 'ATTACK' ? '#ff4a4a' : '#4a9eff',
        borderWidth: 1
      }
    ]
  }

  const options = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        display: false
      },
      tooltip: {
        callbacks: {
          label: (context) => `Magnitude: ${context.parsed.y.toFixed(2)}`
        }
      }
    },
    scales: {
      y: {
        beginAtZero: true,
        grid: {
          color: 'rgba(255, 255, 255, 0.1)'
        },
        ticks: {
          color: '#8b92b0'
        }
      },
      x: {
        grid: {
          display: false
        },
        ticks: {
          color: '#8b92b0',
          maxRotation: 0
        }
      }
    }
  }

  return (
    <div style={{ height: '300px' }}>
      {fftData.length > 0 ? (
        <>
          <Bar data={data} options={options} />
          {mode === 'ATTACK' && fftData.some(v => v > 5) && (
            <div style={{ 
              marginTop: '1rem', 
              padding: '0.8rem', 
              background: 'rgba(255, 74, 74, 0.2)',
              borderRadius: '8px',
              color: '#ff4a4a',
              fontWeight: 600
            }}>
              ⚠️ Anomaly Detected: Unusual frequency patterns in system activity
            </div>
          )}
        </>
      ) : (
        <div style={{ 
          display: 'flex', 
          alignItems: 'center', 
          justifyContent: 'center',
          height: '100%',
          color: '#8b92b0'
        }}>
          Collecting data for FFT analysis...
        </div>
      )}
    </div>
  )
}

export default FFTSpectrum
