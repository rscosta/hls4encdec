/*
    hls4encdec - Encryptor and Decryptor for HTTP Live Streaming (HLS) chunks.  
    
    Usage: hls4encdec -[OPTIONS]:
    Options:
    
    -d decryption mode enabled
    -e encryption mode enabled
    -i input *.m3u8 hls file
    -r decryption benchmark result output file in Comma-Separated Value (Decryption only)
    
    Example 1: hls4encdec -d -i encrypted_video/file.m3u8 -r result.csv
    Example 2: hls4encdec -e -i file.m3u8
*/ 
#include "hls4encdec.h"

/* Hardcoded key */
unsigned char userkey[] = "\x09\x8F\x6B\xCD\x46\x21\xD3\x73\xCA\xDE\x4E\x83\x26\x27\xB4\xF6";

/* Hardcoded Initialization Vector for AES */
unsigned char IV[] =      "\x0A\x91\x72\x71\x6A\xE6\x42\x84\x09\x88\x5B\x8B\x82\x9C\xCB\x05";


/*********************************
 *    FUNCTIONS DECLARATIONS     *
 *********************************/
void hls4encdec_print_usage();
unsigned char *hls4encdec_convert_str_key_to_hex(unsigned char* str);
int hls4encdec_search_in_file(const char *fname, char *str);
void hls4encdec_enc_dec_file(int should_encrypt, FILE *ifp, FILE *ofp, unsigned char *ckey, unsigned char *ivec, double *bench_time);
int hls4encdec_is_valid_hls_manifest(const char *m3u8_filename);
int hls4encdec_check_if_file_exists(const char *filename);
int hls4encdec_write_csv_data(const char *csv_filename, const char *ts_filename, double dec_bench_time);
void hls4encdec_str_replace(char *target, const char *needle, const char *replacement);
int hls4encdec_decrypt_segments(const char *m3u8_filename, const char *csv_filename);

/*********************************
 *         FUNCTIONS             *
 *********************************/
void hls4encdec_print_usage() {
    printf("\nhls4encdec - Encryptor and Decryptor for HTTP Live Streaming (HLS) chunks. Version:%s Author:%s\n", VERSION, AUTHOR);  
    printf("Usage: hls4encdec [OPTIONS]\n\n");
    printf("Options:\n");
    printf(" -d\tdecryption mode enabled\n");
    printf(" -e\tencryption mode enabled\n");
    printf(" -i\tinput *.m3u8 hls file\n");
    printf(" -r\tdecryption benchmark result output file in Comma-Separated Value (Decryption only)\n\n");
    printf("Example 1: hls4dec -d -i file.m3u8 -r result.csv\n");
    printf("Example 2: hls4dec -e -i file.m3u8\n");
}

unsigned char *hls4encdec_convert_str_key_to_hex(unsigned char* str) {
    unsigned char output[16];
    char *result = NULL;
    int i = 0;
    int valid_hex = 0;
    
    while(isxdigit(*str))
    {
      str++;
      valid_hex++;
    }

    if(valid_hex != 32)
      return "";
    
    for (i = 0; i < AES_BLOCK_SIZE; i++)
    {
      sscanf(&str[i * 2], "%2hhx", &output[i]);
#if DEBUG == 1
	  printf("Key[i] = %x\n",output[i]);
#endif

    }
    result = output;

    return result;
}

int hls4encdec_search_in_file(const char *fname, char *str) {
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[512];
	
	if((fp = fopen(fname, "r")) == NULL) {
#if DEBUG == 1
	  printf("File %s not found!\n",fname);
#endif
	  return 0;
	}

	while(fgets(temp, 512, fp) != NULL) {
		if((strstr(temp, str)) != NULL) {
#if DEBUG == 1
		        printf("\n%s\n", temp);
#endif
			find_result++;
		}
		line_num++;
	}

	if(find_result == 0) {
#if DEBUG == 1
	  printf("\nSorry, couldn't find a match.\n");
#endif
	}
	else {
#if DEBUG == 1
	  printf("Total found: %d\n", find_result);
#endif	  
	}
	
	if(fp) {
           fclose(fp);
	}
	return find_result;
}

