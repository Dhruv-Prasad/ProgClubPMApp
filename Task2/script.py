import subprocess
import statistics

# List of executables to run
executables = ['s.exe', 'p.exe', 'pool1.exe', 'pool2.exe', 'pool3.exe']

# List of arguments to run each executable with
N_list = ['100', '300', '750', '1000', '1500', '2000', '2500', '3000', '3500']

# Number of iterations to run (i.e. number of data points I am calculating mean of)
num_runs = 25

# Dictionary to store mean execution times for each executable and argument
mean_times = {}

# Run each executable with each argument in N_list
for exe in executables:
    mean_times[exe] = {}
    for arg in N_list:
        execution_times = []
        for _ in range(num_runs):
            result = subprocess.run([exe, arg], capture_output=True, text=True)
            # Assuming the executable outputs only the execution time
            execution_time = float(result.stdout.strip())
            execution_times.append(execution_time)
            continue

        # Calculate the mean execution time for the current executable and argument
        if execution_times:
            mean_time = statistics.mean(execution_times)
            mean_times[exe][arg] = mean_time
        else:
            mean_times[exe][arg] = None

# Write the mean execution times to a file
with open('mean_times.txt', 'w') as file:
    for exe, times in mean_times.items():
        file.write(f'{exe}:\n')
        for arg, mean_time in times.items():
            if mean_time is not None:
                file.write(f'  Argument {arg}: Mean execution time: {mean_time}\n')
            else:
                pass
        file.write('\n')

print("Done")