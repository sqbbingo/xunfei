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
class QAudioRecorder; //添加录音类

class MicAudio: public QObject
{
    Q_OBJECT
public:
    explicit MicAudio();

// 公有函数，供外部调用
public:
    void startListen();                     //开始监听
    void stopListen();                      //停止监听
    void set_FileName(QString des_path);    //设置需要保存的文件
    void start();                           //初始化并开始录音
    void stop();                            //结束录音并做结尾
    void reset();                           //参数重设

// 音频类型的私有成员
private:
    QAudioInput *mic;
    QIODevice *micSound;
    QAudioRecorder *audioRecorder;

// 一般类型私有成员
private:
    int micVolume;
    QByteArray mBuffer;

//录音使用到的参数
private:
    bool bSaveFile;                                 //是否存储文件

    /* wav音频裸数据放在公有变量 */
    QString dest_path;								// 存储路径
    FILE* fp = NULL;								// wave文件指针


// 私有槽函数
private slots:
    void calcVolume();
    void showVolume();
//    void updateProgress(qint64 duration);//用于显示录音时间

// 信号函数
signals:
    void currVolume(int);

};

#endif // MICAUDIO_H
