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
        std::ifstream ifs{"result.data", std::ios::binary};
        cereal::PortableBinaryInputArchive ar(ifs);
        ar(lvl0_lwe);
    };

    // read the 2 ciphertexts of the result
    std::vector<uint8_t> p;
    {
      std::fstream tfat;
      tfat.open( "ptxt.data", std::ios_base::in | std::ios_base::binary );
      tfat.read( &(p[0]), TFHEpp::lvl1param::n );
      tfat.close();
    };

    // decrypt and print plaintext answer
    std::cout << "Verify RLWE ciphertext is equal to LWE ciphertext." << std::endl;
    std::vector<uint8_t> res = TFHEpp::bootsSymDecrypt<TFHEpp::lvl0param>(lvl0_lwe, sk.key.lvl0);

    std::cout << "-> plaintext      = ";
    for (int i = 0; i < 16; i++) std::cout << static_cast<int>(p[i]);
    std::cout << std::endl;
    std::cout << "-> decrypt result = ";
    for (int i = 0; i < 16; i++) std::cout << static_cast<int>(res[i]);
    std::cout << std::endl;

    for (int i = 0; i < TFHEpp::lvl1param::n; i++) {
      if (static_cast<int>(res[i]) != p[i]) std::cout << i << ": Not matched. ";
    }
}
