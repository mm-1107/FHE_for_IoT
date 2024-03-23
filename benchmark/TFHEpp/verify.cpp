#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <memory>
#include <random>
#include <tfhe++.hpp>
#include <vector>

int main()
{
    // reads the secret key from file
    TFHEpp::SecretKey sk;
    {
        std::ifstream ifs{"secret.key", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        sk.serialize(ar);
    };

    // read the 2 ciphertexts of the result
    std::vector<TFHEpp::TLWE<TFHEpp::lvl0param>> lvl0_lwe;
    {
        std::ifstream ifs{"./lvl0_lwe.data", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(lvl0_lwe);
    };

    // decrypt and print plaintext answer
    std::cout << "Verify RLWE ciphertext is equal to LWE ciphertext." << std::endl;
    std::vector<uint8_t> res = TFHEpp::bootsSymDecrypt<TFHEpp::lvl0param>(lvl0_lwe, sk.key.lvl0);

    std::cout << "-> decrypt result = ";
    for (int i = 0; i < 32; i++) std::cout << static_cast<int>(res[i]);
    std::cout << std::endl;
}
