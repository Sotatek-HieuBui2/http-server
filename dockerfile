# Sử dụng base image Ubuntu
FROM ubuntu:22.04

# Cài các gói cần thiết
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    libssl-dev \
    libhttp-parser-dev \
    libpthread-stubs0-dev \
    pkg-config \
    ninja-build \
    meson \
    && rm -rf /var/lib/apt/lists/*

# Cài Pistache từ source
WORKDIR /tmp
RUN git clone https://github.com/pistacheio/pistache.git && \
    cd pistache && \
    meson setup build && \
    meson compile -C build && \
    meson install -C build

# Copy mã nguồn app của bạn
WORKDIR /app
COPY . .

# Build project
RUN cmake -Bbuild -H. && cmake --build build

# Expose port
EXPOSE 9080

# Run server
CMD ["./build/http_server"]
