#include "Image.h"

int main(int argc, char **argv)
{
    printf("Process Started\n");

    Image airplane("../Sources/Airplane.pgm");

    airplane.read();

    int p = 11;
    int q = 53;
    int e = 41;

    int n = p * q;

    int phi = (p - 1) * (q - 1);

    int d = Utils::exposant_de_dechiffrement(e, phi);

    if (Utils::arePrime(p, q) && Utils::arePrime(e, phi))
    {
        Image binaire("../Results/binaire.pgm", airplane.H(), airplane.W());

        for (int i = 0; i < airplane.getSize(); ++i)
        {
            airplane.get(i)->getValue() < 200 ? binaire.add(new Pixel(0)) : binaire.add(new Pixel(255));
        }
        binaire.write();

        Image out = binaire.chiffrement_RSA(n, e);

        Image decrypted = out.dechiffrement_RSA(n, d);

        out.write();
        decrypted.write();

        std::cout << out.computeEntropy() << "\n";
        std::cout << decrypted.computeEntropy() << "\n";

        out.histogramme("../Results/crypted.dat");
        decrypted.histogramme("../Results/decrypted.dat");
    }
    else{
        printf("No Primes\n");
    }

    printf("Process Terminated\n");
    return 0;
}