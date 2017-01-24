#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>
#include<stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	cv::VideoCapture capVideo;

	cv::Mat imgFrame;

	capVideo.open("SnimakAutoputa.mp4");

	//greska prilikom ucitavanja videaa
	if (!capVideo.isOpened()) {                                                
		std::cout << "\nError reading video file" << std::endl << std::endl;   
		_getch();                   
		return(0);                                                              
	}

	//proveriti dodatno
	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 1) {
		std::cout << "\nError: video file must have at least one frame";
		_getch();
		return(0);
	}

	//ucitavanje prvog imageFrame-a
	capVideo.read(imgFrame);

	char chCheckForEscKey = 0;

	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		cv::imshow("Pracenje drumskog saobracaja", imgFrame);

		//provera da li ima jos frame-ova za citanje
		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {       
			capVideo.read(imgFrame);                           
		}
		else {                                                 
			std::cout << "End of video\n";                     
			break;                                             
		}

		chCheckForEscKey = cv::waitKey(1);      

	}

	if (chCheckForEscKey != 27) {             
		cv::waitKey(0);                     
	}

	return 0;
}


