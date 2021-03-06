#ifndef HSI_CLASS
#define HSI_CLASS
#include "colorspaceinterface.h"
#include <cmath>
#include <iostream>
namespace cs{
/**
 * @brief The HSI color space
 *
 * Describe a color by its Hue,its Saturation and its Intensity
 *
 * Perceptual system
 *
 * H in [0, 2*pi]
 * S in [0,1]
 * I in [0,255]
 *
 * c1 : hue
 * c2 : saturation
 * c3 : intensity
 */
class HSI : public ColorspaceInterface{
public:
    /**
     * @brief HSI default constructor
     *
     * Default color is white (in RGB color space : 255,255,255)
     *
     * @param[in] red
     * @param[in] green
     * @param[in] blue
     */
    HSI(unsigned int red=255, unsigned int green=255, unsigned int blue=255){
        name="hsi";
        c1Min=0.;
        c2Min=0.;
        c3Min=0.;
        c1Max=2*M_PI;
        c2Max=1.;
        c3Max=255.;

        convertFromRGB(red,green,blue);

    }

    /**
    * @brief convertFromRGB convert from RGB to HSI color space
    * @param[in] r red in [0;255]
    * @param[in] g green in [0;255]
    * @param[in] b blue in [0;255]
    */
    virtual void convertFromRGB(unsigned int red, unsigned int green, unsigned int blue){
        checkRGB(red,green,blue);
        bool grayLevel=(red==green) && (green==blue);
        //store rgb color
        r=red;
        g=green;
        b=blue;

        //convert from rgb to HSI color space
        c1=M_PI;
        if(!grayLevel){
            double r_g=double(red)-double(green);
            double r_b=double(red)-double(blue);
            double g_b=double(green)-double(blue);
            double n1=0.5*(r_g+r_b);
            double n2=sqrt(pow(r_g,2)+r_b*g_b);
            c1=acos(n1/n2);
            if(blue>green){
                c1=2*M_PI-c1;
            }

        }

        double sum_rgb=double(red)+double(green)+double(blue);

        c2=0;
        if(!grayLevel){
            double min_rgb=min(red,min(green,blue));
            c2=(3.*min_rgb)/sum_rgb;
            c2=1.-c2;
        }

        c3=sum_rgb;
        c3/=3.;

    }



    /**
    * @brief distance compute normalized ([0;1]) distance between two colors
    *
    * If other color is from an other color space, start by convert it
    *
    * Distance : see A. Koschan. Dense Stereo Correspondence Using Polychromatic Block Matching.
    * In Computer Analysis of Images and Patterns, volume 719, pages 538–542,
    * Budapest, Hungary, September 1993.
    * @param[in] other an other color
    * @return normalized ([0;1]) distance
    */
    virtual double distance(ColorspaceInterface* other) const{
        HSI* o;
        if(other->getName()!=name){
            //convert to right color space
            unsigned red,green,blue;
            other->getRGB(red,green,blue);
            o=new HSI();
            o->convertFromRGB(red,green,blue);

        }else{
            o=(HSI*)other;
        }

        double theta=fabs(c1-o->c1);
        if(theta>M_PI){
            theta=2*M_PI-theta;
        }
        double d=sqrt(pow(c3-o->c3,2)+pow(c2,2)+pow(o->c2,2)-2*c2*o->c2*cos(theta));
        delete o;
        return d;
    }


    /**
    * @brief normalizedDistance compute normalized ([0;1]) distance between two colors
    *
    * If other color is from an other color space, start by convert it
    * @param[in] other an other color
    * @return distance
    */
    virtual double normalizedDistance(ColorspaceInterface* other) const{

        HSI* o;
        if(other->getName()!=name){
            //convert to right color space
            unsigned red,green,blue;
            other->getRGB(red,green,blue);
            o=new HSI();
            o->convertFromRGB(red,green,blue);

        }else{
            o=(HSI*)other;
        }

        double theta=fabs(c1-o->c1);
        if(theta>M_PI){
            theta=2*M_PI-theta;
        }
        double d1=pow(c3-o->c3,2);//in [0,255*255]
        double d2=pow(c2,2)+pow(o->c2,2)-2*c2*(o->c2)*cos(theta);//in [0,4]
        delete o;
        //normalize d1 and d2
        float w1=pow(255.,2);
        float w2=4.;
        d1/=w1;
        d2/=w2;
        //compute weighted sum of d1 and d2
        float tw=w1+w2;
        w1/=tw;
        w2/=tw;
        double  d=d1*w1+d2*w2;
        return d;
    }



};
}


#endif // HSI

