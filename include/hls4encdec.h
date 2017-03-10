#ifndef _HLS4ENCDEC_H_
#define _HLS4ENCDEC_H_

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <math.h>
#include "cpuidh.h"

/* Software version */
#define VERSION "0.1.170309"
#define AUTHOR "Ricardo Costa"

/* CSV file header */
#define CSV_FILE_HEADER "Chunk Filename, Decrypt Process Time (Seconds)"

/* Print output console text colors */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*Enable/Disable Debug Flag*/
#define DEBUG 1

#endif