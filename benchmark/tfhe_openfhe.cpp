#include "binfhecontext.h"
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <gperftools/profiler.h>
using namespace lbcrypto;

int main() {
    double total_time = get_time_msec();
    // Sample Program: Step 1: Set CryptoContext
    auto cc = BinFHEContext();
    cc.GenerateBinFHEContext(STD128);

    // Sample Program: Step 2: Key Generation
    //std::cout << "Generate a rondom key." << std::endl;
    double sk_start = get_time_msec();
    // Generate the secret key
    auto sk = cc.KeyGen();
    //auto keypair = cc.KeyGenPair();
    std::cout << "time for generating sk is " << get_time_msec() - sk_start << std::endl;

    //std::cout << "Generating the bootstrapping keys..." << std::endl;
    ProfilerStart("gatekey.prof");
    double gk_start = get_time_msec();
    // Generate the bootstrapping keys (refresh and switching keys)
    //RingGSWBTKey gk = cc.BTKeyGen(sk);
    cc.BTKeyGen(sk);
    std::cout << "time for generating gk is " << get_time_msec() - gk_start << std::endl;
    ProfilerStop();

    std::cout << "Completed the key generation. It is " << get_time_msec() - total_time << std::endl;

    // Sample Program: Step 3: Create the to-be-evaluated funciton and obtain its corresponding LUT
    // int p = cc.GetMaxPlaintextSpace().ConvertToInt();  // Obtain the maximum plaintext space
    // std::cout << "maximum plaintext space is " << p << std::endl;

    const int vector_size = 100;
    std::vector<LWECiphertext> cipher(vector_size);
    double encryption_start = get_time_msec();
    for (int i = 0; i < vector_size; i++) {
        cipher[i] = cc.Encrypt(sk, 1, FRESH);
    }
    std::cout << "time for encryption is " << get_time_msec() - encryption_start << std::endl;

    std::cout << "Finish encryption. It is " << get_time_msec() - total_time << std::endl;


    return 0;
}
