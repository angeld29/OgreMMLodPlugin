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

//#include "stdafx.h"
#include "OgreLodArchive.h"
#include <OgreLogManager.h>
#include <zlib.h>
#include <OgreException.h>
#include <OgreStringVector.h>
#include "OgreRoot.h"

namespace Ogre {
//-----------------------------------------------------------------------
//LODArchive
//-----------------------------------------------------------------------
LODArchive::LODArchive (const Ogre::String& name, const String& archType )
: Archive(name, archType), version(LOD_MM6), lod_type(LOD_DEFAULT), stream(new std::ifstream())
{
}
//-----------------------------------------------------------------------
LODArchive::~LODArchive ()
{
	unload();
	delete stream;
}
//-----------------------------------------------------------------------
void LODArchive::load()
{
	
	std::ios::openmode mode = std::ios::in | std::ios::binary;
	//stream = Ogre::SharedPtr<std::ifstream>(OGRE_NEW std::ifstream());
	stream->open(mName.c_str(),mode);
	if(stream->fail())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, 
			"Can'open file: " + mName, 
			"LODArchive::load");
	}
	char strbuff[0x41];
	std::fill(strbuff,strbuff+sizeof(strbuff),0);
	int namesize = 0x10;
	stream->read(strbuff,0x10);
	if( strcmp(strbuff,"LOD"))
	{
		stream->close();
		OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, 
			"Incorrect LOD File: " + mName, 
			"LODArchive::load");
	}

	if( !strcmp(strbuff+4,"MMVIII"))
	{
		version = LOD_MM8;
		namesize  = 0x40;
	}
	stream->seekg(0x100);
	stream->read(reinterpret_cast<char*>(&header), 0x20);
	dirname = header.dirname;

	if( Ogre::StringUtil::match(dirname,"maps",false) )
		lod_type = LOD_MAP;
	if( Ogre::StringUtil::match(dirname,"sprites08",false) )
		lod_type = LOD_SPRITE;
	files.reserve(header.num);
	for(int i =0; i< header.num; i++)
	{
		lod_item_s itm;
		stream->read(strbuff,namesize);
		itm.fname =  strbuff;
		stream->read(reinterpret_cast<char*>(&itm.off),4);
		stream->read(reinterpret_cast<char*>(&itm.size),4);
		stream->read(reinterpret_cast<char*>(&itm.hz),4);
		if( version == LOD_MM6)
		{
			stream->read(reinterpret_cast<char*>(&itm.num),4);
		}else
		{
			itm.num = 0;
		}
		files.push_back(itm);
	}
	Ogre::LogManager::getSingleton().logMessage(
		"LODArchive::load "+ mName +":" + Ogre::StringConverter::toString(files.size()));
}
//-----------------------------------------------------------------------
void LODArchive::unload()
{
	files.clear();
	stream->close();
}
//-----------------------------------------------------------------------
Ogre::DataStreamPtr LODArchive::open(const Ogre::String& filename, bool readOnly ) const
{
	std::vector<lod_item_s>::const_iterator it = std::find_if(files.begin(),files.end(), find_filename(filename));
	if(it == files.end())
	{
		Ogre::LogManager::getSingleton().logMessage(
                mName + " - Unable to open file " + filename);
                
			// return null pointer
			return Ogre::DataStreamPtr();
	}
	char* buff = OGRE_ALLOC_T(char,(*it).size, Ogre::MEMCATEGORY_GENERAL);
	stream->seekg((*it).off+header.off);
	stream->read(buff,(*it).size);
	Ogre::DataStreamPtr ptr; 
	switch(lod_type)
	{
	case LOD_MAP:
		ptr = loadDataMap(buff,(*it).size);
		break;
	case LOD_SPRITE:
		ptr = loadDataSprite(buff,(*it).size);
		break;
	case LOD_DEFAULT:
		ptr = loadDataDefault(buff,(*it).size);
		break;
	default:
		ptr = Ogre::DataStreamPtr(OGRE_NEW Ogre::MemoryDataStream(buff,(*it).size, true));
		break;
	}
	if( ptr.isNull() )
	{
		Ogre::LogManager::getSingleton().logMessage(
                mName + " - Error load file " + filename);
		
		ptr = Ogre::DataStreamPtr(OGRE_NEW Ogre::MemoryDataStream(buff,(*it).size, true));
	}
	else
		OGRE_FREE(buff,Ogre::MEMCATEGORY_GENERAL);
	return ptr;
}
//-----------------------------------------------------------------------
LodResourceType LODArchive::GetResourceType(const char*buff) const
{
	if(lod_type == LOD_SPRITE )
		return LodResource_Sprite;
	if(lod_type == LOD_MAP)
		return LodResource_Map;
	int len = (version == 8)? 0x40:0x10;
	int i = 0;
	while(i < len && buff[i])i++;
	i++;
	if( i+4 < len)
	{
		if( (buff[i] =='t' || buff[i] =='T' ) &&
			(buff[i+1] =='g' || buff[i+1] =='G' ) &&
			(buff[i+2] =='a' || buff[i+2] =='A' ) &&
			(buff[i+3] == 0)
			)return LodResource_Bitmap;
	}

	return LodResource_Default;

}
//-----------------------------------------------------------------------
Ogre::DataStreamPtr LODArchive::loadDataDefault(const char*buff, int size) const
{
	int baseoff = (version == LOD_MM6)?0x10:0x40;
	if( size < baseoff + 0x20 )
	{
		Ogre::LogManager::getSingleton().logMessage(
                mName + " LODArchive::loadDataDefault: too small data ");
		return Ogre::DataStreamPtr();
	}
	int*hdr = (int*)(buff + baseoff);
	int psize = hdr[1];
	unsigned long unpsize1 = hdr[0];
	unsigned long unpsize2 = hdr[6];
	/*if( unpsize2 && unpsize2 < unpsize1)
	{
		Ogre::LogManager::getSingleton().logMessage(
                mName + " LODArchive::loadDataDefault: unpsize1 < unpsize2 "+ Ogre::StringConverter::toString(unpsize1)+"<" + Ogre::StringConverter::toString(unpsize2));
	}*/
	unsigned long unpsize = unpsize2;
	if(!unpsize)
		unpsize=psize;
	unsigned char* pdata = (unsigned char*) buff + 0x20 + baseoff;
	const char* adddata = buff + baseoff + 0x20 + psize;
	int addsize = size - (adddata-buff);
	
	
	int restype = GetResourceType(buff);
	int hdrsize = (restype == LodResource_Bitmap )?0x28:0x8;
	
	Ogre::MemoryDataStreamPtr ptr(OGRE_NEW Ogre::MemoryDataStream(unpsize + addsize+hdrsize ));
	unsigned char*hdrdata = ptr->getPtr();
	memset(hdrdata,0,hdrsize);
	*(int*)(hdrdata)= GetResourceType(buff);
	*(int*)(hdrdata+4)= version;
	if(restype == LodResource_Bitmap )
		memcpy(hdrdata+0x8,buff+baseoff, 0x20);

	unsigned char* unpdata = hdrdata  + hdrsize;
	if(unpsize2)
	{
		int z_res = uncompress( unpdata, &unpsize , pdata, psize );
		if ( z_res != Z_OK )
		{
			Ogre::LogManager::getSingleton().logMessage(
                mName + " LODArchive::loadDataDefault: error unpaking, code: "+ Ogre::StringConverter::toString(z_res) );
			return Ogre::DataStreamPtr();
		}
	}else
	{
		memcpy(unpdata, pdata, psize);
	}
	if( addsize > 0  )
		memcpy(unpdata + unpsize2, adddata, addsize );

	return Ogre::DataStreamPtr(ptr);
}
//-----------------------------------------------------------------------
Ogre::DataStreamPtr LODArchive::loadDataSprite(const char*buff, int size) const
{
	int height = *(unsigned short*)( buff+0x12);
	int hdrsize=0x20+height*8;
	int psize = *(int*)(buff+0xc);
	unsigned long unpsize = *(int*)(buff+0x1c);
	if( psize+0x20+height*8 != size)
	{
		Ogre::LogManager::getSingleton().logMessage(
                mName + " LODArchive::loadDataSprite: incorrect sprite data size ");
		return Ogre::DataStreamPtr();
	}
	Ogre::MemoryDataStreamPtr ptr(OGRE_NEW Ogre::MemoryDataStream(unpsize + 8 +hdrsize));
	int* hdrdata = (int*)ptr->getPtr();
	hdrdata[0] = LodResource_Sprite;
	hdrdata[1] = version;
	memcpy(ptr->getPtr()+8,buff,hdrsize);
	unsigned char* unpdata = ptr->getPtr() + 8 + hdrsize;
	const unsigned char* pdata = (unsigned char*) buff+hdrsize;
	int z_res = uncompress( unpdata, &unpsize , pdata, psize );
	if ( z_res != Z_OK )
	{
		Ogre::LogManager::getSingleton().logMessage(
            mName + " LODArchive::loadDataMap: error unpaking, code: "+ Ogre::StringConverter::toString(z_res) );
		return Ogre::DataStreamPtr();
	}

	return Ogre::DataStreamPtr(ptr);
}

