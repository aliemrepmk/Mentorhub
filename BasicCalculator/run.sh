#!/bin/bash

# Enable TCP access in XQuartz
echo "Configuring XQuartz for TCP access"
defaults write org.xquartz.X11 nolisten_tcp -bool false

# Start XQuartz if not running
open -a XQuartz
sleep 2 # Wait for XQuartz to initialize

# Allow *any* client to connect (less secure, but works across VM boundaries)
xhost +

# Get local IP address
IP=$(ipconfig getifaddr en0)

if [ -z "$IP" ]; then
  echo "Could not detect IP from en0"
  exit 1
fi

echo "Using DISPLAY=$IP:0"
export DISPLAY=$IP:0

# Run Docker Compose
docker-compose up --build

# Cleanup
echo "🧹 Shutting down Docker Compose"
docker-compose down

# Optional: revoke xhost access (safe cleanup)
xhost -