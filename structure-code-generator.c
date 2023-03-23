#include <stdio.h>
#include "structure-metadata.h"

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

extern void internal_rename(char* original_structure_name,
                            char* structure_name,
                            char* original_field_name,
                            char* field_name) {
  renamings[number_of_renames].original_structure_name = original_structure_name;
  renamings[number_of_renames].structure_name = structure_name;
  renamings[number_of_renames].original_field_name = original_field_name;
  renamings[number_of_renames].field_name = field_name;
  number_of_renames++;
}

extern void clear_renames() {
  number_of_renames = 0;
}

char* rename_structure_name(char *name) {
  for (int i = 0; i < number_of_renames; i++) {
    if (strcmp(renamings[i].original_structure_name, name) == 0
        && renamings[i].original_field_name == NULL) {
      return renamings[i].structure_name;
    }
  }

  return name;
}

char* rename_field_name(char *struct_name, char *field_name) {
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

struct structure_metadata metadata;

extern void define_structure_start(char *struct_name,
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

extern void define_structure_field(int field_type,
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

// Delay generation until we know all the fields. This will allow us
// to create "print" routines for an entire structure based on it's
// fields.

char* field_type_to_blob_type_string(int value) {
  switch (value) {
  case FIELD_TYPE_SIGNED_BIT_FIELD: return "signed";
  case FIELD_TYPE_UNSIGNED_BIT_FIELD: return "unsigned";
  case FIELD_TYPE_SIGNED_INTEGER: return "signed";
  case FIELD_TYPE_UNSIGNED_INTEGER: return "unsigned";
  case FIELD_TYPE_FLOATING_POINT: return "float";
  case FIELD_TYPE_POINTER: return "pointer";
  case FIELD_TYPE_ARRAY: return "blob";
  case FIELD_TYPE_STRUCTURE: return "blob";
  case FIELD_TYPE_UNION: return "blob";
  }
  printf("ERROR: field type out of range.\n");
  exit(1);
}

extern void define_structure_end(char *struct_name) {
  printf(";;; ======================================================================\n");
  printf(";;; %s (Linux name %s)\n", metadata.name, metadata.original_name);
  printf(";;; ======================================================================\n\n");

  printf("($define-blob (%s bit-size: %d)", metadata.name, metadata.size);

  for (int i = 0; (i < metadata.num_fields); i++) {
    struct structure_field_metadata field = metadata.fields[i];
    printf("\n");
    printf("  (%s field-type: %s bit-offset: %d bit-size: %d)",
           field.field_name,
           field_type_to_blob_type_string(field.field_type),
           field.start_bit,
           (field.end_bit - field.start_bit));
  }
  printf(")\n\n");
}

extern void OLD_define_structure_end(char *struct_name) {
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
