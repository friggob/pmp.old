#ifndef MPV_H
#define MPV_H
#include <QString>
#include <QStringList>

class mpv{
  private:
	int cacheSize;
	bool fullScreen;
	bool cIndex;
	bool noSound;
	bool use43;
	bool useStereo;
	bool use169;
	QString player;
	QStringList argList;

  public:
	mpv();
	void setCacheSize(int c);
	void setSullScreen(bool tf);
	void setcIndex(bool tf);
	void setNoSound(bool tf);
	void setUse43(bool tf);
	void setUse169(bool tf);
	void setUseStereo(bool tf);
	void createArgList(void);
	void mpvPlay(QString entry);
	bool getcIndex();
};

#endif // MPV_H
