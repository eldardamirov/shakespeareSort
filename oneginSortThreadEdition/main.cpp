//
//  main.cpp
//  oneginSortThreadEdition
//
//  Created by Эльдар Дамиров on 12.09.17.
//  Copyright © 2017 Эльдар Дамиров. All rights reserved.
//


#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <bitset>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <time.h>




struct line
    {
    std::bitset <96> lineData;
    std::string str = "";
    };


struct fileMapping
    {
    int fileDescriptor;
    size_t fileSize;
    unsigned char* dataPointer;
    };


class readFromFile
    {
    public:

        readFromFile ( char* inputFileName )
            {
            inputFileName = "input.txt";

            reset();
            init();
            }

        ~readFromFile ()
            {
            munmap ( dataPointer, fileSize );
            close ( mapping->fileDescriptor );
            free ( mapping );
            }



        char getNextChar ()
            {
            //char tempBuffer =  *( ( mapping->dataPointer ) + currentChar );
            char tempBuffer = * ( this->dataPointer + currentChar );
            currentChar++;

            return tempBuffer;
            }

        size_t getFileSize()
            {
            return fileSize;
            }
            
        bool isEnd()
            {
            return ( currentChar + 1 ) >= fileSize;
            }

        void reset()
            {
            currentChar = 0;
            }

    private:

        char* inputFileName = {};
        struct stat st;
        size_t fileSize = 0;
        int fileDescriptor = 0;
        unsigned char* dataPointer = {};
        fileMapping* mapping;
        int currentChar = 0;

        void init()
            {
            fileDescriptor = open ( "input.txt", O_RDONLY , 0 );
            if ( fileDescriptor < 0 )
                {
                printf ( "File descriptor error.\n" );
                abort();
                }


            if ( fstat ( fileDescriptor, &st ) < 0 )
                {
                printf ( "Fstat failed.\n" );
                close ( fileDescriptor );
                abort();
                }
            fileSize = ( size_t ) st.st_size;

            dataPointer = getDataPointer ( fileSize, fileDescriptor );
            mapping = createFileMapping ( fileDescriptor, dataPointer, fileSize );

            }

        unsigned char* getDataPointer ( size_t fileSize, int fileDescriptor )
            {
            unsigned char* tempDataPointer = ( unsigned char* ) mmap ( nullptr, fileSize, PROT_READ, MAP_PRIVATE, fileDescriptor, 0 );

            if ( tempDataPointer == MAP_FAILED )
                {
                printf ( "mmap failed\n" );
                close ( fileDescriptor );
                abort();
                }

            return tempDataPointer;
            }

        fileMapping* createFileMapping ( int fileDescriptor, unsigned char* dataPointer, size_t fileSize )
            {
            fileMapping* tempMapping = ( fileMapping* ) malloc ( sizeof ( fileMapping ) );

            if ( tempMapping == nullptr )
                {
                printf ( "mmap failed\n" );
                munmap ( dataPointer, fileSize );
                close ( fileDescriptor );
                abort();
                }

            tempMapping->fileDescriptor = fileDescriptor;
            tempMapping->fileSize = fileSize;
            tempMapping->dataPointer = dataPointer;

            return tempMapping;
            }

    };


