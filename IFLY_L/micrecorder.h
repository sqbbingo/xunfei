#ifndef MICRECORDER_H
#define MICRECORDER_H

#include <QAudioRecorder>

class MicRecorder:public QObject
{
    Q_OBJECT
public:
    MicRecorder(QString audioFile);

// 公有函数，供外部调用
public:
    void startRecord();
    void stopRecord();

signals:
    void finishRecord();

private slots:
    void onStateChanged(QMediaRecorder::State state);

private:
    QAudioRecorder *audioRecorder;

};

#endif // MICRECORDER_H
