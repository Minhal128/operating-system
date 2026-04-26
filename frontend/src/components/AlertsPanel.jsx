import React from 'react'
import './AlertsPanel.css'

function AlertsPanel({ alerts, onClear }) {
  return (
    <div className="alerts-panel">
      <div className="alerts-header">
        <span className="alert-count">{alerts.length} Active Alerts</span>
        {alerts.length > 0 && (
          <button className="clear-btn" onClick={onClear}>Clear All</button>
        )}
      </div>
      
      <div className="alerts-list">
        {alerts.length === 0 ? (
          <div className="no-alerts">
            <div className="check-icon">✓</div>
            <div>No alerts - System operating normally</div>
          </div>
        ) : (
          alerts.slice(-10).reverse().map(alert => (
            <div key={alert.id} className={`alert-item ${alert.severity}`}>
              <div className="alert-icon">⚠️</div>
              <div className="alert-content">
                <div className="alert-message">{alert.message}</div>
                <div className="alert-time">{alert.timestamp}</div>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  )
}

export default AlertsPanel
