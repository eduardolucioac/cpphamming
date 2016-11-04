/**
    @file    hamming.cpp
    @author  Eduardo Lúcio Amorim Costa (questor)
    @date    11/02/2016
    @version 1.0

    @brief Aplica o método hamming para modificar e recuperar arquivos.

    @section DESCRIPTION

    O código de Hamming é um código de bloco linear, foi desenvolvido por 
    Richard Hamming, é utilizado no processamento de sinal e nas 
    telecomunicações. Esse aplicativo utiliza esse processo para modificar e 
    recuperar arquivos tomando seus bits de 4 em 4.

    @section LICENSE

    Apache License
    Version 2.0, January 2004
    http://www.apache.org/licenses/
    Copyright 2016 Eduardo Lúcio Amorim Costa
*/

#include <fstream>
#include <vector>
#include <iterator>

/**
    @brief  Converte um arquivo em vetor de bytes.

    @param  char* fileName: Nome do arquivo a ser convertido.
    @return std::vector<unsigned char>: Arquivo em array de bytes.
*/
std::vector<unsigned char> readFileBytes(const char* fileName){

    // NOTE: Open the file.
    std::ifstream file(fileName, std::ios::binary);

    // NOTE: Stop eating new lines in binary mode.
    file.unsetf(std::ios::skipws);

    // NOTE: Get its size.
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // NOTE: Reserve capacity.
    std::vector<unsigned char> vec;
    vec.reserve(fileSize);

    // NOTE: Read the data.
    vec.insert(vec.begin(),
               std::istream_iterator<unsigned char>(file),
               std::istream_iterator<unsigned char>());

    return vec;
}

/**
    @brief  Converter arquivo em vetor de bytes.

    @param  char* fileName: Nome do arquivo a ser escrito no diretório de 
    execução.
    @param  std::vector<unsigned char>& fileBytes: Arquivo em bytes.
    @return void.
*/  
void writeFileBytes(const char* fileName, std::vector<unsigned char>& fileBytes){
    std::ofstream file(fileName, std::ios::out|std::ios::binary);
    std::copy(fileBytes.cbegin(), fileBytes.cend(),
        std::ostream_iterator<unsigned char>(file));
}

/**
    @brief  Gerar um valor randômico dentro de range informado.

    @param  int min: Valor mínimo.
    @param  int max: Valor máximo.
    @return int: Valor randômico.
*/
int randomIntNum(int min, int max){
   static bool first = true;
   if ( first ){
      srand(time(NULL)); /**< Seeding for the first time only! */
      first = false;
   }
   return min + rand() % (max + 1 - min);
}

