#!/usr/bin/env python

# Copyright (C) 2022- The University of Notre Dame
# This software is distributed under the GNU General Public License.
# See the file COPYING for details.

# This example shows some of the data handling features of taskvine.
# It performs a BLAST search of the "Landmark" model organism database.
# It works by constructing tasks that download the blast executable
# and landmark database from NCBI, and then performs a short query.

# Each task in the workflow performs a query of the database using
# 16 (random) query strings generated at the manager.
# Both the downloads are automatically unpacked, cached, and shared
# with all the same tasks on the worker.

import ndcctools.taskvine as vine
import random
import argparse
import matplotlib.pyplot as plt
import subprocess
from tqdm import tqdm
import matplotlib.gridspec as gridspec
import getpass
import numpy as np
import sys
import pandas as pd
import os
import time

# Permitted letters in an amino acid sequence
amino_letters = "ACGTUiRYKMSWBDHVN"

# Number of characters in each query
# query_length = 4


def make_query_text(query_count, query_length):
    """Create a query string consisting of {query_count} sequences of {query_length} characters."""
    queries = []
    # print("query_count = ", type(query_count))
    for i in range(query_count):
        query = "".join(random.choices(amino_letters, k=query_length))
        queries.append(query)
    return ">query\n" + "\n".join(queries) + "\n"

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

def convert_txt_xlsx(working_dir):
     # working_dir = "/afs/crc.nd.edu/user/j/jzhou24/scripts/vine-run-info/2023-09-11T155253/vine-logs/"
     print("Converting the log file into xlsx formant...")
     input_file = os.path.join(working_dir, "performance")
     output_file = os.path.join(working_dir, "performance.xlsx")

     data = pd.read_csv(input_file, sep=' ')

     cols = list(data.columns)
     cols.remove('#')
     cols.append(None)
     data.columns = cols

     data.to_excel(output_file, index=False)
     print("Convert Done")


