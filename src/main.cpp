#include "rclcpp/rclcpp.hpp"
#include "udp_loopback_node.hpp"

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<UdpLoopbackNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
