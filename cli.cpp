#include "cli.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <QDebug>
#include <QRegExp>
#include <QMap>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QTextStream>

cli::cli(mpv *player, playList *list) {
	l = list;
	p = player;
	deleteFile = false;
	moveFile = false;
	noSett = false;
	quiet = false;
	noDelete = false;
}

void cli::printHelp() {
	QMap<QString,QString> m;

	m.insert("?|h|help",
	         "This help");
	m.insert("x",
	         "Quit and delete .sett file");
	m.insert("q",
	         "Quit");
	m.insert("a|y",
	         "Delete file");
	m.insert("d",
	         "Move file to dir ./.delete");
	m.insert("dq",
	         "Move file to dir ./.delete and quit");
	m.insert("i",
	         "Toggle forced reindex when playing file");
	m.insert("r",
	         "Replay last file");
	m.insert("l",
	         "Print playlist");
	m.insert("#",
	         "Play file at index # in playlist");
	m.insert("g",
	         "Move file to dir ./_gg");
	m.insert("gq",
	         "Move file to dir ./_gg and quit");
	m.insert("m",
	         "Move file to dir ./sett");
	m.insert("m <dir>",
	         "Move file into dir named <dir>");
	m.insert("mq",
	         "Move file to dir ./sett and quit");
	m.insert("s",
	         "Save playlist into file ./__savelist");
	m.insert("sq",
	         "Save playlist into file ./__savelist and quit");

	qStdout() << "Command:" << endl;
	foreach(QString k, m.keys()) {
		qStdout() << k.rightJustified(8,' ') << ": " <<
		          m[k] << endl;
	}
}

void cli::createSett() {
	QFile sf;
	QDateTime dt(QDateTime::currentDateTime());

	sf.setFileName(l->getEntry()+".sett");
	if(!sf.exists()){
		sf.open(QIODevice::WriteOnly);
		qDebug() << "Creating file" << sf.fileName();

		sf.write(dt.toString("yyyy-MM-dd HH:mm:ss t").toLocal8Bit());

		sf.close();
	}
}

void cli::deleteSett() {
	QFile sf;
	sf.setFileName(l->getEntry()+".sett");
	if(sf.exists()) {
		sf.remove();
		qDebug() << "Deleting file" << sf.fileName();
	}
}

void cli::createDir(QString d) {
	QDir dir;

	dir.setPath(d);
	if(!dir.exists()) {
		QDir::current().mkdir(d);
		qDebug() << dir.path() << "created";
	} else {
		//qDebug() << dir.path() << "already exists";
	}
}

void cli::moveEntry(QString d) {
	createDir(d);
	QFile f(l->getEntry());
	QFileInfo fi(l->getEntry());
	if(f.rename(d+"/"+fi.fileName())) {
		qDebug() << "Moved file to" << d+"/";
		//deleteSett();
		l->removeEntry();
		savePlaylist();
	}
}

void cli::savePlaylist() {
	QStringList list;
	QFile fd("__savefile");

	list = l->exportList();
	if(fd.open(QIODevice::WriteOnly)) {
		foreach(QString ll, list) {
			if(ll == l->getEntry())
				fd.write("*");
			fd.write(ll.toLocal8Bit()+"\n");
		}
		fd.flush();
		fd.close();
	}
}

void cli::run() {
	char *buf;
	char ps1[] = "Command? (h for help) ";
	HIST_ENTRY *he;
	QRegExp re("\\d+");
	QRegExp r2("^(m|g|d).*");

	if(l->getIndex() == 0) {
		p->mpvPlay(l->getEntry());
	} else {
		l->printList();
	}
	if(!noSett) {
		//createSett();
		savePlaylist();
	}

	rl_bind_key('\t',rl_abort);

	while(quiet && (l->getIndex() < l->getMaxIndex())) {
		l->incrementIndex();
		p->mpvPlay(l->getEntry());
	}

	while(!quiet && (buf = readline(ps1)) != NULL) {
		QString b = QString::fromLocal8Bit(buf);
		QString dest;
		bool move;

		move = false;

		if(moveFile)
			dest = "sett";
		if(deleteFile)
			dest = ".delete";

		if(buf[0] != 0) {
			if((he = previous_history()) == NULL || strcmp(he->line,buf) != 0) {
			}
			add_history(buf);
		}

		if(b.startsWith('q')) {
			break;
		} else if(b.startsWith('h') || b.startsWith('?')) {
			printHelp();
			continue;
		} else if(b.startsWith('x')) {
			//deleteSett();
			break;

		} else if(b.startsWith('l')) {
			l->printList();
			continue;
		} else if(b.startsWith('r')) {
			p->mpvPlay(l->getEntry());
			continue;
		} else if(re.exactMatch(b)) {
			if(b.toInt() <= l->getMaxIndex()) {
				qStdout() << "Setting index to " << b.toInt() << endl;
				l->setIndex(b.toInt());
				savePlaylist();
				l->printList();
				continue;
			}
		} else if(b == "i") {
			p->setcIndex(!p->getcIndex());
			p->createArgList();
			continue;
		} else if(r2.exactMatch(b)) {

			switch(b[0].toLatin1()) {
			case 'g':
				dest = "_gg";
				break;
			case 'd':
				dest = ".delete";
				break;
			case 'm':
				dest = "sett";
				break;
			}

			if(b.left(2) == "m ") {
				dest = b.mid(2).trimmed();
			}

			move = true;

			if(b[1] == 'q') {
				moveEntry(dest);
				break;
			}
		} else if(b.startsWith('a') || b.startsWith('y')) {
			if(!noDelete) {
				QFile df(l->getEntry());
				if(df.exists()) {
					if(df.remove()) {
						qStdout() << "Deleted file: " << l->getEntry() << endl;
						l->removeEntry();
					}
				}
			}
		} else if(b.startsWith('s')) {
			savePlaylist();
			if(b[1] == 'q')
				break;
			continue;
		}
		if(move || moveFile || deleteFile) {
			moveEntry(dest);
		}

		if(l->getIndex() >= l->getMaxIndex()) {
			qDebug() << "No more files to play! Bye bye!";
			break;
		}

		if(!noSett) {
			//deleteSett();
		}

		l->incrementIndex();
		p->mpvPlay(l->getEntry());

		if(!noSett) {
			//createSett();
			savePlaylist();
		}
	}
	if(!quiet)
		free(buf);
}

void cli::setDeleteFile(bool t) {
	deleteFile = t;
}

void cli::setMoveFile(bool t) {
	moveFile = t;
}

void cli::setNoSett(bool t) {
	noSett = t;
}

void cli::setQuiet(bool t) {
	quiet = t;
}

void cli::setNoDelete(bool t) {
	noDelete = t;
}
