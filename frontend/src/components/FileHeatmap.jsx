import React from 'react'
import './FileHeatmap.css'

function FileHeatmap({ fileOps, mode }) {
  const intensity = Math.min(fileOps / 10, 10)
  const cells = Array.from({ length: 100 }, (_, i) => {
    const cellIntensity = Math.random() * intensity
    return {
      id: i,
      intensity: cellIntensity,
      color: mode === 'ATTACK' && cellIntensity > 7 ? 'red' : 'blue'
    }
  })

  return (
    <div className="heatmap-grid">
      {cells.map(cell => (
        <div
          key={cell.id}
          className="heatmap-cell"
          style={{
            opacity: cell.intensity / 10,
            backgroundColor: cell.color === 'red' ? '#ff4a4a' : '#4a9eff'
          }}
        />
      ))}
    </div>
  )
}

export default FileHeatmap
