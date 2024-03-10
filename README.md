# FHE Implementation for IoT

# Setup

## Build OpenFHE and TFHEpp

```
$ git clone --recursive [this repo]
$ cd fhe_for_iot
$ docker-compose build
$ docker-compose up -d
```

## Copy binary files

```
$ docker cp tfhe:/openfhe/build/bin/examples/binfhe/pke/encode_encrypt_gperf .
$ mkdir lib && docker cp tfhe:/openfhe/build/lib/. ./lib/
$ docker cp tfhe:/usr/local/lib/. ./lib/
$ scp ./encode_encrypt_gperf pi@raspberrypi.local:~/
$ scp -r lib pi@raspberrypi.local:~/
```

## Run benchmark

```
$ ssh pi@raspberrypi.local
$ export LD_LIBRARY_PATH=/home/pi/lib
$ ./encode_encrypt_gperf
```
