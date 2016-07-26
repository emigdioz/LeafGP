#include "mainwidget.h"
#include "ui_mainwidget.h"

void MainWidget::requestAlgorithmStart()
{
	workerAlgorithm->abort();
	threadGP->wait();
	workerAlgorithm->requestWork();
}

void MainWidget::on_startButton_clicked()
{
	// Copy input data to worker local matrix
	workerAlgorithm->data_matrix = input_data_matrix;
	requestAlgorithmStart();
	showStartedTime();
	startedDateTime = QDateTime::currentDateTime();
	timerGP->start(1000);
}

void MainWidget::receivedProgress1(int value)
{
	ui->circularProgress->setProgress1(value);
}

void MainWidget::receivedRunProgress(int value)
{
	ui->circularProgress->setProgress1(value);
	qDebug()<<value;
}

QString MainWidget::secondsToDHMS(qint64 duration)
{
    QString res;
    int seconds = (int)(duration % 60);
    duration /= 60;
    int minutes = (int)(duration % 60);
    duration /= 60;
    int hours = (int)(duration % 24);
    int days = (int)(duration / 24);
    if ((hours == 0) && (days == 0) && (minutes == 0))
        return res.sprintf("%02d secs", seconds);
    if ((hours == 0) && (days == 0))
        return res.sprintf("%02d mins %02d secs", minutes, seconds);
    if (days == 0)
        return res.sprintf("%02d hrs %02d mins %02d secs", hours, minutes, seconds);
    return res.sprintf("%dd days %02d hrs %02d mins %02d secs", days, hours, minutes, seconds);
}

void MainWidget::showElapsedTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    qint64 seconds = startedDateTime.secsTo(currentDateTime);

    ui->label_86->setText(secondsToDHMS(seconds));
    ui->label_86->update();
}

void MainWidget::showStartedTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    ui->label_84->setText(text);
}

void MainWidget::algorithmFinished()
{
  timerGP->stop();
}
