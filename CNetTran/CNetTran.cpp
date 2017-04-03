#include"CNetTran.h"
#include <errno.h>
#include <sys/types.h>


#include <sys/wait.h>
#include <arpa/inet.h>
#include"unistd.h"
#include"math.h"
#include "stdio.h"
#include<assert.h>

using namespace cv;

CNetBase::CNetBase()
{
}

CNetBase::~CNetBase()
{
}


int CNetBase::sendMsg(const lileMsg& msg)
{
	memset(msgBuffer,0,sizeof(msgBuffer));
	
	char *ptr = msgBuffer;
	
	int value;
	value = NET_TRAN_MSG;
	memcpy(ptr,&value,sizeof(int));
	ptr += sizeof(int);
	
	memcpy(ptr,&msg,sizeof(msg));
	ptr = NULL;
	
	int ret = send(sockfd,msgBuffer,sizeof(msgBuffer),0);
	if(ret == -1)
	{
		close(sockfd);
		e = {0};
		e.type = NET_TRAN_SEND_ERROR;
		sprintf(e.message,"NET_TRAN_SEND_ERROR\n");
		return NET_TRAN_ERROR;
	}
	
	return 0;
}

int CNetBase::recvMsg(lileMsg& msg)
{
	while(true)
	{
		memset(msgBuffer,0, sizeof(msgBuffer));
	
		int ret = recv(sockfd,msgBuffer,sizeof(msgBuffer),0);
		if(ret == -1)
		{
			close(sockfd);
		
 			e = {0};
			e.type = NET_TRAN_RECV_ERROR;
			sprintf(e.message,"NET_TRAN_RECV_ERROR\n");
		
			return NET_TRAN_ERROR;
		}
	
		char *ptr = msgBuffer;
		int value = -1;
	
		memcpy(&value,ptr,sizeof(int));
		ptr += sizeof(int);
	
		if(value != NET_TRAN_MSG)
			continue;
		
		msg = *((lileMsg*)ptr);
		break;
	}
	return 0;
}



int CNetBase::sendFile(const char *buf, const int bufLen)
{
    
	int ret = send(sockfd,pBuffer,bufLen,0);
		recv(sockfd,msgBuffer,sizeof(msgBuffer),0);
		if (ret == -1)
		{
			close(sockfd);
		 	e = {0};
			e.type = NET_TRAN_SEND_ERROR;
			sprintf(e.message,"NET_TRAN_SEND_ERROR\n");
			return NET_TRAN_ERROR; 
		}	
	return 0;
		
}

int CNetBase::recvFile(char *buf, const size_t bufLen)
{
	int curRound = 0;
    int totalRound = ceil(bufLen/bufferLen);
    int curLen = 0;
    size_t totalLen = bufLen;
	
	int dataType = -1;
	char *pBuf = buf;
	
	int preRecvLen = 0;
	char *pNextStartPos = pBuffer;
	int ret = 0;
	while(curRound < totalRound)
	{
		//curRound++;
		memset(pBuffer,0,bufferLen);
	
		ret = recv(sockfd,pBuffer,bufferLen,0);
		if (ret == -1)
		{
		 	e = {0};
			e.type = NET_TRAN_RECV_ERROR;
			sprintf(e.message,"NET_TRAN_RECV_ERROR\n");
			return NET_TRAN_ERROR; 
		}
		
		preRecvLen = ret;
		pNextStartPos += ret;
		while(preRecvLen < bufferLen){
			// start a new recv() to recv the remind data
			ret = recv(sockfd,pNextStartPos,bufferLen-preRecvLen,0);
			if (ret == -1)
			{
				e = {0};
				e.type = NET_TRAN_RECV_ERROR;
				sprintf(e.message,"NET_TRAN_RECV_ERROR\n");
				return NET_TRAN_ERROR; 
			}
			preRecvLen += ret;
			pNextStartPos += ret;			
		}	
		
		
		char *ptr = pBuffer;
        // read stream header
        // data type, current round, total round, current lenth, total lenth

        int value;
		memcpy(&value,ptr,sizeof(int));
		dataType = value;
        ptr += sizeof(int);
		
		if(dataType != NET_TRAN_DATA)
		{
			continue;
		}
		
        memcpy(&value,ptr,sizeof(int));
		curRound = value;
		ptr += sizeof(int);

        memcpy(&value,ptr,sizeof(int));
		totalRound = value;
        ptr += sizeof(int);

       	memcpy(&value,ptr,sizeof(int));
		curLen = value;
        ptr += sizeof(int);
		
		size_t value_t;
        memcpy(&value_t,ptr,sizeof(size_t));
		totalLen = value_t;
        ptr += sizeof(size_t);
		
		// copy data
		memcpy(pBuf,ptr,curLen);
		pBuf += curLen;
		
		send(sockfd,msgBuffer,sizeof(msgBuffer),0);
	}
	return 0;
}