//-----------------------------------------------------------------------
Ogre::DataStreamPtr LODArchive::loadDataMap(const char*buff, int size) const
{
	unsigned int*hdr = (unsigned int*)(buff);
	const unsigned char* pdata = (unsigned char*) buff+8;
	if( size < 8 )
	{
		Ogre::LogManager::getSingleton().logMessage(
                mName + " LODArchive::loadDataMap: too small data ");
		return Ogre::DataStreamPtr();
	}
	int mapversion = version;
	if ( hdr[0] == 0x16741 && hdr[1] == 0x6969766d )
	{
		pdata+=8;
		hdr+=2;
		if( mapversion == LOD_MM6) mapversion = LOD_MM7;
	}
	int psize = hdr[0];
	unsigned long unpsize = hdr[1];
	/*if( psize != buff + size - pdata )
	{
			Ogre::LogManager::getSingleton().logMessage(
                mName + " LODArchive::loadDataMap: invalid mapsize : "+ Ogre::StringConverter::toString(psize) );
			return Ogre::DataStreamPtr();
	}*/
	Ogre::MemoryDataStreamPtr ptr(OGRE_NEW Ogre::MemoryDataStream(unpsize + 8 ));
	int* hdrdata = (int*)ptr->getPtr();
	hdrdata[0] = LodResource_Map;
	hdrdata[1] = mapversion;

	unsigned char* unpdata = ptr->getPtr() + 8;
	int z_res = uncompress( unpdata, &unpsize , pdata, psize );
	if ( z_res != Z_OK )
	{
		Ogre::LogManager::getSingleton().logMessage(
            mName + " LODArchive::loadDataMap: error unpaking, code: "+ Ogre::StringConverter::toString(z_res) );
		return Ogre::DataStreamPtr();
	}

	return Ogre::DataStreamPtr(ptr);
}
//-----------------------------------------------------------------------
Ogre::DataStreamPtr LODArchive::create(const Ogre::String& filename) const
{
		OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, 
			"Modification of LOD archives is not supported", 
			"LODArchive::create");

}



