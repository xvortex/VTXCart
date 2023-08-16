#include "main.h"
#include "tools.h"

char const *get_chip_name (void)
{
  char const *chip_name;

  switch (cur_chip) {
    case CHIP_P:
      chip_name = CHIP_NAME_P;
      break;
    case CHIP_S:
      chip_name = CHIP_NAME_S;
      break;
    case CHIP_M:
      chip_name = CHIP_NAME_M;
      break;
    case CHIP_C:
      chip_name = CHIP_NAME_C;
      break;
    case CHIP_V:
      chip_name = CHIP_NAME_V;
      break;
    default:
      chip_name = 0;
      break;
  }

  return chip_name;
}

char const *get_dump_filename (void)
{
  char const *dump_filename;

  switch (cur_chip) {
    case CHIP_P:
      dump_filename = DUMP_FILENAME_P;
      break;
    case CHIP_S:
      dump_filename = DUMP_FILENAME_S;
      break;
    case CHIP_M:
      dump_filename = DUMP_FILENAME_M;
      break;
    case CHIP_C:
      dump_filename = DUMP_FILENAME_C;
      break;
    case CHIP_V:
      dump_filename = DUMP_FILENAME_V;
      break;
    default:
      dump_filename = 0;
      break;
  }

  return dump_filename;
}

char const *get_prog_filename (void)
{
  char const *prog_filename;

  switch (cur_chip) {
    case CHIP_P:
      prog_filename = PROG_FILENAME_P;
      break;
    case CHIP_S:
      prog_filename = PROG_FILENAME_S;
      break;
    case CHIP_M:
      prog_filename = PROG_FILENAME_M;
      break;
    case CHIP_C:
      prog_filename = PROG_FILENAME_C;
      break;
    case CHIP_V:
      prog_filename = PROG_FILENAME_V;
      break;
    default:
      prog_filename = 0;
      break;
  }

  return prog_filename;
}

uint32_t get_end_address (void)
{
  uint32_t end_address;

  switch (cur_chip) {
    case CHIP_P:
      end_address = END_ADDRESS_P;
      break;
    case CHIP_S:
      end_address = END_ADDRESS_S;
      break;
    case CHIP_M:
      end_address = END_ADDRESS_M;
      break;
    case CHIP_C:
      end_address = END_ADDRESS_C;
      break;
    case CHIP_V:
      end_address = END_ADDRESS_V;
      break;
    default:
      end_address = 0;
      break;
  }

  return end_address;
}
