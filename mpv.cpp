#include "mpv.h"
#include <QDebug>
#include <QProcess>

mpv::mpv(){
  cacheSize = -1;
  fullScreen = true;
  cIndex = false;
  noSound = false;
  use43 = false;
  use169 = false;
  useStereo = false;
  player = "/usr/bin/mpv";
}

void mpv::setCacheSize(int c){
  mpv::cacheSize = c;
}

void mpv::setSullScreen(bool tf){
  mpv::fullScreen = tf;
}

void mpv::setcIndex(bool tf){
  mpv::cIndex = tf;
}

void mpv::setNoSound(bool tf){
  mpv::noSound = tf;
}

void mpv::setUse43(bool tf){
  mpv::use43 = tf;
}

void mpv::setUse169(bool tf){
  mpv::use169 = tf;
}

void mpv::setUseStereo(bool tf){
  mpv::useStereo = tf;
}

void mpv::createArgList(){
  argList.clear();
  if(fullScreen) argList += "--fs";
  if(cIndex) argList += "--index=recreate";
  if(noSound) argList += "--no-audio";
  if(use43) argList += "--video-aspect=4:3";
  if(use169) argList += "--video-aspect=16:9";
  if(useStereo) argList += "--audio-channels=stereo";
  if(cacheSize != -1){
	QString temp("--cache %1");
	argList += temp.arg(cacheSize);
  }
}

void mpv::mpvPlay(QString entry){
  QProcess *proc = new QProcess();
  QStringList a;
  a = argList;

  a += entry;
  qDebug() << a;
  qDebug() << argList;

  proc->start(player,a);

  if(!proc->waitForFinished(-1)){
	qDebug() << "Mpv had an error!";
  }

}

bool mpv::getcIndex(){
  return cIndex;
}
