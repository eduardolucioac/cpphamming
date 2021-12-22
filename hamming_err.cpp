/**
    @file    hamming_err.cpp
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
#include <time.h>

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
    @brief Generate a random value within informed range.

    @param int min: Minimum value.
    @param int max: Maximum value.
    @return int: Random value.
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
    @brief Generate error in a file according to the limitations of hamming.

    @param std::vector<bool> boolBitsArray: Contains the bits of the file in 
        the hamming format.
    @return std::vector<bool>: Original Bits.
*/
std::vector<bool> hammingError(std::vector<bool> boolBitsArray){

    /**
        "boolBitsArray" will always have a size greater than or equal to the 
        largest multiple of 7 that "fits" inside it!
    */
    int forLimit=((int)boolBitsArray.size()/7)*7;

    /**
        Recover the array from 7 to 7 and in this range generate the error!
    */
    for(int boolBitsArrayCount=0; boolBitsArrayCount < forLimit; boolBitsArrayCount+=7){

            /**
                Pseudo-random scheme to invert one of the bits to each 
                "hamming7Bits" array created! The event itself to generate the 
                error is also random being a chance in 7.
            */
            if(randomIntNum(0, 6) == 4){
                int randomPos=randomIntNum(0, 6);
                randomPos=(boolBitsArrayCount + randomPos);

                // Invert a bit.
                boolBitsArray[randomPos]=!boolBitsArray[randomPos];
            }
    }
    return boolBitsArray;
}

/**
    @brief Control and "generate erros" respect hamming method limitations.

    @param char* flNameFrom: File to "generate erros".
    @param char* flNameTo: Name of the file to be created.
    @return void.
*/
void errorHamming(const char* flNameFrom, const char* flNameTo){

    /**
        The functions have been placed one inside the others so that the 
        memory is released immediately after the return of the operation. 
        Before we were duplicating the values (in memory) several times 
        because we were hosting them in variables unnecessarily!
    */
    writeFileBytes(flNameTo, 
                        bitsToBytes(
                            hammingError(
                                bytesToBits(
                                    readFileBytes(
                                        flNameFrom
                                    )
                                )
                            )
                        )
                   );

}

int main(int argc, char *argv[]){

    const char* flNameFrom = argv[1];
    const char* flNameTo = argv[2];

    printf("%s", "> ---------------------------------------------\n");
    printf("Generating error!\n");
    printf("%s", "\n< ---------------------------------------------\n");
    errorHamming(flNameFrom, flNameTo);

    return 0;
}