/**
    @brief  Aplicar paridade hamming ao arquivo.

    @param  std::vector<int>& intBitsArray: Bits do arquivo.
    @return std::vector<int>: Bits do arquivo com a paridade hamming.
*/
std::vector<int> addHammingParity(std::vector<int>& intBitsArray){

    int bit4Count=0;
    std::vector<int> xorParity;
    int e;
    int f;
    std::vector<int> bit4Array;
    std::vector<int> hammingBits;

    for(int intBitsArrayCount=0; intBitsArrayCount < intBitsArray.size(); intBitsArrayCount++){
        if(bit4Count < 4){
            bit4Array.push_back(intBitsArray[intBitsArrayCount]);
            bit4Count++;
        }
        if(bit4Count == 4){
            xorParity.clear();
            for(int z = 0; z < 4; z++){
                if(bit4Array[z] == 1){
                    /**
                        "Checa" quais são os bits ímpares (1) e suas 
                        posições respectivas na logica hamming (7~1) e guarda 
                        num array 3 à 3! By Questor
                    */
                    switch(z){
                        case 0:
                            f=7;
                            break;
                        case 1:
                            f=6;
                            break;
                        case 2:
                            f=5;
                            break;
                        case 3:
                            f=3;
                            break;
                    }
                    for(int w = 2; w >= 0; w--){
                        e = ((f >> w) & 1);
                        xorParity.push_back(e);
                    }
                }
            }

            /**
                Calcula os bits de paridade tomando o array como grupos de 
                3 e acessando os "índices" 0, 1 e 2 de cada um desses grupos de 
                três nessa ordem executando um XOR entre eles! ! By Questor
            */
            int parityBits[3]={0, 0, 0};
            if(xorParity.size() > 0){
                int octalBitPos;
                int xorNumCount;
                for(int octalBitGroupPos=0; octalBitGroupPos < 3; octalBitGroupPos++){
                    xorNumCount=0;
                    octalBitPos=octalBitGroupPos;
                    for(int xorBitsCount=0; xorBitsCount < (xorParity.size()/3); xorBitsCount++){
                        if(xorParity[octalBitPos] == 1){
                            xorNumCount++;
                        }
                        octalBitPos=octalBitPos+3;
                    }
                    if(xorNumCount%2 == 0){
                        parityBits[octalBitGroupPos]=0; // even
                    }else{
                        parityBits[octalBitGroupPos]=1; // odd
                    }
                }
            }

            /**
                Monta um grupo de 7 bits (4 de dados e 3 de paridade)! 
                By Questor
            */
            int hamming7Bits[7]={0, 0, 0, 0, 0, 0, 0};
            hamming7Bits[0] = bit4Array[0];  // D7
            hamming7Bits[1] = bit4Array[1];  // D6
            hamming7Bits[2] = bit4Array[2];  // D5
            hamming7Bits[3] = parityBits[0]; // P4
            hamming7Bits[4] = bit4Array[3];  // D3
            hamming7Bits[5] = parityBits[1]; // P2
            hamming7Bits[6] = parityBits[2]; // P1

            /**
                Esquema pseudo-randômico para inverter um dos bits a cada array 
                "hamming7Bits" criado! O próprio evento para gerar o erro tb é 
                randômico sendo uma chance em 7! By Questor
            */
            if(randomIntNum(0, 6) == 4){
                int randomPos=randomIntNum(0, 6);

                // Inverter um bit! By Questor
                hamming7Bits[randomPos]=!hamming7Bits[randomPos];
            }

            hammingBits.insert(hammingBits.end(), &hamming7Bits[0], &hamming7Bits[7]);
            bit4Count=0;
            bit4Array.clear();
        }
    }
    return hammingBits;
}

/**
    @brief  Converter bytes em bits.

    @param  std::vector<unsigned char>& bytesArray: Bytes do arquivo.
    @return std::vector<int>: Arquivo em bits.
*/
std::vector<int> bytesToBits(std::vector<unsigned char>& bytesArray){
    std::vector<int> intBitsArray;
    std::vector<int> oneByteBits;
    for(unsigned char c : bytesArray){
        for(int o=7; o >= 0; o--){
            oneByteBits.push_back(((c >> o) & 1));
        }
        intBitsArray.insert(std::end(intBitsArray), std::begin(oneByteBits), std::end(oneByteBits));
        oneByteBits.clear();
    }
    return intBitsArray;
}

/**
    @brief  Converter bits em bytes.

    @param  std::vector<int>& bitsArray: Bits do arquivo.
    @param  int byteAdjust=0 [Optional]: 1 - Completa com zeros o array de bits 
    para que seja um múltiplo de 8 (byte); 2 - Não completa com zeros. Padrão 0.
    @return std::vector<unsigned char>: Arquivo em bytes.
*/
std::vector<unsigned char> bitsToBytes(std::vector<int>& bitsArray, int byteAdjust=0){

    if(byteAdjust == 1){

        /**
            O cálculo abaixo destina-se a ajustar o tamanho do array de 
            bits ("bitsArray") para que seja igual a um múltiplo de 8 (tamanho 
            do byte) completando com zeros! A razão para isso é porque a menor 
            unidade de dados possível para c/c++ é 1 byte (8 bits)! By Questor
        */
        int intBitsArraySize=bitsArray.size();
        if((intBitsArraySize%8) > 0){
            int adjustFactor=((((int)intBitsArraySize/8)+1)*8)-intBitsArraySize;
            for(int z=0; z < adjustFactor; z++){
                bitsArray.push_back(0);
            }
        }
    }

    int bit8Count=0;
    std::vector<int> bit8Array;
    unsigned char charByte;
    std::vector<unsigned char> bytesArray;
    for(int intBitsArrayCount=0; intBitsArrayCount < bitsArray.size(); intBitsArrayCount++){
        if(bit8Count < 8){
            bit8Array.push_back(bitsArray[intBitsArrayCount]);
            bit8Count++;
        }
        if(bit8Count == 8){
            charByte = (bit8Array[7]     ) | 
                       (bit8Array[6] << 1) | 
                       (bit8Array[5] << 2) | 
                       (bit8Array[4] << 3) | 
                       (bit8Array[3] << 4) | 
                       (bit8Array[2] << 5) | 
                       (bit8Array[1] << 6) | 
                       (bit8Array[0] << 7);
            bytesArray.push_back(charByte);
            bit8Array.clear();
            bit8Count=0;
        }
    }
    return bytesArray;
}

