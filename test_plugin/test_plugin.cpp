// test_plugin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace Ogre;
using namespace std;
String savePath("C:/angel/Progr/Ogre/_out/");
String MMpath("C:/angel/data/mm/MM_VI/Data/");
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

void SaveStreamToFile(const Ogre::String& fname )
{
	Ogre::DataStreamPtr data = ResourceGroupManager::getSingleton().openResource(fname);
	char*buff = new char[data->size()];

	std::ios::openmode mode = std::ios::binary | std::ios::out;
	std::ofstream rwStream;
	data->read(buff,data->size());
	rwStream.open((savePath+fname).c_str(), mode);
	rwStream.write(buff,data->size());
	rwStream.close();
	delete[] buff;
	std::cout << "Saved: " << fname << " size: "<<data->size() << std::endl;
}

void InitOgre()
{
	String pluginsPath;
#if _DEBUG
		pluginsPath = "plugins_d.cfg";
#else
		pluginsPath = "plugins.cfg";
#endif
    // Set up Root
    root = new Root(pluginsPath,"","");

}
void InitMM()
{
	
  ResourceGroupManager::getSingleton().addResourceLocation(MMpath + "Icons.lod", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation(MMpath + "BITMAPS.LOD", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation(MMpath + "games.lod", "Lod");
  ResourceGroupManager::getSingleton().addResourceLocation(MMpath + "SPRITES.LOD", "Lod");
}
int _tmain(int argc, _TCHAR* argv[])
{
	InitOgre();
	InitMM();
	//TestArchive();
	SaveStreamToFile("2DEvents.txt");
	SaveStreamToFile("2HAxe1");
	Ogre::Image img;
	img.load(Ogre::String("WtrdrXNW"),"General");
	img.save(savePath + "WtrdrXNW.png");

	return 0;
}