class writeToFile
    {
    public:

        writeToFile ( char* fileName, size_t fileSize )
            {
            outputFileName = fileName;
            fileSizeMe = fileSize + 1;


            init();
            }


        void writeNextChar ( char inputChar )
            {
            map [ currentPosition ] = inputChar;
            currentPosition++;
            }

        ~writeToFile()
            {
            msync ( map, fileSizeMe, MS_SYNC );
            munmap ( map, fileSizeMe );
            close ( fileDescriptor );
//            msync ( map, fileSizeMe, MS_SYNC );
            }


    private:

        char* outputFileName = {};
        struct stat st;
        size_t fileSizeMe = 0;
        int fileDescriptor = 0;
        int fileDescription = 0;
        char* map;
        int currentPosition = 0;



        void init()
            {
            fileDescriptor = open ( "output.txt", O_RDWR | O_CREAT , ( mode_t ) 0600 );
            if ( fileDescriptor < 0 )
                {
                printf ( "File descriptor error.\n" );
                abort();
                }

            /*
            if ( fstat ( fileDescriptor, &st ) < 0 )
                {
                printf ( "Fstat failed.\n" );
                close ( fileDescriptor );
                abort();
                }
            */
            //fileSize = ( size_t ) st.st_size;
            //fileSize = fileSize + 1;

            fileDescription = makeFileDescription ( fileDescriptor, fileSizeMe );
            updateFileSize ( fileDescriptor, fileDescription );
            map = mmapFile ( fileDescriptor, fileSizeMe );

            }


        int makeFileDescription ( int fileDescriptor, size_t fileSize )
            {
            //int tempFileDescription = lseek ( fileDescriptor, ( fileSize - 1 ), SEEK_SET );
            int tempFileDescription = lseek ( fileDescriptor, fileSizeMe, SEEK_SET );
            if ( tempFileDescription < 0 )
                {
                close ( fileDescriptor );
                printf ( "lseek error." );
                abort();
                }

            return tempFileDescription;
            }

        void updateFileSize ( int fileDescriptor, int fileDescription )
            {
            //fileDescription = write ( fileDescriptor, "", 1 );
            //write ( fileDescriptor, "", 1 );
            if ( write ( fileDescriptor, "", 1 ) < 0 )
                {
                close ( fileDescriptor );
                printf ( "Writing null string to last file's byte failed." );
                abort();
                }
            }



        char* mmapFile ( int fileDescriptor, size_t fileSize )
            {
            char* tempMap = ( char* ) ( mmap ( 0, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fileDescriptor, 0 ) );
            if ( tempMap == MAP_FAILED )
                {
                close ( fileDescriptor );
                printf ( "File mapping failed." );
                abort();
                }

            return tempMap;
            }








    };



std::bitset <96>& operator + ( std::bitset <96>& tempBitset1, std::bitset <8>& tempBitset2 );
const bool operator > ( std::bitset <96>& tempBitset1, std::bitset <96>& tempBitset2 );
std::bitset <96>& operator + ( std::bitset <96>& tempBitset1, char& newChar );
bool comparator ( line first, line second );




/*
int main()
    {
    readFromFile inputFile ( "input.txt" );
    printf ( "%zu", inputFile.getFileSize() );
    writeToFile action ( "output.txt", inputFile.getFileSize() );

    action.writeNextChar ( 'P' );
    action.writeNextChar ( 'S' );

    return 0;
    }
*/



int main()
    {
    clock_t begin = clock();

    readFromFile inputFile ( "input.txt" );
    writeToFile outputFile ( "output.txt", inputFile.getFileSize() );

    char c = ' ';
    std::vector <line> poem;

    while ( !inputFile.isEnd() )
        {
        c = inputFile.getNextChar();
        line tmp;
        int k = 0;
        while ( c != '\n' && k < 12 )
            {
            tmp.str += c;
            if ( c >= 'A' && c <= 'Z' ) 
                {
                c = 'a' + c - 'A';
                tmp.lineData = tmp.lineData + c;
                k++;
                }
            else if ( c >= 'a' && c <= 'z' )
                {
                tmp.lineData = tmp.lineData + c;
                k++;
                }
            
            if ( !inputFile.isEnd() ) c = inputFile.getNextChar();
            else break;
            }
            
         while ( c != '\n' && !inputFile.isEnd() )
            {
            tmp.str += c;
            c = inputFile.getNextChar();
            }
            
        poem.push_back(tmp);
        }
        
    sort ( poem.begin(), poem.end(), comparator );
    
    for ( int i = 0; i < poem.size(); i++ )
        {
        //std::cout << poem [ i ].str << std::endl;
        for ( int j = 0; j < poem [ i ].str.size(); j++ )
            {
            outputFile.writeNextChar ( poem [ i ].str [ j ] );
            }
        outputFile.writeNextChar ( '\n' );
        }

    double finalTime = clock() - begin;

    printf ( "\n FINAL TIME: %f", finalTime / 1000000 );

    return 0;
    }








