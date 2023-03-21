//
// This program determines metadata about a structure (or union) such
// as the total size of the structure and the offset and size of each
// field. From this is should be possible to generate code, for
// example a generalized print routine written in C or C++ or code in
// another language that allows programatic access to the fields of
// this struture, i.e., part of a FFI.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structure-metadata.h"

// The warnings don't actually stop compilation though they make it
// harders to see other errors....
//
// gcc -Wno-overflow -Wno-int-conversion -Wno-incompatible-pointer-types foo.c && ./a.out 

extern char *field_type_to_string(int value) {
  switch (value) {
  case FIELD_TYPE_SIGNED_BIT_FIELD: return "FIELD_TYPE_SIGNED_BIT_FIELD";
  case FIELD_TYPE_UNSIGNED_BIT_FIELD: return "FIELD_TYPE_UNSIGNED_BIT_FIELD";
  case FIELD_TYPE_SIGNED_INTEGER: return "FIELD_TYPE_SIGNED_INTEGER";
  case FIELD_TYPE_UNSIGNED_INTEGER: return "FIELD_TYPE_UNSIGNED_INTEGER";
  case FIELD_TYPE_FLOATING_POINT: return "FIELD_TYPE_FLOATING_POINT";
  case FIELD_TYPE_POINTER: return "FIELD_TYPE_POINTER";
  case FIELD_TYPE_ARRAY: return "FIELD_TYPE_ARRAY";
  case FIELD_TYPE_STRUCTURE: return "FIELD_TYPE_STRUCTURE";
  case FIELD_TYPE_UNION: return "FIELD_TYPE_UNION";
  }
  printf("ERROR: field type out of range.\n");
  exit(1);
}

extern int is_field_type_addressable(int value) {
  switch (value) {
  case FIELD_TYPE_SIGNED_BIT_FIELD: return 0;
  case FIELD_TYPE_UNSIGNED_BIT_FIELD: return 0;
  }
  return 1;
}

extern int get_bit(unsigned char* bytes, int bit_number) {
  unsigned char b = bytes[bit_number >> 3];
  return ((b >> (bit_number & 7)) & 1);
}

extern int start_bit(unsigned char* bytes, int max_bytes) {
  for (int i = 0; i < max_bytes * 8; i++) {
    if (get_bit(bytes, i)) {
      return i;
    }
  }
  printf("ERROR: First set bit not found!");
  exit(1);
}

extern int end_bit(unsigned char* bytes, int max_bytes) {
  for (int i = 0; i < max_bytes * 8; i++) {
    if (get_bit(bytes, i)) {
      for (int j = i; j < max_bytes * 8; j++) {
        if (get_bit(bytes, j) == 0) {
          return j;
        }
      }
      return max_bytes * 8;
    }
  }
  printf("ERROR: First set bit not found!");
  exit(1);
}
