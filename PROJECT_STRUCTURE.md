# Real-Time Intrusion Detection and Response System

## Project Structure
```
edr-system/
├── backend/
│   ├── src/
│   │   ├── core/
│   │   │   ├── system_calls.h/cpp
│   │   │   ├── file_handler.h/cpp
│   │   │   ├── process_manager.h/cpp
│   │   │   ├── memory_manager.h/cpp
│   │   │   ├── ipc_manager.h/cpp
│   │   ├── monitoring/
│   │   │   ├── monitor.h/cpp
│   │   │   ├── fft_engine.h/cpp
│   │   │   ├── anomaly_detector.h/cpp
│   │   ├── simulation/
│   │   │   ├── attack_simulator.h/cpp
│   │   ├── utils/
│   │   │   ├── logger.h/cpp
│   │   │   ├── performance.h/cpp
│   │   └── main.cpp
│   ├── CMakeLists.txt
│   └── build.sh
├── frontend/
│   ├── src/
│   │   ├── components/
│   │   ├── App.jsx
│   │   └── index.jsx
│   ├── package.json
│   └── public/
└── README.md
```
