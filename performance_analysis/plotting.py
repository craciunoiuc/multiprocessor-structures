import numpy as np
import matplotlib.pyplot as plt

plot_path = 'plots/'
num_threads =   ['1', '2', '4', '8', '16', '32', '64']
hibrid_times =  [160.0, 81.5, 42.26, 31.25, 17.66, 14.24, 16.04]
mpi_times =     [162.35, 81.4, 44.0, 24.4, 18.3, 17.0, 52.6]
pthread_times = [159.5, 81.6, 41.7, 22.5, 17.2, 16.8, 43.6]
openmp_times =  [162.2, 81.5, 42.3, 25.0, 17.86, 15.13, 16.27]
serial_time = 160.3

def plot_scalability():
    hibrid_times_scl = [serial_time / x for x in hibrid_times]
    mpi_times_scl = [serial_time / x for x in mpi_times]
    pthread_times_scl = [serial_time / x for x in pthread_times]
    openmp_times_scl = [serial_time / x for x in openmp_times]
    x = np.arange(len(num_threads))
    fig, ax = plt.subplots()
    ax.bar(x-0.3, mpi_times_scl, width=0.2, align='center', label='MPI')
    ax.bar(x-0.1, pthread_times_scl, width=0.2, align='center', label='PThread')
    ax.bar(x+0.1, openmp_times_scl, width=0.2, align='center', label='OpenMP')
    ax.bar(x+0.3, hibrid_times_scl, width=0.2, align='center', label='Hibrid (MPI+OpenMP)')
    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.set(title='Scalabilitate rulare pentru toate versiunile', ylabel='Speedup', xlabel='Numar thread-uri')
    ax.legend()
    fig.savefig(plot_path + 'scalability.png')
    return

def plot_efficiency():
    hibrid_times_eff = [serial_time / n for n in hibrid_times]
    mpi_times_eff = [serial_time / n for n in mpi_times]
    pthread_times_eff = [serial_time / n for n in pthread_times]
    openmp_times_eff = [serial_time / n for n in openmp_times]
    for i in range(7):
        hibrid_times_eff[i] /= 2**i
        mpi_times_eff[i] /= 2**i
        pthread_times_eff[i] /= 2**i
        openmp_times_eff[i] /= 2**i

    x = np.arange(len(num_threads))
    fig, ax = plt.subplots()
    ax.bar(x-0.3, mpi_times_eff, width=0.2, align='center', label='MPI')
    ax.bar(x-0.1, pthread_times_eff, width=0.2, align='center', label='PThread')
    ax.bar(x+0.1, openmp_times_eff, width=0.2, align='center', label='OpenMP')
    ax.bar(x+0.3, hibrid_times_eff, width=0.2, align='center', label='Hibrid (MPI+OpenMP)')
    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.set(title='Eficienta rulare pentru toate versiunile', ylabel='Eficienta', xlabel='Numar thread-uri')
    ax.legend()
    fig.savefig(plot_path + 'efficiency.png')
    return

def plot_times_bar():
    x = np.arange(len(num_threads))
    fig, ax = plt.subplots()
    ax.bar(x-0.3, mpi_times, width=0.2, align='center', label='MPI')
    ax.bar(x-0.1, pthread_times, width=0.2, align='center', label='PThread')
    ax.bar(x+0.1, openmp_times, width=0.2, align='center', label='OpenMP')
    ax.bar(x+0.3, hibrid_times, width=0.2, align='center', label='Hibrid (MPI+OpenMP)')
    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.set(title='Timpi rulare pentru toate versiunile', ylabel='Secunde', xlabel='Numar thread-uri')
    ax.legend()
    fig.savefig(plot_path + 'times_bar.png')
    return

def plot_times():
    x = np.arange(len(num_threads))
    fig, ax = plt.subplots()
    ax.plot(hibrid_times, label='MPI')
    ax.plot(mpi_times, label='PThread')
    ax.plot(openmp_times, label='OpenMP')
    ax.plot(pthread_times, label='Hibrid (MPI+OpenMP)')
    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.set(title='Timpi rulare pentru toate versiunile', ylabel='Secunde', xlabel='Numar thread-uri')
    ax.legend()
    fig.savefig(plot_path + 'times.png')
    return

def main():
    plot_times_bar()
    plot_times()
    plot_scalability()
    plot_efficiency()
    return

if __name__ == "__main__":
    main()