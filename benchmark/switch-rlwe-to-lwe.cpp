/*
  Example for the sample extraction
 */

#include "binfhecontext-ser.h"

using namespace lbcrypto;

// path where files will be written to
const std::string DATAFOLDER = "demoData";

std::shared_ptr<RLWECiphertextImpl> Encrypt(ConstLWEPrivateKey& sk, ){
  NativeVector sv(sk->GetElement());
  uint32_t N = params->GetN();
  NativeInteger q = params->Getq();
  // ####
  const auto& RGSWParams = params->GetRingGSWParams();
  const auto& polyParams = RGSWParams->GetPolyParams();
  NativePoly skNPoly(polyParams);
  skNPoly.SetValues(skN->GetElement(), Format::COEFFICIENT);
  skNPoly.SetFormat(Format::EVALUATION);


  // ####

  std::vector<NativePoly> res(2);
  NativeVector m(N, q);
  // no need to do NTT as all coefficients of this poly are zero
  res[0] = NativePoly(polyParams, Format::EVALUATION, true);
  res[1] = NativePoly(polyParams, Format::COEFFICIENT, false);
  res[1].SetValues(std::move(m), Format::COEFFICIENT);
  res[1].SetFormat(Format::EVALUATION);

  std::vector<NativePoly> res_rlwe(2);
  res_rlwe[0] = res[0].ToNativePoly();
  res_rlwe[1] = res[1].ToNativePoly();
  return std::make_shared<RLWECiphertextImpl>(std::move(res_rlwe));
}

std::shared_ptr<LWECiphertextImpl> SampleExtractIndex(const std::shared_ptr<LWECryptoParams>& params,
                                                        ConstRLWECiphertext rlwe,
                                                        uint32_t index = 0) {
    uint32_t N = params->GetN();
    uint32_t n = params->Getn();
    NativeInteger mod = params->Getq();
    std::vector<NativePoly>& rlweVec{rlwe->GetElements()};
    NativeVector a(n, mod);
    NativeInteger b = rlweVec[1][index];

    for (size_t i = 0; i <= index; ++i) {
        a[i] = rlweVec[0][index - i];
    }
    for (size_t i = index + 1; i < N; ++i) {
        a[i] = -rlweVec[0][N + index - i];
    }
    auto result = std::make_shared<LWECiphertextImpl>(std::move(a), std::move(b));
    return result;
}

int main() {
    // Generating the crypto context

    auto cc1 = BinFHEContext();

    cc1.GenerateBinFHEContext(STD128);

    std::cout << "Generating keys." << std::endl;

    // Generating the secret key
    auto sk1 = cc1.KeyGen();

    // Generate the bootstrapping keys
    cc1.BTKeyGen(sk1);

    auto params = cc1.GetParams()->GetLWEParams();

    std::cout << "Q: " << params->GetQ() << std::endl;
    std::cout << "N: " << params->GetN() << std::endl;
    std::cout << "q: " << params->Getq() << std::endl;
    std::cout << "n: " << params->Getn() << std::endl;

    std::cout << "Done generating all keys." << std::endl;

    // Serializing key-independent crypto context

    if (!Serial::SerializeToFile(DATAFOLDER + "/cryptoContext.txt", cc1, SerType::BINARY)) {
        std::cerr << "Error serializing the cryptocontext" << std::endl;
        return 1;
    }
    std::cout << "The cryptocontext has been serialized." << std::endl;

    // Serializing refreshing and key switching keys (needed for bootstrapping)

    if (!Serial::SerializeToFile(DATAFOLDER + "/refreshKey.txt", cc1.GetRefreshKey(), SerType::BINARY)) {
        std::cerr << "Error serializing the refreshing key" << std::endl;
        return 1;
    }
    std::cout << "The refreshing key has been serialized." << std::endl;

    if (!Serial::SerializeToFile(DATAFOLDER + "/ksKey.txt", cc1.GetSwitchKey(), SerType::BINARY)) {
        std::cerr << "Error serializing the switching key" << std::endl;
        return 1;
    }
    std::cout << "The key switching key has been serialized." << std::endl;

    // Serializing private keys

    if (!Serial::SerializeToFile(DATAFOLDER + "/sk1.txt", sk1, SerType::BINARY)) {
        std::cerr << "Error serializing sk1" << std::endl;
        return 1;
    }
    std::cout << "The secret key sk1 key been serialized." << std::endl;

    std::cout << "Encrypt N bit with RLWE" << std::endl;
    // TODO
    uint32_t N = params->GetN();
    LWEPrivateKey skN = LWEscheme->KeyGen(params->GetN(), params->GetQ());
    std::vector<uint8_t> p(N);
    auto ctxt_rlwe = Encrypt(skN, p);

    std::cout << "Switch one-RLWE to N-LWE" << std::endl;
    auto ksk = cc1.GetSwitchKey();
    std::vector<std::shared_ptr<LWECiphertextImpl>> LWEciphertexts(N);
    // Sample Extract and KeySwitch
    for (uint32_t i = 0; i < N; ++i) {
        auto ctxt_lwe = SampleExtractIndex(params, ctxt_rlwe, i);
        LWEciphertexts[i] = cc1.GetLWEScheme()->KeySwitch(params, ksk, ctxt_lwe);
    }

    return 0;
}
