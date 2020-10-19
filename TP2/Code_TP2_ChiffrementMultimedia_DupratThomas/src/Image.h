#ifndef Image_H
#define Image_H

#include "Pixel.h"
#define NVX_GRIS 256

class ColorCluster
{
public:
    Pixel *initial_color;
    Pixel *mean_color;
    std::vector<int> corresponding_pixels;

    ColorCluster(Pixel *pix)
    {
        this->initial_color = new Pixel_RGB(pix->R(), pix->G(), pix->B());
        delete (pix);
        this->mean_color = new Pixel_RGB(0, 0, 0);
    }

    ~ColorCluster()
    {
        delete (initial_color);
        delete (mean_color);
    }

    std::string toString()
    {
        std::string str = "\tcouleur initiale: ";
        str += this->initial_color->toString();
        str += "\n\tcouleur moyenne:  ";
        str += this->mean_color->toString();
        str += "\n\tnb pixels associés: ";
        str += std::to_string((int)this->corresponding_pixels.size());
        return str;
    }
};

class Image
{
protected:
    std::vector<Pixel *> pixels;
    std::string imageName;
    int width;
    int height;

private:
    bool hasConverged(const std::vector<ColorCluster *> &clusters)
    {
        for (ColorCluster *cluster : clusters)
        {
            if (!cluster->initial_color->equals(*(cluster->mean_color)) &&
                (cluster->initial_color->distance(cluster->mean_color) > 5))
            {
                return false;
            }
        }
        return true;
    }

    std::string etapeName(int round)
    {
        if (this->isPGM())
        {
            return "../results/temp_" + std::to_string(round) + ".pgm";
        }
        else
        {
            return "../results/temp_" + std::to_string(round) + ".ppm";
        }
    };

    void compute_K_Mean(std::vector<ColorCluster *> &clusters)
    {

        for (int i = 0; i < (int)this->getSize(); i++)
        {
            ColorCluster *closest_cluster = clusters[0];

            for (int j = 1; j < clusters.size(); j++)
            {
                if (this->pixels[i]->distance(closest_cluster->initial_color) >
                    this->pixels[i]->distance(clusters[j]->initial_color))
                {
                    closest_cluster = clusters[j];
                }
            }

            closest_cluster->corresponding_pixels.push_back(i);
            *closest_cluster->mean_color += (*this->pixels[i]);
        }

        for (int i = 0; i < clusters.size(); i++)
        {
            if ((int)clusters[i]->corresponding_pixels.size() == 0)
            {
                std::cerr << "Error\n division by 0\n";
                exit(1);
            }
            else
            {
                *clusters[i]->mean_color /= (int)clusters[i]->corresponding_pixels.size();
            }
        }
    };

    void showClusters(std::vector<ColorCluster *> &clusters)
    {
        for (int i = 0; i < clusters.size(); i++)
        {
            std::cout << "cluster n°" << std::to_string(i) << '\n';
            std::cout << "\tcouleur initiale: " << clusters[i]->initial_color->toString()
                      << "\n\tcouleur moyenne:  " << clusters[i]->mean_color->toString() << "\n";
            std::cout << "\tcluster " << i << ": " << clusters[i]->corresponding_pixels.size()
                      << " pixels.\n";
        }
    }

    void pickRandom(int nb_clusters, std::vector<ColorCluster *> &clusters)
    {
        clusters.resize(nb_clusters);
        clusters.reserve(nb_clusters);

        int accu = 0;
        while (accu < nb_clusters)
        {

            int index = std::experimental::fundamentals_v2::randint(0, this->getSize());
            Pixel *temp = new Pixel_RGB(this->pixels[index]->R(), this->pixels[index]->G(), this->pixels[index]->B());

            if (!contains(clusters, temp))
            {
                clusters[accu] = new ColorCluster(temp);
                accu++;
            }
        }
    };

    bool contains(std::vector<ColorCluster *> &clusters, Pixel *color)
    {
        if ((int)clusters.size() == 0)
        {
            return false;
        }
        for (int i = 0; i < (int)clusters.size(); i++)
        {
            if (clusters[i] != nullptr)
            {
                if (clusters[i]->initial_color->equals(*color))
                {
                    return true;
                }
            }
        }
        return false;
    }

public:
    Image(std::string name)
    {
        this->imageName = name;
        this->width = 0;
        this->height = 0;
    };