/*
int main()
    {
    int initialTime = time ( NULL );

    readFromFile inputFile ( "input.txt" );
    char c = ' ';
    std::vector <line> poem;

    while ( !inputFile.isEnd() )
        {
        c = inputFile.getNextChar();
        line tmp;
        int k = 0;
        while ( c != '\n' && k < 12 )
            {
            tmp.str += c;
            if ( c >= 'A' && c <= 'Z' ) 
                {
                c = 'a' + c - 'A';
                tmp.lineData = tmp.lineData + c;
                k++;
                }
            else if ( c >= 'a' && c <= 'z' )
                {
                tmp.lineData = tmp.lineData + c;
                k++;
                }
            
            if ( !inputFile.isEnd() ) c = inputFile.getNextChar();
            else break;
            }
            
         while ( c != '\n' ) 
            {
            tmp.str += c;
            c = inputFile.getNextChar();
            }
            
        poem.push_back(tmp);
        }
        
    sort ( poem.begin(), poem.end(), comparator );
    
    for ( int i = 0; i < poem.size(); i++ )
        {
        std::cout << poem [ i ].str << std::endl;
        }


    printf ( "\n FINAL TIME: %d", ( time ( NULL ) - initialTime ) );

    return 0;
    }
*/


/*
int main ()
    {
    readFromFile inputFile ( "input.txt" );
    char c = ' ';
    std::string tempLine;
    std::vector <line> poem;
    
    while ( !inputFile.isEnd() )
        {
        line tmp;
        c = inputFile.getNextChar();
        while ( c != '\n' )
            {
            tmp.str += c;
            c = inputFile.getNextChar();
            }
        int i = 0, k = 0;
        int j = tmp.str.size();
        while ( i < 12 )
            {
            if ( tmp.str [ j - k ] >= 'A' && tmp.str [ j - k ] <= 'Z' )
                {
                char f = 'a' + tmp.str [ j - k ] - 'A';
                tmp.lineData = tmp.lineData + f;
                i++;
                }
            else if ( tmp.str [ j - k ] >= 'a' && tmp.str [ j - k ] <= 'z' )
                {
                tmp.lineData = tmp.lineData + tmp.str [ j - k ];
                i++;
                }                
            k++;
            }
            poem.push_back ( tmp );
        }
        
        sort ( poem.begin(), poem.end(), comparator );
    
    for ( int i = 0; i < poem.size(); i++ )
        {
        std::cout << poem [ i ].str << std::endl;
        }
    return 0;
    }
*/



std::bitset <96>& operator + ( std::bitset <96>& tempBitset1, std::bitset <8>& tempBitset2 )
    {
    tempBitset1 = tempBitset1 <<= 8;

    for ( int i = 0; i < 8; i++ )
        {
        tempBitset1 [ i ] = tempBitset2 [ i ];
        }

    return tempBitset1;
    }
    
    

const bool operator > ( std::bitset <96>& tempBitset1, std::bitset <96>& tempBitset2 )
    {
    bool isBigger = 1;
    int index = 95;

    while ( isBigger && index >= 0 )
        {
            isBigger = ( tempBitset1 [ index ] == tempBitset2 [ index ] );
            index --;
        }

    if ( isBigger )
        {
        isBigger = false;
        }
    else
        {
        if ( tempBitset1 [ index + 1 ] )
            {
            isBigger = true;
            }
        else
            {
            isBigger = false;
            }
        }

    return isBigger;

    }


std::bitset <96>& operator + ( std::bitset <96>& tempBitset1, char& newChar )
    {
    std::bitset<8> tempBitset2 ( newChar );
    tempBitset1 = tempBitset1 + tempBitset2;
    
    return tempBitset1;
    }



bool comparator ( line first, line second )
    {
    return second.lineData > first.lineData;
    }























