#! /usr/bin/env python

# Copyright (C) 2023- The University of Notre Dame
# This software is distributed under the GNU General Public License.
# See the file COPYING for details.

# Plot the time spent matching tasks to workers through the information in the performance log

import os
import sys
import matplotlib.pyplot as plt

def read_fields(f, lines_patience = 10):
    for line in f:
        if line[0] != '#':
            lines_patience = lines_patience - 1
        else:
            return line.strip('#\n\r\t ').split()
        if lines_patience < 1:
            break
    sys.stderr.write("Could not find fields. This is probably not a performance log...\n")
    sys.exit(1)

def plot_line():
    print("Plotting line graph about worker performance...")
    plt.figure(figsize=(10, 6))

    for i in range(3):
        plt.plot(plot_xitem, plot_yitems[i], label=plot_labels[i], color=plot_colors[i])

    plt.title('Worker Performance')
    plt.xlabel('Tasks Done')
    plt.ylabel('Number of Workers')
    plt.legend()
    plt.show()
    print("Plot done.")

def plot_bar():
    print("Plotting bar graph about worker performance...")
    plt.figure(figsize=(12, 6))

    for i in range(len(plot_xitem)):
        # Firstly plot the connected workers
        plt.bar(plot_xitem[i], plot_yitems[0][i], color=plot_colors[0], width=1)
        # Then plot the idle and busy workers separately
        plt.bar(plot_xitem[i], plot_yitems[1][i], color=plot_colors[1], width=1)
        plt.bar(plot_xitem[i], plot_yitems[2][i], color=plot_colors[2], width=1, bottom=plot_yitems[1][i])
    
    plt.title('Worker Performance')
    plt.xlabel('Tasks Done')
    plt.ylabel('Number of Workers')
    legend_handles = [plt.Rectangle((0,0),1,1, color=plot_color) for plot_color in plot_colors]
    plt.legend(legend_handles, plot_labels)
    plt.tight_layout()
    
    save_dir = os.path.join(base_dir, "workers_performance")
    filename = str(num_tasks) + "tasks_" + str(num_workers) + "workers"
    save_file = os.path.join(save_dir, filename)
    plt.savefig(save_file)
    plt.show()
    print("Plot done.")


if __name__ == "__main__":
    
    base_dir = "/afs/crc.nd.edu/user/j/jzhou24/scripts/vine-run-info/"
    working_dir = sys.argv[1]
    log_dir = os.path.join(base_dir, working_dir, "vine-logs")
    log_file = os.path.join(log_dir, "performance")

    f = open(log_file)
    fields = read_fields(f)
    f.seek(0)
    
    timestamp_offset = fields.index("timestamp")
    workers_connected_offset = fields.index("workers_connected")
    workers_idle_offset = fields.index("workers_idle")
    workers_busy_offset = fields.index("workers_busy")
    tasks_done_offset = fields.index("tasks_done")

    timestamp_id = 0
    timestamps = []
    workers_connected = []
    workers_idle = []
    workers_busy = []
    tasks_done = []    
    last_task_done_item = -1  

    for line in f:
        if line[0] == "#":
            continue
        items = line.strip("\n\r\t ").split()
        timestamp_id += 1
        timestamps.append(timestamp_id)
        
        workers_connected_item = int(items[workers_connected_offset])
        workers_idle_item = int(items[workers_idle_offset])
        workers_busy_item = int(items[workers_busy_offset])
        task_done_item = int(items[tasks_done_offset])
    
        if len(tasks_done) > 1 and task_done_item == tasks_done[-1]:
            continue

        if len(tasks_done) > 1 and task_done_item - tasks_done[-1] > 1:
            for i_ in range(tasks_done[-1], task_done_item, 1):
                workers_connected.append(workers_connected_item)
                workers_idle.append(workers_idle_item) 
                workers_busy.append(workers_busy_item)
                tasks_done.append(i_)
        
        workers_connected.append(workers_connected_item)
        workers_idle.append(workers_idle_item) 
        workers_busy.append(workers_busy_item)
        tasks_done.append(task_done_item)
             

    plot_colors = ['red', 'purple', 'blue']
    plot_xitem = tasks_done
    plot_yitems = [workers_connected, workers_idle, workers_busy]
    plot_labels = ['connected', 'idle', 'busy']
    
    num_workers = max(workers_connected)
    num_tasks = tasks_done[-1]
    
    plot_bar()
