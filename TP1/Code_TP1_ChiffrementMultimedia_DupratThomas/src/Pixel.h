#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include <vector>
#include "image_ppm.h"
#include <unistd.h>
#include <set>
#include <fstream>
#include <experimental/random>
#include "Eigen/Dense"

using namespace Eigen;

class Pixel
{
protected:
    std::string type;
    int greyValue;

public:
    Pixel()
    {
        this->type = "Pixel";
        this->greyValue = 0;
    };

    Pixel(int level)
    {
        this->type = "Pixel";
        this->greyValue = level;
    }

    Pixel(const Pixel &p2)
    {
        this->type = "Pixel";
        *this = p2;
    }

    ~Pixel(){};

    const std::string &getType()
    {
        return this->type;
    }

    const int &getValue() const { return this->greyValue; };

    void setValue(int grey) { this->greyValue = grey; };

    virtual float distance(Pixel *p2)
    {
        return sqrt(pow(this->getValue() - p2->getValue(), 2));
    };

    virtual bool hasOutOfRange()
    {
        return (this->getValue() < 0 || this->getValue() > 255);
    };

    virtual Pixel * copy(){
        Pixel * p = new Pixel(this->getValue());
        return p;
    };

    virtual void operator+=(const Pixel &p)
    {
        this->greyValue += p.getValue();
    };

    virtual void operator/=(const int &divisor)
    {
        this->greyValue /= divisor;
    };

    virtual Pixel *add(Pixel *p2)
    {
        if (this->getType().compare(p2->getType()) != 0)
        {
            std::cerr << "Can't add two difference types of Pixels.\n";
            exit(1);
        }

        Pixel *pix = new Pixel(this->getValue() + p2->getValue());
        return pix;
    }

    virtual Pixel *substract(Pixel *p2)
    {
        if (this->getType().compare(p2->getType()) != 0)
        {
            std::cerr << "Can't add two difference types of Pixels.\n";
            exit(1);
        }

        Pixel *pix = new Pixel(this->getValue() - p2->getValue());
        return pix;
    }

    virtual std::string toString()
    {
        std::string str = "GreyValue(";
        str += std::to_string(this->getValue());
        str += ")";

        return str;
    };

    virtual int R() const
    {
        return -1;
    };

    virtual int G() const
    {
        return -1;
    };

    virtual int B() const
    {
        return -1;
    };

    virtual void R(int r){};

    virtual void G(int g){};

    virtual void B(int b){};

    virtual int Y() const
    {
        return -1;
    };

    virtual int Cr() const
    {
        return -1;
    };

    virtual int Cb() const
    {
        return -1;
    };

    virtual void Y(int y){};

    virtual void Cr(int cr){};

    virtual void Cb(int cb){};

    virtual bool equals(Pixel const &p)
    {
        return this->getValue() == p.getValue();
    };
};

class Pixel_RGB : public Pixel
{
protected:
    std::array<int, 3> colors;

public:
    Pixel_RGB()
    {
        this->type = "Pixel_RGB";
        this->colors[0] = 0;
        this->colors[1] = 0;
        this->colors[2] = 0;
    };

    Pixel_RGB(int r, int g, int b)
    {
        this->type = "Pixel_RGB";
        this->colors[0] = r;
        this->colors[1] = g;
        this->colors[2] = b;
    }

    int R() const { return this->colors[0]; };
    int G() const { return this->colors[1]; };
    int B() const { return this->colors[2]; };

    void R(int r) { this->colors[0] = r; };
    void G(int g) { this->colors[1] = g; };
    void B(int b) { this->colors[2] = b; };

    float distance(Pixel *p2)
    {
        return sqrt(pow(this->R() - p2->R(), 2) + pow(this->G() - p2->G(), 2) + pow(this->B() - p2->B(), 2));
    };

    bool hasOutOfRange()
    {
        return ((this->R() < 0 || this->R() > 255) ||
                (this->G() < 0 || this->G() > 255) ||
                (this->B() < 0 || this->B() > 255));
    };

    virtual Pixel * copy(){
        Pixel * p = new Pixel_RGB(this->R(), this->G(),this->B());
        return p;
    };

    bool equals(Pixel const &p)
    {
        return this->R() == p.R() && this->G() == p.G() && this->B() == p.B();
    };

    Pixel *add(Pixel *p2)
    {
        if (this->getType().compare(p2->getType()) != 0)
        {
            std::cerr << "Can't add two difference types of Pixels.\n";
            exit(1);
        }

        Pixel *pix = new Pixel_RGB(this->R() + p2->R(),this->G() + p2->G(),this->B() + p2->B());
        return pix;
    }

    Pixel *substract(Pixel *p2)
    {
        if (this->getType().compare(p2->getType()) != 0)
        {
            std::cerr << "Can't add two difference types of Pixels.\n";
            exit(1);
        }

        Pixel *pix = new Pixel_RGB(this->R() - p2->R(),this->G() - p2->G(),this->B() - p2->B());
        return pix;
    }

    void operator+=(const Pixel &p)
    {
        this->colors[0] += p.R();
        this->colors[1] += p.G();
        this->colors[2] += p.B();
    };

    void operator-=(const Pixel &p)
    {
        this->colors[0] -= p.R();
        this->colors[1] -= p.G();
        this->colors[2] -= p.B();
    };

