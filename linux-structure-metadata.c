// Generate "minimal" functions for important Linux/Unix
// data-structures.

#include "structure-metadata.h"

#include <time.h>
#include <sys/stat.h>

typedef struct timespec timespec_t;
typedef struct stat stat_t;

void main() {
  char *current_struct_name = NULL;

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
