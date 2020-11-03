#include <ros/ros.h>
#include <signal.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <string.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <visualization_msgs/Marker.h>
#include <cmath>

ros::Publisher cmdVelPub;
ros::Publisher marker_pub;

void shutdown(int sig)
{
  cmdVelPub.publish(geometry_msgs::Twist());
  ros::Duration(1).sleep(); // sleep for  a second
  ROS_INFO("nav_square.cpp ended!");
  ros::shutdown();
}

void init_markers(visualization_msgs::Marker *marker)
{
  marker->ns       = "waypoints";
  marker->id       = 0;
  marker->type     = visualization_msgs::Marker::CUBE_LIST;
  marker->action   = visualization_msgs::Marker::ADD;
  marker->lifetime = ros::Duration();//0 is forever
  marker->scale.x  = 0.2;
  marker->scale.y  = 0.2;
  marker->color.r  = 1.0;
  marker->color.g  = 0.7;
  marker->color.b  = 1.0;
  marker->color.a  = 1.0;
  marker->header.frame_id = "odom";
  marker->header.stamp = ros::Time::now();
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "nav_move_base");
  std::string topic = "/cmd_vel";
  ros::NodeHandle node;
  //订阅move_base action server
  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base",true);
  marker_pub = node.advertise<visualization_msgs::Marker>("waypoint_markers", 10);
  ros::Publisher cancel_pub =  node.advertise<actionlib_msgs::GoalID>("move_base/cancel",1);
  actionlib_msgs::GoalID empty_goal;

  visualization_msgs::Marker  line_list;
  //中断程序
  signal(SIGINT, shutdown);
  ROS_INFO("move_base_goal.cpp start...");
  double square_size = 10;
  //存储四元树
  geometry_msgs::Quaternion quaternions[4];
  //将角度转换为四元数
  double angle = M_PI/2;
  quaternions[0] = tf::createQuaternionMsgFromRollPitchYaw(0, 0, angle);

  //在 map 坐标系下，一个pos包含了position和四元数
  geometry_msgs::Point point;
  geometry_msgs::Pose pose_list[4];
  point.x = square_size;
  point.y = 0.0;
  point.z = 0.0;
  pose_list[0].position = point;
  pose_list[0].orientation = quaternions[0];

  //在RViz上设置虚拟标记
  init_markers(&line_list);
  line_list.points.push_back(pose_list[0].position);
  cmdVelPub = node.advertise<geometry_msgs::Twist>(topic, 5);
  ROS_INFO("Waiting for move_base action server...");
  //超时60秒，连接不到move_base server,发出警告
  if(!ac.waitForServer(ros::Duration(60)))
  {
    ROS_INFO("Can't connected to move base server");
    return 1;
  }
  ROS_INFO("Connected to move base server");
  ROS_INFO("Starting navigation test");
  int count = 0;
  while(ros::ok())
  {
     //更新marker
     marker_pub.publish(line_list);
     move_base_msgs::MoveBaseGoal goal;
     goal.target_pose.header.frame_id = "map";
     goal.target_pose.header.stamp = ros::Time::now();
     goal.target_pose.pose = pose_list[count];
     while (1)
     {
        cancel_pub.publish(empty_goal);
        ac.sendGoal(goal);
        ROS_INFO("SEND NEW GOAL");
        sleep(5);
     }
  }
  return 0;
}