int CNetBase::handleMsg(const lileMsg& msg)
{
	
}

int CNetBase::sendImage(const IplImage* _img, const int quanlity, const std::string& ext, int compressType)
{
	
	Mat img(_img,false);
	int ret = sendImage(img, quanlity, ext, compressType);
	
	return ret;
}
int CNetBase::sendImage(cv::Mat& img, const int quanlity, const std::string& ext, const int compressType)
{
	vector<int> param(2);
	param[0] = compressType;
	param[1] = quanlity;
	
	int ret = 0;
		
	if(quanlity < 100)  // compressed
	{
		vector<unsigned char> vecBuf;
		imencode(ext,img,vecBuf,param);
		
		size_t sendLen = vecBuf.size();
		unsigned char *tempBuf = NULL;
		do
		{
			tempBuf = (unsigned char *)malloc(sendLen*(sizeof(unsigned char)));
		}
		while(tempBuf == NULL);
		
		for(size_t i = 0; i < sendLen; ++i)
		{
			tempBuf[i] = vecBuf[i];	
		}
		
		
		//size_t ret1 = vector2uchar(vecBuf,tempBuf);
		//////////////////
		
		static int iii= 0;
		if(iii == 0)
		{
			FILE *sendf = fopen("sendf.txt","w");
			for(size_t i = 0; i < sendLen; ++i)
			{
				fprintf(sendf,"%u\n", tempBuf[i]);
			}
			iii = 1;
		}
		/*
		if(ret1 != sendLen)
		{
			free(tempBuf);
			close(sockfd);
		 	e = {0};
			e.type = NET_TRAN_VEC2CHAR_ERROR;
			sprintf(e.message,"NET_TRAN_VEC2CHAR_ERROR\n");
			return NET_TRAN_ERROR;
		}
		*/
		lileMsg msg = {0};
		msg.msgId = NET_TRAN_MSG_PRE_SEND_IMAGE;
		msg.msg_st = sendLen;
		
		ret = sendMsg(msg);    //  the pic info
		
		if(ret == -1)
		{
			free(tempBuf);
			return NET_TRAN_ERROR;	
		}
		
		ret = sendFile((char*)tempBuf,sendLen);
		free(tempBuf);
		tempBuf = NULL;
		return ret;	
	}
	else // uncompressed
	{
		int nChannels = img.channels();
		int elemLen = img.elemSize1();
		size_t sendLen = img.cols*img.rows*nChannels*elemLen;
		
		lileMsg msg = {0};
		msg.msgId = NET_TRAN_MSG_PRE_SEND_IMAGE;
		msg.msg_i[0] = img.cols;
		msg.msg_i[1] = img.rows;
		msg.msg_i[2] = img.depth();
		msg.msg_i[3] = img.channels();
		msg.msg_i[4] = img.type();
		msg.msg_st = sendLen;
		
		
		ret = sendMsg(msg);
		if(ret == -1)
		{
			return NET_TRAN_ERROR;	
		}	
		
		//*
		if(img.isContinuous())
		{
			ret = sendFile((char *)img.data,sendLen);
			return ret;
		}
		/**/
		else
		{
			unsigned char *tempBuf = NULL;
			do
			{
				tempBuf = (unsigned char *)malloc(sendLen*(sizeof(unsigned char)));
			}
			while(tempBuf == NULL);
			
			int r = img.rows;
			int c = img.cols;
			
			unsigned char *ptempBuf = tempBuf;
			unsigned char *pr, *ptr;
			for(int i = 0; i < r; ++i)
			{
				pr = img.data + i * img.step1();
				for(int j = 0; j < c; ++j)
				{
					ptr = pr + j*elemLen*nChannels;
					
					memcpy(ptempBuf,ptr,elemLen*nChannels);
					ptempBuf += elemLen*nChannels;
				}	
			}
			
			ret = sendFile((char*)tempBuf,sendLen);
			free(tempBuf);
			tempBuf = NULL;
			ptempBuf = NULL;
			return ret;
		}
	}
	
}