/**
    @brief  Controlar e aplicar o método hamming.

    @param  char* flNameFrom: Arquivo para aplicar o método hamming.
    @param  char* flNameTo: Nome do arquivo à salvar após aplicar o método 
    hamming.
    @return void.
*/
void applyHamming(const char* flNameFrom, const char* flNameTo){

    std::vector<unsigned char> fileBytesInput;
    fileBytesInput=readFileBytes(flNameFrom);

    std::vector<int> fileBits;
    fileBits = bytesToBits(fileBytesInput);

    std::vector<int> bitsHamming;
    bitsHamming = addHammingParity(fileBits);

    std::vector<unsigned char> fileBytesOutput;
    fileBytesOutput=bitsToBytes(bitsHamming, 1);
    writeFileBytes(flNameTo, fileBytesOutput);
}

/**
    @brief  Corrigir aplicando o método hamming.

    @param  std::vector<int>& hamming7Bit: 7 bits na forma hamming contendo os 
    4 bits originais.
    @return std::vector<int>: 4 bits originais.
*/
std::vector<int> hammingCorrection(std::vector<int>& hamming7Bit){

    std::vector<int> xorOddBitsPos;
    std::vector<int> original4Bit;
    int e;
    int f;
    for(int hamming7BitCount=0; hamming7BitCount < hamming7Bit.size(); hamming7BitCount++){

        if(hamming7Bit[hamming7BitCount] == 1){
        /**
            "Checa" quais são os bits ímpares (1) e suas posições respectivas 
            na logica hamming (7~1) e guarda num array 3 à 3! By Questor
        */

            /**
                Ajustando a posição da lógica de cálculo (hamming) para a 
                posição no array! By Questor
            */
            f=7-hamming7BitCount;

            for(int w = 2; w >= 0; w--){
                e = ((f >> w) & 1);
                xorOddBitsPos.push_back(e);
            }
        }
    }

    /**
        Obtêm a possição do bit a ser invertido/corrigido tomando o array como 
        grupos de 3 e acessando os "índices" 0, 1 e 2 de cada um desses grupos 
        de três nessa ordem executando um XOR entre eles! By Questor
    */
    int corrPosBits[8]={0, 0, 0, 0, 0, 0, 0, 0};
    if(xorOddBitsPos.size() > 0){
        int octalBitPos;
        int xorNumCount;
        for(int octalBitGroupPos=0; octalBitGroupPos < 3; octalBitGroupPos++){
            xorNumCount=0;
            octalBitPos=octalBitGroupPos;
            for(int xorBitsCount=0; xorBitsCount < (xorOddBitsPos.size()/3); xorBitsCount++){
                if(xorOddBitsPos[octalBitPos] == 1){
                    xorNumCount++;
                }
                octalBitPos=octalBitPos+3;
            }
            if(xorNumCount%2 == 0){
                corrPosBits[(octalBitGroupPos+5)]=0; // even
            }else{
                corrPosBits[(octalBitGroupPos+5)]=1; // odd
            }
        }
    }

    // Bit (posição) à ser corrigido/invertido! By Questor
    int flipBit = (corrPosBits[7]     ) | 
                  (corrPosBits[6] << 1) | 
                  (corrPosBits[5] << 2) | 
                  (corrPosBits[4] << 3) | 
                  (corrPosBits[3] << 4) | 
                  (corrPosBits[2] << 5) | 
                  (corrPosBits[1] << 6) | 
                  (corrPosBits[0] << 7);

    /**
        Se flipBit igual á zero, então não há correções a serem feitas! 
        By Questor
    */
    if(flipBit > 0){

        // Transformar o index de correção no index do vetor! By Questor
        flipBit=7-flipBit;

        // Esquema para inverter um dos bits para a correção! By Questor
        hamming7Bit[flipBit]=!hamming7Bit[flipBit];
    }

    // original4Bit contêm os 4 bits originais! By Questor
    original4Bit.push_back(hamming7Bit[0]);
    original4Bit.push_back(hamming7Bit[1]);
    original4Bit.push_back(hamming7Bit[2]);
    original4Bit.push_back(hamming7Bit[4]);

    return original4Bit;
}

