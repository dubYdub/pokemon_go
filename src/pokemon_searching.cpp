#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <std_msgs/Int32.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Bool.h>

using namespace cv;
using namespace std;

static const std::string OPENCV_WINDOW = "Pokemon Search";



class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::Subscriber save_sub_;
  int fileNum;
  Mat img;
  int count;

public:
  ImageConverter()
    : it_(nh_), fileNum(0), count(0)
  {
    // Subscribe to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/camera/rgb/image_raw", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/pokemon_go/searcher", 1);
    save_sub_ = nh_.subscribe("/pokemon_go/save", 1, &ImageConverter::saveImg, this);
    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void saveImg(std_msgs::Bool save){
    ROS_INFO("Catch the pokemon!");
	  if(save.data){
      stringstream stream;
          stream <<"/home/youngwilliam/catkin_ws/src/pokemon_go/image/pokemon" << fileNum <<".jpg";
      imwrite(stream.str(),img);
      cout <<"pokemon" << fileNum << " had Saved."<< endl;
      fileNum++;
	  }
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    // ROS_INFO("Start searching: time = [%d]", ++count);

    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    img = cv_ptr->image;
    // Draw an target square on the video stream
    int height = cv_ptr->image.rows;
    int width = cv_ptr->image.cols;
    cv::rectangle(cv_ptr->image, cv::Point(width/3, height/6),
	  cv::Point(2*width/3, 5*height/6), CV_RGB(255,0,0));

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "pokemon_searching");
  ImageConverter ic;
  ros::spin();
  return 0;
}
