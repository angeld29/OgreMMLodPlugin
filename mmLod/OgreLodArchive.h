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

#ifndef __OgreLodArchive_H__
#define __OgreLodArchive_H__
/// 
#include <OgreLogManager.h>
#include <OgreException.h>
#include <OgreStringVector.h>
#include <OgreArchive.h>
#include <OgreArchiveFactory.h>
#include <OgreSharedPtr.h>
#include "mm_defs.h"
namespace Ogre {
class LODArchive : public Ogre::Archive
{
	private:
		typedef struct
		{
			char dirname[0x10];
			int off,size,hz,num;
		}lod_head_typ;
		struct lod_item_s
		{
			Ogre::String fname;
			int off,size,hz,num;
		};
		struct find_filename
		{
			Ogre::String pattern;
			find_filename(Ogre::String _pattern): pattern(_pattern) {}
			bool operator() ( const lod_item_s& m ) const
			{
				return Ogre::StringUtil::match(m.fname,pattern,false);
			}
		};
		LodVersion version;
		enum { LOD_DEFAULT, LOD_SPRITE, LOD_MAP} lod_type;
		lod_head_typ header;
		Ogre::String   dirname;
		std::vector<lod_item_s> files;
	protected:
		//Ogre::SharedPtr<std::ifstream> stream; 
		std::ifstream* stream; 
	private:
		Ogre::DataStreamPtr loadDataDefault(const char*buff, int size) const;
		Ogre::DataStreamPtr loadDataSprite(const char*buff, int size) const;
		Ogre::DataStreamPtr loadDataMap(const char*buff, int size) const;
		LodResourceType GetResourceType(const char*buff) const;
	public:
       LODArchive (const Ogre::String& name, const String& archType );
        ~LODArchive ();

        /// @copydoc Archive::isCaseSensitive
        bool isCaseSensitive(void) const { return false; };

        /// @copydoc Archive::load
        void load();
        /// @copydoc Archive::unload
        void unload();

        /// @copydoc Archive::open
        Ogre::DataStreamPtr open(const Ogre::String& filename, bool readOnly = true) const;

		/// @copydoc Archive::create
		Ogre::DataStreamPtr create(const Ogre::String& filename) const;

		/// @copydoc Archive::delete
		void remove(const Ogre::String& filename) const;

		/// @copydoc Archive::list
        Ogre::StringVectorPtr list(bool recursive = true, bool dirs = false);

        /// @copydoc Archive::listFileInfo
        Ogre::FileInfoListPtr listFileInfo(bool recursive = true, bool dirs = false);

        /// @copydoc Archive::find
        Ogre::StringVectorPtr find(const Ogre::String& pattern, bool recursive = true,
            bool dirs = false);

        /// @copydoc Archive::findFileInfo
        Ogre::FileInfoListPtr findFileInfo(const Ogre::String& pattern, bool recursive = true,
            bool dirs = false);

        /// @copydoc Archive::exists
        bool exists(const Ogre::String& filename);
		time_t getModifiedTime(const Ogre::String& filename);
};
class LODArchiveFactory : public Ogre::ArchiveFactory
{
public:
    virtual ~LODArchiveFactory() {}
    /// @copydoc FactoryObj::getType
    const Ogre::String& getType(void) const
	{
		static Ogre::String name = "Lod";
        return name;
	};
    /// @copydoc FactoryObj::createInstance
    Ogre::Archive *createInstance( const Ogre::String& name ) 
    {
        return OGRE_NEW LODArchive(name, "Lod");
    }
    /// @copydoc FactoryObj::destroyInstance
    void destroyInstance( Ogre::Archive* arch) { OGRE_DELETE arch; }
};
}
#endif
