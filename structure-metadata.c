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

// The warnings don't actually stop compilation though they make it
// harders to see other errors....
//
// gcc -Wno-overflow -Wno-int-conversion -Wno-incompatible-pointer-types foo.c && ./a.out 

enum FieldType {
  FIELD_TYPE_SIGNED_BIT_FIELD,
  FIELD_TYPE_UNSIGNED_BIT_FIELD,
  FIELD_TYPE_SIGNED_INTEGER,
  FIELD_TYPE_UNSIGNED_INTEGER,
  FIELD_TYPE_FLOATING_POINT,
  FIELD_TYPE_POINTER,
  FIELD_TYPE_ARRAY,
  FIELD_TYPE_STRUCTURE,
  FIELD_TYPE_UNION
};

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

// Forward Declarations. In the future it will be possible to swap
// these out with your own functions.

void define_structure_start(char *struct_name,
                            int size, 
                            int alignment);

void define_structure_end(char *struct_name);

void define_structure_field(int field_type,
                            char *struct_name,
                            char *field_name,
                            int start_bit,
                            int end_bit);
                            
#define BYTE_PTR(ptr) ((unsigned char *) ptr)

#define PTR_OFFSET(ptr_base, ptr_field)                                        \
   (int)(BYTE_PTR(ptr_field) - BYTE_PTR(ptr_base))


