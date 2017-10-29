#include "cli.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <QDebug>
#include <QRegExp>

cli::cli(mpv *player, playList *list){
  l = list;
  p = player;
}

void cli::printHelp(){
  qStdout() << "Command:" << endl;
  qStdout() << "  h|help|?:\tThis help" << endl;
  qStdout() << "  x:\t\tQuit and delete .sett file" << endl;
  qStdout() << "  q:\t\tQuit" << endl;
  qStdout() << "  a|y:\t\tDelete file" << endl;
  qStdout() << "  d:\t\tMove file to dir ./.delete" << endl;
  qStdout() << "  dq:\t\tMove file to dir ./.delete abd quit" << endl;
  qStdout() << "  dx:\t\tMove file to dir ./.delete, delete .sett file and quit" << endl;
  qStdout() << "  d:\t\tMove file to dir ./.delete" << endl;
  qStdout() << "  i:\t\tToggle reindex when playing file" << endl;
  qStdout() << "  r:\t\tReplay last file" << endl;
  qStdout() << "  l:\t\tPrint playlist" << endl;
  qStdout() << "  [n]:\t\tPlay file at index [n] in playlist" << endl;
  qStdout() << "  g|gg:\t\tMove file to dir ./_gg" << endl;
  qStdout() << "  gq:\t\tMove file to dir ./_gg and quit" << endl;
  qStdout() << "  gx:\t\tMove file to dir ./_gg, delete .sett file and quit" << endl;
  qStdout() << "  m:\t\tMove file to dir ./sett" << endl;
  qStdout() << "  mq:\t\tMove file to dir ./sett and quit" << endl;
  qStdout() << "  mx:\t\tMove file to dir ./sett, delete sett file and quit" << endl;
  qStdout() << "  s:\t\tSave playlist into file ./__savelist" << endl;
  qStdout() << "  sq:\t\tSave playlist into file ./__savelist and quit" << endl;
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
