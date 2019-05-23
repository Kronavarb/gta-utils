#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <rwcore.h>

using namespace std;

/* Extracts all textures out of a txd (needs librw) */

void WriteByte(ofstream &file, char b)
{
	file.write(&b, 1);
}

void WriteWord(ofstream &file, short s)
{
	file.write((char*)&s, 2);
}

void writetgas(RwTexDictionary *texd)
{
	int i, j;
	ofstream tgaf;
	char fname[32];
	RwTexture *pic;

	for (j = 0; j < texd->numTextures; j++) {
		pic = &texd->texture[j];
		if (pic->depth != 32)
			exit(10);
		strcpy(fname, pic->Name);
		strcat(fname, ".tga");
		tgaf.open(fname, ios::binary);
		if (tgaf.fail())
			exit(10);
		WriteByte(tgaf, 0);
		WriteByte(tgaf, 0);
		WriteByte(tgaf, 2);
		WriteWord(tgaf, 0);
		WriteWord(tgaf, 0);
		WriteByte(tgaf, 0);
		WriteWord(tgaf, 0);
		WriteWord(tgaf, 0);
		WriteWord(tgaf, pic->width);
		WriteWord(tgaf, pic->height);
		WriteByte(tgaf, 0x20);
		WriteByte(tgaf, 0x28);

		for (i = 0; i < pic->width*pic->height*4; i += 4) {
			WriteByte(tgaf, pic->Data_a[i+2]);
			WriteByte(tgaf, pic->Data_a[i+1]);
			WriteByte(tgaf, pic->Data_a[i]);
			WriteByte(tgaf, pic->Data_a[i+3]);
		}
		tgaf.close();
	}
} 

int main(int argc, char *argv[])
{
	RwStream *txdf;
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
	writetgas(TexD);

	return 0;
}
