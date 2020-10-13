#include "Image.h"

/* methode 1 : rand()%256
 * methode 2 : i ^ rand()%256
 * methode 3 : ((i*accu) ^ random) % 256
 * methode 4 : ((i+accu) ^ random) % 256)
 * methode 5 : ((i+accu) ^ random*accu) % 256)
 * methode 6 : (accu ^ random*accu) % 256
 * (i ^ random*accu) % 256
 * ( (accu ^ random) ^ (multi ^ random)) % 256
 * ( (accu + random) ^ (multi + random)) % 256
 * ------------- random2 uses srand(i) ----------------- <-- It's best
 * (accu * random) + (multi + random2)) % 256 
 * (random + random2) % 256 
 * 
*/

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Not the right amount of parameters.\n";
        exit(1);
    }
    int key = std::atoi(argv[1]);

    Image source("../Sources/snake_grey.pgm");
    source.read();

    Image sortie = source.XOR(key);

    sortie.setName("../Results/snake_out.pgm");

    Image retour = sortie.XOR(key);

    retour.setName("../Results/retour.pgm");
    
    sortie.write();
    retour.write();

    std::cout << "entopie source: " << std::to_string(source.computeEntropy()) << '\n';
    std::cout << "entopie chiffree: " << std::to_string(sortie.computeEntropy()) << '\n';
    std::cout << "entopie decryptee: " << std::to_string(retour.computeEntropy()) << '\n';


    /* -------------- */


    Image secret("../Sources/10_chiffre.pgm");
    secret.read();

    std::vector<Image> images = secret.bruteXORce();

    for(int i = 0; i < 101;i++){
        std::string name = "../Results/BRUTUS/" + std::to_string(i) + ".pgm";
        images.at(i).setName(name);
        images.at(i).write();
    }

    Image OF("../Sources/0F.pgm");
    Image FO("../Sources/0F.pgm");
    OF.read();
    FO.read();
    Image diff("../Results/diffMap.pgm",OF.H(),OF.W());

    diff.buildDifferenceMap(FO, OF);

    diff.write();

    return 0;
}