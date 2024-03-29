#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <random>
#include <tfhe++.hpp>
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
    std::array<TFHEpp::lvl1param::T, TFHEpp::lvl1param::n> pmu;
    for (int i = 0; i < N; i++) pmu[i] = p[i] ? TFHEpp::lvl1param::μ : -TFHEpp::lvl1param::μ;

    // Encrypt the plaintext
    std::cout << "Encrypt N bits with RLWE." << std::endl;
    // one-RLWE ciphertext
    double start = get_time_msec();
    TFHEpp::TRLWE<TFHEpp::lvl1param> ctxt_rlwe =
      TFHEpp::trlweSymEncrypt<TFHEpp::lvl1param>(pmu, TFHEpp::lvl1param::α, sk.key.lvl1);
    std::cout << "time for encryption is " << get_time_msec() - start
              << "[ms]" << std::endl;
    {
        std::ofstream ofs{"ctxt_rlwe.data", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(ctxt_rlwe);
    };
    std::cout << "The RLWE ciphertext has been serialized." << std::endl;

    std::cout << "-> plaintext      = ";
    for (int i = 0; i < 32; i++) std::cout << static_cast<int>(p[i]);
    std::cout << std::endl;
}
