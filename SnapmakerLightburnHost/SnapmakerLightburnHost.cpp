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
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

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

size_t json_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string& data = *static_cast<std::string*>(userdata);
    size_t len = size * nmemb;

    data.append(ptr, len);
    return len;
}

bool IsStatusOk(string jsonResponse)
{
    try
    {
        json data = json::parse(jsonResponse);
        return data["status"];
    }
    catch (std::exception& e)
    {
        std::cout << GetTimeStamp() << "\t-> Error parsing response: " << e.what() << std::endl;
    }
}

void ParseThicknessInfo(string jsonResponse)
{
    try
    {
        json data = json::parse(jsonResponse);
        auto thickness = (double)data["thickness"];
        std::cout << GetTimeStamp() << "\t-> Measured material thickness: " << thickness << "mm" << std::endl;
    }
    catch(std::exception& e)
    {
        std::cout << GetTimeStamp() << "\t-> Error parsing response: " << e.what() << std::endl;
    }
}

CURLcode GetMaterialThicknessFromSnapmaker(string ipAddress)
{
    std::string printerResponse;

    CURL* matThickness;
    matThickness = curl_easy_init();

    CURLcode result;

    auto targetUrlRequest = std::format("http://{}:8080/api/request_Laser_Material_Thickness?x=232&y=178&z=290&feedRate=3000", ipAddress);

    std::cout << GetTimeStamp() << "\t-> cURL: Sending request to laser... ";

    curl_easy_setopt(matThickness, CURLOPT_URL, targetUrlRequest.c_str());
    curl_easy_setopt(matThickness, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(matThickness, CURLOPT_WRITEDATA, &printerResponse);
    curl_easy_setopt(matThickness, CURLOPT_WRITEFUNCTION, json_callback);
    result = curl_easy_perform(matThickness);
    std::cout << std::endl;

    curl_easy_cleanup(matThickness);

    if (result == CURLE_OK && IsStatusOk(printerResponse))
    {
        ParseThicknessInfo(printerResponse);
    }
    else
    {
        std::cout << GetTimeStamp() << "\tcURL: No valid response from Snapmaker. Make sure the material is positioned under the laser." << std::endl;
    }

    return result;
}

CURLcode GetImageFromSnapmaker(string ipAddress)
{
    std::string printerResponse;

    CURL* cameraPos;
    cameraPos = curl_easy_init();

    CURLcode result;

    auto targetUrlRequest = std::format("http://{}:8080/api/request_capture_photo?index=0&x=232&y=178&z=290&feedRate=3000&photoQuality=0", ipAddress);
    
    std::cout << GetTimeStamp() << "\t-> cURL: Sending request to camera... ";
    
    curl_easy_setopt(cameraPos, CURLOPT_URL, targetUrlRequest.c_str());
    curl_easy_setopt(cameraPos, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(cameraPos, CURLOPT_WRITEDATA, &printerResponse);
    curl_easy_setopt(cameraPos, CURLOPT_WRITEFUNCTION, json_callback);
    result = curl_easy_perform(cameraPos);
    std::cout << std::endl;

    curl_easy_cleanup(cameraPos);

    if (result == CURLE_OK && IsStatusOk(printerResponse))
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

    if (result != CURLE_OK)
    {
        std::cout << GetTimeStamp() <<  "\tcURL: No valid response from Snapmaker" << std::endl;
    }

    return result;
}

int main(int argc, char* argv[])
{
    const char enterASCIIChar = 13;
    const char spaceASCIIChar = 32;

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
    std::cout << GetTimeStamp() << "Press SPACE to request material thickness from base position (warning: will move bed & laser!)" << std::endl;

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

                if (CURLE_OK == GetImageFromSnapmaker(ipAddress))
                {
                    image = stbi_load(targetFile.string().c_str(), &width, &height, &comp, 0);

                    if (stbi_failure_reason() && (string("bad png sig").compare(stbi_failure_reason()) != 0))
                        std::cout << GetTimeStamp() << "\t-> STBI: " << stbi_failure_reason() << std::endl;

                    std::cout << GetTimeStamp() << "Image sent to virtual camera. Press ENTER to request a new image." << std::endl;
                }
                else
                {
                    std::cout << GetTimeStamp() << "Failed to retrieve image. Press ENTER to request a new image." << std::endl;
                }
            }

            if (pressedChar == spaceASCIIChar)
            {
                std::cout << GetTimeStamp() << "Material thickness requested, please wait..." << std::endl;

                if (CURLE_OK == GetMaterialThicknessFromSnapmaker(ipAddress))
                {
                    std::cout << GetTimeStamp() << "Material thickness received. Press SPACE to request again." << std::endl;
                }
                else
                {
                    std::cout << GetTimeStamp() << "Failed to retrieve material thickness. Press SPACE to request again." << std::endl;
                }
            }
        }
    }
}