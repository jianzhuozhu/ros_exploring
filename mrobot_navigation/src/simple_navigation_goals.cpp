#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <iostream>
#include "geometry_msgs/PoseStamped.h"
#include <tf/transform_datatypes.h>

#include <boost/thread.hpp>
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int x [5] = { 4, -4, 0, 0}; 
int y [5] = {0, 0 ,-4 , 4};
float x_door[6] = {2,2,2,17,17,17};
float y_door[6] = {1,7,12,1,7,12};
 
int lenght0f_Array = sizeof(x) / sizeof(x[0]);
int lenght0f_Array_door = sizeof(x_door) / sizeof(x_door[0]);


int main(int argc, char** argv){
    ros::init(argc, argv, "simple_navigation_goals");
    ros::NodeHandle n;
    ros::Publisher staticpose_pub = n.advertise<geometry_msgs::PoseStamped>("set_obstacle_points",100);
    ros::Rate loop_rate(10);

    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);
    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
    }
    move_base_msgs::MoveBaseGoal goal;
    //we'll send a goal to the robot to move 1 meter forward
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
 

    //come into the selected door
    goal.target_pose.pose.position.x = x_door[0];
    goal.target_pose.pose.position.y = y_door[0];
     goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(M_PI);
    ROS_INFO("Sending goal");
    ac.sendGoal(goal);
    ac.waitForResult();
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
    ROS_INFO("Hi, the base finished the movement ");
    //set obs
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
    //ROS_INFO("%s", staticpose);
    staticpose_pub.publish(staticpose);
    }
    else
    ROS_INFO("The base failed to finish the movement for some reason");

    
    //get out of maze
    goal.target_pose.pose.position.x = x_door[3];
    goal.target_pose.pose.position.y = y_door[3];
     goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(M_PI);
    ROS_INFO("Sending goal");
    ac.sendGoal(goal);
    ac.waitForResult();
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
    ROS_INFO("Hi, the base finished the movement ");
    }
    else
    ROS_INFO("The base failed to finish the movement for some reason");
    return 0;
}
