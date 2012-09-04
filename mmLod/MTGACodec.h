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
#ifndef MTGACODEC_H
#define MTGACODEC_H

#include <OgreImageCodec.h>

/**
* Implements the ogre image codec interface for loading TGA files
*/
namespace Ogre {
class MTGACodec : public Ogre::ImageCodec
{
public:
	/// Static method to startup and register the codec
	static void startup();
	/// Static method to shutdown and unregister codec
	static void shutdown();

public:
	MTGACodec();
	virtual ~MTGACodec();

	Ogre::DataStreamPtr code(Ogre::MemoryDataStreamPtr& input, Ogre::Codec::CodecDataPtr& pData) const;

	void codeToFile(Ogre::MemoryDataStreamPtr& input, const Ogre::String& outFileName, CodecDataPtr& pData) const;

	DecodeResult decode(Ogre::DataStreamPtr& input) const;

	Ogre::String magicNumberToFileExt(const char* magicNumberPtr, size_t maxbytes) const;

	virtual Ogre::String getType() const;

private:
	static MTGACodec* msInstance;

	//bool loadImage(Ogre::MemoryDataStreamPtr& data, Ogre::ImageCodec::ImageData* imgData, Ogre::MemoryDataStreamPtr& pixelData) const;
};
}
#endif
