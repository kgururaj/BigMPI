#include "bigmpi_impl.h"

/*
 * Synopsis
 *
 * a version of MPI_Type_create_hvector, except the array_of_blocklengths can
 * be larger than 32 bits
 *
 * int MPIX_Type_create_hvector_x(MPI_Count count,
 *                                MPI_Count array_of_blocklengths[],
 *                                MPI_Aint array_of_displacements[],
 *                                MPI_Datatype   oldtype,
 *                                MPI_Datatype * newtype)
 *
 *  Input Parameters
 *
 *   count                   number of blocks -- also number of entries in
 *                           array_of_displacements and array_of_blocklengths
 *                           (non-negative integer)
 *
 *   array_of_blocklengths   number of elements in each block (array of
 *                           non-negative integers)
 *
 *   array_of_displacements  byte displacement of each block (array of
 *                           integers)
 *
 *   oldtype                 old datatype (handle)
 *
 * Output Parameter
 *
 *   newtype           new datatype (handle)
 *
 */
int MPIX_Type_create_hvector_x(int count,
	MPI_Count array_of_blocklengths[], MPI_Aint array_of_displacements[],
	MPI_Datatype oldtype, MPI_Datatype * newtype)
{
    int i, ret;
    MPI_Datatype *types;
    int *blocklens;

    /* The count has to fit into MPI_Aint for BigMPI to work. */
    assert(count<bigmpi_count_max);

    types = malloc(count*sizeof(*types));
    blocklens = malloc(count*sizeof(*blocklens));

    for (i=0; i<count; i++) {
        blocklens[i] = 1;
        MPIX_Type_contiguous_x(array_of_blocklengths[i], oldtype,  &(types[i]));
    }

    ret = MPI_Type_create_struct(count, blocklens, array_of_displacements, types, newtype);

    for (i=0; i<count; i++) {
        MPI_Type_free(&(types[i]));
    }

    free(types);
    free(blocklens);

    return ret;
}
