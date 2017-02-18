/**
    @file    hamming_enc.cpp
    @author  Eduardo Lúcio Amorim Costa (Questor)
    @date    11/02/2016
    @version 1.0

    @brief Applies the hamming method to modify and retrieve files.

    @section DESCRIPTION

    The Hamming code is a linear block code, was developed by Richard Hamming, 
    is used in signal processing and telecommunications. This application uses 
    this process over the bits of a file by taking them 4 out of 4.

    @section LICENSE

    Apache License
    Version 2.0, January 2004
    http://www.apache.org/licenses/
    Copyright 2016 Eduardo Lúcio Amorim Costa
*/

#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

/**
    @brief Converts a file into bytes vector.

    @param char* fileName: Name of the file to be converted.
    @return std::vector<unsigned char>: File in bytes array.
*/
std::vector<unsigned char> readFileBytes(const char* fileName){

    // Open the file.
    std::ifstream file(fileName, std::ios::binary);

    // Stop eating new lines in binary mode.
    file.unsetf(std::ios::skipws);

    // Get its size.
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Reserve capacity.
    std::vector<unsigned char> vec;
    vec.reserve(fileSize);

    // Read the data.
    vec.insert(vec.begin(),
               std::istream_iterator<unsigned char>(file),
               std::istream_iterator<unsigned char>());

    return vec;
}

/**
    @brief Convert a vector of bytes to a file.

    @param char* fileName: Name of the file to be written to the current 
        directory.
    @param std::vector<unsigned char> fileBytes: File in bytes.
    @return void.
*/  
void writeFileBytes(const char* fileName, std::vector<unsigned char> fileBytes){
    std::ofstream file(fileName, std::ios::out|std::ios::binary);
    std::copy(fileBytes.cbegin(), fileBytes.cend(),
        std::ostream_iterator<unsigned char>(file));
}

/**
    @brief Convert bytes to bits.

    @param std::vector<unsigned char> bytesArray: File Bytes.
    @return std::vector<bool>: File in bits.
*/
std::vector<bool> bytesToBits(std::vector<unsigned char> bytesArray){
    std::vector<bool> boolBitsArray;
    std::vector<bool> oneByteBits;
    int bytesArrayCount = bytesArray.size() - 1;
    for( ; bytesArrayCount > -1; bytesArrayCount--){
        for(int o=7; o >= 0; o--){
            oneByteBits.push_back(((bytesArray[bytesArrayCount] >> o) & 1));

            // [Ref.: http://www.cplusplus.com/reference/vector/vector/resize/]
            // Reduces the size of the vector to free up memory.
            bytesArray.resize(bytesArrayCount + 1);

            // [Ref.: http://stackoverflow.com/questions/16173887/how-to-reduce-the-size-of-vector-when-removing-elements-from-it]
            // [Ref.: http://stackoverflow.com/questions/1111078/reduce-the-capacity-of-an-stl-vector]
            // Ensures that the allocated vector space will also be reduced.
            std::vector<unsigned char>(bytesArray).swap(bytesArray);
        }

        // Invert then you can then put the boolBitsArray array in the correct order.
        std::reverse(oneByteBits.begin(), oneByteBits.end());

        boolBitsArray.insert(std::end(boolBitsArray), std::begin(oneByteBits), std::end(oneByteBits));
        oneByteBits.clear();
    }

    // Put in the correct order.
    std::reverse(boolBitsArray.begin(), boolBitsArray.end());

    return boolBitsArray;
}

/**
    @brief Convert bits into bytes.

    @param std::vector<bool> bitsArray: File bits.
    @param int byteAdjust=0 [Optional]: 1 - Complete with zeros the array of 
        bits to be a multiple of 8 (byte); 2 - Does not complete with zeros. Default 0.
    @return std::vector<unsigned char>: File in bytes.
*/
std::vector<unsigned char> bitsToBytes(std::vector<bool> bitsArray, int byteAdjust=0){

    if(byteAdjust == 1){

        /**
            The calculation below is to adjust the size of the array of bits 
            ("bitsArray") to be equal to a multiple of 8 (size of the byte) 
            completing with zeros! The reason for this is because the smaller
            possible data unit for c/c++ is 1 byte (8 bits).
        */
        int boolBitsArraySize=bitsArray.size();
        if((boolBitsArraySize%8) > 0){
            int adjustFactor=((((int)boolBitsArraySize/8)+1)*8)-boolBitsArraySize;
            for(int z=0; z < adjustFactor; z++){
                bitsArray.push_back(0);
            }
        }
    }

    int bit8Count=0;
    std::vector<bool> bit8Array;
    unsigned char charByte;
    std::vector<unsigned char> bytesArray;

    // The array is read back-to-back to free up memory using the "resize" 
    // and "swap" operations.
    int boolBitsArrayCount = bitsArray.size() - 1;
    for( ; boolBitsArrayCount > -1; boolBitsArrayCount--){
        if(bit8Count < 8){
            bit8Array.push_back(bitsArray[boolBitsArrayCount]);
            bit8Count++;
        }
        if(bit8Count == 8){

            // [Ref.: http://www.cplusplus.com/reference/vector/vector/resize/]
            // Reduces the size of the vector to free up memory.
            bitsArray.resize(boolBitsArrayCount + 1);

            // [Ref.: http://stackoverflow.com/questions/16173887/how-to-reduce-the-size-of-vector-when-removing-elements-from-it]
            // [Ref.: http://stackoverflow.com/questions/1111078/reduce-the-capacity-of-an-stl-vector]
            // Ensures that the allocated vector space will also be reduced.
            std::vector<bool>(bitsArray).swap(bitsArray);

            // It resets in the correct default order, because the treatment 
            // scheme below is already reversed.
            charByte = (bit8Array[0]     ) | 
                       (bit8Array[1] << 1) | 
                       (bit8Array[2] << 2) | 
                       (bit8Array[3] << 3) | 
                       (bit8Array[4] << 4) | 
                       (bit8Array[5] << 5) | 
                       (bit8Array[6] << 6) | 
                       (bit8Array[7] << 7);

            bytesArray.push_back(charByte);
            bit8Array.clear();
            bit8Count=0;
        }
    }
    std::reverse(bytesArray.begin(), bytesArray.end());
    return bytesArray;
}

