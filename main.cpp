#include <QCoreApplication>
#include "mpv.h"
#include "playlist.h"
#include "cli.h"
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);
	QCommandLineParser p;
	mpv player;
	playList list;
	cli* c = new cli(&player,&list);

	a.setApplicationName("pmp");
	a.setApplicationVersion(APP_VERSION);

	p.setApplicationDescription("Playlist player using mpv");
	p.addHelpOption();
	p.addVersionOption();

	p.addOptions({
		{"D","Do not delete, even if we say so"},
		{"n","No audio"},
		{"c","Cache size, in kilobytes","cache"},
		{"i","Create index before playing file"},
		{"s","Force 4:3 aspect"},
		{"w","Force 16:9 aspect"},
		{"z","Randomize playlist"},
		{"d","Automatically move file to .delete folder"},
		{"m","Automatically move file to sett folder"},
		{"q","Do not wait for commands between playing files"},
		{"r","Index in playlist to start from","entry"},
		{"t","Force stereo"},
		{"x","Don't create .sett file"}
	});

	p.process(a);
	const QStringList args = p.positionalArguments();

	list.setRandomize(p.isSet("z"));
	if(list.createPlayList(args) < 1) {
		qDebug() << "Error, playlist empty!";
		return 1;
	}
	if(p.isSet("r")) list.setIndex(p.value("r").toInt());

	player.setNoSound(p.isSet("n"));
	player.setcIndex(p.isSet("i"));
	player.setUse43(p.isSet("s"));
	player.setUse169(p.isSet("w"));
	player.setUseStereo(p.isSet("t"));
	if(p.isSet("c")) player.setCacheSize(p.value("c").toInt());
	player.createArgList();

	c->setNoSett(p.isSet("x"));
	c->setDeleteFile(p.isSet("d"));
	c->setMoveFile(p.isSet("m"));
	c->setQuiet(p.isSet("q"));
	c->setNoDelete(p.isSet("D"));
	c->run();

	//return a.exec();
}
