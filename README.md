# FHE Implementation for IoT

# Setup

## Build OpenFHE and TFHEpp

```
$ git clone [this repo]
$ cd FHE_for_IoT
$ docker-compose build
$ docker-compose up -d
```

## Copy binary files

```
$ docker cp tfhe:/benchmark/build/tfhe_openfhe .
$ mkdir lib
$ docker cp tfhe:/usr/local/lib/. ./lib/
$ scp ./tfhe_openfhe pi@raspberrypi.local:~/
$ scp lib/lib*.so* pi@raspberrypi.local:~/lib
```

## Run benchmark

```
$ ssh pi@raspberrypi.local
$ export LD_LIBRARY_PATH=/home/pi/lib
$ ./tfhe_openfhe
```

### Heap profile

```
$ sudo apt install valgrind
$ valgrind --tool=massif --time-unit=ms --massif-out-file=output_keyswitchingkey_to_file.massif ./test
```

Visualize on the Laptop

```
(For Mac)
$ sudo port install massif-visualizer
(For Ubuntu)
$ apt install massif-visualizer
$ ms_print output_keyswitchingkey_to_file.massif
$ massif-visualizer output_keyswitchingkey_to_file.massif
```
