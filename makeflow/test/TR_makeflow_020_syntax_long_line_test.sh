#!/bin/sh

. ../../dttools/test/test_runner_common.sh

prepare()
{
	syntax/long_line_test.pl > syntax/long_line_test.makeflow
	exit 0
}

run()
{
	cd syntax; ../../src/makeflow_analyze -k long_line_test.makeflow && exit 0
	exit 1
}

clean()
{
	rm -f syntax/long_line_test.makeflow
	exit 0
}

dispatch "$@"

# vim: set noexpandtab tabstop=4:
