# draw every log
base_dir="/afs/crc.nd.edu/user/j/jzhou24/scripts/vine-run-info"
log_dir="$base_dir"/$1/vine-logs
vine_plot_txn_log $log_dir/transactions $log_dir/txn_log.png


#for sub_dir in "$base_dir"/*
#do
#	if [[ "$sub_dir" == *vine-cache ]]; then
#		continue	
#	fi
#	log_dir="$sub_dir"/vine-logs
#	bash run_blast.sh -plot $log_dir 
#	vine_plot_txn_log $log_dir/transactions $log_dir/txn_log.png
#	dot -Tpng $log_dir/taskgraph > $log_dir/dag.png	
#done


# draw most recent
# export vine_logs_path="/afs/crc.nd.edu/user/j/jzhou24/scripts/vine-run-info/most-recent/vine-logs"
# bash run_blast.sh -plot $vine_logs_path
# vine_plot_txn_log $vine_logs_path/transactions $vine_logs_path/txn_log.png
# dot -Tpng $vine_logs_path/taskgraph > $vine_logs_path/dag.png
