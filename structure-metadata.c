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
// harders to see other errors.
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

// Forward Declarations

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
  char* name;
  int size;
  int alignment;
  int num_fields;
  struct structure_field_metadata fields[256];
};

// I'm too lazy to write the default code. We want to first convert
// from CamelCase to "snake" case. Then we can drop "_t" if it appears
// at the end.
char* convert_structure_name(char *name) {
  if (strcmp(name, "Foo_t") == 0) {
    return "foo";
  }
  if (strcmp(name, "timespec_t") == 0) {
    return "timespec";
  }
  return name;
}

char* generate_unsigned_load(int start_bit, int end_bit) {
  char *buffer = malloc(1024);
  int size = end_bit - start_bit;
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
    sprintf(buffer, "(store-32 ($add ptr %d) %s)", start_bit / 8, value);
  } else if (size == 64 && ((start_bit % 64) == 0)) {
    sprintf(buffer, "(store-64 ($add ptr %d) %s)", start_bit / 8, value);
  } else {
    sprintf(buffer, "(store-bit-field ptr %d %d %s)", start_bit, end_bit, value);
  }
  return buffer;
}

// These are meant to be redefined later to actually generate code for
// various languages.

struct structure_metadata metadata;

void define_structure_start(char *struct_name,
                            int size, 
                            int alignment) {
  memset(&metadata, 0, sizeof(metadata));
  metadata.name = convert_structure_name(struct_name);
  metadata.size = size;
  metadata.alignment = alignment;
  printf("structure name=%s size=%d alignment=%d\n", struct_name, size, alignment);
}

void define_structure_end(char *struct_name) {
  // This method is useful to generate code once all fields are known.
  printf("(define-function (make-%s $returns pointer) (malloc %d))\n", metadata.name, metadata.size);
  printf("(define-function (free-%s (ptr pointer) $returns void) (free ptr))\n", metadata.name, metadata.size);
  printf("(define-function (size-of-%s $returns uint64) %d)\n", metadata.name, metadata.size);
  for (int i = 0; (i < metadata.num_fields); i++) {
    struct structure_field_metadata field = metadata.fields[i];

    printf("(define-function (%s:load-%s (ptr pointer) $returns uint64)\n  %s)\n",
           metadata.name, 
           field.field_name,
           generate_unsigned_load(field.start_bit, field.end_bit));

    printf("(define-function (%s:store-%s (ptr pointer) (value uint64) $returns uint64)\n  %s)\n",
           metadata.name, 
           field.field_name,
           generate_store(field.start_bit, field.end_bit, "value"));

    if (is_field_type_addressable(field.field_type)) {
      printf("(define-function (%s:address-of-%s (ptr pointer) $returns uint64)\n  ($add ptr %d))\n",
             metadata.name, 
             field.field_name,
             field.start_bit / 8);
    }
  }

  // TODO(jawilson): clear-STRUCTURE-NAME
  // TODO(jawilson): clear-FIELD-NAME
}

void define_structure_field(int field_type,
                            char *struct_name,
                            char *field_name,
                            int start_bit,
                            int end_bit) {
  printf("    %s=%s start=%d end=%d\n", field_type_to_string(field_type), field_name, start_bit, end_bit);
  struct structure_field_metadata field_md = {
    .field_type = field_type,
    .field_name = field_name,
    .start_bit = start_bit,
    .end_bit = end_bit,
  };
  metadata.fields[metadata.num_fields++] = field_md;
}

//
// This is just sample code.
// 

// This could of course come from a header file.
struct Foo {
  int bitfield0 : 3;
  unsigned int bitfield1 : 1;
  int bar;
  long baz;
  long *qux;
  double boof;
  long some_array[5];
};

// If the structure isn't typedef'd, we need to do that now. This
// obviously changed the name of the structure but many code
// generators will want to want to strip off _t anyways...

typedef struct Foo Foo_t;

#include <time.h>

typedef struct timespec timespec_t;

void main() {
  char *current_struct_name = NULL;

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

  DEFINE_STRUCTURE_START(timespec_t);
  {
    UNSIGNED_INTEGER_FIELD(timespec_t, tv_sec);
    UNSIGNED_INTEGER_FIELD(timespec_t, tv_nsec);
  }
  DEFINE_STRUCTURE_END(timespec_t);

  if (current_struct_name != NULL) {
    printf("ERROR: missing DEFINE_STRUCTURE_END\n");
    exit(1);
  }

  exit(0);
}
