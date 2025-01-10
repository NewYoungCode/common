//#include <sstream>
//#include "WebClient.h"
//#include "FileSystem.h"
//#include "Text.h"
//#include "WinTool.h"
//#include "JsonValue.h"
//#include "QrenCode.hpp"
//
//int main() {
//
//	Text::String linkName = L"C:\\Users\\ly\\Downloads";
//	Text::String target = L"D:\\down";
//
//	Text::String cmd = "cmd.exe /c mklink /j " + linkName + " " + target;
//	auto ret = WinTool::ExecuteCmdLine(cmd);
//
//
//	//新增openssl,zlib库
//
//	std::cout << "PCID:" << WinTool::GetComputerID() << std::endl;
//
//	//二维码的使用
//	//QrenCode::Generate("https://www.baidu.com", L"d:/aa.bmp");
//	auto bmp = QrenCode::Generate("https://www.baidu.com");
//	::DeleteObject(bmp);
//
//	WinTool::GetWinVersion();
//
//	JsonValue obj("aa");
//	Json::Value jv;
//
//	WebClient wc;
//
//	Text::String resp;
//	wc.HttpGet("https://songsearch.kugou.com/song_search_v2?platform=WebFilter&pagesize=20&page=1&keyword=dj", &resp);
//
//	auto w = resp.ansi();
//	std::cout << w;
//
//	obj = jv;
//
//	return 0;
//}

#include <sstream>
#include "Text.h"
#include "WinTool.h"
#include "Log.h"

//输入字符串
std::string inputString(const std::string tips) {
	std::string input;
	std::cout << tips;
	std::cin >> input;
	return input;
}

// 获取占用文件的进程PID
std::vector<DWORD> getPid(const Text::String& filePath) {
	std::vector<DWORD> pidList;
	Text::String str = WinTool::ExecuteCmdLine("handle.exe " + filePath);
	auto lines = str.split("\n");
	for (auto& it : lines) {
		auto line = it;
		auto pos = line.find("pid: ");
		if (pos != -1) {
			line = line.substr(pos + 5);
			pos = line.find(" ");
			line = line.substr(0, pos);
			pidList.push_back(std::atoi(line.c_str()));
		}
	}
	return pidList;
}

bool IsDirectoryRedirected(const std::wstring& directory, Text::String& out) {

	HANDLE hFile = CreateFileW(directory.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		auto code = ::GetLastError();
		Log::Info("code %d 无法打开路径  %s", code, Text::String(directory).c_str());
		return true; // 如果打开失败，返回 false
	}
	WCHAR finalPath[MAX_PATH];
	DWORD pathLength = GetFinalPathNameByHandleW(hFile, finalPath, MAX_PATH, FILE_NAME_NORMALIZED);
	CloseHandle(hFile);
	if (pathLength == 0) {
		return false;
	}
	std::wstring path = finalPath;
	size_t pos = path.find(L"\\\\?\\");
	if (pos != -1) {
		path = path.substr(4);
	}
	out = path;
	return  Text::String(directory) != out; // 如果路径不同，说明该目录已重定向
}
bool redirect(const Text::String& linkName, const Text::String& target, char disk) {

	//获取用户名称
	WCHAR user[256]{ 0 };
	DWORD len = 256;
	::GetUserNameW(user, &len);
	std::wstring userName = user;

	//替换用户名和盘符
	Text::Replace((Text::String*)&linkName, "{user}", Text::String(user));
	Text::Replace((Text::String*)&target, "{user}", Text::String(user));
	((Text::String&)target)[0] = disk;

	//查找占用的进程id
	std::vector<DWORD> pids = getPid(linkName);
	for (auto& pid : pids) {
		Log::Info("杀死进程 %d", pid);
		WinTool::CloseProcess(pid);
	}

	//判断是不是已经重定向过了
	Text::String out;
	if (IsDirectoryRedirected(linkName.unicode(), out)) {
		if (Path::Equal(target, out)) {
			return true;
		}
		else {
			Log::Info("[已有的重定向不正确,执行删除!]%s->%s", linkName.c_str(), out.c_str());
			Path::Delete(linkName);
		}
	}

	auto ok = Path::Copy(linkName, target);//先拷贝
	ok = Path::Delete(linkName);//删除

	Text::String cmd = "cmd.exe /c mklink /j " + linkName + " " + target;
	auto ret = WinTool::ExecuteCmdLine(cmd);

	Log::Info("%s", ret.c_str());
	return ret.empty();
}

