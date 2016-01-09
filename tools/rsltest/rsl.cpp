#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <new>

#include <args.h>
#include <rw.h>
#include <src/gtaplg.h>

using namespace std;
using namespace rw;
#include "rsl.h"

void rslMaterialListStreamRead(Stream *stream, RslMaterialList *matlist);
void hanimRead(Stream *stream, RslFrame *f);
void matfxRead(Stream *stream, RslMaterial *mat);

void
RslMatrixSetIdentity(RslMatrix *matrix)
{
	memset(matrix, 0, sizeof(RslMatrix));
	matrix->right.x = 1.0f;
	matrix->up.y = 1.0f;
	matrix->at.z = 1.0f;
}

void
rslObjectHasFrameSetFrame(RslObjectHasFrame *object, RslFrame *f)
{
	if(object->object.parent){
		object->lFrame.prev->next = object->lFrame.next;
		object->lFrame.next->prev = object->lFrame.prev;
	}
	object->object.parent = f;
	if(f){
		object->lFrame.prev = &f->objectList.link;
		object->lFrame.next = f->objectList.link.next;
		f->objectList.link.next->prev = &object->lFrame;
		f->objectList.link.next = &object->lFrame;

		f->root->object.privateFlags |= 1;
		f->object.privateFlags |= 2;
	}
}

RslFrame*
RslFrameCreate(void)
{
	RslFrame *f = new RslFrame;
	rslObjectInitialize(&f->object, 0, 0);
	f->objectList.link.prev = &f->objectList.link;
	f->objectList.link.next = &f->objectList.link;
	RslMatrixSetIdentity(&f->modelling);
	RslMatrixSetIdentity(&f->ltm);
	f->child = NULL;
	f->next = NULL;
	f->root = f;

	f->nodeId = -1;
	f->hier = NULL;

	f->name = NULL;

	f->hierId = 0;
	return f;
}

RslFrame*
RslFrameAddChild(RslFrame *parent, RslFrame *child)
{
	RslFrame *p = (RslFrame*)child->object.parent;
	assert(p == NULL);
	child->next = parent->child;
	parent->child = child;
	child->object.parent = parent;
	child->root = parent->root;
	child->root->object.privateFlags |= 1;
	child->object.privateFlags |= 2;
	return parent;
}

RslFrame*
RslFrameForAllChildren(RslFrame *frame, RslFrameCallBack callBack, void *data)
{
	for(RslFrame *child = frame->child;
	    child;
	    child = child->next)
		if(callBack(child, data) == NULL)
			break;
	return frame;
}

struct StreamFrame
{
	RslV3d right, up, at, pos;
	int32 parent;
	int32 flags;
};

void
rwFrameListStreamRead(Stream *stream, rslFrameList *framelist)
{
	uint32 length;
	StreamFrame strfrm;
	RslFrame *f;

	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	stream->read(&framelist->numFrames, 4);
	framelist->frames = new RslFrame*[framelist->numFrames];
	for(int32 i = 0; i < framelist->numFrames; i++){
		stream->read(&strfrm, sizeof(strfrm));
		f = RslFrameCreate();
		f->modelling.right = strfrm.right;
		f->modelling.up = strfrm.up;
		f->modelling.at = strfrm.at;
		f->modelling.pos = strfrm.pos;
		framelist->frames[i] = f;
		if(strfrm.parent >= 0)
			RslFrameAddChild(framelist->frames[strfrm.parent], f);
	}
	ChunkHeaderInfo header;
	for(int32 i = 0; i < framelist->numFrames; i++){
		f = framelist->frames[i];
		assert(findChunk(stream, ID_EXTENSION, &length, NULL));
		while(length){
			readChunkHeaderInfo(stream, &header);
			if(header.type == ID_HANIMPLUGIN){
				hanimRead(stream, f);
			}else if(header.type == gta::ID_NODENAME){
				f->name = new char[header.length+1];
				memset(f->name, 0, header.length+1);
				stream->read(f->name, header.length);
				f->name[header.length] = '\0';
			}else{
				stream->seek(header.length);
			}
			length -= 12 + header.length;
		}
	}
}

