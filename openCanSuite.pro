TEMPLATE = subdirs
SUBDIRS = qcan canHmi canPlotter canAnalyzer widgets
canPlotter.depends = qcan widgets
canAnalyzer.depends = qcan widgets
canHmi.depends = qcan
widgets.depends = qcan

