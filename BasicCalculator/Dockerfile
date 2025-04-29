# --- Stage 1: Build Stage ---
FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install compilers, cmake, Qt5 libraries
RUN apt-get update && apt-get install -y \
    build-essential cmake \
    qt5-default qtdeclarative5-dev \
    libx11-xcb1 libxcb1 libx11-dev x11-xserver-utils \
    && apt-get clean

# Set working directory
WORKDIR /app

# Copy all project files
COPY . .

# Enable Qt's AUTOMOC
RUN mkdir build && cd build && cmake .. -DCMAKE_AUTOMOC=ON && make

# --- Stage 2: Runtime Stage ---
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install only Qt runtime libraries (NO compilers)
RUN apt-get update && apt-get install -y \
    qt5-default \
    qtdeclarative5-dev \
    libx11-xcb1 libxcb1 libx11-dev x11-xserver-utils \
    && apt-get clean

# Set working directory
WORKDIR /app

# Copy only built binary from builder
COPY --from=builder /app/build/SimpleCalculator .

# Set default command
CMD ["./SimpleCalculator"]    