#define DEFINE_STRUCTURE_START(struct_name)                                    \
  current_struct_name = #struct_name;                                          \
  struct_name struct_name##__instance;                                         \
  define_structure_start(current_struct_name,                                  \
                         sizeof(struct_name##__instance),                      \
                         __alignof__(struct_name##__instance))

#define DEFINE_STRUCTURE_END(struct_name)                                      \
  if (strcmp(current_struct_name, #struct_name) != 0) {                        \
     printf("Mismatched DEFINE_STRUCTURE_END\n");                              \
     exit(1);                                                                  \
  }                                                                            \
  define_structure_end(current_struct_name);                                   \
  clear_renames();                                                             \
  current_struct_name = NULL;

// This is used for UNSIGNED_BIT_FIELD and SIGNED_BIT_FIELD
#define _BIT_FIELD(struct_name, field_name, field_type)                        \
  memset(&struct_name##__instance, 0, sizeof(struct_name));                    \
  struct_name##__instance.field_name = (long long)-1;                          \
  define_structure_field(field_type, #struct_name, #field_name,                \
    start_bit(&struct_name##__instance, sizeof(struct_name)),                  \
    end_bit(&struct_name##__instance, sizeof(struct_name))                     \
  )

#define _NORMAL_FIELD(struct_name, field_name, field_type)                     \
  define_structure_field(field_type, #struct_name, #field_name,                \
    8 * PTR_OFFSET(&struct_name##__instance,                                   \
                   &struct_name##__instance.field_name),                       \
                         8 * (PTR_OFFSET(&struct_name##__instance,             \
                                         &struct_name##__instance.field_name)  \
                              + sizeof(&struct_name##__instance.field_name)))

#define UNSIGNED_BIT_FIELD(struct_name, field_name) \
  _BIT_FIELD(struct_name, field_name, FIELD_TYPE_UNSIGNED_BIT_FIELD)

#define SIGNED_BIT_FIELD(struct_name, field_name) \
  _BIT_FIELD(struct_name, field_name, FIELD_TYPE_SIGNED_BIT_FIELD)

#define SIGNED_INTEGER_FIELD(struct_name, field_name) \
  _BIT_FIELD(struct_name, field_name, FIELD_TYPE_SIGNED_INTEGER)

#define UNSIGNED_INTEGER_FIELD(struct_name, field_name) \
  _BIT_FIELD(struct_name, field_name, FIELD_TYPE_UNSIGNED_INTEGER)

#define FLOATING_POINT_FIELD(struct_name, field_name) \
  _NORMAL_FIELD(struct_name, field_name, FIELD_TYPE_FLOATING_POINT)

#define POINTER_FIELD(struct_name, field_name) \
  _NORMAL_FIELD(struct_name, field_name, FIELD_TYPE_POINTER)

#define ARRAY_FIELD(struct_name, field_name) \
  _NORMAL_FIELD(struct_name, field_name, FIELD_TYPE_ARRAY)

#define STRUCTURE_FIELD(struct_name, field_name) \
  _NORMAL_FIELD(struct_name, field_name, FIELD_TYPE_STRUCTURE)

#define UNION_FIELD(struct_name, field_name) \
  _NORMAL_FIELD(struct_name, field_name, FIELD_TYPE_UNION)
  
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

struct structure_field_metadata {
  int field_type;
  char* field_name;
  int start_bit;
  int end_bit;
};

struct structure_metadata {
  char* original_name;
  char* name;
  int size;
  int alignment;
  int num_fields;
  struct structure_field_metadata fields[256];
};

//
// Structure name and field name "renaming".
//

// In part we are just too lazy to write code for the default case. We
// want to first convert from CamelCase to "-" (snake) case. Then we
// can drop "-t" if it appears at the end. Instead 

int number_of_renames = 0;

struct renaming {
  char* original_structure_name;
  char* structure_name;
  char* original_field_name;
  char* field_name;
};

struct renaming renamings[4096];

void internal_rename(char* original_structure_name,
            char* structure_name,
            char* original_field_name,
            char* field_name) {
  renamings[number_of_renames].original_structure_name = original_structure_name;
  renamings[number_of_renames].structure_name = structure_name;
  renamings[number_of_renames].original_field_name = original_field_name;
  renamings[number_of_renames].field_name = field_name;
  number_of_renames++;
}

void clear_renames() {
  number_of_renames = 0;
}

#define RENAME_STRUCTURE(original_name, new_name) \
  internal_rename(#original_name, new_name, 0, 0)

#define RENAME_FIELD(original_name, original_field_name, field_name)  \
  internal_rename(#original_name, 0, #original_field_name, field_name)
  

char* rename_structure_name(char *name) {
  /*
  if (strcmp(name, "Foo_t") == 0) {
    return "foo";
  }
  if (strcmp(name, "timespec_t") == 0) {
    return "timespec";
  }
  if (strcmp(name, "stat_t") == 0) {
    return "os-file-status";
  }
  */

  for (int i = 0; i < number_of_renames; i++) {
    if (strcmp(renamings[i].original_structure_name, name) == 0
        && renamings[i].original_field_name == NULL) {
      return renamings[i].structure_name;
    }
  }

  return name;
}

char* rename_field_name(char *struct_name, char *field_name) {

  /*

  if (strcmp(struct_name, "timespec_t") == 0) {
    if (strcmp(field_name, "tv_sec") == 0) {
      return "seconds";
    }
    if (strcmp(field_name, "tv_nsec") == 0) {
      return "nanos";
    }
  }
  */

  for (int i = 0; i < number_of_renames; i++) {
    if (strcmp(renamings[i].original_structure_name, struct_name) == 0
        && renamings[i].original_field_name != NULL
        && strcmp(renamings[i].original_field_name, field_name) == 0) {
      return renamings[i].field_name;
    }
  }

  return field_name;
}

//
// Generate an expression for a load or store of a "finite" sized
// quantity.
//
// TODO(jawilson): signed loads, more optimal loading of bitfields.
//

char* generate_unsigned_load(int start_bit, int end_bit) {
  char *buffer = malloc(1024);
  int size = end_bit - start_bit;

  if (size > 64) {
    printf("Size is too large to load as a 64bit unsigned integer");
    exit(1);
  }

  if (size == 8 && ((start_bit % 8) == 0)) {
    sprintf(buffer, "($load-8 ($add ptr %d))", start_bit / 8);
  } else if (size == 16 && ((start_bit % 16) == 0)) {
    sprintf(buffer, "($load-16 ($add ptr %d))", start_bit / 8);
  } else if (size == 32 && ((start_bit % 32) == 0)) {
    sprintf(buffer, "($load-32 ($add ptr %d))", start_bit / 8);
  } else if (size == 64 && ((start_bit % 64) == 0)) {
    sprintf(buffer, "($load-64 ($add ptr %d))", start_bit / 8);
  } else {
    sprintf(buffer, "(load-unsigned-bit-field ptr %d %d)", start_bit, end_bit);
  }
  return buffer;
}

char* generate_store(int start_bit, int end_bit, char* value) {
  char *buffer = malloc(1024);
  int size = end_bit - start_bit;
  if (size == 8 && ((start_bit % 8) == 0)) {
    sprintf(buffer, "($store-8 ($add ptr %d) %s)", start_bit / 8, value);
  } else if (size == 16 && ((start_bit % 16) == 0)) {
    sprintf(buffer, "($store-16 ($add ptr %d) %s)", start_bit / 8, value);
  } else if (size == 32 && ((start_bit % 32) == 0)) {
    sprintf(buffer, "($store-32 ($add ptr %d) %s)", start_bit / 8, value);
  } else if (size == 64 && ((start_bit % 64) == 0)) {
    sprintf(buffer, "($store-64 ($add ptr %d) %s)", start_bit / 8, value);
  } else {
    sprintf(buffer, "(store-bit-field ptr %d %d %s)", start_bit, end_bit, value);
  }
  return buffer;
}

//
// These are meant to be redefined later to actually generate code for
// various languages/purposes.
//

struct structure_metadata metadata;

void define_structure_start(char *struct_name,
                            int size, 
                            int alignment) {
  memset(&metadata, 0, sizeof(metadata));
  metadata.original_name = struct_name;
  metadata.name = rename_structure_name(struct_name);
  metadata.size = size;
  metadata.alignment = alignment;
  if (0) {
    printf("structure name=%s size=%d alignment=%d\n", struct_name, size, alignment);
  }
}


// Delay generation until we know all the fields. This will allow us
// to create "print" routines for an entire structure based on it's
// fields.

void define_structure_end(char *struct_name) {
  printf("\n\n");
  printf(";;; ======================================================================\n");
  printf(";;; %s (Linux name %s)\n", metadata.name, metadata.original_name);
  printf(";;; ======================================================================\n\n");

  printf("($define-function (make-%s $returns pointer) (malloc %d))\n", 
         metadata.name, metadata.size);
  printf("($define-function (size-of-%s $returns uint64) %d)\n", 
         metadata.name, metadata.size);
  printf("($define-function (free-%s (ptr pointer) $returns void) (free ptr))\n", 
         metadata.name, metadata.size);
  printf("($define-function (clear-%s (ptr pointer) $returns void) (memset ptr 0 %d))\n", 
         metadata.name, metadata.size);
  for (int i = 0; (i < metadata.num_fields); i++) {
    struct structure_field_metadata field = metadata.fields[i];

    printf("($define-function (%s:load-%s (ptr pointer) $returns uint64)\n  %s)\n",
           metadata.name, 
           field.field_name,
           generate_unsigned_load(field.start_bit, field.end_bit));

    printf("($define-function (%s:store-%s (ptr pointer) (value uint64) $returns uint64)\n  %s)\n",
           metadata.name, 
           field.field_name,
           generate_store(field.start_bit, field.end_bit, "value"));

    if (is_field_type_addressable(field.field_type)) {
      printf("($define-function (%s:address-of-%s (ptr pointer) $returns uint64)\n  ($add ptr %d))\n",
             metadata.name, 
             field.field_name,
             field.start_bit / 8);
    }
  }

  // TODO(jawilson): STRUCTURE-NAME:clear-FIELD-NAME
}

void define_structure_field(int field_type,
                            char *struct_name,
                            char *field_name,
                            int start_bit,
                            int end_bit) {
  if (0) {
    printf("    %s=%s start=%d end=%d\n", field_type_to_string(field_type), field_name, start_bit, end_bit);
  }
  struct structure_field_metadata field_md = {
    .field_type = field_type,
    .field_name = rename_field_name(struct_name, field_name),
    .start_bit = start_bit,
    .end_bit = end_bit,
  };
  metadata.fields[metadata.num_fields++] = field_md;
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
