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

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/uio.h>

#include "utils/s2n_blob.h"
#include "utils/s2n_result.h"

#define S2N_MIN_STUFFER_GROWTH_IN_BYTES 1024

/* Using a non-zero value
 * (a) makes wiped data easy to see in the debugger
 * (b) makes use of wiped data obvious since this is unlikely to be a valid bit pattern
 */
#define S2N_WIPE_PATTERN 'w'

#define SIZEOF_IN_BITS( t ) (sizeof(t) * CHAR_BIT)

#define SIZEOF_UINT24 3

struct s2n_stuffer {
    /* The data for the s2n_stuffer */
    struct s2n_blob blob;

    /* Cursors to the current read/write position in the s2n_stuffer */
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;

    /* Was this stuffer alloc()'d ? */
    unsigned int alloced:1;

    /* Is this stuffer growable? */
    unsigned int growable:1;

    /* Can this stuffer be safely resized?
     * A growable stuffer can be temporarily tainted by a raw read/write,
     * preventing it from resizing. */
    unsigned int tainted:1;
};

#define s2n_stuffer_data_available( s )   ((s)->write_cursor - (s)->read_cursor)
#define s2n_stuffer_space_remaining( s )  ((s)->blob.size - (s)->write_cursor)
#define s2n_stuffer_is_wiped( s )         ((s)->high_water_mark == 0)
#define s2n_stuffer_is_freed( s )         ((s)->blob.data == NULL)
/* Check basic validity constraints on the stuffer: e.g. that cursors point within the blob */
extern S2N_RESULT s2n_stuffer_validate(const struct s2n_stuffer* stuffer: itype(_Ptr<const struct s2n_stuffer>));

/* Initialize and destroying stuffers */
extern int s2n_stuffer_init(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_blob *in: itype(_Ptr<struct s2n_blob>));
extern int s2n_stuffer_alloc(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t size);
extern int s2n_stuffer_growable_alloc(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t size);
extern int s2n_stuffer_free(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>));
extern int s2n_stuffer_resize(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t size);
extern int s2n_stuffer_resize_if_empty(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t size);
extern int s2n_stuffer_rewind_read(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t size);
extern int s2n_stuffer_reread(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>));
extern int s2n_stuffer_rewrite(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>));
extern int s2n_stuffer_wipe(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>));
extern int s2n_stuffer_wipe_n(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t size);
extern bool s2n_stuffer_is_consumed(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>));

/* Basic read and write */
extern int s2n_stuffer_read(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_blob *out: itype(_Ptr<struct s2n_blob>));
extern int s2n_stuffer_erase_and_read(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_blob *out: itype(_Ptr<struct s2n_blob>));
extern int s2n_stuffer_write(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const struct s2n_blob *in: itype(_Ptr<const struct s2n_blob>));
extern int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint8_t * data: itype(_Ptr<uint8_t>), uint32_t size);
extern int s2n_stuffer_erase_and_read_bytes(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint8_t * data: itype(_Ptr<uint8_t>), uint32_t size);
extern int s2n_stuffer_write_bytes(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint8_t * data: itype(_Ptr<const uint8_t>), const uint32_t size);
extern int s2n_stuffer_writev_bytes(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const struct iovec* iov: itype(_Ptr<const struct iovec>), size_t iov_count, uint32_t offs, uint32_t size);
extern int s2n_stuffer_skip_read(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint32_t n);
extern int s2n_stuffer_skip_write(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t n);

/* Tries to reserve enough space to write n additional bytes into the stuffer.*/
extern int s2n_stuffer_reserve_space(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint32_t n);

/* Raw read/write move the cursor along and give you a pointer you can
 * read/write data_len bytes from/to in-place.
 */
extern void *s2n_stuffer_raw_write(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t data_len);
extern void *s2n_stuffer_raw_read(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint32_t data_len);

/* Send/receive stuffer to/from a file descriptor */
extern int s2n_stuffer_recv_from_fd(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const int rfd, const uint32_t len, uint32_t *bytes_written: itype(_Ptr<uint32_t>) );
extern int s2n_stuffer_send_to_fd(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const int wfd, const uint32_t len, uint32_t *bytes_sent: itype(_Ptr<uint32_t>));

/* Read and write integers in network order */
extern int s2n_stuffer_read_uint8(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint8_t * u : itype(_Ptr<uint8_t>));
extern int s2n_stuffer_read_uint16(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint16_t * u: itype(_Ptr<uint16_t>));
extern int s2n_stuffer_read_uint24(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint32_t * u: itype(_Ptr<uint32_t>));
extern int s2n_stuffer_read_uint32(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint32_t * u: itype(_Ptr<uint32_t>));
extern int s2n_stuffer_read_uint64(struct s2n_stuffer *stuffer : itype(_Ptr<struct s2n_stuffer>), uint64_t * u: itype(_Ptr<uint64_t>));

extern int s2n_stuffer_write_uint8(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint8_t u);
extern int s2n_stuffer_write_uint16(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint16_t u);
extern int s2n_stuffer_write_uint24(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t u);
extern int s2n_stuffer_write_uint32(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint32_t u);
extern int s2n_stuffer_write_uint64(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const uint64_t u);

