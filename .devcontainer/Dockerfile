FROM debian:bookworm-slim

# Avoid interactive apt dialogs
ENV DEBIAN_FRONTEND=noninteractive

# --- System setup ---
RUN apt update && apt upgrade -y && apt install -y --no-install-recommends \
    clang \
    clang-tools \
    cmake \
    ccache \
    make \
    build-essential \
    python3 \
    bash \
    git \
    nodejs \
    npm \
    sudo \
    coreutils \
    findutils \
    util-linux \
    ca-certificates \
    curl \
    lldb \
    gdb \
 && apt clean \
 && rm -rf /var/lib/apt/lists/*

# --- Set up clang as default compiler ---
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100 \
 && cc --version && c++ --version

# --- Install Emscripten SDK ---
WORKDIR /opt
RUN git clone https://github.com/emscripten-core/emsdk.git \
 && cd emsdk \
 && ./emsdk install latest \
 && ./emsdk activate latest

# --- Environment setup ---
ENV EMSDK=/opt/emsdk
ENV EMSDK_NODE=/opt/emsdk/node/latest/bin
ENV EMSDK_UPSTREAM=/opt/emsdk/upstream
ENV EMSCRIPTEN=/opt/emsdk/upstream/emscripten
ENV PATH="${EMSDK}:${EMSDK_NODE}:${EMSCRIPTEN}:${EMSDK_UPSTREAM}/bin:${PATH}"

# Preload environment for all shells
RUN echo "source /opt/emsdk/emsdk_env.sh > /dev/null 2>&1 || true" >> /etc/bash.bashrc
RUN echo "source /opt/emsdk/emsdk_env.sh > /dev/null 2>&1 || true" >> /etc/profile
ENV PATH="/opt/emsdk:/opt/emsdk/upstream/emscripten:/opt/emsdk/upstream/bin:/opt/emsdk/node/latest/bin:${PATH}"

# --- Optional: create non-root user for VSCode DevContainer ---
ARG USER_NAME
ENV USER_NAME=${USER_NAME:-root}
# RUN useradd -ms /bin/bash ${USER_NAME} \
#  && usermod -aG sudo ${USER_NAME} \
#  && echo "${USER_NAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USER_NAME} \
#  && chmod 0440 /etc/sudoers.d/${USER_NAME}

USER ${USER_NAME}
WORKDIR /home/${USER_NAME}

ENV SHELL=/bin/bash

ENTRYPOINT ["/bin/bash"]
