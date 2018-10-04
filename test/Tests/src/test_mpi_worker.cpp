#include "mpi_worker.h"
#include "gtest.h"
#include "masks.h"

TEST(TestMPIWorker, mpi_worker_dont_fail) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    ASSERT_NO_THROW(MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 0));
}

TEST(TestMPIWorker, mpi_worker_sizes_are_correct) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 0);

    ASSERT_EQ(1, mpiWorker.getGuardSize());
    ASSERT_EQ(3, mpiWorker.getMainDomainSize());
    ASSERT_EQ(5, mpiWorker.getFullDomainSize());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_left_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 0);

    ASSERT_EQ(7, mpiWorker.getLeftGuardStart());
    ASSERT_EQ(8, mpiWorker.getLeftGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_right_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 0);

    ASSERT_EQ(3, mpiWorker.getRightGuardStart());
    ASSERT_EQ(4, mpiWorker.getRightGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_main_domain) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 0);

    ASSERT_EQ(0, mpiWorker.getMainDomainStart());
    ASSERT_EQ(3, mpiWorker.getMainDomainEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_left_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 1);

    ASSERT_EQ(3, mpiWorker.getLeftGuardStart());
    ASSERT_EQ(4, mpiWorker.getLeftGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_right_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 1);

    ASSERT_EQ(7, mpiWorker.getRightGuardStart());
    ASSERT_EQ(8, mpiWorker.getRightGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_main_domain) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, simpleMask, 0, 2, 1);

    ASSERT_EQ(4, mpiWorker.getMainDomainStart());
    ASSERT_EQ(7, mpiWorker.getMainDomainEnd());
}

TEST(TestMPIWorker, mpi_worker_copy_gr_correctly_for_process_0) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    for (int i = 0; i <= 8; i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++)
                gr(i, j, k).E[0] = i % 8;
    int guard = 1;
    MPIWorker mpiWorker(gr, guard, simpleMask, 0, 2, 0);

    for (int i = 0; i <= 3; i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++)
                ASSERT_DOUBLE_EQ(i % 8, (mpiWorker.getGrid())(i + guard, j, k).E.x());
}

TEST(TestMPIWorker, mpi_worker_copy_gr_correctly_for_process_1) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    for (int i = 0; i <= 8; i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++)
                gr(i, j, k).E[0] = i % 8;
    int guard = 1;
    MPIWorker mpiWorker(gr, guard, simpleMask, 0, 2, 1);

    for (int i = 4; i <= 7; i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++)
                ASSERT_DOUBLE_EQ(i % 8, (mpiWorker.getGrid())(i - 4 + guard, j, k).E.x());
}


