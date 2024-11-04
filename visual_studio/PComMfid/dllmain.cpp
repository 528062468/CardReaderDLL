// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"


//extern "C" {
//	__declspec(dllexport) BOOL ReadCardNum(CString caPositionId, CString& cdCardNum, CString& csErrInfo);
//}




CString g_szComPort;
map<CString,int> g_map;
NLock Lock;

namespace PComMfid
{
	const char IID_IPComMfid[] = "IPComMfid";
	interface IPComMfid : public IUnknown
	{
		virtual BOOL __stdcall ReadCardNum(CString csPositionId, CString & csCardNum, CString& csErrInfo) = 0;
	};
}



//接口类
class CComMfid : public PComMfid::IPComMfid
{
public:
	CComMfid() : m_refCount(1) {} // 初始化 refCount

	// IUnknown methods
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override
	{
		if (riid == IID_IUnknown /*|| riid == PComMfid::IID_IPComMfid*/)
		{
			*ppv = static_cast<PComMfid::IPComMfid*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}


	virtual ULONG __stdcall AddRef() override
	{
		return InterlockedIncrement(&m_refCount);
	}

	virtual ULONG __stdcall Release() override
	{
		ULONG ref = InterlockedDecrement(&m_refCount);
		if (ref == 0)
		{
			delete this;
		}
		return ref;
	}

	// Implement IPComMfid methods
	virtual BOOL __stdcall ReadCardNum(CString csPositionId, CString & csCardNum, CString& csErrInfo) override
	{
		Lock.Lock();
		try {
			BOOL temp  = run(csPositionId, csCardNum, csErrInfo);
			Lock.UnLock();
			return temp;
		}
		catch (exception& e) {
			csErrInfo = CString(_T("Unhandled Exception: ")) + e.what();
			Lock.UnLock();
			return false;
		}
	}

private:
	ULONG m_refCount; // Reference count for COM object
};


extern "C" __declspec(dllexport) LPVOID CreateObject(LPCSTR lpcsInterfaceName, LPVOID lpInitData)
{
		return new CComMfid(); 
}

extern "C" __declspec(dllexport)  BOOL ReadCardNum2(CString caPositionId, CString& cdCardNum, CString& csErrInfo)
{
	Lock.Lock();
	try {
		BOOL temp  = run(caPositionId, cdCardNum, csErrInfo);
		Lock.UnLock();
		return temp;
	}
	catch (exception& e) {
		csErrInfo = CString(_T("Unhandled Exception: ")) + e.what();
		Lock.UnLock();
		return false;
	}
}

void ShowTopMostMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, lpszText, lpszCaption, nType);
}

void LoadConfig()
{
    g_szComPort = _T("");
    TiXmlDocument doc("MFID_POSITION_MAPPING_TABLE.xml");

    // 加载 XML 文件
    if (!doc.LoadFile()) {
        ShowTopMostMessageBox(_T("文件\"MFID_POSITION_MAPPING_TABLE.xml\"打开失败 "), _T("Error"), MB_OK);
        return;
    }
    // 获取根元素
    TiXmlElement* root = doc.RootElement();
    if (root) {
        const char* com = root->Attribute("COM");
        const char* encoding = root->Attribute("encoding");
        //std::cout << "COM: " << (com ? com : "not found") << std::endl;
        //std::cout << "Encoding: " << (encoding ? encoding : "not found") << std::endl;
        if (com)
        {
            int iComPort = atoi(com);
            if (iComPort > 9)
                g_szComPort.Format(_T("\\\\.\\COM%d"), iComPort);
            else if (iComPort == 0)
            {
                ShowTopMostMessageBox(_T("串口转换失败!"), _T("Error"), MB_OK);
                return;
            }
            else
                g_szComPort.Format(_T("COM%d"), iComPort);
        }
        else
        {
            ShowTopMostMessageBox(_T("扫码器串口解析失败,请检查配置文件!"), _T("Error"), MB_OK);
            return;
        }

        // 遍历子元素
        for (TiXmlElement* map = root->FirstChildElement("Map"); map != nullptr; map = map->NextSiblingElement("Map")) {
            const char* positomIf = map->Attribute("PositonId");
            const char* cardReaderId = map->Attribute("CardReaderId");
            /*std::cout << "PositomIf: " << (positomIf ? positomIf : "not found") << ", "
                << "CardReaderId: " << (cardReaderId ? cardReaderId : "not found") << std::endl;*/
            if (positomIf && cardReaderId)
            {
                g_map[positomIf] = atoi(cardReaderId);
            }
        }
    }
    else {
        ShowTopMostMessageBox(_T("获取根目录失败,请检查文件\"MFID_POSITION_MAPPING_TABLE.xml\" 是否格式异常!"), _T("Error"), MB_OK);
        return;
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH://TODO:一导入动态库即获取xml文件信息
        LoadConfig();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        //判断串口是否打开,打开时则关闭
        break;
    }
    return TRUE;
}


