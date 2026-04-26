import React from 'react'
import { Line } from 'react-chartjs-2'
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
} from 'chart.js'

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
)

function MemoryChart({ history }) {
  const data = {
    labels: history.map((_, i) => i),
    datasets: [
      {
        label: 'System Memory (MB)',
        data: history.map(m => m.systemMemoryUsage / 1024),
        borderColor: '#4a9eff',
        backgroundColor: 'rgba(74, 158, 255, 0.1)',
        fill: true,
        tension: 0.4
      }
    ]
  }

  const options = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        display: false
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
        display: false
      }
    }
  }

  return (
    <div style={{ height: '250px' }}>
      <Line data={data} options={options} />
    </div>
  )
}

export default MemoryChart
