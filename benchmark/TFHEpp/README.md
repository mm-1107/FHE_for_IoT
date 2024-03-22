# RLWE + Sample Extraction

1. `keygen.cpp`
  - Generate SecretKey and EvalKey
2. `client_rlwe.cpp`
  - Encrypt plaintext by RLWE with lvl1param
3. `cloud.cpp`
  - Extracts the encryption of one of the coefficients as a LWE ciphertext. (RLWE->LWE)
  - Switch lvl1param to lvl0param.
