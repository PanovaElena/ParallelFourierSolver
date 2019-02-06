import math

# ligth speed, cm/s

LIGHT_SPEED = 29979245800

# area, cm

ax = 0
bx = 128*LIGHT_SPEED
ay = 0
by = 128*LIGHT_SPEED
az = 0
bz = 1

# grid

nx = 128
ny = 128
nz = 1

dx = (bx-ax)/nx
dy = (by-ay)/ny
dz = (bz-az)/nz

# time step, s

dt = 0.1  # less than courant condition for PSTD

# field solver: PSTD, PSATD, PSATD_omp or FDTD (for sequential launch only)

solver = "PSTD"

# parameters of wave

time_source = 16                  # time of working of source
omega = 2*math.pi / time_source   # frequency of source
omega_envelope = omega            
width_source = dx*8       

# output

dimension_of_output_data = 1   # 1 (OX) or 2 (XOZ)

# number of iterations for sequention launch

n_iter = 350


# parameters for parallel launch

scheme = "copy"    # scheme of exchange: sum or copy

# num of processes

npx = 2
npy = 1
npz = 1

# dimension of guard (number of cells of the grid)

gx = 32
gy = 0
gz = 0

# number of sequential (first) and parallel (second) iterations

n_sequential_iter = 300
n_parallel_iter = n_iter-n_sequential_iter

# max number of iterations in every domain
# if scheme=copy max distance should be half of the guard width

number_of_iterations_in_domain = int(0.3*gx*dx/LIGHT_SPEED/dt)

# parameters of mask

mask = "simple"   # simple or smooth

mwx = gx/4        # number of cells for mask (for smooth only)
mwy = gy/4
mwz = gz/4

# parameters of low frequency filter

filter = "off"    # on or off

fwx = 8           # number of frequences in fourier space which will be changed
fwy = 0
fwz = 0

fnzx = 4          # number of frequences in fourier space which will be set to zero
fnzy = 0
fnzz = 0