IplImage* CNetBase::recvImage(const lileMsg& msg)
{
	Mat mat;
	int ret = recvImage(msg,mat);
	if(ret == 0)
	{
		IplImage ipl = IplImage(mat);
		IplImage *cpy = cvCloneImage(&ipl);
		return cpy;		
	}
	else
		return NULL;	
}
int CNetBase::recvImage(const lileMsg& msg, cv::Mat& img)
{
	int width = msg.msg_i[0];
	int height = msg.msg_i[1];
	int depth = msg.msg_i[2];
	int nChannels = msg.msg_i[3];
	int dataType = msg.msg_i[4];
	size_t bufLen = msg.msg_st;
	
	
	unsigned char *pBuf = NULL;
	do
	{
		pBuf = (unsigned char *)malloc(bufLen*(sizeof(unsigned char)));
	}
	while(pBuf == NULL);
	
	int ret = recvFile((char*)pBuf,bufLen);
	if(ret == NET_TRAN_ERROR)
		return ret;
	
	if(width == 0|| height == 0) //compressed
	{	
		vector<unsigned char> vecBuf;
		//uchar2vector(pBuf,vecBuf,bufLen);
		
		for(size_t i = 0; i < bufLen;++i)
		{
			vecBuf.push_back(pBuf[i]);	
		}
		
		static int j = 0;
		if(j == 0)
		{
			FILE *recv = fopen("recv.txt","w");
			for(size_t i = 0; i < bufLen;++i)
				fprintf(recv,"%u\n",pBuf[i]);
			j = 1;
		}
		

		img = imdecode(vecBuf,IMREAD_UNCHANGED);  // do not use CV_LOAD_IMAGE_COLOR though they are both equal 1
	
	}
	else	//uncompressed
	{
		Mat mat(height,width,dataType,(void*)pBuf);
		img = mat.clone();
	}
	
	free(pBuf);
	pBuf = NULL;
	return 0;
}
	
template <typename T>
size_t CNetBase::CNetBase::vector2uchar(const std::vector<T> &vec,unsigned char *pBuf)
{
	size_t n = 0;
	typename vector<T>::const_iterator it_begin = vec.begin();
	typename vector<T>::const_iterator it_end = vec.end();
	
	unsigned char *ptr = pBuf;
	
	for(typename vector<T>::const_iterator it = it_begin ; it != it_end; ++it)
	{
		T value = *it;
		memcpy(ptr,&value,sizeof(T));
		ptr += sizeof(T);
		++n;
	}
	return n;
}

template <typename T>
int CNetBase::uchar2vector(const unsigned char *buf,std::vector<T> &vec, size_t bufLen)
{
	unsigned char *ptr = (unsigned char* )buf;
	T value;
	
	for(size_t i = 0; i < bufLen; ++i)
	{
		memcpy(&value,ptr,sizeof(T));
		vec.push_back(value);
		ptr += sizeof(T);
	}
	return 0; 
}

netError CNetBase::getLastError(void)
{
	return e;
}


