// MD5.CC - source code for the C++/object oriented translation and
//          modification of MD5.

// Translation and modification (c) 1995 by Mordechai T. Abzug

// This translation/ modification is provided "as is," without express or
// implied warranty of any kind.

// The translator/ modifier does not claim (1) that MD5 will do what you think
// it does; (2) that this translation/ modification is accurate; or (3) that
// this software is "merchantible."  (Language for this disclaimer partially
// copied from the disclaimer below).

/* based on:

   MD5.H - header file for MD5C.C
   MDDRIVER.C - test driver for MD2, MD4 and MD5

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/
#pragma once

#include <stdio.h>
#include <fstream>
#include <iostream>

class MD5 {

public:
// methods for controlled operation:
  MD5              ();  // simple initializer
  void  update     (const unsigned char *input, unsigned int input_length);
  void  update     (std::istream& stream);
  void  update     (FILE *file);
  void  update     (std::ifstream& stream);
  void  finalize   ();

// constructors for special circumstances.  All these constructors finalize
// the MD5 context.
  MD5              (unsigned char *string); // digest string, finalize
  MD5              (std::istream& stream);       // digest stream, finalize
  MD5              (FILE *file);            // digest file, close, finalize
  MD5              (std::ifstream& stream);      // digest stream, close, finalize

// methods to acquire finalized result
  unsigned char    *raw_digest ();  // digest as a 16-byte binary array
  char *            hex_digest ();  // digest as a 33-byte ascii-hex string
  friend std::ostream&   operator<< (std::ostream&, MD5 context);



private:

// next, the private data:
	uint32_t state[4];
	uint32_t count[2];  // number of *bits*, mod 2^64
	uint8_t buffer[64]; // input buffer
	uint8_t digest[16];
	uint8_t finalized;

	// last, the private methods, mostly static:
	void init();                           // called by all constructors
	void transform(const uint8_t* buffer); // does the real update work.  Note
	                                       // that length is implied to be 64.

	static void encode(uint8_t* dest, const uint32_t* src, uint32_t length);
	static void decode(uint32_t* dest, const uint8_t* src, uint32_t length);

	static inline uint32_t rotate_left(uint32_t x, uint32_t n);
	static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);
	static inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s,
	                      uint32_t ac);
	static inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s,
	                      uint32_t ac);
	static inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s,
	                      uint32_t ac);
	static inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s,
	                      uint32_t ac);

};