    Image(std::string name, int h, int w)
    {
        this->imageName = name;
        this->width = w;
        this->height = h;
    };

    Image(std::string name, int h, int w, std::vector<Pixel *> pix)
    {
        this->imageName = name;
        this->width = w;
        this->height = h;
        this->pixels.clear();
        this->pixels.resize(pix.size());
        for (int i = 0; i < pix.size(); i++)
        {
            this->pixels[i] = pix[i];
        }
    };

    Image(std::string name, int h, int w, std::vector<ColorCluster *> clusters)
    {
        this->imageName = name;
        this->width = w;
        this->height = h;
        this->pixels.resize(h * w);
        for (int i = 0; i < clusters.size(); i++)
        {
            for (int j = 0; j < clusters[i]->corresponding_pixels.size(); j++)
            {
                this->pixels[clusters[i]->corresponding_pixels[j]] = clusters[i]->mean_color;
            }
        }
    }

    ~Image(){};

    Pixel *operator[](const int &index)
    {
        return (this->pixels[index]);
    };

    Pixel at(int i)
    {
        return *(pixels[i]);
    };

    Pixel *get(int index)
    {
        return (this->pixels[index]);
    }

    Pixel *get(int i, int j)
    {
        return this->pixels[i * W() + j];
    }

    Pixel at(int i, int j)
    {
        return *(this->pixels[i * W() + j]);
    }

    const std::string &getName() { return this->imageName; };

    void setName(std::string name)
    {
        this->imageName = name;
    };

    const int &W() { return this->width; };
    const int &H() { return this->height; };

    void W(int w) { this->width = w; };
    void H(int h) { this->height = h; };

    int getSize() { return pixels.size(); };

    int getByteSize()
    {
        if (this->isPGM())
        {
            return this->pixels.size();
        }
        if (this->isPPM())
        {
            return 3 * this->pixels.size();
        }

        return -1;
    };

    void setSize(int size)
    {
        this->pixels.resize(size);
    }

    std::vector<Pixel *> getPixels()
    {
        return this->pixels;
    }

    bool isPPM()
    {
        return (this->getName().find(".ppm") != -1);
    };

    bool isPGM()
    {
        return (this->getName().find(".pgm") != -1);
    }

    void set(int i, Pixel *source)
    {
        delete (this->pixels[i]);
        this->pixels[i] = source;
    };

    void set(int y, int x, Pixel *source)
    {
        delete (this->pixels[y * W() + x]);
        this->pixels[y * W() + x] = source;
    };

    void add(Pixel *source)
    {
        this->pixels.push_back(source);
    }

    OCTET *toByteStream()
    {
        OCTET *IMG;
        if (isPGM())
        {
            IMG = (OCTET *)malloc(sizeof(OCTET) * this->getSize());
            int indexPixel = 0;
            for (int i = 0; i < this->getSize(); i++)
            {
                IMG[i] = this->pixels[indexPixel]->getValue();
                indexPixel++;
            }
        }
        if (isPPM())
        {
            if (get(0)->getType().find("RGB") != -1)
            {
                IMG = (OCTET *)malloc(sizeof(OCTET) * this->getByteSize());
                int indexPixel = 0;
                for (int i = 0; i < 3 * this->getSize(); i += 3)
                {
                    IMG[i] = this->pixels[indexPixel]->R();
                    IMG[i + 1] = this->pixels[indexPixel]->G();
                    IMG[i + 2] = this->pixels[indexPixel]->B();
                    indexPixel++;
                }
            }
            if (get(0)->getType().find("YCrCb") != -1)
            {
                IMG = (OCTET *)malloc(sizeof(OCTET) * this->getByteSize());
                int indexPixel = 0;
                for (int i = 0; i < 3 * this->getSize(); i += 3)
                {
                    IMG[i] = this->pixels[indexPixel]->Y();
                    IMG[i + 1] = this->pixels[indexPixel]->Cr();
                    IMG[i + 2] = this->pixels[indexPixel]->Cb();
                    indexPixel++;
                }
            }
        }
        return IMG;
    };

