这是把 十进制的命令和参数转换成具体的命令。
主要为了分析utiltiy 的几个脚本 。

去掉了 HOST_CMD_QUEUE_POINT_NEW 对应的限幅：
//#define PT2_THRESHOLD 40000
//				if(pt[2]>PT2_THRESHOLD)
//					pt[2]=PT2_THRESHOLD;
//				if(pt[2]<-PT2_THRESHOLD)
//					pt[2]=-PT2_THRESHOLD;