BOOL run(CString szPosition, CString& cdCardNum, CString& csErrInfo)
{
    if (g_map.find(szPosition) == g_map.end())
    {
        csErrInfo.Format(_T("找不到位置信息(%s)对应的扫码器,请检测配置文件:"), szPosition);
        for (auto t = g_map.begin(); t != g_map.end(); t++)
        {
            csErrInfo += (t->first + _T(" "));
        }
        if (csErrInfo == _T(""))
            csErrInfo = _T("配置文件解析失败,请重新加载动态库!");
        return false;
    }
    if (g_szComPort.IsEmpty())
    {
        csErrInfo = _T("串口解析失败,请重新加载动态库!");
        return false;
    }
    int iCardAddress = g_map[szPosition];
    unsigned long baud = 19200;
    
    serial::Serial my_serial(g_szComPort.GetBuffer(), baud, serial::Timeout::simpleTimeout(150), serial::eightbits, serial::parity_even);
    
    if (!my_serial.isOpen())
    {
        csErrInfo.Format(_T("串口:%s打开失败!"), g_szComPort);
        return false;
    }

    // Get the Test string
    int count = 0;
    string test_string;
    test_string.resize(7);
    test_string[0] = 0x09;//固定码
    test_string[1] = 'A';//固定码
    test_string[2] = iCardAddress < 10 ? ('0' + iCardAddress) : ('A' + iCardAddress - 10);//模块地址
    test_string[3] = 'F';//功能码
    string ret = GenerateBCC(string(test_string.data(), 4));
    test_string[4] = ret[0];//BCC码高字节(ASCII)
    test_string[5] = ret[1];//BCC码低字节(ASCII)
    test_string[6] = 0x0d;
    {
        size_t bytes_wrote = my_serial.write(test_string);
        string result = my_serial.read(100);
        if (result.size() == 0 
            || result.size() <= test_string.size()//帧长度不对
            || result[0] != 0x0A//帧头不对
            || result[result.size()-1] != 0x0D//帧尾不对
            )
        {
			Sleep(5);//重试
			test_string[3] = 'G';//功能码
			string ret = GenerateBCC(string(test_string.data(), 4));
			test_string[4] = ret[0];
			test_string[5] = ret[1];
            bytes_wrote = my_serial.write(test_string);
            string result = my_serial.read(100);
            if (result.size() == 0
                || result.size() <= test_string.size()//帧长度不对
                || result[0] != 0x0A//帧头不对
                || result[result.size() - 1] != 0x0D//帧尾不对
                )
            {
                if (result.size() == 0)
                    csErrInfo = _T("设备未响应!");
                else if(result.size() < test_string.size())
                    csErrInfo = _T("返回数据长度异常!");
                else if (result[0] != 0x0A)
                    csErrInfo = _T("返回数据帧头格式异常!");
                else if (result[result.size() - 1] != 0x0D)
                    csErrInfo = _T("返回数据帧尾格式异常!");
                else 
					csErrInfo = _T("读取条码失败!");
                my_serial.close();
                return false;
            }
            else
            {//正常读到数据
                if (!CheckBCC(result))//BCC校验失败,重读1次
                {//重新读取条码,0x09	A	1	G		BCC1	BCC2	0x0D
					csErrInfo = _T("BCC校验失败!");
					my_serial.close();
					return false;
                }
                else
                {
                    cdCardNum = GetCardNum(result).c_str();
                    my_serial.close();
					return true;
                }
            }
        }
        else
        {//正常读到数据
            if (!CheckBCC(result))//BCC校验失败,重读1次
            {//重新读取条码,0x09	A	1	G		BCC1	BCC2	0x0D
				csErrInfo = _T("BCC校验失败!");
				my_serial.close();
				return false;
            }
            else
            {
                cdCardNum = GetCardNum(result).c_str();
                my_serial.close();
                return true;
            }
        }
    }//未读到条码的情况下,手动发送读取条码指令{TODO},发完后隔50ms重新读取条码{}
    return true;
}


string GenerateBCC(string src)
{
    UCHAR BCC = 0;
    for (int i = 0; i < src.size(); i++) {
        BCC ^= src[i];
    }
    string data;
    data.resize(2);
    UCHAR hValue = BCC >> 4;
    UCHAR lValue = BCC & 0x0F; // 修正这里
    data[0] = (hValue > 9) ? 'A' + (hValue - 10) : '0' + hValue; // 使用字符的方式
    data[1] = (lValue > 9) ? 'A' + (lValue - 10) : '0' + lValue; // 使用字符的方式
    return data;
}

BOOL CheckBCC(string src)
{
    if (src.size() < 3)
        return false;
    UCHAR BCC = 0;
    for (int i = 0; i < src.size()-3; i++)
    {
        BCC ^= src[i];
    }
    string data;
    data.resize(2);
    UCHAR hValue = BCC >> 4;
    UCHAR lValue = BCC & 0x0F; // 修正这里
    data[0] = (hValue > 9) ? 'A' + (hValue - 10) : '0' + hValue; // 使用字符的方式
    data[1] = (lValue > 9) ? 'A' + (lValue - 10) : '0' + lValue; // 使用字符的方式
    if (src[src.size() - 3] == data[0] && src[src.size() - 2] == data[1])
        return true;
    else
        return false;
}

std::string hexToDecimalWithLeadingZeros(const std::string& hexStr) {
    // 将十六进制字符串转换为十进制数
    unsigned long long decimalValue = 0;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> decimalValue;

    // 将十进制数转换为字符串，并保持前导零
    std::ostringstream decimalStream;
    decimalStream << std::setw(10) << std::setfill('0') << decimalValue;

    return decimalStream.str();
}

string GetCardNum(string str)
{
    string data;
    if (str.size() < 7)
        return data;
    data = hexToDecimalWithLeadingZeros(string(str.data()+4, str.size()-7));
    return data;
}