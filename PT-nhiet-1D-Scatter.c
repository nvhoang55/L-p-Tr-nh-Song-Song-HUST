#include <stdio.h>
#include <mpich/mpi.h>
#include <malloc.h>

#define M 20
#define Time 1
#define dt 0.01
#define dx 0.1
#define D 0.1

//=========================
void DisplayArray(float *T, int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("  %.2f", *(T + i));
    printf("\n");
}

//=========================
void KhoiTao(float *T)
{
    for (int i = 0; i < M; i++)
        *(T + i) = 25.0;
}

//=========================
void Daoham(float *T, float *dT, float Tl, float Tr, int Mc)
{
    float c, l, r;
    for (int i = 0; i < Mc; i++)
    {
        c = *(T + i);
        l = (i == 0) ? Tl : *(T + (i - 1));
        r = (i == Mc - 1) ? Tr : *(T + (i + 1));
        *(dT + i) = (r - 2 * c + l) / (dx * dx);
    }
}

//=========================
int main(int argc, char **argv)
{
    int i, t, Ntime = Time / dt, rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;

    //  Khoi tao gia tri cho cac CPU
    int Mc = M / size;
    float *T, *dT, *Tc, Tl, Tr;

    dT = (float *) malloc(M * sizeof(float));
    T = (float *) malloc(M * sizeof(float));
    Tc = (float *) malloc(Mc * sizeof(float));

    if (rank == 0)
    {
        KhoiTao(T);
        printf("Gia tri khoi tao:\n");
        DisplayArray(T, M);
    }

    //  Gui va nhan input cho cac CPU
    MPI_Scatter(T, Mc, MPI_FLOAT, Tc,
                Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);

    //  Vong lap tinh toan Ntime lan
    for (t = 0; t < Ntime; t++)
    {
        //  Truyen thong phan tu Tl
        if (rank == 0)
        {
            Tl = 100;
            MPI_Send(Tc + Mc - 1, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
        }
        else if (rank == size - 1)
            MPI_Recv(&Tl, 1, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        else
        {
            MPI_Send(Tc + Mc - 1, 1, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
            MPI_Recv(&Tl, 1, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        }
        //  Truyen thong phan tu Tr
        if (rank == 0)
            MPI_Recv(&Tr, 1, MPI_FLOAT, 1, 1, MPI_COMM_WORLD, &stat);
        else if (rank == size - 1)
        {
            Tr = 25;
            MPI_Send(Tc, 1, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(Tc, 1, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
            MPI_Recv(&Tr, 1, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &stat);
        }
        //  Tinh toan dao ham
        Daoham(Tc, dT, Tl, Tr, Mc);
        //  Cap nhat T
        for (i = 0; i < Mc; i++)
            *(Tc + i) = *(Tc + i) + D * dt * (*(dT + i));
        MPI_Barrier(MPI_COMM_WORLD);
    }

    float *result;
    if (rank == 0)
    {
        result = (float *) malloc(M * sizeof(float));
    }

    //  Gui va nhan output tu cac CPU
    MPI_Gather(Tc, Mc, MPI_FLOAT, result, Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        DisplayArray(result, M);
    }

    MPI_Finalize();
    return 0;
}
