// Extract the filename from a full path (e.g., "dir/subdir/file.txt" -> "file.txt")
char* fmtname(char* path) {
    char* p;
    // Walk backward from the end of the string until we hit a '/'
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;  // Move past the '/' to get to the actual name
    return p;
}

void find(char* path, char* target) {
    char buf[512], *p;
    int fd;
    struct dirent de;               // Directory entry (holds filename and inode number)
    struct stat st;                 // File status (holds type: File vs Directory)

    if ((fd = open(path, O_RDONLY)) < 0) return; // Open the path

    if (fstat(fd, &st) < 0) {       // Get file info (is it a folder or a file?)
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:                // --- CASE 1: IT'S A FILE ---
            // Check if the filename matches our target
            if (strcmp(fmtname(path), target) == 0) printf("%s\n", path);
            break;

        case T_DIR:                 // --- CASE 2: IT'S A DIRECTORY ---
            strcpy(buf, path);      // Copy current path to buffer (e.g., "root")
            p = buf + strlen(buf);
            *p++ = '/';             // Add a slash (e.g., "root/")

            // Read the contents of the directory entry by entry
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue; // Skip empty entries

                // CRITICAL: Skip "." (current) and ".." (parent) 
                // to avoid infinite loops (recursion trap)
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;

                // Build the new path (e.g., "root/" + "subdir")
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;      // Null terminate the string

                find(buf, target);  // RECURSION: Search inside this sub-path
            }
            break;
    }
    close(fd);
}
