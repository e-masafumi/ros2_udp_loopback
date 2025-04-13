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
    UdpLoopbackNode() : Node("udp_loopback_node")
    {
        // UDP受信ポートの設定
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to create socket");
            return;
        }

        sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5005);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to bind socket");
            return;
        }

        fcntl(sockfd_, F_SETFL, O_NONBLOCK);  // 非ブロッキング受信

        // Publisher（ROS 2トピックへの送信）
        publisher_ = this->create_publisher<std_msgs::msg::Float32>("udp_echo", 10);

        // タイマーで定期チェック（10ms）
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&UdpLoopbackNode::check_udp, this)
        );
    }

    ~UdpLoopbackNode()
    {
        if (sockfd_ >= 0) close(sockfd_);
    }

private:
    void check_udp()
    {
        uint8_t buf[4];
        sockaddr_in sender_addr;
        socklen_t addrlen = sizeof(sender_addr);

        int len = recvfrom(sockfd_, buf, sizeof(buf), 0,
                           (struct sockaddr*)&sender_addr, &addrlen);

        if (len == 4) {
            float value;
            std::memcpy(&value, buf, sizeof(float));

            // ① UDPで送り返す（ループバック）
            sendto(sockfd_, buf, sizeof(float), 0,
                   (struct sockaddr*)&sender_addr, sizeof(sender_addr));

            // ② ROS 2トピックにpublish
            auto msg = std_msgs::msg::Float32();
            msg.data = value;
            publisher_->publish(msg);
        }
    }

    int sockfd_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};
