#ifndef _RENDERWARE_H_
#define _RENDERWARE_H_
#ifdef _WIN32
  #include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace rw {

typedef char int8;
typedef short int16;
typedef long int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef float float32;

extern char *filename;
extern uint32 version;

#ifdef DEBUG
	#define READ_HEADER(x)\
	header.read(dff);\
	if (header.type != (x)) {\
		cerr << filename;\
		ChunkNotFound((x), dff.tellg());\
	}
#else
	#define READ_HEADER(x)\
	header.read(dff);
#endif


enum PLATFORM_ID
{
	PLATFORM_PS2 = 4,
	PLATFORM_XBOX = 5,
	PLATFORM_D3D8 = 8,
	PLATFORM_D3D9 = 9
};
typedef enum PLATFORM_ID PLATFORM_ID;

enum CHUNK_TYPE {
        CHUNK_NAOBJECT        = 0x0,
        CHUNK_STRUCT          = 0x1,
        CHUNK_STRING          = 0x2,
        CHUNK_EXTENSION       = 0x3,
        CHUNK_CAMERA          = 0x5,
        CHUNK_TEXTURE         = 0x6,
        CHUNK_MATERIAL        = 0x7,
        CHUNK_MATLIST         = 0x8,
        CHUNK_ATOMICSECT      = 0x9,
        CHUNK_PLANESECT       = 0xA,
        CHUNK_WORLD           = 0xB,
        CHUNK_SPLINE          = 0xC,
        CHUNK_MATRIX          = 0xD,
        CHUNK_FRAMELIST       = 0xE,
        CHUNK_GEOMETRY        = 0xF,
        CHUNK_CLUMP           = 0x10,
        CHUNK_LIGHT           = 0x12,
        CHUNK_UNICODESTRING   = 0x13,
        CHUNK_ATOMIC          = 0x14,
        CHUNK_TEXTURENATIVE   = 0x15,
        CHUNK_TEXDICTIONARY   = 0x16,
        CHUNK_ANIMDATABASE    = 0x17,
        CHUNK_IMAGE           = 0x18,
        CHUNK_SKINANIMATION   = 0x19,
        CHUNK_GEOMETRYLIST    = 0x1A,
        CHUNK_ANIMANIMATION   = 0x1B,
        CHUNK_HANIMANIMATION  = 0x1B,
        CHUNK_TEAM            = 0x1C,
        CHUNK_CROWD           = 0x1D,
        CHUNK_RIGHTTORENDER   = 0x1F,
        CHUNK_MTEFFECTNATIVE  = 0x20,
        CHUNK_MTEFFECTDICT    = 0x21,
        CHUNK_TEAMDICTIONARY  = 0x22,
        CHUNK_PITEXDICTIONARY = 0x23,
        CHUNK_TOC             = 0x24,
        CHUNK_PRTSTDGLOBALDATA = 0x25,
        CHUNK_ALTPIPE         = 0x26,
        CHUNK_PIPEDS          = 0x27,
        CHUNK_PATCHMESH       = 0x28,
        CHUNK_CHUNKGROUPSTART = 0x29,
        CHUNK_CHUNKGROUPEND   = 0x2A,
        CHUNK_UVANIMDICT      = 0x2B,
        CHUNK_COLLTREE        = 0x2C,
        CHUNK_ENVIRONMENT     = 0x2D,
        CHUNK_COREPLUGINIDMAX = 0x2E,

	CHUNK_MORPH           = 0x105,
	CHUNK_SKYMIPMAP       = 0x110,
	CHUNK_SKIN            = 0x116,
	CHUNK_PARTICLES       = 0x118,
	CHUNK_HANIM           = 0x11E,
	CHUNK_MATERIALEFFECTS = 0x120,
	CHUNK_ADCPLG          = 0x134,
	CHUNK_BINMESH         = 0x50E,
	CHUNK_NATIVEDATA      = 0x510,

	CHUNK_PIPELINESET      = 0x253F2F3,
	CHUNK_SPECULARMAT      = 0x253F2F6,
	CHUNK_2DFX             = 0x253F2F8,
	CHUNK_NIGHTVERTEXCOLOR = 0x253F2F9,
	CHUNK_COLLISIONMODEL   = 0x253F2FA,
	CHUNK_REFLECTIONMAT    = 0x253F2FC,
	CHUNK_MESHEXTENSION    = 0x253F2FD,
	CHUNK_FRAME            = 0x253F2FE
};
typedef enum CHUNK_TYPE CHUNK_TYPE;

