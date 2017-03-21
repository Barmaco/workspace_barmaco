/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#include "Buffer.h"
#include <assert.h>
#include <memory.h>

namespace extension
{

Buffer::Buffer() 
: m_pbuffer(NULL)
, m_uwritepos(0)
, m_ureadpos(0)
, m_ucontentsize(0)
, m_ubuffersize(512)
{
	m_pbuffer = (char*)malloc(m_ubuffersize);
	memset(m_pbuffer, 0, m_ubuffersize);
}

void Buffer::clear()
{
	m_uwritepos = 0;
	m_ureadpos = 0;
	m_ucontentsize = 0;
	memset(m_pbuffer, 0, m_ubuffersize);
}

Buffer::~Buffer()
{
	free(m_pbuffer);
}

void Buffer::writeData(const char* p_data, unsigned int u_len)
{
	assert(p_data && (u_len > 0));
	reallocBufferSizeInChanged(u_len);
	memcpy(m_pbuffer + m_uwritepos, p_data, u_len);
	m_uwritepos += u_len;
	m_ucontentsize = m_ucontentsize < m_uwritepos ? m_uwritepos : m_ucontentsize;
}

void Buffer::reallocBufferSizeInChanged(unsigned int u_len)
{
	while (true)
	{
		if (m_uwritepos + u_len <= m_ubuffersize)
		{
			break;
		}

		m_ubuffersize *= 2;
		m_pbuffer = (char*)realloc(m_pbuffer, m_ubuffersize);
		memset(m_pbuffer + m_ubuffersize / 2, 0, m_ubuffersize / 2);
	}
}

unsigned int Buffer::getContentSize() const
{
	return m_ucontentsize;
}

bool Buffer::isReadable() const
{
	if (m_ucontentsize == 0)
	{
		return false;
	}

	if (m_ureadpos < m_ucontentsize)
	{
		return true;
	} else 
	{
		return false;
	}
}

bool Buffer::isReadable(unsigned int u_len) const
{
	if (m_ucontentsize == 0 || u_len == 0)
	{
		return false;
	}

	if (m_ureadpos + u_len <= m_ucontentsize)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Buffer::readData(char* p_out_data, unsigned int u_len)
{
	if (isReadable(u_len))
	{
		memcpy(p_out_data, m_pbuffer+m_ureadpos, u_len);
		m_ureadpos += u_len;
	} else if ((m_ucontentsize - m_ureadpos>=0)&&(u_len != 0))
	{
		u_len = m_ucontentsize - m_ureadpos;
		memcpy(p_out_data, m_pbuffer + m_ureadpos, u_len);
		m_ureadpos += u_len;
	}
}

void Buffer::readData(Buffer* p_ret, unsigned int u_len)
{
	if (isReadable(u_len))
	{
		char* p_data = (char*)malloc(u_len);
		readData(p_data, u_len);
		//Buffer* p_ret = new Buffer(p_data, u_len);
		p_ret->writeData(p_data, u_len);
		free(p_data);
	}
}

char* Buffer::readWholeData()
{
	if (m_ucontentsize != 0)
	{
		char* p_buffer = new char[m_ucontentsize];
		memcpy(p_buffer, m_pbuffer, m_ucontentsize);
		m_ureadpos = m_ucontentsize;
		return p_buffer;
	} 
	return NULL;
}

void Buffer::moveLeft(unsigned int u_len)
{
	if (m_ucontentsize == 0)
	{
		return;
	}
	assert(u_len != 0);

	if (u_len >= m_ucontentsize)
	{
		clear();
	} else
	{
		for (unsigned int i = u_len; i < m_ucontentsize; ++i)
		{
			*(m_pbuffer + (i - u_len)) = *(m_pbuffer + i);
			*(m_pbuffer + i) = 0;
 		}

		m_uwritepos = (unsigned int)( 0 < ((int)m_uwritepos - (int)u_len) ? ((int)m_uwritepos - (int)u_len) : 0 );
		m_ureadpos = (unsigned int)(0 < ((int)m_ureadpos - (int)u_len) ? ((int)m_ureadpos - (int)u_len) : 0);
		m_ucontentsize -= u_len;
	}
}

void Buffer::moveRight(unsigned int u_len)
{
	if (m_ucontentsize == 0)
	{
		return;
	}
	assert(u_len != 0);

	reallocBufferSizeInChanged(u_len);

	for (int i = m_ucontentsize - 1; i >= 0; --i)
	{
		*((m_pbuffer + i) + (int)u_len) = *(m_pbuffer + i);
		*(m_pbuffer + i) = 0;
	}

	m_uwritepos += u_len;
	m_ureadpos += u_len;
	m_ucontentsize += u_len;
}

void Buffer::moveWriterIndexToFront()
{
	m_uwritepos = 0;
}

void Buffer::moveWriterIndexToBack()
{
	m_uwritepos = m_ucontentsize;
}

void Buffer::moveReaderIndexToFront()
{
	m_ureadpos = 0;
}

void Buffer::moveReaderIndexToBack()
{
	m_ureadpos = m_ucontentsize;
}

int Buffer::readInt()
{
	if (isReadable(sizeof(int)))
	{
		char p_temp[sizeof(int)] = { 0 };
		memcpy(p_temp, m_pbuffer + m_ureadpos, sizeof(int));
		std::reverse(&(p_temp[0]), &(p_temp[sizeof(int)]));
		int ret = *(int*)p_temp;
		m_ureadpos += sizeof(int);
		return ret;
	} 
	return 0;
}

void Buffer::writeInt(int data)
{
	char p_data[sizeof(int)] = { 0 };
	memcpy(p_data, (char*)(&data), sizeof(int));
	std::reverse(&(p_data[0]), &(p_data[sizeof(int)]));
	writeData(p_data, sizeof(int));
}

unsigned int Buffer::readUInt()
{
	return (unsigned int)readInt();
}

void Buffer::writeUInt(unsigned int data)
{
	char p_data[sizeof(int)] = {0};
	memcpy(p_data, (char*)(&(data)), sizeof(int));
	std::reverse(&(p_data[0]), &(p_data[sizeof(int)]));
	writeData(p_data, sizeof(int));
}

bool Buffer::empty() const
{
	if (getContentSize() == 0)
	{
		return true;
	} 
	return false;
}

std::string Buffer::readString()
{
	int len = readInt();

	std::string str;
	char* p_data = (char*)malloc(len + 1);
	readData(p_data, len);
	p_data[len] = 0;
	str.assign(p_data);
	free(p_data);
	return str;
}

void Buffer::writeString(const char* p_data)
{
	assert(p_data && (strlen(p_data) > 0));

	writeInt(strlen(p_data));
	writeData(p_data, strlen(p_data));
}

}