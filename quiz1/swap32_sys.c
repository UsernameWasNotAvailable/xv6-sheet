#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Helper function to convert a hexadecimal string to an unsigned integer
int htoi_safe(char *s, uint *res) {
    *res = 0;
    char *p = s;

    // Handle optional 0x or 0X prefix
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        p += 2;

    if (*p == '\0') return -1;
    
    while (*p) {
        uint d;
        if (*p >= '0' && *p <= '9') d = *p - '0';
        else if (*p >= 'a' && *p <= 'f') d = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'F') d = *p - 'A' + 10;
        else return -1; // Found an invalid hex character
        
        *res = (*res << 4) | d;
        p++;
    }
    return 0; 
}

int main(int argc, char *argv[])
{
    uint val;
    
    // Ensure the user provided exactly one argument
    if (argc != 2) {
        fprintf(2, "usage: swap32 <32-bit hex value>\n");
        exit(1);
    }

    // FIX: The test script expects the "Input:" line even for invalid arguments
    printf("Input:  %s\n", argv[1]); 

    // Convert string to numeric value
    if (htoi_safe(argv[1], &val) != 0) {
        printf("Invalid argument\n");
        exit(1);
    } 

    // Perform 32-bit byte-level endian swap using bit masking and shifts
    // Move Byte 0 to Byte 3
    uint b0 = (val & 0x000000FF) << 24; 
    // Move Byte 1 to Byte 2
    uint b1 = (val & 0x0000FF00) << 8;  
    // Move Byte 2 to Byte 1
    uint b2 = (val & 0x00FF0000) >> 8;  
    // Move Byte 3 to Byte 0
    uint b3 = (val & 0xFF000000) >> 24; 

    // Combine bytes into the final swapped value
    uint swapped = b0 | b1 | b2 | b3;

    // Print the swapped output in hexadecimal format
    printf("Output: 0x%x\n", swapped);
    
    exit(0);
}
