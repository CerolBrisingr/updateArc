TEMPLATE = subdirs

SUBDIRS = \
    UpdateArc \
    firsttest \
    test_downloader

# where to find the sub projects - give the folders
UpdateArc.subdir = UpdateArc
firsttest.subdir = tests/firsttest
test_downloader.subdir = tests/test_downloader

# what subproject depends on others
firsttest.depends = UpdateArc
test_downloader.depends = UpdateArc

