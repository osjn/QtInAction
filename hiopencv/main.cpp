#include <QCoreApplication>
#include <opencv2/opencv.hpp>
//#include <windows.h>
//#include <stdlib.h>

using namespace std;
string toString(const wstring& ws)
{
//    LPCWSTR pwszSrc = ws.c_str();
//    int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
//    if (nLen == 0)
//        return std::string("");

//    char* pszDst = new char[nLen];
//    if (!pszDst)
//        return std::string("");

//    WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
//    std::string to(pszDst);
//    delete[] pszDst;
//    pszDst = NULL;

//    return to;

    size_t length = 2 * ws.length() + 1;
    setlocale(LC_CTYPE, "");
    char *p = new char[length];
    size_t converted = 0;
    wcstombs_s(&converted, p, length, ws.c_str(), length);
    string to(p);
    delete[] p;

    return to;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    using namespace cv;
    Mat image = imread("background.png");
    imshow(toString(L"中文不乱码"), image);
//    cout << toString(L"你好，OpenCV") << endl;

//    return 0;
    return a.exec();
}
