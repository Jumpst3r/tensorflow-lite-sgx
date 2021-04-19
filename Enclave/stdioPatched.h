


#define O_RDONLY 0x0000 /* open for reading only */
#define O_WRONLY 0x0001 /* open for writing only */
#define O_RDWR 0x0002   /* open for reading and writing */
#define O_APPEND 0x0008 /* writes done at eof */

#define O_CREAT 0x0100 /* create and open file */
#define O_TRUNC 0x0200 /* open and truncate */
#define O_EXCL 0x0400  /* open only if file doesn't already exist */

#define O_TEXT 0x4000     /* file mode is text (translated) */
#define O_BINARY 0x8000   /* file mode is binary (untranslated) */
#define O_WTEXT 0x10000   /* file mode is UTF16 (translated) */
#define O_U16TEXT 0x20000 /* file mode is UTF16 no BOM (translated) */
#define O_U8TEXT 0x40000  /* file mode is UTF8  no BOM (translated) */

int open(const char *filename, int mode);
int read(int file, void *buf, unsigned int size);
int write(int file, void *buf, unsigned int size);
int close(int file);

void fprintf(void *fd, const char *format, ...);

int fread(void *ptr, int size, int count, void *stream);
int fclose(void *stream);
//void *fopen(const char *filename, const char *mode);
int fwrite(const void *ptr, int size, int count, void *stream);

int printf(const char *fmt, ...);

int fseek (int *stream, long int offset, int origin);

int get_filesize(char *filename, long *size);

int load_model(char *modelname, long f_size, unsigned char *data);

int load_input(char *modelname, long f_size, float *data);