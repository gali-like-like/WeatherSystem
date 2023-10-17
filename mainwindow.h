#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QByteArray>
#include <QJsonObject>
#include <QHash>
#include <QJsonDocument>
#include <QJsonArray>
#include <QChart>
#include <qlineseries.h>
#include <QValueAxis>
#include <QSplineSeries>
#include <QDateTimeAxis>
#include <QLegendMarker>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBox_editTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager* manger;
    QNetworkReply* reply;
    QString arg;
    QChart* chart;
    QLineSeries* seriesMin;
    QLineSeries* seriesMax;
    QDateTimeAxis* dateAxis;
    QValueAxis* axisy;
    void do_readyRead();
    void do_finished();
    qint8 indexFromPoint(const QPointF& point,const QLineSeries* series);
    void do_hovered(const QPointF& point,bool state);
    void do_clicked(const QPointF& point);
    void do_errorOccurred(QNetworkReply::NetworkError error);
};
#endif // MAINWINDOW_H
