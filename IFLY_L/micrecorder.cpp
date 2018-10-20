#include "micrecorder.h"
#include <QUrl>

MicRecorder::MicRecorder(QString audioFile)
{
    audioRecorder = new QAudioRecorder;
    // 录音设置
    QAudioEncoderSettings settings;
    settings.setChannelCount(1);
    settings.setSampleRate(1600);
    settings.setBitRate(16);
    settings.setCodec("audio/pcm");
//    settings.setQuality(QMultimedia::NormalQuality);
    settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
    // 导入设置
    audioRecorder->setEncodingSettings(settings);
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(audioFile));
    connect(audioRecorder,SIGNAL(stateChanged(QMediaRecorder::State)),this,SLOT(onStateChanged(QMediaRecorder::State)));
}

void MicRecorder::startRecord()
{
    audioRecorder->record();
}

void MicRecorder::stopRecord()
{
    audioRecorder->stop();
}

void MicRecorder::onStateChanged(QMediaRecorder::State state)
{
    if(state==QMediaRecorder::StoppedState){
        emit finishRecord();
    }
}