    void toImage(OCTET *source, int octets)
    {
        this->pixels.clear();
        this->pixels.shrink_to_fit();

        for (int i = 0; i < octets; i += 3)
        {
            this->pixels.push_back(new Pixel_RGB(source[i], source[i + 1], source[i + 2]));
        }
    };

    Image toYCrCb()
    {
        Image sortie("../results/YCrCb.ppm", this->H(), this->W());

        for (int i = 0; i < this->getSize(); i++)
        {
            sortie.add(Pixel_Converter::toYCrCb(this->get(i)));
        }
        return sortie;
    }

    Image toRGB()
    {
        Image sortie("../results/RGB.ppm", this->H(), this->W());

        for (int i = 0; i < this->getSize(); i++)
        {
            sortie.add(Pixel_Converter::toRGB(this->get(i)));
        }
        return sortie;
    }

    void read()
    {
        int h, w, totalPixels;

        if (this->isPPM())
        {
            OCTET *IMG;
            char c_name[50];

            strcpy(c_name, this->getName().c_str());

            lire_nb_lignes_colonnes_image_ppm(c_name, &h, &w);

            H(h);

            W(w);

            totalPixels = 3 * h * w;

            allocation_tableau(IMG, OCTET, totalPixels);

            lire_image_ppm(c_name, IMG, totalPixels / 3);

            for (int i = 0; i < totalPixels; i += 3)
            {
                pixels.push_back(new Pixel_RGB(IMG[i], IMG[i + 1], IMG[i + 2]));
            }

            free(IMG);
        }
        else if (this->isPGM())
        {
            OCTET *IMG;

            char c_name[50];

            strcpy(c_name, this->getName().c_str());

            lire_nb_lignes_colonnes_image_pgm(c_name, &h, &w);

            H(h);
            W(w);

            totalPixels = h * w;

            allocation_tableau(IMG, OCTET, totalPixels); // attention différent pour ppm (peut-etre devoir /3)

            lire_image_pgm(c_name, IMG, totalPixels);
            for (int i = 0; i < totalPixels; i++)
            {
                pixels.push_back(new Pixel(IMG[i]));
            }

            free(IMG);
        }
    };

    void write()
    {
        int totalPixels = 0;
        if (this->isPPM())
        {
            OCTET *IMG;

            totalPixels = 3 * H() * W();

            allocation_tableau(IMG, OCTET, totalPixels);

            char c_name[50];

            strcpy(c_name, this->getName().c_str());

            int index = 0;
            if (this->at(0).getType().find("YCrCb") != -1)
            {
                for (int i = 0; i < this->getSize(); i++)
                {
                    IMG[index] = pixels[i]->Y();
                    IMG[index + 1] = pixels[i]->Cr();
                    IMG[index + 2] = pixels[i]->Cb();
                    index += 3;
                }
            }
            if (this->at(0).getType().find("RGB") != -1)
            {
                for (int i = 0; i < this->getSize(); i++)
                {
                    IMG[index] = pixels[i]->R();
                    IMG[index + 1] = pixels[i]->G();
                    IMG[index + 2] = pixels[i]->B();
                    index += 3;
                }
            }

            ecrire_image_ppm(c_name, IMG, H(), W());

            free(IMG);
        }
        else if (this->isPGM())
        {
            OCTET *IMG;

            char c_name[50];
            strcpy(c_name, this->getName().c_str());

            allocation_tableau(IMG, OCTET, pixels.size());

            for (int i = 0; i < pixels.size(); i++)
            {
                IMG[i] = pixels[i]->getValue();
            }

            ecrire_image_pgm(c_name, IMG, H(), W());

            free(IMG);
        }
    };

    void copyPixels(Image &source)
    {
        if (this->isPGM() && !source.isPGM())
        {
            std::cerr << "Problem in pixels format\n";
            exit(1);
        }
        if (this->isPPM() && !source.isPPM())
        {
            std::cerr << "Problem in pixels format\n";
            exit(1);
        }

        this->pixels.resize(source.getSize());
        this->pixels.reserve(pixels.size());

        this->H(source.H());
        this->W(source.W());

        for (int i = 0; i < this->pixels.size(); i++)
        {
            if (this->pixels[i] != nullptr)
            {
                delete (this->pixels[i]);
            }
            this->pixels[i] = source[i]->copy();
        }
    };

