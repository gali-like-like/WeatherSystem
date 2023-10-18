#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QNetworkRequest request(QUrl("https://query.asilu.com/weather/baidu"));
    manger = new QNetworkAccessManager(this);
    reply = manger->get(request);
    connect(reply,&QNetworkReply::readyRead,this,&MainWindow::do_readyRead,Qt::DirectConnection);
    connect(reply,&QNetworkReply::finished,this,&MainWindow::do_finished,Qt::DirectConnection);
    connect(reply,&QNetworkReply::errorOccurred,this,&MainWindow::do_errorOccurred,Qt::DirectConnection);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setSortingEnabled(true);
    seriesMin = new QLineSeries(this);
    seriesMax = new QLineSeries(this);
    axisy = new QValueAxis(this);
    dateAxis = new QDateTimeAxis(this);
    chart = new QChart();
    chart->setAnimationDuration(200);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->graphicsView->setChart(chart);
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->addAxis(axisy,Qt::AlignLeft);
    chart->addAxis(dateAxis,Qt::AlignBottom);

    seriesMin->setName(tr("min曲线"));
    seriesMin->setPointLabelsVisible(true);
    seriesMin->setPointLabelsFormat("@yPoint");
    seriesMin->setPointsVisible(true);
    seriesMin->setUseOpenGL(false);
    seriesMin->setBrush(QBrush(QColor("green")));
    seriesMax->setPointLabelsVisible(true);
    seriesMax->setPointsVisible(true);
    seriesMax->setPointLabelsFormat("@yPoint");
    seriesMax->setName(tr("max曲线"));
    seriesMax->setUseOpenGL(false);
    seriesMax->setBrush(QBrush(QColor("red")));
    connect(seriesMax,&QLineSeries::hovered,this,&MainWindow::do_hovered);
    connect(seriesMin,&QLineSeries::hovered,this,&MainWindow::do_hovered);
    axisy->setRange(0,40);
    axisy->setTickCount(5);
    axisy->setLabelsFont(QFont("微软雅黑",15));
    axisy->setTitleText(tr("温度/℃"));
    axisy->setTitleFont(QFont("微软雅黑",18));

    dateAxis->setTitleText(tr("日期"));
    dateAxis->setLabelsFont(QFont("微软雅黑",15));
    dateAxis->setFormat("yyyy-MM-dd");
    dateAxis->setMin(QDateTime::currentDateTime().addDays(-1));
    dateAxis->setMax(QDateTime::currentDateTime().addDays(7));
    dateAxis->setTickCount(8);
    dateAxis->setTitleFont(QFont("微软雅黑",18));
    ui->graphicsView->setRenderHints(QPainter::TextAntialiasing| QPainter::Antialiasing);

    qDebug()<<"添加玩序列";
}

qint8 MainWindow::indexFromPoint(const QPointF& point,const QLineSeries* series)
{
    QList<QPointF> points = series->points();
    qint8 pointCount = points.count();
    for(qint8 i = 0;i<pointCount;i++)
    {
        QPointF nowPoint = points.at(i);
        int dx = qAbs(nowPoint.x()-point.x());
        qDebug()<<QTime::fromMSecsSinceStartOfDay(dx);
        bool ok =false;
        long long num = QString("2e+08").toLongLong(&ok);
        qDebug().noquote()<<num;
        if(dx <= num)
        {
            return i;
        }
    }
    return -1;
}

void MainWindow::do_clicked(const QPointF& point)
{
    qDebug()<<"被点击了";
    QLineSeries* series = qobject_cast<QLineSeries*>(sender());
    series->setSelectedColor(Qt::red);
    int index = this->indexFromPoint(point,series);
    qDebug()<<"index:"<<index;
    series->setPointSelected(index,index>=0?true:false);
}

void MainWindow::do_hovered(const QPointF& point,bool state)
{
    QLineSeries* series = qobject_cast<QLineSeries*>(sender());
    QList<QLegendMarker*> markers = chart->legend()->markers();
    int indexSeries = chart->series().indexOf(series);
    QLegendMarker* marker = markers.at(indexSeries);
    qDebug()<<marker->label();
    QBrush initBrush = marker->brush();
    QColor initColor = initBrush.color();
    int alpha = initColor.alpha();
    QColor ininSeries = series->color();
    int initRed = ininSeries.red();
    if(state)
    {
        initColor.setAlpha(alpha+40>=255?alpha-40:alpha+40);
        initBrush.setColor(initColor);
        marker->setBrush(initBrush);
        ininSeries.setRed(initRed+50>=255?initRed-50:initRed+50);
        series->setColor(ininSeries);
    }
    else
    {
        initColor.setAlpha(alpha-40>=0?alpha-40:alpha+40);
        initBrush.setColor(initColor);
        marker->setBrush(initBrush);
        ininSeries.setRed(initRed-50>=0?initRed-50:initRed+50);
        series->setColor(ininSeries);
    }
}