def plot_performance(log_dir, task_name):
    """ This function is used to plot the accumulated schedulling time for all tasks
        and the separate schedulling time for each task """
    
    print(f"Plotting the performance of {task_name} ...")
    performance_file = os.path.join(log_dir, "performance")
    f = open(performance_file)
    fields = read_fields(f)
    f.seek(0)

    dispatch_offset = fields.index("tasks_dispatched")
    scheduling_offset = fields.index("time_scheduling")

    # STEP1: Get the accmulated time as the number of tasks grow
    num_timestamps = 0
    tasks_dispatched = []
    time_scheduling = []
    for line in f:
        if line[0] == "#":
            continue
        items = line.strip("\n\r\t ").split()
        num_timestamps += 1
        tasks_dispatched.append(int(items[dispatch_offset]))
        time_scheduling.append((items[scheduling_offset]))

    tasks_scheduled = []
    accumulated_time = []
    last_task_id = -1
    for i, task_id in enumerate(tasks_dispatched):
        if int(time_scheduling[i]) == 0:
            continue
        if task_id > last_task_id:
            tasks_scheduled.append(task_id)
            accumulated_time.append(int(time_scheduling[i]))
        last_task_id = task_id

    # STEP2: Generate the scheduled time for each task
    scheduled_task = []
    scheduled_time = []
    # There are a lot of concurrent tasks in a period, split the execution time evenly among them
    for idx, tid in enumerate(tasks_scheduled):
        if tid != tasks_scheduled[-1]:
            next_tid = tasks_scheduled[idx + 1]
            task_gap = next_tid - tid

            this_time, next_time = accumulated_time[idx], accumulated_time[idx + 1]
            time_gap = next_time - this_time
            time_for_each_task = time_gap / task_gap

            for tid_ in range(tid, next_tid, 1):
                scheduled_task.append(tid_)
                scheduled_time.append(time_for_each_task)

    # time_top means the most popular schedulling time of all tasks, with given value indicating how popular it is
    top_value = 98
    time_top = np.percentile(scheduled_time, top_value)
    scheduled_task_top, scheduled_time_top = [], []
    scheduled_task_tail, scheduled_time_tail = [], []
    for idx, time in enumerate(scheduled_time):
        if time < time_top:
            scheduled_task_top.append(scheduled_task[idx])
            scheduled_time_top.append(scheduled_time[idx])
        else:
            scheduled_task_tail.append(scheduled_task[idx])
            scheduled_time_tail.append(scheduled_time[idx])

    # STEP3: Draw three figures: the left one is accumulated time with accumulated tasks
    # the left top one is the schedulling time for each task in most cases
    # the right bottom one is the schedulling time for each task which could have extremely large number in some exceptional cases
    plt.figure(figsize=(10, 6))
    gs = gridspec.GridSpec(2, 2, width_ratios=[2, 1])
    
    ax1 = plt.subplot(gs[:, 0])
    ax2 = plt.subplot(gs[0, 1])
    ax3 = plt.subplot(gs[1, 1])

    ax1.plot(tasks_scheduled, accumulated_time)
    ax1.set_ylim(ymin=0)
    ax1.set_xlabel("Tasks Dispatched")
    ax1.set_ylabel("Accumulated Time (us)")

    ax2.plot(scheduled_task_top, scheduled_time_top)
    ax2.set_ylim(ymin=0)
    ax2.set_xlabel(f"Task ID (top {top_value:.2f}%)")
    ax2.set_ylabel("Dispatching Time (us)")

    ax3.plot(scheduled_task_tail, scheduled_time_tail)
    ax3.set_ylim(ymin=0)
    ax3.set_xlabel(f"Task ID (tail {100 - top_value:.2f}%)")
    ax3.set_ylabel("Dispatching Time (us)")
    
    plt.tight_layout()
    save_png = os.path.join(log_dir, task_name + '_performance.png')
    plt.savefig(save_png)

    print("Plot Done")

    return


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="vine_example_blast.py",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description="""This example shows some of the data handling features of taskvine.
It performs a BLAST search of the "Landmark" model organism database.
It works by constructing tasks that download the blast executable
and landmark database from NCBI, and then performs a short query.

Each task in the workflow performs a query of the database using
16 (random) query strings generated at the manager.
Both the downloads are automatically unpacked, cached, and shared
with all the same tasks on the worker.""",
    )

    parser.add_argument(
        "--query-length",
        nargs="?",
        type=int,
        help="query length",
        default=4,
    )

    parser.add_argument(
        "--task-count",
        nargs="?",
        type=int,
        help="the number of tasks to generate.",
        default=1000,
    )
    parser.add_argument(
        "--query-count",
        nargs="?",
        type=int,
        help="the number of queries to generate per task.",
        default=10,
    )
    parser.add_argument(
        "--name",
        nargs="?",
        type=str,
        help="name to assign to the manager.",
        default=f"vine-blast-{getpass.getuser()}",
    )
    parser.add_argument(
        "--port",
        nargs="?",
        type=int,
        help="port for the manager to listen for connections. If 0, pick any available.",
        default=9123,
    )
    parser.add_argument(
        "--disable-peer-transfers",
        action="store_true",
        help="disable transfers among workers.",
        default=False,
    )
    parser.add_argument(
        "--max-concurrent-transfers",
        nargs="?",
        type=int,
        help="maximum number of concurrent peer transfers",
        default=3,
    )
    parser.add_argument(
        "--worker-count",
        nargs="?",
        type=int,
        default=10,
    )
    parser.add_argument(
        "--plot",
        nargs="?",
    )
    args = parser.parse_args()

    task_name = args.name
    plot_file = args.plot
    if args.plot:
        plot_performance(plot_file, "task")
        exit(0)

    print("query-count is ", args.query_count)
    m = vine.Manager(port=args.port)

    m.set_name(args.name)
    query_length = args.query_length

    if args.disable_peer_transfers:
        m.disable_peer_transfers()
    
    if args.max_concurrent_transfers:
        m.tune("worker-source-max-transfers", args.max_concurrent_transfers)

    print("Declaring files...")
    blast_url = m.declare_url(
        "https://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/2.13.0/ncbi-blast-2.13.0+-x64-linux.tar.gz",
        cache="always",  # with "always", workers keep this file until they are terminated
    )
    blast = m.declare_untar(blast_url, cache="always")

    landmark_url = m.declare_url(
        "https://ftp.ncbi.nlm.nih.gov/blast/db/landmark.tar.gz",
        cache="always",
    )
    landmark = m.declare_untar(landmark_url)

    print("Declaring tasks...")
    for i in range(args.task_count):
        query = m.declare_buffer(make_query_text(args.query_count, args.query_length))
        t = vine.Task(
            command="blastdir/ncbi-blast-2.13.0+/bin/blastp -db landmark -query query.file",
            inputs={
                query: {"remote_name": "query.file"},
                blast: {"remote_name": "blastdir"},
                landmark: {"remote_name": "landmark"},
            },
            env={"BLASTDB": "landmark"},
            cores=1,
        )

        task_id = m.submit(t)
        # print(f"submitted task {t.id}: {t.command}")

    print("Files and tasks are all declared, forking a process to submit them to workers...")

    pid = os.fork()
    if pid == 0:
        os.execvp("/afs/crc.nd.edu/user/j/jzhou24/miniconda3/bin/vine_submit_workers",
                  ["vine_submit_workers", "-T", "condor", "crcfe01.crc.nd.edu", f"{m.port}", f"{args.worker_count}"])
    else:
        os.wait()

    print(f"TaskVine listening for workers on {m.port}")
    start_time = time.time()
    print("Waiting for tasks to complete...")

    pbar = tqdm(total=args.task_count)

    while not m.empty():
        t = m.wait(5)
        if t:
            if t.successful():
                end_time = time.time()
                # print(f"task{t.id} success, runtime = {end_time - start_time:.2f}s")
                pbar.update(1)
            elif t.completed():
                print(
                    f"task {t.id} completed with an executin error, exit code {t.exit_code}"
                )
            else:
                print(f"task {t.id} failed with status {t.result}")
                pbar.update(1)

    pbar.close()
    print("all tasks complete!")

    # plot the figure for execution performance
    plot_performance(m.logging_directory, task_name)

    # convert the txt performance file into xlsx format
    convert_txt_xlsx(m.logging_directory)
