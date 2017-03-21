/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <string>

namespace extension
{

class Buffer
{
public:
	Buffer();
	~Buffer();

protected:
	char* m_pbuffer;
	unsigned int m_uwritepos;
	unsigned int m_ureadpos;
	unsigned int m_ucontentsize;
	unsigned int m_ubuffersize;

public:
	void clear();
	bool empty() const;
protected:
	inline void reallocBufferSizeInChanged(unsigned int u_len);
public:
	void writeData(const char* p_data, unsigned int u_len);
	void readData(char* p_out_data, unsigned int u_len);
	void readData(Buffer* p_ret, unsigned int u_len);
	// warning: need to delete[]
	char* readWholeData();
	int readInt();
	void writeInt(int data);
	unsigned int readUInt();
	void writeUInt(unsigned int data);
	std::string readString();
	void writeString(const char* p_data);
public:
	unsigned int getContentSize() const;
	bool isReadable() const;
	bool isReadable(unsigned int u_len) const;
	void moveLeft(unsigned int u_len);
	void moveRight(unsigned int u_len);
	void moveWriterIndexToFront();
	void moveWriterIndexToBack();
	void moveReaderIndexToFront();
	void moveReaderIndexToBack();
};

}
