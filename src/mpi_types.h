#ifndef SAT_SOLVER_MPI_TYPES_H
#define SAT_SOLVER_MPI_TYPES_H

#include <mpi.h>

/**
 * This is the struct that we can use in our code
 */
struct meta {
    char message_type;
    unsigned count;
};

/**
 * inline function that handles the setup of the mpi datatype
 * PLEASE make sure that MPI_INIT was invoked before this is called
 *
 * @return the MPI_Datatype that corresponds to the struct Meta
 */
inline MPI_Datatype setup_meta_type() {
    MPI_Aint offsets[2], extent;
    MPI_Datatype meta_data_type, old_types[2];
    int block_counts[2];

    // set offsets and block counts for the char
    offsets[0] = 0;
    old_types[0] = MPI_CHAR;
    block_counts[0] = 1;

    // set offsets and block counts for the unsigned
    MPI_Type_extent(MPI_CHAR, &extent);
    offsets[1] = 4 * extent;
    old_types[1] = MPI_UNSIGNED;
    block_counts[1] = 1;

    // define the structured type and commit it
    MPI_Type_struct(2, block_counts, offsets, old_types, &meta_data_type);
    MPI_Type_commit(&meta_data_type);
    return meta_data_type;
}

#endif //SAT_SOLVER_MPI_TYPES_H
