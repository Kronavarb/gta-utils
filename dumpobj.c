#include <stdio.h>
#include <stdlib.h>
#include "rwcore.h"
#include "rpworld.h"

int main(int argc, char *argv[])
{
	int vert;
	RwStream *fdff;
	RpClump *Clump;
	int i, j, k;
	int g, f;

	if (argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		exit(1);
	}

	fdff = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, argv[1]);
	if (fdff == NULL) {
		fprintf(stderr, "Couldn't open file\n");
		exit(1);
	}
	Clump = RpClumpStreamRead(fdff);
	RwStreamClose(fdff, NULL);

	printf("# Read dff\n");
	for (i = 0; i < Clump->numAtomics; i++) {
		printf("# Atomic %d\n", i);
		g = Clump->Atm[i].GeoIndex;
		f = Clump->Atm[i].FrmIndex;
		for (j = 0; j < Clump->Geo[g].numVertices; j++) {
			printf("v %f %f %f\n", 
			Clump->Geo[g].Vertex[j][0],
			Clump->Geo[g].Vertex[j][1],
			Clump->Geo[g].Vertex[j][2]);
		}
		printf("# Done Vertices %d\n", i);
		for (j = 0; j < Clump->Geo[g].numSplits; j++) {
			printf("# Split %d\n", j);
			RpMesh *split;
			split = &Clump->Geo[g].Split[j];
			if (Clump->Geo[g].FaceType == 0) {
			for (k = 0; k < split->numIndices-2; k+=2) {
				printf("f %d %d %d\n", split->Indices[k],
				  split->Indices[k+1], split->Indices[k+2]);
			}
			} else {
			for (k = 0; k < split->numIndices-2; k++) {
				printf("f %d %d %d\n", split->Indices[k],
				  split->Indices[k+1], split->Indices[k+2]);
			}
			}
		}
	}

	RpClumpDestroy(Clump);
}
