# Set CPU architecture
FROM arm64v8/debian:bullseye
RUN apt-get update && apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential \
  g++ libomp-dev cmake git libgoogle-perftools-dev wget unzip autoconf libtool


# Build TFHEpp and checkout c++17 version
WORKDIR /root/
RUN git clone https://github.com/virtualsecureplatform/TFHEpp --recursive
WORKDIR /root/TFHEpp
RUN git checkout 0bfd980
WORKDIR /root/TFHEpp/thirdparty
RUN git clone https://github.com/fujitsu/xbyak_aarch64.git
WORKDIR /root/TFHEpp/thirdparty/xbyak_aarch64
RUN make -j$(nproc) && make install
COPY ./benchmark/TFHEpp/*.* /root/TFHEpp/tutorial/
WORKDIR /root/TFHEpp
RUN mkdir /root/TFHEpp/build
WORKDIR /root/TFHEpp/build
RUN cmake .. -DUSE_RANDEN=OFF -DUSE_SPQLIOX_AARCH64=ON \
  -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_TUTORIAL=ON && make -j$(nproc)


# Build OpenFHE (cmake with tcmalloc)
# WORKDIR /root/
# RUN git clone https://github.com/i04347/openfhe.git
# RUN mkdir /root/openfhe/build
# WORKDIR /root/openfhe/build
# RUN cmake -DWITH_TCM=ON .. && make tcm && make -j$(($(nproc)/4)) && make install

# Build benchmark
# COPY ./benchmark /benchmark
# RUN mkdir /benchmark/build
# WORKDIR /benchmark/build
# RUN cmake .. && make
