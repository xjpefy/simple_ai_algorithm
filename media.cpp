#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <cstdint>
#include <vector>
#include <chrono>
#include <string.h>
#include <cstring>

#define WM_SOCKET (WM_USER + 1)

using namespace std;




//windows版执行端

bool Self_Working = 1;
bool Self_Communicate = 0;
bool Self_Capture = 0;
bool Self_Action = 0;

int Inside_MODEL_TIME = -1;  //模型时间
int RECV_MODEL_TIME = 0;  //

bool If_Send_Text = 0;
bool If_Display_Text = 0;
bool If_Show_Window = 1;

vector<char> Recv_Text;  //接收运算端的文本
vector<char> Curr_Text;  //当前界面的文本

const int RECV_PORT = 5095;   // 使用端口
const long long VERIFICATION = 0b01010101010101010101010101010101;//校验码

SOCKET execuation_fd = INVALID_SOCKET;

HWND hEditInputText;
HWND hStaticStateShow;
HWND hwnd;
HWND test_time_Show;//测试使用延时

auto start_test = chrono::steady_clock::now();
auto now_test = chrono::steady_clock::now();
int late_time = 0;



//系统时间获取函数
int get_current_second()
{
    //获取当前时间点
    auto system_now = chrono::system_clock::now();
    int current_system_seconds = chrono::duration_cast<chrono::seconds>(system_now.time_since_epoch()).count();

//返回为格林威治时间
return current_system_seconds;
}
//函数结束


//截图函数
bool CaptureScreenRGB(
    vector<uint8_t>& return_RGB_map,
    int& return_width, int& return_height)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
    //获取显示器真实分辨率（不受 DPI 缩放影响）
    DEVMODE dm{};
    dm.dmSize = sizeof(dm);
    if ( !EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm) )
        return false;

    return_width  = static_cast<int>(dm.dmPelsWidth);
    return_height = static_cast<int>(dm.dmPelsHeight);

    //获取屏幕 DC
    HDC hScreen = GetDC(NULL);
    if (!hScreen) return false;

    HDC hMemDC = CreateCompatibleDC(hScreen);
    if (!hMemDC) {
        ReleaseDC(NULL, hScreen);
        return false;
    }

    //创建位图（按真实分辨率）
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, return_width, return_height);
    if (!hBitmap) {
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreen);
        return false;
    }

    SelectObject(hMemDC, hBitmap);

    //拷贝屏幕内容
    BitBlt(hMemDC, 0, 0, return_width, return_height, hScreen, 0, 0, SRCCOPY);

    //准备位图信息
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = return_width;
    bmi.bmiHeader.biHeight = -return_height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;    // BGR
    bmi.bmiHeader.biCompression = BI_RGB;

    int stride = ( (return_width * 3 + 3) & ~3 );//奇怪，待优化
    vector<uint8_t> bgrData(stride * return_height);
    return_RGB_map.resize(return_width * return_height * 3);

    //取像素
    if (!GetDIBits(hMemDC, hBitmap, 0, return_height,
        bgrData.data(), &bmi, DIB_RGB_COLORS))
    {
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreen);
        return false;
    }

    //BGR->RGB
    for (int y = 0; y < return_height; ++y)
    {
        const uint8_t* src = bgrData.data() + y * stride;
        uint8_t* dst = return_RGB_map.data() + y * return_width * 3;

        for (int x = 0; x < return_width; ++x)
        {
            int i = x * 3;
            dst[i + 0] = src[i + 2]; // R
            dst[i + 1] = src[i + 1]; // G
            dst[i + 2] = src[i + 0]; // B
        }
    }

    //清理
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreen);

return true;
}
//函数结束

//字符格式转换函数
vector<TCHAR> char_change_to_TCHAR(vector<char> charVec)
{
    // #ifdef _UNICODE
    //     string str(charVec.begin(), charVec.end());
    //     int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    //     vector<TCHAR> tcharVec(len);
    //     MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, tcharVec.data(), len);
    // #else
        vector<TCHAR> tcharVec(charVec.begin(),charVec.end());
    // #endif

    return tcharVec;
}