void hls4encdec_enc_dec_file(int should_encrypt, FILE *ifp, FILE *ofp, unsigned char *ckey, unsigned char *ivec, double *bench_time) {

    const unsigned BUFSIZE=4096;
    unsigned char *read_buf = malloc(BUFSIZE);
    unsigned char *cipher_buf;
    unsigned blocksize;
    int out_len;
    EVP_CIPHER_CTX ctx;
    double bench_time_result = 0;

    /* Configure AES-128-CBC Algorithm */
    EVP_CipherInit(&ctx, EVP_aes_128_cbc(), ckey, ivec, should_encrypt);
    blocksize = EVP_CIPHER_CTX_block_size(&ctx);
    cipher_buf = malloc(BUFSIZE + blocksize);

#if DEBUG == 1
    if(should_encrypt)
	printf("Starting encryption engine...\n");
    else
	printf("Starting decryption engine...\n");
#endif	  

    while (1) {

        /* Read in data in blocks until EOF. Update the ciphering with each read. */
        int numRead = fread(read_buf, sizeof(unsigned char), BUFSIZE, ifp);
        start_time();
        EVP_CipherUpdate(&ctx, cipher_buf, &out_len, read_buf, numRead);
        end_time();
        bench_time_result += secs;
        fwrite(cipher_buf, sizeof(unsigned char), out_len, ofp);
        if (numRead < BUFSIZE) { /* Reached EOF */
            break;
        }
    }

    /* Cipher the final block and write it out. */
    EVP_CipherFinal(&ctx, cipher_buf, &out_len);
    fwrite(cipher_buf, sizeof(unsigned char), out_len, ofp);

    /* Copy benchmark time result */
    *bench_time = bench_time_result;

#if DEBUG == 1
    fprintf (stderr,"%6.6f seconds\n", bench_time_result);
#endif

    /* Free memory */
    free(cipher_buf);
    free(read_buf);
}

int hls4encdec_is_valid_hls_manifest(const char *m3u8_filename) {

      /* 0 = nok | 1 = ok */
      if(hls4encdec_search_in_file(m3u8_filename,"#EXTM3U") < 1) {
#if DEBUG == 1
       printf("File %s - Invalid M3U8 Header\n", m3u8_filename);
#endif	  
	return 0;
      }
      if(hls4encdec_search_in_file(m3u8_filename,"#EXT-X-PLAYLIST-TYPE:VOD") < 1) {
#if DEBUG == 1
       printf("File %s - It is not a VOD Content Type\n", m3u8_filename);
#endif	  
	return 0;
      }
#if 0     
      if(hls4encdec_search_in_file(m3u8_filename,"#EXT-X-KEY:METHOD=AES-128") < 1) {
#if DEBUG == 1
       printf("File %s - It is not a AES-128 Encryption type\n", m3u8_filename);
#endif	  
	return 0;
      } 
      if(hls4encdec_search_in_file(m3u8_filename,"URI=") < 1) {
#if DEBUG == 1
       printf("File %s - A key should be set\n", m3u8_filename);
#endif	  
	return 0;
      } 
      if(hls4encdec_search_in_file(m3u8_filename,"IV=") > 0) {
#if DEBUG == 1
       printf("File %s - Initialization Vector is not supported for this version\n", m3u8_filename);
#endif	  
	return 0;
      }
#endif      
      if(hls4encdec_search_in_file(m3u8_filename,"#EXTINF:") < 1) {
#if DEBUG == 1
       printf("File %s - At least 1 segment should be available\n", m3u8_filename);
#endif	  
	return 0;
      } 

      return 1;
}

int hls4encdec_check_if_file_exists(const char *filename) {
  if(access(filename, F_OK) != -1)
    return 1;

  return 0;
}