void MainWindow::do_errorOccurred(QNetworkReply::NetworkError error)
{
    qDebug()<<"发生错误："<<error;
}

void MainWindow::do_readyRead()
{
    seriesMax->clear();
    seriesMin->clear();
    QStringList colNames;
    colNames<<"日期"<<"温度"<<"天气"<<"风况";
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount(7);
    for(int i = 0;i<4;i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(colNames[i]);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item->setFont(QFont("微软雅黑",18));
        ui->tableWidget->setHorizontalHeaderItem(i,item);
    }
    ui->tableWidget->verticalHeader()->setVisible(false);
    QByteArray bytes = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject obj = doc.object();
    QJsonArray array = obj.value("weather").toArray();
    int valueCount = array.count();
    qDebug()<<"行："<<valueCount;

    for (int i = 0; i < valueCount;i++)
    {
        QJsonValue value = array.at(i);
        QJsonObject subObj = value.toObject();
        QString date = subObj.value("date").toString();
        QString temp = subObj.value("temp").toString();
        temp.chop(1);
        QStringList maxMin;
        if(temp.contains("/"))
            maxMin = temp.split("/");
        else if(temp.contains("~"))
            maxMin = temp.split("~");
        else
            maxMin<<temp<<temp;
        int max = maxMin[0].toInt();
        int min = maxMin[1].toInt();
        qDebug()<<QString("%1：[%2,%3]").arg(i).arg(max).arg(min);
        QDateTime now = QDateTime::currentDateTime().addDays(i);
        //只能写成QDateTime.toMSecsSinceEpoch();
        seriesMin->append(now.toMSecsSinceEpoch(),min);
        seriesMax->append(now.toMSecsSinceEpoch(),max);
        QString weather = subObj.value("weather").toString();
        QString wind = subObj.value("wind").toString();
        QTableWidgetItem* item1 = new QTableWidgetItem(date);
        QTableWidgetItem* item2 = new QTableWidgetItem(temp);
        QTableWidgetItem* item3 = new QTableWidgetItem(weather);
        QTableWidgetItem* item4 = new QTableWidgetItem(wind);
        item1->setFont(QFont("微软雅黑",16));
        item2->setFont(QFont("微软雅黑",16));
        item3->setFont(QFont("微软雅黑",16));
        item4->setFont(QFont("微软雅黑",16));
        ui->tableWidget->setItem(i,0,item1);
        ui->tableWidget->setItem(i,1,item2);
        ui->tableWidget->setItem(i,2,item3);
        ui->tableWidget->setItem(i,3,item4);
    }
}

void MainWindow::do_finished()
{
    qDebug()<<"完成";
    qDebug()<<"max数据："<<seriesMax->points();
    chart->setTitle(tr("%1天气预告").arg(arg));
    chart->setTitleFont(QFont("微软雅黑",19));
    if(chart->series().isEmpty())
    {
        chart->addSeries(seriesMax);
        chart->addSeries(seriesMin);
        seriesMax->attachAxis(axisy);
        seriesMax->attachAxis(dateAxis);
        seriesMin->attachAxis(axisy);
        seriesMin->attachAxis(dateAxis);
    }
    ui->comboBox->addItem(ui->comboBox->currentText());
}

MainWindow::~MainWindow()
{
    delete reply;
    delete chart;
    delete ui;
}

void MainWindow::on_comboBox_editTextChanged(const QString &arg1)
{
    arg = arg1;
    QUrl url = QUrl::fromUserInput(QString("https://query.asilu.com/weather/baidu?city=%1").arg(arg));
    QNetworkRequest quest(url);
    reply = manger->get(quest);
    connect(reply,&QNetworkReply::readyRead,this,&MainWindow::do_readyRead);
    connect(reply,&QNetworkReply::finished,this,&MainWindow::do_finished);
    connect(reply,&QNetworkReply::errorOccurred,this,&MainWindow::do_errorOccurred);
}

