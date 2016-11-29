// ftable.c
// Generates a frequency table for a given text or a subset of that text.
// usage: ftable [ -v ] [ -s num ] [ -p num ] [ infile [ outfile ] ]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <math.h>

#define NUMBER_OF_LETTERS_IN_ALPHABET 26

void print_usage() {
    fprintf(stderr, "usage: ftable [ -v ] [ -s num ] [ -p num ] "
                            "[ infile [ outfile ] ]\n");
}

void print_open_output_file_error() {
    fprintf(stderr, "Failed to open up output file.\n");
}

void print_open_input_file_error() {
    fprintf(stderr, "Failed to open up input file.\n");
}
 
 //opens files from argv or stdin/stdout by default
int openFilePointers(FILE **inputFp, char *inputFileName, FILE **outputFp,
                     char *outputFileName, int hasInputFiles) {
    if(hasInputFiles) {
        if(!inputFileName) {
            print_usage();
            return 3;
        }

        *inputFp = fopen(inputFileName, "r");
        if(!*inputFp) {
            print_open_input_file_error();
            return 3;
        }

        if(outputFileName) {
            *outputFp = fopen(outputFileName, "w");
            if(!*outputFp) {
                print_open_output_file_error();
                return 3;
            }
        } else {
            *outputFp = stdout;
        }

    } else {
        *inputFp = stdin;
        *outputFp = stdout;
    }
    return 0;
}

//Writes the frequency table to the output file
void generateFrequencyTable(FILE *inputFp, FILE *outputFp, int numCharsToSkip,
                            int nthCharacterCount) {
    char c;
    int table[NUMBER_OF_LETTERS_IN_ALPHABET] = { 0 }, totalChars = 0, 
                                               characterCountIndex = 0;
    int i, t, letterCount;
    double letterAverage;
    while((c = getc(inputFp)) != EOF) {
        if(isalpha(c)) {
            if(!numCharsToSkip) {
                characterCountIndex %= nthCharacterCount;
                if(!characterCountIndex++) {
                    table[toupper(c) - 'A']++;
                    totalChars++;
                }
            } else if(numCharsToSkip) {
                numCharsToSkip--;
            }
        }
    }
    fprintf(outputFp, "Total chars: %d\n", totalChars);
    for(i = 0; i < NUMBER_OF_LETTERS_IN_ALPHABET; i++) {
        letterCount = table[i];
        if(totalChars == 0) {
            letterAverage = 0;
        } else { 
            letterAverage = (double)letterCount/totalChars * 100;
        }
        fprintf(outputFp, "%c:%9c%d (%6.2f%%) ", i + 'A', ' ', letterCount, 
                                                letterAverage);
        int starLength = (int)ceil(letterAverage);
        for(t = 0; t < starLength; t++) {
            fprintf(outputFp, "*");
        }
        fprintf(outputFp, "\n");
    }
}

int main(int argc, char *argv[])
{
    int option = 0;
    FILE *inputFp, *outputFp;
    int verbose = 0, willSkip = 0, skipN = 0, willCountEveryOther = 0,
        countEveryOther = 1;

    //The d flag performs a decoding rather than an encoding
    while ((option = getopt(argc, argv,"vs:p:")) != -1) {
        switch (option) {
            case 'v' : 
                verbose = 1;
                break;
            case 's' :
                willSkip = 1;
                skipN = atoi(optarg); 
                break;
            case 'p' :
                willCountEveryOther = 1;
                countEveryOther = atoi(optarg);
                break;
            default: 
                print_usage(); 
                return 2;
        }
    }

    int error = openFilePointers(&inputFp, argv[optind], 
                                 &outputFp, argv[optind + 1], 
                                 optind < argc);
    if(error) {
        fclose(inputFp);
        fclose(outputFp);
        return error;
    }

    generateFrequencyTable(inputFp, outputFp, skipN, countEveryOther);
    
    fclose(inputFp);
    fclose(outputFp);
    return 0;
}
