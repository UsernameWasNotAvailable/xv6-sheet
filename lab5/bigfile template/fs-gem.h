#define NDIRECT 11
#define NINDIRECT (BSIZE / sizeof(uint))
#define NDINDIRECT (NINDIRECT * NINDIRECT)

// ---------------------------------------------------------
// -> EXAM CHANGE HERE: UPDATE MAXFILE FOR HARD CAP <-
// Original lab: #define MAXFILE (NDIRECT + NINDIRECT + NDINDIRECT)
// ---------------------------------------------------------
#define MAXFILE 10000