//获取文本
int acquire_curr_text()
{
    int num;

    // #ifdef _UNICODE//宽字符
    //     vector<wchar_t> wideVec;
    //     num = GetWindowTextLengthW(hEditInputText);
    //     GetWindowTextW(hEditInputText, wideVec.data(), num + 1);

    //     num = WideCharToMultiByte(CP_UTF8, 0, wideVec.data(), wideVec.size(), nullptr, 0, nullptr, nullptr);
    //     if(num > 65534)
    //         num = 65534;

    //     Curr_Text.resize(num + 1);

    //     WideCharToMultiByte(CP_UTF8, 0, wideVec.data(), wideVec.size(), Curr_Text.data(), num, nullptr, nullptr);
    // #else//窄字符
        num = GetWindowTextLengthA(hEditInputText);
        if(num > 65534)
            num = 65534;
        Curr_Text.resize(num + 1);
        GetWindowTextA(hEditInputText, Curr_Text.data(), num + 1);
    // #endif

    return num;
}

//程序指令

//发送信息概览
struct Send_overview{
    char execulate_action_num = 0;
    unsigned short width;   // 图片宽度
    unsigned short height;  // 图片高度
    unsigned short text_num;// 文本字节
    int code_time;    // 程序时间
};//10字节


//接收指令概览
struct Recv_Instruct{
    char if_working;
    char if_commun;
    char if_action;
    char if_capture;
    char action_num;
    unsigned short text_num;// 文本字节量
    int model_time;   // 模型时间
};//11字节


//行动输出 windows版本
struct Model_Action{
    char action_order = 0;//行动序号
    char action_kind = 0;//行动类型，1鼠标，2键盘，3文本传回，4文本显示，5窗口隐藏，6窗口显示
    unsigned char vk;//键盘可选类型
    char mouseData; //鼠标可选类型，滚轮120/格 
    short x,y;//鼠标可选类型，偏移量 
    unsigned int dwFlags;// 行为标志 
};





//自动化输出函数
void Action_to_Sendinput(const Model_Action& act)
{
    INPUT input{};
    input.type = (act.action_kind == 1) ? INPUT_MOUSE : INPUT_KEYBOARD;

    if (act.action_kind == 1) {
        // 鼠标输出
        input.mi.dx = act.x;
        input.mi.dy = act.y;
        input.mi.mouseData = act.mouseData *120;
        input.mi.dwFlags = act.dwFlags;

        input.mi.time = 0;
        input.mi.dwExtraInfo = 0;
        SendInput(1, &input, sizeof(INPUT));

    } else if (act.action_kind == 2) {
        // 键盘输出
        input.ki.wVk = act.vk;
        input.ki.wScan = MapVirtualKey(act.vk, MAPVK_VK_TO_VSC);
        input.ki.dwFlags = act.dwFlags;

        input.ki.time = 0;
        input.ki.dwExtraInfo = 0;
        SendInput(1, &input, sizeof(INPUT));

    } else if (act.action_kind == 3) {
        //回传文本
        If_Send_Text = 1;

    } else if (act.action_kind == 4) {
        //显示文本
        If_Display_Text = 1;
        vector<TCHAR> text = char_change_to_TCHAR(Recv_Text);
        SetWindowText(hEditInputText, text.data());
        Curr_Text = Recv_Text;

    } else if (act.action_kind == 5) {
        //隐藏窗口
        If_Show_Window = 0;
        ShowWindow(hwnd, SW_MINIMIZE);

    } else if (act.action_kind == 6) {
        //显示窗口
        If_Show_Window = 1;
        ShowWindow(hwnd, SW_SHOW);
    }


}//函数结束



