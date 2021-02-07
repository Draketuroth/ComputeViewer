#ifndef PRINTHELPER_H
#define PRINTHELPER_H

#include <stdexcept>
#include <string>
#include <winerror.h>
#include <stdio.h>
#include <wchar.h>

inline std::string HrToString(HRESULT hr)
{
    char buf[64] = {};
    sprintf_s(buf, "HRESULT of 0x%08X", static_cast<unsigned int>(hr));
    return std::string(buf);
}

inline void PrintStatus(const char* status, const char* msg) 
{
    printf("[%s] %s\n", status, msg);
}

inline bool ReportStatus(HRESULT hr, const char* msg)
{
    bool status = hr == S_OK;
    status ? PrintStatus("OK", msg) : PrintStatus("FAIL", msg);
    return status;
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), _hr(hr) {}
    HRESULT error() const { return _hr; }
private:
    const HRESULT _hr;
};

#endif