/**
    @brief  Corrigir aplicando o método hamming.

    @param  std::vector<int>& intBitsArray: Contém os bits do arquivo no 
    formato hamming. Os bits serão tomados de 7 em 7.
    @return std::vector<int>: Bits originais.
*/
std::vector<int> hammingRecover(std::vector<int>& intBitsArray){

    /**
        "intBitsArray" sempre terá um tamanho maior ou igual ao maior múltiplo 
        de 7 que "cabe" dentro dele! By Questor
    */
    int forLimit=((int)intBitsArray.size()/7)*7;

    int bit7Count=0;
    std::vector<int> bit7Array;
    std::vector<int> bit4Array;
    std::vector<int> originalBits;
    for(int intBitsArrayCount=0; intBitsArrayCount < forLimit; intBitsArrayCount++){
        if(bit7Count < 7){
            bit7Array.push_back(intBitsArray[intBitsArrayCount]);
            bit7Count++;
        }
        if(bit7Count == 7){
            bit4Array=hammingCorrection(bit7Array);
            bit7Array.clear();
            originalBits.insert(std::end(originalBits), std::begin(bit4Array), std::end(bit4Array));
            bit4Array.clear();
            bit7Count=0;
        }
    }
    return originalBits;
}

/**
    @brief  Controlar e recuperar usando o método hamming.

    @param  char* flNameFrom: Arquivo para recuperar com método hamming.
    @param  char* flNameTo: Nome do arquivo à recuperar com o método hamming.
    @return void.
*/
void removeHamming(const char* flNameFrom, const char* flNameTo){

    std::vector<unsigned char> fileBytesInput;
    fileBytesInput=readFileBytes(flNameFrom);

    std::vector<int> fileBitsHamming;
    fileBitsHamming=bytesToBits(fileBytesInput);

    std::vector<int> fileBits;
    fileBits=hammingRecover(fileBitsHamming);

    std::vector<unsigned char> fileBytesOutput;
    fileBytesOutput=bitsToBytes(fileBits);

    writeFileBytes(flNameTo, fileBytesOutput);
}

/**
    @brief  Converte string para int.

    @param  const char* str: Valor à ser convertido.
    @param  int h=0 [Optional]: ??????????????.
    @return constexpr unsigned int: Valor convertido.
*/
constexpr unsigned int str2int(const char* str, int h=0){
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

int main(int argc, char *argv[]){

    const char* actionNow = argv[1];
    /**
        actionNow values:
        0 - Aplicar hamming (já gera erro aleatório);
        1 - Recuperar do hamming.
        By Questor!
    */

    if(actionNow == NULL){
        actionNow = "";
    }

    const char* flNameFrom = argv[2];
    const char* flNameTo = argv[3];

    switch(str2int(actionNow)){
        case str2int("0"):
            // Aplicar hamming!

            printf("%s", "> ---------------------------------------------\n");
            printf("Converting file to hamming format!\n");
            printf("%s", "\n< ---------------------------------------------\n");
            applyHamming(flNameFrom, flNameTo);
            break;
        case str2int("1"):
            // Recuperar hamming!

            printf("%s", "> ---------------------------------------------\n");
            printf("Recovering file from hamming format!\n");
            printf("%s", "\n< ---------------------------------------------\n");
            removeHamming(flNameFrom, flNameTo);
            break;
        default:
            printf("%s", "> ---------------------------------------------\n");
            printf("ERROR: Invalid parameters!");
            printf("%s", "\n< ---------------------------------------------\n");
            break;
    }
    return 0;
}
