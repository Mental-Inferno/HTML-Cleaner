#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXLENGTH 65535 //Maxlength of a PATH in Windows
#define MEMORY 1048576 //One megabyte worth of char storage

void AppendFile (FILE* FilePtr, char ListOfChar[MEMORY], int StopPosition);
char ReadCharFromFile (FILE* FilePtr, int OffsetFromStart);
bool DetectFileEnd (FILE* FilePtr, long int OffsetFromStart);

int main(void) {

    FILE* TempFile;
    FILE* TargetFile;
    char* TargetFileAddress = (char*)calloc(MAXLENGTH+1, sizeof(char));
    char* TempFileAddress = "TempTxtFile.txt";
    char CharacterResevoir[MEMORY];
    char CurrentChar = 'a';
    int ResevoirCounter = 0;
    int Counter = 0;
    int LengthOfAddress = 0;
    long int Cursor = -1;
    bool FileEnded = false;


    //Setting up the temporary file
    TempFile = fopen(TempFileAddress, "w+"); // Creates a blank text file where the filtered conetent is temporarily stored in the same directory as the program

    //If the file cannot be created, an error message will pop up and the program will end
    if(TempFile == NULL) { 
        printf("Error -- Temporary file cannot be created!\n");   
        exit(1);             
    }
    fclose(TempFile); //Close file for now

    //Locating target file
    printf("Please enter (without surrounding in quotations) the PATH (file address including file name) of the .txt file you want to clean\n");
    fgets(TargetFileAddress, MAXLENGTH, stdin); //Gets PATH to .txt file from user

    //Removing newline character from PATH
    Counter = 0;
    while(TargetFileAddress[Counter] != '\n') {
        Counter++;
    }
    TargetFileAddress[Counter] = '\0';

    //Array is shortened to conserve memory
    Counter = 0;
    while(TargetFileAddress[Counter] != '\0') { //Counts how many characters are in the string
        Counter++;
    }

    LengthOfAddress = Counter+1;
    TargetFileAddress = (char*)realloc(TargetFileAddress, LengthOfAddress*sizeof(char)); //Adjusts size of pointer to length of address

    TargetFile = fopen(TargetFileAddress, "r"); //Opens uner input .txt file for reading to make sure it exists
    //If the file cannot be opened, an error message will pop up and the program will end
    if(TargetFile == NULL) { 
        printf("Error -- File cannot be opened/found!\n");   
        exit(1);             
    }
    else{
        printf("File found and opened succesfully!\n");
    }
    fclose(TargetFile);

    //Add an ending sequence to the file that is not expected to be written in the responses
    TargetFile = fopen(TargetFileAddress, "a");
    CharacterResevoir[0] = '|';
    CharacterResevoir[1] = 'O';
    CharacterResevoir[2] = '_';
    CharacterResevoir[3] = 'o';
    CharacterResevoir[4] = '|';
    AppendFile(TargetFile, CharacterResevoir, 4);
    fclose(TargetFile);

    //The entire target file is copied to the temporary file character by character. Any HTML formatting enclosed and including '<' and '>' is ommitted
    printf("Starting HTML cleaning...\n");
    FileEnded = false;
    Cursor = -1;

    while (FileEnded == false) {
        ResevoirCounter = -1;
        TargetFile = fopen(TargetFileAddress, "r");

        while ((ResevoirCounter < (MEMORY-1)) && (FileEnded == false)){ //Keeps on reading characters from source and storing them in the array. Ends when array is full or end of file is reached.
            Cursor++;
            CurrentChar = ReadCharFromFile(TargetFile, Cursor);

            if (CurrentChar == '<') { //HTML formatting detected
                while (CurrentChar != '>') {//Skipping HTML formatting
                    Cursor++;
                    CurrentChar = ReadCharFromFile(TargetFile, Cursor);
                }
            }

            else if ((CurrentChar == '|') && (DetectFileEnd(TargetFile, Cursor) == true)) { //If first character of ending sequence is detected
                FileEnded = true; //Stop the read cycle of target file and write cycle to temp file
            }

            else if ((CurrentChar == '\n') && (ReadCharFromFile(TargetFile, Cursor+1) == '\n')) {//Accounts for the C program reading the enter in the text file as two newline characters
                ResevoirCounter++;
                CharacterResevoir[ResevoirCounter] = '\n';
                Cursor++; //Skips reading the next newline character
            }

            else { //Normal text is detected
                ResevoirCounter++;
                CharacterResevoir[ResevoirCounter] = CurrentChar;
            }
        }
        fclose(TargetFile); //Reading Sequence is ended

        TempFile = fopen(TempFileAddress, "a");
        AppendFile(TempFile, CharacterResevoir, ResevoirCounter); //Storing current data in resevoir into temp file
        fclose(TempFile);
    
        printf("%ld characters have been processed from the original file\n", Cursor);
    }
    TempFile = fopen(TempFileAddress, "a");//Adding ending sequence to temp file
    CharacterResevoir[0] = '|';
    CharacterResevoir[1] = 'O';
    CharacterResevoir[2] = '_';
    CharacterResevoir[3] = 'o';
    CharacterResevoir[4] = '|';
    AppendFile(TargetFile, CharacterResevoir, 4);
    fclose(TempFile); 
    printf("Cleaned data copied to temporary file\nBeginning data transfer to original file...\n");
    
    //Target file is wiped clean
    TargetFile = fopen(TargetFileAddress, "w");
    fclose(TargetFile);

    //The temporary file will be copied back over to the target file but the ending sequence will be ommitted
        FileEnded = false;
    Cursor = -1;

    while (FileEnded == false) {
        ResevoirCounter = -1;
        TempFile = fopen(TempFileAddress, "r");

        while ((ResevoirCounter < (MEMORY-1)) && (FileEnded == false)){ //Keeps on reading characters from source and storing them in the array. Ends when array is full or end of file is reached.
            Cursor++;
            CurrentChar = ReadCharFromFile(TempFile, Cursor);

            if ((CurrentChar == '|') && (DetectFileEnd(TempFile, Cursor) == true)) { //If first character of ending sequence is detected
                FileEnded = true; //Stop the read cycle of temp file and write cycle to target file
            }

            else if ((CurrentChar == '\n') && (ReadCharFromFile(TempFile, Cursor+1) == '\n')) {//Accounts for the C program reading the enter in the text file as two newline characters
                ResevoirCounter++;
                CharacterResevoir[ResevoirCounter] = '\n';
                Cursor++; //Skips reading the next newline character
            }

            else { //Normal text is detected
                ResevoirCounter++;
                CharacterResevoir[ResevoirCounter] = CurrentChar;
            }
        }
        fclose(TempFile); //Reading Sequence is ended

        TargetFile = fopen(TargetFileAddress, "a");
        AppendFile(TargetFile, CharacterResevoir, ResevoirCounter); //Storing current data in resevoir into temp file
        fclose(TempFile);
    
        printf("%ld characters have been transferred to the original file\n", Cursor);
    }
    printf("HTML cleaning complete!\n");

    //Remove temporary file and end program
    if (remove(TempFileAddress) == 0) {
        printf("Temproray file deleted successfully\n"); 
    }  
    else {
        printf("Unable to delete temporary file. File is located in the same directory as this program and is named: TempTxtFile.txt\n");
    }
    printf("Ending program...\n");
    return 0;
}

