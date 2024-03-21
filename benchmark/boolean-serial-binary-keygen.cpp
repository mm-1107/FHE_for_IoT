//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
//
// All rights reserved.
//
// Author TPOC: contact@openfhe.org
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==================================================================================

/*
  Example for FHEW with binary serialization
 */

#include "binfhecontext-ser.h"

using namespace lbcrypto;

// path where files will be written to
const std::string DATAFOLDER = "demoData";

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

    return 0;
}
