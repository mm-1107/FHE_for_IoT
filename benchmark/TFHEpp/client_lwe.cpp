#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>
#include <chrono>

inline double get_time_msec(void) {
    return static_cast<double>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count()) / 1000000;
}

int main()
{
    // reads the secret key from file
    TFHEpp::SecretKey sk;
    {
        std::ifstream ifs{"secret.key", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        sk.serialize(ar);
    };
    // Generate random inputs
    std::cout << "Generate random inputs." << std::endl;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<uint32_t> binary(0, 1);
    std::uint32_t N = TFHEpp::lvl1param::n;
    std::vector<uint8_t> p(N);
    for (int i = 0; i < N; i++) p[i] = binary(engine);

    // Encrypt the plaintext
    std::cout << "Encrypt N bits with LWE." << std::endl;
    // N-LWE ciphertext
    double start = get_time_msec();
    std::vector<TFHEpp::TLWE<TFHEpp::lvl0param>> ctxt_lwe =
      TFHEpp::bootsSymEncrypt<TFHEpp::lvl0param>(p, sk.key.lvl0);
    std::cout << "time for encryption is " << get_time_msec() - start
              << "[ms]" << std::endl;
    {
        std::ofstream ofs{"ctxt_lwe.data", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(ctxt_lwe);
    };
    std::cout << "The LWE ciphertext has been serialized." << std::endl;
}
