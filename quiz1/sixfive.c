#include "kernel/types.h"
#include "user/user.h"

// Helper function to validate and print the buffer
void check_and_print(char *buf, int len, int is_numeric) {
  if (len > 0 && is_numeric && len <= 3) {
    buf[len] = '\0';
    int only_five_six = 1;
    for (int k = 0; k < len; k++) {
      if (buf[k] != '5' && buf[k] != '6') {
        only_five_six = 0;
        break;
      }
    }
    if (only_five_six) {
      printf("%s\n", buf);
    }
  }
}

int main(int argc, char* argv[]) {
  char c;
  char buffer[32];
  int i = 0;
  int is_numeric = 1;
  int fd;
  char *sep = " -\r\t\n./,"; // Separators that define word boundaries

  for (int j = 1; j < argc; j++) {
    if ((fd = open(argv[j], 0)) < 0) {
      printf("Error opening file\n");
      continue;
    }

    while (read(fd, &c, 1) > 0) {
      if (strchr(sep, c)) {
        // We hit a separator, check the word we just finished
        check_and_print(buffer, i, is_numeric);
        // Reset for next word
        i = 0;
        is_numeric = 1;
      } else if (c >= '0' && c <= '9') {
        if (i < 31) {
          buffer[i++] = c;
        }
      } else {
        // Word contains non-digit characters
        is_numeric = 0;
      }
    }
    
    // --- CRITICAL FIX FOR MISSING 565 ---
    // If the file ends without a separator, process the final word in the buffer
    check_and_print(buffer, i, is_numeric);

    close(fd);
    // Reset indices before moving to the next file
    i = 0;
    is_numeric = 1;
  }
  exit(0);
}
