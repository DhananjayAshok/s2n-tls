/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include "crypto/s2n_evp.h"

#define S2N_MAX_DIGEST_LEN SHA512_DIGEST_LENGTH

typedef enum {
    S2N_HASH_NONE=0,
    S2N_HASH_MD5,
    S2N_HASH_SHA1,
    S2N_HASH_SHA224,
    S2N_HASH_SHA256,
    S2N_HASH_SHA384,
    S2N_HASH_SHA512,
    S2N_HASH_MD5_SHA1,
    /* Don't add any hash algorithms below S2N_HASH_SENTINEL */
    S2N_HASH_SENTINEL
} s2n_hash_algorithm;

/* The low_level_digest stores all OpenSSL structs that are alg-specific to be used with OpenSSL's low-level hash API's. */
union s2n_hash_low_level_digest {
    MD5_CTX md5;
    SHA_CTX sha1;
    SHA256_CTX sha224;
    SHA256_CTX sha256;
    SHA512_CTX sha384;
    SHA512_CTX sha512;
    struct {
        MD5_CTX md5;
        SHA_CTX sha1;
    } md5_sha1;
};

/* The evp_digest stores all OpenSSL structs to be used with OpenSSL's EVP hash API's. */
struct s2n_hash_evp_digest {
    struct s2n_evp_digest evp;
    /* Always store a secondary evp_digest to allow resetting a hash_state to MD5_SHA1 from another alg. */
    struct s2n_evp_digest evp_md5_secondary;
};

/* s2n_hash_state stores the s2n_hash implementation being used (low-level or EVP),
 * the hash algorithm being used at the time, and either low_level or high_level (EVP) OpenSSL digest structs.
 */
struct s2n_hash_state {
    const struct s2n_hash *hash_impl: itype(_Ptr<const struct s2n_hash>);
    s2n_hash_algorithm alg;
    uint8_t is_ready_for_input;
    uint64_t currently_in_hash;
    union {
        union s2n_hash_low_level_digest low_level;
        struct s2n_hash_evp_digest high_level;
    } digest;
};

/* The s2n hash implementation is abstracted to allow for separate implementations, using
 * either OpenSSL's low-level algorithm-specific API's or OpenSSL's EVP API's.
 */
struct s2n_hash {
    int (*alloc) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>) );
    int (*allow_md5_for_fips) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
    int (*init) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), s2n_hash_algorithm alg);
    int (*update) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), const void *data: itype(_Ptr<const void>), uint32_t size);
    int (*digest) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), void *out: itype(_Ptr<void>), uint32_t size);
    int (*copy) (struct s2n_hash_state *to: itype(_Ptr<struct s2n_hash_state>), struct s2n_hash_state *from: itype(_Ptr<struct s2n_hash_state>));
    int (*reset) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
    int (*free) (struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
};

extern int s2n_hash_digest_size(s2n_hash_algorithm alg, uint8_t *out: itype(_Ptr<uint8_t>));
extern int s2n_hash_block_size(s2n_hash_algorithm alg, uint64_t *block_size: itype(_Ptr<uint64_t>));
extern bool s2n_hash_is_available(s2n_hash_algorithm alg);
extern int s2n_hash_is_ready_for_input(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
extern int s2n_hash_new(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
S2N_RESULT s2n_hash_state_validate(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
extern int s2n_hash_allow_md5_for_fips(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
extern int s2n_hash_init(struct s2n_hash_state *state, s2n_hash_algorithm alg);
extern int s2n_hash_update(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), const void *data: itype(_Ptr<const void>), uint32_t size);
extern int s2n_hash_digest(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), void *out: itype(_Ptr<void>), uint32_t size);
extern int s2n_hash_copy(struct s2n_hash_state *to: itype(_Ptr<struct s2n_hash_state>), struct s2n_hash_state *from: itype(_Ptr<struct s2n_hash_state>));
extern int s2n_hash_reset(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
extern int s2n_hash_free(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>));
extern int s2n_hash_get_currently_in_hash_total(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), uint64_t *out: itype(_Ptr<uint64_t>));
extern int s2n_hash_const_time_get_currently_in_hash_block(struct s2n_hash_state *state: itype(_Ptr<struct s2n_hash_state>), uint64_t *out: itype(_Ptr<uint64_t>));
