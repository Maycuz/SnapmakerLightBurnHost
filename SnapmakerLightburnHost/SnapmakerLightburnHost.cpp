// SnapmakerLightburnHost.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define STB_IMAGE_IMPLEMENTATION

#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <format>
#include <fstream>
#include <filesystem>
#include <time.h>

#include "softcam.h"
#include "stb_image.h"

#include "curl/curl.h"

using namespace std;

std::filesystem::path targetFile;

string GetTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "[%d-%m %X] ");
    return ss.str();
}

// Source: https://cplusplus.com/forum/general/46477/
// callback function writes data to a std::ostream
static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
    if (userp)
    {
        std::ostream& os = *static_cast<std::ostream*>(userp);
        std::streamsize len = size * nmemb;
        if (os.write(static_cast<char*>(buf), len))
            return len;
    }

    return 0;
}

CURLcode getImageFromSnapmaker(string ipAddress)
{
    CURL* cameraPos;
    cameraPos = curl_easy_init();

    CURLcode result;

    auto targetUrlRequest = std::format("http://{}:8080/api/request_capture_photo?index=0&x=232&y=178&z=290&feedRate=3000&photoQuality=0", ipAddress);
    
    std::cout << GetTimeStamp() << "\t-> cURL: Sending request to camera... ";
    
    curl_easy_setopt(cameraPos, CURLOPT_URL, targetUrlRequest.c_str());
    curl_easy_setopt(cameraPos, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(cameraPos, CURLOPT_WRITEFUNCTION, NULL);
    result = curl_easy_perform(cameraPos);
    std::cout << std::endl;

    if (result == CURLE_OK)
    {
        std::ofstream ofs(targetFile.string(), std::ostream::binary);

        CURL* cameraImage;
        cameraImage = curl_easy_init();

        string targetUrlGetImage = std::format("http://{}:8080/api/get_camera_image?index=0", ipAddress);

        std::cout << GetTimeStamp() << "\t-> cURL: Retrieving image... ";
        curl_easy_setopt(cameraImage, CURLOPT_URL, targetUrlGetImage.c_str());
        curl_easy_setopt(cameraImage, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(cameraImage, CURLOPT_FILE, &ofs);
        curl_easy_setopt(cameraImage, CURLOPT_WRITEFUNCTION, &data_write);
        result = curl_easy_perform(cameraImage);
        std::cout << std::endl;

        curl_easy_cleanup(cameraImage);

        ofs.close();
    }

    curl_easy_cleanup(cameraPos);

    if (result != CURLE_OK)
    {
        std::cout << GetTimeStamp() <<  "\tcURL: Error retrieving image" << std::endl;
    }

    return result;
}

int main(int argc, char* argv[])
{
    const char enterASCIIChar = 13;
    
    if (argc <= 1)
    {
        std::cout << "Please pass an IP address as the first argument." << std::endl;
        exit(1);
    }

    auto ipAddress = string(argv[1]);

    targetFile = std::filesystem::current_path() / "latest.jpg";
    scCamera cam = scCreateCamera(1024, 1280, 60);

    std::cout << GetTimeStamp() << std::format("Virtual camera has started @ {}", ipAddress) << std::endl;
    std::cout << GetTimeStamp() << "Press ENTER to request a new image from base position (warning: will move bed & laser!)" << std::endl;

    int width, height, comp;
    auto image = stbi_load(targetFile.string().c_str(), &width, &height, &comp, 0);

    for (;;)
    {
        scSendFrame(cam, image);
        
        if (_kbhit())
        {
            auto pressedChar = _getch();

            if (pressedChar == enterASCIIChar)
            {
                std::cout << GetTimeStamp() << "New image requested, please wait..." << std::endl;

                if (CURLE_OK == getImageFromSnapmaker(ipAddress))
                {
                    image = stbi_load(targetFile.string().c_str(), &width, &height, &comp, 0);

                    if (stbi_failure_reason() && (string("bad png sig").compare(stbi_failure_reason()) != 0))
                        std::cout << GetTimeStamp() << "\t-> STBI: " << stbi_failure_reason() << std::endl;

                    std::cout << GetTimeStamp() << "Image sent to virtual camera. Press ENTER to request a new image." << std::endl;
                }
            }
        }
    }
}