    void operator/=(const int &divisor)
    {
        this->colors[0] /= divisor;
        this->colors[1] /= divisor;
        this->colors[2] /= divisor;
    };

    std::string toString()
    {
        std::string str = "RGB(";
        str += std::to_string(this->R());
        str += ", ";
        str += std::to_string(this->G());
        str += ", ";
        str += std::to_string(this->B());
        str += ")";
        return str;
    };
};

class Pixel_YCrCb : public Pixel
{
protected:
    std::array<int, 3> compounds;

public:
    Pixel_YCrCb()
    {
        this->type = "Pixel_YCrCb";
        this->compounds[0] = 0;
        this->compounds[1] = 0;
        this->compounds[2] = 0;
    };

    Pixel_YCrCb(int y, int cr, int cb)
    {
        this->type = "Pixel_YCrCb";
        this->compounds[0] = y;
        this->compounds[1] = cr;
        this->compounds[2] = cb;
    }

    int Y() const { return this->compounds[0]; };
    int Cr() const { return this->compounds[1]; };
    int Cb() const { return this->compounds[2]; };

    void Y(int y) { this->compounds[0] = y; };
    void Cr(int cr) { this->compounds[1] = cr; };
    void Cb(int cb) { this->compounds[2] = cb; };

    float distance(Pixel *p2)
    {
        return sqrt(pow(this->Y() - p2->Y(), 2) + pow(this->Cr() - p2->Cr(), 2) + pow(this->Cb() - p2->Cb(), 2));
    };

    bool hasOutOfRange()
    {
        return ((this->Y() < 0 || this->Y() > 255) ||
                (this->Cr() < 0 || this->Cr() > 255) ||
                (this->Cb() < 0 || this->Cb() > 255));
    };

    virtual Pixel * copy(){
        Pixel * p = new Pixel_YCrCb(this->Y(), this->Cr(),this->Cb());
        return p;
    };

    Pixel *add(Pixel *p2)
    {
        if (this->getType().compare(p2->getType()) != 0)
        {
            std::cerr << "Can't add two difference types of Pixels.\n";
            exit(1);
        }

        Pixel *pix = new Pixel_YCrCb(this->Y() + p2->Y(),this->Cr() + p2->Cr(),this->Cb() + p2->Cb());
        return pix;
    }

    Pixel *substract(Pixel *p2)
    {
        if (this->getType().compare(p2->getType()) != 0)
        {
            std::cerr << "Can't add two difference types of Pixels.\n";
            exit(1);
        }

        Pixel *pix = new Pixel_YCrCb(this->Y() - p2->Y(),this->Cr() - p2->Cr(),this->Cb() - p2->Cb());
        return pix;
    }

    void operator+=(const Pixel &p)
    {
        this->compounds[0] += p.Y();
        this->compounds[1] += p.Cr();
        this->compounds[2] += p.Cb();
    };

    void operator-=(const Pixel &p)
    {
        this->compounds[0] -= p.Y();
        this->compounds[1] -= p.Cr();
        this->compounds[2] -= p.Cb();
    };

    void operator/=(const int &divisor)
    {
        this->compounds[0] /= divisor;
        this->compounds[1] /= divisor;
        this->compounds[2] /= divisor;
    };

    bool equals(Pixel const &p)
    {
        return this->Y() == p.Y() && this->Cr() == p.Cr() && this->Cb() == p.Cb();
    };

    std::string toString()
    {
        std::string str = "YCrCb(";
        str += std::to_string(this->Y());
        str += ", ";
        str += std::to_string(this->Cr());
        str += ", ";
        str += std::to_string(this->Cb());
        str += ")";
        return str;
    };
};

class Pixel_Converter
{
public:
    static Pixel_YCrCb *toYCrCb(Pixel *pixel)
    {
        if (pixel->getType().find("RGB") == -1)
        {
            std::cerr << "You should pass an RGB pixel.\n";
            exit(1);
        }

        Pixel_YCrCb *pixie = new Pixel_YCrCb();

        VectorXd v(3);
        v << 0, 128, 128;

        MatrixXd m(3, 3);
        m << 0.299, 0.587, 0.114,
            -0.169, -0.331, 0.500,
            0.500, -0.419, -0.081;

        VectorXd rgb(3);

        rgb << pixel->R(), pixel->G(), pixel->B();

        VectorXd ycbcr(3);

        ycbcr = v + m * rgb;

        pixie->Y((int)ycbcr(0));
        pixie->Cr((int)ycbcr(2));
        pixie->Cb((int)ycbcr(1));

        return pixie;
    }

    static Pixel_RGB *toRGB(Pixel *pixel)
    {
        Pixel_RGB *pixie = new Pixel_RGB();

        VectorXd rgb(3);
        VectorXd ycbcr(3);
        VectorXd k(3);

        MatrixXd m(3, 3);

        m << 1.000, 0.000, 1.403,
            1.000, -0.344, -0.714,
            1.000, 1.773, 0.000;

        ycbcr << pixel->Y(), pixel->Cb(), pixel->Cr();
        k << 0, 128, 128;

        rgb = m * (ycbcr - k);

        pixie->R(abs((int)rgb(0)));
        pixie->G(abs((int)rgb(1)));
        pixie->B(abs((int)rgb(2)));

        return pixie;
    }
};