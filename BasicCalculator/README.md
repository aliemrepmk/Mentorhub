# Simple Qt Calculator

This project is a simple **GUI calculator** built using **C++** and **Qt**, demonstrating basic arithmetic operations and GUI interaction.  
It is organized to support **native builds** and **Dockerized builds** for easier portability and development.

---

## ✨ Features

- Clean, responsive GUI with Qt Widgets
- Lightweight C++ codebase
- Native or Dockerized development support
- Automatic build/run script included (`run.sh`)

---

## 🛠 Requirements

**For Native Build:**
- Ubuntu 20.04+ / MacOS / Linux
- `CMake` ≥ 3.5
- `g++` (or `clang++`)
- `Qt5` libraries (`qt5-default`, `qtdeclarative5-dev`, etc.)

**For Docker Build:**
- Docker installed
- Docker Compose installed
- XQuartz (if using MacOS) for GUI display

---

## 🏗 How to Build and Run

### 🔹 Native Build (without Docker)

```bash
# Install required packages (example for Ubuntu)
sudo apt-get install build-essential cmake qt5-default qtdeclarative5-dev

# Create a build directory
mkdir build
cd build

# Generate Makefiles and build
cmake ..
make

# Run the application
./SimpleCalculator
```

---

### 🔹 Build and Run using Docker

If you prefer an isolated environment, Docker is fully supported.

```bash
# Make the script executable
chmod +x run.sh

# Launch the application (builds automatically)
./run.sh
```

✅ `run.sh` automatically:
- Starts XQuartz (on MacOS)
- Configures display settings
- Builds the image via Docker Compose
- Cleans up after exit

---

## 🐳 Manual Docker Commands (Alternative)

If you prefer manual steps:

```bash
# Build the Docker image
docker build -t simple-calculator .

# Allow X11 access (Mac/Linux)
xhost +

# Run the container
docker run -it --rm \
  -e DISPLAY=$(ipconfig getifaddr en0):0 \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  simple-calculator
```

---

## 📜 Notes

- On **MacOS**, make sure **XQuartz** is installed and running with "**Allow connections from network clients**" enabled.
- Make sure **Docker** is installed and running.
- You may need to run `xhost +` before launching the container.
- For better security, you can later restrict `xhost` access again.