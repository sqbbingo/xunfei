#include "iflyvoice.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "msp_types.h"
#include "qisr.h"
#include <QDebug>
#include<iostream>
using namespace std;

#define MANDARIN 1
#define CANTONESE 2

iFlyVoice::iFlyVoice()
{
    params_normal = "sub=iat,auf=audio/L16;rate=16000,aue=speex-wb,ent=sms8k,rst=plain,rse=gb2312,accent=mandarin";//mandarin
    params_accent1 = "sub=iat,auf=audio/L16;rate=16000,aue=speex-wb,ent=sms8k,rst=plain,rse=gb2312,accent=cantonese";
}

// 登录函数
bool iFlyVoice::login(const char* login_param)
{
    int ret = MSPLogin(NULL,NULL,login_param);
    if(MSP_SUCCESS == ret)
    {
  //      qDebug()<<"登陆成功";
        return true;
    }else{
   //     qDebug()<<"登陆失败";
        return false;
    }
}

// 退出函数
bool iFlyVoice::logout()
{
    int ret = MSPLogout();
    if(MSP_SUCCESS == ret)
    {
        return true;
    }else{
        return false;
    }
}

// 语音听写转换函数：这里开了一个新的线程，避免翻译过程对录音过程的干扰
void iFlyVoice::speech2Text(const char *fileName, int iaccent, char* grammarId)
{
    audioFileName = fileName;
    accent = iaccent;
    grammar_id = grammarId;
    start();
}

// 启动新的线程执行run内部函数，以下内容才是翻译的过程，而不是speech2Text，speech2Text只是为了把参数传递进来
void iFlyVoice::run()
{
//    qDebug()<<"New Thread";
    QString textResult = "EMPTY";
    // 读取音频信息
    FILE *audioFile = fopen(audioFileName, "rb");
    if (audioFile == NULL){                                 // 打开文件失败
        qDebug()<<"OPEN_FILE_FAILED";
        return;
    }
    fseek(audioFile, 0, SEEK_END);                          // 指向音频文件末端
    unsigned long audioLen = ftell(audioFile);              // 读取音频长度
    fseek(audioFile, 0, SEEK_SET);                          // 指向音频文件始端
    char* audioData = (char *)malloc(audioLen);             // 分配内存控件
    fread((void *)audioData, audioLen, 1, audioFile);       // 写入信息
    fclose(audioFile);


    // 利用讯飞语音库识别
    const char* params = NULL;
    // - 选择口音
    if(accent==MANDARIN){
        params = params_normal;
    }else if(accent==CANTONESE){
        params = params_accent1;
    }

    // - 基本参数设置
    char recResult[2048] = { 0 };                                                // 返回结果
    const char *rslt = NULL;                                                     // 单次返回指针
    long audioCount = 0;                                                         // 音频计数器
    int audioStatus = MSP_AUDIO_SAMPLE_CONTINUE;                                 // 初始化音频状态,用来告知MSC音频发送是否完成
    int epStatus = MSP_EP_LOOKING_FOR_SPEECH;                                    // 初始化端点检测器状态，用来告知音频检测的位置
    int recStatus = MSP_REC_STATUS_SUCCESS;                                      // 初始化识别器返回的状态，提醒用户及时开始\停止获取识别结果
    int errCode = 0;                                                             // 初始化语音识别的错误代码

    // - 开始一次语音识别
    const char* sessionID = QISRSessionBegin(NULL, params, &errCode);
    if(errCode != MSP_SUCCESS){                                                  // 识别错误
        qDebug()<<"QISR_BEGIN_FAILED";
        return;
    }

    // - 开始写入音频信息
    /* ---------------------------------------------------------------------------------
       计算音频的长度frameLen: 1帧音频20ms, 若16位音频且采样率16k，则在1帧内：
       采样次数: N= 采样率×采样时间 = 16000×(1000ms/20ms) = 320次
       单次采样大小：M = (16bit/8)Byte = 2Byte; 因为 1Byte = 8 Bit
       因此，每帧的音频大小为：frameLen = N×M = 640Byte
       --------------------------------------------------------------------------------- */
    int frameLen = 640;
    while(1)
    {
        unsigned long len = 10*frameLen;                 // 每次写入200ms音频
        int ret = 0;
        if (audioLen < 2*len){                          // 当音频长度小于2倍的基本长度
            len = audioLen;
        }

        // 检查音频读取状态
        audioStatus = MSP_AUDIO_SAMPLE_CONTINUE;         // 有后继音频
        if (audioCount == 0){
            audioStatus = MSP_AUDIO_SAMPLE_FIRST;        // 第一次读取
        }

        // 读取audio_data数据,设置音频长度len,音频状态audio_status。
        ret = QISRAudioWrite(sessionID, (const void *)&audioData[audioCount], len, audioStatus, &epStatus, &recStatus );
        if(ret != MSP_SUCCESS){
            break;
        }

        // 识别成功，有识别结果返回
        if(recStatus == MSP_REC_STATUS_SUCCESS){
            rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);
            if (NULL != rslt){strcat(recResult, rslt);}
        }

        //检测到音频后端点,停止写入音频
        if(epStatus == MSP_EP_AFTER_SPEECH){
            break;
        }
        audioCount += (long)len;
        audioLen -= (long)len;
    }
    free(audioData);
    // 读取最终的文本信息
    QISRAudioWrite(sessionID, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &epStatus, &recStatus);
    while (recStatus != MSP_REC_STATUS_COMPLETE && 0 == errCode)
    {
        rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);//获取结果
        //qDebug()<<"rslt="<<rslt;
        if (NULL != rslt)
        {
            strcat(recResult, rslt);
            textResult = QString::fromLocal8Bit(recResult);
           // cout<<1<<endl;
        }
    }
    // 关闭语音识别
    QISRSessionEnd(sessionID, NULL);
    emit finishTrans(textResult);
}
