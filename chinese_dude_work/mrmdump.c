void memdump(char* fmt, char* data) {
    char* p = fmt;                  // Pointer to the instruction string (e.g., "i p c")
    char* curr = data;              // Pointer to the raw memory we are decoding

    while (*p) {                    // Loop through each instruction character
        if (*p == 'i') {
            // Treat current bytes as a 4-byte Integer
            printf("%d\n", *(int*)curr);
            curr += 4;              // Move forward 4 bytes
        } 
        else if (*p == 'p') {
            // Treat current bytes as an 8-byte Address/Pointer (Hex)
            printf("%lx\n", *(uint64*)curr);
            curr += 8;              // Move forward 8 bytes
        } 
        else if (*p == 'h') {
            // Treat current bytes as a 2-byte Short Integer
            printf("%d\n", *(short*)curr);
            curr += 2;              // Move forward 2 bytes
        } 
        else if (*p == 'c') {
            // Treat current byte as a 1-byte Character
            printf("%c\n", *curr);
            curr += 1;              // Move forward 1 byte
        } 
        else if (*p == 's') {
            // 's' means curr holds an 8-byte pointer TO a string elsewhere
            char* str_ptr = *(char**)curr;
            printf("%s\n", str_ptr);
            curr += 8;              // Move forward 8 bytes (the size of the pointer)
        } 
        else if (*p == 'S') {
            // 'S' means the string starts exactly where curr is pointing
            printf("%s\n", curr);
            // Since 'S' consumes the "rest of the data," we exit here
            return;
        }
        p++;                        // Move to the next instruction in fmt
    }
}