    void histogramme(const std::string fileName)
    {
        if (this->at(0).getType().compare("Pixel") == 0)
        {
            int occurence[256];

            for (int i = 0; i < 256; i++)
            {
                occurence[i] = 0;
            }

            for (int i = 0; i < this->getSize(); i++)
            {
                occurence[this->pixels[i]->getValue()] += 1;
            }

            std::ofstream fileStream(fileName.c_str());

            if (fileStream)
            {
                for (int i = 0; i < 256; i++)
                {
                    fileStream << i << " " << occurence[i] << "\n";
                }
                fileStream.close();
            }
            else
            {
                std::cerr << "Impossible d'ouvrir le fichier.\n";
            }
        }
    };

    void zeroed()
    {
        if (this->isPPM())
        {
            for (int i = 0; i < pixels.size(); i++)
            {
                pixels[i]->R(0);
                pixels[i]->G(0);
                pixels[i]->B(0);
            }
        }
        else if (this->isPGM())
        {
            for (int i = 0; pixels.size(); i++)
            {
                pixels[i]->setValue(0);
            }
        }
    }

    std::vector<Image> K_Mean(int nb_clusters)
    {
        int round = 0;
        std::vector<ColorCluster *> clusters;
        std::vector<Image> etapes;

        pickRandom(nb_clusters, clusters);

        while (!hasConverged(clusters))
        {
            std::cout << "round n°" << round << "\n";
            if (round != 0)
            {
                for (ColorCluster *cluster : clusters)
                {
                    delete (cluster->initial_color);
                    cluster->initial_color = cluster->mean_color;
                    cluster->mean_color = new Pixel_RGB(0, 0, 0);
                    cluster->corresponding_pixels.clear();
                    cluster->corresponding_pixels.resize(0);
                    cluster->corresponding_pixels.shrink_to_fit();
                }
            }

            std::string img_name = etapeName(round);

            compute_K_Mean(clusters);

            Image temp(img_name, H(), W(), clusters);

            etapes.push_back(temp);
            round++;
        }

        Image temp("../results/image_finale.ppm", H(), W(), clusters);
        etapes.push_back(temp);

        return etapes;
    };

    bool contains(Pixel_RGB *color)
    {
        for (int i = 0; i < this->getSize(); i++)
        {
            if (this->pixels[i]->equals(*color))
            {
                return true;
            }
        }
        return false;
    }

    void cross_compression(Image &source)
    {
        this->pixels.clear();
        this->pixels.resize(0);
        this->pixels.shrink_to_fit();

        for (int y = 0; y < source.H(); y += 2)
        {
            for (int x = 0; x < source.W(); x += 2)
            {
                if (y == 0 && x == 0)
                {
                    Pixel *moyenne = new Pixel((source.at(y + 1, x).getValue() + source.at(y, x + 1).getValue()) / 2);

                    this->pixels.push_back(moyenne);
                }
                else if (y == source.H() - 1 && x == source.W() - 1)
                {
                    Pixel *moyenne = new Pixel((source.at(y - 1, x).getValue() + source.at(y, x - 1).getValue()) / 2);

                    this->pixels.push_back(moyenne);
                }
                else if (y == 0)
                {
                    Pixel *moyenne = new Pixel((source.at(y + 1, x).getValue() + source.at(y, x + 1).getValue() + source.at(y, x - 1).getValue()) / 3);

                    this->pixels.push_back(moyenne);
                }
                else if (x == 0)
                {
                    Pixel *moyenne = new Pixel((source.at(y + 1, x).getValue() + source.at(y - 1, x).getValue() + source.at(y, x + 1).getValue()) / 3);

                    this->pixels.push_back(moyenne);
                }
                else
                {
                    Pixel *moyenne = new Pixel((source.at(y + 1, x).getValue() + source.at(y - 1, x).getValue() + source.at(y, x + 1).getValue() + source.at(y, x - 1).getValue()) / 4);

                    this->pixels.push_back(moyenne);
                }
            }
        }
    }

