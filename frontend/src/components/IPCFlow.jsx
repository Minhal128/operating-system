import React from 'react'
import './IPCFlow.css'

function IPCFlow({ messages }) {
  const flows = Array.from({ length: Math.min(messages / 100, 10) }, (_, i) => ({
    id: i,
    source: `Process-${i * 2}`,
    target: `Process-${i * 2 + 1}`,
    count: Math.floor(Math.random() * 100)
  }))

  return (
    <div className="ipc-flow">
      {flows.map(flow => (
        <div key={flow.id} className="flow-item">
          <div className="flow-source">{flow.source}</div>
          <div className="flow-arrow">
            <div className="arrow-line" />
            <div className="arrow-head">→</div>
          </div>
          <div className="flow-target">{flow.target}</div>
          <div className="flow-count">{flow.count} msgs</div>
        </div>
      ))}
    </div>
  )
}

export default IPCFlow
