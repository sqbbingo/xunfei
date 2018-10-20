#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "micaudio.h"
#include "iflyvoice.h"
#include "WaveRecorder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_run_clicked();
    void showCurrVolume(int volume);
    void showRecText(QString text);

    void on_pushButton_clicked();

private:
    bool recStatus;

private:
    Ui::MainWindow *ui;
    MicAudio *mic;
    iFlyVoice *mVoice;
//    WaveRecorder wRecorder;
};

#endif // MAINWINDOW_H