    void simple_compression(Image &source)
    {
        this->pixels.clear();
        this->pixels.resize(0);
        this->pixels.shrink_to_fit();

        for (int y = 0; y < source.H(); y += 2)
        {
            for (int x = 0; x < source.W(); x += 2)
            {
                this->pixels.push_back(new Pixel(source.at(y, x).getValue()));
            }
        }
    }

    void reverse_simple_compression(Image &composante1, Image &composante2, Image &composante3, std::string type)
    {
        Image *pt = &composante1;
        if (composante2.getSize() > pt->getSize())
        {
            pt = &composante2;
        }
        if (composante3.getSize() > pt->getSize())
        {
            pt = &composante3;
        }

        this->setSize(pt->getSize());

        if (type.find("YCrCb") != -1 || type.find("ycrcb") != -1)
        {
            for (int y = 0; y < this->H(); y += 2)
            {
                for (int x = 0; x < this->W(); x += 2)
                {
                    Pixel *pix = new Pixel_YCrCb(composante1.at(y, x).getValue(),
                                                 composante2.at(y / 2, x / 2).getValue(),
                                                 composante3.at(y / 2, x / 2).getValue());

                    this->set(y, x, pix);
                    this->set(y + 1, x, pix);
                    this->set(y + 1, x + 1, pix);
                    this->set(y, x + 1, pix);
                }
            }
        }
        if (type.find("RGB") != -1 || type.find("rgb") != -1)
        {
            for (int y = 0; y < this->H(); y += 2)
            {
                for (int x = 0; x < this->W(); x += 2)
                {
                    Pixel *pix = new Pixel_RGB(composante1.at(y / 2, x / 2).getValue(),
                                               composante2.at(y, x).getValue(),
                                               composante3.at(y / 2, x / 2).getValue());

                    this->set(y, x, pix);
                    this->set(y + 1, x, pix);
                    this->set(y + 1, x + 1, pix);
                    this->set(y, x + 1, pix);
                }
            }
        }
    };

    float PSNR(Image &img)
    {
        if (this->isPPM())
        {
            if (img[0]->getType().find("RGB") != -1)
            {
                int accuR = 0;
                int accuG = 0;
                int accuB = 0;

                for (int i = 0; i < this->getSize(); i++)
                {
                    accuR += pow((this->pixels[i]->R() - img[i]->R()), 2);
                    accuG += pow((this->pixels[i]->G() - img[i]->G()), 2);
                    accuB += pow((this->pixels[i]->B() - img[i]->B()), 2);
                }

                float EQM_R = (float)accuR / (float)this->getSize();
                float EQM_G = (float)accuG / (float)this->getSize();
                float EQM_B = (float)accuB / (float)this->getSize();

                float PSNR_R = 10.0 * log10(((float)pow(255, 2) / EQM_R));
                float PSNR_G = 10.0 * log10((float)(pow(255, 2) / EQM_G));
                float PSNR_B = 10.0 * log10((float)(pow(255, 2) / EQM_B));

                return (PSNR_R + PSNR_G + PSNR_B) / 3.f;
            }

            if (img[0]->getType().find("YCrCb") != -1)
            {
                int accuY = 0;
                int accuCr = 0;
                int accuCb = 0;

                for (int i = 0; i < this->getSize(); i++)
                {
                    accuY += pow((this->pixels[i]->Y() - img[i]->Y()), 2);
                    accuCr += pow((this->pixels[i]->Cr() - img[i]->Cr()), 2);
                    accuCb += pow((this->pixels[i]->Cb() - img[i]->Cb()), 2);
                }

                float EQM_Y = (float)accuY / (float)this->getSize();
                float EQM_Cr = (float)accuCr / (float)this->getSize();
                float EQM_Cb = (float)accuCb / (float)this->getSize();

                float PSNR_Y = 10.0 * log10(((float)pow(255, 2) / EQM_Y));
                float PSNR_Cr = 10.0 * log10((float)(pow(255, 2) / EQM_Cr));
                float PSNR_Cb = 10.0 * log10((float)(pow(255, 2) / EQM_Cb));

                return (PSNR_Y + PSNR_Cr + PSNR_Cb) / 3.f;
            }
        }
        if (this->isPGM())
        {
            int accu = 0;

            for (int i = 0; i < this->getSize(); i++)
            {
                accu += pow((this->at(i).getValue() - img.at(i).getValue()), 2);
            }

            float EQM = (float)accu / (float)this->getSize();

            return 10.0 * log10(((float)pow(255, 2) / EQM));
        }
        return -1.0;
    };

