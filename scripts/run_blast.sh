#!/bin/bash

help_info="
Usage: bash $0 [-opt]
opt:
	-h                   print help information
	-p  <port>           the working port
	-ql <query_length>   the length of sequence for each query
	-qc <query_count>    the number of queries for each task
	-tc <task_count>     the number of taskGs in total
	-wc <worker_count>   the number of workers

example:
	bash $0 -p 9123 -tc 10000 -wc 1000
"


# -plot <perf_file>    plot from task performance information file, once choose this option, you can only plot a figure from given vine_info file
blast_dir="/afs/crc.nd.edu/user/j/jzhou24/my_cctools/taskvine/src/examples/"
blast_path="${blast_dir}vine_example_blast.py"

ql=4
qc=2

while [[ "$#" -gt 0 ]]; do
	case $1 in
		-h)
			printf "%s\n" "$help_info"
			shift
			exit 0
			;;
		-plot)
			python $blast_path --plot "$2"
			exit 0
			;;			
		-p)
			port="$2"
			shift 2
			;;
		-ql)
			ql="$2"
			shift 2
			;;
		-qc)
			qc="$2"
			shift 2
			;;
		-tc)
			tc="$2"
			shift 2
			;;
		-name)
			name="$2"
			shift 2
			;;
		-wc)
			wc="$2"
			shift 2
			;;
		*)
			echo "Invalid Option: $1"
			exit 1
			;;
	esac
done	


echo "port=$port query_length=$ql query_count=$qc task_count=$tc"
name="tasks$tc"_"workers$wc"
python $blast_path --port $port --query-length $ql --query-count $qc --task-count $tc --worker-count $wc --name $name
python3 /afs/crc.nd.edu/user/j/jzhou24/my_cctools/taskvine/src/tools/vine_worker_performance.py most-recent
