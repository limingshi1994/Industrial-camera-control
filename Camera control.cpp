#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/video.hpp"
#include <chrono>
#include <thread>
#include <conio.h>

using namespace BGAPI2;
using namespace std;
using namespace cv;
using namespace this_thread;
using namespace chrono;

int main()
{
	cv::VideoWriter cvVideoCreator;                    //create opencv video creator
	cv::Mat openCvImage;                               //create an opencv image
	cv::Mat I;
	cv::String videoFileName = "Real_time_feed.avi";   //Define Video Filename
	cv::Size frameSize = cv::Size(300, 300);         //Define video frame size (frame width * height)
	cvVideoCreator.open(videoFileName, CV_FOURCC('D', 'I', 'V', 'X'), 20, frameSize, true);

    //DECLARATIONS OF VARIABLES
	BGAPI2::ImageProcessor * imgProcessor = NULL;

    BGAPI2::SystemList *systemList = NULL;
    BGAPI2::System * pSystem = NULL;
    BGAPI2::String sSystemID;

    BGAPI2::InterfaceList *interfaceList = NULL;
    BGAPI2::Interface * pInterface = NULL;
    BGAPI2::String sInterfaceID;

    BGAPI2::DeviceList *deviceList = NULL;
    BGAPI2::Device * pDevice = NULL;
    BGAPI2::String sDeviceID;

    BGAPI2::DataStreamList *datastreamList = NULL;
    BGAPI2::DataStream * pDataStream = NULL;
    BGAPI2::String sDataStreamID;

    BGAPI2::BufferList *bufferList = NULL;
    BGAPI2::Buffer * pBuffer = NULL;
    BGAPI2::String sBufferID;
    int returncode = 0;

	//Load image processor
	try
	{
		imgProcessor = new BGAPI2::ImageProcessor();
		std::cout << "ImageProcessor version:    " << imgProcessor->GetVersion() << std::endl;
		if (imgProcessor->GetNodeList()->GetNodePresent("DemosaicingMethod") == true)
		{
			imgProcessor->GetNodeList()->GetNode("DemosaicingMethod")->SetString("NearestNeighbor"); // NearestNeighbor, Bilinear3x3, Baumer5x5
			std::cout << "    Demosaicing method:    " << imgProcessor->GetNodeList()->GetNode("DemosaicingMethod")->GetString() << std::endl;
		}
		std::cout << std::endl;
	}
	catch (BGAPI2::Exceptions::IException& ex)
	{
		returncode = (returncode == 0) ? 1 : returncode;
		std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
		std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
		std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
	}

    std::cout << "SYSTEM LIST" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    //COUNTING AVAILABLE SYSTEMS (TL producers)
    try
    {
        systemList = SystemList::GetInstance();
        systemList->Refresh();
        std::cout << "5.1.2   Detected systems:  " << systemList->size() << std::endl;

        //SYSTEM DEVICE INFORMATION
        for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
        {
            std::cout << "  5.2.1   System Name:     " << sysIterator->second->GetFileName() << std::endl;
            std::cout << "          System Type:     " << sysIterator->second->GetTLType() << std::endl;
            std::cout << "          System Version:  " << sysIterator->second->GetVersion() << std::endl;
            std::cout << "          System PathName: " << sysIterator->second->GetPathName() << std::endl << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //OPEN THE SECOND SYSTEM IN THE LIST WITH A CAMERA CONNECTED (if cognex is installed in this pc)
    try
    {
        for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
        {
			if (sysIterator == systemList->begin())
			{
				continue;
			}
			std::cout << "SYSTEM" << std::endl;
            std::cout << "######" << std::endl << std::endl;

            try
            {
                sysIterator->second->Open();
                std::cout << "5.1.3   Open next system " << std::endl;
                std::cout << "  5.2.1   System Name:     " << sysIterator->second->GetFileName() << std::endl;
                std::cout << "          System Type:     " << sysIterator->second->GetTLType() << std::endl;
                std::cout << "          System Version:  " << sysIterator->second->GetVersion() << std::endl;
                std::cout << "          System PathName: " << sysIterator->second->GetPathName() << std::endl << std::endl;
                sSystemID = sysIterator->first;
                std::cout << "        Opened system - NodeList Information " << std::endl;
                std::cout << "          GenTL Version:   " << sysIterator->second->GetNode("GenTLVersionMajor")->GetValue() << "." << sysIterator->second->GetNode("GenTLVersionMinor")->GetValue() << std::endl << std::endl;

                std::cout << "INTERFACE LIST" << std::endl;
                std::cout << "##############" << std::endl << std::endl;

                try
                {
                    interfaceList = sysIterator->second->GetInterfaces();
                    //COUNT AVAILABLE INTERFACES
                    interfaceList->Refresh(100); // timeout of 100 msec
                    std::cout << "5.1.4   Detected interfaces: " << interfaceList->size() << std::endl;

                    //INTERFACE INFORMATION
                    for (InterfaceList::iterator ifIterator = interfaceList->begin(); ifIterator != interfaceList->end(); ifIterator++)
                    {
                        std::cout << "  5.2.2   Interface ID:      " << ifIterator->first << std::endl;
                        std::cout << "          Interface Type:    " << ifIterator->second->GetTLType() << std::endl;
                        std::cout << "          Interface Name:    " << ifIterator->second->GetDisplayName() << std::endl << std::endl;
                    }
                }
                catch (BGAPI2::Exceptions::IException& ex)
                {
                    returncode = (returncode == 0) ? 1 : returncode;
                    std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
                    std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
                    std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
                }


                std::cout << "INTERFACE" << std::endl;
                std::cout << "#########" << std::endl << std::endl;

                //OPEN THE NEXT INTERFACE IN THE LIST
                try
                {
                    for (InterfaceList::iterator ifIterator = interfaceList->begin(); ifIterator != interfaceList->end(); ifIterator++)
                    {
                        try
                        {
                            std::cout << "5.1.5   Open interface " << std::endl;
                            std::cout << "  5.2.2   Interface ID:      " << ifIterator->first << std::endl;
                            std::cout << "          Interface Type:    " << ifIterator->second->GetTLType() << std::endl;
                            std::cout << "          Interface Name:    " << ifIterator->second->GetDisplayName() << std::endl;
                            ifIterator->second->Open();
                            //search for any camera is connetced to this interface
                            deviceList = ifIterator->second->GetDevices();
                            deviceList->Refresh(100);
                            if(deviceList->size() == 0)
                            {
                                std::cout << "5.1.13   Close interface (" << deviceList->size() << " cameras found) " << std::endl << std::endl;
                                ifIterator->second->Close();
                            }
                            else
                            {
                                sInterfaceID = ifIterator->first;
                                std::cout << "   " << std::endl;
                                std::cout << "        Opened interface - NodeList Information " << std::endl;
                                if( ifIterator->second->GetTLType() == "GEV" )
                                {
                                    std::cout << "          GevInterfaceSubnetIPAddress: " << ifIterator->second->GetNode("GevInterfaceSubnetIPAddress")->GetValue() << std::endl;
                                    std::cout << "          GevInterfaceSubnetMask:      " << ifIterator->second->GetNode("GevInterfaceSubnetMask")->GetValue() << std::endl;
                                }
                                if( ifIterator->second->GetTLType() == "U3V" )
                                {
                                    //std::cout << "          NodeListCount:     " << ifIterator->second->GetNodeList()->GetNodeCount() << std::endl;
                                }
                                std::cout << "  " << std::endl;
                                break;
                            }
                        }
                        catch (BGAPI2::Exceptions::ResourceInUseException& ex)
                        {
                            returncode = (returncode == 0) ? 1 : returncode;
                            std::cout << " Interface " << ifIterator->first << " already opened " << std::endl;
                            std::cout << " ResourceInUseException: " << ex.GetErrorDescription() << std::endl;
                        }
                    }
                }
                catch (BGAPI2::Exceptions::IException& ex)
                {
                    returncode = (returncode == 0) ? 1 : returncode;
                    std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
                    std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
                    std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
                }


                //if a camera is connected to the system interface then leave the system loop
                if(sInterfaceID != "")
                {
                        break;
                }
            }
            catch (BGAPI2::Exceptions::ResourceInUseException& ex)
            {
                returncode = (returncode == 0) ? 1 : returncode;
                std::cout << " System " << sysIterator->first << " already opened " << std::endl;
                std::cout << " ResourceInUseException: " << ex.GetErrorDescription() << std::endl;
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    if ( sSystemID == "" )
    {
        std::cout << " No System found " << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pSystem = (*systemList)[sSystemID];
    }


    if (sInterfaceID == "")
    {
        std::cout << " No camera found " << sInterfaceID << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pInterface = (*interfaceList)[sInterfaceID];
    }


    std::cout << "DEVICE LIST" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    try
    {
        //COUNTING AVAILABLE CAMERAS
        deviceList = pInterface->GetDevices();
        deviceList->Refresh(100);
        std::cout << "5.1.6   Detected devices:         " << deviceList->size() << std::endl;

        //DEVICE INFORMATION BEFORE OPENING
        for (DeviceList::iterator devIterator = deviceList->begin(); devIterator != deviceList->end(); devIterator++)
        {
            std::cout << "  5.2.3   Device DeviceID:        " << devIterator->first << std::endl;
            std::cout << "          Device Model:           " << devIterator->second->GetModel() << std::endl;
            std::cout << "          Device SerialNumber:    " << devIterator->second->GetSerialNumber() << std::endl;
            std::cout << "          Device Vendor:          " << devIterator->second->GetVendor() << std::endl;
            std::cout << "          Device TLType:          " << devIterator->second->GetTLType() << std::endl;
            std::cout << "          Device AccessStatus:    " << devIterator->second->GetAccessStatus() << std::endl;
            std::cout << "          Device UserID:          " << devIterator->second->GetDisplayName() << std::endl << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }


    std::cout << "DEVICE" << std::endl;
    std::cout << "######" << std::endl << std::endl;

    //OPEN THE FIRST CAMERA IN THE LIST
    try
    {
        for (DeviceList::iterator devIterator = deviceList->begin(); devIterator != deviceList->end(); devIterator++)
        {
            try
            {
                std::cout << "5.1.7   Open first device " << std::endl;
                std::cout << "          Device DeviceID:        " << devIterator->first << std::endl;
                std::cout << "          Device Model:           " << devIterator->second->GetModel() << std::endl;
                std::cout << "          Device SerialNumber:    " << devIterator->second->GetSerialNumber() << std::endl;
                std::cout << "          Device Vendor:          " << devIterator->second->GetVendor() << std::endl;
                std::cout << "          Device TLType:          " << devIterator->second->GetTLType() << std::endl;
                std::cout << "          Device AccessStatus:    " << devIterator->second->GetAccessStatus() << std::endl;
                std::cout << "          Device UserID:          " << devIterator->second->GetDisplayName() << std::endl << std::endl;
                devIterator->second->Open();
                sDeviceID = devIterator->first;
                std::cout << "        Opened device - RemoteNodeList Information " << std::endl;
                std::cout << "          Device AccessStatus:    " << devIterator->second->GetAccessStatus() << std::endl;

                //SERIAL NUMBER
                if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceSerialNumber"))
                    std::cout << "          DeviceSerialNumber:     " << devIterator->second->GetRemoteNode("DeviceSerialNumber")->GetValue() << std::endl;
                else if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceID"))
                    std::cout << "          DeviceID (SN):          " << devIterator->second->GetRemoteNode("DeviceID")->GetValue() << std::endl;
                else
                    std::cout << "          SerialNumber:           Not Available "  << std::endl;

                //DISPLAY DEVICEMANUFACTURERINFO
                if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceManufacturerInfo"))
                    std::cout << "          DeviceManufacturerInfo: " << devIterator->second->GetRemoteNode("DeviceManufacturerInfo")->GetValue() << std::endl;

                //DISPLAY DEVICEFIRMWAREVERSION OR DEVICEVERSION
                if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceFirmwareVersion"))
                    std::cout << "          DeviceFirmwareVersion:  " << devIterator->second->GetRemoteNode("DeviceFirmwareVersion")->GetValue() << std::endl;
                else if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceVersion"))
                    std::cout << "          DeviceVersion:          " << devIterator->second->GetRemoteNode("DeviceVersion")->GetValue() << std::endl;
                else
                    std::cout << "          DeviceVersion:          Not Available "  << std::endl;

                if(devIterator->second->GetTLType() == "GEV")
                {
                    std::cout << "          GevCCP:                 " << devIterator->second->GetRemoteNode("GevCCP")->GetValue() << std::endl;
                    std::cout << "          GevCurrentIPAddress:    " << devIterator->second->GetRemoteNode("GevCurrentIPAddress")->GetValue() << std::endl;
                    std::cout << "          GevCurrentSubnetMask:   " << devIterator->second->GetRemoteNode("GevCurrentSubnetMask")->GetValue() << std::endl;
                }
                std::cout << "  " << std::endl;
                break;
            }
            catch (BGAPI2::Exceptions::ResourceInUseException& ex)
            {
                returncode = (returncode == 0) ? 1 : returncode;
                std::cout << " Device  " << devIterator->first << " already opened " << std::endl;
                std::cout << " ResourceInUseException: " << ex.GetErrorDescription() << std::endl;
            }
            catch (BGAPI2::Exceptions::AccessDeniedException& ex)
            {
                returncode = (returncode == 0) ? 1 : returncode;
                std::cout << " Device  " << devIterator->first << " already opened " << std::endl;
                std::cout << " AccessDeniedException " << ex.GetErrorDescription() << std::endl;
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    if (sDeviceID == "")
    {
        std::cout << " No Device found " << sDeviceID << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pDevice = (*deviceList)[sDeviceID];
    }


    std::cout << "DEVICE PARAMETER SETUP" << std::endl;
    std::cout << "######################" << std::endl << std::endl;

	try
	{
		//DEVICE PARAMETER SETUP VCXG
		pDevice->GetRemoteNode("AcquisitionStop")->Execute();
		std::cout << "          AcquisitionStop:          Done" << std::endl;
		std::cout << std::endl;

		// DIGITAL IO CONTROL
		//=====================
		/*
		pDevice->GetRemoteNode("LineSelector")->SetString("Line0");
		std::cout << "          LineSelector:             " << pDevice->GetRemoteNode("LineSelector")->GetString() << std::endl;
		std::cout << "          LineMode:                 " << pDevice->GetRemoteNode("LineMode")->GetString() << std::endl; // Line0 LineMode = Input only
		pDevice->GetRemoteNode("LineInverter")->SetBool(false);
		std::cout << "          LineInverter:             " << pDevice->GetRemoteNode("LineInverter")->GetBool() << std::endl;
		std::cout << "          LineStatus:               " << pDevice->GetRemoteNode("LineStatus")->GetBool() << std::endl;
		pDevice->GetRemoteNode("LineDebouncerHighTimeAbs")->SetDouble(1.0); // in usec
		std::cout << "          LineDebouncerHighTimeAbs: " << pDevice->GetRemoteNode("LineDebouncerHighTimeAbs")->GetDouble() << std::endl;
		pDevice->GetRemoteNode("LineDebouncerLowTimeAbs")->SetDouble(1.0); // in usec
		std::cout << "          LineDebouncerLowTimeAbs:  " << pDevice->GetRemoteNode("LineDebouncerLowTimeAbs")->GetDouble() << std::endl;
		std::cout << std::endl;
		*/

		/*

		try
		{
		//SET TRIGGER MODE OFF (FreeRun)
		pDevice->GetRemoteNode("TriggerMode")->SetString("Off");
		std::cout << "         TriggerMode:             " << pDevice->GetRemoteNode("TriggerMode")->GetValue() << std::endl;
		std::cout << std::endl;
		}
		catch (BGAPI2::Exceptions::IException& ex)
		{
		returncode = (returncode == 0) ? 1 : returncode;
		std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
		std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
		std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
		}



		*/

		// ACQUISITION CONTROL
		//=====================
		pDevice->GetRemoteNode("TriggerMode")->SetString("On");
		std::cout << "          TriggerMode:              " << pDevice->GetRemoteNode("TriggerMode")->GetString() << std::endl;
		pDevice->GetRemoteNode("TriggerSource")->SetString("Software");
		std::cout << "          TriggerSource:            " << pDevice->GetRemoteNode("TriggerSource")->GetString() << std::endl;
		//pDevice->GetRemoteNode("TriggerActivation")->SetString("RisingEdge"); // RisingEdge or FallingEdge
		//std::cout << "          TriggerActivation:        " << pDevice->GetRemoteNode("TriggerActivation")->GetString() << std::endl;
		pDevice->GetRemoteNode("TriggerDelay")->SetDouble(0.0); // delay in usec
		std::cout << "          TriggerDelay:             " << pDevice->GetRemoteNode("TriggerDelay")->GetDouble() << std::endl;

		// INPUT EXPOSURETIME
		double expstime;
		std::cout << "please enter the designated exposure time : " << std::endl;
		std::cin >> expstime;

		pDevice->GetRemoteNode("ExposureTime")->SetDouble(expstime);
		std::cout << "          ExposureTime     : " << pDevice->GetRemoteNode("ExposureTime")->GetDouble() << std::endl;
		//mExposureTime = pDevice->GetRemoteNode("ExposureTime")->GetDouble();
		std::cout << std::endl;
	}
	catch (BGAPI2::Exceptions::IException& ex)
	{
		returncode = 0 == returncode ? 1 : returncode;
		std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
		std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
		std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
	}

    std::cout << "DATA STREAM LIST" << std::endl;
    std::cout << "################" << std::endl << std::endl;

    try
    {
        //COUNTING AVAILABLE DATASTREAMS
        datastreamList = pDevice->GetDataStreams();
        datastreamList->Refresh();
        std::cout << "5.1.8   Detected datastreams:     " << datastreamList->size() << std::endl;

        //DATASTREAM INFORMATION BEFORE OPENING
        for (DataStreamList::iterator dstIterator = datastreamList->begin(); dstIterator != datastreamList->end(); dstIterator++)
        {
            std::cout << "  5.2.4   DataStream ID:          " << dstIterator->first << std::endl << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }


    std::cout << "DATA STREAM" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    //OPEN THE FIRST DATASTREAM IN THE LIST
    try
    {
        for (DataStreamList::iterator dstIterator = datastreamList->begin(); dstIterator != datastreamList->end(); dstIterator++)
        {
            std::cout << "5.1.9   Open first datastream " << std::endl;
            std::cout << "          DataStream ID:          " << dstIterator->first << std::endl << std::endl;
            dstIterator->second->Open();
            sDataStreamID = dstIterator->first;
            std::cout << "        Opened datastream - NodeList Information " << std::endl;
            std::cout << "          StreamAnnounceBufferMinimum:  " << dstIterator->second->GetNode("StreamAnnounceBufferMinimum")->GetValue() << std::endl;
            if( dstIterator->second->GetTLType() == "GEV" )
            {
                std::cout << "          StreamDriverModel:            " << dstIterator->second->GetNode("StreamDriverModel")->GetValue() << std::endl;
            }
            std::cout << "  " << std::endl;
            break;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    if (sDataStreamID == "")
    {
        std::cout << " No DataStream found " << sDataStreamID << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        pDevice->Close();
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pDataStream = (*datastreamList)[sDataStreamID];
    }


    std::cout << "BUFFER LIST" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    try
    {
        //BufferList
        bufferList = pDataStream->GetBufferList();

        // 1 buffers using internal buffer mode
        for(int i=0; i<1; i++)
        {
            pBuffer = new BGAPI2::Buffer();
            bufferList->Add(pBuffer);
        }
        std::cout << "5.1.10   Announced buffers:       " << bufferList->GetAnnouncedCount() << " using " << pBuffer->GetMemSize() * bufferList->GetAnnouncedCount() << " [bytes]" << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    try
    {
        for (BufferList::iterator bufIterator = bufferList->begin(); bufIterator != bufferList->end(); bufIterator++)
        {
            bufIterator->second->QueueBuffer();
        }
        std::cout << "5.1.11   Queued buffers:          " << bufferList->GetQueuedCount() << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << " " << std::endl;

    std::cout << "CAMERA START" << std::endl;
    std::cout << "############" << std::endl << std::endl;

    //START DataStream acquisition
    try
    {
        pDataStream->StartAcquisitionContinuous();
        std::cout << "5.1.12   DataStream started " << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //START CAMERA
    try
    {
        std::cout << "5.1.12   " << pDevice->GetModel() << " started " << std::endl;
        pDevice->GetRemoteNode("AcquisitionStart")->Execute();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

	//CAPTURE IMAGES
    std::cout << " " << std::endl;
    std::cout << "CAPTURE IMAGES BY IMAGE POLLING / TRIGGER / TIME-LAPSE" << std::endl;
    std::cout << "######################################################" << std::endl << std::endl;


    BGAPI2::Buffer * pBufferFilled = NULL;
	try
	{

		BGAPI2::Image * pImage = NULL;
		//for (int i = 0; i < 10; i++)                                   //fixed number
		while(pDataStream->GetIsGrabbing())                              //Stream
		//while(1)                                                       //continuously
		{

			//SOFTWARE TRIGGER
			//std::cout << " Execute TriggerSoftware " << i + 1 << std::endl;
			//pDevice->GetRemoteNode("TriggerSoftware")->Execute();
				pDevice->GetRemoteNode("TriggerSoftware")->Execute();

				pBufferFilled = pDataStream->GetFilledBuffer(1000); //timeout 1000 msec
				if (pBufferFilled == NULL)
				{
					std::cout << "Error: Buffer Timeout after 1000 msec" << std::endl;
				}
				else if (pBufferFilled->GetIsIncomplete() == true)
				{
					std::cout << "Error: Image is incomplete" << std::endl;
					// queue buffer again
					pBufferFilled->QueueBuffer();
				}
				else
				{
					std::cout << " Image " << std::setw(5) << pBufferFilled->GetFrameID();
					std::cout << " received in memory address " << std::hex << pBufferFilled->GetMemPtr() << std::dec;
					std::cout << " " << pBufferFilled->GetPixelFormat();
					std::cout << std::endl;

					if ((pBufferFilled->GetPixelFormat() == "Mono8") || (pBufferFilled->GetPixelFormat() == "BayerRG8") || (pBufferFilled->GetPixelFormat() == "BayerGB8")) //openCV format (CV_8UC1)
					{
						cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC1, (char *)((bo_uint64)(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset()));
						cv::Mat* imConvert = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC1); //memory allocation
						cv::Mat* imCopy = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC1); //memory allocation

						bo_uint64 imConvert_FrameID = pBufferFilled->GetFrameID();
						bo_uint64 imCopy_FrameID = pBufferFilled->GetFrameID();
						bo_uint64 imClone_FrameID = pBufferFilled->GetFrameID();

						imOriginal->convertTo(*imConvert, CV_8UC1, 1.0); //full copy with previous memory allocation AND scaling of 1.0
						imOriginal->copyTo(*imCopy); //full copy with previous memory allocation in cv::Mat imCopy = cv::Mat(height, width, CV_8UC1)
						cv::Mat imClone = imOriginal->clone(); //full copy with memory allocation



						//SAVE IMAGE Mono8 (CV_8UC1) IMAGE
						//PNG

						std::vector<int> compression_params;
						compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
						compression_params.push_back(3); // 0 ... 9 (max compression), default 3
						std::ostringstream png_name;
						png_name << "D:\\image captured\\cv_Mono8_image_" << imConvert_FrameID << ".png";
						cv::imwrite(png_name.str(), *imConvert, compression_params);
						std::cout << " Image " << std::setw(5) << imConvert_FrameID << " saved as '" << png_name.str() << "'" << std::endl;



						//JPEG
						/*
						std::vector<int> jpeg_params;
						jpeg_params.push_back(CV_IMWRITE_JPEG_QUALITY);
						jpeg_params.push_back(95); // 0 ... 100 (max quality), default 95
						std::ostringstream jpeg_name;
						jpeg_name << "cv_Mono8_image_" << imCopy_FrameID << ".jpg";
						cv::imwrite(jpeg_name.str(), *imCopy, jpeg_params);
						std::cout << " Image " << std::setw(5) << imCopy_FrameID << " saved as '" << jpeg_name.str() << "'" << std::endl;
						*/



						//TIFF
						/*
						std::ostringstream tiff_name;
						tiff_name << "cv_Mono8_image_" << imClone_FrameID << ".tif";
						cv::imwrite(tiff_name.str(), imClone);
						std::cout << " Image " << std::setw(5) << imClone_FrameID << " saved as '" << tiff_name.str() << "'" << std::endl;
						*/



						BGAPI2::Image * pTransformImage = NULL;
						if (pImage == NULL)
							pImage = imgProcessor->CreateImage((bo_uint)pBufferFilled->GetWidth(), (bo_uint)(int)pBufferFilled->GetHeight(), pBufferFilled->GetPixelFormat(), pBufferFilled->GetMemPtr(), pBufferFilled->GetMemSize());
						else
							pImage->Init((bo_uint)pBufferFilled->GetWidth(), (bo_uint)(int)pBufferFilled->GetHeight(), pBufferFilled->GetPixelFormat(), pBufferFilled->GetMemPtr(), pBufferFilled->GetMemSize());
						std::cout << "  pImage.Pixelformat:             " << pImage->GetPixelformat() << std::endl;
						std::cout << "  pImage.Width:                   " << pImage->GetWidth() << std::endl;
						std::cout << "  pImage.Height:                  " << pImage->GetHeight() << std::endl;
						std::cout << "  pImage.Buffer:                  " << std::hex << pImage->GetBuffer() << std::dec << std::endl;

						double fBytesPerPixel = pImage->GetNode("PixelFormatBytes")->GetDouble();
						std::cout << "  Bytes per image:                " << (long)((pImage->GetWidth())*(pImage->GetHeight())*fBytesPerPixel) << std::endl;
						std::cout << "  Bytes per pixel:                " << fBytesPerPixel << std::endl;


						// OPENCV GET IMAGE FROM BUFFER
						openCvImage = cv::Mat(pImage->GetHeight(), pImage->GetWidth(), CV_8UC1, (int *)pImage->GetBuffer());

						// create OpenCV window ---
						std::string windowname = "Live feed";
						cv::resize(openCvImage, I, cv::Size(500, 500), 0, 0, CV_INTER_LINEAR);
						cv::namedWindow(windowname, CV_WINDOW_NORMAL);

						//display the current image in the window ----
						cv::imshow(windowname, I);
						cv::waitKey(1);

						// queue buffer again
						std::cout << " Image " << std::setw(5) << pBufferFilled->GetFrameID() << " queue buffer " << std::endl;
						pBufferFilled->QueueBuffer();

						//release opencv images
						delete imOriginal;                     //release opencv image
						delete imCopy;
						delete imConvert;
						//sleep_for(seconds(2));
						std::cout << "Press a character to continue:" << endl;
						getch();
					}
					else
					{
						// queue buffer again
						std::cout << " Image " << std::setw(5) << pBufferFilled->GetFrameID() << " queue buffer " << std::endl;
						pBufferFilled->QueueBuffer();
					}
					std::cout << " " << std::endl;
				}
		}
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << " " << std::endl;


    std::cout << "CAMERA STOP" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    //STOP CAMERA
    try
    {
        //SEARCH FOR 'AcquisitionAbort'
        if(pDevice->GetRemoteNodeList()->GetNodePresent("AcquisitionAbort"))
        {
            pDevice->GetRemoteNode("AcquisitionAbort")->Execute();
            std::cout << "5.1.12   " << pDevice->GetModel() << " aborted " << std::endl;
        }

        pDevice->GetRemoteNode("AcquisitionStop")->Execute();
        std::cout << "5.1.12   " << pDevice->GetModel() << " stopped " << std::endl;
        std::cout << std::endl;

        BGAPI2::String sExposureNodeName = "";
        if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTime")) {
            sExposureNodeName = "ExposureTime";
        }
        else if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTimeAbs")) {
            sExposureNodeName = "ExposureTimeAbs";
        }
        std::cout << "         ExposureTime:                   " << std::fixed << std::setprecision(0) << pDevice->GetRemoteNode(sExposureNodeName)->GetDouble() << " [" << pDevice->GetRemoteNode(sExposureNodeName)->GetUnit() << "]" << std::endl;
        if( pDevice->GetTLType() == "GEV" )
        {
            if(pDevice->GetRemoteNodeList()->GetNodePresent("DeviceStreamChannelPacketSize"))
                std::cout << "         DeviceStreamChannelPacketSize:  " << pDevice->GetRemoteNode("DeviceStreamChannelPacketSize")->GetInt() << " [bytes]" << std::endl;
            else
                std::cout << "         GevSCPSPacketSize:              " << pDevice->GetRemoteNode("GevSCPSPacketSize")->GetInt() << " [bytes]" << std::endl;
                std::cout << "         GevSCPD (PacketDelay):          " << pDevice->GetRemoteNode("GevSCPD")->GetInt() << " [tics]" << std::endl;
        }
        std::cout << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //STOP DataStream acquisition
    try
    {
        if( pDataStream->GetTLType() == "GEV" )
        {
            //DataStream Statistic
            std::cout << "         DataStream Statistics " << std::endl;
            std::cout << "           DataBlockComplete:              " << pDataStream->GetNodeList()->GetNode("DataBlockComplete")->GetInt() << std::endl;
            std::cout << "           DataBlockInComplete:            " << pDataStream->GetNodeList()->GetNode("DataBlockInComplete")->GetInt() << std::endl;
            std::cout << "           DataBlockMissing:               " << pDataStream->GetNodeList()->GetNode("DataBlockMissing")->GetInt() << std::endl;
            std::cout << "           PacketResendRequestSingle:      " << pDataStream->GetNodeList()->GetNode("PacketResendRequestSingle")->GetInt() << std::endl;
            std::cout << "           PacketResendRequestRange:       " << pDataStream->GetNodeList()->GetNode("PacketResendRequestRange")->GetInt() << std::endl;
            std::cout << "           PacketResendReceive:            " << pDataStream->GetNodeList()->GetNode("PacketResendReceive")->GetInt() << std::endl;
            std::cout << "           DataBlockDroppedBufferUnderrun: " << pDataStream->GetNodeList()->GetNode("DataBlockDroppedBufferUnderrun")->GetInt() << std::endl;
            std::cout << "           Bitrate:                        " << pDataStream->GetNodeList()->GetNode("Bitrate")->GetDouble() << std::endl;
            std::cout << "           Throughput:                     " << pDataStream->GetNodeList()->GetNode("Throughput")->GetDouble() << std::endl;
            std::cout << std::endl;
        }
        if( pDataStream->GetTLType() == "U3V" )
        {
            //DataStream Statistic
            std::cout << "         DataStream Statistics " << std::endl;
            std::cout << "           GoodFrames:            " << pDataStream->GetNodeList()->GetNode("GoodFrames")->GetInt() << std::endl;
            std::cout << "           CorruptedFrames:       " << pDataStream->GetNodeList()->GetNode("CorruptedFrames")->GetInt() << std::endl;
            std::cout << "           LostFrames:            " << pDataStream->GetNodeList()->GetNode("LostFrames")->GetInt() << std::endl;
            std::cout << std::endl;
        }

        //BufferList Information
        std::cout << "         BufferList Information " << std::endl;
        std::cout << "           DeliveredCount:        " << bufferList->GetDeliveredCount() << std::endl;
        std::cout << "           UnderrunCount:         " << bufferList->GetUnderrunCount() << std::endl;
        std::cout << std::endl;

        pDataStream->StopAcquisition();
        std::cout << "5.1.12   DataStream stopped " << std::endl;
        bufferList->DiscardAllBuffers();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << std::endl;


    std::cout << "RELEASE" << std::endl;
    std::cout << "#######" << std::endl << std::endl;

    //Release buffers
    std::cout << "5.1.13   Releasing the resources " << std::endl;
    try
    {
        while( bufferList->size() > 0)
        {
            pBuffer = bufferList->begin()->second;
            bufferList->RevokeBuffer(pBuffer);
            delete pBuffer;
        }
        std::cout << "         buffers after revoke:    " << bufferList->size() << std::endl;

        pDataStream->Close();
        pDevice->Close();
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    std::cout << std::endl;
    std::cout << "End" << std::endl << std::endl;

    std::cout << "Input any number to close the program:";
    int endKey = 0;
    std::cin >> endKey;
    return returncode;
}
