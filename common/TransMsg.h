#pragma once

#include <cstddef>
#include <string.h>
#include <cstdlib>
#include <iostream>

class CTransMsg
{
public:
	enum {header_length = 4};
	enum {max_body_length = 1024};

	CTransMsg();
	~CTransMsg();

	const char * Data() const
	{
		return data_;
	}

	char * Data()
	{
		return data_;
	}

	//通信数据大小
	std::size_t Length() const
	{
		return header_length + body_length_;
	}

	const char * Body() const
	{
		return data_ + header_length;
	}

	char * Body()
	{
		return data_ + header_length;
	}

	std::size_t BodyLength() const
	{
		return body_length_;
	}

	void SetBodyLength(std::size_t new_length)
	{
		body_length_ = new_length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}

	bool decode_header()
	{
		char header[header_length + 1] = "";
		strncat_s(header, data_, header_length);
		body_length_ = std::atoi(header);
		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}
		else
			return true;
	}

	void encode_header()
	{
		char header[header_length + 1] = "";
		sprintf_s(header, "%4d", static_cast<int>(body_length_));
		std::memcpy(data_, header, header_length);
	}


private:
	char data_[header_length + max_body_length];
	std::size_t body_length_;
};

