TEMPLATE = subdirs
SUBDIRS = qcan canPlotter canAnalyzer widgets
canPlotter.depends = qcan widgets
canAnalyzer.depends = qcan widgets
widgets.depends = qcan