#define NORMALSCALE (float32) 1/128
#define VERTSCALE1 (float32) 1/128 /* normally used */
#define VERTSCALE2 (float32) 1/1024 /* used by cars and peds */
#define UVSCALE   (float32) 1/4096

enum { 
        FLAGS_TRISTRIP   = 0x01, 
        FLAGS_POSITIONS  = 0x02, 
        FLAGS_TEXTURED   = 0x04, 
        FLAGS_PRELIT     = 0x08, 
        FLAGS_NORMALS    = 0x10, 
        FLAGS_LIGHT      = 0x20, 
        FLAGS_MODULATEMATERIALCOLOR  = 0x40, 
        FLAGS_TEXTURED2  = 0x80
};

#define MATFX_BUMPMAP 0x1
#define MATFX_ENVMAP 0x2
#define MATFX_BUMPENVMAP
#define MATFX_DUAL 0x4
#define MATFX_UVTRANSFORM 0x5
#define MATFX_DUALUVTRANSFORM 0x6

#define FACETYPE_STRIP 0x1
#define FACETYPE_LIST 0x0

/* gta3 ps2: 302, 304, 310 
 * gta3 pc: 304, 310, 401ffff, 800ffff, c02ffff
 * gtavc ps2: c02ffff
 * gtavc pc: c02ffff, 800ffff, 1003ffff
 * gtasa: 1803ffff */

enum {
        GTA3_1 = 0x00000302,
        GTA3_2 = 0x00000304,
        GTA3_3 = 0x00000310,
        GTA3_4 = 0x0800FFFF,
        VCPS2  = 0x0C02FFFF,
        VCPC   = 0x1003FFFF,
        SA     = 0x1803FFFF
};

struct Frame
{
	float32 rotationMatrix[9];
	float32 position[3];
	int32 parent;

	/* Extensions */

	/* node name */
	std::string name;

	/* hanim */
	bool hasHAnim;
	uint32 hAnimUnknown1;
	int32 hAnimBoneId;
	uint32 hAnimBoneCount;
	uint32 hAnimUnknown2;
	uint32 hAnimUnknown3;
	std::vector<int32> hAnimBoneIds;
	std::vector<uint32> hAnimBoneNumbers;
	std::vector<uint32> hAnimBoneTypes;

	/* functions */
	void readStruct(std::ifstream &dff);
	void readExtension(std::ifstream &dff);
	uint32 writeStruct(std::ofstream &dff);
	uint32 writeExtension(std::ofstream &dff);

	Frame(void);
	~Frame(void);
};


struct Atomic
{
	uint32 frameIndex;
	uint32 geometryIndex;

	/* Extensions */

	/* right to render */
	bool hasRightToRender;
	uint32 rightToRenderVal1;
	uint32 rightToRenderVal2;

	/* particles */
	bool hasParticles;
	uint32 particlesVal;

	/* pipelineset */
	bool hasPipelineSet;
	uint32 pipelineSetVal;

	/* material fx */
	bool hasMaterialFx;
	uint32 materialFxVal;

	/* functions */
	void read(std::ifstream &dff);
	uint32 write(std::ofstream &dff);
	void readExtension(std::ifstream &dff);
	Atomic(void);
	~Atomic(void);
};

struct Texture
{
	uint32 filterFlags;
	std::string name;
	std::string maskName;

	/* Extensions */

	/* sky mipmap */
	bool hasSkyMipmap;

	/* functions */
	void read(std::ifstream &dff);
	uint32 write(std::ofstream &dff);
	void readExtension(std::ifstream &dff);
	Texture(void);
	~Texture(void);
};

struct Material
{
	uint32 flags;
	uint8 color[4];
	uint32 unknown;
	bool hasTex;
	float32 surfaceProps[3]; /* ambient, specular, diffuse */

	Texture texture;

	/* Extensions */

	/* right to render */
	bool hasRightToRender;
	uint32 rightToRenderVal1;
	uint32 rightToRenderVal2;

	/* matfx */
	// todo
	Texture reflection;

	/* reflection mat */
	bool hasReflectionMat;
	float32 reflectionChannelAmount[4];
	float32 reflectionIntensity;

