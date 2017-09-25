// CompressionDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <Snappy/snappy.h>
#include <LZ4/lz4.h>
#include <ctime>
using namespace std;

void TestLZ4ProtoMsg() {
	struct SMsg {
		SMsg() : len(0) { memset(szMsg, 0, sizeof(szMsg)); }
		char szMsg[10240];
		int len;
	};

	ifstream ifs("protomsg.txt", std::iostream::in | std::iostream::binary);
	vector<SMsg> msgs;
	do {
		SMsg msg;
		while (ifs) {
			ifs.get(msg.szMsg[msg.len++]);

			if (msg.len >= 6 && !strcmp(msg.szMsg + msg.len - 6, "\r\n\r\n\r\n")) {
				msg.len -= 6;
				break;
			}
		}

		msgs.push_back(msg);
	} while (ifs);
	ifs.close();

	int count = 1;
	for_each(msgs.begin(), msgs.end(), [&count](SMsg& msg)->void {
		char szCompress[10240] = "";
		int compressLen = LZ4_compress_default(msg.szMsg, szCompress, msg.len, sizeof(szCompress));

		char szDecompress[10240] = "";
		int decompressLen = LZ4_decompress_safe(szCompress, szDecompress, compressLen, sizeof(szDecompress));

		printf("%5d old:%5d com:%5d decom:%5d same:%d ratio:%2.2f\n",
			count++, msg.len, compressLen, decompressLen, msg.len == decompressLen ? 1 : 0, float(compressLen) / msg.len);
	});
}

void TestLZ4ZX() {
	struct SMsg {
		SMsg() : len(0) { memset(szMsg, 0, sizeof(szMsg)); }
		char szMsg[102400];
		int len;
	};

	ifstream ifs("zx.txt", std::iostream::in | std::iostream::binary);
	vector<SMsg> msgs;
	do {
		SMsg msg;
		while (ifs) {
			ifs.get(msg.szMsg[msg.len++]);
			if (msg.len >= 100000)
				break;
		}

		msgs.push_back(msg);
	} while (ifs);
	ifs.close();

	int count = 1;
	for_each(msgs.begin(), msgs.end(), [&count](SMsg& msg)->void {
		char szCompress[102400] = "";
		clock_t tStart = clock();
		int compressLen = LZ4_compress_default(msg.szMsg, szCompress, msg.len, sizeof(szCompress));
		clock_t cost = clock() - tStart;

		char szDecompress[102400] = "";
		int decompressLen = LZ4_decompress_safe(szCompress, szDecompress, compressLen, sizeof(szDecompress));

		printf("%5d old:%5d com:%5d decom:%5d same:%d ratio:%2.2f\n",
			count++, msg.len, compressLen, decompressLen, msg.len == decompressLen ? 1 : 0, float(compressLen) / msg.len);
	});
}


void TestProtoMsg() {
	struct SMsg {
		SMsg() : len(0) { memset(szMsg, 0, sizeof(szMsg)); }
		char szMsg[10240];
		int len;
	};

	ifstream ifs("protomsg.txt", std::iostream::in | std::iostream::binary);
	vector<SMsg> msgs;
	do {
		SMsg msg;
		while (ifs) {
			ifs.get(msg.szMsg[msg.len++]);

			if (msg.len >= 6 && !strcmp(msg.szMsg + msg.len - 6, "\r\n\r\n\r\n")) {
				msg.len -= 6;
				break;
			}
		}

		msgs.push_back(msg);
	} while (ifs);
	ifs.close();

	int count = 1;
	for_each(msgs.begin(), msgs.end(), [&count](SMsg& msg)->void {
		string strCompress;
		snappy::Compress(msg.szMsg, msg.len, &strCompress);

		string strCopy = strCompress;
		string strCopy1 = strCompress.c_str();

		string strUnCompress;
		snappy::Uncompress(strCompress.c_str(), strCompress.length(), &strUnCompress);

		printf("%5d old:%5d comp:%5d uncom:%5d same:%d ratio:%2.2f\n",
			count++, msg.len, strCompress.length(), strUnCompress.length(), msg.len == strUnCompress.length() ? 1 : 0, float(strCompress.length()) / msg.len);
	});
}

void TestZX() {
	struct SMsg {
		SMsg() : len(0) { memset(szMsg, 0, sizeof(szMsg)); }
		char szMsg[102400];
		int len;
	};

	ifstream ifs("zx.txt", std::iostream::in | std::iostream::binary);
	vector<SMsg> msgs;
	do {
		SMsg msg;
		while (ifs) {
			ifs.get(msg.szMsg[msg.len++]);
			if (msg.len >= 100000)
				break;
		}

		msgs.push_back(msg);
	} while (ifs);
	ifs.close();

	int count = 1;
	for_each(msgs.begin(), msgs.end(), [&count](SMsg& msg)->void {
		clock_t tStart = clock();
		string strCompress;
		snappy::Compress(msg.szMsg, msg.len, &strCompress);
		clock_t cost = clock() - tStart;

		string strUnCompress;
		snappy::Uncompress(strCompress.c_str(), strCompress.length(), &strUnCompress);

		printf("%5d old:%5d comp:%5d uncom:%5d same:%d ratio:%2.2f %u\n",
			count++, msg.len, strCompress.length(), strUnCompress.length(), msg.len == strUnCompress.length() ? 1 : 0, float(strCompress.length()) / msg.len,
			cost);
	});
}

int main()
{
	// 	string str = "�ɳ��Ĳ�����������Ϊ˭��ֹͣ����֪��������֪�����ʱ��ĽŲ�����������Ҳ�����˺쳾�Ķɿڣ�����������ڶ��ű߻����Ŷ��ٵĳ���ԹŮ��Ψ�������ĸ�����²�֪�ö����˸�Ըһ���������쳾�ɿ��£�������������˳���ӿ���Ǹ����Ե��㣬��Ը�������ص��㡣������������ǣ�֣�������ԸΪ���غ�";
	// 	cout << "str: " << str << " " << str.length() << endl;
	// 
	// 	string strCompress;
	// 	snappy::Compress(str.c_str(), str.length(), &strCompress);
	// 	cout << "strCompress: " << strCompress << " " << strCompress.length() << endl;
	// 
	// 	string strUncompress;
	// 	snappy::Uncompress(strCompress.c_str(), strCompress.length(), &strUncompress);
	// 	cout << "strUncompress: " << strUncompress << endl;

	// 	ostringstream oss;
	// 	oss << endl;
	// 	string str = oss.str();

	// 	std::string str("Test \0string");
	// 	std::cout << str.length() << endl;

	TestProtoMsg();

	//TestZX();

	//TestLZ4ProtoMsg();

	//TestLZ4ZX();

	system("pause");
	return 0;
}