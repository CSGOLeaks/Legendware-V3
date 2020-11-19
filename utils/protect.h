#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WS2tcpip.h>
#include <WinSock2.h>
#include "md5.h"
#include "twofish.h"
#include "utils\imports.h"

#define crypt_str(s) s

__forceinline std::string get_key()
{
    std::string key;

    WSADATA wsaData;
    auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result)
        return key;

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* addr_result = nullptr;
    result = getaddrinfo(crypt_str("95.216.251.218"), crypt_str("1337"), &hints, &addr_result);

    if (result)
    {
        WSACleanup();
        return key;
    }

    auto connect_socket = INVALID_SOCKET;

    for (auto ptr = addr_result; ptr; ptr = ptr->ai_next)
    {
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (connect_socket == INVALID_SOCKET)
        {
            WSACleanup();
            return key;
        }

        result = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (result == SOCKET_ERROR)
        {
            closesocket(connect_socket);
            connect_socket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(addr_result);

    if (connect_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return key;
    }

    result = send(connect_socket, crypt_str("qewrwqertdfszvdzfdg"), 19, 0);

    if (result == SOCKET_ERROR)
    {
        closesocket(connect_socket);
        WSACleanup();
        return key;
    }

    result = shutdown(connect_socket, SD_SEND);

    if (result == SOCKET_ERROR)
    {
        closesocket(connect_socket);
        WSACleanup();
        return key;
    }

    auto buffer = new char[32];
    result = recv(connect_socket, buffer, 32, 0);

    for (auto i = 0; i < result; i++)
        key.push_back(buffer[i]);

    closesocket(connect_socket);
    WSACleanup();

    delete[] buffer;
    return md5(key);
}

__forceinline std::string get_data(const std::string& url)
{
    WSADATA wsaData;
    auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result)
        return crypt_str("");

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* addr_result = nullptr;
    result = getaddrinfo(crypt_str("95.216.251.218"), crypt_str("1337"), &hints, &addr_result);

    if (result)
    {
        WSACleanup();
        return crypt_str("");
    }

    auto connect_socket = INVALID_SOCKET;

    for (auto ptr = addr_result; ptr; ptr = ptr->ai_next)
    {
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (connect_socket == INVALID_SOCKET)
        {
            WSACleanup();
            return crypt_str("");
        }

        result = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (result == SOCKET_ERROR)
        {
            closesocket(connect_socket);
            connect_socket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(addr_result);

    if (connect_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return crypt_str("");
    }

    size_t size = 0;
    result = send(connect_socket, url.c_str(), url.size(), 0);

    if (result == SOCKET_ERROR)
    {
        closesocket(connect_socket);
        WSACleanup();
        return crypt_str("");
    }

    result = shutdown(connect_socket, SD_SEND);

    if (result == SOCKET_ERROR)
    {
        closesocket(connect_socket);
        WSACleanup();
        return crypt_str("");
    }

    auto buffer = new char[65536];
    result = recv(connect_socket, buffer, 65536, 0);

    std::vector <uint8_t> crypted_data;

    for (auto i = 0; i < result; i++)
        crypted_data.push_back(buffer[i]);

    closesocket(connect_socket);
    WSACleanup();

    delete[] buffer;
    return decrypt_string(crypted_data.data(), get_key(), result / 16);
}

__forceinline std::string get_hwid()
{
    auto serial_path = (std::string)getenv(crypt_str("SystemDrive")) + crypt_str("/");

    DWORD serial_number = 0;
    IFH(GetVolumeInformation)(serial_path.c_str(), NULL, NULL, &serial_number, NULL, NULL, NULL, NULL);

    char serial_str[10];
    _ultoa(serial_number, serial_str, 10);

    return md5(serial_str);
}