	/* specular mat */
	bool hasSpecularMat;
	float32 specularLevel;
	std::string specularName;

	/* uv anim */
	// to do

	/* functions */
	void read(std::ifstream &dff);
	void readExtension(std::ifstream &dff);
	uint32 write(std::ofstream &dff);
	Material(void);
	~Material(void);
};

struct MeshExtension
{
	uint32 unknown;

	std::vector<float32> vertices;
	std::vector<float32> texCoords;
	std::vector<uint8> vertexColors;
	std::vector<uint16> faces;
	std::vector<uint16> assignment;

	std::vector<std::string> textureName;
	std::vector<std::string> maskName;
	std::vector<float32> unknowns;
};

struct Split
{
	uint32 matIndex;
	std::vector<uint32> indices;	
};

struct Geometry
{
	uint32 flags;
	uint32 numUVs;
	bool hasNativeGeometry;

	uint32 vertexCount;
	std::vector<float32> vertices;
	std::vector<float32> normals;
	std::vector<uint16> faces;
	std::vector<uint8> vertexColors;
	std::vector<float32> texCoords1;
	std::vector<float32> texCoords2;

	float32 boundingSphere[4];
	uint32 bsPos;
	uint32 bsNor;

	std::vector<Material> materialList;

	/* Extensions */

	/* bin mesh */
	uint32 faceType;
	uint32 numIndices;
	std::vector<Split> splits;

	/* skin */
	bool hasSkin;
	uint32 boneCount;
	uint32 specialIndexCount;
	uint32 unknown1;
	uint32 unknown2;
	std::vector<uint8> specialIndices;
	std::vector<uint32> vertexBoneIndices;
	std::vector<float32> vertexBoneWeights;
	std::vector<float32> inverseMatrices;

	/* mesh extension */
	bool hasMeshExtension;
	MeshExtension *meshExtension;

	/* night vertex colors */
	bool hasNightColors;
	uint32 nightColorsUnknown;
	std::vector<uint8> nightColors;

	/* 2dfx */
	// to do

	/* morph (only flag) */
	bool hasMorph;

	/* functions */
	void read(std::ifstream &dff);
	uint32 write(std::ofstream &dff);
	void readExtension(std::ifstream &dff);

	void readMeshExtension(std::ifstream &dff);
	uint32 writeMeshExtension(std::ofstream &dff);

	void readXboxNativeData(std::ifstream &dff);
	void readPs2NativeData(std::ifstream &dff);
	void deleteOverlapping(std::vector<uint32> &typesRead, uint32 split);
	void readData(uint32 vertexCount, uint32 type,
                      uint32 split, std::ifstream &dff);

	bool isDegenerateFace(uint32 i, uint32 j, uint32 k);
	void generateFaces(void);

	Geometry(void);
	~Geometry(void);

	void writeMsh(std::ofstream &msh);
};

struct Clump
{
	std::vector<Atomic> atomicList;
	std::vector<Frame> frameList;
	std::vector<Geometry> geometryList;

	/* Extensions */
	/* collision file */
	// to do

	/* functions */
	bool read(std::ifstream &dff);
	void readExtension(std::ifstream &dff);
	void write(std::ofstream &dff);
	Clump(void);
	~Clump(void);

	void writeMsh(std::ofstream &msh);
};

struct HeaderInfo
{
	uint32 type;
	uint32 length;
	uint32 version;
	void read(std::ifstream &rw);
	uint32 write(std::ofstream &rw);
};

void ChunkNotFound(CHUNK_TYPE chunk, uint32 address);
uint32 writeInt8(int8 tmp, std::ofstream &rw);
uint32 writeUInt8(uint8 tmp, std::ofstream &rw);
uint32 writeInt16(int16 tmp, std::ofstream &rw);
uint32 writeUInt16(uint16 tmp, std::ofstream &rw);
uint32 writeInt32(int32 tmp, std::ofstream &rw);
uint32 writeUInt32(uint32 tmp, std::ofstream &rw);
uint32 writeFloat32(float32 tmp, std::ofstream &rw);
int8 readInt8(std::ifstream &rw);
uint8 readUInt8(std::ifstream &rw);
int16 readInt16(std::ifstream &rw);
uint16 readUInt16(std::ifstream &rw);
int32 readInt32(std::ifstream &rw);
uint32 readUInt32(std::ifstream &rw);
float32 readFloat32(std::ifstream &rw);

}

#endif
