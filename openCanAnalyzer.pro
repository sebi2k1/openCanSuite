TEMPLATE = subdirs
SUBDIRS = qcan canPlotter widgets
canPlotter.depends = qcan widgets
widgets.depends = qcan

