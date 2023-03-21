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

char *field_type_to_string(int value) {
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

int is_field_type_addressable(int value) {
  switch (value) {
  case FIELD_TYPE_SIGNED_BIT_FIELD: return 0;
  case FIELD_TYPE_UNSIGNED_BIT_FIELD: return 0;
  }
  return 1;
}

int get_bit(unsigned char* bytes, int bit_number) {
  unsigned char b = bytes[bit_number >> 3];
  return ((b >> (bit_number & 7)) & 1);
}

int start_bit(unsigned char* bytes, int max_bytes) {
  for (int i = 0; i < max_bytes * 8; i++) {
    if (get_bit(bytes, i)) {
      return i;
    }
  }
  printf("ERROR: First set bit not found!");
  exit(1);
}

int end_bit(unsigned char* bytes, int max_bytes) {
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

// This is a demo struct and can go away once we have enough coverage
// from real structures.

struct Foo {
  int bitfield0 : 3;
  unsigned int bitfield1 : 1;
  int bar;
  long baz;
  long *qux;
  double boof;
  long some_array[5];
};

// If the structure isn't typedef'd, like the one above, we need to do
// that now otherwise our macros won't work. This obviously changes
// the name of the structure but many code generators will want to
// want to strip off _t and make other renamings anyways.

typedef struct Foo Foo_t;

// Finally we are ready to start generating "minimal" functions for
// important Linux/Unix data-structures.

#include <time.h>
#include <sys/stat.h>

typedef struct timespec timespec_t;
typedef struct stat stat_t;

void main() {
  char *current_struct_name = NULL;

  // TODO(jawilson): remove once all of these combinations are done.
  DEFINE_STRUCTURE_START(Foo_t);
  {
    SIGNED_BIT_FIELD(Foo_t, bitfield0);
    UNSIGNED_BIT_FIELD(Foo_t, bitfield1);
    UNSIGNED_INTEGER_FIELD(Foo_t, bar);
    UNSIGNED_INTEGER_FIELD(Foo_t, baz);
    POINTER_FIELD(Foo_t, qux);
    FLOATING_POINT_FIELD(Foo_t, boof);
    ARRAY_FIELD(Foo_t, some_array);
  }
  DEFINE_STRUCTURE_END(Foo_t);

  // ======================================================================
  // timespec (Linux name timespec_t)
  // ======================================================================

  RENAME_STRUCTURE(timespec_t, "timespec");
  RENAME_FIELD(timespec_t, tv_sec, "seconds");
  RENAME_FIELD(timespec_t, tv_nsec, "nanos");

  DEFINE_STRUCTURE_START(timespec_t);
  {
    UNSIGNED_INTEGER_FIELD(timespec_t, tv_sec);
    UNSIGNED_INTEGER_FIELD(timespec_t, tv_nsec);
  }
  DEFINE_STRUCTURE_END(timespec_t);

  // ======================================================================
  // os-file-status (Linux name stat_t)
  // ======================================================================

  RENAME_STRUCTURE(stat_t, "os-file-status");
  RENAME_FIELD(stat_t, st_dev, "device-id");
  RENAME_FIELD(stat_t, st_ino, "inode-number");
  RENAME_FIELD(stat_t, st_mode, "mode");
  RENAME_FIELD(stat_t, st_nlink, "hardlink-count");
  RENAME_FIELD(stat_t, st_uid, "user-id");
  RENAME_FIELD(stat_t, st_gid, "group-id");
  RENAME_FIELD(stat_t, st_rdev, "special-file-device-id");
  RENAME_FIELD(stat_t, st_size, "size");
  RENAME_FIELD(stat_t, st_blksize, "file-system-block-size");
  RENAME_FIELD(stat_t, st_blocks, "number-of-blocks");
  RENAME_FIELD(stat_t, st_atim, "access-time");
  RENAME_FIELD(stat_t, st_mtim, "modification-time");
  RENAME_FIELD(stat_t, st_ctim, "modification-status-change-time");

  DEFINE_STRUCTURE_START(stat_t);
  {
    UNSIGNED_INTEGER_FIELD(stat_t, st_dev);
    UNSIGNED_INTEGER_FIELD(stat_t, st_ino);
    UNSIGNED_INTEGER_FIELD(stat_t, st_mode);
    UNSIGNED_INTEGER_FIELD(stat_t, st_nlink);
    UNSIGNED_INTEGER_FIELD(stat_t, st_uid);
    UNSIGNED_INTEGER_FIELD(stat_t, st_gid);
    UNSIGNED_INTEGER_FIELD(stat_t, st_rdev);
    UNSIGNED_INTEGER_FIELD(stat_t, st_size);
    UNSIGNED_INTEGER_FIELD(stat_t, st_blksize);
    UNSIGNED_INTEGER_FIELD(stat_t, st_blocks);
    STRUCTURE_FIELD(stat_t, st_atim);
    STRUCTURE_FIELD(stat_t, st_mtim);
    STRUCTURE_FIELD(stat_t, st_ctim);
  }
  DEFINE_STRUCTURE_END(stat_t);

  // ======================================================================

  if (current_struct_name != NULL) {
    printf("ERROR: missing DEFINE_STRUCTURE_END\n");
    exit(1);
  }

  exit(0);
}
