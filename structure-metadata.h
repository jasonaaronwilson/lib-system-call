#ifndef __STRUCTURE_METADATA_H__

#include <stddef.h>

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
  
#define RENAME_STRUCTURE(original_name, new_name) \
  internal_rename(#original_name, new_name, 0, 0)

#define RENAME_FIELD(original_name, original_field_name, field_name)  \
  internal_rename(#original_name, 0, #original_field_name, field_name)
  
extern char *field_type_to_string(int value);
extern int is_field_type_addressable(int value);

// Forward Declarations. In the future it will be possible to swap
// these out with your own functions.

extern void define_structure_start(char *struct_name,
                                   int size, 
                                   int alignment);

extern void define_structure_end(char *struct_name);

extern void define_structure_field(int field_type,
                                   char *struct_name,
                                   char *field_name,
                                   int start_bit,
                                   int end_bit);

extern void internal_rename(char* original_structure_name,
                            char* structure_name,
                            char* original_field_name,
                            char* field_name);

extern void clear_renames();

extern int start_bit(unsigned char* bytes, int max_bytes);
extern int end_bit(unsigned char* bytes, int max_bytes);
                            
#endif /* __STRUCTURE_METADATA_H__ */
