#include "playlist.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QDebug>

playList::playList()
{
  randomize = false;
  pIndex = 0;
}

void playList::setRandomize(bool tf){
  playList::randomize = tf;
}

void playList::setIndex(int idx){
  playList::pIndex = idx;
}

QString playList::getEntry(){

  if(pIndex >= getMaxIndex())
	pIndex = getMaxIndex();

  return playList::pList[pIndex];
}

void playList::printList(){
  int iLen;
  int tw;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  tw = w.ws_col;
  iLen = QString::number(pList.count()).length();

  foreach(QString x,pList){
	QString num;
	num = QString::number(pList.indexOf(x));
	if(pList.indexOf(x) == pIndex){
	  qStdout() << "*" <<
				   num.rightJustified(iLen,' ') <<
				   " : " <<
				   x.leftJustified(tw-10,' ',true) << endl;
	}else{
	  qStdout() << num.rightJustified(iLen+1,' ') <<
				   " : " <<
				   x.leftJustified(tw-10,' ',true) << endl;
	}
  }
  qStdout().flush();
}

int playList::getIndex(){
  return pIndex;
}

int playList::getMaxIndex(){
  return pList.count()-1;
}

int playList::createPlayList(QStringList l){
  int listLen;
  QMimeType mt;
  QMimeDatabase md;

  foreach(QString p,l){
	QFileInfo fi(p);
	if(fi.exists()){
	  mt = md.mimeTypeForFile(fi.filePath(), QMimeDatabase::MatchContent);

	  if(mt.name().startsWith("video/") && !pList.contains(p)){
		pList << p;
	  }	else if(mt.inherits("text/plain") && !fi.fileName().endsWith(".sett")){
		qStdout() << fi.fileName() << " is a text file, assuming playlist!" << endl;
		QFile fd;
		fd.setFileName(p);
		fd.open(QIODevice::ReadOnly);
		QTextStream ts(&fd);
		QStringList sl;

		while(!ts.atEnd()){
		  QString ls = ts.readLine();
		  QFileInfo fi2(ls);

		  if(!l.contains(ls) &&
			 !l.contains(fi2.absoluteFilePath()) &&
			 !l.contains(fi2.filePath())){
			sl.append(ls);
		  }else{
			qDebug() << "Found duplicate path!" << ls;
		  }
		}

		fd.close();
		createPlayList(sl);
	  }else{
		qDebug() << p << ":" << mt.name();
	  }
	}
  }

  listLen = pList.count();
  return listLen;
}
