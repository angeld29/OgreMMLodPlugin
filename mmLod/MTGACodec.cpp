/*
 *  MM Mapview
 *  Copyright (C) 2011  Angel (angeldeath29@gmail.com)
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "OgreStableHeaders.h"
#include "mm_defs.h"
#include "MTGACodec.h"
#include "OgreLogManager.h"
#include "OgreException.h"
#include "OgreImage.h"
#include "OgreStringConverter.h"
//#include "OgreRoot.h"
//#include "OgreRenderSystem.h"

namespace Ogre {
MTGACodec* MTGACodec::msInstance = 0;
#pragma pack(push,1)
struct mtga_header_s{
	uint32 magic, version;
	uint32 image0Size, compressedSize;
	uint16 width,height;
	uint16 widthpow,heightpow,maxx,maxy;
	uint32 unk1;
	uint32 uncompressedSize, flags;
};
struct mtga_pal_s{
	uint8 r,g,b;
};
#pragma pack(pop)
void MTGACodec::startup(void)
{
	if (!msInstance)
	{
		Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "MTGA codec registering");

		msInstance = OGRE_NEW MTGACodec();
		Ogre::Codec::registerCodec(msInstance);
	}

}

void MTGACodec::shutdown(void)
{
	if(msInstance)
	{
		Ogre::Codec::unRegisterCodec(msInstance);
		OGRE_DELETE msInstance;
		msInstance = 0;
	}

}

MTGACodec::MTGACodec()
{
}

MTGACodec::~MTGACodec()
{
}

Ogre::String MTGACodec::getType() const
{
	return "mtga";
}

Ogre::DataStreamPtr MTGACodec::code(Ogre::MemoryDataStreamPtr& input, Ogre::Codec::CodecDataPtr& pData) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
		"MTGA encoding not supported",
		"MTGACodec::code" ) ;
}

void MTGACodec::codeToFile(Ogre::MemoryDataStreamPtr& input,
						  const Ogre::String& outFileName, Ogre::Codec::CodecDataPtr& pData) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
		"MTGA encoding not supported",
		"MTGACodec::code" ) ;
}

Ogre::Codec::DecodeResult MTGACodec::decode(Ogre::DataStreamPtr& stream) const
{
	// Read 4 character code
	mtga_header_s hdr;
	//uint32 fileType;
	stream->read(&hdr, sizeof(hdr));
	if (LodResource_Bitmap != hdr.magic)
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This is not a MTGA file!", "MTGACodec::decode");
	}
	
	mtga_pal_s pal[256];
	assert(stream->size() == sizeof(hdr)+hdr.uncompressedSize + sizeof(pal));

	stream->seek(sizeof(mtga_header_s)+hdr.uncompressedSize);
	stream->read(pal,sizeof(pal));

	bool isTransparent = false;
	mtga_pal_s& clr = pal[0];
	if( (clr.r == 0 && clr.g >250 && clr.b > 250) || (clr.r > 250 && clr.g ==0 && clr.b > 250))
		isTransparent = true;


	Ogre::ImageCodec::ImageData* imgData = OGRE_NEW Ogre::ImageCodec::ImageData();
	imgData->format = PF_BYTE_BGRA;
	imgData->width = hdr.width;
	imgData->height = hdr.height;
	imgData->num_mipmaps = 3;
	imgData->depth = 1;

	imgData->size = Image::calculateSize(imgData->num_mipmaps, 1, 
			imgData->width, imgData->height, imgData->depth, imgData->format);
	
	Ogre::MemoryDataStreamPtr pixelData;
	
	pixelData.bind(OGRE_NEW MemoryDataStream(imgData->size));

	// Now deal with the data
	unsigned char* destPtr = pixelData->getPtr();
	stream->seek(sizeof(mtga_header_s));
	size_t width = hdr.width;
	size_t height = hdr.height;
	for(size_t mip = 0; mip <= imgData->num_mipmaps; ++mip)
	{
			for (size_t y = 0; y < height; y ++)
			{
				for (size_t x = 0; x < width; x ++)
				{
					unsigned char idx;
					stream->read(&idx,1);
					mtga_pal_s& clr = pal[idx];
					assert(destPtr-pixelData->getPtr() < imgData->size);
					
					*destPtr++ = clr.b;
					*destPtr++ = clr.g;
					*destPtr++ = clr.r;
					*destPtr++ = (idx == 0 && isTransparent)?0:255;
				}
			}
			width /=2;
			height /=2;

	}
	


	DecodeResult ret;
	ret.first = pixelData;
	ret.second = CodecDataPtr(imgData);
	return ret;
}

Ogre::String MTGACodec::magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const
{
	if (maxbytes >= sizeof(LodResourceType))
	{
//		LodResourceType fileType;
//		memcpy(&fileType, magicNumberPtr, sizeof(LodResourceType));

		if ( LodResource_Bitmap == *(LodResourceType*)magicNumberPtr )
		{
			return Ogre::String("mtga");
		}
	}

	return Ogre::StringUtil::BLANK;

}
}
