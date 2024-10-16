//
// Created by shivesh on 9/17/19.
//
#include "ars_40X/ros/object_list_ros.hpp"

#include <geometry_msgs/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "ars_40X/Object.h"
#include "ars_40X/ObjectList.h"
#include <visualization_msgs/MarkerArray.h>
#include <sensor_msgs/PointCloud.h>

namespace ars_40X {
    ObjectListROS::ObjectListROS(ros::NodeHandle &nh, ARS_40X_CAN *ars_40X_can) :
            ars_40X_can_(ars_40X_can) {
        object_0_status_ = ars_40X_can_->get_object_0_status();
        object_1_general_ = ars_40X_can_->get_object_1_general();
//  object_2_quality_ = ars_40X_can_->get_object_2_quality();
//  object_3_extended_ = ars_40X_can_->get_object_3_extended();
        objects_data_pub_ = nh.advertise<ObjectList>("ars_40X/objects", 10);

        makerArrayPub_=nh.advertise<visualization_msgs::MarkerArray >("visualization_marker_distination",5);

        radar_obstacle_pub_=nh.advertise<sensor_msgs::PointCloud>("radar_obstacles",5);
    }

    ObjectListROS::~ObjectListROS() {
    }

    void ObjectListROS::set_frame_id(std::string frame_id) {
        frame_id_ = frame_id;
    }

    void ObjectListROS::pub_object_position() {
        visualization_msgs::MarkerArray marker_array;

        for(const auto& object:object_list.objects){
            visualization_msgs::Marker marker;

            marker.header.frame_id=frame_id_;
            marker.header.stamp=ros::Time::now();
            marker.type=visualization_msgs::Marker::CUBE;
            marker.id=object.id;
            //设置物体大小
            marker.scale.x=0.25;
            marker.scale.y=0.25;
            marker.scale.z=0.25;
            //设置物体颜色
            marker.color.r=2;
            marker.color.g=0;
            marker.color.b=0;
            marker.color.a=1;

            marker.pose.position.x=object.position.pose.position.x;
            marker.pose.position.y=object.position.pose.position.y;
            marker.pose.position.z=0.25;
            marker.lifetime=ros::Duration(0.01);

            marker_array.markers.push_back(marker);

            ///文字数组
            visualization_msgs::Marker marker2;
            marker2.action=visualization_msgs::Marker::ADD;
            marker2.header.frame_id=frame_id_;
            marker2.header.stamp=ros::Time::now();
            marker2.id=object.id+100;
            marker2.type=visualization_msgs::Marker::TEXT_VIEW_FACING;

            marker2.scale.x=0.5;
            marker2.scale.y=0.5;
            marker2.scale.z=0.5;

            marker2.color.r=2;
            marker2.color.g=0;
            marker2.color.b=0;
            marker2.color.a=0.4;

            marker2.pose.position.x=object.position.pose.position.x;
            marker2.pose.position.y=object.position.pose.position.y;
            marker2.pose.position.z=0.75;
            marker2.lifetime=ros::Duration(0.01);

            std::ostringstream str;
            str << "Radar " << "x: " << object.position.pose.position.x << " y:" << object.position.pose.position.y;
            marker2.text=str.str();
            marker_array.markers.push_back(marker2);

        }

        makerArrayPub_.publish(marker_array);

    }

    void ObjectListROS::pub_radar_obstacle() {
        sensor_msgs::PointCloud radar_obstacle;
        radar_obstacle.header.stamp=ros::Time::now();
        radar_obstacle.header.frame_id=frame_id_;

        for(const auto &object:object_list.objects){
            radar_obstacle.points.emplace_back();
            radar_obstacle.points.back().x=object.position.pose.position.x;
            radar_obstacle.points.back().y=object.position.pose.position.y;
            radar_obstacle.points.back().z=0;
        }

        radar_obstacle_pub_.publish(radar_obstacle);
    }


    void ObjectListROS::send_object_0_status() {
        object_list.header.stamp = ros::Time::now();
        object_list.header.frame_id = frame_id_;
//  object_list.objects.erase(
//      object_list.objects.begin() ,
//      object_list.objects.begin() + object_list.objects.size());

//        pub_object_position();///调试

        pub_radar_obstacle();

//        objects_data_pub_.publish(object_list);


        object_list.objects.clear();
//  object_2_quality_id_ = 0;
//  object_3_extended_id_ = 0;
    }

    void ObjectListROS::send_object_1_general() {
        Object object;
        object.id = object_1_general_->get_object_id();
        object.position.pose.position.x = object_1_general_->get_object_long_dist();
        object.position.pose.position.y = object_1_general_->get_object_lat_dist();
        object.relative_velocity.twist.linear.x = object_1_general_->get_object_long_rel_vel();
        object.relative_velocity.twist.linear.y = object_1_general_->get_object_lat_rel_vel();
        object.dynamic_property = object_1_general_->get_object_dyn_prop();
        object.rcs = object_1_general_->get_object_rcs();
        object_list.objects.push_back(object);
    }

//void ObjectListROS::send_object_2_quality() {
//  object_list.objects[object_2_quality_id_].position.covariance[0] =
//      pow(object_2_quality_->get_object_lat_dist_rms(), 2);
//  object_list.objects[object_2_quality_id_].position.covariance[7] =
//      pow(object_2_quality_->get_object_long_dist_rms(), 2);
//  object_list.objects[object_2_quality_id_].position.covariance[35] =
//      pow(object_2_quality_->get_object_orientation_rms(), 2);
//  object_list.objects[object_2_quality_id_].relative_velocity.covariance[0] =
//      pow(object_2_quality_->get_object_long_rel_vel_rms(), 2);
//  object_list.objects[object_2_quality_id_].relative_velocity.covariance[7] =
//      pow(object_2_quality_->get_object_lat_rel_vel_rms(), 2);
//  object_list.objects[object_2_quality_id_].relative_acceleration.covariance[0] =
//      pow(object_2_quality_->get_object_long_rel_accel_rms(), 2);
//  object_list.objects[object_2_quality_id_].relative_acceleration.covariance[7] =
//      pow(object_2_quality_->get_object_lat_rel_accel_rms(), 2);
//  object_list.objects[object_2_quality_id_].meas_state = object_2_quality_->get_object_meas_state();
//  object_list.objects[object_2_quality_id_].prob_of_exist =
//      object_2_quality_->get_object_prob_of_exist();
//  ++object_2_quality_id_;
//}
//
//void ObjectListROS::send_object_3_extended() {
//  object_list.objects[object_3_extended_id_].length = object_3_extended_->get_object_length();
//  object_list.objects[object_3_extended_id_].width = object_3_extended_->get_object_width();
//  tf2::Quaternion q;
//  q.setRPY(0, 0, object_3_extended_->get_object_orientation_angle() * M_PI / 180.0);
//  object_list.objects[object_3_extended_id_].position.pose.orientation.w = q.getW();
//  object_list.objects[object_3_extended_id_].position.pose.orientation.x = q.getX();
//  object_list.objects[object_3_extended_id_].position.pose.orientation.y = q.getY();
//  object_list.objects[object_3_extended_id_].position.pose.orientation.z = q.getZ();
//  object_list.objects[object_3_extended_id_].class_type = object_3_extended_->get_object_class();
//  ++object_3_extended_id_;
//}
}