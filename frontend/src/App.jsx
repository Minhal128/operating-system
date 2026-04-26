import React, { useState, useEffect } from 'react'
import ProcessTree from './components/ProcessTree'
import MemoryChart from './components/MemoryChart'
import FileHeatmap from './components/FileHeatmap'
import FFTSpectrum from './components/FFTSpectrum'
import AlertsPanel from './components/AlertsPanel'
import './App.css'

function App() {
  const [metrics, setMetrics] = useState({
    processCount: 0,
    memoryAllocated: 0,
    systemMemoryUsage: 0,
    fileOperations: 0,
    ipcMessages: 0,
    cpuUsage: 0
  })
  
  const [mode, setMode] = useState('NORMAL')
  const [alerts, setAlerts] = useState([])
  const [metricsHistory, setMetricsHistory] = useState([])
  const [processes, setProcesses] = useState([])
  const [isConnected, setIsConnected] = useState(false)

  useEffect(() => {
    // Fetch real data from backend
    const fetchMetrics = async () => {
      try {
        const response = await fetch('/api/metrics.json')
        if (response.ok) {
          const data = await response.json()
          
          const newMetrics = {
            processCount: data.processCount || 0,
            memoryAllocated: data.memoryAllocated || 0,
            systemMemoryUsage: data.systemMemoryUsage || 0,
            fileOperations: data.fileOperations || 0,
            ipcMessages: data.ipcMessages || 0,
            cpuUsage: data.cpuUsage || 0,
            timestamp: data.timestamp || Date.now()
          }
          
          setMetrics(newMetrics)
          setMetricsHistory(prev => [...prev.slice(-100), newMetrics])
          setMode(data.mode || 'NORMAL')
          setProcesses(data.processes || [])
          setIsConnected(true)
          
          // Update alerts
          if (data.alerts && data.alerts.length > 0) {
            const newAlerts = data.alerts.map((msg, idx) => ({
              id: Date.now() + idx,
              message: msg,
              severity: 'high',
              timestamp: new Date().toLocaleTimeString()
            }))
            setAlerts(prev => [...prev, ...newAlerts].slice(-20))
          }
        } else {
          setIsConnected(false)
        }
      } catch (error) {
        console.error('Failed to fetch metrics:', error)
        setIsConnected(false)
      }
    }

    // Fetch immediately
    fetchMetrics()
    
    // Then fetch every second
    const interval = setInterval(fetchMetrics, 1000)
    return () => clearInterval(interval)
  }, [])

  return (
    <div className="app">
      <header className="header">
        <h1>🛡️ EDR System - Real-Time Intrusion Detection</h1>
        <div className="mode-selector">
          <div className={`status-indicator ${isConnected ? 'connected' : 'disconnected'}`}>
            {isConnected ? '● Connected to Backend' : '○ Disconnected'}
          </div>
          <span className={`mode-badge ${mode === 'ATTACK' ? 'attack' : 'normal'}`}>
            {mode} MODE
          </span>
        </div>
      </header>

      <div className="metrics-bar">
        <div className="metric">
          <span className="label">Processes</span>
          <span className="value">{metrics.processCount}</span>
        </div>
        <div className="metric">
          <span className="label">System Memory (MB)</span>
          <span className="value">{(metrics.systemMemoryUsage / 1024).toFixed(0)}</span>
        </div>
        <div className="metric">
          <span className="label">File Ops</span>
          <span className="value">{metrics.fileOperations}</span>
        </div>
        <div className="metric">
          <span className="label">IPC Messages</span>
          <span className="value">{metrics.ipcMessages}</span>
        </div>
        <div className="metric">
          <span className="label">CPU Usage</span>
          <span className="value">{metrics.cpuUsage.toFixed(1)}%</span>
        </div>
      </div>

      <div className="dashboard">
        <div className="grid">
          <div className="panel">
            <h2>Process Tree</h2>
            <ProcessTree processCount={metrics.processCount} mode={mode} processes={processes} />
          </div>

          <div className="panel">
            <h2>Memory Usage</h2>
            <MemoryChart history={metricsHistory} />
          </div>

          <div className="panel">
            <h2>File Activity Heatmap</h2>
            <FileHeatmap fileOps={metrics.fileOperations} mode={mode} />
          </div>



          <div className="panel large">
            <h2>FFT Frequency Spectrum (Anomaly Detection)</h2>
            <FFTSpectrum history={metricsHistory} mode={mode} />
          </div>

          <div className="panel">
            <h2>Alerts</h2>
            <AlertsPanel alerts={alerts} onClear={() => setAlerts([])} />
          </div>
        </div>
      </div>
    </div>
  )
}

export default App
