#include "bigmpi_impl.h"

size_t get_MPI_size(MPI_Datatype datatype, size_t count)
{
  int element_size = 0;
  MPI_Type_size(datatype, &element_size);
  size_t total_size = count*((size_t)element_size);
  return total_size;
}

int MPIX_Send_x(const void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
    int rc = MPI_SUCCESS;
    size_t total_size = get_MPI_size(datatype, count);
    if (likely (total_size <= bigmpi_int_max )) {
        rc = MPI_Send(buf, (int)count, datatype, dest, tag, comm);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(total_size, MPI_UINT8_T, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Send(buf, 1, newtype, dest, tag, comm);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Recv_x(void *buf, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
    int rc = MPI_SUCCESS;
    size_t total_size = get_MPI_size(datatype, count);
    if (likely (total_size <= bigmpi_int_max )) {
        rc = MPI_Recv(buf, (int)count, datatype, source, tag, comm, status);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(total_size, MPI_UINT8_T, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Recv(buf, 1, newtype, source, tag, comm, status);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Isend_x(const void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request * request)
{
    int rc = MPI_SUCCESS;

    size_t total_size = get_MPI_size(datatype, count);
    if (likely (total_size <= bigmpi_int_max )) {
        rc = MPI_Isend(buf, (int)count, datatype, dest, tag, comm, request);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(total_size, MPI_UINT8_T, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Isend(buf, 1, newtype, dest, tag, comm, request);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Irecv_x(void *buf, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request * request)
{
    int rc = MPI_SUCCESS;

    size_t total_size = get_MPI_size(datatype, count);
    if (likely (total_size <= bigmpi_int_max )) {
        rc = MPI_Irecv(buf, (int)count, datatype, source, tag, comm, request);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(total_size, MPI_UINT8_T, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Irecv(buf, 1, newtype, source, tag, comm, request);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Sendrecv_x(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype, int dest, int sendtag,
                    void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int source, int recvtag,
                    MPI_Comm comm, MPI_Status *status)
{
    int rc = MPI_SUCCESS;

    if (likely (sendcount <= bigmpi_int_max && recvcount <= bigmpi_int_max )) {
        rc = MPI_Sendrecv(sendbuf, (int)sendcount, sendtype, dest, sendtag,
                          recvbuf, (int)recvcount, recvtype, source, recvtag,
                          comm, status);
    } else if (sendcount <= bigmpi_int_max && recvcount > bigmpi_int_max ) {
        MPI_Datatype newrecvtype;
        MPIX_Type_contiguous_x(recvcount, recvtype, &newrecvtype);
        MPI_Type_commit(&newrecvtype);
        rc = MPI_Sendrecv(sendbuf, (int)sendcount, sendtype, dest, sendtag,
                          recvbuf, 1, newrecvtype, source, recvtag,
                          comm, status);
        MPI_Type_free(&newrecvtype);
    } else if (sendcount > bigmpi_int_max && recvcount <= bigmpi_int_max ) {
        MPI_Datatype newsendtype;
        MPIX_Type_contiguous_x(sendcount, sendtype, &newsendtype);
        MPI_Type_commit(&newsendtype);
        rc = MPI_Sendrecv(sendbuf, 1, newsendtype, dest, sendtag,
                          recvbuf, (int)recvcount, recvtype, source, recvtag,
                          comm, status);
        MPI_Type_free(&newsendtype);
    } else {
        MPI_Datatype newsendtype, newrecvtype;
        MPIX_Type_contiguous_x(sendcount, sendtype, &newsendtype);
        MPIX_Type_contiguous_x(recvcount, recvtype, &newrecvtype);
        MPI_Type_commit(&newsendtype);
        MPI_Type_commit(&newrecvtype);
        rc = MPI_Sendrecv(sendbuf, 1, newsendtype, dest, sendtag,
                          recvbuf, 1, newrecvtype, source, recvtag,
                          comm, status);
        MPI_Type_free(&newsendtype);
        MPI_Type_free(&newrecvtype);
    }
    return rc;
}

int MPIX_Sendrecv_replace_x(void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int sendtag,
                            int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Sendrecv_replace(buf, (int)count, datatype, dest, sendtag, source, recvtag, comm, status);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Sendrecv_replace(buf, 1, newtype, dest, sendtag, source, recvtag, comm, status);
        MPI_Type_free(&newtype);
    }
    return rc;
}


int MPIX_Ssend_x(const void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Ssend(buf, (int)count, datatype, dest, tag, comm);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Ssend(buf, 1, newtype, dest, tag, comm);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Rsend_x(const void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Rsend(buf, (int)count, datatype, dest, tag, comm);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Rsend(buf, 1, newtype, dest, tag, comm);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Issend_x(const void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Issend(buf, (int)count, datatype, dest, tag, comm, request);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Issend(buf, 1, newtype, dest, tag, comm, request);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Irsend_x(const void *buf, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Irsend(buf, (int)count, datatype, dest, tag, comm, request);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Irsend(buf, 1, newtype, dest, tag, comm, request);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Mrecv_x(void *buf, MPI_Count count, MPI_Datatype datatype, MPI_Message *message, MPI_Status *status)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Mrecv(buf, (int)count, datatype, message, status);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Mrecv(buf, 1, newtype, message, status);
        MPI_Type_free(&newtype);
    }
    return rc;
}

int MPIX_Imrecv_x(void *buf, MPI_Count count, MPI_Datatype datatype, MPI_Message *message, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

    if (likely (count <= bigmpi_int_max )) {
        rc = MPI_Imrecv(buf, (int)count, datatype, message, request);
    } else {
        MPI_Datatype newtype;
        MPIX_Type_contiguous_x(count, datatype, &newtype);
        MPI_Type_commit(&newtype);
        rc = MPI_Imrecv(buf, 1, newtype, message, request);
        MPI_Type_free(&newtype);
    }
    return rc;
}
