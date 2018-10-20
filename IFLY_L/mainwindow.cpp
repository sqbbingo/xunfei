#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mic = new MicAudio;
    mVoice = new iFlyVoice;
    connect(mVoice,SIGNAL(finishTrans(QString)),this,SLOT(showRecText(QString)));
    recStatus = false;
    const char* login_param = "appid = 5bcad0b9";//5b84c58e 59910961 5bcad0b9
    if(!mVoice->login(login_param)){
        QMessageBox::warning(this,"警告","请检查appid是否正确！","确定");
    }
}

MainWindow::~MainWindow()
{
    mVoice->logout();
    delete ui;
}

void MainWindow::on_run_clicked()
{
    QString text = ui->run->text();
    if(text=="开始"){
        mic->startListen();
        connect(mic,SIGNAL(currVolume(int)),this,SLOT(showCurrVolume(int)));
        ui->run->setText("停止");
    }else if(text=="停止") {
        mic->stopListen();
        ui->run->setText("开始");
    }
}

void MainWindow::showCurrVolume(int volume)
{
    // 当音量大于1000时，开始录音
    if(volume>1000&&(!recStatus)){
        wRecorder.set_FileName("audio.wav");
        wRecorder.Start();
        qDebug()<<"开始录音"<<volume;
        recStatus = true;
    }

    // 当音量小于50时，停止录音
    if(volume<500&&recStatus){
        wRecorder.Stop();
        wRecorder.Reset();
        qDebug()<<"停止录音"<<volume;
        recStatus = false;
        mVoice->speech2Text("audio.wav",1,NULL);
    }
    ui->volume->setValue(volume);
}

void MainWindow::showRecText(QString text)
{
    ui->recText->setText(text);
}
