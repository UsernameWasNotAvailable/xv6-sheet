#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// reference code given from question zzz :<
// int main(int argc, char *argv[])
// {
//  if (argc != 2) {
//  fprintf(2, "usage: swap16 <16-bit hex value>\n");
//  exit(1);
//  }

//  // Parse hexadecimal (base 16)
//  uint val = atoi(argv[1], 0, 16) & 0xFFFF;
//  // 8-bit (byte) swap of 16-bit value
//  uint high = (val & 0xFF00) >> 8;
//  uint low = (val & 0x00FF);
//  uint swapped = (low << 8) | high;
//  printf("Input: 0x%04x\n", val);
//  printf("Output: 0x%04x\n", swapped);
//  exit(0);
// }


// Helper function, uh cuz i stuck with the strtol :<<<<< 
// uint xtoi(char *s) {
//     uint res = 0;
//     // Skip 0x or 0X prefix if present
//     if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { // check for 0x or 0X
//         s += 2;
//     }
//     while (*s) { // process each character
//         uint val;
//         if (*s>='0' && *s<='9'){ // check for digit
//             val = *s-'0';
//         } 
//         else if (*s>='a' && *s<='f') { // check for lowercase hex letter
//             val = *s-'a'+10;
//         }
//         else if (*s>='A'&&*s<='F') { // check for uppercase hex letter
//             val = *s-'A'+10;
//         }
//         else{ 
//             break; 
//         }
//         res = (res << 4) | (val & 0xF); // shift left 4 bits and add new value
//         s++; // move to next character
//     }
//     return res; 
// }

// int main(int argc, char *argv[])
// {
//     if (argc != 2) { // expect exactly one argument
//         fprintf(2, "usage: swap32_sys <32-bit hex value>\n"); // changed to swap32_sys
//         exit(1);
//     }

//     // Use our helper function instead of strtol :<<<<
//     uint val = xtoi(argv[1]); // parse input string as hex argv 1 cuz argv 0 is program name

//     uint b0 = (val & 0x000000FF) << 24; // extract byte 0 and shift to byte 3 position
//     uint b1 = (val & 0x0000FF00) << 8; // extract byte 1 and shift to byte 2 position
//     uint b2 = (val & 0x00FF0000) >> 8; // extract byte 2 and shift to byte 1 position
//     uint b3 = (val & 0xFF000000) >> 24; // extract byte 3 and shift to byte 0 position

//     uint swapped = b0 | b1 | b2 | b3; // combine all bytes to get the swapped value

//     printf("Input:  0x%x\n", val);
//     printf("Output: 0x%x\n", swapped);

//     exit(0);
// }









// Your xtoi logic, updated to return 1 for success or 0 for invalid input
int xtoi(char *s, uint *res) {
    *res = 0;
    // Skip 0x or 0X prefix if present
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }

    // Check if string is empty after 0x
    if (*s == '\0') return 0;

    while (*s) {
        uint val;
        if (*s >= '0' && *s <= '9') {
            val = *s - '0';
        } 
        else if (*s >= 'a' && *s <= 'f') {
            val = *s - 'a' + 10;
        }
        else if (*s >= 'A' && *s <= 'F') {
            val = *s - 'A' + 10;
        }
        else { 
            return 0; // Invalid character found, return failure
        }
        *res = (*res << 4) | (val & 0xF);
        s++;
    }
    return 1; // Successfully parsed
}



int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(2, "usage: swap32_sys <32-bit hex value>\n");
        exit(1);
    }

    uint val;
    // FIX: Added an extra space after the colon to match the autograder
    printf("Input:  %s\n", argv[1]); 

    if (xtoi(argv[1], &val) == 0) {
        printf("Invalid argument\n");
    } else {
        uint swapped = endianswap(val);
        printf("Output: 0x%x\n", swapped);
    }

    exit(0);
}