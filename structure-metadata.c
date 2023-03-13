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
  FIELD_TYPE_BIT_FIELD,
  FIELD_TYPE_SIGNED_INTEGER,
  FIELD_TYPE_UNSIGNED_INTEGER,
  FIELD_TYPE_FLOATING_POINT,
  FIELD_TYPE_POINTER,
  FIELD_TYPE_ARRAY,
  FIELD_TYPE_STRUCTURE,
  FIELD_TYPE_UNION
};

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

#define BIT_FIELD(struct_name, field_name)                                     \
  memset(&struct_name##__instance, 0, sizeof(struct_name));                    \
  struct_name##__instance.field_name = (long long)-1;                          \
  define_structure_field(FIELD_TYPE_BIT_FIELD, #struct_name, #field_name,      \
    start_bit(&struct_name##__instance, sizeof(struct_name)),                  \
    end_bit(&struct_name##__instance, sizeof(struct_name))                     \
  )

#define NORMAL_FIELD(struct_name, field_name, field_type)                      \
  define_structure_field(field_type, #struct_name, #field_name,                \
    8 * PTR_OFFSET(&struct_name##__instance,                                   \
                   &struct_name##__instance.field_name),                       \
                         8 * (PTR_OFFSET(&struct_name##__instance,             \
                                         &struct_name##__instance.field_name)  \
                              + sizeof(&struct_name##__instance.field_name)))

#define SIGNED_INTEGER_FIELD(struct_name, field_name)                          \
  memset(&struct_name##__instance, 0, sizeof(struct_name));                    \
  struct_name##__instance.field_name = (long long)-1;                          \
  define_structure_field(FIELD_TYPE_SIGNED_INTEGER, #struct_name, #field_name, \
              start_bit(&struct_name##__instance, sizeof(struct_name)),        \
              end_bit(&struct_name##__instance, sizeof(struct_name))           \
  )

#define UNSIGNED_INTEGER_FIELD(struct_name, field_name)                        \
  memset(&struct_name##__instance, 0, sizeof(struct_name));                    \
  struct_name##__instance.field_name = (long long)-1;                          \
  define_structure_field(FIELD_TYPE_UNSIGNED_INTEGER, #struct_name, #field_name, \
              start_bit(&struct_name##__instance, sizeof(struct_name)),        \
              end_bit(&struct_name##__instance, sizeof(struct_name))           \
  )

#define FLOATING_POINT_FIELD(struct_name, field_name)                          \
 NORMAL_FIELD(struct_name, field_name, FIELD_TYPE_FLOATING_POINT)

#define POINTER_FIELD(struct_name, field_name)                                 \
  memset(&struct_name##__instance, 0, sizeof(struct_name));                    \
  struct_name##__instance.field_name = (long long)-1;                          \
  define_structure_field(FIELD_TYPE_POINTER, #struct_name, #field_name,        \
              start_bit(&struct_name##__instance, sizeof(struct_name)),        \
              end_bit(&struct_name##__instance, sizeof(struct_name))           \
  )

#define ARRAY_FIELD(struct_name, field_name)                                          \
  define_structure_field(FIELD_TYPE_ARRAY, #struct_name, #field_name,                 \
    8 * PTR_OFFSET(&struct_name##__instance, &struct_name##__instance.field_name[0]), \
              sizeof(&struct_name##__instance.field_name))

#define STRUCTURE_FIELD(struct_name, field_name)                                   \
  define_structure_field(FIELD_TYPE_STRUCTURE, #struct_name, #field_name,          \
    8 * PTR_OFFSET(&struct_name##__instance, &struct_name##__instance.field_name), \
              sizeof(&struct_name##__instance.field_name))

#define UNION_FIELD(struct_name, field_name)                                       \
  define_structure_field(FIELD_TYPE_STRUCTURE, #struct_name, #field_name,          \
    8 * PTR_OFFSET(&struct_name##__instance, &struct_name##__instance.field_name), \
              sizeof(&struct_name##__instance.field_name))
  
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

// void output_bytes(unsigned char* bytes, int num_bytes) {
//  for (int i = 0; i < num_bytes; i++) {
//    printf("%02x", bytes[i]);
//  }
//  printf("\n");
// }

void define_structure_start(char *struct_name,
                            int size, 
                            int alignment) {
  printf("structure name=%s size=%d alignment=%d\n", struct_name, size, alignment);
}

void define_structure_end(char *struct_name) {
  // This method is useful to generate code once all fields are known.
}

void define_structure_field(int field_type,
                            char *struct_name,
                            char *field_name,
                            int start_bit,
                            int end_bit) {
  printf("structure name=%s field=%s start=%d end=%d\n", 
         struct_name, field_name, start_bit, end_bit);
}

// This could of course come from a header file.
struct Foo {
  unsigned int bitfield0 : 3;
  unsigned int bitfield1 : 1;
  int bar;
  long baz;
  long *qux;
  double boof;
};

// If the structure isn't typedef'd, we need to do that now.
typedef struct Foo Foo_t;

void main() {
  char *current_struct_name = NULL;
  DEFINE_STRUCTURE_START(Foo_t);
  BIT_FIELD(Foo_t, bitfield0);
  BIT_FIELD(Foo_t, bitfield1);
  UNSIGNED_INTEGER_FIELD(Foo_t, bar);
  UNSIGNED_INTEGER_FIELD(Foo_t, baz);
  POINTER_FIELD(Foo_t, qux);
  FLOATING_POINT_FIELD(Foo_t, boof);
  DEFINE_STRUCTURE_END(Foo_t);

  if (current_struct_name != NULL) {
    printf("ERROR: missing DEFINE_STRUCTURE_END\n");
    exit(1);
  }
  exit(0);
}
