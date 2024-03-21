#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>

int main()
{
    // generate a random key
    std::cout << "Generating keys." << std::endl;
    std::unique_ptr<TFHEpp::SecretKey> sk =
        std::make_unique<TFHEpp::SecretKey>();
    TFHEpp::EvalKey ek;
    ek.emplacebkfft<TFHEpp::lvl01param>(*sk);
    ek.emplaceiksk<TFHEpp::lvl10param>(*sk);

    // Get RLWE key
    TFHEpp::Key<TFHEpp::lvl10param::domainP> sk_N =
      sk->key.get<TFHEpp::lvl10param::domainP>();
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

    // Generate random inputs
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint32_t> binary(0, 1);
    std::uint32_t N = TFHEpp::lvl10param::domainP::n;
  	// unsigned char plaintext[128];
  	// for(int i = 0; i < 128; i++) plaintext[i] = U8dist(engine);
    // std::array<typename TFHEpp::lvl1param::T, TFHEpp::lvl1param::n> pmu;
    // for (int i = 0; i < 128; i++) for(int j = 0; j < 8; j++) pmu[i*8+j] = (plaintext[i] >> j) & 1;
    // const TFHEpp::Polynomial<TFHEpp::lvl1param> plainpoly = {binary(engine)};
    std::array<typename TFHEpp::lvl1param::T, TFHEpp::lvl1param::n> plainpoly;
    // one-RLWE ciphertext
    TFHEpp::TRLWE<TFHEpp::lvl1param> ctxt_rlwe =
      TFHEpp::trlweSymEncrypt<TFHEpp::lvl1param>(plainpoly, TFHEpp::lvl1param::α, sk->key.lvl1);

    // N-LWE ciphertext
    std::vector<TFHEpp::TLWE<TFHEpp::lvl1param>> ctxt_lwe(N);
    for (int i = 0; i < N; i++) TFHEpp::SampleExtractIndex<TFHEpp::lvl1param>(ctxt_lwe[i], ctxt_rlwe, i);

    // export the ciphertexts to a file
    // {
    //     std::ofstream ofs{"cloud.data", std::ios::binary};
    //     cereal::PortableBinaryOutputArchive ar(ofs);
    //     ar(ciphertext);
    // };
}