//关闭打开游戏的时候弹出的GameBar
void closeGameBar() {

	{
		//计算机\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\GameDVR  (AppCaptureEnabled 32dword类型 改为0)
		HKEY subKey = NULL;
		do
		{
			if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\GameDVR", NULL, KEY_ALL_ACCESS, &subKey)) {
				break;
			}
			//删除AppCaptureEnabled
			::RegDeleteValueW(subKey, L"AppCaptureEnabled");
			DWORD value = 0; // 要设置的值
			if (::RegSetValueExW(subKey, L"AppCaptureEnabled", 0, REG_DWORD, (const BYTE*)&value, sizeof(value))) {
				break;
			}
		} while (false);
		::RegCloseKey(subKey);
	}

	{
		//计算机\HKEY_CURRENT_USER\System\GameConfigStore  (GameDVR_Enabled 改为0)
		HKEY subKey = NULL;
		do
		{
			if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CURRENT_USER, L"System\\GameConfigStore", NULL, KEY_ALL_ACCESS, &subKey)) {
				break;
			}
			//删除GameDVR_Enabled
			::RegDeleteValueW(subKey, L"GameDVR_Enabled");
			DWORD value = 0; // 要设置的值
			if (::RegSetValueExW(subKey, L"GameDVR_Enabled", 0, REG_DWORD, (const BYTE*)&value, sizeof(value))) {
				break;
			}
		} while (false);
		::RegCloseKey(subKey);
	}

	{
		//关闭桌面的"了解此图片"
		//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\HideDesktopIcons\NewStartPanel\{2cc5ca98-6485-489a-920e-b3e88a6ccce3} DWORD32 1 关闭
		HKEY subKey = NULL;
		do
		{
			if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel", NULL, KEY_ALL_ACCESS, &subKey)) {
				break;
			}
			::RegDeleteValueW(subKey, L"{2cc5ca98-6485-489a-920e-b3e88a6ccce3}");
			DWORD value = 1; // 要设置的值
			if (::RegSetValueExW(subKey, L"{2cc5ca98-6485-489a-920e-b3e88a6ccce3}", 0, REG_DWORD, (const BYTE*)&value, sizeof(value))) {
				break;
			}
		} while (false);
		::RegCloseKey(subKey);
	}
	{
		//桌面显示我的"我的电脑"
		//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\HideDesktopIcons\NewStartPanel\{20D04FE0-3AEA-1069-A2D8-08002B30309D} DWORD32 0显示我的电脑
		HKEY subKey = NULL;
		do
		{
			if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel", NULL, KEY_ALL_ACCESS, &subKey)) {
				break;
			}
			::RegDeleteValueW(subKey, L"{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
			DWORD value = 0; // 要设置的值
			if (::RegSetValueExW(subKey, L"{20D04FE0-3AEA-1069-A2D8-08002B30309D}", 0, REG_DWORD, (const BYTE*)&value, sizeof(value))) {
				break;
			}
		} while (false);
		::RegCloseKey(subKey);
	}
}

int main2() {



	Log::Enable = true;

	system("title 系统优化瘦身");

	auto a = inputString("C盘数据移动至哪个盘符?(A-Z):");

	char disk = 'D';//转移到的盘符
	if (!a.empty()) {
		disk = a[0];
	}

	if (disk == 'C' || disk == 'c') {
		Log::Info("你搁这儿搁这儿呢?");
		system("pause");
		return 0;
	}

	Log::Info("将会把C盘部分数据移动至:[%c]盘(不会影响原有数据)", disk);
	system("pause");

	//打开游戏的时候关闭gamebar的弹窗
	closeGameBar();

	//关闭系统休眠
	system("cmd.exe /c powercfg -h off");

	redirect(L"C:\\Users\\{user}\\Documents", L"?:\\Users\\{user}\\Documents", disk);//文档目录
	redirect(L"C:\\Users\\{user}\\Downloads", L"?:\\Users\\{user}\\Downloads", disk);//下载目录
	redirect(L"C:\\Users\\{user}\\Desktop", L"?:\\Users\\{user}\\Desktop", disk);//桌面目录
	redirect(L"C:\\Users\\{user}\\Music", L"?:\\Users\\{user}\\Music", disk);//音频目录
	redirect(L"C:\\Users\\{user}\\Videos", L"?:\\Users\\{user}\\Videos", disk);//视频目录
	redirect(L"C:\\Users\\{user}\\Pictures", L"?:\\Users\\{user}\\Pictures", disk);//照片目录
	redirect(L"C:\\Users\\{user}\\AppData\\Local\\Temp", L"?:\\Users\\{user}\\AppData\\Local\\Temp", disk);//Temp目录
	//system("explorer.exe");
	system("pause");
	return 0;
}

int main() {

	WinTool::RegisterLicenser("D:\\C++\\CoinsBuy\\build\\Debug\\LoopBuy.exe", ::Time::Now().ToString());
	auto data = WinTool::FindLicenser("D:\\C++\\CoinsBuy\\build\\Debug\\LoopBuy.exe");

	system("adb devices");
	system("1");
	system("pause");
	system("adb reboot edl");
	system("9008");
	system("pause");
	system("done");
	system("pause");
	File::Delete("667.zip");
	Path::Delete("667");
}