    void compression_predictive()
    {
        Image predictionMap("../results/predictionMap.pgm", this->H(), this->W());
        Image differenceMap("../results/differenceMap.pgm", this->H(), this->W());

        predictionMap.buildPredictionMap(*this);

        differenceMap.buildDifferenceMap(*this, predictionMap);

        predictionMap.write();

        differenceMap.write();
    };

    void buildPredictionMap(Image &source)
    {

        for (int y = 0; y < source.H(); y++)
        {
            for (int x = 0; x < source.W(); x++)
            {
                if (y == 0 && x == 0)
                {
                    this->add(new Pixel(source.at(y, x).getValue()));
                }
                else if (y == 0)
                {
                    this->add(new Pixel(source.at(y, x - 1).getValue()));
                }
                else if (x == 0)
                {
                    this->add(new Pixel(source.at(y - 1, x).getValue()));
                }
                else
                {
                    //Pixel *current = new Pixel(0.25 * (3 * (source.at(y, x - 1).getValue() + source.at(y - 1, x).getValue()) - 2 * source.at(y - 1, x - 1).getValue()));
                    Pixel *current = new Pixel(0.5 * (source.at(y, x - 1).getValue() + source.at(y - 1, x).getValue()));
                    this->add(current);
                }
            }
        }
    };

    void buildDifferenceMap(Image &source, Image &predict)
    {
        for (int y = 0; y < source.H(); y++)
        {
            for (int x = 0; x < source.W(); x++)
            {
                if (y == 0 && x == 0)
                {
                    this->add(new Pixel(source.at(y, x).getValue()));
                }
                else
                {
                    int difference = source.at(y, x).getValue() - predict.at(y, x).getValue();
                    difference += 128;
                    this->add(new Pixel(difference));
                }
            }
        }
    };

    void returnFromDiff(Image &diff)
    {
        Image temp("../results/temp.pgm", diff.H(), diff.W());
        Image A("../results/A.pgm", diff.H(), diff.W());

        for (int y = 0; y < diff.H(); y++)
        {
            for (int x = 0; x < diff.W(); x++)
            {
                int valeur_difference = diff.at(y, x).getValue();
                if (y == 0 && x == 0)
                {
                    temp.add(new Pixel(valeur_difference));
                }
                else if (y == 0)
                {
                    temp.add(new Pixel(valeur_difference + diff.at(y, x - 1).getValue()));
                }
                else if (x == 0)
                {
                    temp.add(new Pixel(valeur_difference + diff.at(y - 1, x).getValue()));
                }
                else
                {
                    Pixel *current = new Pixel(valeur_difference + 0.25 * (3 * (diff.at(y, x - 1).getValue() + diff.at(y - 1, x).getValue()) - 2 * diff.at(y - 1, x - 1).getValue()));
                    //Pixel * current = new Pixel( 0.5 * (diff.at(y,x-1).getValue() + diff.at(y-1,x).getValue()));
                    temp.add(current);
                }
            }
        }

        A.buildPredictionMap(temp);

        temp.write();
        A.write();
    };