RslHAnimHierarchy*
RslHAnimHierarchyCreate(int32 numNodes, uint32 *nodeFlags, int32 *nodeIDs, int32 flags, int32 maxKeySize)
{
	RslHAnimHierarchy *hier = new RslHAnimHierarchy;
	memset(hier, 0, sizeof(RslHAnimHierarchy));
	if(maxKeySize < 0x24)
		maxKeySize = 0x24;
	hier->flags = flags;
	hier->numNodes = numNodes;
	hier->parentFrame = 0;
	hier->maxKeyFrameSize = maxKeySize;
	hier->currentKeyFrameSize = 0x24;

	int32 msz = numNodes*0x40 + 0x3f;
	uint8 *p = new uint8[msz];
	memset(p, 0, msz);
	hier->pMatrixArrayUnaligned = p;
	uintptr ip = (uintptr)p;
	ip &= ~0x3f;
	hier->pMatrixArray = (float32*)ip;

	hier->pNodeInfo = new RslHAnimNodeInfo[numNodes];
	for(int32 i = 0; i < numNodes; i++){
		hier->pNodeInfo[i].id = nodeIDs[i];
		hier->pNodeInfo[i].index = i;
		hier->pNodeInfo[i].flags = nodeFlags[i];
		hier->pNodeInfo[i].frame = NULL;
	}
	hier->parentHierarchy = hier;
	return hier;
}

static void
hanimRead(Stream *stream, RslFrame *f)
{
	int32 version;
	int32 id;
	int32 numNodes;
	int32 flags;
	int32 maxKeySize;
	uint32 *nodeFlags;
	int32 *nodeIDs;

	version = stream->readI32();
	assert(version == 0x100);
	id = stream->readI32();
	f->nodeId = id;
	numNodes = stream->readI32();
	if(numNodes == 0)
		return;

	flags = stream->readI32();
	maxKeySize = stream->readI32();
	nodeFlags = new uint32[numNodes];
	nodeIDs = new int32[numNodes];
	memset(nodeFlags, 0, numNodes*4);
	memset(nodeIDs, 0, numNodes*4);
	for(int32 i = 0; i < numNodes; i++){
		nodeIDs[i] = stream->readI32();
		stream->readI32();
		nodeFlags[i] = stream->readI32();
	}
	f->hier = RslHAnimHierarchyCreate(numNodes, nodeFlags,
	                                  nodeIDs, flags, maxKeySize);
	delete[] nodeFlags;
	delete[] nodeIDs;
}


RslAtomic*
RslAtomicCreate(void)
{
	RslAtomic *a = new RslAtomic;
	memset(a, 0, sizeof(RslAtomic));
	rslObjectInitialize(&a->object, 1, 0);
	a->object.object.flags = 5;
	a->object.object.privateFlags |= 1;
	rslObjectHasFrameSetFrame(&a->object, NULL);
	return a;
}

RslAtomic*
RslAtomicSetFrame(RslAtomic *atomic, RslFrame *frame)
{
	rslObjectHasFrameSetFrame(&atomic->object, frame);
	return atomic;
}

RslAtomic*
RslAtomicStreamRead(Stream *stream, rslFrameList *framelist)
{
	uint32 length;
	int32 buf[4];
	RslAtomic *a;
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	stream->read(buf, 16);
	a = RslAtomicCreate();
	a->object.object.flags = buf[2];
	assert(findChunk(stream, ID_GEOMETRY, NULL, NULL));

	RslPS2ResEntryHeader res, *rp;
	stream->read(&res, sizeof(RslPS2ResEntryHeader));
	RslGeometry *g = RslGeometryCreatePS2(res.size & 0xFFFFF);
	rp = (RslPS2ResEntryHeader*)(g+1);
	*rp++ = res;
	stream->read(rp, (res.size&0xFFFFF)-sizeof(RslPS2ResEntryHeader));

	rslMaterialListStreamRead(stream, &g->matList);
	a->geometry = g;
	RslAtomicSetFrame(a, framelist->frames[buf[0]]);

	// This is not how it's done in LCS, they got extensions wrong :(
	// Geometry
	ChunkHeaderInfo header;
	assert(findChunk(stream, ID_EXTENSION, &length, NULL));
	while(length){
		readChunkHeaderInfo(stream, &header);
		if(header.type == ID_SKIN){
			g->skin = RslSkinStreamRead(stream, g);
		}else
			stream->seek(header.length);
		length -= 12 + header.length;
	}

	// Atomic
	assert(findChunk(stream, ID_EXTENSION, &length, NULL));
	while(length){
		readChunkHeaderInfo(stream, &header);
		stream->seek(header.length);
		length -= 12 + header.length;
	}
	return a;
}

