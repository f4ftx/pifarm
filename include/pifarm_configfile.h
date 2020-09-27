#ifndef PIFARM_CONFIGFILE_H
#define PIFARM_CONFIGFILE_H

#include "pifarm_commons.h"

/* Configuration file parser */
void        parse_keyword     (const char * const key, const char * const val) ;
config_t    *read_config_file (const char * const config_filename) ;

#endif
