#ifndef __N2T_FREAK_H__
#define __N2T_FREAK_H__

#include "opencv2/core/core.hpp"

#include "opencv2/features2d/features2d.hpp"
#ifdef __cplusplus
#include <limits>

using namespace std;
using namespace cv;

namespace cv
{
class CV_EXPORTS N2TFREAK : public DescriptorExtractor
{
public:
    /** Constructor
         * @param orientationNormalized enable orientation normalization
         * @param scaleNormalized enable scale normalization
         * @param patternScale scaling of the description pattern
         * @param nbOctave number of octaves covered by the detected keypoints
         * @param selectedPairs (optional) user defined selected pairs
    */
    explicit N2TFREAK( bool orientationNormalized = true,
           bool scaleNormalized = true,
           float patternScale = 22.0f,
           int nOctaves = 4,
           const vector<int>& selectedPairs = vector<int>());
    N2TFREAK( const N2TFREAK& rhs );
    N2TFREAK& operator=( const N2TFREAK& );

    virtual ~N2TFREAK();

    /** returns the descriptor length in bytes */
    virtual int descriptorSize() const;

    /** returns the descriptor type */
    virtual int descriptorType() const;

    /** select the 512 "best description pairs"
         * @param images grayscale images set
         * @param keypoints set of detected keypoints
         * @param corrThresh correlation threshold
         * @param verbose print construction information
         * @return list of best pair indexes
    */
    vector<int> selectPairs( const vector<Mat>& images, vector<vector<KeyPoint> >& keypoints,
                      const double corrThresh = 0.7, bool verbose = true );

    //AlgorithmInfo* info() const;

    enum
    {
        NB_SCALES = 64, NB_PAIRS = 512, NB_ORIENPAIRS = 45
    };

protected:
    virtual void computeImpl( const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors ) const;
    void buildPattern();
    uchar meanIntensity( const Mat& image, const Mat& integral, const float kp_x, const float kp_y,
                         const unsigned int scale, const unsigned int rot, const unsigned int point ) const;

    bool orientationNormalized; //true if the orientation is normalized, false otherwise
    bool scaleNormalized; //true if the scale is normalized, false otherwise
    double patternScale; //scaling of the pattern
    int nOctaves; //number of octaves
    bool extAll; // true if all pairs need to be extracted for pairs selection

    double patternScale0;
    int nOctaves0;
    vector<int> selectedPairs0;

    struct PatternPoint
    {
        float x; // x coordinate relative to center
        float y; // x coordinate relative to center
        float sigma; // Gaussian smoothing sigma
    };

    struct DescriptionPair
    {
        uchar i; // index of the first point
        uchar j; // index of the second point
    };

    struct OrientationPair
    {
        uchar i; // index of the first point
        uchar j; // index of the second point
        int weight_dx; // dx/(norm_sq))*4096
        int weight_dy; // dy/(norm_sq))*4096
    };

    vector<PatternPoint> patternLookup; // look-up table for the pattern points (position+sigma of all points at all scales and orientation)
    int patternSizes[NB_SCALES]; // size of the pattern at a specific scale (used to check if a point is within image boundaries)
    DescriptionPair descriptionPairs[NB_PAIRS];
    OrientationPair orientationPairs[NB_ORIENPAIRS];
};

}//end namespace cv


#endif /* __cplusplus */

#endif