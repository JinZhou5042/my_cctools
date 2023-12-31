include(manual.h)dnl
HEADER(makeflow_graph_log)

SECTION(NAME)
BOLD(makeflow_graph_log) - convert a makeflow log into a graph

SECTION(SYNOPSIS)
CODE(makeflow_graph_log PARAM(logfile) PARAM(imagefile))

SECTION(DESCRIPTION)

BOLD(makeflow_graph_log) converts a makeflow log into a graph
that shows the number of tasks ready, running, and completed over time.
The output graph is generated by using gnuplot to create a postscript
file, which is then converted to the desired type using ImageMagick.
The type of the output file is determined by the name, so any image
type supported by ImageMagick can be selected.

SECTION(OPTIONS)
None.

SECTION(EXAMPLES)

LONGCODE_BEGIN
makeflow_graph_log small.makeflowlog small.pdf
makeflow_graph_log big.makeflowlog big.gif
LONGCODE_END

SECTION(COPYRIGHT)

COPYRIGHT_BOILERPLATE

SECTION(SEE ALSO)

SEE_ALSO_MAKEFLOW

FOOTER
