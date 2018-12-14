#include "mpi_worker.h"
#include "gtest.h"
#include "mask.h"

TEST(TestMPIWorker, mpi_worker_dont_fail) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    ASSERT_NO_THROW(MPIWorker mpiWorker(gr, vec3<int>(1), MaskSimple, 0, 2, 0));
}

TEST(TestMPIWorker, mpi_worker_sizes_are_correct) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1,0,0), MaskSimple, 0, 2, 0);

    ASSERT_EQ(1, mpiWorker.getGuardSize().x);
    ASSERT_EQ(3, mpiWorker.getMainDomainSize().x);
    ASSERT_EQ(5, mpiWorker.getFullDomainSize().x);
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_left_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1, 0, 0), MaskSimple, 0, 2, 0);

    ASSERT_EQ(7, mpiWorker.getLeftGuardStart().x);
    ASSERT_EQ(8, mpiWorker.getLeftGuardEnd().x);
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_right_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1, 0, 0), MaskSimple, 0, 2, 0);

    ASSERT_EQ(3, mpiWorker.getRightGuardStart().x);
    ASSERT_EQ(4, mpiWorker.getRightGuardEnd().x);
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_main_domain) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1, 0, 0), MaskSimple, 0, 2, 0);

    ASSERT_EQ(0, mpiWorker.getMainDomainStart().x);
    ASSERT_EQ(3, mpiWorker.getMainDomainEnd().x);
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_left_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1, 0, 0), MaskSimple, 0, 2, 1);

    ASSERT_EQ(3, mpiWorker.getLeftGuardStart().x);
    ASSERT_EQ(4, mpiWorker.getLeftGuardEnd().x);
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_right_guard) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1, 0, 0), MaskSimple, 0, 2, 1);

    ASSERT_EQ(7, mpiWorker.getRightGuardStart().x);
    ASSERT_EQ(8, mpiWorker.getRightGuardEnd().x);
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_main_domain) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, vec3<int>(1, 0, 0), MaskSimple, 0, 2, 1);

    ASSERT_EQ(4, mpiWorker.getMainDomainStart().x);
    ASSERT_EQ(7, mpiWorker.getMainDomainEnd().x);
}

TEST(TestMPIWorker, mpi_worker_copy_gr_correctly_for_process_0) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < gr.gnyRealCells(); j++)
            for (int k = 0; k < gr.gnzRealCells(); k++)
                gr.E[0](i, j, k) = i % 8;
    vec3<int> guard = vec3<int>(1, 0, 0);
    MPIWorker mpiWorker(gr, guard, MaskSimple, 0, 2, 0);

    for (int i = 0; i < mpiWorker.getMainDomainSize().x; i++)
        for (int j = 0; j < mpiWorker.getMainDomainSize().y; j++)
            for (int k = 0; k < mpiWorker.getMainDomainSize().z; k++)
                ASSERT_DOUBLE_EQ(i % 8, (mpiWorker.getGrid()).E.x(i + guard.x, j + guard.y, k + guard.z));
}

TEST(TestMPIWorker, mpi_worker_copy_gr_correctly_for_process_1) {
    Grid3d gr(8, 1, 1, 0, 1, 0, 1, 0, 1);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < gr.gnyRealCells(); j++)
            for (int k = 0; k < gr.gnzRealCells(); k++)
                gr.E[0](i, j, k) = i % 8;
    vec3<int> guard = vec3<int>(1, 0, 0);
    MPIWorker mpiWorker(gr, guard, MaskSimple, 0, 2, 1);

    for (int i = 0; i < mpiWorker.getMainDomainSize().x; i++)
        for (int j = 0; j < mpiWorker.getMainDomainSize().y; j++)
            for (int k = 0; k < mpiWorker.getMainDomainSize().z; k++)
                ASSERT_DOUBLE_EQ((i+4) % 8, (mpiWorker.getGrid()).E.x(i + guard.x, j + guard.y, k + guard.z));
}


