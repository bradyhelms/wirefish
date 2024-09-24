#include <iostream>
#include <iomanip>
#include <unistd.h>
#include "stdlib.h"
#include <sstream>
#include "EthLayer.h"
#include "IPv4Layer.h"
#include "TcpLayer.h"
#include "Packet.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"

void usage();


int main(int argc, char** argv) 
{
    if (argc == 1) {
        usage();
        return 0;
    }

    int opt = -1;
    std::string interface_addr;
    unsigned int capture_time = 0;

    while ((opt = getopt(argc, argv, "a:t:")) != -1) 
    {
        switch(opt)
        {
            case 'a':
                if (optarg && *optarg) 
                {
                    interface_addr = optarg;
                    break;
                } else 
                {
                    std::cout << "Must provide an ip address (-a flag)"
                        << std::endl;
                    return 0;
                }
            case 't':
                if (optarg && *optarg) 
                {
                    std::stringstream s(optarg);
                    s >> capture_time;
                    break;
                } else 
                {
                    std::cout << "Must provide a capture run time\n"
                        << "\tExample: wire_fish -t 10\n"
                        << "\t10 can be replaced with any number of seconds."
                        << std::endl;
                    return 0;
                }
            default:
                usage();
                return 0;
        }
    }

    auto* device = pcpp::PcapLiveDeviceList::
        getInstance()
        .getPcapLiveDeviceByIp(interface_addr);

    if (device == nullptr) 
    {
        std::cerr << "Cannot find device with address: "
            << interface_addr << std::endl;
    }

    if (!device->open()) {
        std::cerr << "Cannot open device: " 
            << device->getName() << std::endl;
    }

    // Start capture
    if (capture_time == 0)
    {
        capture_time = 10;
        std::cout << "Running a " << capture_time 
            << " second capture (default length) on " 
            << interface_addr << std::endl;
    } else
    {
        std::cout << "Running a " << capture_time
            << " second capture on " 
            << interface_addr << std::endl;
    }

    std::cout << std::endl;

    pcpp::RawPacketVector rawPacketVec;
    device->startCapture(rawPacketVec);
    pcpp::multiPlatformSleep(capture_time);
    device->stopCapture();

    std::cout   << std::setw(5)         << "No."
                << std::setw(20)        << "Source MAC"
                << std::setw(20)        << "Dest MAC"
                << std::setw(20)        << "Source IP"
                << std::setw(20)        << "Dest IP"
                << std::setw(10)        << "Src Port"
                << std::setw(10)        << "Dest Port"
                << std::endl;

    unsigned int packet_count = 1;
    unsigned int unprocessed_packets = 0;
    for (const auto& packet : rawPacketVec) 
    {
        pcpp::Packet parsedPacket(packet);
        auto* ethernetLayer = parsedPacket.getLayerOfType<pcpp::EthLayer>();
        auto* ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
        auto* tcpLayer = parsedPacket.getLayerOfType<pcpp::TcpLayer>();

        if (ethernetLayer == nullptr)
        {
            //std::cerr << "Error processing the Ethernet layer" << std::endl;
            ++unprocessed_packets;
            continue;
        }

        if (ipLayer == nullptr) 
        {
            //std::cerr << "Error processing the IPv4 layer" << std::endl;
            ++unprocessed_packets;
            continue;
        }
        
        if (tcpLayer == nullptr)
        {
            //std::cerr << "Error processing the TCP layer" << std::endl;
            ++unprocessed_packets;
        }

        std::cout << std::setw(5)  << packet_count++
                  << std::setw(20) << ethernetLayer->getSourceMac()
                  << std::setw(20) << ethernetLayer->getDestMac()  
                  << std::setw(20) << ipLayer->getSrcIPAddress()   
                  << std::setw(20) << ipLayer->getDstIPAddress()   
                  << std::setw(10) << tcpLayer->getSrcPort()   
                  << std::setw(10) << tcpLayer->getDstPort()   
                  << std::endl;
    }

    std::cout << "\nUnprocessed packets: " << unprocessed_packets << std::endl;
}

void usage() 
{
    std::cout   << "usage: wire_fish [option] [arg]" << "\n"
                << "\t-a [IP address (DDN)]"         << "\n"
                << "\t-t [time in seconds]"          << "\n"
        << std::endl;
}
