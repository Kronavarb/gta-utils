#include <stdio.h>
#include <stdlib.h>

/* Dumps a Chunk to stdout (hex values converted to ascii) or 
 * a file (binary) */
int main(int argc, char *argv[])
{
	int i;
	int offset;
	int length;
	int dumpbinary;
	unsigned char byte;
	FILE *rw, *binout;
	
	if (argc < 3) {
		fprintf(stderr, "Usage: %s file address_of_header", argv[0]);
		fprintf(stderr, " [outfile]\n");
		exit(1);
	}
	if (argc > 3) {
		dumpbinary = 1;
		if ((binout = fopen(argv[3], "wb")) == NULL) {
			fprintf(stderr, "Could not open file: %s\n", argv[3]);
			exit(1);
		}
	} else {
		dumpbinary = 0;
	}
	if ((rw = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Could not open file: %s\n", argv[1]);
		exit(1);
	}
	sscanf(argv[2], "%X", &offset);

	fseek(rw, offset, 0);
	fseek(rw, 4, SEEK_CUR);
	fread(&length, 4, 1, rw);
	fseek(rw, 4, SEEK_CUR);

	if (dumpbinary) {
		for (i = 0; i < length; i++)
			putc(getc(rw), binout);
	} else {
		for (i = 0; i < length; i++) {
			if ((i % 16) == 0)
				printf("\n");
			byte = getc(rw);
			printf("%02X ", byte);
		}
		printf("\n");
	}

	return 0;
}
