#include <stdio.h>
#include <stdlib.h>

typedef struct rwheader {
	unsigned int type;
	unsigned int size;
	unsigned int version;
} rwheader;

typedef struct split {
	unsigned int indexcount;
	unsigned int mat;
	float **vertices;
	float **uv;
	unsigned short *flags;
} split;

typedef struct geometry {
	split *splt;
	int nextgeo;
	int facecnt, vertexcnt, splitcount, indexcount;
	short flags;
} geometry;

int main(int argc, char *argv[])
{
	int i, j, k, faceinc;
	rwheader rwh;
	geometry *geo;

	int geocnt;
	FILE *obj, *dff;

	int blocksize, nextblock, unk;
	short comp_vert[4];

	if ((dff = fopen(argv[1], "r")) == NULL) {
		printf("error\n");
		exit(1);
	}
	if ((obj = fopen(argv[2], "w")) == NULL) {
		printf("error\n");
		exit(1);
	}

	fread(&rwh, 4, 3, dff); /* clump */
	fread(&rwh, 4, 3, dff); /* struct */
	fseek(dff, rwh.size, SEEK_CUR); 
	fread(&rwh, 4, 3, dff); /* Frame list */
	fseek(dff, rwh.size, SEEK_CUR);
	fread(&rwh, 4, 3, dff); /* geometry list */
	fread(&rwh, 4, 3, dff); /* Struct */
	fread(&geocnt, 4, 1, dff);
	geo = (geometry *) malloc(sizeof(geometry)*geocnt);
	for (k = 0; k < geocnt; k++) {
		fread(&rwh, 4, 3, dff); /* geometry */
		geo[k].nextgeo = rwh.size + ftell(dff);
		fread(&rwh, 4, 3, dff); /* struct */
		fread(&geo[k].flags, 2, 1, dff); 
		fseek(dff, 2, SEEK_CUR);
		fread(&geo[k].facecnt, 4, 1, dff);
		fread(&geo[k].vertexcnt, 4, 1, dff);
		fseek(dff, rwh.size-12, SEEK_CUR);
		fread(&rwh, 4, 3, dff); /* Material list */
		fseek(dff, rwh.size, SEEK_CUR);
		fread(&rwh, 4, 3, dff); /* Extension */
		fread(&rwh, 4, 3, dff); /* Bin Mesh PLG */
		fseek(dff, 4, SEEK_CUR);
		fread(&geo[k].splitcount, 4, 1, dff);
		fread(&geo[k].indexcount, 4, 1, dff);	
		geo[k].splt = (split *) malloc(sizeof(split)*geo[k].splitcount);
		for (i = 0; i < geo[k].splitcount; i++) {
			fread(&geo[k].splt[i].indexcount, 4, 1, dff);
			fread(&geo[k].splt[i].mat, 4, 1, dff);
		}
		fread(&rwh, 4, 3, dff); /* Native Data PLG */
		fread(&rwh, 4, 3, dff); /* Struct - broken header */

		
		fseek(dff, 4, SEEK_CUR);
		for (i = 0; i < geo[k].splitcount; i++) {
			fread(&blocksize, 4, 1, dff);
			nextblock = blocksize + ftell(dff) + 4;
			fseek(dff, 8, SEEK_CUR);
			fread(&unk, 4, 1, dff);
			fseek(dff, 16*unk-8, SEEK_CUR);

			/* Vertices */
			geo[k].splt[i].vertices = (float **) malloc(geo[k].splt[i].indexcount*4);
			geo[k].splt[i].flags = (short *) malloc(geo[k].splt[i].indexcount*2);
			for (j = 0; j < geo[k].splt[i].indexcount; j++) {
				geo[k].splt[i].vertices[j] = (float *) malloc(12);
				fread(comp_vert, 2, 4, dff);
				geo[k].splt[i].vertices[j] [0] = (float) comp_vert[0] / 128;
				geo[k].splt[i].vertices[j] [1] = (float) comp_vert[1] / 128;
				geo[k].splt[i].vertices[j] [2] = (float) comp_vert[2] / 128;
				geo[k].splt[i].flags[j] = comp_vert[3];
			}
			if ((geo[k].splitcount % 2) != 0)
				fseek(dff, 8, SEEK_CUR);

			/* UV Coordinates - possiby not correct*/
			geo[k].splt[i].uv = (float **) malloc(geo[k].splt[i].indexcount*4);
			for (j = 0; j < geo[k].splt[i].indexcount; j++) {
				geo[k].splt[i].uv[j] = (float *) malloc(8);
				fread(comp_vert, 2, 2, dff);
				geo[k].splt[i].uv[j] [0] = (float) comp_vert[0] / 4096;
				geo[k].splt[i].uv[j] [1] = (float) comp_vert[1] / 4096;
			}

			fseek(dff, nextblock, 0);
		}
		fseek(dff, geo[k].nextgeo, 0);
	}
	
	faceinc = 0;
	for (k = 0; k < geocnt; k++) {
		for (i = 0; i < geo[k].splitcount; i++) {
			for (j= 0; j < geo[k].splt[i].indexcount; j++) {
				fprintf(obj, "v %f %f %f\n", geo[k].splt[i].vertices[j] [0], 
						geo[k].splt[i].vertices[j] [1], geo[k].splt[i].vertices[j] [2]);
			}
			for (j= 0; j < geo[k].splt[i].indexcount; j++) {
				fprintf(obj, "vt %f %f\n", geo[k].splt[i].uv[j] [0], 
						geo[k].splt[i].uv[j] [1]);
			}
			for (j = 2; j < geo[k].splt[i].indexcount; j++) {
				if (geo[k].splt[i].flags[j] == 0x8000) /* 0x8000 - interrupt vertex strip */
					continue;
				fprintf(obj, "f %d/%d %d/%d %d/%d\n", j+1+faceinc, j+1+faceinc, 
					j+faceinc, +faceinc, j-1+faceinc, j-1+faceinc);
			}
				faceinc += geo[k].splt[i].indexcount;
		} 
	}
	return 0;
}
