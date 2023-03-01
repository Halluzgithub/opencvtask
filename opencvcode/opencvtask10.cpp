#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

RNG rng(12345);//���������ɫ ����Ϊ12345
void connected_component_stats(Mat& image);

int main(int argc, char** argv) {
    Mat src = imread("Resources/10.png");
    if (src.empty()) {
        printf("could not load image...\n");
    }
    imshow("src", src);

    connected_component_stats(src);

    while (1)
        if (waitKey(1) == 32)
            break;
    return 0;
}

void connected_component_stats(Mat& image) {
    // ��ֵ��
    Mat gray, binary;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);////THRESH_OTSU����Ӧȡ��ֵ��ʡȥ���Լ�����ֵ���鷳�����������������ֶ�ȡ����ֵ����Ч
    //imshow("binary", binary);
    // ��̬ѧ����
    Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    morphologyEx(binary, binary, MORPH_DILATE, k);//���� �Ŀ���еĿն�
    imshow("��̬ѧ������", binary);

    Mat labels = Mat::zeros(image.size(), CV_32S);// labelsΪ���ͼ�񣨱���index=0�����Ǳ����������ͨ�����Ϊ�±�ֵ�����һ
    Mat stats, centroids;//stats״̬����size=��ͨ����*5����centroids��ͨ�����ĵľ���size=��ͨ����*2��
    // stats ��ÿһ����ǵ�ͳ����Ϣ����һ��5�еľ���ÿһ�ж�Ӧÿ����ͨ�������Ӿ��ε�x��y��width��height�����
    int num_labels = connectedComponentsWithStats(binary, labels, stats, centroids, 8);
    printf("total labels : %d\n", (num_labels - 1));

    //�õ�ÿ����ͨ�����ɫ
    vector<Vec3b> colors(num_labels);
    // background color��ɫ
    colors[0] = Vec3b(0, 0, 0);
    // object color��ɫ
    for (int i = 1; i < num_labels; i++) {
        colors[i] = Vec3b(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
    }

    // �õ����ͼ��
    Mat dst = Mat::zeros(image.size(), image.type());
    int w = image.cols;
    int h = image.rows;
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            int label = labels.at<int>(row, col);;//��������ͨ����Ÿ���label
            if (label == 0) continue;//���Ǳ�����ͨ��
            dst.at<Vec3b>(row, col) = colors[label];//������ͨ��������ɫ����dstͼ��
        }
    }

    for (int i = 1; i < num_labels; i++) {
        Vec2d pt = centroids.at<Vec2d>(i, 0);
        //�൱��  Vec2d pt;   pt[0] = centroids.at<double>(i, 0);   pt[1] = centroids.at<double>(i, 1);
        int x = stats.at<int>(i, CC_STAT_LEFT);//stats.at<int>(��ͨ�����,״̬):
        int y = stats.at<int>(i, CC_STAT_TOP);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);
        int area = stats.at<int>(i, CC_STAT_AREA);
        //printf("area : %d, center point(%.2f, %.2f)\n", area, pt[0], pt[1]);
        rectangle(dst, Rect(x, y, width, height), Scalar(255, 0, 255), 1, 8, 0);//��Ӿ��Σ���ɫ��
    }

    imshow("dst", dst);
}