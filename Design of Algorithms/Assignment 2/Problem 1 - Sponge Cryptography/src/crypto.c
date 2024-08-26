/* 
crypto.c

Implementations for cryptography primatives and functions
  making use of them.

Skeleton written by Aidan Dang for COMP20007 Assignment 2 2022
  with Minor modifications by Grady Fitzpatrick
  implementation by <Edwin Zhu>
*/
#include <crypto.h>
#include <sponge.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The sponge's rate, in bytes. This represents the maximum number of bytes
// which can be read from or written the start of the sponge's state before a
// permutation must occur.
#define RATE 16
// Delimiter byte value used after absorption of the message
#define DELIMITER_A 0xAD
// Delimiter byte used at the end of the last-used block
#define DELIMITER_B 0X77

// Helpful min function that might be useful.
uint64_t min(uint64_t a, uint64_t b) { return a < b ? a : b; }

// implements the keying phase 
void keying_phase(sponge_t *sponge, uint8_t const *key, uint64_t key_len, uint64_t *remainder);

// implements the absorbing phase
void absorb_phase(sponge_t *sponge, uint8_t const *msg, uint64_t msg_len, 
                  char method, uint8_t *output, uint64_t *remaining);

// implements the demarcation phase
void demarcate_phase(sponge_t *sponge, uint64_t remainder);

// implements the squeezing phase
void squeeze_phase(sponge_t *sponge, uint64_t tag_len, uint8_t *tag);

// finds part of the decrypted text during decryption
void plaintext_write(sponge_t *sponge, uint8_t *plaintext, uint8_t const *ciphertext, 
                     uint64_t index, uint64_t len);


void hash(uint8_t *output, uint64_t output_len, uint8_t const *msg,
          uint64_t msg_len) {
  sponge_t sponge; 
  sponge_init(&sponge);

  uint64_t remainder;

  absorb_phase(&sponge, msg, msg_len, 'h', output, &remainder);
  demarcate_phase(&sponge, remainder);
  squeeze_phase(&sponge, output_len, output);
}

void mac(uint8_t *tag, uint64_t tag_len, uint8_t const *key, uint8_t const *msg,
         uint64_t msg_len) {
  sponge_t sponge;
  sponge_init(&sponge);

  uint64_t remainder;

  keying_phase(&sponge, key, CRYPTO_KEY_SIZE, &remainder);
  absorb_phase(&sponge, msg, msg_len, 'h', tag, &remainder);
  demarcate_phase(&sponge, remainder);
  squeeze_phase(&sponge, tag_len, tag);
}


void auth_encr(uint8_t *ciphertext, uint8_t *tag, uint64_t tag_len,
               uint8_t const *key, uint8_t const *plaintext,
               uint64_t text_len) {
  sponge_t sponge;
  sponge_init(&sponge);

  uint64_t remainder;

  keying_phase(&sponge, key, CRYPTO_KEY_SIZE, &remainder);
  absorb_phase(&sponge, plaintext, text_len, 'e', ciphertext, &remainder);
  demarcate_phase(&sponge, remainder);
  squeeze_phase(&sponge, tag_len, tag);
}

int auth_decr(uint8_t *plaintext, uint8_t const *key, uint8_t const *ciphertext,
              uint64_t text_len, uint8_t const *tag, uint64_t tag_len) {
  sponge_t sponge;
  sponge_init(&sponge);

  // fill in elements to check with tag at the end
  uint8_t test_tag[tag_len];

  uint64_t remainder;

  keying_phase(&sponge, key, CRYPTO_KEY_SIZE, &remainder);
  absorb_phase(&sponge, ciphertext, text_len, 'd', plaintext, &remainder);
  demarcate_phase(&sponge, remainder);
  squeeze_phase(&sponge, tag_len, test_tag);

  // check if tag is valid
  uint64_t i;
  for (i = 0; i < tag_len; i++) {
    if (test_tag[i] != tag[i]) {
      return 1;
    }
  }

  return 0;
}


void keying_phase(sponge_t *sponge, uint8_t const *key, uint64_t key_len, uint64_t *remainder) {
  absorb_phase(sponge, key, key_len, 'h', NULL, remainder);
}


void absorb_phase(sponge_t *sponge, uint8_t const *msg, uint64_t msg_len, 
                  char method, uint8_t *output, uint64_t *remaining) {
  uint64_t index = 0;
  uint64_t remainder;

  // decryption
  if (method == 'd') {
    // absorbing majority
    while (index <= msg_len - RATE) {
      // find part of the decrypted message corresponding to the current index
      plaintext_write(sponge, output, msg, index, RATE);

      // override current rate of the sponge with current part of the ciphertext
      sponge_write(sponge, msg + index, RATE, false);

      sponge_permute(sponge);
      index += RATE;
    }

    // absorbing remainder
    remainder = msg_len - index;
    plaintext_write(sponge, output, msg, index, remainder);
    sponge_write(sponge, msg + index, remainder, false); 
  }

  // encryption
  else if (method == 'e') {
    // absorbing majority
    while (index <= msg_len - RATE) {
      sponge_write(sponge, msg + index, RATE, true);

      // read from sponge into ciphertext
      sponge_read(output + index, sponge, RATE);

      sponge_permute(sponge);
      index += RATE;
    }

    // absorbing remainder
    remainder = msg_len - index;
    sponge_write(sponge, msg + index, remainder, true);
    sponge_read(output + index, sponge, remainder);
  }

  // hash or MAC
  else if (method == 'h') {
    // absorbing majority
    while (index <= msg_len - RATE) {
      sponge_write(sponge, msg + index, RATE, true);
      sponge_permute(sponge);
      index += RATE;
    }

    // absorbing remainder
    remainder = msg_len - index;
    sponge_write(sponge, msg + index, remainder, true);
  }

  *remaining = remainder;  
}


void demarcate_phase(sponge_t *sponge, uint64_t remainder) {
  sponge_demarcate(sponge, remainder, DELIMITER_A);
  sponge_demarcate(sponge, RATE - 1, DELIMITER_B);
  sponge_permute(sponge);
}


void squeeze_phase(sponge_t *sponge, uint64_t tag_len, uint8_t *tag) {
  uint64_t index = 0;

  while (index < tag_len - RATE) {
    sponge_read(tag + index, sponge, RATE);
    sponge_permute(sponge);
    index += RATE;
  }

  uint64_t remainder = tag_len - index;
  sponge_read(tag + index, sponge, remainder);
}


void plaintext_write(sponge_t *sponge, uint8_t *plaintext, uint8_t const *ciphertext, 
                     uint64_t index, uint64_t len) {
  uint64_t i; 

  for (i = 0; i < len; i++) {
      plaintext[index + i] = ciphertext[index + i] ^ (sponge->state)[i];
  }
}                    