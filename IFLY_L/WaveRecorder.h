#pragma once
#include <QString>
/****************************************************************************
*
*						MMAPI API WaveRecorder录音类
*
*@类名：WaveRecorder
*
*@成员函数：
*
*--【在Start之前设置】
*	void set_FileName(QString des_path);
*	|_______设置需要保存的文件目标，不设置则不保存
*
*--【Stop之前任意设置】
*	void set_Callback(CNKDATAUpdateCallback fn);
*	|_______设置得到新的ChunkData时的处理函数，可以不设置
*
*--【Start会消耗3~4ms的时间】
*	void Start();
*	|_______初始化并开始录音
*
*--【Stop至少会消耗4ms的时间】
*	void Stop();
*	|_______结束录音并做收尾
*
*--【在Stop之后用】
*	void Reset();
*	|_______重置状态并清除set过的参数
*
* @备注：构造函数尝试打开音频设备，耗时约43ms，关闭设备于析构函数，耗时约900ms
*      需要配合winmm.lib使用。这里winmm.lib已被编译成libwinmm.a用于调用（低版本Qt仅能使用.a的lib文件）
*
* @示例：
*  1. 实时获取音频数据并保存于相对路径下test.wav
*     WaveRecorder sound;
*     sound.set_FileName("test.wav");
*     sound.Start();
*    - 这个阶段就在录音 -
*     sound.Stop();
*     sound.Reset();
*
*  2. 实时获取音频数据，并发送信息至回调函数
*    - 先声明回调函数 -
*     void CALLBACK ChunkArrived(array <char, CHUNCK_SIZE> ChunkData, UINT ChunksCount, bool bIsLast);
*    - 设置回调函数 -
*     sound.set_Callback(ChunkArrived);
*     sound.Start();
*    - 这个阶段就在录音，录音过程回调函数的输入参数ChunkData，ChunksCount，bIsLast会有相应变化 -
*     sound.Stop();
*     sound.Reset();
*
*  3. 直接处理RawData，简单直接粗暴
*     sound.Start();
*    - 这个阶段就在录音 -
*     sound.Stop();
*     sound.Reset(); // 这就OK了
*
****************************************************************************
*
*  作者：毕成·zzu
*  补充：TShadow 2017/03/07
*
****************************************************************************/
#ifndef WAVERECORDER_H
#define WAVERECORDER_H

#include <Windows.h>
#include <iostream>
#include <vector>
#include <array>
#include <stdio.h>
#include "mmsystem.h"
using namespace std;


// PCM录音参数
#define CHANNEL_MUM 1								// 声道数
#define SAMPLE_RATE 16000							// 每秒采样率
#define SAMPLE_BITS 16								// 采样位深
#define CHUNCK_SIZE (SAMPLE_RATE*SAMPLE_BITS/8*1)	// 缓存数据块大小 = 采样率*位深/2*秒（字节）
#define BUFFER_LAYER 5								// 缓冲区层数


/* 回调函数 */
typedef void(__stdcall *CNKDATAUpdateCallback)(array <char, CHUNCK_SIZE> ChunkData,UINT ChunksCount, bool bIsLast);
/*WaveRecorder类*/
class WaveRecorder 
{

public:

	WaveRecorder();									// 构造，打开设备
	~WaveRecorder();								// 析构，关闭设备
	void set_Callback(CNKDATAUpdateCallback fn);	// 设置实时处理数据块的回调函数，可以不设置
    void set_FileName(QString des_path);				// 设置需要保存的文件目标，不设置则不保存
	void Start();									// 初始化并开始录音
	void Stop();									// 结束录音并做收尾
	void Reset();									// 参数重设
	static array <char, CHUNCK_SIZE> ChunkData;		// 当前块数据
	static vector<array<char, CHUNCK_SIZE>> RawData;// 已录制的裸数据
	static UINT ChunksCount;						// 裸数据内块计数

private://

	void WaveInitFormat(							// 设置PCM格式
		LPWAVEFORMATEX WaveFormat,					//	.PCM
		WORD Ch,									//	.CHANNEL
		DWORD SampleRate,							//	.SAMPLE_RATE
		WORD BitsPerSample);						//	.SAMPLE_BITS
	void WaveFileWrite();
	static DWORD (CALLBACK WaveXAPI_Callback)(		// WaveXAPI回调函数
		HWAVEIN hwavein,							//	.输入设备
		UINT uMsg,									//	.消息
		DWORD dwInstance,							//	.保留
		DWORD dwParam1,								//	.刚填充好的缓冲区指针
		DWORD dwParam2);							//	.保留

	static CNKDATAUpdateCallback callback;			// 回调函数指针
	static BOOL bCallback;							// 是否有回调函数

	HWAVEIN hwi;									// 音频输入设备
	static WAVEHDR pwh[BUFFER_LAYER];				// 硬件缓冲区

	bool bSaveFile;									// 是否存储文件
	static bool stop;								// 是否触发停止
	static bool dat_ignore;							// 防止重复记录

	/* wav音频头部格式 */
	typedef struct WAVEPCMHDR
	{
		char            riff[4];					// = "RIFF"
		UINT32			size_8;						// = FileSize - 8
		char            wave[4];					// = "WAVE"
		char            fmt[4];						// = "fmt "
		UINT32			fmt_size;					// = PCMWAVEFORMAT的大小 : 
		//PCMWAVEFORMAT
		UINT16	        format_tag;					// = PCM : 1
		UINT16	        channels;					// = 通道数 : 1
		UINT32			samples_per_sec;			// = 采样率 : 8000 | 6000 | 11025 | 16000
		UINT32			avg_bytes_per_sec;			// = 每秒平均字节数 : samples_per_sec * bits_per_sample / 8
		UINT16		    block_align;				// = 每采样点字节数 : wBitsPerSample / 8
		UINT16			bits_per_sample;			// = 量化精度: 8 | 16
		char            data[4];					// = "data";
		//DATA
		UINT32			data_size;					// = 裸数据长度 
	} WAVEPCMHDR;
	/* 默认wav音频头部数据 */
	WAVEPCMHDR WavHeader =
	{
		{ 'R', 'I', 'F', 'F' },	
		0,						
		{ 'W', 'A', 'V', 'E' },
		{ 'f', 'm', 't', ' ' },
		sizeof(PCMWAVEFORMAT) ,
		WAVE_FORMAT_PCM,
		1,
		SAMPLE_RATE,
		SAMPLE_RATE*(SAMPLE_BITS / 8),
		SAMPLE_BITS / 8,
		SAMPLE_BITS,
		{ 'd', 'a', 't', 'a' },
		0
	};
	/* wav音频裸数据放在公有变量 */
    string dest_path;								// 存储路径
	FILE* fp = NULL;								// wave文件指针
	
};// WaveRecorder


#endif