    Image Haarr(std::string name)
    {

        Image sortie(name, H(), W());
        sortie.copyPixels(*this);
        /*
        Image haut_gauche("../results/haut-gauche.pgm", H() / 4, W() / 4);
        Image haut_droite("../results/haut-droite.pgm", H() / 4, W() / 4);
        Image bas_gauche("../results/bas-gauche.pgm", H() / 4, W() / 4);
        Image bas_droite("../results/bas-droit.pgm", H() / 4, W() / 4);

        OCTET *_source = toByteStream();

        OCTET _haut_gauche[getSize() / 2];
        OCTET _haut_droite[getSize() / 2];
        OCTET _bas_gauche[getSize() / 2];
        OCTET _bas_droite[getSize() / 2];

        for (int i = 0; i < H() / 2; i++)
            for (int j = 0; j < W() / 2; j++)
                _haut_gauche[i * W() / 2 + j] = _source[i * W() + j];

        for (int i = 0; i < H() / 2; i++)
            for (int j = 0; j < W() / 2; j++)
                _haut_droite[i * W() / 2 + j] = _source[(i + H() / 2) * W() + j];

        for (int i = 0; i < H() / 2; i++)
            for (int j = 0; j < W() / 2; j++)
                _bas_gauche[i * W() / 2 + j] = _source[i * W() + (j + W() / 2)];

        for (int i = 0; i < H() / 2; i++)
            for (int j = 0; j < W() / 2; j++)
                _bas_droite[i * W() / 2 + j] = _source[(i + H() / 2) * W() + (j + W() / 2)];

        for (int i = 0; i < getSize() / 2; i++)
        {
            haut_gauche.add(new Pixel(_haut_gauche[i]));
            haut_droite.add(new Pixel(_haut_droite[i]));
            bas_gauche.add(new Pixel(_bas_gauche[i]));
            bas_droite.add(new Pixel(_bas_droite[i]));
        }
        haut_gauche.write();
        haut_droite.write();
        bas_gauche.write();
        bas_droite.write();*/

        Image BF("../results/BF.pgm", H() / 2, W() / 2);
        Image MFV("../results/MFV.pgm", H() / 2, W() / 2);
        Image MFH("../results/MFH.pgm", H() / 2, W() / 2);
        Image HF("../results/HF.pgm", H() / 2, W() / 2);

        for (int y = 0; y < H() - 1; y += 2)
        {
            for (int x = 0; x < W() - 1; x += 2)
            {
                float A = (float)get(y, x)->getValue();
                float B = (float)get(y, x + 1)->getValue();
                float C = (float)get(y + 1, x)->getValue();
                float D = (float)get(y + 1, x + 1)->getValue();

                int _BF = (int)(0.25 * (A + B + C + D));
                int _MFV = (int)(0.25 * ((A + B) - (C + D)));
                int _MFH = (int)(0.25 * ((A - B) + (C - D)));
                int _HF = (int)(0.25 * (A - B - C - D));

                _MFV += 128;
                _MFH += 128;

                BF.add(new Pixel(_BF));
                MFV.add(new Pixel(_MFV));
                MFH.add(new Pixel(_MFH));
                HF.add(new Pixel(128));
            }
        }

        /*for (int i = 0; i < getSize() / 4; i++)
        {
            if(BF.get(i)->hasOutOfRange()){
                std::cout << "BF: " << BF.get(i)->toString() << "\n";
            }
            if(MFV.get(i)->hasOutOfRange()){
                std::cout << "MFV: " << MFV.get(i)->toString() << "\n";
            }
            if (MFH.get(i)->hasOutOfRange())
            {
                std::cout << "MFH: " << MFH.get(i)->toString() << "\n";
            }
            if (HF.get(i)->hasOutOfRange())
            {
                std::cout << "HF: " << HF.get(i)->toString() << "\n";
            }
        }*/

        for (int y = 0; y < H(); y++)
        {
            for (int x = 0; x < W(); x++)
            {
                if (y < H() / 2 && x < W() / 2)
                    sortie.set(y, x, BF.get(y, x)->copy());

                if (y < H() / 2 && x >= W() / 2)
                    sortie.set(y, x, MFV.get(y, x - (W() / 2))->copy());

                if (y >= H() / 2 && x < W() / 2)
                    sortie.set(y, x, MFH.get(y - (H() / 2), x)->copy());

                if (y >= H() / 2 && x >= W() / 2)
                    sortie.set(y, x, HF.get(y - (H() / 2), x - (W() / 2))->copy());
            }
        }

        return sortie;
    }

