# Set CPU architecture
FROM --platform=linux/arm/v8 ubuntu:22.04
RUN apt-get update && apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential \
  g++ libomp-dev cmake git libgoogle-perftools-dev wget unzip autoconf libtool

# Install profiler
WORKDIR /root/
RUN git clone https://github.com/gperftools/gperftools
WORKDIR /root/gperftools
RUN ./autogen.sh && ./configure && make -j$(($(nproc)/2)) && make install

# Build OpenFHE (cmake with tcmalloc)
COPY ./openfhe /openfhe
RUN mkdir /openfhe/build
WORKDIR /openfhe/build
RUN cmake -DWITH_TCM=ON .. && make tcm && make -j$(($(nproc)/4))

# Install cereal
WORKDIR /root/
RUN wget https://github.com/USCiLab/cereal/archive/v1.3.2.zip
RUN unzip v1.3.2.zip && rm v1.3.2.zip
RUN export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/root/cereal-1.3.2/include"

# Install xbyak_aarch64
WORKDIR /root/
RUN git clone https://github.com/fujitsu/xbyak_aarch64.git
RUN export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/root/xbyak_aarch64"

# Build TFHEpp
COPY ./TFHEpp /TFHEpp
RUN mkdir /TFHEpp/build
WORKDIR /TFHEpp/build
# RUN cmake .. -DENABLE_TEST=ON -DUSE_SPQLIOX_AARCH64=ON -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)