RslSkin*
RslSkinStreamRead(Stream *stream, RslGeometry *g)
{
	uint32 info;
	RslSkin *skin = new RslSkin;
	memset(skin, 0, sizeof(RslSkin));
	info = stream->readU32();
	// LCS is different here, doesn't matter
	info &= 0xFF;
	skin->numBones = info;
	skin->numUsedBones = info;
	skin->invMatrices = new float[skin->numBones*16];
	stream->read(skin->invMatrices, skin->numBones*16*4);
	return skin;
}



RslClump*
RslClumpCreate(void)
{
	RslClump *clump = new RslClump;
	rslObjectInitialize(&clump->object, 2, 0);
	clump->atomicList.link.prev = &clump->atomicList.link;
	clump->atomicList.link.next = &clump->atomicList.link;
	return clump;
}

RslClump*
RslClumpStreamRead(Stream *stream)
{
	uint32 version, length;
	int32 buf[3];
	int32 numAtomics;
	rslFrameList framelist;
	RslClump *clump;

	assert(findChunk(stream, ID_STRUCT, NULL, &version));
	if(version > 0x33000){
		stream->read(buf, 12);
		numAtomics = buf[0];
	}else
		stream->read(&numAtomics, 4);

	clump = RslClumpCreate();
	assert(findChunk(stream, ID_FRAMELIST, NULL, NULL));
	rwFrameListStreamRead(stream, &framelist);
	clump->object.parent = framelist.frames[0];

	for(int32 i = 0; i < numAtomics; i++){
		assert(findChunk(stream, ID_ATOMIC, NULL, &version));
		RslAtomic *a = RslAtomicStreamRead(stream, &framelist);
		RslClumpAddAtomic(clump, a);
	}

	ChunkHeaderInfo header;
	assert(findChunk(stream, ID_EXTENSION, &length, NULL));
	while(length){
		readChunkHeaderInfo(stream, &header);
		stream->seek(header.length);
		length -= 12 + header.length;
	}
	return clump;
}

RslClump*
RslClumpAddAtomic(RslClump *clump, RslAtomic *a)
{
	a->inClumpLink.prev = &clump->atomicList.link;
	a->inClumpLink.next = clump->atomicList.link.next;
	clump->atomicList.link.next->prev = &a->inClumpLink;
	clump->atomicList.link.next = &a->inClumpLink;
	a->clump = clump;
	return clump;
}

RslClump*
RslClumpForAllAtomics(RslClump *clump, RslAtomicCallBack callback, void *pData)
{
	RslAtomic *a;
	RslLLLink *link;
	for(link = rslLLLinkGetNext(&clump->atomicList.link);
	    link != &clump->atomicList.link;
	    link = link->next){
		a = rslLLLinkGetData(link, RslAtomic, inClumpLink);
		if(callback(a, pData) == NULL)
			break;
	}
	return clump;
}

RslGeometry*
RslGeometryCreatePS2(uint32 sz)
{
	sz += sizeof(RslGeometry);
	RslGeometry *g = (RslGeometry*)new uint8[sz];
	memset(g, 0, sz);
	rslObjectInitialize(&g->object, 8, 0);
	g->refCount = 1;
	return g;
}

RslGeometry*
RslGeometryForAllMaterials(RslGeometry *geometry, RslMaterialCallBack fpCallBack, void *pData)
{
	for(int32 i = 0; i < geometry->matList.numMaterials; i++)
		if(fpCallBack(geometry->matList.materials[i], pData) == NULL)
			break;
	return geometry;
}

struct RslMaterialChunkInfo
{
    int32             flags;
    RslRGBA           color;    // used
    int32             unused;
    bool32            textured; // used
    SurfaceProperties surfaceProps;
};

RslMaterial*
RslMaterialCreate(void)
{
	RslMaterial *mat = new RslMaterial;
	mat->texture = NULL;
	mat->color = RslRGBA{255, 255, 255, 255};
	mat->refCount = 1;
	mat->matfx = NULL;
	return mat;
}

