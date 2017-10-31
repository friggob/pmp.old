#include "playlist.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QDebug>

void playList::shuffleList(){
  QStringList t;
  QStringList t2;
  t = pList;

  qsrand(time(NULL));

  while(t.count() > 0){
	int i;
	i = qrand() % t.count();
	t2.append(t[i]);
	t.removeAt(i);
  }
  pList = t2;
}

playList::playList(){
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
	  qStdout() << "*";
	}else{
	  qStdout() << " ";
	}
	qStdout() << num.rightJustified(iLen,' ') <<
				 " : " <<
				 x.leftJustified(tw-10,' ',true) << endl;
  }
  qStdout().flush();
}

int playList::getIndex(){
  return pIndex;
}

int playList::getMaxIndex(){
  return pList.count()-1;
}

void playList::removeEntry(){
  pList.removeOne(playList::getEntry());
  pIndex--;
}

void playList::incrementIndex(){
  pIndex++;
}

QStringList playList::exportList(){
  return pList;
}

int playList::createPlayList(QStringList l){
	int listLen;
	QMimeType mt;
	QMimeDatabase md;

	foreach(QString p,l){
		QFileInfo fi(p);
		if(fi.exists()){
			mt = md.mimeTypeForFile(fi.filePath(), QMimeDatabase::MatchContent);

			if(!pList.contains(p) &&
				 ( mt.name().startsWith("video/") ||
					 mt.name() == "application/x-matroska" ||
					 mt.name() == "application/vnd.ms-asf" ||
					 mt.name() == "application/x-riff" ||
					 mt.name() == "application/octet-stream" ||
					 mt.name().startsWith("audio/") )){

				pList << p;

			}	else if(mt.inherits("text/plain") && !fi.fileName().endsWith(".sett")){
				//qStdout() << fi.fileName() << " is a text file, assuming playlist!" << endl;
				QFile fd;
				fd.setFileName(p);
				fd.open(QIODevice::ReadOnly);
				QTextStream ts(&fd);
				QStringList sl;

				while(!ts.atEnd()){
					QString ls = ts.readLine();
					QFileInfo fi2(ls);

					if(ls.startsWith("*")){
						ls.remove(0,1);
						settFile = ls;
					}
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
			}else if(mt.inherits("text/plain") && fi.fileName().endsWith(".sett")){
				settFile = fi.fileName();
				settFile.replace(QString(".sett"),"");

				qDebug() << "File with .sett file:" << settFile;
			}else{
				if(!mt.inherits("inode/directory"))
					qDebug() << p << ":" << mt.name();
			}
		}
	}

	if(randomize){
		shuffleList();
	}
	if(settFile.count() > 0){
		QRegExp r(settFile);
		r.setPatternSyntax(QRegExp::FixedString);
		if((pIndex = pList.indexOf(r)) == -1){
			pIndex = 0;
		}
		settFile = "";
	}

	listLen = pList.count();
	return listLen;
}
