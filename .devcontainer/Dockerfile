# ==========================================================
# Modern WebGPU / Dawn Development Environment
# Ubuntu 24.04 + NVIDIA + Vulkan + C++23
# ==========================================================

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# ----------------------------------------------------------
# Base setup + NVIDIA repositories
# ----------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    gnupg \
    ca-certificates \
    curl \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && \
    dpkg -i cuda-keyring_1.1-1_all.deb && \
    rm cuda-keyring_1.1-1_all.deb && \
    apt-get update

# ----------------------------------------------------------
# Core build tools and Python (fixed)
# ----------------------------------------------------------
RUN apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    clangd \
    clang-format \
    clang-tidy \
    g++-13 \
    gcc-13 \
    gdb \
    lldb \
    liblldb-dev \
    ccache \
    make \
    git \
    pkg-config \
    ninja-build \
    unzip \
    zip \
    vim \
    less \
    sudo \
    && apt-get clean && \
    rm -rf /var/lib/apt/lists/*    

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    python3-pip \
    python3-full \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# ----------------------------------------------------------
# Latest CMake
# ----------------------------------------------------------
RUN wget https://github.com/Kitware/CMake/releases/download/v3.30.9/cmake-3.30.9-linux-x86_64.sh \
    && mkdir -p /opt/cmake \
    && sh cmake-3.30.9-linux-x86_64.sh --prefix=/opt/cmake --skip-license \
    && ln -sf /opt/cmake/bin/cmake /usr/local/bin/cmake \
    && rm cmake-3.30.9-linux-x86_64.sh

# ----------------------------------------------------------
# Vulkan & graphics libs
# ----------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    libvulkan1 \
    vulkan-tools \
    vulkan-validationlayers \
    libvulkan-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    mesa-common-dev \
    mesa-vulkan-drivers \
    spirv-tools \
    spirv-headers \
    glslang-tools \
    nodejs \
    npm \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# ----------------------------------------------------------
# Node + WebGPU JS types (for host testing)
# ----------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends nodejs npm && \
    npm install -g @webgpu/types && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# ----------------------------------------------------------
# Default compilers and environment
# ----------------------------------------------------------
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

ENV CC=gcc
ENV CXX=g++
ENV NVIDIA_VISIBLE_DEVICES=all
ENV NVIDIA_DRIVER_CAPABILITIES=graphics,utility,compute

# ----------------------------------------------------------
# Optional non-root developer user
# ----------------------------------------------------------
ARG USER_NAME=developer
RUN useradd -m -s /bin/bash -G sudo ${USER_NAME} && \
    echo "${USER_NAME} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# drop to non-root user
USER ${USER_NAME}
WORKDIR /home/${USER_NAME}

ENV SHELL=/bin/bash
ENTRYPOINT ["/bin/bash"]