RslMaterial*
RslMaterialStreamRead(Stream *stream)
{
	uint32 length;
	RslMaterialChunkInfo chunk;
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	stream->read(&chunk, sizeof(chunk));
	RslMaterial *mat = RslMaterialCreate();
	mat->color = chunk.color;
	if(chunk.textured)
		mat->texture = RslTextureStreamRead(stream);

	ChunkHeaderInfo header;
	assert(findChunk(stream, ID_EXTENSION, &length, NULL));
	while(length){
		readChunkHeaderInfo(stream, &header);
		if(header.type == ID_MATFX)
			matfxRead(stream, mat);
		else
			stream->seek(header.length);
		length -= 12 + header.length;
	}
	return mat;
}

void
RslMatFXMaterialSetEffects(RslMaterial *mat, int32 effect)
{
	if(effect != MatFX::NOTHING){
		if(mat->matfx == NULL){
			mat->matfx = new RslMatFX;
			memset(mat->matfx, 0, sizeof(RslMatFX));
		}
		mat->matfx->effectType = effect;
	}else{
		RslMatFX *matfx = mat->matfx;
		if(matfx->env.texture)
			RslTextureDestroy(matfx->env.texture);
		delete matfx;
		mat->matfx = NULL;
	}
}

void
matfxRead(Stream *stream, RslMaterial *mat)
{
	int32 effect = stream->readI32();
	RslMatFXMaterialSetEffects(mat, effect);
	if(effect == MatFX::BUMPMAP){
		stream->seek(12);
		return;
	}
	RslMatFX *mfx = mat->matfx;
	int32 type = stream->readI32();
	float32 coef;
	int32 fbalpha;
	switch(type){
	case MatFX::ENVMAP:
		coef = stream->readF32();
		fbalpha = stream->readI32();
		mfx->env.frame = NULL;
		mfx->env.texture = RslTextureStreamRead(stream);
		mfx->env.intensity = coef;
		break;
	case MatFX::BUMPMAP:
		coef = stream->readF32();
		break;
	}
}

void
rpMaterialListAppendMaterial(RslMaterialList *matlist, RslMaterial *mat)
{
	if(matlist->space <= matlist->numMaterials){
		matlist->space += 16;
		RslMaterial **mats = matlist->materials;
		matlist->materials = new RslMaterial*[matlist->space];
		if(matlist->space-16)
			memcpy(matlist->materials, mats, (matlist->space+16)*sizeof(RslMaterial*));
		delete[] mats;
	}
	matlist->materials[matlist->numMaterials++] = mat;
}

void
rslMaterialListStreamRead(Stream *stream, RslMaterialList *matlist)
{
	int32 numMaterials;
	RslMaterial *mat;
	assert(findChunk(stream, ID_MATLIST, NULL, NULL));
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	numMaterials = stream->readI32();
	int32 *refs = new int32[numMaterials];
	stream->read(refs, 4*numMaterials);
	for(int32 i = 0; i < numMaterials; i++){
		assert(refs[i] < 0);
		assert(findChunk(stream, ID_MATERIAL, NULL, NULL));
		mat = RslMaterialStreamRead(stream);
		rpMaterialListAppendMaterial(matlist, mat);
	}
	delete[] refs;
}

RslTexDictionary*
RslTexDictionaryCreate(void)
{
	RslTexDictionary *dict = new RslTexDictionary;
	memset(dict, 0, sizeof(RslTexDictionary));
	rslObjectInitialize(&dict->object, 6, 0);
	dict->texturesInDict.link.prev = &dict->texturesInDict.link;
	dict->texturesInDict.link.next = &dict->texturesInDict.link;
	return dict;
}

RslTexture*
RslTexDictionaryAddTexture(RslTexDictionary *dict, RslTexture *tex)
{
	if(tex->dict){
		tex->lInDictionary.prev->next = tex->lInDictionary.next;
		tex->lInDictionary.next->prev = tex->lInDictionary.prev;
	}
	tex->dict = dict;
	tex->lInDictionary.prev = &dict->texturesInDict.link;
	tex->lInDictionary.next = dict->texturesInDict.link.next;
	dict->texturesInDict.link.next->prev = &tex->lInDictionary;
	dict->texturesInDict.link.next = &tex->lInDictionary;
	return tex;
}

