#include "cli.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <QDebug>
#include <QRegExp>
#include <QMap>

cli::cli(mpv *player, playList *list){
  l = list;
  p = player;
}

void cli::printHelp(){
  QMap<QString,QString> m;

  m.insert("?|h|help","This help");
  m.insert("x","Quit and delete .sett file");
  m.insert("q","Quit");
  m.insert("a|y","Delete file");
  m.insert("d","Move file to dir ./.delete");
  m.insert("dq","Move file to dir ./.delete and quit");
  m.insert("dx","Move file to dir ./.delete, delete .sett file and quit");
  m.insert("i","Toggle forced reindex when playing file");
  m.insert("r","Replay last file");
  m.insert("l","Print playlist");
  m.insert("#","Play file at index # in playlist");
  m.insert("g","Move file to dir ./_gg");
  m.insert("gq","Move file to dir ./_gg and quit");
  m.insert("gx","Move file to dir ./_gg, delete .sett file and quit");
  m.insert("m","Move file to dir ./sett");
  m.insert("mq","Move file to dir ./sett and quit");
  m.insert("mx","Move file to dir ./sett, delete .sett file and quit");
  m.insert("s","Save playlist into file ./__savelist");
  m.insert("sq","Save playlist into file ./__savelist and quit");

  qStdout() << "Command:" << endl;
  foreach(QString k, m.keys()){
	qStdout() << k.rightJustified(8,' ') << ": " <<
				 m[k] << endl;
  }
}

void cli::run(){
  char *buf;
  char ps1[] = "Command? (h for help) ";
  HIST_ENTRY *he;
  QRegExp re("\\d+");

  //l->printList();
  p->mpvPlay(l->getEntry());

  rl_bind_key('\t',rl_abort);
  while((buf = readline(ps1)) != NULL){
	QString b = QString::fromLocal8Bit(buf);

	if(buf[0] != 0){
	  if((he = previous_history()) == NULL || strcmp(he->line,buf) != 0){
		add_history(buf);
	  }
	}

	if(b.startsWith('q')){
	  break;
	}
	else if(b.startsWith('h') || b.startsWith('?')){
	  printHelp();
	  continue;
	}else if(b.startsWith('x')){
	  //deleteSett()
	  break;

	}else if(b.startsWith('l')){
	  l->printList();
	  continue;

	}else if(b.startsWith('r')){
	  l->setIndex(l->getIndex()-1);

	}else if(re.exactMatch(b)){
	  qDebug() << b << "is a number!";
	  qDebug() << l->getMaxIndex();
	  if(b.toInt() <= l->getMaxIndex()){
		qDebug() << "Setting index to" << b.toInt();
		l->setIndex(b.toInt()-1);
	  }
	}else if(b == "i"){
	  p->setcIndex(!p->getcIndex());
	  p->createArgList();
	  continue;
	}

	if(l->getIndex() >= l->getMaxIndex()){
	  qDebug() << "No more files to play! Bye bye!";
	  break;
	}
	l->setIndex(l->getIndex()+1);
	p->mpvPlay(l->getEntry());
  }
  free(buf);
}
