cpphamming - hamming process to modify and recover files taking their bit 4 in 4
=============

What is cpphamming?
-----
The Hamming code is a linear block code developed by Richard Hamming and is used in signal processing and telecommunications. This application uses this process to modify and recover files taking their bit 4 in 4 using the hamming process.

Compile!
-----

 * Linux (The cool and good way!) =D

For Linux it is enough have g++ installed!

 * To compile and use...

```
cd "/<pat_to_cpphamming>/cpphamming/"

g++ ./hamming_enc.cpp -o hamming_enc
./hamming_enc metamorphosis.txt metamorphosis_enc.bin

g++ ./hamming_err.cpp -o hamming_err
./hamming_err metamorphosis_enc.bin metamorphosis_err.bin

g++ ./hamming_dec.cpp -o hamming_dec
./hamming_dec metamorphosis_err.bin metamorphosis_dec.txt
```

 * Windows (The "hard" way!)

For windows use Cygwin...

http://mingw-w64.org/doku.php/download/cygwin

And install as instructions here...

http://www2.warwick.ac.uk/fac/sci/moac/people/students/peter_cock/cygwin/part1

NOTE I: For "path" use "C:\cygwin64\bin;C:\cygwin64\usr\sbin" 
    (setup-x86_64.exe).
NOTE II: This version does not support "C++ 2011"/"C++11" by default, so we 
    have to add the "-std=c++11" parameter.
NOTE III: Use the command (cmd.exe) to run g++.

 * To compile and use...

```
cd "<letter>:\<pat_to_cpphamming>\cpphamming"

g++ -std=c++11 hamming_enc.cpp -o hamming_enc.exe
hamming_enc.exe metamorphosis.txt metamorphosis_enc.bin

g++ -std=c++11 hamming_err.cpp -o hamming_err.exe
hamming_err.exe metamorphosis_enc.bin metamorphosis_err.bin

g++ -std=c++11 hamming_dec.cpp -o hamming_dec.exe
hamming_dec.exe metamorphosis_err.bin metamorphosis_dec.txt
```

About!
-----

This project was created as activity on the Project Integrator I discipline taught by teacher Max Vizcarra Melgar (Master and Doctor).

Instituto de Educação Superior de Brasília IESB/Centro Universitário IESB

Contact
-----

groovimdoc@gmail.com

Brazil-DF

<img border="0" alt="GrooVim Doc" src="http://upload.wikimedia.org/wikipedia/commons/thumb/6/6d/Map_of_Brazil_with_flag.svg/180px-Map_of_Brazil_with_flag.svg.png" height="15%" width="15%"/>
