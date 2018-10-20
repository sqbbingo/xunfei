#ifndef IFLYVOICE_H
#define IFLYVOICE_H

/****************************************************************************
*
*						 科大讯飞语音识别类（QT版）
*
*@类名：iFlyVoice，继承自QThread，多线程类
*
*@成员函数（供外部调用）：
*   ★ bool login(const char* login_param)
*     - 用于登录讯飞平台，需在speech2Text函数前使用
*     - 返回布尔值，true即为登陆成功，false即为登录失败
*     - login_param: 开发者的appid
*
*   ★ bool logout()
*     - 用于退出讯飞平台，建议在speech2Text函数后使用
*     - 返回布尔值，类似login
*
*   ★ void speech2Text(const char* audioFileName, int iaccent, char *grammarId);
*     - 用于将录入音频转换为文字，无返回值，该函数会在新开辟的线程内执行，实际转换过程在run函数内部
*     - audioFileName：音频文件的文件名（含路径）
*     - iaccent: 音频内容口音。MANDARIN为普通话，CANTONESE为粤语
*     - grammarId：识别语义
*
* @成员函数（供内部调用）：
*   ☆ void run()
*     - 多线程里的一个虚函数，执行start函数会自动执行run内的函数
*
*   ☆ void finishTrans(QString text);
*     - 信号函数，用于通知翻译完成，并传递翻译内容text。
*     - text：语音听写后的翻译内容
*
* ****************************************************************************
*
* 作者：TShadow
*
* 修改时间：2017/03/11
*
* ***************************************************************************/

#include <QString>
#include <QThread>

class iFlyVoice:public QThread
{
    Q_OBJECT

public:
    iFlyVoice();

// 公有函数，可供外部调用
public:
    bool login(const char* login_param);                                               // 登录
    bool logout();                                                                     // 登出
    void speech2Text(const char* audioFileName, int iaccent, char *grammarId);         // 语音转文本

private:
     const char* params_normal;                       // 普通话
     const char* params_accent1;                      // 粤语
     const char* audioFileName;                       // 文件路径名
     char* grammar_id;                                // 目前未使用，预留
     int accent;                                      // 口音

protected:
     void run();                                      // 这是多线程里的一个虚函数，由start函数开启

signals:
     void finishTrans(QString text);
};

#endif // IFLYVOICE_H
