#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXLENGTH 65535

void AppendFile (char* FileAddress, char CharacterToBeAdded);
char ReadCharFromFile (char* FileAddress, long int OffsetFromStart);
bool DetectFileEnd (char* FileAddress, long int OffsetFromStart);

int main(void) {

    FILE* TempFile;
    FILE* TargetFile;
    char* TargetFileAddress = (char*)calloc(MAXLENGTH+1, sizeof(char));
    char* TempFileAddress = "TempTxtFile.txt";
    char CurrentChar = 'a';
    int Counter = 0;
    int LengthOfAddress = 0;
    int WordCount = 0;
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
    AppendFile(TargetFileAddress, '|');
    AppendFile(TargetFileAddress, 'O');
    AppendFile(TargetFileAddress, '_');
    AppendFile(TargetFileAddress, 'o');
    AppendFile(TargetFileAddress, '|');

    //The entire target file is copied to the temporary file character by character. Any HTML formatting enclosed and including '<' and '>' is ommitted
    printf("Starting HTML cleaning...\n");
    FileEnded = false;
    Cursor = -1;
    WordCount = 0;
    while (FileEnded == false) {

        Cursor++;
        CurrentChar = ReadCharFromFile(TargetFileAddress, Cursor);

        if (CurrentChar == '<') { //HTML formatting detected
            while (CurrentChar != '>') {//Skipping HTML formatting
                Cursor++;
                CurrentChar = ReadCharFromFile(TargetFileAddress, Cursor);
            }
        }
        else if ((CurrentChar == '|') && (DetectFileEnd(TargetFileAddress, Cursor) == true)) { //If first character of ending sequence is detected
            AppendFile(TempFileAddress, '|');
            AppendFile(TempFileAddress, 'O');
            AppendFile(TempFileAddress, '_');
            AppendFile(TempFileAddress, 'o');
            AppendFile(TempFileAddress, '|');
            FileEnded = true;
        }
        else if ((CurrentChar == '\n') && (ReadCharFromFile(TargetFileAddress, Cursor+1) == '\n')) {//Accounts for the C program reading the enter in the text file as two newline characters
            AppendFile(TempFileAddress, '\n');
            Cursor++;
        }
        else { //Normal text is detected
            AppendFile(TempFileAddress, CurrentChar);
        }

        if ((Cursor/10001) > WordCount){
            printf("%ld characters have been transferred to the temporary file\n", Cursor);
            WordCount++;
        }

    }
    printf("Cleaned data copied to temporary file\nBeginning data transfer to original file...\n");
    
    //Target file is wiped clean
    TargetFile = fopen(TargetFileAddress, "w");
    fclose(TargetFile);

    //The temporary file will be copied back over to the target file but the ending sequence will be ommitted
    FileEnded = false;
    Cursor = -1;
    WordCount = 0;
    while (FileEnded == false) {

        Cursor++;
        CurrentChar = ReadCharFromFile(TempFileAddress, Cursor);

        if ((CurrentChar == '|') && (DetectFileEnd(TempFileAddress, Cursor) == true)) { //Ending sequence detetcted
            FileEnded = true;
        }
        else if ((CurrentChar == '\n') && (ReadCharFromFile(TempFileAddress, Cursor+1) == '\n')) {//Accounts for the C program reading the eneter in the text file as two newline characters
            AppendFile(TargetFileAddress, '\n');
            Cursor++;
        }
        else { //Normal text is detected
            AppendFile(TargetFileAddress, CurrentChar);
        }

        if ((Cursor/10001) > WordCount){
            printf("%ld characters have been transferred to the original file\n", Cursor);
            WordCount++;
        }
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

void AppendFile (char* FileAddress, char CharacterToBeAdded) {
    
    FILE* FilePtr;

    FilePtr = fopen(FileAddress, "a");
    fflush(FilePtr);
    fprintf(FilePtr, "%c", CharacterToBeAdded);
    fclose(FilePtr);

    return;
}

char ReadCharFromFile (char* FileAddress, long int OffsetFromStart) {
    
    FILE* FilePtr;
    char Output = 'a';

    FilePtr = fopen(FileAddress, "r");
    fseek(FilePtr, OffsetFromStart, SEEK_SET);//Brings cursor to current position of interest
    fscanf(FilePtr, "%c", &Output);
    fclose(FilePtr);

    return Output;
}

bool DetectFileEnd (char* FileAddress, long int OffsetFromStart) {

    bool Output = false;
    char CurrentChar = 'a';
    long int Offset = OffsetFromStart;

    CurrentChar = ReadCharFromFile(FileAddress, Offset);

    //Testing for the ending sequence
    if (CurrentChar == '|') {
        Offset++;
        CurrentChar = ReadCharFromFile(FileAddress, Offset);

        if (CurrentChar == 'O') {
            Offset++;
            CurrentChar = ReadCharFromFile(FileAddress, Offset);

            if (CurrentChar == '_') {
                Offset++;
                CurrentChar = ReadCharFromFile(FileAddress, Offset);

                if (CurrentChar == 'o') {
                    Offset++;
                    CurrentChar = ReadCharFromFile(FileAddress, Offset);

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