//-----------------------------------------------------------------------
void LODArchive::remove(const Ogre::String& filename) const
{
}



//-----------------------------------------------------------------------
// This is never called as far as I can see.
Ogre::StringVectorPtr LODArchive::list(bool recursive, bool dirs)
{
    std::vector<lod_item_s>::const_iterator it = files.begin();
	   
	Ogre::StringVectorPtr ptr = Ogre::StringVectorPtr(OGRE_NEW Ogre::StringVector());
	for( std::vector<lod_item_s>::const_iterator it = files.begin(); it < files.end(); it++)
	{
		ptr->push_back((*it).fname);
	}
    return ptr;
}



//-----------------------------------------------------------------------
// Also never called.
Ogre::FileInfoListPtr LODArchive::listFileInfo(bool recursive, bool dirs )
{
    //!!!not implemented
	Ogre::FileInfoListPtr ptr = Ogre::FileInfoListPtr(OGRE_NEW Ogre::FileInfoList());
    return ptr;
}

//-----------------------------------------------------------------------
// After load() is called, find("*") is called once. It doesn't seem
// to matter that we return an empty list, exists() gets called on
// the correct files anyway.
Ogre::StringVectorPtr LODArchive::find(const Ogre::String& pattern, bool recursive, bool dirs)
{
    //!!!not implemented
    Ogre::StringVectorPtr ptr = Ogre::StringVectorPtr(OGRE_NEW Ogre::StringVector());
    std::vector<lod_item_s>::iterator it = files.begin();
	for(;;)
	{
		it = std::find_if(it, files.end(), find_filename(pattern));
		if(it == files.end())
			break;
		ptr->push_back((*it).fname);
		it++;
	}
    return ptr;
}

//-----------------------------------------------------------------------
Ogre::FileInfoListPtr LODArchive::findFileInfo(const Ogre::String& pattern, bool recursive,  bool dirs)
{
	//need
	Ogre::FileInfoListPtr ptr = Ogre::FileInfoListPtr(OGRE_NEW Ogre::FileInfoList());
    std::vector<lod_item_s>::iterator it = files.begin();
	for(;;)
	{
		it = std::find_if(it,files.end(), find_filename(pattern));
		if(it == files.end())
			break;
        Ogre::FileInfo fi;
        fi.archive = this;
        fi.filename = (*it).fname;
        // It apparently doesn't matter that we return bogus
        // information
        fi.path = "";
		fi.compressedSize = (*it).size;
		fi.uncompressedSize = 0;
        ptr->push_back(fi);
		it++;
	}
    return ptr;
}

//-----------------------------------------------------------------------
bool LODArchive::exists(const Ogre::String& filename)
{
	return files.end() != std::find_if(files.begin(),files.end(), find_filename(filename)) ;
}
//-----------------------------------------------------------------------
time_t LODArchive::getModifiedTime(const Ogre::String& filename) { return 0; }
//-----------------------------------------------------------------------

}