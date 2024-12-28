TEMPLATE = subdirs

SUBDIRS = \
    project \
    firsttest \

# where to find the sub projects - give the folders
project.subdir = project
firsttest.subdir = tests/firsttest

# what subproject depends on others
firsttest.depends = project

