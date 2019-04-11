#pragma once
#ifndef AES_ENCRYPT_FILE
#define AES_ENCRYPT_FILE

#include "../Lib/prog_defines.h"

typedef unsigned __int64 QWORD;

class Cryptor
{
public:
	static void crypt_func(const String& filename, const String& filename2, bool is_decrypt);
private:
};

#endif