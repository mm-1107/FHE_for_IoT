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
    // Deserializing the cryptocontext

    BinFHEContext cc;
    if (Serial::DeserializeFromFile(DATAFOLDER + "/cryptoContext.txt", cc, SerType::BINARY) == false) {
        std::cerr << "Could not deserialize the cryptocontext" << std::endl;
        return 1;
    }
    std::cout << "The cryptocontext has been deserialized." << std::endl;

    // deserializing the refreshing and switching keys (for bootstrapping)

    // RingGSWACCKey refreshKey;
    // if (Serial::DeserializeFromFile(DATAFOLDER + "/refreshKey.txt", refreshKey, SerType::BINARY) == false) {
    //     std::cerr << "Could not deserialize the refresh key" << std::endl;
    //     return 1;
    // }
    // std::cout << "The refresh key has been deserialized." << std::endl;
    //
    // LWESwitchingKey ksKey;
    // if (Serial::DeserializeFromFile(DATAFOLDER + "/ksKey.txt", ksKey, SerType::BINARY) == false) {
    //     std::cerr << "Could not deserialize the switching key" << std::endl;
    //     return 1;
    // }
    // std::cout << "The switching key has been deserialized." << std::endl;
    //
    // // Loading the keys in the cryptocontext
    // cc.BTKeyLoad({refreshKey, ksKey});

    // Deserializing the secret key

    LWEPrivateKey sk;
    if (Serial::DeserializeFromFile(DATAFOLDER + "/sk1.txt", sk, SerType::BINARY) == false) {
        std::cerr << "Could not deserialize the secret key" << std::endl;
        return 1;
    }
    std::cout << "The secret key has been deserialized." << std::endl;

    // OPERATIONS WITH DESERIALIZED KEYS AND CIPHERTEXTS
    // double start = get_time_msec();
    auto ct1 = cc.Encrypt(sk, 1);
    // std::cout << "Encryption time[ms] is " << get_time_msec() - start << std::endl;

    // Serializing a ciphertext

    if (!Serial::SerializeToFile(DATAFOLDER + "/ct1.txt", ct1, SerType::BINARY)) {
        std::cerr << "Error serializing ct1" << std::endl;
        return 1;
    }
    std::cout << "A ciphertext has been serialized." << std::endl;

    return 0;
}