RslTexDictionary*
RslTexDictionaryForAllTextures(RslTexDictionary *dict, RslTextureCallBack fpCallBack, void *pData)
{
	RslTexture *t;
	RslLLLink *link;
	for(link = rslLLLinkGetNext(&dict->texturesInDict.link);
	    link != &dict->texturesInDict.link;
	    link = link->next){
		t = rslLLLinkGetData(link, RslTexture, lInDictionary);
		if(fpCallBack(t, pData) == NULL)
			break;
	}
	return dict;
}

// TODO!
void
RslTextureDestroy(RslTexture *texture)
{
	delete texture;
}

RslTexture*
RslTextureCreate(RslRaster *raster)
{
	RslTexture *tex = new RslTexture;
	memset(tex, 0, sizeof(RslTexture));
	tex->raster = raster;
	return tex;
}

RslTexture*
RslTextureStreamRead(Stream *stream)
{
	uint32 length;
	RslTexture *tex = RslTextureCreate(NULL);
	assert(findChunk(stream, ID_TEXTURE, NULL, NULL));
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	stream->readI32();	// filter addressing
	assert(findChunk(stream, ID_STRING, &length, NULL));
	stream->read(tex->name, length);
	assert(findChunk(stream, ID_STRING, &length, NULL));
	stream->read(tex->mask, length);

	ChunkHeaderInfo header;
	assert(findChunk(stream, ID_EXTENSION, &length, NULL));
	while(length){
		readChunkHeaderInfo(stream, &header);
		stream->seek(header.length);
		length -= 12 + header.length;
	}
	return tex;
}

static uint32
guessSwizzling(uint32 w, uint32 h, uint32 d, uint32 mipmaps)
{
	uint32 swiz = 0;
	for(uint32 i = 0; i < mipmaps; i++){
		switch(d){
		case 4:
			if(w >= 32 && h >= 16)
				swiz |= 1<<i;
			break;
		case 8:
			if(w >= 16 && h >= 4)
				swiz |= 1<<i;
			break;
		}
		w /= 2;
		h /= 2;
	}
	return swiz;
}

RslRaster*
RslCreateRasterPS2(uint32 w, uint32 h, uint32 d, uint32 mipmaps)
{
	RslRasterPS2 *r;
	r = new RslRasterPS2;
	uint32 tmp, logw = 0, logh = 0;
	for(tmp = 1; tmp < w; tmp <<= 1)
		logw++;
	for(tmp = 1; tmp < h; tmp <<= 1)
		logh++;
	r->flags = 0;
	r->flags |= logw&0x3F;
	r->flags |= (logh&0x3F)<<6;
	r->flags |= d << 12;
	r->flags |= mipmaps << 20;
	uint32 swiz = guessSwizzling(w, h, d, mipmaps);
	r->flags |= swiz << 24;
	return (RslRaster*)r;
}

RslTexture*
RslReadNativeTexturePS2(Stream *stream)
{
	RslPs2StreamRaster rasterInfo;
	uint32 len;
	uint32 buf[2];
	RslTexture *tex = RslTextureCreate(NULL);
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	stream->read(buf, sizeof(buf));
	assert(buf[0] == 0x00505350); /* "PSP\0" */
	assert(findChunk(stream, ID_STRING, &len, NULL));
	stream->read(tex->name, len);
	assert(findChunk(stream, ID_STRING, &len, NULL));
	stream->read(tex->mask, len);
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	assert(findChunk(stream, ID_STRUCT, &len, NULL));
	stream->read(&rasterInfo, sizeof(rasterInfo));
	assert(findChunk(stream, ID_STRUCT, &len, NULL));
	tex->raster = RslCreateRasterPS2(rasterInfo.width,
		rasterInfo.height, rasterInfo.depth, rasterInfo.mipmaps);
	tex->raster->ps2.data = new uint8[len];
	stream->read(tex->raster->ps2.data, len);
	assert(findChunk(stream, ID_EXTENSION, &len, NULL));
	stream->seek(len);
	return tex;
}

RslTexDictionary*
RslTexDictionaryStreamRead(Stream *stream)
{
	assert(findChunk(stream, ID_STRUCT, NULL, NULL));
	int32 numTex = stream->readI32();
	RslTexDictionary *txd = RslTexDictionaryCreate();
	for(int32 i = 0; i < numTex; i++){
		assert(findChunk(stream, ID_TEXTURENATIVE, NULL, NULL));
		RslTexture *tex = RslReadNativeTexturePS2(stream);
		RslTexDictionaryAddTexture(txd, tex);
	}
	return txd;
}