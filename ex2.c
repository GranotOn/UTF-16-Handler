#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_this_big_endian()
{ // If this machine is big endian
    int x = 1;
    unsigned char *y = (unsigned char *)&x;
    return (int)(y[0] == 0);
}

int is_file_big_endian(char buffer[2])
{ // If input file is big endian
    const char UTF_16_BE[2] = {0xFE, 0xFF};
    return strncmp(UTF_16_BE, buffer, 2) == 0;
}

char getNewLine(char *iencode)
{ // return a newline designator between operating systems.
    if (strcmp(iencode, "-mac") == 0 || strcmp(iencode, "-win") == 0)
        return '\r';
    else if (strcmp(iencode, "-unix") == 0)
        return '\n';
}

void swapBytes(unsigned char buffer[2])
{
    unsigned char temp = buffer[0];
    buffer[0] = buffer[1];
    buffer[1] = temp;
}

void handleSwitches(unsigned char buffer[2], int swapFlag, int endianFlag)
{ // Swap buffer's bytes according to -swap and/or endians difference.
    if (endianFlag)
        swapBytes(buffer);

    if (swapFlag)
        swapBytes(buffer);
}

int main(int argc, char *argv[])
{
    // Insufficient parameters (no output file, no destination system).
    if (argc < 3 || argc == 4)
        return 1;

    // Parameters
    char *inputf = argv[1];                                    // input file
    char *outputf = argv[2];                                   // output file
    char *iencode, *oencode;                                   // input os, output os
    int toDifferentOS = (argc >= 4);                           // flag - should convert to different os
    int toSwap = (argc == 6 && strcmp(argv[5], "-swap") == 0); // swap flagged turned on

    if (toDifferentOS)
    {
        iencode = argv[3];                             // input os
        oencode = argv[4];                             // output os
        toDifferentOS = strcmp(iencode, oencode) != 0; // convert only if os are different
    }

    FILE *fptr = fopen(inputf, "rb"); // input file in read-binary

    if (fptr == NULL) // No input file found at that location
        return 1;

    unsigned char buffer[2];
    FILE *optr = fopen(outputf, "wb"); // output file in write-binary

    fread(buffer, 2, 1, fptr);

    // Compare endians of file and this OS to manage newline
    int file_be = is_file_big_endian(buffer);
    int this_be = is_this_big_endian();
    int should_convert_endians = (file_be != this_be);

    if (toSwap)
        swapBytes(buffer);

    fwrite(buffer, 2, 1, optr);

    while (fread(buffer, 2, 1, fptr) == 1)
    {
        int skip = 0; // Flag to skip next buffer read (converting from windows)

        if (toDifferentOS && (buffer[0] == getNewLine(iencode) || buffer[1] == getNewLine(iencode)))
        { // If this is a newline char, and we need to transpose OS
            if (strcmp(iencode, "-win") == 0)
            { // From windows

                // If it's a unix based system, write the next byte exclusively.
                // If it's a mac based system, we need to skip the next byte, so turn 'skip' on.
                if (strcmp(oencode, "-unix") == 0)
                    fread(buffer, 2, 1, fptr);
                else
                    skip = 1;
            }
            else if (strcmp(oencode, "-win") == 0)
            { // To windows

                char cr[2] = "\r", nl[2] = "\n"; // Carriage return & newline

                handleSwitches(cr, toSwap, should_convert_endians);
                handleSwitches(nl, toSwap, should_convert_endians);

                fwrite(cr, 2, 1, optr);
                fwrite(nl, 2, 1, optr);

                continue;
            }
            else
            { // Mac to unix or vice-versa
                char newline[2];

                if (strcmp(oencode, "-unix") == 0)
                    strcpy(newline, "\n");
                else
                    strcpy(newline, "\r");

                handleSwitches(newline, toSwap, should_convert_endians);

                fwrite(newline, 2, 1, optr);

                continue;
            }
        }

        if (toSwap)
            swapBytes(buffer);

        fwrite(buffer, 2, 1, optr);

        if (skip) // Skip the next 2 bytes (converting from windows)
            fread(buffer, 2, 1, fptr);
    }

    fclose(optr);
    fclose(fptr);

    return 0;
}