#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //读QSS
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("Open Success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else{
        qDebug("Open Failed");
    }

    //读配置
    QString fileName = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);    //拼接路径，转成本地路径
    QSettings settings(config_path,QSettings::IniFormat);   //读取ini文件
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host+ ":" + gate_port;


    MainWindow w;
    w.show();
    return a.exec();
}
