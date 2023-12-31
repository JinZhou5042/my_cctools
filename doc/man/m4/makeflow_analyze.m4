include(manual.h)dnl
HEADER(makeflow_analyze)

SECTION(NAME)
BOLD(makeflow_analyze) - analysis of Makeflow workflows

SECTION(SYNOPSIS)
CODE(makeflow_analyze [options] PARAM(dagfile))

SECTION(DESCRIPTION)

BOLD(makeflow_analyze) is a collection of tools to provide insight into the structure of workflows. This includes syntax analysis and dag statistics.

SECTION(OPTIONS)
SUBSECTION(Commands)
OPTIONS_BEGIN
OPTION_ARG(b, bundle-dir, directory)Create portable bundle of workflow.
OPTION_FLAG(h,help)Show this help screen.
OPTION_FLAG(I,show-input)Show input files.
OPTION_FLAG(k,syntax-check)Syntax check.
OPTION_FLAG(O,show-output)Show output files.
OPTION_FLAG(v,version)Show version string.
OPTIONS_END

SECTION(EXAMPLES)

Analyze the syntax of a workflow:
LONGCODE_BEGIN
makeflow_analyze -k Makeflow
LONGCODE_END

SECTION(COPYRIGHT)

COPYRIGHT_BOILERPLATE

SECTION(SEE ALSO)

SEE_ALSO_MAKEFLOW

FOOTER
