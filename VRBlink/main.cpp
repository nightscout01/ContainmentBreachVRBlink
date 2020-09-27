// Edited HTC Vive Pro Eye SAnipal Sample Code
// Edited By Maurice Montag 2020
// Presses the DirectX Blink Spacebar keycode when a user blinks in the VR headset
// used for SCP:CB in VR

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <Windows.h>
#include "WinUser.h"
#include <thread>
#include "SRanipal.h"
#include "SRanipal_Eye.h"
#include "SRanipal_Lip.h"
#include "SRanipal_Enums.h"
#pragma comment (lib, "SRanipal.lib")
using namespace ViveSR;

std::thread *t = nullptr;
bool EnableEye = false;
bool EnableLip = false;
bool looping = false;

UINT PressKeyScan(WORD scanCode);
UINT ReleaseKeyScan(WORD scanCode);

void streaming() {
	
	ViveSR::anipal::Eye::EyeData eye_data;
	int result = ViveSR::Error::WORK;
	while (looping) {
		bool pressed = false;
		if (EnableEye) {
			int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);
			if (result == ViveSR::Error::WORK) {
				float blink = eye_data.verbose_data.left.eye_openness;
				//printf("%f\n",blink);
				if (blink < 0.5f) {	// press space on left eye blink (for testing, need to be able to see if we have blinked)
					PressKeyScan(0x39);
				}
				else{
					ReleaseKeyScan(0x39);
				}
				
			}
		}
	}
}

int main() {
	printf("SRanipal Sample\n\nPlease refer the below hotkey list to try apis.\n");
	printf("[`] Exit this program.\n");
	printf("[0] Release all anipal engines.\n");
	printf("[1] Initial Eye engine\n");
    printf("[2] Initial Lip engine\n");
	printf("[3] Launch a thread to query data.\n");
	printf("[4] Stop the thread.\n");
	
	if(!ViveSR::anipal::Eye::IsViveProEye()){
		printf("\n\nthis device does not have eye-tracker, please change your HMD\n");
		return 0;
	}
	char str = 0;
	int error, handle = NULL;
	while (true) {

		if (str != '\n' && str != EOF) { printf("\nwait for key event :"); }
		str = getchar();
		if (str == '`') break;
		else if (str == '0') {
			error = ViveSR::anipal::Release(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE);
			printf("Successfully release all anipal engines.\n");
			EnableEye = false;
		}
		else if (str == '1') {
			error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, NULL);
			if (error == ViveSR::Error::WORK) { EnableEye = true; printf("Successfully initialize Eye engine.\n"); }
			else if (error == ViveSR::Error::RUNTIME_NOT_FOUND) printf("please follows SRanipal SDK guide to install SR_Runtime first\n");
			else printf("Fail to initialize Eye engine. please refer the code %d of ViveSR::Error.\n", error);
		}
        else if (str == '2') {
			error = ViveSR::anipal::Initial(ViveSR::anipal::Lip::ANIPAL_TYPE_LIP, NULL);
			if (error == ViveSR::Error::WORK) { EnableLip = true; printf("Successfully initialize Lip engine.\n"); }
			else if (error == ViveSR::Error::RUNTIME_NOT_FOUND) printf("please follows SRanipal SDK guide to install SR_Runtime first\n");
			else printf("Fail to initialize Lip engine. please refer the code %d of ViveSR::Error.\n", error);
		}
        else if (str == '3') {
            if (t == nullptr) {
                t = new std::thread(streaming);
                if (t)   looping = true;
            }
        }
        else if (str == '4') {
            looping = false;
            if (t == nullptr) continue;
            t->join();
            delete t;
            t = nullptr;
        }
	}
	ViveSR::anipal::Release(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE);
    ViveSR::anipal::Release(ViveSR::anipal::Lip::ANIPAL_TYPE_LIP);
}

UINT PressKeyScan(WORD scanCode)
{
    INPUT input[1] = { 0 };
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = NULL;
    input[0].ki.wScan = scanCode;
    input[0].ki.dwFlags = KEYEVENTF_SCANCODE;

    UINT ret = SendInput(1, input, sizeof(INPUT));

    return ret;
}

UINT ReleaseKeyScan(WORD scanCode)
{
    INPUT input[1] = { 0 };
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = NULL;
    input[0].ki.wScan = scanCode;
    input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;


    UINT ret = SendInput(1, input, sizeof(INPUT));

    return ret;
}