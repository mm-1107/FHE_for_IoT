#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>
#include <cassert>
int main()
{
    // generate a random key
    std::cout << "Generating keys." << std::endl;
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    TFHEpp::EvalKey ek;
    ek.emplacebkfft<TFHEpp::lvl01param>(*sk);
    ek.emplaceiksk<TFHEpp::lvl10param>(*sk);

    // export the secret key to file for later use
    {
        std::ofstream ofs{"secret.key", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        sk->serialize(ar);
    };

    // export the cloud key to a file (for the cloud)
    {
        std::ofstream ofs{"cloud.key", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ek.serialize(ar);
    };
    std::cout << "Done generating all keys." << std::endl;

    // Generate random inputs
    std::cout << "Generate random inputs." << std::endl;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint8_t> U8dist(0, UINT8_MAX);
    std::uint32_t N = TFHEpp::lvl1param::n;
  	unsigned char plaintext[128];
  	for(int i = 0; i < 128; i++) plaintext[i] = U8dist(engine);
    std::array<bool, TFHEpp::lvl1param::n> p;
    for (int i = 0; i < 128; i++) for(int j = 0; j < 8; j++) p[i*8+j] = (plaintext[i] >> j) & 1;
    // for (int i = 0; i < N; i++) p[i] = 0&1;
    // p[0] = 1&1; p[5] = 1&1;
    std::array<TFHEpp::lvl1param::T, TFHEpp::lvl1param::n> pmu;
    for (int i = 0; i < N; i++) pmu[i] = p[i] ? TFHEpp::lvl1param::μ : -TFHEpp::lvl1param::μ;

    std::cout << "Encrypt N bits with RLWE." << std::endl;
    // one-RLWE ciphertext
    TFHEpp::TRLWE<TFHEpp::lvl1param> ctxt_rlwe =
      TFHEpp::trlweSymEncrypt<TFHEpp::lvl1param>(pmu, TFHEpp::lvl1param::α, sk->key.lvl1);

    std::array<bool, TFHEpp::lvl1param::n> p2 = TFHEpp::trlweSymDecrypt<TFHEpp::lvl1param>(ctxt_rlwe, sk->key.lvl1);
    for (int i = 0; i < 32; i++) std::cout << p[i];
    std::cout << std::endl;
    for (int i = 0; i < 32; i++) std::cout << p2[i];
    std::cout << std::endl;

    std::cout << "Extracts the encryption of one of the coefficients as a LWE ciphertext." << std::endl;
    // N-LWE ciphertext
    std::vector<TFHEpp::TLWE<TFHEpp::lvl1param>> lvl1_lwe(N);
    for (int i = 0; i < N; i++) TFHEpp::SampleExtractIndex<TFHEpp::lvl1param>(lvl1_lwe[i], ctxt_rlwe, i);

    std::vector<uint8_t> res = TFHEpp::bootsSymDecrypt<TFHEpp::lvl1param>(lvl1_lwe, sk->key.lvl1);
    for (int i = 0; i < N; i++) {
      std::cout << res[i];
      // if (res[i] != p[i]) std::cout << "Not matched. ";
    }

    std::cout << "Switch lvl1param to lvl0param." << std::endl;
    // Switch lvl1param to lvl0param
    std::vector<TFHEpp::TLWE<TFHEpp::lvl0param>> lvl0_lwe(N);
    for (int i = 0; i < N; i++) TFHEpp::IdentityKeySwitch<TFHEpp::lvl10param>(lvl0_lwe[i], lvl1_lwe[i], *ek.iksklvl10);

    std::cout << "Verify RLWE ciphertext is equal to LWE ciphertext." << std::endl;
    // decrypt and print plaintext answer
    // std::vector<uint8_t> res = TFHEpp::bootsSymDecrypt<TFHEpp::lvl0param>(lvl0_lwe, sk->key.lvl0);
    // for (int i = 0; i < N; i++) {
    //   std::cout << res[i];
    //   std::cout << p[i];
    //   // if (res[i] != p[i]) {
    //   //   std::cout << "Not matched. ";
    //   // }
    // }
    // std::cout << std::endl;
}
