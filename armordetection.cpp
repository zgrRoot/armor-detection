#include "armordetection.h"

 
ArmorDetection::ArmorDetection() = default;
 
 
ArmorDetection::ArmorDetection(Mat & input) {
	frame = input;
}

void ArmorDetection::setInputImage(Mat input) {
	frame = input;
	currentCenter.x = 0;
	currentCenter.y = 0;
}

//ͼ��Ԥ����
void ArmorDetection::Pretreatment(){
	Mat input;
	//Point p, center;
	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	vector<Rect> boundRect(contours.size());
	Point2f vertex[4];

	//��ֵ��������ͨ�����
	vector<Mat> channels;
	//Mat dst;
	split(frame, channels);
	//threshold(frame, dst, 200, 255, THRESH_BINARY);
	//cvtColor(dst, dst, CV_BGR2GRAY);
	//imshow("dst", dst);
	if (1)
		thresholdImage = channels.at(2) - channels.at(0);
	else thresholdImage = channels.at(0) - channels.at(2);
	

	int threshold1 = 50;
	int threshold2 = 255;
	cvCreateTrackbar("controlm", "mask", &threshold1, 255);
	cvCreateTrackbar("controlm", "mask", &threshold2, 255);
	 

	 

		//������ǿ
	//dst.copyTo(mask);
		thresholdImage.copyTo(mask);

		Canny(mask, mask, threshold1, threshold2, 3);
		Mat  element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		dilate(mask, mask, element);
		imshow("mask", mask);
		findContours(mask, contours, hireachy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//ɸѡ��ȥ��һ���־���
		for (int i = 0; i < contours.size(); ++i) {
			RotatedRect minRect = minAreaRect(Mat(contours[i]));
			minRect.points(vertex);
			if (minRect.size.width > minRect.size.height) {
				minRect.angle += 90;
				float t = minRect.size.width;
				minRect.size.width = minRect.size.height;
				minRect.size.height = t;
			}

			if ((minRect.size.width * 10 > minRect.size.height)
				&& (minRect.size.width * 1 < minRect.size.height)
				&& (abs(minRect.angle) < 30)) {
				minRects.push_back(minRect);
			}//̫С����ȥ
			//��������
			for (int j = 0; j < 4; j++)
			{
				line(frame, vertex[j], vertex[(j + 1) % 4], Scalar(255, 0, 0), 2);
			}
		//�ڻ������Ļ�һ��ʮ��
			line(frame, Point(frame.cols / 2 - 15, frame.rows / 2),
				Point(frame.cols / 2 + 15, frame.rows / 2), Scalar(0, 255, 255), 5);
			line(frame, Point(frame.cols / 2, frame.rows / 2 - 15),
				Point(frame.cols / 2, frame.rows / 2 + 15), Scalar(0, 255, 255), 5);
			 circle(frame, Point(frame.cols / 2, frame.rows / 2), 4, Scalar(0, 0, 255), -1);

		}
		//imshow("frame", frame);
    }


	Point2f ArmorDetection::GetArmorCenter() {
		//�������о��Σ��������
		RotatedRect leftRect, rightRect;
		vector<int*> reliability;
		vector<float*> reliability2;
		double area[2], distance, height;
		 
			if (minRects.size() < 2) {
				LostTarget();
				return currentCenter;
			}
		
		for (int i = 0; i < minRects.size(); ++i) {
			for (int j = i + 1; j < minRects.size(); ++j) {
				int temp[2];
				float pemp[2];
				leftRect = minRects[i];
				rightRect = minRects[j];

				//����ʶ����ĵ����������ɸѡ
				float angleDiff_ = abs(leftRect.angle - rightRect.angle);
				//���Ȳ����
				float LenDiff_ratio = abs(leftRect.size.height-rightRect.size.height) / max(leftRect.size.height, rightRect.size.height);
				//ɸѡ
				if (angleDiff_ >light_max_angle_diff_ ||
					LenDiff_ratio >light_max_height_diff_ratio_)
				{
					continue;
				}

				
				//���ҵ���������
				float dis = Distance(leftRect.center, rightRect.center);
				//���ҵ������ȵ�ƽ��ֵ
				float meanLen = (leftRect.size.height + rightRect.size.height) / 2;
				//���ҵ������ĵ�y�Ĳ�ֵ
				float yDiff = abs(leftRect.center.y - rightRect.center.y);
				//y�����
				float yDiff_ratio = yDiff / meanLen;
				//���ҵ������ĵ�x�Ĳ�ֵ
				float xDiff = abs(leftRect.center.x - rightRect.center.x);
				//x�����
				float xDiff_ratio = xDiff / meanLen;
				//��������������ȱ�ֵ
				float ratio = dis / meanLen;
				double half_height = (leftRect.size.height + right.size.height) / 4;
				float distance2 = Distance(leftRect.center, rightRect.center);
				//ɸѡ
				if (yDiff_ratio >light_max_y_diff_ratio_ ||
					xDiff_ratio <light_min_x_diff_ratio_ ||
					ratio >armor_max_aspect_ratio_ ||
					ratio <armor_min_aspect_ratio_||abs(leftRect.center.y-rightRect.center.x)<0.4*half_height||leftRect.center.x!=rightRect.center.x||leftRect.size.height!=rightRect.size.height
					||distance>3.0*half_height)
				{
					continue;
				}
				line(frame, Point(leftRect.center.x, leftRect.center.y + leftRect.size.height / 2), Point(rightRect.center.x, rightRect.center.y + rightRect.size.height / 2), Scalar(255, 0, 0), 2);
				line(frame, Point(rightRect.center.x, rightRect.center.y + rightRect.size.height / 2), Point(rightRect.center.x, rightRect.center.y - rightRect.size.height / 2), Scalar(255, 0, 0), 2);
				line(frame, Point(rightRect.center.x, rightRect.center.y - rightRect.size.height / 2), Point(leftRect.center.x, leftRect.center.y - leftRect.size.height / 2), Scalar(255, 0, 0), 2);
				line(frame, Point(leftRect.center.x, leftRect.center.y - leftRect.size.height / 2), Point(leftRect.center.x, leftRect.center.y + leftRect.size.height / 2), Scalar(255, 0, 0), 2);
				
				//ɸѡװ�װ�
				//ɸѡ��Сװ�װ�

				int armorType = ratio >armor_big_armor_ratio?1:0;
				float vertex[4][2];
				float ratiOff = (armorType == 1) ? max(armor_big_armor_ratio - ratio, float(0)) : max(armor_small_armor_ratio - ratio, float(0));
				float yOff = yDiff / meanLen;
				float  rotationScore= -(ratiOff * ratiOff + yOff * yOff);
			 
				//�õ�ƥ���װ�װ�,������ָ�������鴢������
				//����
				/*
				vertex[0][0] = leftRect.center.x;
				vertex[0][1] = leftRect.center.y+leftRect.size.height/2;
				//����
				vertex[1][0] = rightRect.center.x;
				vertex[1][1] =rightRect.center.y+rightRect.size.height/2 ;
				//����
				vertex[2][0] = leftRect.center.x;
				vertex[2][1] = leftRect.center.y - leftRect.size.height / 2;
				//����
				vertex[3][0] = rightRect.center.x;
				vertex[3][1] = rightRect.center.y - rightRect.size.height / 2;
				*/
				 
				temp[0] = (float)i;
				temp[1] = (float)j;
				pemp[0] = rotationScore;
				pemp[1] = (abs(rightRect.center.x - leftRect.center.x)) * 2 * leftRect.size.height;
				   
				reliability.push_back(temp);
				reliability2.push_back(pemp);
				break;
			}
			 
		}
		//�Եõ���װ�װ����ɸѡ
		//��װ�װ���з������㣬������÷���ߵ�װ�װ���д��
		 if (reliability.empty()) {
			LostTarget();
			return currentCenter;
		}
		else {
			 for (int t = 0; t < reliability.size(); ++t) {
				 if (reliability2[k][1] < reliability2[k + 1][1])
					 reliability2[k][0] = 0;
			 }



			 float maxLevel = 0;int index = 0;
			for (int k = 0; k < reliability.size();++k) {
				if (reliability2[k][0]<maxLevel) {
					maxLevel = reliability[k][2];
					index = k;
				}
			}

			currentCenter.x = (minRects[reliability[index][0]].center.x + minRects[reliability[index][1]].center.x) / 2;
			currentCenter.y = (minRects[reliability[index][0]].center.y + minRects[reliability[index][1]].center.y) / 2;

			//����һ�εĽ���Ա�
			if (lastCenter.x == 0 && lastCenter.y == 0) {
				lastCenter = currentCenter;
				lost = 0;
			}
			else {
				double difference = Distance(currentCenter, lastCenter);
				if (difference > 300) {
					LostTarget();
					return currentCenter;
				}
			}
			if (((frame.rows / 2 )- 10) < currentCenter.x < ((frame.rows / 2) - 10)) {


				line(frame, Point(currentCenter.x - 10, currentCenter.y - 10),
					Point(currentCenter.x + 10, currentCenter.y + 10), Scalar(255, 255, 0), 5);
				line(frame, Point(currentCenter.x + 10, currentCenter.y - 10),
					Point(currentCenter.x - 10, currentCenter.y + 10), Scalar(255, 255, 0), 5);
				circle(frame, currentCenter, 7.5, Scalar(0, 0, 255), 5);
			}
			
			 
			/*center[0] = currentCenter.x;
			center[1] = currentCenter.y;
			snprintf(str, 4, "%f", center[0]);
			snprintf(str2, 4, "%f", center[1]);
			 
 			putText(frame, "str", Point(currentCenter.x - 15, currentCenter.y), CV_FONT_BOLD, 0.7, Scalar(0, 255, 0), 2, 8);
			putText(frame, str2, Point(currentCenter.x + 15, currentCenter.y), CV_FONT_BOLD, 0.7, Scalar(0, 255, 0), 2, 8);*/
			imshow("frame", frame);  
			return currentCenter;
		}
	}


	void ArmorDetection::LostTarget() {
		lost++;
		if (lost < 3) {
			currentCenter = lastCenter;
		}
		else {
			currentCenter = Point2f(0, 0);
			lastCenter = Point2f(0, 0);
		}
	}

	double ArmorDetection::Distance(Point2f a, Point2f b) {
		return sqrt((a.x - b.x) * (a.x - b.x) +
			(a.y - b.y) * (a.y - b.y));
	}

	double ArmorDetection::max(double first, double second) {
		return first > second ? first : second;
	}

	double ArmorDetection::min(double first, double second) {
		return first < second ? first : second;
	}
