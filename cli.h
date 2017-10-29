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

  public:
	cli(mpv* player,playList* list);
	void run();
};

#endif // CLI_H