//通信线程函数
int Socket_thread()
{
    Curr_Text.reserve(50*1024);
    Recv_Text.reserve(50*1024);

    //socket启动
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "WSAStartup 失败\n";
        return 1;
    }

    execuation_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(RECV_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(execuation_fd, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        cout << "bind 失败，错误码：" << err << "\n";
        closesocket(execuation_fd);
        return 1;
    }

    if (listen(execuation_fd, 5) == SOCKET_ERROR)
    {
        cout << "listen 失败，错误码：" << WSAGetLastError() << "\n";
        closesocket(execuation_fd);
        return 1;
    }

    sockaddr_in client{};
    int len = sizeof(client);

    execuation_fd = accept(execuation_fd, (sockaddr*)&client, &len);

    //若线程连接成功
    Self_Communicate = 1;

    //更改缓冲区大小
    int snd_buf = 30 * 1024 * 1024;  // 30 MB
    setsockopt(execuation_fd, SOL_SOCKET, SO_SNDBUF,
        (char*)&snd_buf, sizeof(snd_buf));

    int rcv_buf = 512*1024; // 512 KB
    setsockopt(execuation_fd, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_buf, sizeof(rcv_buf));

    BOOL flag = TRUE;
    setsockopt( execuation_fd, IPPROTO_TCP, TCP_NODELAY,
        (char*)& flag, sizeof(flag) );

    vector<char> Data_Recv_Vector;
    vector<char> Data_Send_Vector;
    Data_Recv_Vector.reserve(512*1024);
    Data_Send_Vector.reserve(30*1024*1024);

    Recv_Instruct r_i;
    char action_buffer[60];

    long long verification = 0;

    //进入工作循环
    while(Self_Working && Self_Communicate)
    {
        start_test = chrono::steady_clock::now();

        /*接收消息*/
        do{
            recv(execuation_fd, (char*)&verification, sizeof(long long), 0);
        }while(verification != VERIFICATION) ;//校验码不通过，文件顺序错误，

        recv(execuation_fd, Data_Recv_Vector.data(), 15, 0);// ===== 文件大小 =====

        memcpy( &r_i.if_working, Data_Recv_Vector.data(), 1);
        memcpy( &r_i.if_commun, Data_Recv_Vector.data() + 1, 1);
        memcpy( &r_i.if_action, Data_Recv_Vector.data() + 2, 1);
        memcpy( &r_i.if_capture, Data_Recv_Vector.data() + 3, 1);
        memcpy( &r_i.action_num, Data_Recv_Vector.data() + 4, 1);
        memcpy( &r_i.text_num, Data_Recv_Vector.data() + 5, 2);
        memcpy( &r_i.model_time, Data_Recv_Vector.data() + 7, 4);

        Data_Recv_Vector.clear();

        Self_Working = r_i.if_working;
        Self_Communicate = r_i.if_commun;
        Self_Capture = r_i.if_capture;
        Self_Action = r_i.if_action;
        RECV_MODEL_TIME = r_i.model_time;


        //传入文本接收
        if(r_i.text_num != 0)
        {
            Recv_Text.reserve(r_i.text_num + 1);
            Recv_Text.resize(r_i.text_num);
            int already_read = 0;

            while(already_read < r_i.text_num*4)
            {
                already_read += recv(execuation_fd, (char*)&Recv_Text + already_read, r_i.text_num*4 - already_read, 0);
            }

            Recv_Text.push_back('\0');
        }

        execulate_action_num = 0;
        //行动指令接收
        if(r_i.action_num != 0)
        {
            recv(execuation_fd, action_buffer, r_i.action_num*12, 0);

            //执行模型行动
            if(Self_Action == 1)
            {
                for(int r = 0;r < r_i.action_num;r++)
                {
                    Model_Action output;
                    output.action_order = action_buffer[r*12];
                    output.action_kind = action_buffer[r*12 + 1];
                    output.vk = (unsigned char)action_buffer[r*12 + 2];
                    output.mouseData = action_buffer[r*12 + 3];
                    output.x = action_buffer[r*12 + 4];
                    output.y = action_buffer[r*12 + 6];
                    output.dwFlags = (unsigned int)action_buffer[r*12 + 4];

                    Action_to_Sendinput(output);

                    execulate_action_num++;
                }
            }
        }

        
        //接收完成

        //更新模型时间
        if( Inside_MODEL_TIME != RECV_MODEL_TIME)
        {
            //丢包情况判断
            if( abs(RECV_MODEL_TIME - Inside_MODEL_TIME) > 1)
            {
                //首次，更新
                if( Inside_MODEL_TIME == -1 )
                    Inside_MODEL_TIME = RECV_MODEL_TIME;
                else {
                    //丢包
                }
            }

            Inside_MODEL_TIME = RECV_MODEL_TIME;
        }

        vector<uint8_t> Send_Rgb_Map;
        int width = 0, heigth = 0;

        //进行截图工作
        if(Self_Capture)
        {
            CaptureScreenRGB(Send_Rgb_Map, width, heigth);
        }

        int length = 0;
        //文本发送工作
        if(If_Send_Text)
        {
            length = acquire_curr_text();
        }

        /*发送消息*/
        Send_overview so;
        so.execulate_action_num = execulate_action_num;
        so.width = width;
        so.height = heigth;
        so.text_num = length;
        so.code_time = Inside_MODEL_TIME;

        // ===== 文件概览 =====
        Data_Send_Vector.insert(Data_Send_Vector.end(), (char*)&VERIFICATION, (char*)&VERIFICATION + 8);
        Data_Send_Vector.insert(Data_Send_Vector.end(), (char*)&so.execulate_action_num, (char*)&so.execulate_action_num + 1);
        Data_Send_Vector.insert(Data_Send_Vector.end(), (char*)&so.width, (char*)&so.width + 2);
        Data_Send_Vector.insert(Data_Send_Vector.end(), (char*)&so.height, (char*)&so.height + 2);
        Data_Send_Vector.insert(Data_Send_Vector.end(), (char*)&so.text_num, (char*)&so.text_num + 2);
        Data_Send_Vector.insert(Data_Send_Vector.end(), (char*)&so.code_time, (char*)&so.code_time + 4);

        // ===== 数据图文 =====
        Data_Send_Vector.insert(Data_Send_Vector.end(), (const char*)Send_Rgb_Map.data(), (const char*)Send_Rgb_Map.data() + so.width*so.height*3);
        if(If_Send_Text == 1)
        {
            Data_Send_Vector.insert(Data_Send_Vector.end(), (const char*)Curr_Text.data(), (const char*)Curr_Text.data() + so.text_num);
            If_Send_Text = 0;
        }

        send(execuation_fd, (const char*)Data_Send_Vector.data(), Data_Send_Vector.size(), 0);

        Data_Send_Vector.clear();
        Curr_Text.clear();
        Send_Rgb_Map.clear();

        
        //延时测试
        now_test = chrono::steady_clock::now();
        late_time = chrono::duration_cast<chrono::milliseconds>(now_test - start_test).count();

        //传输定时
        while(late_time < 100)
        {
            now_test = chrono::steady_clock::now();
            late_time = chrono::duration_cast<chrono::milliseconds>(now_test - start_test).count();
        }

    }

    //发送线程关闭
    closesocket(execuation_fd);
    WSACleanup();

return 0;
}
//函数结束


