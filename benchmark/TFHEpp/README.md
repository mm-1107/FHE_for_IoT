# RLWE + Sample Extraction

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
- Docker on M2 Mac


  | | Encryption [ms] |Ciphertext |Client Memory Usage | Server [ms]|
  | ---- | ---- | ---- | ---- | ---- |
  | RLWE+Sample Extraction |1.70| 8.1K |324KB|1106.59|
  | LWE |166.84| 2.5MB |2.8MB| N/A |
