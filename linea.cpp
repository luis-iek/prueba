#include<ros/ros.h>
#include<geometry_msgs/Twist.h>
#include<turtlesim/Pose.h>
#include<sstream>
#include<math.h>

//#define PI 3.141592654

ros::Publisher vel_pub;
ros::Subscriber pose_sub;
turtlesim::Pose turtlesim_pose;

const double x_min = 0.0;
const double y_min = 0.0;
const double x_max = 11.0;
const double y_max = 11.0;
const double PI = acos(-1);

void move(double , double , bool );
void rotate(double, double, bool);
double deg2rad(double);
double setDesOr(double);
void poseCallback(const turtlesim::Pose::ConstPtr &);

using namespace std;

int main(int argc, char **argv){
	ros::init(argc, argv, "mover");
	ros::NodeHandle n;
	double speed, dist, ang_speed, angle, abs_a;
	bool isF, clockwise;
	vel_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
	pose_sub = n.subscribe("/turtle1/pose", 10, poseCallback);

	cout << "enter the angular velocity (degree/sec): ";
	cin >> ang_speed;
	cout << "enter the desired angle (degrees): ";
	cin >> angle;
	cout << "clockwise?: ";
	cin >> clockwise;
	rotate(deg2rad(ang_speed), deg2rad(angle), clockwise);

	cout << "enter speed: ";
	cin >> speed;
	cout << "enter distance: ";
	cin >> dist;
	cout << "forward?:  ";
	cin >> isF;
	move(speed, dist, isF);

	/*cout << "enter the absolute angle: ";
	cin >>abs_a;
	abs_a = setDesOr(abs_a);*/
}

void move(double speed, double distance, bool isforward){
	geometry_msgs::Twist vel_msg;
	double t0, t1, cur_dis=0;
	if(isforward){
		vel_msg.linear.x = abs(speed);
		vel_msg.linear.y = abs(speed);
	}
	else{
		vel_msg.linear.x = -abs(speed);
		vel_msg.linear.y = -abs(speed);
	}
	t0 = ros::Time::now().toSec();
	ros::Rate rate(10);	// Loop at 10 Hz
	while(cur_dis < distance){
		vel_pub.publish(vel_msg);
		t1 = ros::Time::now().toSec();
		cur_dis = speed * (t1 - t0);
		ros::spinOnce();
		rate.sleep();
	}
}

void rotate(double ang_speed, double rel_ang, bool clockwise){
	geometry_msgs::Twist vel_msg;
	vel_msg.linear.x = 0;
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;
	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;
	if (clockwise)	vel_msg.angular.z = -abs(ang_speed);
	else	vel_msg.angular.z = abs(ang_speed);
	double cur_ang = 0.0;
	double t0, t1;
	ros::Rate rate(10);
	t0 = ros::Time::now().toSec();
	while(cur_ang < rel_ang){
		vel_pub.publish(vel_msg);
		t1 = ros::Time::now().toSec();
		cur_ang = ang_speed * (t1 - t0);
		ros::spinOnce();
		rate.sleep();
	}
	vel_msg.angular.z = 0;
	vel_pub.publish(vel_msg);
}

double deg2rad(double input){
	return input * PI / 180.0;
}

double setDesOr(double des){
	double rel;
	rel = des - turtlesim_pose.theta;
	bool clockwise = ((rel < 0)? true : false);
	// cout << des << "," << turtlesim_pose.theta << rel
	rotate (abs(rel), abs(rel), clockwise);
}

void poseCallback(const turtlesim::Pose::ConstPtr & pose_msg){
	turtlesim_pose.x = pose_msg -> x;
	turtlesim_pose.y = pose_msg -> y;
	turtlesim_pose.theta = pose_msg -> theta;
}