/* Allocate space now for network order integers that will be written later.
 * These are primarily intended to handle the vector type defined in the RFC:
 * https://tools.ietf.org/html/rfc8446#section-3.4 */
struct s2n_stuffer_reservation {
    struct s2n_stuffer *stuffer : itype(_Ptr<struct s2n_stuffer>);
    uint32_t write_cursor;
    uint8_t length;
};
/* Check basic validity constraints on the s2n_stuffer_reservation: e.g. stuffer validity. */
extern S2N_RESULT s2n_stuffer_reservation_validate(const struct s2n_stuffer_reservation* reservation: itype(_Ptr<const struct s2n_stuffer_reservation>));
extern int s2n_stuffer_reserve_uint16(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer_reservation *reservation: itype(_Ptr<struct s2n_stuffer_reservation>));
extern int s2n_stuffer_reserve_uint24(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer_reservation *reservation: itype(_Ptr<struct s2n_stuffer_reservation>));
extern int s2n_stuffer_write_vector_size(struct s2n_stuffer_reservation* reservation: itype(_Ptr<struct s2n_stuffer_reservation>));

/* Copy one stuffer to another */
extern int s2n_stuffer_copy(struct s2n_stuffer *from: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *to: itype(_Ptr<struct s2n_stuffer>), const uint32_t len);

/* Read and write base64 */
extern int s2n_stuffer_read_base64(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *out: itype(_Ptr<struct s2n_stuffer>));
extern int s2n_stuffer_write_base64(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *in: itype(_Ptr<struct s2n_stuffer>));

/* Useful for text manipulation ... */
#define s2n_stuffer_write_char( stuffer, c )  s2n_stuffer_write_uint8( (stuffer), (uint8_t) (c) )
#define s2n_stuffer_read_char( stuffer, c )  s2n_stuffer_read_uint8( (stuffer), (uint8_t *) (c) )
#define s2n_stuffer_write_str( stuffer, c )  s2n_stuffer_write_bytes( (stuffer), (const uint8_t *) (c), strlen((c)) )
#define s2n_stuffer_write_text( stuffer, c, n )  s2n_stuffer_write_bytes( (stuffer), (const uint8_t *) (c), (n) )
#define s2n_stuffer_read_text( stuffer, c, n )  s2n_stuffer_read_bytes( (stuffer), (uint8_t *) (c), (n) )
extern int s2n_stuffer_read_expected_str(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const char *expected: itype(_Nt_array_ptr<const char>));
extern int s2n_stuffer_peek_char(struct s2n_stuffer *s2n_stuffer: itype(_Ptr<struct s2n_stuffer>), char *c: itype(_Nt_array_ptr<char>));
extern int s2n_stuffer_read_token(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *token: itype(_Ptr<struct s2n_stuffer>), char delim);
extern int s2n_stuffer_read_line(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *token: itype(_Ptr<struct s2n_stuffer>));
extern int s2n_stuffer_peek_check_for_str(struct s2n_stuffer *s2n_stuffer: itype(_Ptr<struct s2n_stuffer>), const char *expected: itype(_Nt_array_ptr<const char>));
extern int s2n_stuffer_skip_whitespace(struct s2n_stuffer *s2n_stuffer: itype(_Ptr<struct s2n_stuffer>), uint32_t *skipped: itype(_Ptr<uint32_t>));
extern int s2n_stuffer_skip_to_char(struct s2n_stuffer *stuffer, char target);
extern int s2n_stuffer_skip_expected_char(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const char expected, const uint32_t min, const uint32_t max, uint32_t *skipped: itype(_Ptr<uint32_t>));
extern int s2n_stuffer_skip_read_until(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const char *target: itype(_Nt_array_ptr<const char>));
extern int s2n_stuffer_alloc_ro_from_string(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), const char *str: itype(_Nt_array_ptr<const char>));
extern int s2n_stuffer_init_ro_from_string(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), uint8_t *data: itype(_Array_ptr<uint8_t>) count(length), uint32_t length);

/* Read a private key from a PEM encoded stuffer to an ASN1/DER encoded one */
extern int s2n_stuffer_private_key_from_pem(struct s2n_stuffer *pem: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *asn1: itype(_Ptr<struct s2n_stuffer>));

/* Read a certificate  from a PEM encoded stuffer to an ASN1/DER encoded one */
extern int s2n_stuffer_certificate_from_pem(struct s2n_stuffer *pem: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *asn1: itype(_Ptr<struct s2n_stuffer>));

/* Read DH parameters om a PEM encoded stuffer to a PKCS3 encoded one */
extern int s2n_stuffer_dhparams_from_pem(struct s2n_stuffer *pem: itype(_Ptr<struct s2n_stuffer>), struct s2n_stuffer *pkcs3: itype(_Ptr<struct s2n_stuffer>));

extern bool s2n_is_base64_char(unsigned char c);

/* Copies all valid data from "stuffer" into "out".
 * The old blob "out" pointed to is freed.
 * It is the responsibility of the caller to free the free "out".
 */
extern int s2n_stuffer_extract_blob(struct s2n_stuffer *stuffer: itype(_Ptr<struct s2n_stuffer>), struct s2n_blob *out: itype(_Ptr<struct s2n_blob>));
