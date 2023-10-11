if [[ $# == 1 ]];
then
	condor_rm -all
	ps aux | grep jzhou24 | grep python | tr -s ' ' |  cut -d ' ' -f 2 | xargs kill -9
fi

if [[ $# == 2 ]];
then
	ss -anp | grep -oP "$1.*" | grep -oP '(?<=pid=).*' | cut -d ',' -f 1 | xargs kill -9

fi


