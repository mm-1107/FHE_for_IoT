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
    TFHEpp::EvalKey ek;

    // reads the cloud key from file
    {
        const std::string path = "./cloud.key";
        std::ifstream ifs("./cloud.key", std::ios::binary);
        cereal::PortableBinaryInputArchive ar(ifs);
        ek.serialize(ar);
    }
    // import input
    TFHEpp::TRLWE<TFHEpp::lvl1param> ctxt_rlwe;
    {
        std::ifstream ifs{"ctxt_rlwe.data", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(ctxt_rlwe);
    }

    std::cout << "Extracts the coefficients and switch lvl1param to lvl0param." << std::endl;
    TFHEpp::TLWE<TFHEpp::lvl1param> lvl1_lwe;
    std::vector<TFHEpp::TLWE<TFHEpp::lvl0param>> lvl0_lwe(TFHEpp::lvl1param::n);
    double start = get_time_msec();
    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
      // RLWE to LWE
      TFHEpp::SampleExtractIndex<TFHEpp::lvl1param>(lvl1_lwe, ctxt_rlwe, i);
      // Switch lvl1param to lvl0param
      TFHEpp::IdentityKeySwitch<TFHEpp::lvl10param>(lvl0_lwe[i], lvl1_lwe, *ek.iksklvl10);
    }
    std::cout << "time for swiching is " << get_time_msec() - start
              << "[ms]" << std::endl;

    {
        std::ofstream ofs{"lvl0_lwe.data", std::ios::binary};
        cereal::PortableBinaryOutputArchive ar(ofs);
        ar(lvl0_lwe);
    };
    std::cout << "The LWE ciphertext has been serialized." << std::endl;

}
