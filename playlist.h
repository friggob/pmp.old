#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <QStringList>
#include <QString>
#include <QTextStream>

inline QTextStream& qStdout(){
  static QTextStream r{stdout};
  return r;
}

class playList
{
  private:
	bool randomize;
	int pIndex;
	QStringList pList;
	void shuffleList();
	QString settFile;

  public:
	playList();
	void setRandomize(bool tf);
	void setIndex(int idx);
	int createPlayList(QStringList l);
	QString getEntry();
	void printList(void);
	int getIndex();
	int getMaxIndex();
	void removeEntry();
	void incrementIndex();
	QStringList exportList();
};

#endif // PLAYLIST_H
