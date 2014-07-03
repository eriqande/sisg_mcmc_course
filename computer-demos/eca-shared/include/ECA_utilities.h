/* Memory Allocation Prototypes */
extern double *dvector(int Dummy, int High);
extern void free_dvector(double *Array, int Dummy, int High);
extern int *ivector(int Dummy, int High);
extern void free_ivector(int *Array, int Dummy, int High);
extern double **dmatrix(int Dummy1, int HiRows, int Dummy2, int HiCols);
extern void free_dmatrix(double **Matrix, int Dummy1, int HiRows, int Dummy2, int HiCols);
extern int **imatrix(int NumRows, int NumCols);
extern void free_imatrix(int **Matrix, int NumRows);