int hls4encdec_write_csv_data(const char *csv_filename, const char *ts_filename, double dec_bench_time) {
     int find_header = 0;
     char line[64];
     char temp[128];
     FILE *csvfp = fopen(csv_filename, "a+");

    if(NULL == csvfp)
    {
#if DEBUG == 1
       printf("Error opening file %s\n", csv_filename);
#endif	  
       return -1;
    }

    while(fgets(temp, 128, csvfp) != NULL) {
      /* Find for the main header */
      if((strstr(temp, CSV_FILE_HEADER)) != NULL) {
#if DEBUG == 1
       printf("CSV Header already found!\n");
#endif	  
	find_header=1;
       }
    }
    
    /* Write Time result and Media parameters to CSV File */
    if(!find_header) {
      /* Write CSV Header */
      strcpy(line, CSV_FILE_HEADER); 
	  
      fputs(line, csvfp);
    }
	
     /* Write CSV Content */
     /* Chunk Filename */
     fputs("\n", csvfp);
	  
     strncpy(line, ts_filename, sizeof(line)); 
	  
     fputs(line, csvfp);

     /* Decrypt Process Time */
     fputs(", ", csvfp);

     sprintf(line, "%f", dec_bench_time);

     fputs(line, csvfp);

    fclose(csvfp);
    
    return 0;
}

void hls4encdec_str_replace(char *target, const char *needle, const char *replacement)
{
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        /* copy part before needle */
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        /* copy replacement string */
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        /* adjust pointers, move on */
        tmp = p + needle_len;
    }

    /* write altered string back to target */
    strcpy(target, buffer);
}

int hls4encdec_enc_dec_segments(int should_encrypt, const char *m3u8_filename, const char *csv_filename) {
    FILE *m3u8fp = NULL;
    FILE *segmentfp = NULL;
    FILE *tmpsegmentfp = NULL;
    FILE *keyfp = NULL;
    int bytes_read, bytes_write;
    double decrypt_bench_time = 0;
    char path[512];
    char ts_path[512];
    char temp[512];
    char m3u8_path[512];
    /*char key_path[512];*/
    char enc_dec_m3u8_path
[512];
    unsigned char indata[AES_BLOCK_SIZE];
    unsigned char outdata[AES_BLOCK_SIZE];
    unsigned char decryptdata[AES_BLOCK_SIZE];

    AES_KEY key;

    clock_t start, end;

#if DEBUG == 1
    /* Get total of segments */
    int n_segments = hls4encdec_search_in_file(m3u8_filename, ".ts");

    printf("Total of Chunks Found = %d\n", n_segments);
#endif	  

    /* Open *.m3u8 file */
    if((m3u8fp = fopen(m3u8_filename, "r+")) == NULL) {
#if DEBUG == 1
     printf("File %s not found!\n",m3u8_filename);
#endif
     return -1;
     }

    /* Set Mode file to specific folder */
    strcpy(enc_dec_m3u8_path, "cp ");
    strcat(enc_dec_m3u8_path, m3u8_filename);

    /* Extract m3u8 path */
    strcpy(m3u8_path, dirname((char*)m3u8_filename));

    if(should_encrypt) {
     strcat(enc_dec_m3u8_path, " ./encrypted_video");
     system("mkdir -p encrypted_video");
     printf("=============> %s\n",enc_dec_m3u8_path);
     system(enc_dec_m3u8_path);
#if DEBUG == 1
     printf("Copying *.m3u8 file into ./encrypted_video folder\n");
#endif
    } 
    else {  
     strcat(enc_dec_m3u8_path, " ./decrypted_video");
     system("mkdir -p decrypted_video");
     printf("=============> %s\n",enc_dec_m3u8_path);
      system(enc_dec_m3u8_path);
#if DEBUG == 1
     printf("Copying *.m3u8 file into ./decrypted_video folder\n");
#endif
    }
    
    while(fgets(temp, 512, m3u8fp) != NULL) {
	if((strstr(temp, ".ts")) != NULL) {
#if DEBUG == 1
        printf("\nCurrent segment ->%s\n", temp);
#endif
	/* Get Full segment path */
	strcpy(path, m3u8_path);
	strcat(path, "/");
	strncat(path, temp, strlen(temp)-1);
 	
#if DEBUG == 1
	printf("Chunk path = (%s)\n", path);
#endif	  

	/* Open segment file*/
        if((segmentfp = fopen(path, "r+")) == NULL) {
#if DEBUG == 1
	  printf("Error opening file %s\n", path);
#endif	  
	  return -1;
	}

	/* Get Full segment path for ts files */
	if(should_encrypt)
	  strcpy(ts_path, "./encrypted_video/");
	else
	  strcpy(ts_path, "./decrypted_video/");

	strncat(ts_path, temp, strlen(temp)-1);
 
	/*Create decrypt file result*/    
	tmpsegmentfp = fopen(ts_path, "w+");
	
	if(NULL == tmpsegmentfp) {
#if DEBUG == 1
	  printf("Error opening file %s\n", ts_path);
#endif	  
	  return -1;
	}

	/* Encrypt/Decrypt files */
        hls4encdec_enc_dec_file(should_encrypt, segmentfp, tmpsegmentfp, userkey, IV, &decrypt_bench_time);
	 
	if(!should_encrypt) {
	  /* Write data in CSV File */
#if DEBUG == 1
	  printf("Writing to CSV File...\n");
#endif	  
	  hls4encdec_write_csv_data(csv_filename, path, decrypt_bench_time);
	}
	/* Close all files */
	fclose(segmentfp);
	fclose(tmpsegmentfp);
      }
    }

    fclose(m3u8fp);

    return 0;
}

