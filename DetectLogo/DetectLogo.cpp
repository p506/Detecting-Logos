/**
 * @file MatchTemplate_Demo.cpp
 * @brief Sample code to use the function MatchTemplate
 * @author OpenCV team
 */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <vector> 
#include <Windows.h>
#include <cstdlib>

using namespace std;
using namespace cv;

//! [declare]
/// Global Variables
Mat img; Mat templ; Mat result; Mat cropImage;

const int mean_thresold = 10;
const int group_threshold = 4;
const int max_Method = 5;
struct Match
{
    Point mPosition;
    int mCount = 1;
}match;
vector <Match> matchResult;
//! [declare]

/// Function Headers
bool DetectLogo(char*, char*);
Point MatchingMethod(int, void*, int);
/**
 * @function main
 */
int main(int argc, char** argv)
{
    if (!DetectLogo(argv[1], argv[2]))
        cout << "Can't find logo in the image" << endl;
    else
        cout << "Found Logo" << endl;
}

bool DetectLogo(char* _letter, char* _logo){
    //! [load_image]
    /// Load image and template
    img = imread(_letter, IMREAD_COLOR);
    templ = imread(_logo, IMREAD_COLOR);

    //! [exception resize templ]
    while(templ.rows > img.rows || templ.cols > img.cols)
        resize(templ, templ, Size(), 0.5, 0.5);
    //! [exception resize templ]
    
    if (img.empty() || templ.empty())
    {
        cout << "Can't read one of the images" << endl;
        return false;
    }
    //! [load_image]

    //! [find correct position]
	for (int i = 0; i <= max_Method; i++)
    {
        Point pos = MatchingMethod(0, 0, i);
        int j;
        for (j = 0; j < matchResult.size(); j++) {
            if (matchResult[j].mPosition == pos) {
                matchResult[j].mCount++;
                break;
            }
        }
        if (j == matchResult.size()) {
            Match t_match;
            t_match.mPosition = pos;
            t_match.mCount = 1;
            matchResult.push_back(t_match);
        }
    }
    if (matchResult.size() >= group_threshold)
        return false;
    
    int cor_index = 0;
    int max_pick = 0;
    for (int i = 0; i < matchResult.size(); i++)
    {
        if (matchResult[i].mCount > max_pick) {
            cor_index = i;
            max_pick = matchResult[i].mCount;
        }
    }
    Point matchLoc = matchResult[cor_index].mPosition;
    //! [find correct position]
    
	//! [Crop Logo]
    Rect matchRc(matchLoc.x, matchLoc.y, templ.cols, templ.rows);
    cropImage = img(matchRc);
	//! [Crop Logo]
    
    //! [Compare Image]
	
	if (abs(mean(cropImage).val[2] - mean(templ).val[2]) 
        + abs(mean(cropImage).val[1] - mean(templ).val[1]) 
        + abs(mean(cropImage).val[0] - mean(templ).val[0]) > mean_thresold)
        return false;
    //! [Compare Image]

    return true;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
Point MatchingMethod(int, void*, int match_method)
{
    //! [create_result_matrix]
    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);
    //! [create_result_matrix]

    //! [match_template]
    /// Do the Matching and Normalize
    
    matchTemplate(img, templ, result, match_method);
    //! [match_template]

    //! [normalize]
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
    //! [normalize]

    //! [best_match]
    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    //! [best_match]

    //! [match_loc]
    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }
    //! [match_loc]
    return matchLoc;
}
