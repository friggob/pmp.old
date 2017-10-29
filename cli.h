#ifndef CLI_H
#define CLI_H
#include <playlist.h>
#include "mpv.h"

class cli
{
  private:
	mpv* p;
	playList* l;
	void printHelp();
	void createSett();
	void deleteSett();
	void createDir(QString d);
	void moveEntry(QString d = "sett");
	void savePlaylist();
	bool moveFile;
	bool deleteFile;
	bool noSett;
	bool quiet;
	bool noDelete;

  public:
	cli(mpv* player,playList* list);
	void run();
	void setDeleteFile(bool t);
	void setMoveFile(bool t);
	void setNoSett(bool t);
	void setQuiet(bool t);
	void setNoDelete(bool t);
};

#endif // CLI_H
