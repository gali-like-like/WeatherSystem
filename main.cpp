#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug()<<"123";
    qDebug()<<"456";
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "untitled6_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    QFile file(":/new/prefix1/mainwindow.qss");
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QString qss = file.readAll();
        qDebug()<<qss;
        file.close();
        w.setStyleSheet(qss);
    }
    w.show();
    return a.exec();
}
