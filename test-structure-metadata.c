#include "structure-metadata.h"

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

  exit(0);
}
