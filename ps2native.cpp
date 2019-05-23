#include "renderware.h"

#include <cstdio>
using namespace std;

namespace rw {

uint32 index;

void readChunk(uint8 *chunk8, uint32 *chunk32, ifstream &dff)
{
	dff.read(reinterpret_cast <char *> (chunk8), 0x10);
	dff.seekg(-0x10, ios::cur);
	dff.read(reinterpret_cast <char *> (chunk32), 0x10);
}

void Geometry::readPs2NativeData(ifstream &dff)
{
	HeaderInfo header;

	READ_HEADER(CHUNK_STRUCT); /* wrong size */

	uint32 platform = readUInt32(dff);
	if (platform != 0x04) {
		cerr << "error: native data not in ps2 format\n";
		return;
	}
	index = 0;
	vector<uint32> typesRead;
	for (uint32 i = 0; i < splits.size(); i++) {
		uint32 splitSize = readUInt32(dff);
		dff.seekg(4, ios::cur);
//		uint32 unknown = readUInt32(dff);

		uint32 numIndices = splits[i].indices.size();
		splits[i].indices.clear();
		uint32 end = splitSize + dff.tellg();
		uint8 chunk8[16];
		uint32 chunk32[4];

		uint32 blockStart = dff.tellg();
		bool reachedEnd;
		bool sectionALast = false;
		bool sectionBLast = false;
		bool dataAread = false;
		while (dff.tellg() < end) {
			/* sectionA  */
			reachedEnd = false;
			while (!reachedEnd && !sectionALast) {
				readChunk(chunk8, chunk32, dff);
				switch (chunk8[3]) {
				case 0x30: {
					/* read all split data when we find the
					 * first data block and ignore all
					 * other blocks */
					if (dataAread) {
						/* skip dummy data */
						dff.seekg(0x10, ios::cur);
						break;
					}
					uint32 oldPos = dff.tellg();
					uint32 dataPos = blockStart +
					                   chunk32[1]*0x10;
					dff.seekg(dataPos, ios::beg);
					readData(numIndices,chunk32[3], i, dff);
					dff.seekg(oldPos + 0x10, ios::beg);
					break;
				}
				case 0x60:
					sectionALast = true;
				case 0x10:
					reachedEnd = true;
					dataAread = true;
					break;
				default:
					break;
				}
			}

			/* sectionB */
			reachedEnd = false;
			while (!reachedEnd && !sectionBLast) {
				readChunk(chunk8, chunk32, dff);
				switch (chunk8[3]) {
				case 0x00:
				case 0x07:
					readData(chunk8[14],chunk32[3], i, dff);
					/* remember what sort of data we read */
					typesRead.push_back(chunk32[3]);
					break;
				case 0x04:
					if (chunk8[7] == 0x15)
						;//first
					else if (chunk8[7] == 0x17)
						;//not first

					if ((chunk8[11] == 0x11 &&
					      chunk8[15] == 0x11) ||
					    (chunk8[11] == 0x11 &&
					      chunk8[15] == 0x06)) {
						// last
						dff.seekg(end, ios::beg);
						typesRead.clear();
						sectionBLast = true;
					} else if (chunk8[11] == 0 &&
					           chunk8[15] == 0) {
						deleteOverlapping(typesRead, i);
						typesRead.clear();
						// last
					}
					reachedEnd = true;
					break;
				default:
					break;
				}
			}
		}
	}
}


void Geometry::readData(uint32 vertexCount, uint32 type,
                        uint32 split, ifstream &dff)
{
	float32 vertexScale;
	if (flags & FLAGS_NORMALS)
		vertexScale = VERTSCALE2;
	else
		vertexScale = VERTSCALE1;

	uint32 size;
	type &= 0xFF00FFFF;
	/* TODO: read greater chunks */
	switch (type) {
	/* Vertices */
	case 0x68008000: {
		size = 3 * sizeof(float32);
		for (uint32 j = 0; j < vertexCount; j++) {
			vertices.push_back(readFloat32(dff));
			vertices.push_back(readFloat32(dff));
			vertices.push_back(readFloat32(dff));
			splits[split].indices.push_back(index++);
		}
		break;
	} case 0x6D008000: {
		size = 4 * sizeof(int16);
		int16 vertex[4];
		for (uint32 j = 0; j < vertexCount; j++) {
			dff.read(reinterpret_cast <char *>
				 (vertex), 4*sizeof(int16));
			uint32 flag = vertex[3] & 0xFFFF;
			if (flag == 0x0000) {
				vertices.push_back(vertex[0] * vertexScale);
				vertices.push_back(vertex[1] * vertexScale);
				vertices.push_back(vertex[2] * vertexScale);
				splits[split].indices.push_back(index++);
			} else if (flag == 0x8000) {
				vertices.push_back(vertex[0] * vertexScale);
				vertices.push_back(vertex[1] * vertexScale);
				vertices.push_back(vertex[2] * vertexScale);
				splits[split].indices.push_back(index-1);
				splits[split].indices.push_back(index++);
			}
		}
		break;
	/* Texture coordinates */
	} case 0x64008001: {
		size = 2 * sizeof(float32);
		for (uint32 j = 0; j < vertexCount; j++) {
			texCoords1.push_back(readFloat32(dff));
			texCoords1.push_back(readFloat32(dff));
		}
		break;
	} case 0x6D008001: {
		size = 4 * sizeof(int16);
		int16 texCoord[4];
		for (uint32 j = 0; j < vertexCount; j++) {
			dff.read(reinterpret_cast <char *>
				 (texCoord), 4*sizeof(int16));
			/* TODO: don't know if this is correct */
			texCoords1.push_back(texCoord[0] * UVSCALE);
			texCoords1.push_back(texCoord[1] * UVSCALE);
			texCoords2.push_back(texCoord[2] * UVSCALE);
			texCoords2.push_back(texCoord[3] * UVSCALE);
		}
		break;
	} case 0x65008001: {
		size = 2 * sizeof(int16);
		int16 texCoord[2];
		for (uint32 j = 0; j < vertexCount; j++) {
			dff.read(reinterpret_cast <char *>
				 (texCoord), 2*sizeof(int16));
			texCoords1.push_back(texCoord[0] * UVSCALE);
			texCoords1.push_back(texCoord[1] * UVSCALE);
		}
		break;
	/* Vertex colors */
	} case 0x6D00C002: {
		size = 8 * sizeof(uint8);
		for (uint32 j = 0; j < vertexCount; j++) {
			vertexColors.push_back(readUInt8(dff));
			nightColors.push_back(readUInt8(dff));
			vertexColors.push_back(readUInt8(dff));
			nightColors.push_back(readUInt8(dff));
			vertexColors.push_back(readUInt8(dff));
			nightColors.push_back(readUInt8(dff));
			vertexColors.push_back(readUInt8(dff));
			nightColors.push_back(readUInt8(dff));
		}
		break;
	} case 0x6E00C002: {
		size = 4 * sizeof(uint8);
		for (uint32 j = 0; j < vertexCount; j++) {
			vertexColors.push_back(readUInt8(dff));
			vertexColors.push_back(readUInt8(dff));
			vertexColors.push_back(readUInt8(dff));
			vertexColors.push_back(readUInt8(dff));
		}
		break;
	/* Normals */
	} case 0x6E008002: case 0x6E008003: {
		size = 4 * sizeof(int8);
		int8 normal[4];
		for (uint32 j = 0; j < vertexCount; j++) {
			dff.read(reinterpret_cast <char *>
				 (normal), 4*sizeof(int8));
			normals.push_back(normal[0] * NORMALSCALE);
			normals.push_back(normal[1] * NORMALSCALE);
			normals.push_back(normal[2] * NORMALSCALE);
		}
		break;
	} case 0x6A008003: {
		size = 3 * sizeof(int8);
		int8 normal[3];
		for (uint32 j = 0; j < vertexCount; j++) {
			dff.read(reinterpret_cast <char *>
				 (normal), 3*sizeof(int8));
			normals.push_back(normal[0] * NORMALSCALE);
			normals.push_back(normal[1] * NORMALSCALE);
			normals.push_back(normal[2] * NORMALSCALE);
		}
		break;
	/* Unknown floats */
	} case 0x6C008004: case 0x6C008003: case 0x6C008001: {
		size = 4 * sizeof(float32);
		dff.seekg(vertexCount*size, ios::cur);
		break;
	}
	default:
		cout << "unknown data type: " << hex << type;
		cout << " " << filename << " " << hex << dff.tellg() << endl;
		break;
	}

	/* skip padding */
	if (vertexCount*size % 0x10 != 0)
		dff.seekg(0x10 - vertexCount*size % 0x10, ios::cur);
}

void Geometry::deleteOverlapping(vector<uint32> &typesRead, uint32 split)
{
	uint32 size;
	for (uint32 i = 0; i < typesRead.size(); i++) {
		switch (typesRead[i] & 0xFF00FFFF) {
		/* Vertices */
		case 0x68008000:
		case 0x6D008000:
			size = vertices.size();
			vertices.resize(size-2*3);

			size = splits[split].indices.size();
			splits[split].indices.resize(size-2);
			index -= 2;
			break;
		/* Texture coordinates */
		case 0x6D008001:
			size = texCoords2.size();
			texCoords2.resize(size-2*2);
		case 0x64008001:
		case 0x65008001:
			size = texCoords1.size();
			texCoords1.resize(size-2*2);
			break;
		/* Vertex colors */
		case 0x6D00C002:
			size = nightColors.size();
			nightColors.resize(size-2*4);
		case 0x6E00C002:
			size = vertexColors.size();
			vertexColors.resize(size-2*4);
			break;
		/* Normals */
		case 0x6E008002:
		case 0x6E008003:
		case 0x6A008003:
			size = normals.size();
			normals.resize(size-2*3);
			break;
		/* Unknown floats */
		case 0x6C008004: case 0x6C008003: case 0x6C008001:
			// don't do anything, we didn't even read them
			break;
		default:
			cout << "unknown data type: ";
			cout << hex <<typesRead[i] << endl;
			break;
		}
	}
}

}
