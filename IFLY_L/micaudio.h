#ifndef MICAUDIO_H
#define MICAUDIO_H

/****************************************************************************
*
*						 麦克风音量实时跟踪类
*
* @类名：MicAudio 继承自QObject
*
* @成员函数（供外部调用）：
*   ★  void startListen()
*     - 开始监听系统音量，无返回参数
*
*   ★  void stopListen()
*     - 停止监听系统音量，无返回参数
*
* @成员函数（供内部调用）：
*   ☆  void calcVolume()
*     - 用于计算当前音量，为了方便细分，量化至0~32767
*     - 需要注意，接受音量的progressBar应调整取值范围
*
*   ☆  void showVolume()
*     - 用于获取音量的槽函数，当前版本默认刷新时间为100ms，若需要调整，
*       可在cpp文件中自行更改
*     - 主要目的在于发送当前音量给目标函数体，发送信号为currVolume(int)
*
*   ☆  void currVolume(int)
*     - 将当前音量传递出去的信号函数（signal）
*
* ******************************************************************************
*
* 作者：TShadow
*
* 修改时间：2017/03/11
*
*****************************************************************************/

#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QByteArray>
#include <QIODevice>
#include <QObject>

class MicAudio: public QObject
{
    Q_OBJECT
public:
    explicit MicAudio();

// 公有函数，供外部调用
public:
    void startListen();
    void stopListen();

// 音频类型的私有成员
private:
    QAudioInput *mic;
    QIODevice *micSound;

// 一般类型私有成员
private:
    int micVolume;
    QByteArray mBuffer;

// 私有槽函数
private slots:
    void calcVolume();
    void showVolume();

// 信号函数
signals:
    void currVolume(int);

};

#endif // MICAUDIO_H
