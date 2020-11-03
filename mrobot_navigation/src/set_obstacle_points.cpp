#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "obstacle_points");
  ros::NodeHandle n;
  ros::Publisher staticpose_pub = n.advertise<geometry_msgs::PoseStamped>("set_obstacle_points",100);
  ros::Rate loop_rate(10);

  int count=0;
  while (ros::ok())
  {
    geometry_msgs::PoseStamped staticpose;
    staticpose.header.stamp = ros::Time::now();;
    staticpose.header.frame_id = "obstacle";
    staticpose.pose.position.x =0;
    staticpose.pose.position.y = 1;
    staticpose.pose.position.z = 0;
    staticpose.pose.orientation.x = 0.7153;
    staticpose.pose.orientation.y = -0.0051;
    staticpose.pose.orientation.z = 0.0278;
    staticpose.pose.orientation.w = 0.6983;

    geometry_msgs::PoseStamped groundtruth;
    groundtruth.header.stamp = ros::Time::now();;
    groundtruth.header.frame_id = "map";
    groundtruth.pose.position.x = 0;
    groundtruth.pose.position.y = 12.5;
    groundtruth.pose.position.z = 0;
    groundtruth.pose.orientation.x = 0;
    groundtruth.pose.orientation.y = 0;
    groundtruth.pose.orientation.z = 0;
    groundtruth.pose.orientation.w = 0;

    //ROS_INFO("%s", staticpose);
    staticpose_pub.publish(staticpose);
    staticpose_pub.publish(groundtruth);
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
    std::cout<<"count:"<<count<<std::endl;
  }

  return 0;
}