/**
    @file    hamming_dec.cpp
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
    @brief Correct by applying the hamming method.

    @param std::vector<bool> hamming7Bit: 7 bits in the hamming form 
        containing the 4 original bits.
    @return std::vector<bool>: 4 original bits.
*/
std::vector<bool> hammingCorrection(std::vector<bool> hamming7Bit){

    std::vector<bool> xorOddBitsPos;
    std::vector<bool> original4Bit;
    bool e;
    int f;
    for(int hamming7BitCount=0; hamming7BitCount < hamming7Bit.size(); hamming7BitCount++){

        if(hamming7Bit[hamming7BitCount] == 1){
        /**
            Verifies what are the odd bits (1) and their positions 
            in the hamming logic (7~1) and stored in an array 3 to 
            3.
        */

            /**
                Adjusting the position of the calculation logic (hamming) to 
                the position in the array.
            */
            f=7-hamming7BitCount;

            for(int w = 2; w >= 0; w--){
                e = ((f >> w) & 1);
                xorOddBitsPos.push_back(e);
            }
        }
    }

    /**
        They get the position of the bit to be inverted/corrected by taking 
        the array as groups of 3 and accessing the "indexes" 0, 1 and 2 of 
        each of these groups of three in that order and running an XOR between 
        them.
    */
    bool corrPosBits[8]={0, 0, 0, 0, 0, 0, 0, 0};
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

    // Bit (position) to be corrected/inverted (converts the position value 
    // into bits for int).
    int flipBit = (corrPosBits[7]     ) | 
                  (corrPosBits[6] << 1) | 
                  (corrPosBits[5] << 2) | 
                  (corrPosBits[4] << 3) | 
                  (corrPosBits[3] << 4) | 
                  (corrPosBits[2] << 5) | 
                  (corrPosBits[1] << 6) | 
                  (corrPosBits[0] << 7);

    /**
        If flipBit equals zero, then there are no fixes to be made.
    */
    if(flipBit > 0){

        // Transform the correction index into the vector index.
        flipBit=7-flipBit;

        // Scheme to invert one of the bits for the correction.
        hamming7Bit[flipBit]=!hamming7Bit[flipBit];
    }

    // original4Bit contains the original 4 bits. As the source reading is 
    // done from forward to back we also invert here to adjust.
    original4Bit.push_back(hamming7Bit[4]);
    original4Bit.push_back(hamming7Bit[2]);
    original4Bit.push_back(hamming7Bit[1]);
    original4Bit.push_back(hamming7Bit[0]);

    return original4Bit;
}

/**
    @brief Correct by applying the hamming method.

    @param std::vector<bool> boolBitsArray: Contains the bits of the file in 
        the hamming format. The bits will be taken 7-by-7.
    @return std::vector<bool>: Original Bits.
*/
std::vector<bool> removeHammingParity(std::vector<bool> boolBitsArray){

    int bit7Count=0;
    std::vector<bool> bit7Array;
    std::vector<bool> bit4Array;
    std::vector<bool> originalBits;

    /**
        "boolBitsArray" will always have a size greater than or equal to the 
        largest multiple of 7 that "fits" inside it.
        The array is read backwards to free up memory using the "resize" and 
        "swap" operations.
    */
    int boolBitsArrayCount = ((((int)boolBitsArray.size()/7)*7) - 1);

    for( ; boolBitsArrayCount > -1; boolBitsArrayCount--){
        if(bit7Count < 7){
            bit7Array.push_back(boolBitsArray[boolBitsArrayCount]);
            bit7Count++;
        }
        if(bit7Count == 7){

            // [Ref.: http://www.cplusplus.com/reference/vector/vector/resize/]
            // Reduces the size of the vector to free up memory.
            boolBitsArray.resize(boolBitsArrayCount + 1);

            // [Ref.: http://stackoverflow.com/questions/16173887/how-to-reduce-the-size-of-vector-when-removing-elements-from-it]
            // [Ref.: http://stackoverflow.com/questions/1111078/reduce-the-capacity-of-an-stl-vector]
            // Ensures that the allocated vector space will also be reduced.
            std::vector<bool>(boolBitsArray).swap(boolBitsArray);

            // Replaces in the correct default order.
            std::reverse(bit7Array.begin(), bit7Array.end());

            bit4Array=hammingCorrection(bit7Array);
            bit7Array.clear();
            originalBits.insert(std::end(originalBits), std::begin(bit4Array), std::end(bit4Array));
            bit4Array.clear();
            bit7Count=0;
        }
    }

    std::reverse(originalBits.begin(), originalBits.end());
    return originalBits;
}

/**
    @brief Control and retrieve using the hamming method.

    @param char* flNameFrom: File to recover with hamming method.
    @param char* flNameTo: Name of the file to be recovered with the hamming 
        method.
    @return void.
*/
void recoverHamming(const char* flNameFrom, const char* flNameTo){

    /**
        The functions have been placed one inside the others so that the 
        memory is released immediately after the return of the operation. 
        Before we were duplicating the values (in memory) several times 
        because we were hosting them in variables unnecessarily!
    */
    writeFileBytes(flNameTo, 
                        bitsToBytes(
                            removeHammingParity(
                                bytesToBits(
                                    readFileBytes(flNameFrom)
                                )
                            )
                        )
                    );

}

int main(int argc, char *argv[]){

    const char* flNameFrom = argv[1];
    const char* flNameTo = argv[2];

    printf("%s", "> ---------------------------------------------\n");
    printf("Correcting error!\n");
    printf("%s", "\n< ---------------------------------------------\n");
    recoverHamming(flNameFrom, flNameTo);

    return 0;
}
