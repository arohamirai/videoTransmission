/*
*
*
* author:lile(wslf0115@gmail.com)
*
*
*/

#ifndef CNETTRAN_H
#define CNETTRAN_H

#include"opencv2/opencv.hpp"
#include<string>
#include<vector>
#include <netinet/in.h>

enum TRAN_ERROR_TYPE {
	NET_TRAN_ERROR = -1,
	NET_TRAN_SUCESS,
	NET_TRAN_CREATE_SOCKET_ERROR,
	NET_TRAN_BIND_ERROR,
	NET_TRAN_LISTEN_ERROR,
	NET_TRAN_ACCEPT_ERROR,
	NET_TRAN_CONNECT_ERROR,
	NET_TRAN_SEND_ERROR,
	NET_TRAN_RECV_ERROR,
	NET_TRAN_VEC2CHAR_ERROR
};

typedef struct _netError
{
	int type;
	char message[50];
} netError;

// send data type
enum STREAM_TYPE{	
	NET_TRAN_DATA = 1,
	NET_TRAN_MSG
};

// message id
enum MSG_ID{
	NET_TRAN_MSG_PRE_CLOSE = 1, 	// socket prepare to shutdown
	NET_TRAN_MSG_PRE_SEND_IMAGE, 	// prepare to send image
	NET_TRAN__MSG_PRE_SEND_FILE, 	// prepare to send file
	NET_TRAN_MSG_RECVED_MSG			//
};


typedef struct _lileMsg
{
	int msgId;
	union
	{
		char msg_c[8];
		int msg_i[8];
		float msg_f[8];
		double msg_d[8];
	};
	size_t msg_st;
	
} lileMsg;  // 80 char




class CNetBase
{
public:
	CNetBase();
	~CNetBase();
	
public:
	//int initNet(int nPort, const std::string& serverIp);
	int sendMsg(const lileMsg& msg);
	int recvMsg(lileMsg& msg);
	virtual int handleMsg(const lileMsg& msg);
	
	int sendFile(const char *buf, const size_t bufLen);
	int recvFile(char *buf, const size_t bufLen);
	
	int sendImage(const IplImage* _img, const int quanlity = 100, const std::string& ext = ".jpg", const int compressType = CV_IMWRITE_JPEG_QUALITY);
	int sendImage(cv::Mat& img, const int quanlity = 100, const std::string& ext = ".jpg", const int compressType = CV_IMWRITE_JPEG_QUALITY);
	IplImage* recvImage(const lileMsg& msg);
	int recvImage(const lileMsg& msg, cv::Mat& img);
	
	netError getLastError(void);
	
public:
	virtual int unInitNet();

protected:
	int sockfd;
	netError e;



private:
	char msgBuffer[100];  // for msg, is enough for header and struct _lileMsg 


};


class CClientTerminal:public CNetBase
{
public:
	CClientTerminal();
	~CClientTerminal();
	
public:	
	int initNet(const int nPort, const std::string& serverIp);
	virtual int unInitNet();
};




class CServerTerminal:public CNetBase
{
public:
	CServerTerminal();
	~CServerTerminal();
	
public:
	int initNet(const int nPort);
	virtual int unInitNet();
	
public:
	std::string getRemoteIp(void);
	
private:
	struct sockaddr_in remote_addr;
    int listenfd;	
};




#endif
