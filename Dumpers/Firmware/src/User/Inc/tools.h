#ifndef __TOOLS__H__
#define __TOOLS__H__

#ifdef __cplusplus
 extern "C" {
#endif

char const *get_chip_name (void);
char const *get_dump_filename (void);
char const *get_prog_filename (void);
uint32_t get_end_address (void);

#ifdef __cplusplus
}
#endif

#endif /* __TOOLS__H__ */
