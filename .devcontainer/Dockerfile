FROM ubuntu:24.04

ARG USER=user

# receive user id from devcontainer.json    
ARG USER_UID=1000
ARG USER_GID=1000

ENV DEBIAN_FRONTEND=noninteractive

# misc tools
RUN apt update -y && apt install -y \
    clangd \
    clang-format \
    sudo \
    locales \
    doxygen \
    git \
    build-essential \
    make \
    pkg-config \
    cmake \
    ninja-build \
    yq \
    jq

# Install core build tools and networking utilities
RUN apt update && apt install -y \
    build-essential \
    g++ \
    cmake \
    iproute2 \
    iputils-ping \
    net-tools \
    tcpdump \
    libpcap-dev \
    sudo \
    gdb \
    valgrind \
    && apt clean

# optional cross compile to windows    
RUN apt update -y && apt install -y \
    gcc-mingw-w64-x86-64 \
    g++-mingw-w64-x86-64 

RUN locale-gen en_GB.UTF-8 && update-locale

# Create user with specific UID/GID to match host user
# Handle existing ubuntu user/group by reassigning them to different IDs first
RUN if getent passwd ubuntu >/dev/null; then \
        usermod -u 2000 ubuntu; \
    fi \
    && if getent group ubuntu >/dev/null; then \
        groupmod -g 2000 ubuntu; \
    fi \
    && groupadd --gid $USER_GID $USER \
    && useradd --uid $USER_UID --gid $USER_GID -m $USER \
    && echo "$USER:$USER" | chpasswd \
    && adduser $USER sudo

# Allow members of sudo group to execute any command without password
RUN echo "$USER            ALL = (ALL) NOPASSWD: ALL" >> /etc/sudoers

RUN printf "alias ll='ls -la'" >> /home/$USER/.bashrc

CMD /bin/bash