// SnapmakerLightburnHost.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "softcam.h"
#include "stb_image.h"

#include "curl/curl.h"

using namespace std;

//const string ipAddress = "192.168.178.89";
//
//void getImageFromSnapmaker(CURL *curlInstance, string ipAddress)
//{
//
//}

int main()
{
    //CURL* curl;
    //CURLcode result;
    //string readBuffer;

    FILE* fileStream;
    fopen_s(&fileStream, "C:\\Users\\trist\\Desktop\\example.jpg", "rb");

    int width, height, comp;

    auto image = stbi_load_from_file(fileStream, &width, &height, &comp, 0);
    
    scCamera cam = scCreateCamera(width, height, 1);

    std::cout << "Virtual camera has started" << std::endl;

    for (;;)
    {
        scSendFrame(cam, image);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
