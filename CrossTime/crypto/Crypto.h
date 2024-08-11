#pragma once
#include <openssl/md5.h>
#include "Public.h"
class Crypto
{
public:
	static Buffer MD5(const Buffer& text);
};