    Image recompo()
    {
        Image recompo("../results/recompo.pgm", H(), W());
        recompo.copyPixels(*this);

        Image recompo_BF("../results/recompo_BF.pgm", H() / 2, W() / 2);
        Image recompo_MFV("../results/recompo_MFV.pgm", H() / 2, W() / 2);
        Image recompo_MFH("../results/recompo_MFH.pgm", H() / 2, W() / 2);
        Image recompo_HF("../results/recompo_HF.pgm", H() / 2, W() / 2);

        for (int y = 0; y < H(); y++)
        {
            for (int x = 0; x < W(); x++)
            {
                if (y < H() / 2 && x < W() / 2)
                    recompo_BF.add(this->get(y, x)->copy());

                if (y < H() / 2 && x >= W() / 2)
                    recompo_MFV.add(this->get(y, x)->copy());

                if (y >= H() / 2 && x < W() / 2)
                    recompo_MFH.add(this->get(y, x)->copy());

                if (y >= H() / 2 && x >= W() / 2)
                    recompo_HF.add(this->get(y, x)->copy());
            }
        }

        int accu = 0;

        for (int y = 0; y < H() - 1; y += 2)
        {
            for (int x = 0; x < W() - 1; x += 2)
            {
                Pixel *mean = new Pixel(recompo_BF.get(accu)->getValue() + 0.143 * ((float)recompo_MFV.get(accu)->getValue() - 128.0) + 0.143 * ((float)recompo_MFH.get(accu)->getValue() - 128.0));

                recompo.set(y, x, mean);

                recompo.set(y, x + 1, mean);

                recompo.set(y + 1, x, mean);

                recompo.set(y + 1, x + 1, mean);

                accu++;
            }
        }

        return recompo;
    }

    Image XOR(int key)
    {
        Image sortie("../Results/XOR_out.pgm", H(), W());

        if (key < 0 || key > 100)
        {
            std::cerr << "Value out of bounds.\n";
            exit(1);
        }
        srand(key);
        int random = rand();

        std::vector<int> sequence;
        for (int i = 0; i < getSize(); i++)
        {
            srand(i);
            int random2 = rand();
            sequence.push_back((random + random2) % 256);
        }

        for (int i = 0; i < getSize(); i++)
        {
            Pixel *p = new Pixel(abs(sequence.at(i)) ^ abs(get(i)->getValue()));
            sortie.add(p);
        }

        return sortie;
    };

    float computeEntropy()
    {
        std::vector<float> probas;

        float sum = 0.f;

        for(int i = 0; i < NVX_GRIS;++i){
            probas.push_back(0.f);
        }

        for(int i = 0; i < getSize();++i){
            probas[abs(get(i)->getValue())] += 1.f;
        }

        float size = (float) getSize();

        for(int i = 0; i < NVX_GRIS;i++){
            float realProba = abs(probas[i] / size);
            if(realProba != 0.000000){
                sum += (realProba * log2(realProba));
            }
        }
        
        return sum *= -1.f;
    };

    std::vector<Image> bruteXORce()
    {
        float min = std::numeric_limits<float>::infinity();
        int goodKey = 0;
        std::vector<Image> images;
        for (int i = 0; i < 101; i++)
        {
            images.push_back(XOR(i));
        }
        return images;
    }

    /* normalement pour obtenir e on ferait 
     * std::vector<int> exposantsDeChiffrement = exposants_de_chiffrement(phi);
     * srand(time(NULL));
     * int e = exposantsDeChiffrement[rand()%exposantsDeChiffrement.size()];
     * mais la on va faire e = 17 pour le tp
     */
    Image chiffrement_RSA(int n, int e)
    {
        Image out("../Results/RSA_encrypted.pgm", H(), W());

        for(int i = 0; i < getSize();i++){
            int grayValue = Utils::exponentiation_modulaire(get(i)->getValue(),e,n);
            out.add(new Pixel(grayValue));
        }

        return out;
    }

    Image dechiffrement_RSA(int n, int d){

        Image out("../Results/RSA_decrypted.pgm",H(),W());

        for(int i = 0; i < getSize();i++){
            int grayValue = Utils::exponentiation_modulaire(get(i)->getValue(),d,n);
            out.add(new Pixel(grayValue));
        }

        return out;

    }
};

#endif