#ifndef UDP_LOOPBACK_NODE_HPP
#define UDP_LOOPBACK_NODE_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

class UdpLoopbackNode : public rclcpp::Node
{
public:
    UdpLoopbackNode();
    ~UdpLoopbackNode();

private:
    void check_udp();

    int sockfd_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

#endif  // UDP_LOOPBACK_NODE_HPP
