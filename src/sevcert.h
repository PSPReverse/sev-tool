/* ************************************************************************
 * Copyright 2018 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/

#ifndef SEVCERT_H
#define SEVCERT_H

#include "sevapi.h"
#include <string>
#include <openssl/evp.h>

// Public global functions
static std::string sev_empty = "NULL";
void print_sev_cert_readable(const SEV_CERT *cert,
                             std::string& out_str = sev_empty);
void print_sev_cert_hex(const SEV_CERT *cert);
void print_cert_chain_buf_readable(const SEV_CERT_CHAIN_BUF *p,
                                   std::string& out_str = sev_empty);
void print_cert_chain_buf_hex(const SEV_CERT_CHAIN_BUF *p);
void read_priv_key_pem_into_rsakey(const std::string& file_name,
                                   RSA **rsa_priv_key);
void read_priv_key_pem_into_eckey(const std::string& file_name,
                                  EC_KEY **ec_priv_key);
bool read_priv_key_pem_into_evpkey(const std::string file_name,
                                   EVP_PKEY **evp_priv_key);
bool write_pub_key_pem(const std::string& file_name, EVP_PKEY *evp_key_pair);
bool write_priv_key_pem(const std::string& file_name, EVP_PKEY *evp_key_pair);

class SEVCert {
private:
    bool calc_hash_digest(const SEV_CERT *cert,
                          uint32_t pub_key_algo,
                          uint32_t pub_key_offset,
                          HMACSHA256 *sha_digest_256,
                          HMACSHA512 *sha_digest_384);
    SEV_ERROR_CODE validate_usage(uint32_t Usage);
    SEV_ERROR_CODE validate_rsa_pub_key(const SEV_CERT *cert,
                                        const EVP_PKEY *PublicKey);
    SEV_ERROR_CODE validate_public_key(const SEV_CERT *cert,
                                       const EVP_PKEY *PublicKey);
    SEV_ERROR_CODE validate_signature(const SEV_CERT *child_cert,
                                      const SEV_CERT *parent_cert,
                                      EVP_PKEY *parent_signing_key);
    SEV_ERROR_CODE validate_body(const SEV_CERT *cert);

    SEV_CERT m_child_cert;

public:
    SEVCert(SEV_CERT& cert) { m_child_cert = cert; }
    ~SEVCert() {};

    const SEV_CERT *data() { return &m_child_cert; }

    bool generate_ecdh_key_pair(EVP_PKEY **evp_key_pair);
    bool create_godh_cert(EVP_PKEY **godh_key_pair,
                          uint8_t api_major,
                          uint8_t api_minor);
    bool create_oca_cert(EVP_PKEY **oca_key_pair,
                         uint8_t api_major,
                         uint8_t api_minor);
    bool sign_with_key(uint32_t Version, uint32_t pub_key_usage,
                       uint32_t pub_key_algorithm, EVP_PKEY **priv_key,
                       uint32_t sig1_usage, uint32_t sig1_algo);
    SEV_ERROR_CODE compile_public_key_from_certificate(const SEV_CERT *cert,
                                                       EVP_PKEY *evp_pub_key);
    SEV_ERROR_CODE decompile_public_key_into_certificate(SEV_CERT *cert,
                                                         EVP_PKEY *evp_pubkey);
    SEV_ERROR_CODE verify_sev_cert(const SEV_CERT *parent_cert1,
                                   const SEV_CERT *parent_cert2 = NULL);
};

#endif /* sevcert_h */
