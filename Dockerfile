# NLM Development Docker Container
# Ubuntu-based container with all development tools

FROM ubuntu:22.04

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=1000

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    bash \
    build-essential \
    cmake \
    curl \
    git \
    pkg-config \
    wget \
    unzip \
    xz-utils \
    software-properties-common \
    python3 \
    python3-pip \
    python3-dev \
    clang \
    clangd \
    lldb \
    valgrind \
    gdb \
    doxygen \
    graphviz \
    make \
    g++ \
    gcc \
    libboost-all-dev \
    libeigen3-dev \
    libhdf5-dev \
    libopenmpi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libpng-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libwebp-dev \
    libgtk-3-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libatlas-base-dev \
    gfortran \
    && rm -rf /var/lib/apt/lists/*

# Create user
RUN groupadd -g ${USER_GID} ${USERNAME} && useradd -m -s /bin/bash -u ${USER_UID} -g ${USER_GID} ${USERNAME}

# Switch to user
USER ${USERNAME}

# Install Python packages
RUN pip3 install --no-cache-dir --upgrade pip \
    scikit-build-core \
    pybind11 \
    numpy \
    pytest \
    pytest-cov \
    coverage \
    black \
    flake8 \
    mypy \
    types-setuptools \
    sphinx \
    sphinx-rtd-theme \
    breathe \
    exhale \
    jupyter \
    ipykernel \
    ipython \
    matplotlib \
    seaborn \
    pandas \
    scipy \
    scikit-learn \
    nbsphinx \
    recommonmark \
    docutils \
    jsonschema \
    typing-extensions \
    functools32

# Set up Python kernel for Jupyter
RUN python3 -m pip install ipykernel
python3 -m ipykernel install --user --name=nlm-env

# Install C++ linters and formatters
RUN curl -L https://cpcli.io/install | bash -s -- --user "${USERNAME}" \
    "clang-format-latest" \
    "clang-tidy-latest" \
    "cppcheck-latest"

# Clone NLM repository (for development)
WORKDIR /workspace
RUN git clone https://github.com/nlm-project/nlm.git .

# Set up development environment
RUN mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
RUN cd build && make -j$(nproc)

# Set up git user
RUN git config --global user.name "NLM Developer" && git config --global user.email "developer@nlm-project.org"

# Install additional tools for development
RUN pip3 install --no-cache-dir \
    ninja-build \
    cmake-format \
    cmakelang \
    pre-commit \
    tox \
    virtualenv \
    poetry \
    pipenv \
    setuptools-rust \
    maturin

# Set up shell
RUN echo "export PATH=/home/${USERNAME}/.local/bin:$PATH" >> /home/${USERNAME}/.bashrc

# Install VS Code extensions via command line
RUN code --install-extension ms-vscode.cpptools && \
    code --install-extension ms-vscode.cmake-tools && \
    code --install-extension james-yu.hello-world-py && \
    code --install-extension bierner.markdown-preview-github-styles

# Set working directory
WORKDIR /workspace