/**
    @brief Apply hamming parity to file.

    @param std::vector<bool> boolBitsArray: File Bits.
    @return std::vector<bool>: File Bits with hamming par.
*/
std::vector<bool> addHammingParity(std::vector<bool> boolBitsArray){

    int bit4Count=0;
    std::vector<bool> xorParity;
    int e;
    int f;
    std::vector<bool> bit4Array;
    std::vector<bool> hammingBits;

    /**
        The array is read back-to-back to free up memory using the resize and 
        swap operations.
    */
    int boolBitsArrayCount = boolBitsArray.size() - 1;
    for( ; boolBitsArrayCount > -1; boolBitsArrayCount--){
        if(bit4Count < 4){
            bit4Array.push_back(boolBitsArray[boolBitsArrayCount]);
            bit4Count++;
        }
        if(bit4Count == 4){
            xorParity.clear();

            /**
                [Ref.: http://www.cplusplus.com/reference/vector/vector/resize/]
                Reduces the size of the vector to free up memory.
            */
            boolBitsArray.resize(boolBitsArrayCount + 1);

            /**
                [Ref.: http://stackoverflow.com/questions/16173887/how-to-reduce-the-size-of-vector-when-removing-elements-from-it]
                [Ref.: http://stackoverflow.com/questions/1111078/reduce-the-capacity-of-an-stl-vector]
                Ensures that the allocated vector space will also be reduced.
            */
            std::vector<bool>(boolBitsArray).swap(boolBitsArray);

            // Replace in the correct default order.
            std::reverse(bit4Array.begin(), bit4Array.end());

            for(int z = 0; z < 4; z++){
                if(bit4Array[z] == 1){
                    /**
                        Verifies what are the odd bits (1) and their positions 
                        in the hamming logic (7~1) and stored in an array 3 to 
                        3.
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
                Computes the parity bits by taking the array as groups of 3 
                and accessing the "indexes" 0, 1, and 2 of each of these groups 
                of three in that order by running an XOR between them.
            */
            bool parityBits[3]={0, 0, 0};
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

            // Assemble a group of 7 bits (4 data and 3 parity).
            bool hamming7Bits[7]={0, 0, 0, 0, 0, 0, 0};

            /**
                We insert in reverse order because the file array is read 
                backwards!
            */
            hamming7Bits[6] = bit4Array[0];  // D7
            hamming7Bits[5] = bit4Array[1];  // D6
            hamming7Bits[4] = bit4Array[2];  // D5
            hamming7Bits[3] = parityBits[0]; // P4
            hamming7Bits[2] = bit4Array[3];  // D3
            hamming7Bits[1] = parityBits[1]; // P2
            hamming7Bits[0] = parityBits[2]; // P1

            hammingBits.insert(hammingBits.end(), &hamming7Bits[0], &hamming7Bits[7]);
            bit4Count=0;
            bit4Array.clear();
        }
    }

    // Put in the right order.
    std::reverse(hammingBits.begin(), hammingBits.end());
    return hammingBits;
}

/**
    @brief Control and apply the hamming method.

    @param char* flNameFrom: File to apply the hamming method.
    @param char* flNameTo: Name the file to save after applying the hamming 
        method.
    @return void.
*/
void applyHamming(const char* flNameFrom, const char* flNameTo){

    /**
        The functions have been placed one inside the others so that the 
        memory is released immediately after the return of the operation. 
        Before we were duplicating the values (in memory) several times 
        because we were hosting them in variables unnecessarily!
    */
    writeFileBytes(flNameTo, 
                        bitsToBytes(
                            addHammingParity(
                                bytesToBits(
                                    readFileBytes(flNameFrom)
                                )
                            )
                        , 1)
                   );
}

int main(int argc, char *argv[]){

    const char* flNameFrom = argv[1];
    const char* flNameTo = argv[2];

    printf("%s", "> ---------------------------------------------\n");
    printf("Converting to hamming format!\n");
    printf("%s", "\n< ---------------------------------------------\n");
    applyHamming(flNameFrom, flNameTo);

    return 0;
}