int CNetBase::unInitNet()
{
	int value = -1;
	const char *ptr = msgBuffer;
	memcpy(&value,ptr,sizeof(int));
	ptr += sizeof(int);
	
	
	memset(msgBuffer,0,sizeof(msgBuffer));
	lileMsg msg = {0};
	msg.msgId = NET_TRAN_MSG_PRE_CLOSE;
	
	int ret = sendMsg(msg);
	if(ret != 0)
	{
		close(sockfd);
		return 0;
	}

	/////////////////////////// tbd
	
	int n = 10;
	while(n-- > 0)
	{
		ret = recvMsg(msg);
		if(value == NET_TRAN_MSG && msg.msg_i[0] == NET_TRAN_MSG_RECVED_MSG)
		{
			close(sockfd);
		}			
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// CClientTerminal ///////////////////////////////////////////////////
//*

CClientTerminal::CClientTerminal()
{	
}

CClientTerminal::~CClientTerminal()
{
}
/**/


int CClientTerminal::initNet(int nPort, const string& serverIp)
{
	 //创建套接字
    if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1) {
		e = {0};
		e.type = NET_TRAN_CREATE_SOCKET_ERROR;
		sprintf(e.message,"NET_TRAN_CREATE_SOCKET_ERROR\n");
		
		return NET_TRAN_ERROR;
    }
	
	//绑定端口地址
	struct sockaddr_in remote_addr;
    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(nPort);
    remote_addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
	
	// 链接远程端口
	 if (connect(sockfd ,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr)) == -1 ) {
		 close(sockfd);
		 e = {0};
		 e.type = NET_TRAN_CONNECT_ERROR;
		 sprintf(e.message,"NET_TRAN_CONNECT_ERROR\n");
		
		 return NET_TRAN_ERROR; 
    }
	
	return 0;
}

int CClientTerminal::unInitNet()
{
	//memset(msgBuffer,0,sizeof(msgBuffer));
	lileMsg msg = {0};
	msg.msgId = NET_TRAN_MSG_PRE_CLOSE;
	
	int ret = sendMsg(msg);
	if(ret != 0)
	{
		close(sockfd);
		return 0;
	}

	
	struct linger optval = {0};
	optval.l_onoff = 1;
	optval.l_linger = 60; // 60秒
	
	setsockopt(sockfd, SOL_SOCKET,SO_LINGER,&optval,sizeof(optval));
	
	/*
	if(value == NET_TRAN_MSG && msg.msg_i[0] == NET_TRAN_MSG_RECVED_MSG)
	{
		close(sockfd);
	}
	/**/
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// CRecvTerminal ///////////////////////////////////////////////////
//*
CServerTerminal::CServerTerminal()
{
}

CServerTerminal::~CServerTerminal()
{
}

/**/


int CServerTerminal::initNet(int nPort)
{
	//绑定端口地址
	struct sockaddr_in server_addr;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(nPort);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
    //创建套接字
    if ((listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)
	{
		e = {0};
		e.type = NET_TRAN_CREATE_SOCKET_ERROR;
		sprintf(e.message,"NET_TRAN_CREATE_SOCKET_ERROR\n");
		
		return NET_TRAN_ERROR;
    }
	//绑定socket
	if( bind(listenfd,(struct sockaddr *)&server_addr,sizeof(sockaddr)) == -1)
	{
		close(listenfd);
		e = {0};
		e.type = NET_TRAN_BIND_ERROR;
		sprintf(e.message,"NET_TRAN_BIND_ERROR\n");
		
		return NET_TRAN_ERROR;
	}
	// 开始监听
	if(listen(listenfd,SOMAXCONN) == -1)
	{
		close(listenfd);
		e = {0};
		e.type = NET_TRAN_LISTEN_ERROR;
		sprintf(e.message,"NET_TRAN_LISTEN_ERROR\n");
		
		return NET_TRAN_ERROR;
	}
	
	// 等待连接
	socklen_t socklen = sizeof(sockaddr);
    if((sockfd = accept(listenfd,(sockaddr*)(&remote_addr),&socklen))==-1)
	{
		close(listenfd);
		close(sockfd);
		
 		e = {0};
		e.type = NET_TRAN_ACCEPT_ERROR;
		sprintf(e.message,"NET_TRAN_ACCEPT_ERROR\n");
		
		return NET_TRAN_ERROR;
 	}else
	{
		printf("a client accept,ip = :%s",getRemoteIp().c_str());	
	}
	
	return 0;	
}


int CServerTerminal::unInitNet()
{
	close(listenfd);
	close(sockfd);
}



string CServerTerminal::getRemoteIp(void)
{
	string ip = inet_ntoa(remote_addr.sin_addr);
	return ip;
}