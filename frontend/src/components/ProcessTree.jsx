import React from 'react'
import './ProcessTree.css'

function ProcessTree({ processCount, mode, processes = [] }) {
  // Use real processes if available, otherwise generate mock data
  const displayProcesses = processes.length > 0 
    ? processes.slice(0, 20)
    : Array.from({ length: Math.min(processCount, 20) }, (_, i) => ({
        pid: 1000 + i,
        name: `Process-${i + 1}`,
        running: true
      }))

  return (
    <div className="process-tree">
      {displayProcesses.map((proc, idx) => (
        <div key={proc.pid || idx} className={`process-node ${mode === 'ATTACK' && idx > 15 ? 'suspicious' : 'normal'}`}>
          <div className="process-icon">⚙️</div>
          <div className="process-info">
            <div className="process-name">{proc.name}</div>
            <div className="process-pid">PID: {proc.pid}</div>
          </div>
          {mode === 'ATTACK' && idx > 15 && (
            <div className="warning-badge">⚠️</div>
          )}
        </div>
      ))}
      {processCount > 20 && (
        <div className="more-processes">
          +{processCount - 20} more processes
        </div>
      )}
    </div>
  )
}

export default ProcessTree
