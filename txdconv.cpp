#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <rwcore.h>

using namespace std;

void WriteTextureNative(RwTexture *tex, RwStream *outf)
{
	int i;
	RwTexture *tp;
	RwInt32 tmp32[2];
	RwInt16 tmp16[2];
	RwInt8 tmp8[4];

	RwStreamWriteChunkHeader(outf, rwID_TEXTURENATIVE,
	  24 + 88 + tex->width*tex->height*tex->depth/8);
	RwStreamWriteChunkHeader(outf, rwID_STRUCT,
	  88 + 4 + tex->width*tex->height*tex->depth/8);

	tmp32[0] = rwID_PCD3D8;
	RwStreamWriteInt32(outf, tmp32, sizeof(RwInt32));
	tmp32[0] = tex->FilterFlags; /* and tex wrap */
	RwStreamWriteInt32(outf, tmp32, sizeof(RwInt32));
	RwStreamWrite(outf, tex->Name, 32);
	RwStreamWrite(outf, tex->MaskName, 32);
	tmp32[0] = 0x0500;
	tmp32[1] = tex->HasAlpha ? 1 : 0;
	RwStreamWriteInt32(outf, tmp32, 2*sizeof(RwInt32));
	tmp16[0] = tex->width;
	tmp16[1] = tex->height;
	RwStreamWriteInt16(outf, tmp16, 2*sizeof(RwInt16));
	tmp8[0] = tex->depth;
	tmp8[1] = 0;
	tmp8[2] = 0x04;
	tmp8[3] = 0;
	RwStreamWrite(outf, tmp8, 4*sizeof(RwInt8));

	tmp32[0] = tex->width*tex->height*tex->depth/8;
	RwStreamWriteInt32(outf, tmp32, sizeof(RwInt32));

//	tmp8[0] = 0xFF;
//	for (i = 0; i < tmp32[0]; i++)
//		RwStreamWrite(outf, tmp8, sizeof(RwInt8));
	RwStreamWrite(outf, tex->Data_a, tmp32[0]*sizeof(RwInt8));

	RwStreamWriteChunkHeader(outf, rwID_EXTENSION, 0);
}

void WriteTxd(RwTexDictionary *txd, RwStream *outf)
{
	int i;
	int texsize;
	RwTexture *tp;
	RwInt16 tmp16[2];
	
	texsize = 0;
	for (i = 0; i < txd->numTextures; i++) {
		tp = &txd->texture[i];	
		if (tp->depth != 32) {
			cerr << "Oh no\n";
			exit(1);
		}
		texsize += 88 + 4 + tp->width*tp->height*tp->depth/8;
	}
	RwStreamWriteChunkHeader(outf, rwID_TEXDICTIONARY,
	    28 + texsize + 36 * txd->numTextures);
	RwStreamWriteChunkHeader(outf, rwID_STRUCT, 2*sizeof(RwInt16));
	tmp16[0] = txd->numTextures;
	tmp16[1] = 0;
	RwStreamWriteInt16(outf, tmp16, 2*sizeof(RwInt16));
	for (i = 0; i < txd->numTextures; i++)
		WriteTextureNative(&txd->texture[i], outf);

	RwStreamWriteChunkHeader(outf, rwID_EXTENSION, 0);
}

int main(int argc, char *argv[])
{
	int i;
	RwStream *txdf;
	RwStream *outf;
	RwTexDictionary *TexD;

	if (argc < 2) {
		cout << "Usage: " << argv[0] << " txd\n";
		exit(1);
	}
	txdf = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, argv[1]);
	if (txdf == NULL) {
		cerr << "Couldn't open file: " << argv[1] << endl;
		exit(1);
	}

	TexD = RwTexDictionaryStreamRead(txdf);
	outf = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE,
	         (void *) "new.txd");
	if (outf == NULL)
		exit(3);
	WriteTxd(TexD, outf);
	for (i = 0; i < TexD->numTextures; i++)
		cout << TexD->texture[i].depth << " " <<
		     TexD->texture[i].Name << endl;

	return 0;
}
