#include "micaudio.h"
#include <QDebug>
#include <QTimer>
#include <QAudioRecorder>
#include <QFileDialog>

#define bufferSize 4096

MicAudio::MicAudio():mBuffer(bufferSize,0)         // 初始化mBuffer的size为4096
{
    // 设置录制格式
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(16000);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::UnSignedInt);
    // 录音设备
    QAudioDeviceInfo micDevice(QAudioDeviceInfo::defaultInputDevice());  // 默认输入设备
    if(!micDevice.isFormatSupported(format)){
        format = micDevice.nearestFormat(format);                        // 如果设备不满足以上要求，更换至相邻参数
    }
    // 初始化mic
    mic = new QAudioInput(micDevice,format,this);

    //录音文件
    audioRecorder = new QAudioRecorder;
    //QStringList inputs = audioRecorder->audioInput();

    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/pcm");
    audioSettings.setQuality(QMultimedia::HighQuality);
    //audioSettings.setbitRate();
    audioSettings.setChannelCount(1);                   //声道数
    audioSettings.setSampleRate(1600);                  //每秒采样率
    audioRecorder->setAudioSettings(audioSettings);
}


// 开始监听并建立连接
void MicAudio::startListen()
{
    micSound = mic->start();
    connect(micSound,SIGNAL(readyRead()),this,SLOT(calcVolume()));
}

// 关闭监听并断开连接
void MicAudio::stopListen()
{
    mic->stop();
    disconnect(micSound,SIGNAL(readyRead()),this,SLOT(calcVolume()));
}

void MicAudio::set_FileName(QString Target)
{
    //若启用存档则保存一个wav文件
    bSaveFile = true;
    dest_path = Target;
//    errno_t err = fopen_s(&fp,dest_path.c_str(),'wb');
//    if(err > 0)
//    {
//        qDebug()<<"文件创建失败";
//        bSaveFile = false;
//    }

}

void MicAudio::start()
{
    audioRecorder->setAudioInput(audioRecorder->audioInput());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile("/home/bingo/下载/a"));
    audioRecorder->record();
}

void MicAudio::stop()
{
    audioRecorder->stop();
}


// 量化音量大小
void MicAudio::calcVolume()
{
    if(!mic){
        qDebug()<<"MIC设备错误！";
        return;
    }
    qint64 micLen = mic->bytesReady();                        // 读取录音单位长度
    if(micLen>bufferSize){
        micLen = bufferSize;                                  // 不能超过mBuffer的size
    }

    qint64 soundLen = micSound->read(mBuffer.data(),micLen);  // soundLen = 0则意味着没有数据被读入

    micVolume = 0;
    int value;
    if(soundLen>0){
        short * bufferData = (short*)mBuffer.data();          // 量化至short范围，当然int也是可以的
        short maxValue = 32767;
        short minValue = -32767;
        for(int i=0;i<micLen;i++){
            // 取最大值
            value = std::max(std::min(bufferData[i],maxValue), minValue);
            micVolume = std::max(micVolume,value);
        }

        // 以100ms的刷新频率监听音量
        QTimer::singleShot(100, this, SLOT(showVolume()));
    }
}

// 显示音量并发射信号，监听音量完成
void MicAudio::showVolume()
{
    emit currVolume(micVolume);
}