void AppendFile (FILE* FilePtr, char ListOfChar[MEMORY], int StopIndex) {

    for(int i=0; i<=StopIndex; i++){
        fflush(FilePtr);
        fprintf(FilePtr, "%c", ListOfChar[i]);
    }
    
    return;
}

char ReadCharFromFile (FILE* FilePtr, int OffsetFromStart) {
    
    char Output = 'a';

    fseek(FilePtr, OffsetFromStart, SEEK_SET);//Brings cursor to current position of interest
    fscanf(FilePtr, "%c", &Output);

    return Output;
}

bool DetectFileEnd (FILE* FilePtr, long int OffsetFromStart) {

    bool Output = false;
    char CurrentChar = 'a';
    long int Offset = OffsetFromStart;

    CurrentChar = ReadCharFromFile(FilePtr, Offset);

    //Testing for the ending sequence
    if (CurrentChar == '|') {
        Offset++;
        CurrentChar = ReadCharFromFile(FilePtr, Offset);

        if (CurrentChar == 'O') {
            Offset++;
            CurrentChar = ReadCharFromFile(FilePtr, Offset);

            if (CurrentChar == '_') {
                Offset++;
                CurrentChar = ReadCharFromFile(FilePtr, Offset);

                if (CurrentChar == 'o') {
                    Offset++;
                    CurrentChar = ReadCharFromFile(FilePtr, Offset);

                    if (CurrentChar == '|') {
                        Output = true;
                    }
                    else {
                        Output = false;
                    }
                }
                else {
                    Output = false;
                }
            }
            else {
                Output = false;
            }
        }
        else {
            Output = false;
        }
    }
    else {
        Output = false;
    }

    return Output;
}