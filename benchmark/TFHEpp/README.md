# RLWE + Sample Extraction

## Setup

### Build TFHEpp

```
$ git clone [this repo]
$ cd FHE_for_IoT
$ docker-compose build
$ docker-compose up -d
```

### Copy binary files

```
$ mkdir bin
$ docker cp tfhe:/root/TFHEpp/build/tutorial/keygen bin/
$ docker cp tfhe:/root/TFHEpp/build/tutorial/client_rlwe bin/
$ docker cp tfhe:/root/TFHEpp/build/tutorial/client_lwe bin/
$ scp bin/* pi@raspberrypi.local:~/  
```

### Run benchmark

```
$ ssh pi@raspberrypi.local
$ ./keygen
$ ./client_rlwe
$ ./client_lwe
```

1. `keygen.cpp`
  - Generate SecretKey and EvalKey
2. `client_rlwe.cpp`
  - Encrypt plaintext by RLWE with lvl1param
3. `cloud.cpp`
  - Extracts the encryption of one of the coefficients as a LWE ciphertext. (RLWE->LWE)
  - Switch lvl1param to lvl0param.
4. `verify.cpp`
  - Verify RLWE ciphertext is equal to LWE ciphertext.


## Experiment
- Size of plaintext is 1024 bit.
- Client: Raspberry Pi 3 Model B+
- Server: Docker on M2 Mac

  | | Encryption [ms] |Ciphertext |Client Memory Usage | Server [ms]|
  | ---- | ---- | ---- | ---- | ---- |
  | LWE |4910.2| 2.5MB |2.8MB| N/A |
  | RLWE+Sample Extraction |49.4| 8.1K |324KB|1106.59|
