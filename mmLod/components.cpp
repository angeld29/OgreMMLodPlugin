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
// components.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

using namespace Ogre;
using namespace std;
Root *root=0;
bool TestArchive()
{
  //root->addResourceLocation("d:\\a.zip", "Zip");
  root->addResourceLocation("../../media/packs/dragon.zip", "Zip");
  // Pick a sample file
  String tex = "legs.jpg";
  //String tex = "a.h";
  cout << "Opening file: " << tex << endl;

  // Get it from the resource system
  DataStreamPtr data = ResourceGroupManager::getSingleton().openResource(tex, "General");
  if( !data.isNull() )
  {
	cout << "Size: " << data->size() << endl;
	return true;
  }else
  {
	return false;
  }	
}
//DataStreamPtr fstrm;
//FileStreamDataStream *fptr ;
/*void TestStream()
{
	//String fname( "../../media/packs/chiropteraDM.txt");
	std::ifstream* roStream = new std::ifstream;
	roStream ->open	("../../media/packs/chiropteraDM.txt", std::ios::in | std::ios::binary);
	//fptr =  new FileStreamDataStream("tt", roStream, true);
	//fstrm = DataStreamPtr(fptr);
	DataStreamPtr ss = DataStreamPtr(new LodDataStream( Ogre::SharedPtr<std::ifstream>(roStream),	0x203,500));
	std::cout <<  ss -> size() << endl;
	while(!ss->eof())
	{
		cout << ss->getLine(false) << endl;
	}
	cout << 
		ss->getAsString() 
		<< endl;
}*/
void SaveStreamToFile(const Ogre::String& fname, Ogre::DataStreamPtr data)
{
	char*buff = new char[data->size()];
	std::ios::openmode mode = std::ios::binary | std::ios::out;
	std::ofstream rwStream;
	data->read(buff,data->size());
	rwStream.open(fname, mode);
	rwStream.write(buff,data->size());
	rwStream.close();
	delete[] buff;
	std::cout << "Saved: " << fname << " size: "<<data->size() << std::endl;
}
int _tmain(int argc, _TCHAR* argv[])
{
  // Disable Ogre logging
  new LogManager;
  Log *log = LogManager::getSingleton().createLog("");
  //log->setDebugOutputEnabled(false);

  // Set up Root
  Root *root = new Root("","","");
  //MTGACodec::startup();
  //ArchiveManager::getSingleton().addArchiveFactory( new LODArchiveFactory );
  
  //addResourceLocation("D:/games/mm8bukarus/Data/bitmaps.lod", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation("D:/games/MMCollection/MM_VI/Data/Icons.lod", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation("D:/games/MMCollection/MM_VI/Data/BITMAPS.LOD", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation("D:/games/MMCollection/MM_VI/Data/games.lod", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation("D:/games/MMCollection/MM_VI/Data/SPRITES.LOD", "Lod");
  /*SaveStreamToFile("d:\\_\\2HAxe1", ResourceGroupManager::getSingleton().openResource("2HAxe1"));
  SaveStreamToFile("d:\\_\\2DEvents.txt", ResourceGroupManager::getSingleton().openResource("2DEvents.txt"));
  SaveStreamToFile("d:\\_\\ZNWC.STR", ResourceGroupManager::getSingleton().openResource("ZNWC.STR"));
  SaveStreamToFile("d:\\_\\armormid", ResourceGroupManager::getSingleton().openResource("armormid"));
  SaveStreamToFile("d:\\_\\WtrdrXNW", ResourceGroupManager::getSingleton().openResource("WtrdrXNW"));
  SaveStreamToFile("d:\\_\\d05.blv", ResourceGroupManager::getSingleton().openResource("d05.blv"));
  SaveStreamToFile("d:\\_\\oute3.ddm", ResourceGroupManager::getSingleton().openResource("oute3.ddm"));
  SaveStreamToFile("d:\\_\\3Gem07", ResourceGroupManager::getSingleton().openResource("3Gem07"));
  SaveStreamToFile("d:\\_\\wwalk1F2", ResourceGroupManager::getSingleton().openResource("wwalk1F2"));*/
  Ogre::Image img;
  img.load(Ogre::String("WtrdrXNW"),"General");
  img.save("d:\\_\\WtrdrXNW.png");
  ODMmap map;
  map.load("oute3.odm");
  /*Ogre::Image img2;
  img2.load(Ogre::String("armormid"),"General");
  img2.save("d:\\_\\armormid.png");*/
  //std::cout <<  ResourceGroupManager::getSingleton().resourceExists("General","alphanum1") << std::endl;
  
  //TestArchive();
  //TestStream();
  //cout << "End TestStream()" << endl;

  return 0;
}