VOID CALLBACK A_TimerProc(HWND, UINT, UINT_PTR, DWORD)
{
    if(Inside_MODEL_TIME != -1)
    {
        Inside_MODEL_TIME += 1;
    }
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:

        // 创建输入框 EDIT
        hEditInputText = CreateWindowExA(
            0, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
            20, 40, 250, 150,
            hwnd, (HMENU)1, NULL, NULL
        );

        Edit_LimitText(hEditInputText, 15000);

        //创建静态文本 STATIC
        hStaticStateShow = CreateWindowEx(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE,
            20, 10, 120, 20,
            hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        //创建测试文本
        test_time_Show = CreateWindowEx(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE,
            150, 10, 120, 20,
            hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        // 设置定时器，每100毫秒更新一次
        SetTimer(hwnd, 1, 100, NULL);

        // 定时器2号，每1毫秒更新一次
        SetTimer(NULL, 2, 1, A_TimerProc);
        
        return 0;

    case WM_TIMER:
        wchar_t communicate_buf[32];
        swprintf(communicate_buf, 32, L"通讯状态: %d连接", Self_Communicate);//把整数转换成字符串
        SetWindowTextW(hStaticStateShow, communicate_buf);

        wchar_t time_late_buf[32];
        swprintf(time_late_buf, 32, L"通讯耗时%dms", late_time);
        SetWindowTextW(test_time_Show, time_late_buf);
        break;
    
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }

return DefWindowProc(hwnd, msg, wParam, lParam);
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    //使用两线程，线程一负责界面显示、消息循环
    //线程二负责截图、发送图文数据、接收指令、接收文本
    Curr_Text.reserve(1024);

    //窗口建立
    const wchar_t CLASS_NAME[] = L"MyWinClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW, CLASS_NAME, L"界面显示",
        //WS_OVERLAPPEDWINDOW,//测试设置
        WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX,//原始窗口
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    
    //传输线程启动
    thread t1(Socket_thread);
    t1.detach();

    //程序消息循环
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) && Self_Working)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //结束程序
return 0;
}


//实验算法需为执行端设置虚拟机或者两台电脑，注意执行端和运算端传输的数据约为60mb/s
//为防止执行端意外关闭自己，执行端窗口无关闭键，要关闭程序时选择任务管理器或运算端关闭
