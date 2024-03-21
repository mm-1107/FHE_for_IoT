# Set CPU architecture
FROM arm64v8/debian:bullseye
RUN apt-get update && apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential \
  g++ libomp-dev cmake git libgoogle-perftools-dev wget unzip autoconf libtool

# Build OpenFHE (cmake with tcmalloc)
WORKDIR /root/
RUN git clone -b experiment https://github.com/i04347/openfhe.git
RUN mkdir /root/openfhe/build
WORKDIR /root/openfhe/build
RUN cmake -DWITH_TCM=ON .. && make tcm && make -j$(($(nproc)/4)) && make install

# Install xbyak_aarch64
WORKDIR /root/
RUN git clone https://github.com/fujitsu/xbyak_aarch64.git
ENV CPLUS_INCLUDE_PATH $CPLUS_INCLUDE_PATH:/root/xbyak_aarch64

# Build TFHEpp (from HE3DB)
WORKDIR /root/
RUN git clone https://github.com/zhouzhangwalker/HE3DB.git
RUN mkdir /root/HE3DB/thirdparty/TFHEpp/build
WORKDIR /root/HE3DB/thirdparty/TFHEpp/build
RUN cmake .. -DUSE_RANDEN=OFF -DUSE_SPQLIOX_AARCH64=ON -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)

# Build benchmark
COPY ./benchmark /benchmark
RUN mkdir /benchmark/build
WORKDIR /benchmark/build
RUN cmake .. && make