int main(int argc, char *argv[]) {
    int option = 0;
    char m3u8_filename[128];
    char csv_filename[128];
    int m3u8_param_set = 0;
    int csv_param_set = 0;
    int mode = -1;
    int decrypt_mode = 0;
    int res = 0;
    
    /* Specifying the expected options */
    while ((option = getopt(argc, argv,"edi:r:")) != -1) {
        switch (option) {
             case 'e' : 
	                mode = 1; /*Encryption enabled */
                 break;
             case 'd' : 
	                mode = 0; /*Decryption enabled */
                break;
             case 'i' : strncpy(m3u8_filename, optarg, 128); /* m3u8 filename */
	                m3u8_param_set = 1;
                 break;
             case 'r' : strncpy(csv_filename, optarg, 128); /* csv filename */
	                csv_param_set = 1;
                 break;
             default: hls4encdec_print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    
     if (m3u8_param_set == 0) {
	printf("\n[ERROR] - Invalid parameter for argument -i\n\n");
	hls4encdec_print_usage();
	exit(EXIT_FAILURE);
      }
      if(mode == 0){
      if (csv_param_set == 0) {
	printf("\n[ERROR] - Invalid parameter for argument -r\n\n");
	hls4encdec_print_usage();
	exit(EXIT_FAILURE);
      }
      }
 
    /* Check if hls manifest (*.m3u8) is a valid file */
    if(!hls4encdec_is_valid_hls_manifest(m3u8_filename)) {
      printf("\n[ERROR] - Invalid %s Manifest File\n", m3u8_filename);
      exit(EXIT_FAILURE);
    }

    /* Check if mode is encrypt/decrypt */
    if(mode == -1) {
      printf("\n[ERROR] - One Mode should be set!\n");
      exit(EXIT_FAILURE);
    }
      
    printf("\nInitializing HLS decrypt content from %s file....\n", m3u8_filename); 

    /* Call encryptor/decryptor function */
    res = hls4encdec_enc_dec_segments(mode, m3u8_filename, csv_filename);    

    /* Print final results */
    if(res != 0) {
      if(mode)
	printf(ANSI_COLOR_RED  "Failed while encrypting files. Error = %d"  ANSI_COLOR_RESET "\n", res);
      else
	printf(ANSI_COLOR_RED  "Failed while decrypting files. Error = %d"  ANSI_COLOR_RESET "\n", res);
    } 
    else {
      if(mode) {
	printf(ANSI_COLOR_GREEN "Encryption done successfully!" ANSI_COLOR_RESET "\n");
	printf(ANSI_COLOR_YELLOW "Encrypted files are saved in ./encrypted_video folder." ANSI_COLOR_RESET "\n");
      }
      else {
	printf(ANSI_COLOR_GREEN "Decryption done successfully!" ANSI_COLOR_RESET "\n");
	printf(ANSI_COLOR_YELLOW "Decrypted files are saved in ./decrypted_video folder." ANSI_COLOR_RESET "\n");
      }
    }
    return 0;
}


