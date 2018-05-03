#include "gtest.h"
#include "mpi_worker.h"

TEST(TestMPIWorker, mpi_worker_dont_fail) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    ASSERT_NO_THROW(MPIWorker mpiWorker(gr, 1, 3, 0));
}

TEST(TestMPIWorker, mpi_worker_sizes_are_correct) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 0);

    ASSERT_EQ(1, mpiWorker.getGuardSize());
    ASSERT_EQ(3, mpiWorker.getMainDomainSize());
    ASSERT_EQ(5, mpiWorker.getFullDomainSize());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_left_guard) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 0);

    ASSERT_EQ(8, mpiWorker.getLeftGuardStart());
    ASSERT_EQ(9, mpiWorker.getLeftGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_right_guard) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 0);

    ASSERT_EQ(3, mpiWorker.getRightGuardStart());
    ASSERT_EQ(4, mpiWorker.getRightGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_0_main_domain) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 0);

    ASSERT_EQ(0, mpiWorker.getMainDomainStart());
    ASSERT_EQ(3, mpiWorker.getMainDomainEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_left_guard) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 1);

    ASSERT_EQ(2, mpiWorker.getLeftGuardStart());
    ASSERT_EQ(3, mpiWorker.getLeftGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_right_guard) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 1);

    ASSERT_EQ(6, mpiWorker.getRightGuardStart());
    ASSERT_EQ(7, mpiWorker.getRightGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_1_main_domain) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 1);

    ASSERT_EQ(3, mpiWorker.getMainDomainStart());
    ASSERT_EQ(6, mpiWorker.getMainDomainEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_2_left_guard) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 2);

    ASSERT_EQ(5, mpiWorker.getLeftGuardStart());
    ASSERT_EQ(6, mpiWorker.getLeftGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_2_right_guard) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 2);

    ASSERT_EQ(0, mpiWorker.getRightGuardStart());
    ASSERT_EQ(1, mpiWorker.getRightGuardEnd());
}

TEST(TestMPIWorker, mpi_worker_create_correct_gr_rank_2_main_domain) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    MPIWorker mpiWorker(gr, 1, 3, 2);

    ASSERT_EQ(6, mpiWorker.getMainDomainStart());
    ASSERT_EQ(9, mpiWorker.getMainDomainEnd());
}

TEST(TestMPIWorker, mpi_worker_copy_gr_correctly) {
    Grid3d gr(9, 1, 1, 0, 1, 0, 1, 0, 1);
    gr(4, 1, 1).E = vec3<double>(1, 1, 1);
    MPIWorker mpiWorker(gr, 1, 3, 1);

    ASSERT_EQ(gr(4,1,1).E.x(), (mpiWorker.getGrid())(2,1,1).E.x());
    ASSERT_EQ(1, (mpiWorker.getGrid())(2, 1, 1).E.x());
}